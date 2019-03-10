#pragma once
#include "Command.hpp"
#include "Clock.hpp"

#include "SFML/Window/Event.hpp"
#include <SFML/Network/TcpSocket.hpp>

#include <map>

class CommandQueue;

class Player
{
public:
	enum class Action{MoveLeftP1, MoveRightP1, JumpP1, MoveDownP1, MoveLeftP2, MoveRightP2, MoveDownP2, JumpP2, ActionCount};
	enum class MissionStatus{MissionRunning, MissionSuccessP1, MissionSuccessP2, MissionFailure};

public:
	Player(sf::TcpSocket* socket);
	void handleEvent(const sf::Event& event, CommandQueue& commands);
	void handleRealtimeInput(CommandQueue& commands);
	void handleRealtimeNetworkInput(CommandQueue& commands);
	void					handleNetworkEvent(Action action, CommandQueue& commands);
	void					handleNetworkRealtimeChange(Action action, bool actionEnabled);

	void assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key getAssignedKey(Action action) const;
	std::string getTime();
	void pauseTimer();
	void unpauseTimer();
	void resetTimer();

	void setMissionStatus(MissionStatus status);
	MissionStatus getMissionStatus() const;
	void disableAllRealtimeActions();
	bool isLocal() const;
private:
	void initializeActions();
	static bool isRealtimeAction(Action action);

private:
	bool hasLost;

	std::map<sf::Keyboard::Key, Action> mKeyBinding;

	std::map<Action, bool>		mActionProxies;
	std::map<Action, Command> mActionBinding;
	MissionStatus mCurrentMissionStatus;
	Stopwatch mTimer;
	int mIdentifier;
	sf::TcpSocket*				mSocket;
};