#pragma once
//Entity/Scene node category, used to dispatch messages

enum class Category {
	None = 0,
	SceneAirLayer = 1 << 0,
	PlayerCharacter = 1 << 1,
	ParticleSystem = 1 << 2,
	SoundEffect = 1 << 3,
	Platform = 1 << 4,
	Lever = 1 << 5,
	Network = 1 << 9,
};