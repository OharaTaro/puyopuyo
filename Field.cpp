#include "Field.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	// �Ղ�̈ʒu���i�񎟌��z��̃C���f�b�N�X�j
	struct PuyoIndex
	{
		int x;
		int y;
	};

	// �Ղ�̐F
	enum
	{
		Color_None,	// ������Ă��Ȃ�
		Color_Red,
		Color_Green,
		Color_Blue,
		Color_Yellow,

		Color_Num
	};
	constexpr unsigned int kColor[Color_Num] =
	{
		0x000000,
		0xff0000,
		0x00ff00,
		0x0000ff,
		0xffff00,
	};
}

Field::Field():
	m_fieldX(200),
	m_fieldY(64),
	m_frameCount(0)
{
	for (int x = 0; x < kFieldX; x++)
	{
		for (int y = 0; y < kFieldY; y++)
		{
			// �Ֆʂɉ���������Ă��Ȃ���Ԃɂ���
			m_field[y][x] = 0;
			m_eraseField[y][x] = 0;
			m_fallRate[y][x] = 0.0f;
		}
	}
}

Field::~Field()
{
}

void Field::init()
{
	createRandom();
}

void Field::update()
{
	(this->*m_pUpdateFunc)();
}

void Field::draw()
{
	for (int x = 0; x < kFieldX; x++)
	{
		for (int y = 0; y < kFieldY; y++)
		{
			int posX = x * kPuyoSize + m_fieldX;
			int posY = y * kPuyoSize + m_fieldY;

			DrawBox(posX, posY,
				posX + kPuyoSize, posY + kPuyoSize,
				0xffffff, false
			);

			if (m_field[y][x] != Color_None)
			{
				// �_�ŏ���
				bool isBlink = false;	// true�̎��͓_�ŏ����Ŕ�\��
				int shakeX = 0;
				int shakeY = 0;

				if (m_eraseField[y][x])
				{
					// ������Ղ��h�炷
					shakeX = GetRand(8) - 4;
					shakeY = GetRand(8) - 4;
					// �_�ŏ���
					if ((m_frameCount / 4) % 2)
					{
					//	isBlink = true;
					}
				}

				if(!isBlink)
				{
					unsigned int colorType = m_field[y][x];
					int puyoPosX = posX + kPuyoSize / 2 + shakeX;
					int puyoPosY = posY + kPuyoSize / 2 + shakeY;

					puyoPosY += static_cast<int>(m_fallRate[y][x] * kPuyoSize);

					DrawCircle(puyoPosX,
						puyoPosY,
						kPuyoSize / 2,
						kColor[colorType],
						true
					);
				}

			}
		}
	}
}

void Field::updateNormal()
{
	m_frameCount++;
	if (m_frameCount >= 60 * 2)
	{
		if (isErasePuyo())
		{
			printfDx("updateNormal -> updateErase\n");

			m_pUpdateFunc = &Field::updateErase;
			m_frameCount = 0;
		}
	}
}

void Field::updateErase()
{
	m_frameCount++;
	if (m_frameCount >= 60 * 1)
	{
		// ���ۂɂՂ������
		for (int x = 0; x < kFieldX; x++)
		{
			for (int y = 0; y < kFieldY; y++)
			{
				if (m_eraseField[y][x])
				{
					m_field[y][x] = Color_None;
				}
			}
		}

		// m_eraseField���N���A����
		for (int x = 0; x < kFieldX; x++)
		{
			for (int y = 0; y < kFieldY; y++)
			{
				m_eraseField[y][x] = 0;
			}
		}

		printfDx("updateErase -> updateFall\n");
		// updateFall�i���Ƃ������j�Ɉڍs����
		m_pUpdateFunc = &Field::updateFall;
		m_frameCount = 0;
		// �����x�������������Ă���
		for (int x = 0; x < kFieldX; x++)
		{
			for (int y = 0; y < kFieldY; y++)
			{
				m_fallRate[y][x] = 0.0f;
			}
		}
	}
}

void Field::updateFall()
{
	m_frameCount++;

//	if (m_frameCount < 16)	return;
//	m_frameCount = 0;

	// �Ղ�̗����x�����v�Z����
	float rate = static_cast<float>(m_frameCount) / 16.0f;

	// �������Ղ悪���邩�ǂ���
	bool isFall = false;

	for (int y = (kFieldY - 1) - 1; y >= 0; y--)	// ��ԉ��̈��̍s����`�F�b�N����
	{
		for (int x = 0; x < kFieldX; x++)
		{
			// ������Ղ悪���������Ȃ�
			if (m_field[y][x] == Color_None)	continue;

			int checkX = x;
			int checkY = y+1;
			assert(checkX >= 0);
			assert(checkX < kFieldX);
			assert(checkY >= 0);
			assert(checkY < kFieldY);

			if( (m_field[checkY][checkX] == Color_None) ||
				(m_fallRate[checkY][checkX] > 0.0f) )
			{
				if (rate < 1.0f)
				{
					m_fallRate[y][x] = rate;
#if false
					// ������Ղ������ɂ���Ղ�͑S�����Ƃ�
					for (int tempY = y - 1; y >= 0; y--)
					{
						assert(tempY >= 0);
						if (m_field[tempY][x] != Color_None)
						{
							m_fallRate[tempY][x] = rate;
						}
					}
#endif
				}
				else
				{
					m_field[checkY][checkX] = m_field[y][x];
					m_field[y][x] = Color_None;
					m_fallRate[y][x] = 0.0f;
				}
				isFall = true;
			}
		}
	}

	if (m_frameCount >= 16)
	{
		m_frameCount = 0;
	}


	if (!isFall)
	{
		printfDx("updateFall -> updateNormal\n");

		m_pUpdateFunc = &Field::updateNormal;
		m_frameCount = 0;

		// �����x�������������Ă���
		for (int x = 0; x < kFieldX; x++)
		{
			for (int y = 0; y < kFieldY; y++)
			{
				m_fallRate[y][x] = 0.0f;
			}
		}
	}
}

void Field::createRandom()
{
	for (int x = 0; x < kFieldX; x++)
	{
		for (int y = 0; y < kFieldY; y++)
		{
			// �����_���ɂȂɂ��u��
			m_field[y][x] = GetRand(3)+1;
		}
	}
}

bool Field::isSameColor(int colorType, int x, int y)
{
	// �͈͊O�ɂՂ�͒u����Ȃ�
	if (x < 0)	return false;
	if (x > kFieldX - 1)	return false;
	if (y < 0)	return false;
	if (y > kFieldY - 1)	return false;

	// �w�肵���ʒu�Ɏw�肵���F���u����Ă��邩���`�F�b�N����
//	if (m_field[y][x] == colorType)	return true;
//	return false;

	return (m_field[y][x] == colorType);
}

void Field::recursionCheck(int tempField[kFieldY][kFieldX], int colorType, int x, int y)
{
	// �Ⴄ�F�Ȃ̂ŏI��
	if (!isSameColor(colorType, x, y))
	{
		return;
	}
	// �����F�̏ꍇ
	tempField[y][x] = 1;	// �����F���Ȃ����Ă���
	// ����ɂ�������㉺���E���`�F�b�N����
	constexpr PuyoIndex offset[4] =
	{
		{ 1, 0},	// �E
		{-1, 0},	// ��
		{ 0, 1},	// ��
		{ 0,-1},	// ��
	};

	for (auto& ofs : offset)
	{
		int indexX = x + ofs.x;
		int indexY = y + ofs.y;
		// �͈͊O�̓`�F�b�N���Ȃ�
		if (indexX < 0)	continue;
		if (indexX > kFieldX - 1)	continue;
		if (indexY < 0)	continue;
		if (indexY > kFieldY - 1)	continue;

		if (tempField[indexY][indexX] == 0)	// ���łɂȂ����Ă��锻�肳��Ă��镔���̓`�F�b�N���Ȃ�
		{
			recursionCheck(tempField, colorType, indexX, indexY);
		}
	}
}

bool Field::isErasePuyo()
{
	bool isErase = false;
	int tempField[kFieldY][kFieldX];

	// �����Ղ��������
	for (int x = 0; x < kFieldX; x++)
	{
		for (int y = 0; y < kFieldY; y++)
		{
			m_eraseField[y][x] = 0;
		}
	}

	// �Ȃ����Ă��������
	for (int x = 0; x < kFieldX; x++)
	{
		for (int y = 0; y < kFieldY; y++)
		{
			// �h��Ԃ��`�F�b�N�p�̓񎟌��z�񏉊���
			clearTempField(tempField);
			// �`�F�b�N�J�n�ʒu����h��Ԃ�������s��
			recursionCheck(tempField, m_field[y][x], x, y);
			// �h��Ԃ�������`�F�b�N����
			int count = countTempField(tempField);

		//	printfDx("(%d, %d) count = %d\n", x, y, count);
			if (count >= kPuyoEraseNum)
			{
				// �Ղ������
				isErase = true;
				erasePuyoData(tempField);
			}

		}
	}

	return isErase;
}

void Field::clearTempField(int tempField[kFieldY][kFieldX])
{
	for (int x = 0; x < kFieldX; x++)
	{
		for (int y = 0; y < kFieldY; y++)
		{
			tempField[y][x] = 0;
		}
	}
}

int Field::countTempField(int tempField[kFieldY][kFieldX]) const
{
	int count = 0;
	for (int x = 0; x < kFieldX; x++)
	{
		for (int y = 0; y < kFieldY; y++)
		{
			if (tempField[y][x])
			{
				count++;
			}
		}
	}
	return count;
}

void Field::erasePuyoData(int tempField[kFieldY][kFieldX])
{
	for (int x = 0; x < kFieldX; x++)
	{
		for (int y = 0; y < kFieldY; y++)
		{
			if (tempField[y][x])
			{
			//	m_field[y][x] = Color_None;
				m_eraseField[y][x] = 1;
			}
		}
	}
}
