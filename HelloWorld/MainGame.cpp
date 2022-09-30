#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

int DISPLAY_WIDTH = 1024;
int DISPLAY_HEIGHT = 768;
int DISPLAY_SCALE = 1;

enum ScreenSide
{
	TOPVERTICAL = 0,
	RIGHTHORIZONTAL,
	BOTTOMVERTICAL,
	LEFTHORIZONTAL
};

struct GameState
{
	int lives = 0;
	int score = 0;
	ScreenSide screenSide = TOPVERTICAL;
};

GameState gameState;

enum GameObjectType
{
	TYPE_NULL = -1,
	TYPE_CHARACTER,
	TYPE_APPLE,
	TYPE_APPLE_STILL,
	TYPE_LEMON,
	TYPE_LEMON_STILL,
	TYPE_ORANGE,
	TYPE_ORANGE_STILL,
	TYPE_PEAR,
	TYPE_PEAR_STILL,
	TYPE_STRAWBERRY,
	TYPE_STRAWBERRY_STILL,
	TYPE_WATERMELON,
	TYPE_WATERMELON_STILL,
	TYPE_DESTROYED
};

void HandlePlayerControls();
void GenerateFruits();
void UpdateFruits();
void UpdateDestroyedFruit();

void UpdateFruitMovement(GameObject& object, int screenSide);
void StillFruits(std::vector<int>& vFruitStill, int pair_score, int penalty);
void CollideWithFruits(std::vector<int>& vFruits, GameObjectType& TYPE_FRUIT_STILL, const char[80]);
void Wrapping(GameObject& object, Point2f& pos);

void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();
	Play::LoadBackground("Data\\Backgrounds\\dawnbackground.png");
	Play::StartAudioLoop("chip1master");
	Play::CreateGameObject(TYPE_CHARACTER, { DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 }, 50, "mushroom_idle_right");
}

bool MainGameUpdate( float elapsedTime )
{
	Play::DrawBackground();
	HandlePlayerControls();
	GenerateFruits();
	UpdateFruits();
	UpdateDestroyedFruit();
	Play::DrawFontText("64px", "USE ARROW KEYS TO MOVE. SPACE TO DASH", { DISPLAY_WIDTH / 2, 30 }, Play::CENTRE);
	Play::DrawFontText("64px", "SCORE: " + std::to_string(gameState.score), { DISPLAY_WIDTH / 4, DISPLAY_HEIGHT - 30 }, Play::CENTRE);
	Play::DrawFontText("64px", "FRAMES: " + std::to_string(Play::GetGameObjectByType(TYPE_CHARACTER).frame), {3 * DISPLAY_WIDTH / 4, DISPLAY_HEIGHT - 30 }, Play::CENTRE);
	Play::PresentDrawingBuffer();
	return Play::KeyDown( VK_ESCAPE );
}

int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}


void HandlePlayerControls()
{
	GameObject& obj_character = Play::GetGameObjectByType(TYPE_CHARACTER);
	int player_speed = 7;
	if (Play::KeyDown(VK_RIGHT))
	{
		obj_character.velocity = { player_speed,0 };
		Play::SetSprite(obj_character, "mushroom_walk_right", 0.25f);
	}
	else if (Play::KeyDown(VK_LEFT))
	{
		obj_character.velocity = { -player_speed,0 };
		Play::SetSprite(obj_character, "mushroom_walk_left", 0.25f);
	}
	else if (Play::KeyDown(VK_DOWN))
	{
		Play::SetSprite(obj_character, "mushroom_walk_right", 0.25f);
		obj_character.velocity = { 0,player_speed };
	}
	else if (Play::KeyDown(VK_UP))
	{
		Play::SetSprite(obj_character, "mushroom_walk_right", 0.25f);
		obj_character.velocity = { 0,-player_speed };
	}
	else
	{
		Play::SetSprite(obj_character, "mushroom_idle_right", 0.25f);
		obj_character.velocity = { 0,0 };
		obj_character.animSpeed *= 0.5f;
	}

	if (Play::KeyPressed(VK_SPACE))
	{
		obj_character.velocity *= 10;
	}

	Play::UpdateGameObject(obj_character);
	Wrapping(obj_character, obj_character.pos);
	Play::DrawObjectRotated(obj_character);
}

void GenerateFruits()
{
	if (Play::RandomRoll(50) == 1)
	{
		int fruit = Play::RandomRoll(6); //decide which fruit the sprite is
		int side = Play::RandomRoll(4);
		if (fruit == 1)
		{
			int id_apple = Play::CreateGameObject(TYPE_APPLE, { 0,0 }, 20, "apple_run");
			GameObject& obj_apple = Play::GetGameObject(id_apple);
			obj_apple.animSpeed = 0.25f;
			UpdateFruitMovement(obj_apple, side);
		}  
		else if (fruit == 2) //lemon
		{
			int id_lemon = Play::CreateGameObject(TYPE_LEMON, { 0,0 }, 20, "lemon_run");
			GameObject& obj_lemon = Play::GetGameObject(id_lemon);
			obj_lemon.animSpeed = 0.25f;
			UpdateFruitMovement(obj_lemon, side);
		}
		else if (fruit == 3) //orange
		{
			int id_orange = Play::CreateGameObject(TYPE_ORANGE, { 0,0 }, 20, "orange_run");
			GameObject& obj_orange = Play::GetGameObject(id_orange);
			obj_orange.animSpeed = 0.25f;
			UpdateFruitMovement(obj_orange, side);
		}
		else if (fruit == 4) //pear
		{
			int id_pear = Play::CreateGameObject(TYPE_PEAR, { 0,0 }, 20, "pear_run");
			GameObject& obj_pear = Play::GetGameObject(id_pear);
			obj_pear.animSpeed = 0.25f;
			UpdateFruitMovement(obj_pear, side);
		}
		else if (fruit == 5) //strawberry
		{
			int id_strawberry = Play::CreateGameObject(TYPE_STRAWBERRY, { 0,0 }, 20, "strawberry_run");
			GameObject& obj_strawberry = Play::GetGameObject(id_strawberry);
			obj_strawberry.animSpeed = 0.25f;
			UpdateFruitMovement(obj_strawberry, side);
		}
		else if (fruit == 6) //watermelon
		{
			int id_watermelon = Play::CreateGameObject(TYPE_WATERMELON, { 0,0 }, 20, "watermelon_run");
			GameObject& obj_watermelon = Play::GetGameObject(id_watermelon);
			obj_watermelon.animSpeed = 0.25f;
			UpdateFruitMovement(obj_watermelon, side);
		}
	}
}

void UpdateFruitMovement(GameObject& object, int screenside)
{
	switch (screenside)
	{
	case 1:
		gameState.screenSide = TOPVERTICAL;
		break;
	case 2:
		gameState.screenSide = BOTTOMVERTICAL;
		break;
	case 3:
		gameState.screenSide = RIGHTHORIZONTAL;
		break;
	case 4:
		gameState.screenSide = LEFTHORIZONTAL;
		break;
	}

	switch (gameState.screenSide)
	{
	case TOPVERTICAL:
		object.pos = { Play::RandomRoll(DISPLAY_WIDTH), 0 };
		object.velocity = { 0,3 };
		break;

	case BOTTOMVERTICAL:
		object.pos = { Play::RandomRoll(DISPLAY_WIDTH), DISPLAY_HEIGHT };
		object.velocity = { 0,-3 };
		break;

	case RIGHTHORIZONTAL:
		object.pos = { 0, Play::RandomRoll(DISPLAY_HEIGHT) };
		object.velocity = { 3, 0 };
		break;

	case LEFTHORIZONTAL:
		object.pos = { DISPLAY_WIDTH, Play::RandomRoll(DISPLAY_HEIGHT) };
		object.velocity = { -3, 0 };
		break;
	}
}

void StillFruits(std::vector<int>& vFruitStill, int pair_score, int penalty)
{

	for (int id : vFruitStill)
	{
		GameObject& obj_fruit_still = Play::GetGameObject(id);
		Play::UpdateGameObject(obj_fruit_still);

		if (obj_fruit_still.frame > 100)
		{
			if (obj_fruit_still.frame % 2 == 0)
			{
				Play::DrawObject(obj_fruit_still);
			}

			if (obj_fruit_still.frame > 200)
			{
				gameState.score -= penalty;
				obj_fruit_still.type = TYPE_DESTROYED;
			}
		}
		else
		{
			Play::DrawObject(obj_fruit_still);
		}

		if (vFruitStill.size() == 2)
		{
			gameState.score += pair_score;
			obj_fruit_still.type = TYPE_DESTROYED;
		}
	}
}

void CollideWithFruits(std::vector<int>& vFruits, const GameObjectType& TYPE_FRUIT_STILL, const char* fruit)
{
	GameObject& obj_character = Play::GetGameObjectByType(TYPE_CHARACTER);
	for (int id : vFruits)
	{
		GameObject& obj_fruit = Play::GetGameObject(id);
		if (Play::IsColliding(obj_fruit, obj_character))
		{
			Play::SetSprite(obj_fruit, fruit, 0.25f);
			obj_fruit.velocity = { 0,0 };
			obj_fruit.type = TYPE_FRUIT_STILL;
		}

		Play::UpdateGameObject(obj_fruit);
		Play::DrawObject(obj_fruit);

		if (!Play::IsVisible)
			Play::DestroyGameObject(id);
	}
}

void UpdateFruits()
{
	// apples
	std::vector<int> vApples = Play::CollectGameObjectIDsByType(TYPE_APPLE);
	CollideWithFruits(vApples, TYPE_APPLE_STILL, "apple_still");

	std::vector<int> vApplesStill = Play::CollectGameObjectIDsByType(TYPE_APPLE_STILL);
	StillFruits(vApplesStill, 50, 25);
	//

	//Lemons
	std::vector<int> vLemons = Play::CollectGameObjectIDsByType(TYPE_LEMON);
	CollideWithFruits(vLemons, TYPE_LEMON_STILL, "lemon_still");

	std::vector<int> vLemonsStill = Play::CollectGameObjectIDsByType(TYPE_LEMON_STILL);
	StillFruits(vLemonsStill, 50, 25);
	//

	//Oranges
	std::vector<int> vOranges = Play::CollectGameObjectIDsByType(TYPE_ORANGE);
	CollideWithFruits(vOranges, TYPE_ORANGE_STILL, "orange_still");

	std::vector<int> vOrangesStill = Play::CollectGameObjectIDsByType(TYPE_ORANGE_STILL);
	StillFruits(vOrangesStill, 50, 25);
	//

	//Pears
	std::vector<int> vPears = Play::CollectGameObjectIDsByType(TYPE_PEAR);
	CollideWithFruits(vPears, TYPE_PEAR_STILL, "pear_still");

	std::vector<int> vPearsStill = Play::CollectGameObjectIDsByType(TYPE_PEAR_STILL);
	StillFruits(vPearsStill, 50, 25);
	//

	//Strawberries
	std::vector<int> vStrawberries = Play::CollectGameObjectIDsByType(TYPE_STRAWBERRY);
	CollideWithFruits(vStrawberries, TYPE_STRAWBERRY_STILL, "strawberry_still");

	std::vector<int> vStrawberriesStill = Play::CollectGameObjectIDsByType(TYPE_STRAWBERRY_STILL);
	StillFruits(vStrawberriesStill, 50, 25);
	//

	//Watermelons
	std::vector<int> vWatermelons = Play::CollectGameObjectIDsByType(TYPE_WATERMELON);
	CollideWithFruits(vWatermelons, TYPE_WATERMELON_STILL, "watermelon_still");

	std::vector<int> vWatermelonsStill = Play::CollectGameObjectIDsByType(TYPE_WATERMELON_STILL);
	StillFruits(vWatermelonsStill, 50, 25);
	//
}

void UpdateDestroyedFruit()
{
	std::vector<int> vDead = Play::CollectGameObjectIDsByType(TYPE_DESTROYED);

	for (int id_dead : vDead)
	{
		GameObject& obj_dead = Play::GetGameObject(id_dead);
		obj_dead.animSpeed = 0.2f; 
		Play::UpdateGameObject(obj_dead);

		/*if (obj_dead.frame % 2) 
			Play::DrawObjectRotated(obj_dead, (10 - obj_dead.frame) / 10.0f); */

		if (!Play::IsVisible(obj_dead) || obj_dead.frame >= 10)  
			Play::DestroyGameObject(id_dead);
	}
}

void Wrapping(GameObject& object, Point2f& pos)
{
	if (Play::IsLeavingDisplayArea(object))
	{
		float x = pos.x;
		float y = pos.y;

		if (!Play::IsVisible(object))
		{
			if (x < 0 || x > DISPLAY_WIDTH)
				object.pos = { DISPLAY_WIDTH - x, y };
			if (y < 0 || y > DISPLAY_HEIGHT)
				object.pos = { x, DISPLAY_HEIGHT - y };
		}
	}
}

/*
Things to add:
Different fruits have different spawn rates/speeds/penalty/pair_score?
Collecting different fruits allows for perks, i.e. moving faster. Abilitiy to dash
Audio played when a pair is collected
Audio played when a fruit "times out"
*/