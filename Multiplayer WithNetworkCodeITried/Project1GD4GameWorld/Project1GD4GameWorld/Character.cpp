#include "Character.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "CommandQueue.hpp"
#include "ResourceHolder.hpp"
#include "SoundNode.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>


namespace
{
	const std::vector<CharacterData> Table = initializeCharacterData();
}

Character::Character(Type type, const TextureHolder& textures, const FontHolder& fonts, int identifier)
	: Entity(Table[static_cast<int>(type)].hitpoints)
	, mType(type)
	, mSprite(textures.get(Table[static_cast<int>(type)].texture), Table[static_cast<int>(type)].textureRect)
	, mAnimation(textures.get(TextureIDs::Character))
	, mTravelledDistance(0.f)
	, mDirectionIndex(0)
	, mMaxJumpHeight(1000.f)
	, mIsJumping(false)
	, mHealthDisplay(nullptr)
	, mPlayerNumberDisplay(nullptr)
	, mIdentifier(identifier)
{

	mAnimation.setFrameSize(sf::Vector2i(75, 100));
	mAnimation.setNumFrames(10);
	mAnimation.setDuration(sf::seconds(1));

	centreOrigin(mSprite);
	centreOrigin(mAnimation);

	mJumpCommand.category = static_cast<int>(Category::SceneAirLayer);
	mJumpCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		mCurrJumpHeight = mMaxJumpHeight;
	};

	std::unique_ptr<TextNode> playerNumberDisplay(new TextNode(fonts, ""));
	mPlayerNumberDisplay = playerNumberDisplay.get();
	attachChild(std::move(playerNumberDisplay));

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));
}

void Character::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mAnimation, states);
}

void Character::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Update texts and roll animation
	updateTexts();
	updateAnimation(dt);

	////Jump Functions
	checkJump(dt, commands);
	jumpAction(dt);

	Entity::updateCurrent(dt, commands);
}
void Character::setFacingRight(bool flag)
{
	mAnimation.setFacingRight(flag);
}

bool Character::isFacingRight()
{
	return mAnimation.isFacingRight();
}
unsigned int Character::getCategory() const
{
	return static_cast<int>(Category::PlayerCharacter);
}

sf::FloatRect Character::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

int	Character::getIdentifier()
{
	return mIdentifier;
}

void Character::setIdentifier(int identifier)
{
	mIdentifier = identifier;
}

void Character::remove()
{
	Entity::remove();
}

float Character::getMaxSpeed() const
{
	return Table[static_cast<int>(mType)].speed;
}

//Ryan - Modified by Jaysson {
void Character::checkJump(sf::Time dt, CommandQueue& commands)
{
	if (mJumpPressed && !mIsJumping)
	{
		commands.push(mJumpCommand);
		mJumpPressed = false;
		cannotJump();
	}
}

void Character::updateTexts()
{
	mPlayerNumberDisplay->setString("Player " + std::to_string(mIdentifier + 1));
	mPlayerNumberDisplay->setPosition(0.f, -70.f);
	mPlayerNumberDisplay->setRotation(-getRotation());
	
	//mHealthDisplay->setString(std::to_string(getHitpoints()) + " HP");
	//mHealthDisplay->setPosition(0.f, -70.f);
	//mHealthDisplay->setRotation(-getRotation());
}

void Character::beginJump()
{
	mJumpPressed = true;
}

void Character::canJump()
{
	mIsJumping = false;
}

void Character::cannotJump()
{
	mIsJumping = true;
}

bool Character::isJumping()
{
	return mIsJumping;
}

void Character::jumpAction(sf::Time dt)
{
	if (mIsJumping == true) 
	{
		sf::Vector2f velocity = getPosition();
		float vy = velocity.y - (mCurrJumpHeight *dt.asSeconds());
		setPosition(velocity.x, vy);
		mCurrJumpHeight -= (mCurrJumpHeight * dt.asSeconds());
		if (mCurrJumpHeight <= mMaxJumpHeight/2)
		{
			mCurrJumpHeight -= (mCurrJumpHeight * dt.asSeconds());
		}
	}
}

void Character::updateAnimation(sf::Time dt)
{
	if (getVelocity().x == 0.f)
	{
		if (!mAnimation.isIdle())
		{
			mAnimation.restart();
			mAnimation.setIsIdle(true);
		}
		mAnimation.update(dt);
	}
	else
	{
		if (mAnimation.isIdle())
		{
			mAnimation.restart();
			mAnimation.setIsIdle(false);
		}
		mAnimation.update(dt);
	}
}

void Character::playLocalSound(CommandQueue& commands, SoundEffectIDs effect)
{
	sf::Vector2f worldPosition = getWorldPosition();

	Command command;
	command.category = static_cast<int>(Category::SoundEffect);
	command.action = derivedAction<SoundNode>([effect, worldPosition](SoundNode& node, sf::Time)
	{
		node.playSound(effect, worldPosition);
	});
	commands.push(command);
}