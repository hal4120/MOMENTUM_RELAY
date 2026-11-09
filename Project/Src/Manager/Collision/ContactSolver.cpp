#include "ContactSolver.h"
#include "../../Common/Quaternion.h"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <chrono>
#include "../../Object/Common/Collider/ColliderBase.h"
#include "../../Object/Common/ActorBase/ActorBase.h"
#include "../../Object/Common/PhysicsBody/PhysicsBody.h"

namespace {
    struct BodyState {
        PhysicsBody* body = nullptr;
        Vector3 center = Vector3(0.0f);
        float invMass = 0.0f;
        Quaternion orientation = Quaternion::Identity();
        bool character = false;
        bool activeVelocity = false;

        explicit BodyState(ColliderBase* collider) {
            if (!collider) return;
            center = collider->GetPos();
            ActorBase* actor = collider->GetActor();
            if (!actor) return;
            body = actor->GetPhysicsBody();
            orientation = actor->GetTrans().rotation;
            if (!body || collider->GetMobility() != COLLIDER_MOBILITY::Movable || body->IsFrozen()) return;
            character = body->GetMode() == PhysicsBody::MotionMode::Character;
            activeVelocity = true;
            // CharacterもDynamicと同じ逆質量で接触インパルスを解く。
            // 直立維持はPhysicsBodyのrotationEnabledで制御する。
            // スリープはDynamicにのみ適用される。
            if (body->GetMode() == PhysicsBody::MotionMode::Kinematic) return;
            if (body->GetMode() == PhysicsBody::MotionMode::Dynamic && body->IsSleeping()) return;
            invMass = body->GetInverseMass();

        }
        Vector3 VelocityAt(const Vector3& lever) const {
            return body && activeVelocity ? body->GetPointVelocity(lever) : Vector3(0.0f);
        }
        Vector3 InverseInertia(const Vector3& v) const {
            return invMass > 0.0f && body ? body->MultiplyInverseInertia(v, orientation) : Vector3(0.0f);
        }
        void Apply(const Vector3& impulse, const Vector3& lever) const {
            if (invMass > 0.0f && body) body->ApplyImpulseAtPoint(impulse, lever, orientation);
        }
    };
    struct Impulses {
        float normal = 0.0f;
        Vector3 tangentImpulse = Vector3(0.0f);
    };
    // 接触点は物体中心からの相対座標で照合する。
    // 生ポインタはキャッシュの識別子としてのみ使用し、参照しない。
    struct PairKey {
        const ColliderBase* a;
        const ColliderBase* b;
        bool operator==(const PairKey& other) const { return a == other.a && b == other.b; }
    };
    struct PairHash {
        size_t operator()(const PairKey& key) const {
            const size_t x = reinterpret_cast<size_t>(key.a);
            const size_t y = reinterpret_cast<size_t>(key.b);
            return (x >> 4) ^ ((y >> 4) * size_t(0x9e3779b9));
        }
    };
    // ワールド座標のレバーアームを剛体ローカル座標に変換する。
    // 前フレームから物体が回転しても、同じ面・頂点の接触を追跡できる。
    Vector3 ToLocalAnchor(const ColliderBase* collider, const Vector3& worldLever) {
        if (!collider || !collider->GetActor()) return worldLever;
        return collider->GetActor()->GetTrans().rotation.Inversed().Rotate(worldLever);
    }
    struct CachedPoint {
        Vector3 offsetA = Vector3(0.0f);
        Vector3 offsetB = Vector3(0.0f);
        Impulses impulses;
        float bounceTarget = 0.0f; // このフレームの反発目標速度
    };
    struct CachedPair {
        Vector3 normal = Vector3(0.0f);
        std::array<CachedPoint, 4> points{};
        int count = 0;
        unsigned long long frame = 0;
    };
    std::unordered_map<PairKey, CachedPair, PairHash> contactCache;
    unsigned long long currentFrame = 0;
    std::vector<ContactManifold> pendingContacts;
    ContactSolver::FrameStats lastFrameStats;
    struct PositionCorrection {
        ColliderBase* a = nullptr;
        ColliderBase* b = nullptr;
        Vector3 displacement = Vector3(0.0f);
        bool oneSide = false;
    };
    std::vector<PositionCorrection> pendingCorrections;
    float CorrectionInverseMass(ColliderBase* collider) {
        if (!collider || collider->GetMobility() != COLLIDER_MOBILITY::Movable) return 0.0f;
        ActorBase* actor = collider->GetActor();
        if (!actor) return 0.0f;
        PhysicsBody* body = actor->GetPhysicsBody();
        if (!body || body->IsFrozen()) return 0.0f;
        // 位置補正も速度拘束と同じ逆質量を使用する。
        return body->GetInverseMass();
    }
    // 位置拘束の反復解法。検出時の貫通量を基準とし、同一フレーム中に
    // 他の拘束が既に動かした量を差し引いて、二重補正を防ぐ。
    // 回転に伴う新規接触は次フレームの衝突検出で扱う。
    void SolvePositionConstraints(const std::vector<PositionCorrection>& corrections) {
        constexpr int kIterations = 8;
        constexpr float kSlop = 0.01f;
        constexpr float kRelaxation = 0.8f;
        constexpr float kEpsilon = 1.0e-8f;

        // 複数のColliderが同じActorに属する場合、補正はActor単位で集約する。
        // Collider単位で集計すると同じTransformを複数回移動してしまう。
        std::unordered_map<ActorBase*, Vector3> moved;
        // 位置拘束に登場するActor数の上限を見込んで再ハッシュを減らす。
        // 接触ごとに実際に配分した移動量を記録する。
        // Actorの合計移動量を全接触へ再配分すると、通知量が重複する。
        std::vector<Vector3> movedA(corrections.size(), Vector3(0.0f));
        std::vector<Vector3> movedB(corrections.size(), Vector3(0.0f));

        for (int iteration = 0; iteration < kIterations; ++iteration) {
            for (size_t correctionIndex = 0; correctionIndex < corrections.size(); ++correctionIndex) {
                const PositionCorrection& correction = corrections[correctionIndex];
                ColliderBase* a = correction.a;
                ColliderBase* b = correction.b;
                if (!a || !b) continue;
                ActorBase* actorA = a->GetActor();
                ActorBase* actorB = b->GetActor();
                if (!actorA || !actorB || actorA == actorB) continue;

                const float lengthSq = correction.displacement.LengthSq();
                if (lengthSq <= kEpsilon) continue;
                const float depth = std::sqrt(lengthSq);
                const Vector3 normal = correction.displacement / depth;
                const float invA = CorrectionInverseMass(a);
                const float invB = correction.oneSide ? 0.0f : CorrectionInverseMass(b);
                const float total = invA + invB;
                if (total <= kEpsilon) continue;

                const float alreadySeparated = (moved[actorA] - moved[actorB]).Dot(normal);
                const float remaining = depth - kSlop - alreadySeparated;
                if (remaining <= 0.0f) continue;

                // Character優先の例外は設けず、逆質量比で補正を分担する。
                const float ratioA = invA / total;

                const Vector3 delta = normal * (remaining * kRelaxation);
                if (ratioA > 0.0f) {
                    const Vector3 stepA = delta * ratioA;
                    moved[actorA] += stepA;
                    movedA[correctionIndex] += stepA;
                }
                if (ratioA < 1.0f && !correction.oneSide) {
                    const Vector3 stepB = -delta * (1.0f - ratioA);
                    moved[actorB] += stepB;
                    movedB[correctionIndex] += stepB;
                }
            }
        }

        // Transformへの移動はActorにつき1回。
        // 既存のCollider経由APIを利用するが、同じActorを重複して移動させない。
        std::unordered_map<ActorBase*, ColliderBase*> representatives;
        for (const PositionCorrection& correction : corrections) {
            if (correction.a && correction.a->GetActor())
                representatives.emplace(correction.a->GetActor(), correction.a);
            if (correction.b && correction.b->GetActor())
                representatives.emplace(correction.b->GetActor(), correction.b);
        }
        for (const auto& entry : moved) {
            if (entry.second.LengthSq() <= kEpsilon) continue;
            auto it = representatives.find(entry.first);
            if (it != representatives.end()) {
                it->second->SetTransformPosAdd(entry.second);
                PhysicsBody* body = entry.first->GetPhysicsBody();
                // 静止中の剛体が位置補正で移動した場合は次フレームから再評価。
                if (body && body->IsSleeping() && entry.second.LengthSq() > kSlop * kSlop)
                    body->WakeUp();
            }
        }

        // 接触点ごとの位置拘束から微小な回転補正を計算する。
        // Characterは姿勢を維持し、Dynamicのみ回転させる。
        // 接触点は検出時のワールド座標なので、1フレームでの回転を制限する。
        constexpr float kMaxAngularCorrection = 0.035f; // 約2度/フレーム
        constexpr float kAngularRelaxation = 0.12f;
        std::unordered_map<ActorBase*, Vector3> angularCorrections;
        for (const ContactManifold& contact : pendingContacts) {
            if (!contact.a || !contact.b || contact.count <= 0) continue;
            ActorBase* actorA = contact.a->GetActor();
            ActorBase* actorB = contact.b->GetActor();
            if (!actorA || !actorB || actorA == actorB) continue;
            const Vector3 n = contact.normal;
            if (n.LengthSq() <= kEpsilon) continue;
            const Vector3 normal = n.Normalized();
            const Vector3 centerA = contact.a->GetPos();
            const Vector3 centerB = contact.b->GetPos();
            PhysicsBody* bodyA = actorA->GetPhysicsBody();
            PhysicsBody* bodyB = actorB->GetPhysicsBody();
            const bool dynamicA = bodyA && bodyA->GetMode() == PhysicsBody::MotionMode::Dynamic
                && !bodyA->IsFrozen() && bodyA->IsRotationEnabled()
                && contact.a->GetMobility() == COLLIDER_MOBILITY::Movable;
            const bool dynamicB = bodyB && bodyB->GetMode() == PhysicsBody::MotionMode::Dynamic
                && !bodyB->IsFrozen() && bodyB->IsRotationEnabled()
                && contact.b->GetMobility() == COLLIDER_MOBILITY::Movable;
            if (!dynamicA && !dynamicB) continue;

            for (int i = 0; i < contact.count; ++i) {
                const float depth = contact.depths[i] - kSlop;
                if (depth <= 0.0f) continue;
                const Vector3 ra = contact.points[i] - centerA;
                const Vector3 rb = contact.points[i] - centerB;
                const Vector3 torqueA = ra.Cross(normal);
                const Vector3 torqueB = rb.Cross(normal);
                const Vector3 invIa = dynamicA
                    ? bodyA->MultiplyInverseInertia(torqueA, actorA->GetTrans().rotation)
                    : Vector3(0.0f);
                const Vector3 invIb = dynamicB
                    ? bodyB->MultiplyInverseInertia(torqueB, actorB->GetTrans().rotation)
                    : Vector3(0.0f);
                const float invMassA = CorrectionInverseMass(contact.a);
                const float invMassB = CorrectionInverseMass(contact.b);
                const float denominator = invMassA + invMassB
                    + torqueA.Dot(invIa) + torqueB.Dot(invIb);
                if (denominator <= kEpsilon) continue;
                const float lambda = depth * kAngularRelaxation / denominator
                    / static_cast<float>(contact.count);
                if (dynamicA) angularCorrections[actorA] += invIa * lambda;
                if (dynamicB) angularCorrections[actorB] -= invIb * lambda;
            }
        }
        for (const auto& entry : angularCorrections) {
            Vector3 delta = entry.second;
            const float angle = delta.Length();
            if (angle <= kEpsilon) continue;
            if (angle > kMaxAngularCorrection)
                delta *= kMaxAngularCorrection / angle;
            entry.first->ApplyPhysicsRotation(delta);
            ++lastFrameStats.angularCorrections;
            PhysicsBody* body = entry.first->GetPhysicsBody();
            if (body && body->IsSleeping()) body->WakeUp();
        }

        // 押し出し通知は実際の接触別移動量で行う。
        // 逆質量に基づく分配結果をそのまま通知する。
        for (size_t i = 0; i < corrections.size(); ++i) {
            const PositionCorrection& correction = corrections[i];
            if (!correction.a || !correction.b) continue;
            if (movedA[i].LengthSq() > kEpsilon)
                correction.a->CallOnPushed(correction.a->GetTag(), *correction.b, movedA[i]);
            if (!correction.oneSide && movedB[i].LengthSq() > kEpsilon)
                correction.b->CallOnPushed(correction.b->GetTag(), *correction.a, movedB[i]);
        }
    }

}

void ContactSolver::BeginFrame()
{
    pendingContacts.clear();
    pendingCorrections.clear();
    ++currentFrame;
    if (currentFrame == 0) { contactCache.clear(); ++currentFrame; }
}

void ContactSolver::EndFrame()
{
    // 接触しなくなった組は直ちに破棄。解放済みColliderのアドレス再利用も防ぐ。
    for (auto it = contactCache.begin(); it != contactCache.end();) {
        if (it->second.frame != currentFrame) it = contactCache.erase(it);
        else ++it;
    }
}

void ContactSolver::Clear()
{
    contactCache.clear();
    pendingContacts.clear();
    pendingCorrections.clear();
    lastFrameStats = ContactSolver::FrameStats{};
    currentFrame = 0;
}

namespace {
}

void ContactSolver::AddPositionCorrection(ColliderBase* a, ColliderBase* b, const Vector3& displacement)
{
    if (a && b && displacement.LengthSq() > 1.0e-8f)
        pendingCorrections.push_back({ a, b, displacement, false });
}

void ContactSolver::AddOneSideCorrection(ColliderBase* movable, ColliderBase* obstacle, const Vector3& displacement)
{
    if (movable && obstacle && displacement.LengthSq() > 1.0e-8f)
        pendingCorrections.push_back({ movable, obstacle, displacement, true });
}

void ContactSolver::AddContact(const ContactManifold& input)
{
    // 検出器からの入力をここで検証・正規化する。
    // 不正な接触点をソルバーへ渡すと摩擦やWarm Startingが不安定になる。
    if (!input.a || !input.b || input.a == input.b || input.count <= 0) return;
    ActorBase* actorA = input.a->GetActor();
    ActorBase* actorB = input.b->GetActor();
    if (!actorA || !actorB || actorA == actorB) return;
    if (input.normal.LengthSq() <= 1.0e-8f) return;

    ContactManifold contact;
    contact.a = input.a;
    contact.b = input.b;
    contact.normal = input.normal.Normalized();
    contact.penetration = (std::max)(0.0f, input.penetration);

    // 同じ位置の接触点を複数回解かない。特に面クリッピングの
    // 重複点は法線・摩擦インパルスの過大適用につながる。
    constexpr float kPointMergeDistanceSq = 1.0e-6f;
    const int inputCount = (std::min)(input.count, static_cast<int>(input.points.size()));
    for (int i = 0; i < inputCount; ++i) {
        bool duplicate = false;
        for (int j = 0; j < contact.count; ++j) {
            if ((contact.points[j] - input.points[i]).LengthSq() <= kPointMergeDistanceSq) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            // 接触点の深さも保持する。従来のAddPoint(point)も使用可能。
            contact.AddPoint(input.points[i], input.depths[i]);
        } else {
            // 重複点は深い方を採用して接触拘束の不足を防ぐ。
            for (int j = 0; j < contact.count; ++j) {
                if ((contact.points[j] - input.points[i]).LengthSq() <= kPointMergeDistanceSq) {
                    contact.depths[j] = (std::max)(contact.depths[j], input.depths[i]);
                    break;
                }
            }
        }
    }
    if (contact.count == 0) return;

    // 位置拘束は同一マニフォールドにつき1つ。
    // 接触点の深さを合計すると面接触で過剰に押し出すため、最大値を採用する。
    float maxDepth = 0.0f;
    for (int i = 0; i < contact.count; ++i)
        maxDepth = (std::max)(maxDepth, contact.depths[i]);
    // 同一Colliderペアの同方向接触を統合する。Meshの複数三角形や
    // 判定経路の重複で同じペアが何度も登録されても、速度拘束と
    // 位置補正を過剰に適用しないようにする。
    constexpr float kSameNormalDot = 0.98f;
    constexpr float kSamePointDistanceSq = 1.0e-6f;
    for (ContactManifold& existing : pendingContacts) {
        if (existing.a != contact.a || existing.b != contact.b ||
            existing.normal.Dot(contact.normal) < kSameNormalDot) continue;

        for (int i = 0; i < contact.count; ++i) {
            int duplicateIndex = -1;
            for (int j = 0; j < existing.count; ++j) {
                if ((existing.points[j] - contact.points[i]).LengthSq() <= kSamePointDistanceSq) {
                    duplicateIndex = j;
                    break;
                }
            }
            if (duplicateIndex >= 0) {
                existing.depths[duplicateIndex] = (std::max)(
                    existing.depths[duplicateIndex], contact.depths[i]);
            } else if (existing.count < static_cast<int>(existing.points.size())) {
                existing.AddPoint(contact.points[i], contact.depths[i]);
            } else {
                // 4点上限の場合、最も浅い接触点を深い点で置き換える。
                // 接触点の増加でインパルスが過剰にならないようにする。
                int shallowest = 0;
                for (int j = 1; j < existing.count; ++j)
                    if (existing.depths[j] < existing.depths[shallowest]) shallowest = j;
                if (contact.depths[i] > existing.depths[shallowest]) {
                    existing.points[shallowest] = contact.points[i];
                    existing.depths[shallowest] = contact.depths[i];
                }
            }
        }
        existing.penetration = (std::max)(existing.penetration, contact.penetration);
        float mergedDepth = 0.0f;
        for (int i = 0; i < existing.count; ++i)
            mergedDepth = (std::max)(mergedDepth, existing.depths[i]);
        // 同一ペアの位置補正も一度だけ登録し、最大深さに更新する。
        for (PositionCorrection& correction : pendingCorrections) {
            if (correction.a == existing.a && correction.b == existing.b && !correction.oneSide &&
                correction.displacement.LengthSq() > 1.0e-8f &&
                correction.displacement.Normalized().Dot(existing.normal) >= kSameNormalDot) {
                correction.displacement = existing.normal * mergedDepth;
                return;
            }
        }
        if (mergedDepth > 0.0f)
            pendingCorrections.push_back({existing.a, existing.b, existing.normal * mergedDepth, false});
        return;
    }

    pendingContacts.push_back(contact);
    if (maxDepth > 0.0f) {
        pendingCorrections.push_back({
            contact.a, contact.b, contact.normal * maxDepth, false
        });
    }
}

namespace {
void SolveOne(const ContactManifold& contact, bool warmStart, bool applyRolling)
{
    if (!contact.a || !contact.b || contact.count <= 0 || contact.normal.LengthSq() < 1.0e-8f) return;
    // 睡眠中の物体が移動する物体と接触したら起こす。
    // 静止床との継続接触では起こさず、不要な微振動を防ぐ。
    PhysicsBody* rawA = contact.a->GetActor() ? contact.a->GetActor()->GetPhysicsBody() : nullptr;
    PhysicsBody* rawB = contact.b->GetActor() ? contact.b->GetActor()->GetPhysicsBody() : nullptr;
    const float wakeSpeedSq = 0.01f;
    if (rawA && rawA->IsSleeping() && rawB &&
        (rawB->GetMode() == PhysicsBody::MotionMode::Character ||
         (!rawB->IsSleeping() && rawB->GetVelocity().LengthSq() > wakeSpeedSq))) rawA->WakeUp();
    if (rawB && rawB->IsSleeping() && rawA &&
        (rawA->GetMode() == PhysicsBody::MotionMode::Character ||
         (!rawA->IsSleeping() && rawA->GetVelocity().LengthSq() > wakeSpeedSq))) rawB->WakeUp();
    BodyState a(contact.a), b(contact.b);
    // 両方が不動の場合のみ拘束計算を省略する。
    // Characterは逆質量を持つので静的床との接触も通常の計算へ進む。
    if (a.invMass + b.invMass <= 1.0e-8f) return;
    const Vector3 n = contact.normal.Normalized();
    const float staticA = a.body ? a.body->GetStaticFriction() : 0.65f;
    const float staticB = b.body ? b.body->GetStaticFriction() : 0.65f;
    const float dynamicA = a.body ? a.body->GetDynamicFriction() : 0.5f;
    const float dynamicB = b.body ? b.body->GetDynamicFriction() : 0.5f;
    const float staticFriction = std::sqrt((std::max)(0.0f, staticA * staticB));
    const float dynamicFriction = std::sqrt((std::max)(0.0f, dynamicA * dynamicB));
    const float restA = a.invMass > 0.0f ? a.body->GetRestitution() : 0.0f;
    const float restB = b.invMass > 0.0f ? b.body->GetRestitution() : 0.0f;
    const float restitution = (std::max)(restA, restB);
    std::array<Impulses, 4> accumulated{};
    const int count = (std::min)(contact.count, 4);
    const PairKey key{ contact.a, contact.b };
    CachedPair& cache = contactCache[key];
    // 反発は新しい衝突でのみ発生させる。
    // 継続接触中に位置補正・重力で生じた微小な侵入速度を
    // 新しい衝突として扱うと、不自然な再反発が起こる。
    if (warmStart) {
        const bool continuingContact =
            cache.frame + 1 == currentFrame && cache.normal.Dot(n) > 0.95f;
        for (int i = 0; i < count; ++i) {
            const Vector3 ra = contact.points[i] - a.center;
            const Vector3 rb = contact.points[i] - b.center;
            const float incoming = (a.VelocityAt(ra) - b.VelocityAt(rb)).Dot(n);
            cache.points[i].bounceTarget =
                !continuingContact && incoming < -1.0f
                ? -restitution * incoming : 0.0f;
        }
    }
    // 同じ向きの接触のみ再利用。回転や移動で接触位置が変わった点は捨てる。
    if (warmStart && cache.frame + 1 == currentFrame && cache.normal.Dot(n) > 0.95f) {
        std::array<bool, 4> matched{};
        for (int i = 0; i < count; ++i) {
            const Vector3 ra = contact.points[i] - a.center;
            const Vector3 rb = contact.points[i] - b.center;
            // 比較する接触点のローカル座標は候補点によらず同じ
            const Vector3 localA = ToLocalAnchor(contact.a, ra);
            const Vector3 localB = ToLocalAnchor(contact.b, rb);
            int best = -1;
            float bestDist = 1.0f; // ローカル相対座標の距離二乗の許容値
            for (int j = 0; j < cache.count; ++j) {
                if (matched[j]) continue;
                const float dist = (localA - cache.points[j].offsetA).LengthSq() +
                    (localB - cache.points[j].offsetB).LengthSq();
                if (dist < bestDist) { bestDist = dist; best = j; }
            }
            if (best >= 0) {
                matched[best] = true;
                accumulated[i] = cache.points[best].impulses;
                // 古い接触インパルスを適用し、次の反復で差分のみを求める。
                a.Apply(n * accumulated[i].normal + accumulated[i].tangentImpulse, ra);
                b.Apply(-n * accumulated[i].normal - accumulated[i].tangentImpulse, rb);
            }
        }
    }
    // 同一フレームの後続反復では、直前の蓄積値を引き継ぐ（再適用しない）。
    if (!warmStart && cache.frame == currentFrame && cache.count == count) {
        for (int i = 0; i < count; ++i) accumulated[i] = cache.points[i].impulses;
    }
    // 接線基底は法線のみで決まるため、接触点ごとの再計算を避ける。
    const Vector3 reference = std::abs(n.x) < 0.577350269f
        ? Vector3(1.0f, 0.0f, 0.0f)
        : Vector3(0.0f, 1.0f, 0.0f);
    const Vector3 t1 = n.Cross(reference).Normalized();
    const Vector3 t2 = n.Cross(t1);

    for (int iteration = 0; iteration < 1; ++iteration) {
        for (int i = 0; i < count; ++i) {
            const Vector3 ra = contact.points[i] - a.center;
            const Vector3 rb = contact.points[i] - b.center;
            const Vector3 relative = a.VelocityAt(ra) - b.VelocityAt(rb);
            const float vn = relative.Dot(n);
            // 同じ接触法線との外積は各物体につき一度だけ計算
            const Vector3 rnA = ra.Cross(n);
            const Vector3 rnB = rb.Cross(n);
            const float denominator = a.invMass + b.invMass +
                rnA.Dot(a.InverseInertia(rnA)) + rnB.Dot(b.InverseInertia(rnB));
            if (denominator <= 1.0e-8f) continue;
            // 後続反復でも反発目標を維持し、跳ね返りを打ち消さない。
            const float bounce = cache.points[i].bounceTarget;
            const float previous = accumulated[i].normal;
            accumulated[i].normal = (std::max)(0.0f, previous + (-vn + bounce) / denominator);
            const float j = accumulated[i].normal - previous;
            a.Apply(n * j, ra);
            b.Apply(-n * j, rb);

            // 接触面の2方向を同時に扱う摩擦拘束。
            // 1方向ずつの近似だと、回転する球や斜め方向の滑りで摩擦量が偏る。
            const Vector3 newRelative = a.VelocityAt(ra) - b.VelocityAt(rb);
            const Vector3 tangential = newRelative - n * newRelative.Dot(n);
            const Vector3 previousTangent = accumulated[i].tangentImpulse;
            const Vector3 rA1 = ra.Cross(t1), rA2 = ra.Cross(t2);
            const Vector3 rB1 = rb.Cross(t1), rB2 = rb.Cross(t2);
            const float massSum = a.invMass + b.invMass;
            // 同じ方向の逆慣性テンソル計算は行列の対角・非対角成分で共有する
            const Vector3 inertiaA2 = a.InverseInertia(rA2);
            const Vector3 inertiaB2 = b.InverseInertia(rB2);
            const float k11 = massSum + rA1.Dot(a.InverseInertia(rA1)) + rB1.Dot(b.InverseInertia(rB1));
            const float k22 = massSum + rA2.Dot(inertiaA2) + rB2.Dot(inertiaB2);
            const float k12 = rA1.Dot(inertiaA2) + rB1.Dot(inertiaB2);
            const float determinant = k11 * k22 - k12 * k12;
            if (determinant <= 1.0e-10f) continue;
            const float vt1 = tangential.Dot(t1);
            const float vt2 = tangential.Dot(t2);
            // 2x2有効質量行列を逆行列で解く。速度がほぼゼロでも
            // 蓄積摩擦を維持・調整できるため、静止接触の不安定さを抑える。
            const float impulse1 = (-k22 * vt1 + k12 * vt2) / determinant;
            const float impulse2 = ( k12 * vt1 - k11 * vt2) / determinant;
            Vector3 candidate = previousTangent + t1 * impulse1 + t2 * impulse2;
            candidate -= n * candidate.Dot(n);
            const float staticLimit = staticFriction * accumulated[i].normal;
            const float candidateLength = candidate.Length();
            if (candidateLength > staticLimit) {
                const float dynamicLimit = dynamicFriction * accumulated[i].normal;
                candidate = candidateLength > 1.0e-8f
                    ? candidate * ((std::min)(1.0f, dynamicLimit / candidateLength))
                    : Vector3(0.0f);
            }
            accumulated[i].tangentImpulse = candidate;
            const Vector3 deltaTangent = candidate - previousTangent;
            a.Apply(deltaTangent, ra);
            b.Apply(-deltaTangent, rb);
        }
    }
    cache.normal = n;
    cache.count = count;
    cache.frame = currentFrame;
    for (int i = 0; i < count; ++i) {
        // 次の反復に必要な蓄積インパルスは毎回更新する。
        cache.points[i].impulses = accumulated[i];
    }
    // ローカル接触位置は同一フレーム内の後続反復では参照しない。
    // 最終反復でのみ保存し、次フレームのウォームスタート照合に使う。
    if (applyRolling) {
        for (int i = 0; i < count; ++i) {
            cache.points[i].offsetA = ToLocalAnchor(contact.a, contact.points[i] - a.center);
            cache.points[i].offsetB = ToLocalAnchor(contact.b, contact.points[i] - b.center);
        }
    }
    if (applyRolling) {
        if (a.invMass > 0.0f && a.body) a.body->ApplyContactRollingResistance(n);
        if (b.invMass > 0.0f && b.body) b.body->ApplyContactRollingResistance(-n);
    }
}
}

ContactSolver::FrameStats ContactSolver::GetLastFrameStats()
{
    return lastFrameStats;
}

void ContactSolver::SolveAll()
{
    using Clock = std::chrono::steady_clock;
    const auto toMs = [](Clock::duration duration) {
        return std::chrono::duration<double, std::milli>(duration).count();
    };
    lastFrameStats = FrameStats{};
    lastFrameStats.manifolds = static_cast<int>(pendingContacts.size());
    lastFrameStats.positionConstraints = static_cast<int>(pendingCorrections.size());
    for (const ContactManifold& contact : pendingContacts)
        lastFrameStats.contactPoints += contact.count;

    const auto velocityStart = Clock::now();
    // 全接触ペアを横断して反復する。積み重ねた箱にも衝撃が伝わる。
    for (int iteration = 0; iteration < 10; ++iteration) {
        for (const ContactManifold& contact : pendingContacts) {
            SolveOne(contact, iteration == 0, iteration == 9);
        }
    }
    lastFrameStats.velocitySolveMs = toMs(Clock::now() - velocityStart);
    const auto sleepStart = Clock::now();
    // 接触が全て解決された後でスリープ判定。非接地物体は必ず起床させる。
    struct SleepSupport { bool supported = false; bool moving = false; };
    std::unordered_map<PhysicsBody*, SleepSupport> support;
    // 同一フレーム内での再ハッシュを抑える。
    for (const ContactManifold& contact : pendingContacts) {
        if (!contact.a || !contact.b) continue;
        PhysicsBody* bodyA = contact.a->GetActor() ? contact.a->GetActor()->GetPhysicsBody() : nullptr;
        PhysicsBody* bodyB = contact.b->GetActor() ? contact.b->GetActor()->GetPhysicsBody() : nullptr;
        if (bodyA && bodyA->GetMode() == PhysicsBody::MotionMode::Dynamic) {
            auto& entry = support[bodyA];
            if (contact.normal.y > 0.7f) {
                entry.supported = true;
                if (bodyB && (bodyB->GetMode() == PhysicsBody::MotionMode::Character ||
                    (!bodyB->IsSleeping() && bodyB->GetVelocity().LengthSq() > 0.01f))) entry.moving = true;
            }
        }
        if (bodyB && bodyB->GetMode() == PhysicsBody::MotionMode::Dynamic) {
            auto& entry = support[bodyB];
            if (contact.normal.y < -0.7f) {
                entry.supported = true;
                if (bodyA && (bodyA->GetMode() == PhysicsBody::MotionMode::Character ||
                    (!bodyA->IsSleeping() && bodyA->GetVelocity().LengthSq() > 0.01f))) entry.moving = true;
            }
        }
    }
    for (const auto& item : support) item.first->UpdateSleepState(item.second.supported, item.second.moving);
    lastFrameStats.sleepUpdateMs = toMs(Clock::now() - sleepStart);
    const auto positionStart = Clock::now();
    // Solve velocity constraints before changing positions. Contacts were generated
    // from a consistent snapshot of transforms earlier in the frame.
    SolvePositionConstraints(pendingCorrections);
    lastFrameStats.positionSolveMs = toMs(Clock::now() - positionStart);
    // 回転位置補正もpendingContactsを使用するため、位置補正後に破棄する。
    pendingContacts.clear();
    pendingCorrections.clear();
}

