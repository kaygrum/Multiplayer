#pragma once
#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"
#include "Command.hpp"
#include "TextNode.hpp"
#include "CharacterAnimation.hpp"

#include "SFML/Graphics/Sprite.hpp"

class Character : public Entity
{
public:
	enum class Type{One, TypeCount};

public:
	Character(Type type, const TextureHolder& texture, const FontHolder& fonts, int identifier);
	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;
	virtual void remove();
	float getMaxSpeed() const;

	void playLocalSound(CommandQueue& commands, SoundEffectIDs effect);
	int	getIdentifier();
	void setIdentifier(int identifier);

	void beginJump();
	void canJump();
	void cannotJump();
	bool isJumping();
	void jumpAction(sf::Time dt);

	void setFacingRight(bool flag);
	bool isFacingRight();

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void checkJump(sf::Time dt, CommandQueue& commands);
	void updateTexts();

	void updateAnimation(sf::Time dt);

private:
	Type mType;
	sf::Sprite mSprite;
	Command mJumpCommand;
	CharacterAnimation mAnimation;
	bool mIsJumping;
	bool mJumpPressed;

	float mTravelledDistance;
	float mMaxJumpHeight;
	float mCurrJumpHeight;
	std::size_t mDirectionIndex;
	TextNode* mPlayerNumberDisplay;
	TextNode* mHealthDisplay;

	int mIdentifier;
};