#include "GameConsoleWindow.h"

int GameConsoleWindow::mInstanceNumber;

GameConsoleWindow::GameConsoleWindow(CEGUI::GUIContext* guiContext)
{
	mConsoleWindow = NULL;
	mInstanceNumber = 0;
	mNamePrefix = "";
	CreateCEGUIWindow(guiContext);
	setVisible(true);
	mConsole = false;
}

void GameConsoleWindow::setVisible(bool visible)
{
	mConsoleWindow->setVisible(visible);
	mConsole = visible;

	CEGUI::Editbox* editBox = (CEGUI::Editbox*)mConsoleWindow->getChild(mNamePrefix + "Editbox");
	if (visible)
		editBox->activate();
	else
		editBox->deactivate();
}

bool GameConsoleWindow::isVisible()
{
	return mConsoleWindow->isVisible();
}

void GameConsoleWindow::CreateCEGUIWindow(CEGUI::GUIContext* guiContext)
{
	CEGUI::WindowManager* windowManager = CEGUI::WindowManager::getSingletonPtr();

	mNamePrefix = ++mInstanceNumber + "_";

	mConsoleWindow = windowManager->loadLayoutFromFile("ConsoleWindow.layout");

	if (mConsoleWindow)
	{
		guiContext->getRootWindow()->addChild(mConsoleWindow);
		(this)->RegisterHandlers();
	}
	else
	{
		CEGUI::Logger::getSingleton().logEvent("Error: Unable to load the ConsoleWindow from .layout!");
	}
}

void GameConsoleWindow::RegisterHandlers()
{
	mConsoleWindow->getChild(mNamePrefix + "Submit")->subscribeEvent(
		CEGUI::PushButton::EventClicked,
		CEGUI::Event::Subscriber(&GameConsoleWindow::Handle_SendButtonPressed, this));

	mConsoleWindow->getChild(mNamePrefix + "Editbox")-> subscribeEvent(
		CEGUI::Editbox::EventTextAccepted,
		CEGUI::Event::Subscriber(&GameConsoleWindow::Handle_TextSubmitted, this));
}

bool GameConsoleWindow::Handle_TextSubmitted(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs* args = static_cast<const CEGUI::WindowEventArgs*>(&e);

	CEGUI::String msg = mConsoleWindow->getChild(mNamePrefix + "Editbox")->getText();
	(this)->ParseText(msg);
	mConsoleWindow->getChild(mNamePrefix + "Editbox")->setText("");

	return true;
}

bool GameConsoleWindow::Handle_SendButtonPressed(const CEGUI::EventArgs& e)
{
	CEGUI::String msg = mConsoleWindow->getChild(mNamePrefix + "Editbox")->getText();
	(this)->ParseText(msg);
	mConsoleWindow->getChild(mNamePrefix + "Editbox")->setText("");

	return true;
}

void GameConsoleWindow::ParseText(CEGUI::String inMsg)
{
	std::string inString = inMsg.c_str();

	if (inString.length() >= 1)
	{
		if (inString.at(0) == '@')
		{
			std::string::size_type commandEnd = inString.find(" ", 1);
			std::string command = inString.substr(1, commandEnd - 1);
			std::string commandArgs = inString.substr(commandEnd + 1, inString.length() - (commandEnd + 1));

			for (std::string::size_type i = 0; i < command.length(); i++)
			{
				command[i] = tolower(command[i]);
			}

			if (command == "say")
			{
				std::string outString = "You: " + commandArgs;
				(this)->OutputText(outString);
			}
			else if (command == "quit")
			{

			}
			else
			{
				std::string outString = "<" + inString + "> is an invalid command.";
				(this)->OutputText(outString, CEGUI::Colour(1.0f, 0.0f, 0.0f));
			}
		}
		else
		{
			(this)->OutputText(inString);
		}
	}
}

void GameConsoleWindow::OutputText(CEGUI::String inMsg, CEGUI::Colour colour)
{
	CEGUI::Listbox* outputWindow = static_cast<CEGUI::Listbox*>(mConsoleWindow->getChild(mNamePrefix + "History"));

	CEGUI::ListboxTextItem* newItem = 0;

	newItem = new CEGUI::ListboxTextItem(inMsg);
	newItem->setTextColours(colour);
	outputWindow->addItem(newItem);
}
