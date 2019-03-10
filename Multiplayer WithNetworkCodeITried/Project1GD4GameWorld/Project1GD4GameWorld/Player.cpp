#include "Player.hpp"
#include "CommandQueue.hpp"
#include "Character.hpp"
#include "Foreach.hpp"

#include "NetworkProtocol.hpp"

#include <SFML/Network/Packet.hpp>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std::placeholders;

struct CharacterMover
{
	CharacterMover(float vx, float vy, int identifier) : velocity(vx, vy), playerID(identifier)
	{

	}
	void operator() (Character& character, sf::Time) const
	{
		if (character.getIdentifier() == playerID)
		{
			character.accelerate(velocity * character.getMaxSpeed());
			if ((velocity.x > 0) && !character.isFacingRight())
			{
				character.setFacingRight(true);
				//std::cout << velocity.x << std::endl;
			}
			else if ((velocity.x < 0) && character.isFacingRight())
			{
				character.setFacingRight(false);
				//std::cout << velocity.x  << std::endl;
			}
		}
	}
	sf::Vector2f velocity;
	int playerID;
};

struct CharacterJumpTrigger
{
	CharacterJumpTrigger(int identifier)
		: playerID(identifier)
	{
	}

	void operator() (Character& character, sf::Time dt) const
	{
		if (character.getIdentifier() == playerID)
		{
			character.beginJump();
		}
	}

	int playerID;
};

Player::Player(sf::TcpSocket* socket)
	: mCurrentMissionStatus(MissionStatus::MissionRunning)
	, hasLost(true)
	, mSocket(socket)
{
	/*	Jaysson
		Two player's keybind are added
		Need to be changed in the future for networking
	*/
	//Set initial key bindings
	mKeyBinding[sf::Keyboard::Left] = Action::MoveLeftP1;
	mKeyBinding[sf::Keyboard::Right] = Action::MoveRightP1;
	mKeyBinding[sf::Keyboard::Up] = Action::JumpP1;
	mKeyBinding[sf::Keyboard::Down] = Action::MoveDownP1;

	mKeyBinding[sf::Keyboard::A] = Action::MoveLeftP2;
	mKeyBinding[sf::Keyboard::D] = Action::MoveRightP2;
	mKeyBinding[sf::Keyboard::W] = Action::JumpP2;
	mKeyBinding[sf::Keyboard::S] = Action::MoveDownP2;

	mTimer.Start(true);
	initializeActions();

	//set initial action bindings
	//Assign all categories to the player's Character
	for (auto& pair : mActionBinding)
	{
		pair.second.category = static_cast<unsigned int>(Category::PlayerCharacter);
	}
}

bool Player::isLocal() const
{
	// No key binding means this player is remote
	//return mKeyBinding != nullptr;
	return true;
}
void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		//check if key pressed is in the key bindings, if so trigger command
		auto found = mKeyBinding.find(event.key.code);
		if (found != mKeyBinding.end() && !isRealtimeAction(found->second))
		{
			commands.push(mActionBinding[found->second]);
		}
	}
}
void Player::disableAllRealtimeActions()
{
	FOREACH(auto& action, mActionProxies)
	{
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Client::PlayerRealtimeChange);
		packet << mIdentifier;
		packet << static_cast<sf::Int32>(action.first);
		packet << false;
		mSocket->send(packet);
	}
}
void Player::handleRealtimeInput(CommandQueue& commands)
{
	//Check if any key binding keys are pressed
	for (auto pair : mKeyBinding)
	{
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
		{
			commands.push(mActionBinding[pair.second]);
		}
	}
}

void Player::assignKey(Action action, sf::Keyboard::Key key)
{
	//Remove all keys that are already mapped to an action
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end();)
	{
		if (itr->second == action)
		{
			mKeyBinding.erase(itr++);
		}
		else
		{
			++itr;
		}
		//insert new binding
		mKeyBinding[key] = action;
	}
}

sf::Keyboard::Key Player::getAssignedKey(Action action) const
{
	for (auto pair : mKeyBinding)
	{
		if (pair.second == action)
		{
			return pair.first;
		}
	}
	return sf::Keyboard::Unknown;
}

std::string Player::getTime()
{
	return mTimer.getTimeMins();
}

void Player::pauseTimer()
{
	mTimer.Stop();
}

void Player::unpauseTimer()
{
	mTimer.Start();
}

void Player::resetTimer()
{
	mTimer.Start(true);
}
void Player::handleRealtimeNetworkInput(CommandQueue& commands)
{
	if (mSocket && !isLocal())
	{
		// Traverse all realtime input proxies. Because this is a networked game, the input isn't handled directly
		FOREACH(auto pair, mActionProxies)
		{
			if (pair.second && isRealtimeAction(pair.first))
				commands.push(mActionBinding[pair.first]);
		}
	}
}

void Player::handleNetworkEvent(Action action, CommandQueue& commands)
{
	commands.push(mActionBinding[action]);
}

void Player::handleNetworkRealtimeChange(Action action, bool actionEnabled)
{
	mActionProxies[action] = actionEnabled;
}
void Player::setMissionStatus(MissionStatus status)
{
	mCurrentMissionStatus = status;
}

Player::MissionStatus Player::getMissionStatus() const
{
	return mCurrentMissionStatus;
}

void Player::initializeActions()
{
	/*	Jaysson
		Character mover is modified to take in player's identifier
		This is to tell the mover which character to move when a key is pressed
	*/
	//Player 1 with identifier 0
	mActionBinding[Action::MoveLeftP1].action = derivedAction<Character>(CharacterMover(-2, 0.f, 0));
	mActionBinding[Action::MoveRightP1].action = derivedAction<Character>(CharacterMover(2, 0.f, 0));
	mActionBinding[Action::JumpP1].action = derivedAction<Character>(CharacterJumpTrigger(0));
	mActionBinding[Action::MoveDownP1].action = derivedAction<Character>(CharacterMover(0.f, 1, 0));
	 
	//Player 2 with identifier 1
	mActionBinding[Action::MoveLeftP2].action = derivedAction<Character>(CharacterMover(-2, 0.f, 1));
	mActionBinding[Action::MoveRightP2].action = derivedAction<Character>(CharacterMover(2, 0.f, 1));
	mActionBinding[Action::JumpP2].action = derivedAction<Character>(CharacterJumpTrigger(1));
	mActionBinding[Action::MoveDownP2].action = derivedAction<Character>(CharacterMover(0.f, 1, 1));
}

bool Player::isRealtimeAction(Action action)
{
	switch (action)
	{
	case Action::MoveLeftP1:
	case Action::MoveRightP1:
	case Action::JumpP1:
	case Action::MoveDownP1:
	case Action::MoveLeftP2:
	case Action::MoveRightP2:
	case Action::JumpP2:
	case Action::MoveDownP2:
		return true;
	default:
		return false;
	}
}