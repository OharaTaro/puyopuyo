#pragma once
class Field
{
public:
	// 盤面のサイズ
	static constexpr int kFieldX = 6;
	static constexpr int kFieldY = 12;

	// マスのサイズ
	static constexpr int kPuyoSize = 32;

	// つなげると消える数
	static constexpr int kPuyoEraseNum = 4;

public:
	Field();
	virtual ~Field();

	void init();
	void update();
	void draw();

private:
	// ゲーム処理
	void updateNormal();
	// ぷよ消える演出
	void updateErase();
	// 下にぷよが落ちていく
	void updateFall();

	// ランダムに配置された盤面を生成する
	void createRandom();

	// 同じ色のぷよがあるかをチェックする
	bool isSameColor(int colorType, int x, int y);

	// 再帰を利用して始点から同じ色でつながっているぷよのある場所のtempFieldに１を立てる
	void recursionCheck(int tempField[kFieldY][kFieldX], int colorType, int x, int y);

	// 消えるぷよがあるかをチェックする
	bool isErasePuyo();

	// tempField操作
	void clearTempField(int tempField[kFieldY][kFieldX]);	// 0クリア
	int countTempField(int tempField[kFieldY][kFieldX]) const;	// 1になっているマスを数える
	void erasePuyoData(int tempField[kFieldY][kFieldX]);	// 1になっているマスのぷよを消す

private:
	// メンバ関数ポインタ
	void (Field::*m_pUpdateFunc)() = &Field::updateNormal;

	// 現在の盤面のデータ
	int m_field[kFieldY][kFieldX];

	// 消すぷよを覚えておく	0:消さない	1:消す
	int m_eraseField[kFieldY][kFieldX];

	// 落ちるぷよの落ち度合	0.0:その位置にいる ~ 1.0:一つ下の位置に表示される
	float m_fallRate[kFieldY][kFieldX];

	// 盤面の表示左上座標
	int m_fieldX;
	int m_fieldY;

	// フレームカウント
	int m_frameCount;
};

