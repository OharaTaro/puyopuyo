#include "DxLib.h"

#include "GameMain.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//------------------------------------------------------
	// ウインドウ設定とDXライブラリの初期化
	//------------------------------------------------------
	// Windowsモードにする
	ChangeWindowMode(TRUE);
	// 画面サイズの設定
	SetGraphMode(800, 600, 32);

	// DXライブラリ初期化
	if (DxLib_Init() == -1)
	{
		//初期化に失敗
		return -1;
	}
	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);

	GameMain* pGameMain = new GameMain;
	pGameMain->init();

	//------------------------------------------------------
	// ゲーム部分
	//------------------------------------------------------
	// メインループ処理
	while (ProcessMessage() == 0)
	{
		//裏画面消す
		ClearDrawScreen();

		pGameMain->update();

		pGameMain->draw();

		//裏画面を表画面を入れ替える
		ScreenFlip();
	}

	delete pGameMain;

	//------------------------------------------------------
	// DXライブラリ終了とプログラム終了
	//------------------------------------------------------
	DxLib_End();
	return 0;
}