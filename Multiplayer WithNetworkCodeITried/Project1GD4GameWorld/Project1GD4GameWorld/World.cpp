#include "World.hpp"
#include "TextNode.hpp"
#include "ParticleNode.hpp"
#include "PostEffect.hpp"
#include "SoundNode.hpp"
#include "Foreach.hpp"
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>

World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, bool networked)
	: mTarget(outputTarget)
	, mSceneTexture()
	, mWorldView(outputTarget.getDefaultView())
	, mTextures()
	, mFonts(fonts)
	, mSounds(sounds)
	, mSceneGraph()
	, mGravityForce(30000.f)
	, mSceneLayers()
	, mWorldBounds(-2000.f, 0.f, 14000.f, mWorldView.getSize().y)
	, mSpawnPosition(mWorldView.getSize().x / 8.f, 10.f)
	, mScrollSpeed(50.f)
	, mScrollSpeedCompensation(1.f)
	, mPlayerCharacters()
	, mNetworkedWorld(networked)
	, mNetworkNode(nullptr)
	, mActiveEnemies()
{
	mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);

	loadTextures();
	buildScene();

	// Prepare the view
	mWorldView.setCenter(mSpawnPosition);
}
void World::setWorldScrollCompensation(float compensation)
{
	mScrollSpeedCompensation = compensation;
}
void World::update(sf::Time dt)
{
	/*	Jaysson
		Move the world view (camera) by following the player infront
		i.e. player with the highest x coordinates
	*/
	float lead = 0.f;
	FOREACH(Character* c, mPlayerCharacters)
		if (c->getPosition().x > lead)
			lead = c->getPosition().x;
	mWorldView.setCenter(lead, mWorldBounds.height - mWorldView.getSize().y / 2.f);
	
	//Ryan
	createPlatforms();
	createInteractables();

	// Collision detection and response (may destroy entities)
	handleCollisions();

	/*	Jaysson
		Took the foreach function from networking code
		As preparation for implementing networking into the game, instead of hard-coding seperate checks for two characters
	*/
	FOREACH(Character* c, mPlayerCharacters)
		c->setVelocity(0.f, 0.f);

	//Ryan - Updates Gravity to Player Objects
	updateGravity(dt);

	destroyOutOfBoundPlayer();

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);

	// Remove all destroyed entities, create new ones
	mSceneGraph.removeWrecks();
	

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();
	updateSounds();

}

void World::draw()
{
	mTarget.setView(mWorldView);
	mTarget.draw(mSceneGraph);
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

bool World::hasAlivePlayer() const
{
	return mPlayerCharacters.size() > 0;
}

int World::hasPlayerReachedEnd() const
{
	FOREACH(Character* c, mPlayerCharacters)
	{
		if (c->getPosition().x > 12000.f)
		{
			return c->getIdentifier();
		}
	}
	return -1;
}

void World::loadTextures()
{
	mTextures.load(TextureIDs::Entities, "Media/Textures/Entities.png");
	mTextures.load(TextureIDs::Platform, "Media/Textures/Platform.png");
	mTextures.load(TextureIDs::Jungle, "Media/Textures/Background.png");
	mTextures.load(TextureIDs::Particle, "Media/Textures/Particle.png");
	mTextures.load(TextureIDs::FinishLine, "Media/Textures/FinishLine.png");
	mTextures.load(TextureIDs::Character, "Media/Textures/Character.png");
	mTextures.load(TextureIDs::Lever, "Media/Textures/Lever.png");
}

bool World::pollGameAction(GameActions::Action& out)
{
	return mNetworkNode->pollGameAction(out);
}
void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float xBorderDistance = 0.f;
	const float yBorderDistance = 50.f;

	FOREACH(Character* c, mPlayerCharacters)
	{
		sf::Vector2f position = c->getPosition();
		//Don't allow the character to go too far up
		position.y = std::max(position.y, yBorderDistance);
		c->setPosition(position);
	}
}

void World::destroyOutOfBoundPlayer()
{
	/*	Jaysson
		Checks if the player if inside the playzone
		If they goes into the left side of camera view or below the camera view, they lose and character is removed.
	*/
	Character* toBeRemoved;
	bool removingCharacter = false;

	FOREACH(Character* c, mPlayerCharacters)
	{
		//if the player couldn't catch up to the lead player or the player fall below the view bound
		//200 pixels of space is added to the left, allowing player falling behind to catch up even though the character is not visible
		if (c->getPosition().x < (mWorldView.getCenter().x - mWorldView.getSize().x / 2) - 200.f || c->getPosition().y > mWorldView.getSize().y)
		{
			//destroy the character
			c->destroy();
			removingCharacter = true;
			//record which player to be removed from the mPlayerCharacters vector
			//the player doesn't get moved from the vector now as it will cause the foreach loop to goes out of bound on index.
			toBeRemoved = c;
		}
	}

	//Remove the player recorded for removal after foreach loop
	if (removingCharacter)
	{
		mPlayerCharacters.erase(std::find(mPlayerCharacters.begin(), mPlayerCharacters.end(), toBeRemoved));
		std::cout << "Player " + std::to_string(toBeRemoved->getIdentifier() + 1) + " lost!" << std::endl;
	}
}
void World::setCurrentBattleFieldPosition(float lineX)
{
	mWorldView.setCenter(mWorldView.getCenter().y, lineX - mWorldView.getSize().x / 2);
	mSpawnPosition.x = mWorldBounds.height;
}
void World::setWorldHeight(float height)
{
	mWorldBounds.height = height;
}
//written by Ryan modified by Jaysson for Multiplayer {
void World::updateGravity(sf::Time dt)
{
	FOREACH(Character* c, mPlayerCharacters)
	{
		sf::Vector2f velocity = c->getVelocity();
		float vy = velocity.y + (mGravityForce *dt.asSeconds());
		c->setVelocity(velocity.x, vy);
	}
}
//}

Character* World::getCharacter(int identifier) const
{
	FOREACH(Character* a, mPlayerCharacters)
	{
		if (a->getIdentifier() == identifier)
			return a;
	}

	return nullptr;
}

void World::removeCharacter(int identifier)
{
	Character* character = getCharacter(identifier);
	if (character)
	{
		character->destroy();
		mPlayerCharacters.erase(std::find(mPlayerCharacters.begin(), mPlayerCharacters.end(), character));
	}
}

bool matchesCategories(SceneNode::Pair& colliders, Category type1, Category type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if (static_cast<int>(type1) & category1 && static_cast<int>(type2) & category2)
	{
		return true;
	}
	else if (static_cast<int>(type1) & category2 && static_cast<int>(type2) & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

//Ryan
void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	FOREACH(SceneNode::Pair pair, collisionPairs)
	{
		if (matchesCategories(pair, Category::PlayerCharacter, Category::Platform))
		{
			auto& player = static_cast<Character&>(*pair.first);
			auto& platform = static_cast<Platform&>(*pair.second);

			sf::Vector2f playerPosition = player.getPosition();
			sf::Vector2f platformPosition = platform.getPosition();
			if (playerPosition.y < platformPosition.y)
			{
				playerPosition.y = platformPosition.y - 48.f;

				player.setPosition(playerPosition);
				player.canJump();
			}
		}

		//Collision checks for player is removed for now
		//It causes issues where when a player walk into another, it elevates one character to stand on top of another character
		/*
		if (matchesCategories(pair, Category::PlayerCharacter, Category::PlayerCharacter))
		{
			auto& playerA = static_cast<Character&>(*pair.first);
			auto& playerB = static_cast<Character&>(*pair.second);
			
			sf::Vector2f playerPositionA = playerA.getPosition();
			sf::Vector2f playerPositionB = playerB.getPosition();
			if (playerPositionA.y < playerPositionB.y)
			{
				playerPositionA.y = playerPositionB.y - 90.f;
				
				playerA.setPosition(playerPositionA);
				playerA.canJump();
			}
			else if (playerPositionB.y < playerPositionA.y)
			{
				playerPositionB.y = playerPositionA.y - 90.f;
				
				playerB.setPosition(playerPositionB);
				playerB.canJump();
			}
		}
		*/

		if (matchesCategories(pair, Category::PlayerCharacter, Category::Lever))
		{
			auto& lever = static_cast<Interactable&>(*pair.second);

			if (!lever.hasInteractedWith())
			{
				lever.InteractWith();
				addPlatform(Platform::Type::Moving, lever.getPlatform().position.x, lever.getPlatform().position.y);

			}
		}
	}
}

void World::updateSounds()
{
	// Set listener's position to player position
	mSounds.setListenerPosition(mWorldView.getCenter());

	// Remove unused sounds
	mSounds.removeStoppedSounds();
}

//Modified by both Ryan and Jaysson {
void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < Layer::LayerCount; ++i)
	{
		Category category = (i == Layer::LowerAir) ? Category::SceneAirLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	// Prepare the tiled background
	sf::Texture& jungleTexture = mTextures.get(TextureIDs::Jungle);
	jungleTexture.setRepeated(true);

	float viewHeight = mWorldView.getSize().y;
	sf::IntRect textureRect(mWorldBounds);
	textureRect.height += static_cast<int>(viewHeight);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> jungleSprite(new SpriteNode(jungleTexture, textureRect));
	jungleSprite->setPosition(mWorldBounds.left, mWorldBounds.top);
	mSceneLayers[Layer::Background]->attachChild(std::move(jungleSprite));

	// Add the finish line to the scene
	sf::Texture& finishTexture = mTextures.get(TextureIDs::FinishLine);
	std::unique_ptr<SpriteNode> finishSprite(new SpriteNode(finishTexture));
	finishSprite->setPosition(12000.f, 0.f);
	finishSprite->rotate(90);
	mSceneLayers[Background]->attachChild(std::move(finishSprite));

	// Add particle node to the scene
	std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(Particle::Type::Smoke, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(smokeNode));

	// Add exhaust particle node to the scene
	std::unique_ptr<ParticleNode> exhaustNode(new ParticleNode(Particle::Type::Exhaust, mTextures));
	mSceneLayers[LowerAir]->attachChild(std::move(exhaustNode));

	//Add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(mSounds));
	mSceneGraph.attachChild(std::move(soundNode));

	// Add Platforms and interactables to the lists of both.
	addPlatforms();
	addInteractables();

	std::unique_ptr<Character> playerOne(new Character(Character::Type::One, mTextures, mFonts, 0));
	playerOne->setPosition(mWorldView.getCenter());
	playerOne->setIdentifier(0);
	mPlayerCharacters.push_back(playerOne.get());
	mSceneLayers[UpperAir]->attachChild(std::move(playerOne));

	std::unique_ptr<Character> playerTwo(new Character(Character::Type::One, mTextures, mFonts, 1));
	playerTwo->setPosition(mWorldView.getCenter());
	playerTwo->setIdentifier(1);
	mPlayerCharacters.push_back(playerTwo.get());
	mSceneLayers[UpperAir]->attachChild(std::move(playerTwo));

	// Add network node, if necessary
	if (mNetworkedWorld)
	{
		std::unique_ptr<NetworkNode> networkNode(new NetworkNode());
		mNetworkNode = networkNode.get();
		mSceneGraph.attachChild(std::move(networkNode));
	}
}

Character* World::addCharacter(int identifier)
{
	std::unique_ptr<Character> player(new Character(Character::Type::One, mTextures, mFonts, 0));
	player->setPosition(mWorldView.getCenter());
	player->setIdentifier(identifier);

	mPlayerCharacters.push_back(player.get());
	mSceneLayers[UpperAir]->attachChild(std::move(player));
	return mPlayerCharacters.back();
}
//Ryan {
#pragma region Platforms
//Adds Platforms 
void World::addPlatforms() 
{

	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 50.f;
	// two types of platfomrs, stationary, are longer and are static, where the moving platfomrs are moving left to right. 
	addPlatform(Platform::Type::Stationary, -viewBounds.width / 2, -viewBounds.height / 2 +borderDistance);
	addPlatform(Platform::Type::Stationary, 520.f -viewBounds.width / 2, 90.f -viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Stationary, 1300.f-viewBounds.width / 2,  180.f -viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Stationary, 1900.f -viewBounds.width / 2, -viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Stationary, 2430.f -viewBounds.width / 2, -viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Moving, 3000.f- viewBounds.width / 2, 90.f -viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Moving, 3500.f - viewBounds.width / 2, 90.f - viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Stationary, 4200.f - viewBounds.width / 2, - viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Moving, 4800.f - viewBounds.width / 2, 90.f - viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Stationary, 5500.f - viewBounds.width / 2, 90.f - viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Moving, 6300.f - viewBounds.width / 2, 90.f - viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Stationary, 7150.f - viewBounds.width / 2, - viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Moving, 8000.f - viewBounds.width / 2, 90.f - viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Stationary, 8800.f - viewBounds.width / 2, 180.f - viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Stationary, 8200.f - viewBounds.width / 2, 360.f - viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Stationary, 9600.f - viewBounds.width / 2, 270.f - viewBounds.height / 2 + borderDistance);
	addPlatform(Platform::Type::Stationary, 11200.f - viewBounds.width / 2, - viewBounds.height / 2 + borderDistance);
}
void World::addPlatform(Platform::Type type, float relX, float relY)
{
	PlatformsPoint platform(type, mWorldView.getSize().x / 2.f + relX, mWorldBounds.height - mWorldView.getSize().y / 2.f - relY);
	mWorldPlatforms.push_back(platform);
}

void World::createPlatforms()
{
	// Create all Platforms
	while (!mWorldPlatforms.empty()
		&& mWorldPlatforms.back().y > getBattlefieldBounds().top)
	{
		PlatformsPoint platform = mWorldPlatforms.back();

		std::unique_ptr<Platform> platforms(new Platform(platform.type, mTextures));
		platforms->setPosition(platform.x, platform.y);
		
		mSceneLayers[Layer::UpperAir]->attachChild(std::move(platforms));

		mWorldPlatforms.pop_back();
	}
}
#pragma endregion

#pragma region Interactables

void World::addInteractables()
{
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 50.f;
	// For this interactable, We are passing in moving, but the values gets a bit messed up which causes it to crash when we colide.
	addInteractable(Interactable::Type::Lever, 8300.f - viewBounds.width / 2, 380.f - viewBounds.height / 2 + borderDistance, Platform::Type::Moving, 10100.f - viewBounds.width / 2, 720 - viewBounds.height / 2 + borderDistance);
	//addInteractable(Interactable::Type::Lever, - viewBounds.width / 2, 10 - viewBounds.height / 2 + borderDistance, Platform::Type::Moving,- viewBounds.width / 2, 360.f - viewBounds.height / 2 + borderDistance);
}

void World::addInteractable(Interactable::Type type, float relX, float relY, Platform::Type pType, float pX, float pY)
{
	InteractablePoint interactable(type, mWorldView.getSize().x / 2.f + relX, mWorldBounds.height - mWorldView.getSize().y / 2.f - relY, pType, mWorldView.getSize().x / 2.f + pX, mWorldBounds.height - mWorldView.getSize().y / 2.f - pY);
	mInteractables.push_back(interactable);
}

void World::createInteractables()
{
	// Create all Platforms
	while (!mInteractables.empty())
	{
		InteractablePoint interactable = mInteractables.back();

		std::unique_ptr<Interactable> interactables(new Interactable(interactable.type, mTextures));
		interactables->setPosition(interactable.x, interactable.y);
		interactables->setNewPlatform(interactable.px, interactable.py, interactable.pType);

		mSceneLayers[Layer::UpperAir]->attachChild(std::move(interactables));

		mInteractables.pop_back();
	}
}
#pragma endregion

//}
sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const
{
	// Return view bounds + some area at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}