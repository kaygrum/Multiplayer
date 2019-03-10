#pragma once
#include "State.hpp"
#include "StateIdentifiers.hpp"
#include "ResourceIdentifiers.hpp"

#include "SFML/System/NonCopyable.hpp"
#include "SFML/System/Time.hpp"

#include <vector>
#include <functional>
#include <map>

namespace sf
{
	class Event;
	class RenderWindow;
}

class StateStack : private sf::NonCopyable
{
public:
	enum class ActionIDs{Push, Pop, Clear};

public:
	explicit StateStack(State::Context context);

	template<typename T>
	void registerState(StateIDs stateID);
	template <typename T, typename Param1>
	void registerState(StateIDs stateID, Param1 arg1);
	void update(sf::Time dt);
	void draw();
	void handleEvent(const sf::Event& event);

	void pushState(StateIDs stateID);
	void popState();
	void clearStates();

	bool isEmpty() const;

private:
	State::Ptr createState(StateIDs stateID);
	void applyPendingChanges();

private:
	struct PendingChange
	{
		explicit PendingChange(ActionIDs action, StateIDs stateID = StateIDs::None);
		ActionIDs action;
		StateIDs stateID;
	};

private:
	std::vector<State::Ptr> mStack;
	std::vector<PendingChange> mPendingList;
	State::Context mContext;
	std::map<StateIDs, std::function<State::Ptr()>> mFactories;
};

template<typename T> 
void StateStack::registerState(StateIDs stateID)
{
	mFactories[stateID] = [this]()
	{
		return State::Ptr(new T(*this, mContext));
	};
}
template <typename T, typename Param1>
void StateStack::registerState(StateIDs stateID, Param1 arg1)
{
	mFactories[stateID] = [this, arg1]()
	{
		return State::Ptr(new T(*this, mContext, arg1));
	};
}