#include "GameState.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "MusicPlayer.hpp"

GameState::GameState(StateStack& stack, Context context) 
	:State(stack, context)
	, mWorld(*context.window, *context.fonts, *context.sounds)
	, mPlayer(*context.player)	
{
	mPlayer.setMissionStatus(Player::MissionStatus::MissionRunning);

	//Play the mission theme
	context.player->resetTimer();
	context.music->play(MusicIDs::MissionTheme);
}

void GameState::draw()
{
	mWorld.draw();
}

bool GameState::update(sf::Time dt)
{
	mWorld.update(dt);

	if (mWorld.hasPlayerReachedEnd() != -1)
	{
		/*	Jaysson
			Currently hard-coded to check for which player reached the end. Need to be changed in the future
			At the moment hasPlayerReachedEnd() tells us the player's Identifier who reached the end.
		*/

		//If Player 1 reached the end
		if (mWorld.hasPlayerReachedEnd() == 0)
			mPlayer.setMissionStatus(Player::MissionStatus::MissionSuccessP1);
		//If Player 2 reached the end
		if (mWorld.hasPlayerReachedEnd() == 1)
			mPlayer.setMissionStatus(Player::MissionStatus::MissionSuccessP2);
		requestStackPush(StateIDs::LeaderBoard);
	}

	if (!mWorld.hasAlivePlayer())
	{
		mPlayer.setMissionStatus(Player::MissionStatus::MissionFailure);
		requestStackPush(StateIDs::GameOver);
	}

	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleRealtimeInput(commands);

	return true;
}

bool GameState::handleEvent(const sf::Event & event)
{
	//Game input handling
	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleEvent(event, commands);

	//Escape pressed, trigger pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		requestStackPush(StateIDs::Pause);
	}
	return true;
}


