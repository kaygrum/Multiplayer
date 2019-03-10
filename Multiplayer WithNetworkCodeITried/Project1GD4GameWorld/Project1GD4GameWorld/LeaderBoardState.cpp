	#include "LeaderBoardState.h"
#include "Utility.hpp"
#include "Player.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
//Created by Ryan 
LeaderBoardState::LeaderBoardState(StateStack & stack, Context context)
	: State(stack, context)
	, mLeaderBoardText()
	, mElapsedTime(sf::Time::Zero)
{
	sf::Font& font = context.fonts->get(FontIDs::Main);
	sf::Vector2f windowSize(context.window->getSize());

	mLeaderBoardText.setFont(font);

	//Jaysson
	Player::MissionStatus winningPlayer = context.player->getMissionStatus();

	mLeaderBoardText.setString("Player " + std::to_string(winningPlayer == Player::MissionStatus::MissionSuccessP1 ? 1 : 2) + " Wins! \n" + "Time Elapsed: " + context.player->getTime());


	mLeaderBoardText.setCharacterSize(70);
	centreOrigin(mLeaderBoardText);
	mLeaderBoardText.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);
}

void LeaderBoardState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	// Create dark, semitransparent background
	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(mLeaderBoardText);
}

bool LeaderBoardState::update(sf::Time dt)
{
	// Show state for 3 seconds, after return to menu
	mElapsedTime += dt;
	if (mElapsedTime > sf::seconds(3))
	{
		requestStackClear();
		requestStackPush(StateIDs::GameOver);
	}
	return false;
}

bool LeaderBoardState::handleEvent(const sf::Event & event)
{
	return false;
}
