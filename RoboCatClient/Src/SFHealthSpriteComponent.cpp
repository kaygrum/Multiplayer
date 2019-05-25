#include <RoboCatClientPCH.h>

SFTrashSpriteComponent::SFTrashSpriteComponent(GameObject * inGameObject) :
	SFSpriteComponent(inGameObject)
{
}

sf::Sprite & SFTrashSpriteComponent::GetSprite()
{
	// Update the sprite based on the game object stuff.
	auto pos = mGameObject->GetLocation();
	pos.mX -= 35;
	pos.mY -= 15;

	m_sprite.setPosition(pos.mX, pos.mY);
	int Trash = mGameObject->GetAsCat()->GetTrash();
	m_sprite.setTexture(*SFTextureManager::sInstance->GetTexture(GetCorrectTexture(Trash)));

	return m_sprite;
}

std::string SFTrashSpriteComponent::GetCorrectTexture(int Trash)
{

		return "Trash" + std::to_string(Trash);
}
