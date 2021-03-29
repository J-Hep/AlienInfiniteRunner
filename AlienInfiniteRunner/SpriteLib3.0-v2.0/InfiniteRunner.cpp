#include "InfiniteRunner.h"
#include "Utilities.h"
#include "timer.h"

int tilesFor = 32;

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

		double astroScale = 0.35;
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

		//tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
		tempPhsBody = PhysicsBody(entity, tempBody, float((tempSpr.GetHeight() - shrinkY) / 2.f), vec2(0.f, 0.f), false, PLAYER, ENVIRONMENT | ENEMY | OBJECTS | PICKUP | TRIGGER | HEXAGON, 0.5f, 3.f);
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

		ECS::GetComponent<Transform>(alienBoss).SetPosition(vec3(0.f, 0.f, 10.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(alienBoss);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(alienBoss);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(-100.f), float32(50.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		//tempPhsBody = PhysicsBody(entity, tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, PLAYER, ENEMY | OBJECTS | PICKUP | TRIGGER, 0.5f, 3.f);
		tempPhsBody = PhysicsBody(alienBoss, tempBody, float((tempSpr.GetHeight() - shrinkY) / 2.25f), vec2(0.f, 0.f), false, ENEMY, ENVIRONMENT | ENEMY | OBJECTS | PICKUP | TRIGGER | HEXAGON, 0.5f, 3.f);
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

	//Setup static tunnel Background
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "DarkGraySquare.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 500, 175);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 75.f, 5.f));

	}


	//Setup static Background
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "PlaceHolderSpace.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 500, 500);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 50.f, 1.f));

	}

	//Setup static Background floor
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "PlaceHolderMoon.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 500, 500);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, -200.f, 2.f));

	}


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

			double tileSizing = 0.2;

			std::string fileName = "ARandomTile.png";
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

			std::string fileName = "ARandomTile.png";
			ECS::GetComponent<Sprite>(roofTiles[i]).LoadSprite(fileName, 128 * tileSizing, 128 * tileSizing);
			ECS::GetComponent<Transform>(roofTiles[i]).SetPosition(vec3(-187.f + (i * 25), 165.f, 10.f));

		}
	}

	for (int i = 0; i < 16; i++) {

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
				float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false, GROUND, PLAYER | /*ENEMY |*/ OBJECTS | HEXAGON);
			tempPhsBody.SetColor(vec4(0.f, 1.f, 0.f, 0.3f));

		}
	}

	//Binding scrolling to mainPlayer KEEP THESE AT THE VERY BOTTOM OF INITSCENE
	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
}

void InfiniteRunner::Update(){
	
	float canisterSpeed = -0.75f; //default -0.25f?

	int setBackDistance = -215; //Adjusdt these to line up tiles -0.25f
	int reAppearDistance = 250; //adjust these to line up tiles

	for (int i = 0; i < 16; i++) {
		auto& cani = ECS::GetComponent<PhysicsBody>(canisters[i]);
		cani.SetPosition(b2Vec2(cani.GetPosition().x + canisterSpeed, cani.GetPosition().y));

		if (cani.GetPosition().x < setBackDistance) {
			cani.SetPosition(b2Vec2(reAppearDistance + canisterSpeed, cani.GetPosition().y));
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

	std::cout<<"Delta: "<<Timer::currentClock<<std::endl;
	

}

void InfiniteRunner::KeyboardHold(){

	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());

	float speed = 1.f;
	b2Vec2 vel = b2Vec2(0.f, 0.f);

	if (Input::GetKey(Key::A))
	{
	//	player.GetBody()->ApplyForceToCenter(b2Vec2(-400000.f * speed, 0.f), true);
	}
	if (Input::GetKey(Key::D))
	{
		//player.GetBody()->ApplyForceToCenter(b2Vec2(400000.f * speed, 0.f), true);
	}
}

void InfiniteRunner::KeyboardDown(){
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	auto& canJump = ECS::GetComponent<CanJump>(MainEntities::MainPlayer());
	auto& alienRef = ECS::GetComponent<PhysicsBody>(alienBoss);


	double jumpCalc = player.GetMass() * 98.f;
	double jumpCalcAlien = alienRef.GetMass() * 98.f;


	if (canJump.m_canJump)
	{
		if (Input::GetKeyDown(Key::Space))
		{
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



	//Double jump mode
	if (player.GetPosition().y < 28 && canJump.m_canJump == false)
	{

		canJump.m_canJump = true;

	}



/* Flappy bird mode
	if (!canJump.m_canJump)
	{
		if (Input::GetKeyDown(Key::Space))
		{
			canJump.m_canJump = true;
		}
	}
	
*/
}
