#include "CityCollisionActor.h"
#include "../Common/Collider/BoxCollider.h"
#include "../Common/Collider/MeshCollider.h"

namespace {
// The imported triangles are already in game world coordinates. Do not
// rebuild them from an MV1 model during ActorBase::Init().
class ImportedMeshCollider final : public MeshCollider {
public:
    ImportedMeshCollider() : MeshCollider(COLLIDER_TAG::Stage) {}
    void Init(void) override {}
};
}

CityCollisionActor::CityCollisionActor(const CityMap& map) {
    SetColliderMobility(COLLIDER_MOBILITY::Static);
    for (const auto& box : map.GetBoxCollisions()) {
        AddCollider(new BoxCollider(COLLIDER_TAG::Stage, box.size, box.center, box.rotation));
    }
    for (const auto& mesh : map.GetMeshCollisions()) {
        ImportedMeshCollider* collider = new ImportedMeshCollider();
        if (!collider->Setup(mesh.vertices, mesh.indices)) {
            delete collider;
            continue;
        }
        AddCollider(collider);
    }
}
