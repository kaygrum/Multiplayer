#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"

#include "SFML/Graphics/Sprite.hpp"

#pragma once
class Platform : public Entity
{
public:
	 enum class Type {Stationary, Moving, TypeCount};

public:
	Platform(Type type, const TextureHolder& texture);
	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void updateMovementPattern(sf::Time dt);

private:
	Type mType;
	sf::Sprite mSprite;
	float mDistanceTravelled;
	float mMove;
};