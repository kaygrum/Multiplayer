/*Jaysson
	This class is modified specifically for the character animation (from the original animation.cpp)
	Animation.cpp is kept as we might need it for other animation in the future. (Or modifying animation.cpp to suit character's animation)
*/
#include "CharacterAnimation.hpp"

#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Texture.hpp"

#include <cassert>

CharacterAnimation::CharacterAnimation()
	: mSprite()
	, mFrameSize()
	, mNumFrames(0)
	, mCurrentFrame(0)
	, mDuration(sf::Time::Zero)
	, mElapsedTime(sf::Time::Zero)
	, mIsIdle(false)
	, mIsFacingRight(true)
{
}

CharacterAnimation::CharacterAnimation(const sf::Texture& texture)
	: mSprite(texture)
	, mFrameSize()
	, mNumFrames(0)
	, mCurrentFrame(0)
	, mDuration(sf::Time::Zero)
	, mElapsedTime(sf::Time::Zero)
	, mIsIdle(false)
	, mIsFacingRight(true)
{
}

void CharacterAnimation::setTexture(const sf::Texture& texture)
{
	mSprite.setTexture(texture);
}

const sf::Texture * CharacterAnimation::getTexture() const
{
	return mSprite.getTexture();
}

void CharacterAnimation::setFrameSize(sf::Vector2i frameSize)
{
	mFrameSize = frameSize;
}

sf::Vector2i CharacterAnimation::getFrameSize() const
{
	return mFrameSize;
}

void CharacterAnimation::setNumFrames(std::size_t numFrames)
{
	mNumFrames = numFrames;
}

std::size_t CharacterAnimation::getNumFrames() const
{
	return mNumFrames;
}

void CharacterAnimation::setDuration(sf::Time duration)
{
	mDuration = duration;
}

sf::Time CharacterAnimation::getDuration() const
{
	return mDuration;
}

void CharacterAnimation::setIsIdle(bool flag)
{
	mIsIdle = flag;
}

bool CharacterAnimation::isIdle() const
{
	return mIsIdle;
}

void CharacterAnimation::setFacingRight(bool flag)
{
	mIsFacingRight = flag;
}

bool CharacterAnimation::isFacingRight()
{
	return mIsFacingRight;
}


void CharacterAnimation::restart()
{
	mCurrentFrame = 0;
}

bool CharacterAnimation::isFinished() const
{
	return mCurrentFrame >= mNumFrames;
}

sf::FloatRect CharacterAnimation::getLocalBounds() const
{
	return sf::FloatRect(getOrigin(), static_cast<sf::Vector2f>(getFrameSize()));
}

sf::FloatRect CharacterAnimation::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}

/*	Jaysson - Modified
	This is modified to contains
		- Repeats the idle sprites when player is standing still (when position.x is the same)
		- Run animation is displayed when player is moving (not including jumping at the moment, as position.x remains unchanged)
*/
void CharacterAnimation::update(sf::Time dt)
{
	sf::Time timePerFrame = mDuration / static_cast<float>(mNumFrames);
	mElapsedTime += dt;

	sf::Vector2i textureBounds(mSprite.getTexture()->getSize());
	sf::IntRect textureRect = mSprite.getTextureRect();

	if (mIsIdle)
	{
		//When idle, repeats the idle sprites containing 4 frames
		mNumFrames = 4;
		timePerFrame = mDuration / static_cast<float>(mNumFrames);
		textureRect.top = 100;
		if (mCurrentFrame == 0)
		{
			textureRect = sf::IntRect(0, textureRect.top, mFrameSize.x, mFrameSize.y);
		}
		while (mElapsedTime >= timePerFrame)
		{
			if (mCurrentFrame >= mNumFrames - 1)
			{
				mCurrentFrame = 0;
				textureRect.left = 0;
			}
			else
			{
				mCurrentFrame++;
				textureRect.left += textureRect.width;
			}
			mElapsedTime -= timePerFrame;
		}
		mSprite.setTextureRect(textureRect);
	}
	else
	{
		//When moving, display 10 frames of moving sprites
		mNumFrames = 10;
		timePerFrame = mDuration / static_cast<float>(mNumFrames);
		textureRect.top = 0;
		if (mCurrentFrame == 0)
		{
			textureRect = sf::IntRect(0, textureRect.top, mFrameSize.x, mFrameSize.y);
		}
		while (mElapsedTime >= timePerFrame)
		{
			/*	When reaching the 10th(last) sprite, go back to the 7th frames instead of the starting frame(frame 0)
				This is because the first few frames has the character raising the hand when starting to run,
				which makes the animation looks weird if it goes from last frame -> first frame.
			*/
			if (mCurrentFrame >= mNumFrames - 1)
			{
				mCurrentFrame = 6;
				textureRect.left = textureRect.width * 6;
			}
			else
			{
				mCurrentFrame++;
				textureRect.left += textureRect.width;
			}
			mElapsedTime -= timePerFrame;
		}
		mSprite.setTextureRect(textureRect);
	}
	changeDirection();
}

void CharacterAnimation::changeDirection()
{
	if (mSprite.getScale().x > 0 && !mIsFacingRight)
	{
		//Rotate the sprite horizontally
		mSprite.setScale(-1.f, 1.f);
		//As the sprite coordinates is at the top left corner, rotating the sprite will cause it being reflected
		//Offset value of sprite's width is added to solve the issue
		mSprite.setPosition(mSprite.getPosition().x + mSprite.getLocalBounds().width, mSprite.getPosition().y);
	}
	else if (mSprite.getScale().x < 0 && mIsFacingRight)
	{
		//Rotate the sprite
		mSprite.setScale(1.f, 1.f);
		//Revert the offset above when changing direction again
		mSprite.setPosition(mSprite.getPosition().x - mSprite.getLocalBounds().width, mSprite.getPosition().y);
	}
}

void CharacterAnimation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(mSprite, states);
}