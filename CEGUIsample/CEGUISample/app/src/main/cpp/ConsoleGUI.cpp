#include "ConsoleGUI.h"
#include "CEGUI/CEGUI.h"

ConsoleGUI::ConsoleGUI()
: ConsoleWindow(0)
, showConsole(false)
{
    ConsoleWindow = CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("console.layout");

    if (ConsoleWindow)
    {
    } else {
        // Something bad happened and we didn't successfully create the window lets output the information
        CEGUI::Logger::getSingleton().logEvent("Error: Unable to load the ConsoleWindow from .layout");
    }
   setVisible(false);
}

ConsoleGUI::~ConsoleGUI()
{
}

void ConsoleGUI::OutputText(std::string inMsg)
{
    if(!showConsole || !ConsoleWindow)
        return;
	CEGUI::ListWidget *outputWindow = static_cast<CEGUI::ListWidget*>(ConsoleWindow->getChild("History"));
    outputWindow->addItem(inMsg.c_str());
    outputWindow->ensureIndexIsVisible(outputWindow->getItemAtIndex(outputWindow->getItemCount() - 1));
}

void ConsoleGUI::ClearConsole()
{
    if(!showConsole || !ConsoleWindow)
        return;
    CEGUI::ListWidget *outputWindow = static_cast<CEGUI::ListWidget*>(ConsoleWindow->getChild("History"));
    outputWindow->clearList();
}

void ConsoleGUI::setVisible(const bool visible)
{
    ConsoleWindow->setVisible(visible);
    showConsole = visible;
}