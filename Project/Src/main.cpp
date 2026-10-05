#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "pch.h"

#include"Application/Application.h"

// NVIDIA Optimus / AMD Dual Graphics 対応用
extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;  // NVIDIA用
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; // AMD用
}

//----------------
// WinMain関数
//----------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//アプリケーションクラスの実体化したインスタンスの作成
	Application::CreateIns();
	if (Application::GetIns().IsInitFail()) { return -1; }

	//ゲーム起動
	Application::GetIns().Run();

	//解放処理
	Application::GetIns().Release();
	if (Application::GetIns().IsReleaseFail()) { return -1; }
	Application::DeleteIns();

	return 0;		//ゲーム終了
}
