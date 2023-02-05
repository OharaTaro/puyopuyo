#pragma once

class Field;
class GameMain
{
public:
	GameMain();
	virtual ~GameMain();

	void init();
	void update();
	void draw();
private:
	Field* m_pField;
};

