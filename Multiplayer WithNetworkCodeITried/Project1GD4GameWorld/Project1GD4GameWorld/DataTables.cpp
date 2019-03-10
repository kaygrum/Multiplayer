#include "DataTables.hpp"
#include "Character.hpp"
#include "Particle.hpp"
#include "Constants.hpp"
#include "Platform.hpp"
#include "Interactable.hpp"

using namespace std::placeholders;

/*	Jaysson
	Removed unused data related to characters, projectiles and pickups.
	Some refractoring are also being made in other classes(such as World.cpp) as these are moved
*/

std::vector<CharacterData> initializeCharacterData()
{
	//There are currently only one character type, more could be added in the future
	//Example: Character with different sprites
	std::vector<CharacterData> data(static_cast<int>(Character::Type::TypeCount));
	data[static_cast<int>(Character::Type::One)].hitpoints = PLAYERHITPOINTS;
	data[static_cast<int>(Character::Type::One)].speed = 200.f;
	data[static_cast<int>(Character::Type::One)].texture = TextureIDs::Character;
	data[static_cast<int>(Character::Type::One)].textureRect = sf::IntRect(0, 0, 75, 100);

	return data;
}

std::vector<ParticleData> initializeParticleData()
{
	std::vector<ParticleData> data(static_cast<int>(Particle::Type::ParticleCount));
	data[static_cast<int>(Particle::Type::Exhaust)].color = sf::Color(255, 255, 50);
	data[static_cast<int>(Particle::Type::Exhaust)].lifetime = sf::seconds(0.6f);

	data[static_cast<int>(Particle::Type::Smoke)].color = sf::Color(50, 50, 50);
	data[static_cast<int>(Particle::Type::Smoke)].lifetime = sf::seconds(4.0);

	return data;
}

//Ryan {
std::vector<PlatformData> initializePlatformData()
{
	 std::vector<PlatformData> data(static_cast<int>(Platform::Type::TypeCount));
	 data[static_cast<int>(Platform::Type::Stationary)].texture = TextureIDs::Platform;
	 data[static_cast<int>(Platform::Type::Stationary)].textureRect = sf::IntRect(0, 0, 492, 20);
	 data[static_cast<int>(Platform::Type::Moving)].canMove = false;

	 data[static_cast<int>(Platform::Type::Moving)].texture = TextureIDs::Platform;
	 data[static_cast<int>(Platform::Type::Moving)].textureRect = sf::IntRect(100, 0, 292,  20);
	 data[static_cast<int>(Platform::Type::Moving)].canMove = true;

	 return data;
}
std::vector<InteractableData> initializeInteractableData()
{
	std::vector<InteractableData> data(static_cast<int>(Platform::Type::TypeCount));
	data[static_cast<int>(Interactable::Type::Lever)].texture = TextureIDs::Lever;
	data[static_cast<int>(Platform::Type::Stationary)].textureRect = sf::IntRect(0, 0, 24, 24);
	data[static_cast<int>(Platform::Type::Stationary)].interactedWith = false;
	return data;
}
//}
