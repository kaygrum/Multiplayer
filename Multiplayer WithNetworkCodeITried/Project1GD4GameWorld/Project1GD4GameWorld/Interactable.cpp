#include "Interactable.hpp"
#include "ResourceHolder.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
//Ryan

namespace
{
	const std::vector<InteractableData> Table = initializeInteractableData();
}

Interactable::Interactable(Type type, const TextureHolder & textures) : Entity(255)
, mType(type)
, mSprite(textures.get(Table[static_cast<int>(type)].texture), Table[static_cast<int>(type)].textureRect)
, mInteactedWith(Table[static_cast<int>(type)].interactedWith)
{
}

unsigned int Interactable::getCategory() const
{
	return static_cast<int>(Category::Lever);
}

sf::FloatRect Interactable::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

HeldPlatform Interactable::getPlatform()
{
	return mHeldPlat;
}

void Interactable::setNewPlatform(float vx, float vy, Platform::Type type)
{
	mHeldPlat = HeldPlatform(vx, vy, type);
}



bool Interactable::hasInteractedWith()
{
	return mInteactedWith;
}

void Interactable::InteractWith()
{
	mInteactedWith = true;
}

void Interactable::drawCurrent(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}

void Interactable::updateCurrent(sf::Time dt, CommandQueue & commands)
{
}

