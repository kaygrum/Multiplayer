#pragma once
#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"
#include "Platform.hpp"

#include "SFML/Graphics/Sprite.hpp"
//Ryan

struct HeldPlatform
{
	HeldPlatform()
	{
	}
	HeldPlatform(float vx, float vy, Platform::Type type) : position(vx, vy)
	{

	}
	sf::Vector2f position;
	Platform::Type type;
};

class Interactable : public Entity
{
public:
	enum class Type { Lever, TypeCount };

public:
	Interactable(Type type, const TextureHolder& textures);
	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;
	HeldPlatform getPlatform();
	void setNewPlatform(float vx, float vy, Platform::Type type);
	bool hasInteractedWith();
	void InteractWith();

private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);

private:
	Type mType;
	sf::Sprite mSprite; 
	HeldPlatform mHeldPlat;
	bool mInteactedWith;
};