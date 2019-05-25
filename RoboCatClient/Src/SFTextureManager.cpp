#include <RoboCatClientPCH.h>

std::unique_ptr<SFTextureManager> SFTextureManager::sInstance;

void SFTextureManager::StaticInit()
{
	sInstance.reset(new SFTextureManager());
}

SFTextureManager::SFTextureManager()
{
	CacheTexture("cat", "../Assets/cat.png");
	CacheTexture("heart", "../Assets/heart.png");
	CacheTexture("bullet", "../Assets/bullet.png");
	CacheTexture("tile", "../Assets/tile.png");
	CacheTexture("wood", "../Assets/wood.png");
	CacheTexture("stone", "../Assets/stone.png");
	CacheTexture("grass", "../Assets/grass.png");
	CacheTexture("start_screen", "../Assets/start_screen.png");
	CacheTexture("lost_screen", "../Assets/lost_screen.png");
	CacheTexture("winner_screen", "../Assets/winner_screen.png");

	// Player textures.
	CacheTexture("hitman", "../Assets/hitman.png");
	CacheTexture("hatman", "../Assets/hatman.png");
	CacheTexture("man_blue", "../Assets/man_blue.png");
	CacheTexture("man_brown", "../Assets/man_brown.png");
	CacheTexture("man_old", "../Assets/man_old.png");
	CacheTexture("robot", "../Assets/robot.png");
	CacheTexture("soldier", "../Assets/soldier.png");
	CacheTexture("woman", "../Assets/woman.png");
	CacheTexture("zombie", "../Assets/zombie.png");

	// Trashbar frames.
	CacheTexture("Trash10", "../Assets/Trashbar/health10.png");
	CacheTexture("Trash9", "../Assets/Trashbar/health9.png");
	CacheTexture("Trash8", "../Assets/Trashbar/health8.png");
	CacheTexture("Trash7", "../Assets/Trashbar/health7.png");
	CacheTexture("Trash6", "../Assets/Trashbar/health6.png");
	CacheTexture("Trash5", "../Assets/Trashbar/health5.png");
	CacheTexture("Trash4", "../Assets/Trashbar/health4.png");
	CacheTexture("Trash3", "../Assets/Trashbar/health3.png");
	CacheTexture("Trash2", "../Assets/Trashbar/health2.png");
	CacheTexture("Trash1", "../Assets/Trashbar/health1.png");
	CacheTexture("Trash0", "../Assets/Trashbar/health0.png");
	CacheTexture("armor10", "../Assets/Trashbar/armor10.png");
	CacheTexture("armor9", "../Assets/Trashbar/armor9.png");
	CacheTexture("armor8", "../Assets/Trashbar/armor8.png");
	CacheTexture("armor7", "../Assets/Trashbar/armor7.png");
	CacheTexture("armor6", "../Assets/Trashbar/armor6.png");
	CacheTexture("armor5", "../Assets/Trashbar/armor5.png");
	CacheTexture("armor4", "../Assets/Trashbar/armor4.png");
	CacheTexture("armor3", "../Assets/Trashbar/armor3.png");
	CacheTexture("armor2", "../Assets/Trashbar/armor2.png");
	CacheTexture("armor1", "../Assets/Trashbar/armor1.png");

}

SFTexturePtr SFTextureManager::GetTexture(const string & inTextureName)
{
	return mNameToTextureMap[inTextureName];
}

bool SFTextureManager::CacheTexture(string inName, const char * inFileName)
{
	SFTexturePtr newTex(new sf::Texture());
	if (!newTex->loadFromFile(inFileName))
		return false;

	mNameToTextureMap[inName] = newTex;
	return true;
}
