#include "InfiniteRunner.h"
#include "Utilities.h"
#include "timer.h"
#include"game.h"

int tilesFor = 35;
int canisterFor = 4;
bool gameOverState = false;

InfiniteRunner::InfiniteRunner(std::string name) :Scene(name)
{
	//Gravity
	m_gravity = b2Vec2(0.f, -98.f);
	m_physicsWorld->SetGravity(m_gravity);

}

void InfiniteRunner::InitScene(float windowWidth, float windowHeight){

	//Dynamically allocates the register
	m_sceneReg = new entt::registry;

	ECS::AttachRegister(m_sceneReg);

	float aspectRatio = windowWidth / windowHeight;

	//Setup main camera entity
	{
		//Creates camera
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, true);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		ECS::AttachComponent<HorizontalScroll>(entity);
		ECS::AttachComponent<VerticalScroll>(entity);

		float tempValues = 100.f; //DEFAULT IS 75.f

		vec4 temp = vec4(-tempValues, tempValues, -tempValues, tempValues);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
	}

	//Changes scene color
	{
		Scene::SetClearColor(vec4(54,28,98,100));
	}

	//Player
	{
		//Create entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainPlayer(entity, true);
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<CanJump>(entity);

		//Set up components
		std::string fileName = "Astro128.png";

		double astroScale = 0.25; //Default is 0.35?
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 128 * astroScale, 128 * astroScale);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 10.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(0.f), float32(50.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, PLAYER, /*ENEMY |*/GROUND| OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
		//tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetHeight() - shrinkY) / 2.f), vec2(0.f, 0.f), false, PLAYER, ENVIRONMENT | ENEMY | OBJECTS | PICKUP | TRIGGER | HEXAGON, 0.5f, 3.f);
		//std::vector<b2Vec2> points = {b2Vec2(-tempSpr.GetWidth()/2.f, -tempSpr.GetHeight()/2.f), b2Vec2(tempSpr.GetWidth()/2.f, -tempSpr.GetHeight()/2.f), b2Vec2(0.f, tempSpr.GetHeight()/2.f)};
		//tempPhsBody = PhysicsBody(entity, BodyType::TRIANGLE, tempBody, points, vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);

		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetFixedRotation(true);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
		tempPhsBody.SetGravityScale(1.f);


	}

	//Alien Entity 
	{
		//Create entity
		  alienBoss = ECS::CreateEntity();
		//ECS::SetIsMainPlayer(entity, true);
		ECS::AttachComponent<Sprite>(alienBoss);
		ECS::AttachComponent<Transform>(alienBoss);

		ECS::AttachComponent<PhysicsBody>(alienBoss);
		ECS::AttachComponent<CanJump>(alienBoss);

		//Set up components
		std::string fileName = "AlienPose2.png";

		double alienScale = 0.75;
		ECS::GetComponent<Sprite>(alienBoss).LoadSprite(fileName, 128 * alienScale, 128 * alienScale);

		ECS::GetComponent<Transform>(alienBoss).SetPosition(vec3(0.f, 0.f, 25.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(alienBoss);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(alienBoss);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(-100.f), float32(50.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		//Square collison for alien
		tempPhsBody = PhysicsBody(alienBoss, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, ENEMY, GROUND/*PLAYER|ENEMY | OBJECTS | PICKUP | TRIGGER*/, 0.5f, 3.f);


		//tempPhsBody = PhysicsBody(alienBoss, tempBody, float((tempSpr.GetHeight() - shrinkY) / 2.25f), vec2(0.f, 0.f), false, ENEMY, ENVIRONMENT | ENEMY | OBJECTS | PICKUP | TRIGGER | HEXAGON, 0.5f, 3.f);
		//std::vector<b2Vec2> points = {b2Vec2(-tempSpr.GetWidth()/2.f, -tempSpr.GetHeight()/2.f), b2Vec2(tempSpr.GetWidth()/2.f, -tempSpr.GetHeight()/2.f), b2Vec2(0.f, tempSpr.GetHeight()/2.f)};
		//tempPhsBody = PhysicsBody(entity, BodyType::TRIANGLE, tempBody, points, vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);

		tempPhsBody.SetRotationAngleDeg(0.f);
		tempPhsBody.SetFixedRotation(true);
		tempPhsBody.SetColor(vec4(1.f, 0.f, 1.f, 0.3f));
		tempPhsBody.SetGravityScale(1.f);


	}


	//Setup static Top Platform
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Sets up components
		std::string fileName = "GraySquare.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 500, 10);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 2.f));

		ECS::GetComponent<Sprite>(entity).SetTransparency(0);

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0.f), float32(0.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
			float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY | OBJECTS | HEXAGON);
		tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

	}

	//Setup gap filler floor
	{
		//Creates entity
		gapFillerOne = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(gapFillerOne);
		ECS::AttachComponent<Transform>(gapFillerOne);

		//Sets up components
		std::string fileName = "Soil.png";
		ECS::GetComponent<Sprite>(gapFillerOne).LoadSprite(fileName, 500, 128*0.2);
		ECS::GetComponent<Transform>(gapFillerOne).SetPosition(vec3(0.f, -7.f, 6.f));
	}


	//Setup static Roof Platform
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Sets up components
		std::string fileName = "GraySquare.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 500, 10);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 2.f));

		ECS::GetComponent<Sprite>(entity).SetTransparency(0);

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0.f), float32(157.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX),
			float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | ENEMY | OBJECTS | HEXAGON);
		tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

	}

	//Setup gap filler roof
	{
		//Creates entity
		gapFillerTwo = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(gapFillerTwo);
		ECS::AttachComponent<Transform>(gapFillerTwo);

		//Sets up components
		std::string fileName = "Soil.png";
		ECS::GetComponent<Sprite>(gapFillerTwo).LoadSprite(fileName, 500, 128 * 0.2);
		ECS::GetComponent<Transform>(gapFillerTwo).SetPosition(vec3(0.f, 165.f, 6.f));
	}

	//Setup static tunnel Background
	{
		//Creates entity
		backgroundTile[0] = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(backgroundTile[0]);
		ECS::AttachComponent<Transform>(backgroundTile[0]);

		//Sets up components
		std::string fileName = "BackgroundTunnel1.png";
		ECS::GetComponent<Sprite>(backgroundTile[0]).LoadSprite(fileName, 1000, 175);

		ECS::GetComponent<Sprite>(backgroundTile[0]).SetTransparency(1);

		ECS::GetComponent<Transform>(backgroundTile[0]).SetPosition(vec3(0.f, 75.f, 5.f));

	}

	{
		//Creates entity
		backgroundTile[1] = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(backgroundTile[1]);
		ECS::AttachComponent<Transform>(backgroundTile[1]);

		//Sets up components
		std::string fileName = "BackgroundTunnel1.png";
		ECS::GetComponent<Sprite>(backgroundTile[1]).LoadSprite(fileName, 1000, 175);

		ECS::GetComponent<Sprite>(backgroundTile[1]).SetTransparency(1);

		ECS::GetComponent<Transform>(backgroundTile[1]).SetPosition(vec3(1000.f, 75.f, 5.f));
		
	}


	//Setup static Background
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "Space3.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 2150 /2.5, 1060/2.5);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 50.f, 1.f));

	}

	//Setup static Background floor
	/*{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "PlaceHolderMoon.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 1000, 1000);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, -350.f, 2.f));

	}*/

	{
		//Creates background blending outof bounds
		boundFillerBot = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(boundFillerBot);
		ECS::AttachComponent<Transform>(boundFillerBot);

		//Sets up components
		std::string fileName = "CaveShadingBot.png";
		ECS::GetComponent<Sprite>(boundFillerBot).LoadSprite(fileName, 1280, 384);
		ECS::GetComponent<Transform>(boundFillerBot).SetPosition(vec3(0.f, -200.f, 5.f));
	}

	{
		//Creates background blending outof bounds
		boundFillerTop = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(boundFillerTop);
		ECS::AttachComponent<Transform>(boundFillerTop);

		//Sets up components
		std::string fileName = "CaveShadingTop.png";
		ECS::GetComponent<Sprite>(boundFillerTop).LoadSprite(fileName, 1280, 384);
		ECS::GetComponent<Transform>(boundFillerTop).SetPosition(vec3(0.f, 350.f, 5.f));
	}

	//Setup game over screen
	{
		//Creates entity
		 gameoverScreen = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(gameoverScreen);
		ECS::AttachComponent<Transform>(gameoverScreen);

		//Sets up components
		std::string fileName = "GameOverScreen2.png";
		ECS::GetComponent<Sprite>(gameoverScreen).LoadSprite(fileName, (128*10)/3, (128 * 5)/3);
		ECS::GetComponent<Transform>(gameoverScreen).SetPosition(vec3(0.f, 25.f, 100.f));

		ECS::GetComponent<Sprite>(gameoverScreen).SetTransparency(0);

	}

	//TRIGGER TEST
	//Creates entity
	auto entity = ECS::CreateEntity();


	//Add components
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<Trigger*>(entity);

	//Sets up components
	std::string fileName = "BeachBall.png";
	ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 15, 150);

	ECS::GetComponent<Sprite>(entity).SetTransparency(0);

	ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 80.f));
	ECS::GetComponent<Trigger*>(entity) = new Trigger();
	ECS::GetComponent<Trigger*>(entity)->SetTriggerEntity((MainEntities::MainPlayer()));
	ECS::GetComponent<Trigger*>(entity)->AddTargetEntity(MainEntities::MainPlayer());

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	float shrinkX = 0.f;
	float shrinkY = 0.f;
	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(-60.f), float32(80.f));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), true, TRIGGER, PLAYER);
	tempPhsBody.SetColor(vec4(1.f, 0.f, 0.f, 0.3f));

	for (int i = 0; i < tilesFor; i++)
	{
		//setup floor path
		{
			//Creates entity
			 floorTiles[i] = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(floorTiles[i]);
			ECS::AttachComponent<Transform>(floorTiles[i]);

			//Sets up components
			int randomTiling = rand() % 10;

			double tileSizing = 0.2;

			std::string fileName = "Soil.png";

			if (randomTiling > 5) {
			 fileName = "EnrichedSoil.png";
			}

			ECS::GetComponent<Sprite>(floorTiles[i]).LoadSprite(fileName, 128 * tileSizing, 128 * tileSizing);
			ECS::GetComponent<Transform>(floorTiles[i]).SetPosition(vec3(-187.f + (i * 25), -7.f, 10.f));

		}

		//setup roof path
		{
			//Creates entity
			roofTiles[i] = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(roofTiles[i]);
			ECS::AttachComponent<Transform>(roofTiles[i]);

			//Sets up components

			double tileSizing = 0.2;
			int randomTiling = rand() % 10;

			std::string fileName = "Soil.png";

			if (randomTiling > 5) {
				fileName = "EnrichedSoil.png";
			}

			ECS::GetComponent<Sprite>(roofTiles[i]).LoadSprite(fileName, 128 * tileSizing, 128 * tileSizing);
			ECS::GetComponent<Transform>(roofTiles[i]).SetPosition(vec3(-187.f + (i * 25), 165.f, 10.f));

		}
	}

	for (int i = 0; i < canisterFor; i++) {

		//Setup canister
		{
			//Creates entity
			canisters[i] = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(canisters[i]);
			ECS::AttachComponent<Transform>(canisters[i]);
			ECS::AttachComponent<PhysicsBody>(canisters[i]);

			//Sets up components
			std::string fileName = "RandomAlienTileSets2.png";

			double canisterSize = 0.2;

			ECS::GetComponent<Sprite>(canisters[i]).LoadSprite(fileName, (128 * canisterSize), (128 * canisterSize));
			ECS::GetComponent<Transform>(canisters[i]).SetPosition(vec3(0.f, 0.f, 20.f));

			auto& tempSpr = ECS::GetComponent<Sprite>(canisters[i]);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(canisters[i]);

			float shrinkX = 0.f;
			float shrinkY = 0.f;
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(190.f), float32(18.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempPhsBody = PhysicsBody(canisters[i], tempBody, float(tempSpr.GetWidth() - shrinkX),
				float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, OBJECTS, PLAYER | /*ENEMY |*/ OBJECTS | HEXAGON);
			tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

		}
	}

	//Binding scrolling to mainPlayer KEEP THESE AT THE VERY BOTTOM OF INITSCENE
	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void InfiniteRunner::Update(){
	
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& gameOverSplash = ECS::GetComponent<Sprite>(gameoverScreen);
	auto& gameOverTransform = ECS::GetComponent<Transform>(gameoverScreen);


	float canisterSpeed = -0.75f; //default -0.25f?
	float backgroundSpeed = -0.5;

	int setBackBackground = -1000;
	int reAppearBack = 1000;

	int setBackDistance = -300; //Adjusdt these to line up tiles -0.25f
	int reAppearDistance = 575; //adjust these to line up tiles

	

	//Random positioning of canisters
	for (int i = 0; i < canisterFor; i++) {
		auto& caniPhyBody = ECS::GetComponent<PhysicsBody>(canisters[i]);
		auto& caniSprite = ECS::GetComponent<Sprite>(canisters[i]);
		

		caniPhyBody.SetPosition(b2Vec2(caniPhyBody.GetPosition().x + canisterSpeed, caniPhyBody.GetPosition().y));

		if (caniPhyBody.GetPosition().x < setBackDistance) {

			//Random position based
			//caniPhyBody.SetPosition(b2Vec2(reAppearDistance + canisterSpeed, rand()% 130 + 18));
			

		}
	}


	for (int i = 0; i < tilesFor; i++)
	{
		auto& rTiles = ECS::GetComponent<Transform>(roofTiles[i]);
		auto& fTiles = ECS::GetComponent<Transform>(floorTiles[i]);

	//	rTiles.SetPosition(vec3(-187.f + (i * 25), 165.f, 10.f));
	//	fTiles.SetPosition(vec3(-187.f + (i * 25), -7.f, 10.f));

		rTiles.SetPosition(vec3(rTiles.GetPosition().x + canisterSpeed, rTiles.GetPosition().y,10.f));
		fTiles.SetPosition(vec3(fTiles.GetPosition().x + canisterSpeed, fTiles.GetPosition().y,10.f));

		if (rTiles.GetPositionX() < setBackDistance) {
			rTiles.SetPosition(vec3(reAppearDistance + canisterSpeed, rTiles.GetPosition().y, 10.f));
		}

		if (fTiles.GetPositionX() < setBackDistance) {
			fTiles.SetPosition(vec3(reAppearDistance + canisterSpeed, fTiles.GetPosition().y, 10.f));
		}

	}

	for (int i = 0; i < 2; i++) {
		auto& backgroundSprite = ECS::GetComponent<Transform>(backgroundTile[i]);
		backgroundSprite.SetPosition(vec3(backgroundSprite.GetPosition().x + backgroundSpeed, backgroundSprite.GetPosition().y, 5.f));

		if (backgroundSprite.GetPositionX() < setBackBackground) {
			backgroundSprite.SetPosition(vec3(reAppearBack + backgroundSpeed, backgroundSprite.GetPosition().y, 10.f));
		}
		
	}


	//Check if player is in kill zone / gameover area
	if (player.GetPosition().x < -60) {
		gameOverState = true;
	}

	else {
		gameOverState = false;
	}

	if (gameOverState == true) {

		gameOverSplash.SetTransparency(1);

		gameOverTransform.SetPosition(vec3(player.GetPosition().x, player.GetPosition().y,100.f));

	}

	if (gameOverState == false) {
		gameOverSplash.SetTransparency(0);
	}



	//std::cout<<"Delta: "<<Timer::currentClock<<std::endl;
	//std::cout << "Time: " << Timer::time << std::endl;
	

}

void InfiniteRunner::KeyboardHold(){

	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());

	float speed = 1.f;
	b2Vec2 vel = b2Vec2(0.f, 0.f);




	if (Input::GetKey(Key::A))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(-400000.f * speed, 0.f), true);
	}
	if (Input::GetKey(Key::D))
	{
		player.GetBody()->ApplyForceToCenter(b2Vec2(400000.f * speed, 0.f), true);
	}
}

void InfiniteRunner::KeyboardDown(){
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& canJump = ECS::GetComponent<CanJump>(MainEntities::MainPlayer());
	auto& alienRef = ECS::GetComponent<PhysicsBody>(alienBoss);

	double jumpCalc = player.GetMass() * 98.f;
	double jumpCalcAlien = alienRef.GetMass() * 98.f;

	if (Input::GetKeyDown(Key::R) /*&& gameOverState == true*/)
	{
		player.SetPosition(b2Vec2(0,35));
		

		//Changing environments on reset
		std::string fileName = "ARandomTile.png";
		double tileSizing = 0.20;

		int Tilerandomizer = rand() % 30; //0-10 is soil biome, 11-10 is corridor, 20+ is ____ biome
		std::cout << Tilerandomizer << std::endl;

		

		for (int i = 0; i < tilesFor; i++) {
			auto& roofTilesToChange = ECS::GetComponent<Sprite>(roofTiles[i]);
			auto& floorTilesToChange = ECS::GetComponent<Sprite>(floorTiles[i]);


			if (Tilerandomizer < 10) {
				int randomTiling = rand() % 10;

				roofTilesToChange.SetTransparency(1);

				fileName = "Soil.png";

				if (randomTiling > 5) {
					fileName = "EnrichedSoil.png";
				}
			}

			if (Tilerandomizer > 10 && Tilerandomizer < 20) {
				int randomTiling = rand() % 10;

				roofTilesToChange.SetTransparency(1);

				fileName = "ARandomTile.png";

				if (randomTiling < 3) {
					fileName = "PlateWire.png";
				}
			}

			if (Tilerandomizer > 20) {
				int randomTiling = rand() % 10;

				roofTilesToChange.SetTransparency(0);

				fileName = "moon_tileset_base.png";

				if (randomTiling > 5) {
					fileName = "moon_tileset_accent.png";
				}
			}

			roofTilesToChange.LoadSprite(fileName, 128 * tileSizing, 128 * tileSizing);
			floorTilesToChange.LoadSprite(fileName, 128 * tileSizing, 128 * tileSizing);

			roofTilesToChange.Draw();
			floorTilesToChange.Draw();

		}

		for (int i = 0; i < 2; i++) {

			auto& backgroundToChange = ECS::GetComponent<Sprite>(backgroundTile[i]);
			auto& fillerToChangeOne = ECS::GetComponent<Sprite>(gapFillerOne);
			auto& fillerToChangeTwo = ECS::GetComponent<Sprite>(gapFillerTwo);
			auto& boundToChangeBot = ECS::GetComponent<Sprite>(boundFillerBot);
			auto& boundToChangeTop = ECS::GetComponent<Sprite>(boundFillerTop);

			std::string fileName = "BackgroundTunnel1.png";
			std::string gapFillerFilename = "Soil.png";
			std::string boundFilenameTop = "CaveShadingTop.png";
			std::string boundFilenameBot = "CaveShadingBot.png";

			if (Tilerandomizer < 10) {
				fillerToChangeTwo.SetTransparency(1);
				boundToChangeTop.SetTransparency(1);
				fileName = "BackgroundTunnel1.png";
				gapFillerFilename = "Soil.png";

				 boundFilenameTop = "CaveShadingTop.png";
				 boundFilenameBot = "CaveShadingBot.png";

			}

			if (Tilerandomizer > 10 && Tilerandomizer < 20) {
				fillerToChangeTwo.SetTransparency(1);
				boundToChangeTop.SetTransparency(1);
				fileName = "CorridorBackground.png";
				gapFillerFilename = "ARandomTile.png";

				boundFilenameTop = "PlateShadingTop.png";
				boundFilenameBot = "PlateShadingBot.png";

			}

			if (Tilerandomizer > 20) {
				fillerToChangeTwo.SetTransparency(0);
				boundToChangeTop.SetTransparency(0);
				fileName = "MoonStuff.png";
				gapFillerFilename = "moon_tileset_base.png";

				boundFilenameTop = "MoonShadingTop.png";
				boundFilenameBot = "MoonShadingBot.png";
			}

			
			ECS::GetComponent<Sprite>(gapFillerOne).LoadSprite(gapFillerFilename, 500, 128 * 0.2);
			ECS::GetComponent<Sprite>(gapFillerOne).Draw();

			ECS::GetComponent<Sprite>(gapFillerTwo).LoadSprite(gapFillerFilename, 500, 128 * 0.2);
			ECS::GetComponent<Sprite>(gapFillerTwo).Draw();

			boundToChangeBot.LoadSprite(boundFilenameBot,1280,384);
			boundToChangeTop.LoadSprite(boundFilenameTop, 1280, 384);

			boundToChangeBot.Draw();
			boundToChangeTop.Draw();

			backgroundToChange.LoadSprite(fileName, 1000, 175);
			backgroundToChange.Draw();
			

		}



	}


	if (canJump.m_canJump)
	{
		if (Input::GetKeyDown(Key::Space) || Input::GetKeyDown(Key::W) || Input::GetKeyDown(Key::UpArrow))
		{

			Timer::Reset();


			player.GetBody()->ApplyLinearImpulseToCenter(b2Vec2(0.f, jumpCalc), true);

			//To allow alien to jump
			//alienRef.GetBody()->ApplyLinearImpulseToCenter(b2Vec2(0.f, jumpCalcAlien), true);

			canJump.m_canJump = false;
		}
	}
	
	//std::cout << "Player Y location: " << player.GetPosition().y;

}

void InfiniteRunner::KeyboardUp(){

	auto& canJump = ECS::GetComponent<CanJump>(MainEntities::MainPlayer());
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	
	//Single jump mode
	//if (Timer::time > 1.f)
	{

	//	canJump.m_canJump = true;

	}


	//Double jump mode
	if (player.GetPosition().y < 28 && canJump.m_canJump == false || Timer::time > 2.f)
	{

		canJump.m_canJump = true;

	}



//flaappy bird mode
/*	if (!canJump.m_canJump)
	{
		if (Input::GetKeyDown(Key::Space))
		{
			canJump.m_canJump = true;
		}
	}*/
	

}
