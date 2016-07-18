#ifndef CONSOLE_GUI_H
#define CONSOLE_GUI_H

#include <string>

namespace CEGUI
{
    class Window;
}

class ConsoleGUI
{
public:
    ConsoleGUI();
    ~ConsoleGUI();
    void setVisible(const bool visible);
    bool isVisible() const { return showConsole; }
    void OutputText(std::string inMsg);
    void ClearConsole();

private:
    CEGUI::Window *ConsoleWindow;
    bool showConsole;
};

#endif