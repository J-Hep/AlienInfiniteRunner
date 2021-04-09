#pragma once

#include "Scene.h"

class InfiniteRunner : public Scene
{

public:
	InfiniteRunner(std::string name);
	//InfiniteRunner(float windowWidth, float windowHeight) override;

	void InitScene(float windowWidth, float windowHeight) override;

	void Update() override;

	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;

protected:
	int alienBoss;
	int loseStateCollision;
	int gameoverScreen;
	int gapFillerOne;
	int boundFillerTop;
	int boundFillerBot;
	int screenEffect;
	int mainMenu;
	int gapFillerTwo;
	int canisters[64];
	int roofTiles[64];
	int floorTiles[64];
	int backgroundTile[1];
	

};
