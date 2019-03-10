#pragma once
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Character.hpp"
#include "Command.hpp"
#include "CommandQueue.hpp"
#include "BloomEffect.hpp"
#include "SoundPlayer.hpp"
#include "Platform.hpp"
#include "Interactable.hpp"
#include "NetworkProtocol.hpp"

#include "SFML/System/NonCopyable.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/Graphics/Texture.hpp"

#include <array>
#include <queue>

//Forward declaration
namespace sf
{
	class RenderTarget;
}

class NetworkNode;

class World : private sf::NonCopyable {
public:
	explicit World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, bool networked = false);
	void update(sf::Time dt);
	void draw();

	CommandQueue& getCommandQueue();
	Character* addCharacter(int identifier);
	bool hasAlivePlayer() const;
	int hasPlayerReachedEnd() const;
	bool pollGameAction(GameActions::Action& out);
	Character* getCharacter(int identifier) const;
	void removeCharacter(int identifier);
	void setCurrentBattleFieldPosition(float lineY);
	void setWorldHeight(float height);
	void setWorldScrollCompensation(float compensation);

	sf::FloatRect getViewBounds() const;
	sf::FloatRect getBattlefieldBounds() const;
private:
	void loadTextures();
	void buildScene();
	void adaptPlayerPosition();
	void updateGravity(sf::Time dt);
	void destroyOutOfBoundPlayer();
	void handleCollisions();
	void updateSounds();
	void hasWon();

	void addPlatforms();
	void addPlatform(Platform::Type type, float relX, float relY);
	void createPlatforms();
	//void destroyEntitiesOutsideView();

	void addInteractables();
	void addInteractable(Interactable::Type type, float relX, float relY, Platform::Type pType, float pX, float pY);
	void createInteractables();
	float mGravityForce;


private:
	enum Layer{Background, LowerAir, UpperAir, LayerCount};

	struct SpawnPoint
	{
		SpawnPoint(Character::Type type, float x, float y)
			: type(type)
			, x(x)
			, y(y)
		{
		}

		Character::Type type;
		float x;
		float y;
	};

	struct InteractablePoint
	{
         InteractablePoint(Interactable::Type type, float x, float y, Platform::Type pType, float px, float py)
			 : type(type)
			 , x(x)
			 , y(y)
			 , pType(pType)
			 , px(px)
			 , py(py)
		 {
		 }

		 Interactable::Type type;
		 float x;
		 float y;
		 Platform::Type pType;
		 float px;
		 float py;
	};
	struct PlatformsPoint
	{
	    PlatformsPoint(Platform::Type type, float x, float y)
			: type(type)
			, x(x)
			, y(y)
		{
		}

		Platform::Type type;
		float x;
		float y;
	};
private:
	sf::RenderTarget& mTarget;
	sf::RenderTexture mSceneTexture;
	sf::View mWorldView;
	TextureHolder mTextures;
	FontHolder&	mFonts;
	SoundPlayer& mSounds;

	SceneNode mSceneGraph;
	std::array<SceneNode*, static_cast<int>(Layer::LayerCount)> mSceneLayers;
	CommandQueue mCommandQueue;

	sf::FloatRect mWorldBounds;
	sf::Vector2f mSpawnPosition;
	float mScrollSpeedCompensation;
	float mScrollSpeed;

	std::vector<Character*>	mPlayerCharacters;
	std::vector<PlatformsPoint> mWorldPlatforms;
	std::vector<InteractablePoint> mInteractables;
	std::vector<Character*> mActiveEnemies;

	bool								mNetworkedWorld;
	NetworkNode*						mNetworkNode;
	BloomEffect	mBloomEffect;
};
