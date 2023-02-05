#pragma once
class Field
{
public:
	// �Ֆʂ̃T�C�Y
	static constexpr int kFieldX = 6;
	static constexpr int kFieldY = 12;

	// �}�X�̃T�C�Y
	static constexpr int kPuyoSize = 32;

	// �Ȃ���Ə����鐔
	static constexpr int kPuyoEraseNum = 4;

public:
	Field();
	virtual ~Field();

	void init();
	void update();
	void draw();

private:
	// �Q�[������
	void updateNormal();
	// �Ղ�����鉉�o
	void updateErase();
	// ���ɂՂ悪�����Ă���
	void updateFall();

	// �����_���ɔz�u���ꂽ�Ֆʂ𐶐�����
	void createRandom();

	// �����F�̂Ղ悪���邩���`�F�b�N����
	bool isSameColor(int colorType, int x, int y);

	// �ċA�𗘗p���Ďn�_���瓯���F�łȂ����Ă���Ղ�̂���ꏊ��tempField�ɂP�𗧂Ă�
	void recursionCheck(int tempField[kFieldY][kFieldX], int colorType, int x, int y);

	// ������Ղ悪���邩���`�F�b�N����
	bool isErasePuyo();

	// tempField����
	void clearTempField(int tempField[kFieldY][kFieldX]);	// 0�N���A
	int countTempField(int tempField[kFieldY][kFieldX]) const;	// 1�ɂȂ��Ă���}�X�𐔂���
	void erasePuyoData(int tempField[kFieldY][kFieldX]);	// 1�ɂȂ��Ă���}�X�̂Ղ������

private:
	// �����o�֐��|�C���^
	void (Field::*m_pUpdateFunc)() = &Field::updateNormal;

	// ���݂̔Ֆʂ̃f�[�^
	int m_field[kFieldY][kFieldX];

	// �����Ղ���o���Ă���	0:�����Ȃ�	1:����
	int m_eraseField[kFieldY][kFieldX];

	// ������Ղ�̗����x��	0.0:���̈ʒu�ɂ��� ~ 1.0:����̈ʒu�ɕ\�������
	float m_fallRate[kFieldY][kFieldX];

	// �Ֆʂ̕\��������W
	int m_fieldX;
	int m_fieldY;

	// �t���[���J�E���g
	int m_frameCount;
};

