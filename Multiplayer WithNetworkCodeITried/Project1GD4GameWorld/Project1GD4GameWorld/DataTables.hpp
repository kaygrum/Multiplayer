#pragma once
#include "ResourceIdentifiers.hpp"

#include "SFML/System/Time.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"

#include <vector>
#include <functional>

class Character;

struct Direction
{
	Direction(float angle, float distance) : angle(angle), distance(distance)
	{
	}

	float angle;
	float distance;
};

struct CharacterData
{
	int hitpoints;
	float speed;
	TextureIDs texture;
	sf::IntRect textureRect;
	std::vector<Direction> directions;
};

struct ParticleData
{
	sf::Color color;
	sf::Time lifetime;
};

struct PlatformData
{
	TextureIDs texture;
	sf::IntRect textureRect;
	bool canMove;
};

struct InteractableData
{
	TextureIDs texture;
	sf::IntRect textureRect;
	bool interactedWith;
};

std::vector<CharacterData> initializeCharacterData();
std::vector<InteractableData> initializeInteractableData();
std::vector<ParticleData> initializeParticleData();
std::vector<PlatformData> initializePlatformData();