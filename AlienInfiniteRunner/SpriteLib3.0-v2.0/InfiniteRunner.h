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

};
