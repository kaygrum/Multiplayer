#include "SettingsState.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, mGUIContainer()
{
	mBackgroundSprite.setTexture(context.textures->get(TextureIDs::TitleScreen));

	/*	Jaysson
		Added another set of 4 buttons for second player in the setting
	*/
	// Build key binding buttons and labels
	addButtonLabel(Player::Action::MoveLeftP1, 150.f, "Move Left P1", context);
	addButtonLabel(Player::Action::MoveRightP1, 200.f, "Move Right P1", context);
	addButtonLabel(Player::Action::JumpP1, 250.f, "Move Up P1", context);
	addButtonLabel(Player::Action::MoveDownP1, 300.f, "Move Down P1", context);

	addButtonLabel(Player::Action::MoveLeftP2, 400.f, "Move Left P2", context);
	addButtonLabel(Player::Action::MoveRightP2, 450.f, "Move Right P2", context);
	addButtonLabel(Player::Action::JumpP2, 500.f, "Move Up P2", context);
	addButtonLabel(Player::Action::MoveDownP2, 550.f, "Move Down P2", context);

	updateLabels();

	auto backButton = std::make_shared<GUI::Button>(context);
	backButton->setPosition(80.f, 650.f);
	backButton->setText("Back");
	backButton->setCallback(std::bind(&SettingsState::requestStackPop, this));

	mGUIContainer.pack(backButton);
}

void SettingsState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool SettingsState::update(sf::Time)
{
	return true;
}

bool SettingsState::handleEvent(const sf::Event& event)
{
	bool isKeyBinding = false;

	// Iterate through all key binding buttons to see if they are being pressed, waiting for the user to enter a key
	for (std::size_t action = 0; action < static_cast<int>(Player::Action::ActionCount); ++action)
	{
		if (mBindingButtons[action]->isActive())
		{
			isKeyBinding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				getContext().player->assignKey(static_cast<Player::Action>(action), event.key.code);
				mBindingButtons[action]->deactivate();
			}
			break;
		}
	}

	// If pressed button changed key bindings, update labels; otherwise consider other buttons in container
	if (isKeyBinding)
		updateLabels();
	else
		mGUIContainer.handleEvent(event);

	return false;
}

void SettingsState::updateLabels()
{
	Player& player = *getContext().player;

	for (std::size_t i = 0; i < static_cast<int>(Player::Action::ActionCount); ++i)
	{
		sf::Keyboard::Key key = player.getAssignedKey(static_cast<Player::Action>(i));
		mBindingLabels[i]->setText(toString(key));
	}
}

void SettingsState::addButtonLabel(Player::Action action, float y, const std::string& text, Context context)
{
	mBindingButtons[static_cast<int>(action)] = std::make_shared<GUI::Button>(context);
	mBindingButtons[static_cast<int>(action)]->setPosition(80.f, y);
	mBindingButtons[static_cast<int>(action)]->setText(text);
	mBindingButtons[static_cast<int>(action)]->setToggle(true);

	mBindingLabels[static_cast<int>(action)] = std::make_shared<GUI::Label>("", *context.fonts);
	mBindingLabels[static_cast<int>(action)]->setPosition(300.f, y + 15.f);

	mGUIContainer.pack(mBindingButtons[static_cast<int>(action)]);
	mGUIContainer.pack(mBindingLabels[static_cast<int>(action)]);
}
