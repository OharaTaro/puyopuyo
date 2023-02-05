#include "Field.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	// ぷよの位置情報（二次元配列のインデックス）
	struct PuyoIndex
	{
		int x;
		int y;
	};

	// ぷよの色
	enum
	{
		Color_None,	// おかれていない
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
			// 盤面に何もおかれていない状態にする
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
				// 点滅処理
				bool isBlink = false;	// trueの時は点滅処理で非表示
				int shakeX = 0;
				int shakeY = 0;

				if (m_eraseField[y][x])
				{
					// 消えるぷよを揺らす
					shakeX = GetRand(8) - 4;
					shakeY = GetRand(8) - 4;
					// 点滅処理
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
		// 実際にぷよを消す
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

		// m_eraseFieldをクリアする
		for (int x = 0; x < kFieldX; x++)
		{
			for (int y = 0; y < kFieldY; y++)
			{
				m_eraseField[y][x] = 0;
			}
		}

		printfDx("updateErase -> updateFall\n");
		// updateFall（落とす処理）に移行する
		m_pUpdateFunc = &Field::updateFall;
		m_frameCount = 0;
		// 落ち度合を初期化しておく
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

	// ぷよの落ち度合を計算する
	float rate = static_cast<float>(m_frameCount) / 16.0f;

	// 落ちたぷよがあるかどうか
	bool isFall = false;

	for (int y = (kFieldY - 1) - 1; y >= 0; y--)	// 一番下の一つ上の行からチェックする
	{
		for (int x = 0; x < kFieldX; x++)
		{
			// 落ちるぷよがそもそもない
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
					// 落ちるぷよよりも上にあるぷよは全部落とす
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

		// 落ち度合を初期化しておく
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
			// ランダムになにか置く
			m_field[y][x] = GetRand(3)+1;
		}
	}
}

bool Field::isSameColor(int colorType, int x, int y)
{
	// 範囲外にぷよは置かれない
	if (x < 0)	return false;
	if (x > kFieldX - 1)	return false;
	if (y < 0)	return false;
	if (y > kFieldY - 1)	return false;

	// 指定した位置に指定した色が置かれているかをチェックする
//	if (m_field[y][x] == colorType)	return true;
//	return false;

	return (m_field[y][x] == colorType);
}

void Field::recursionCheck(int tempField[kFieldY][kFieldX], int colorType, int x, int y)
{
	// 違う色なので終了
	if (!isSameColor(colorType, x, y))
	{
		return;
	}
	// 同じ色の場合
	tempField[y][x] = 1;	// 同じ色がつながっている
	// さらにそこから上下左右をチェックする
	constexpr PuyoIndex offset[4] =
	{
		{ 1, 0},	// 右
		{-1, 0},	// 左
		{ 0, 1},	// 下
		{ 0,-1},	// 上
	};

	for (auto& ofs : offset)
	{
		int indexX = x + ofs.x;
		int indexY = y + ofs.y;
		// 範囲外はチェックしない
		if (indexX < 0)	continue;
		if (indexX > kFieldX - 1)	continue;
		if (indexY < 0)	continue;
		if (indexY > kFieldY - 1)	continue;

		if (tempField[indexY][indexX] == 0)	// すでにつながっている判定されている部分はチェックしない
		{
			recursionCheck(tempField, colorType, indexX, indexY);
		}
	}
}

bool Field::isErasePuyo()
{
	bool isErase = false;
	int tempField[kFieldY][kFieldX];

	// 消すぷよを初期化
	for (int x = 0; x < kFieldX; x++)
	{
		for (int y = 0; y < kFieldY; y++)
		{
			m_eraseField[y][x] = 0;
		}
	}

	// つながっていたら消す
	for (int x = 0; x < kFieldX; x++)
	{
		for (int y = 0; y < kFieldY; y++)
		{
			// 塗りつぶしチェック用の二次元配列初期化
			clearTempField(tempField);
			// チェック開始位置から塗りつぶし判定を行う
			recursionCheck(tempField, m_field[y][x], x, y);
			// 塗りつぶせる個数をチェックする
			int count = countTempField(tempField);

		//	printfDx("(%d, %d) count = %d\n", x, y, count);
			if (count >= kPuyoEraseNum)
			{
				// ぷよを消す
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
