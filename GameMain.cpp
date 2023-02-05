#include "GameMain.h"
#include "Field.h"

GameMain::GameMain()
{
	m_pField = new Field;
}
GameMain::~GameMain()
{
	delete m_pField;
}

void GameMain::init()
{
	m_pField->init();
}
void GameMain::update()
{
	m_pField->update();
}
void GameMain::draw()
{
	m_pField->draw();
}