#include "DxLib.h"

#include "GameMain.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//------------------------------------------------------
	// �E�C���h�E�ݒ��DX���C�u�����̏�����
	//------------------------------------------------------
	// Windows���[�h�ɂ���
	ChangeWindowMode(TRUE);
	// ��ʃT�C�Y�̐ݒ�
	SetGraphMode(800, 600, 32);

	// DX���C�u����������
	if (DxLib_Init() == -1)
	{
		//�������Ɏ��s
		return -1;
	}
	// �_�u���o�b�t�@���[�h
	SetDrawScreen(DX_SCREEN_BACK);

	GameMain* pGameMain = new GameMain;
	pGameMain->init();

	//------------------------------------------------------
	// �Q�[������
	//------------------------------------------------------
	// ���C�����[�v����
	while (ProcessMessage() == 0)
	{
		//����ʏ���
		ClearDrawScreen();

		pGameMain->update();

		pGameMain->draw();

		//����ʂ�\��ʂ����ւ���
		ScreenFlip();
	}

	delete pGameMain;

	//------------------------------------------------------
	// DX���C�u�����I���ƃv���O�����I��
	//------------------------------------------------------
	DxLib_End();
	return 0;
}