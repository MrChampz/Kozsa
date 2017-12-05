#pragma once

#include <CEGUI/CEGUI.h>

class GameConsoleWindow
{
public:
	GameConsoleWindow(CEGUI::GUIContext* guiContext);
	void setVisible(bool visible);
	bool isVisible();

private:
	void CreateCEGUIWindow(CEGUI::GUIContext* guiContext);
	void RegisterHandlers();
	bool Handle_TextSubmitted(const CEGUI::EventArgs& e);
	bool Handle_SendButtonPressed(const CEGUI::EventArgs& e);
	void ParseText(CEGUI::String inMsg);
	void OutputText(CEGUI::String inMsg, CEGUI::Colour colour = CEGUI::Colour(0xFFFFFFFF));

	CEGUI::Window*	mConsoleWindow;
	CEGUI::String	mNamePrefix;
	static int		mInstanceNumber;
	bool			mConsole;
};