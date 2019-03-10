#include "Platform.hpp"
#include "ResourceHolder.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
//Ryan
#include <cmath>
#include <iostream>
using namespace std;
namespace
{
	const std::vector<PlatformData> Table = initializePlatformData();
}
Platform::Platform(Type type, const TextureHolder & textures) : Entity(255)
,mType(type)
,mSprite(textures.get(Table[static_cast<int>(type)].texture), Table[static_cast<int>(type)].textureRect)
, mDistanceTravelled(26.f)
,mMove(1.f)
{
}

unsigned int Platform::getCategory() const
{
	return static_cast<int>(Category::Platform);
}

sf::FloatRect Platform::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

void Platform::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}

void Platform::updateCurrent(sf::Time dt, CommandQueue & commands)
{
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);
}

void Platform::updateMovementPattern(sf::Time dt)
{
	//Movement pattern
	bool canMove = Table[static_cast<int>(mType)].canMove;
	if (canMove)
	{
		sf::Vector2f position = getPosition();
		// Moved long enough in current direction: Change direction
		if (mDistanceTravelled >= 50.f)
		{
			mMove = -1.5;
		}
		else if (mDistanceTravelled <= -50.f)
		{
			mMove = 1.5;
		}
		
		float vx = position.x + mMove;

		setPosition(vx, position.y);
		//cout << mDistanceTravelled << endl;
		mDistanceTravelled += mMove;
	}
}
