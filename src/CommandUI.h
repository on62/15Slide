#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include "Grid15/Grid.h"

#include <cstdint>

#include <string>

#include <unordered_map>


/** \class CommandUI
 *
 * \brief A UI for commands from the terminal for 15Slide
 *
 * \date 2017-2018
 *
 * Valid Commands
 *
 * <table>
 *      <tr>
 *           <th>Command</th>
 *           <th>Function</th>
 *      </tr>
 *      <tr>
 *          <td>help</td>
 *          <td>Displays a list of valid commands</td>
 *      </tr>
 *      <tr>
 *          <td>demo</td>
 *          <td>Walks you through the game</td>
 *      </tr>
 *      <tr>
 *          <td>newgame</td>
 *          <td>Ends the current game and starts a new one</td>
 *      </tr>
 *      <tr>
 *          <td>slide num</td>
 *          <td>Slides the tile with the number given</td>
 *      </tr>
 *      <tr>
 *          <td>print str</td>
 *          <td>Prints the current "grid" or the "goal" grid</td>
 *      </tr>
 *      <tr>
 *          <td>save str</td>
 *          <td>Saves the game to the specified file</td>
 *      </tr>
 *      <tr>
 *          <td>load str</td>
 *          <td>Loads the game from the specified file</td>
 *      </tr>
 *      <tr>
 *          <td>options</td>
 *          <td>Displays a list of valid options</td>
 *      </tr>
 *      <tr>
 *          <td>enable str</td>
 *          <td>Enables the specified option</td>
 *      </tr>
 *      <tr>
 *          <td>disable str</td>
 *          <td>Disables the specified option</td>
 *      </tr>
 *      <tr>
 *          <td>about</td>
 *          <td>Cool stuff about 15Slide</td>
 *      </tr>
 *      <tr>
 *          <td>exit</td>
 *          <td>Exit 15Slide</td>
 *      </tr>
 * </table>
 */
class CommandUI
{
public:
    /* Command Handling */
    void start(Grid15::Grid &grid);//start the ui

    void handleCommand(const std::string &inputtedLine, Grid15::Grid &grid);


    /* Grid Management */
    static void printGrid(Grid15::Grid::gridArray_t grid);
    static void printGrid(const Grid15::Grid &grid);

private:
    /* Tutorials, Settings and Information */
    static void runDemo();
    static void displayHelp();
    static void displayAbout();

    // Options (with defaults)
    static void displayOptions();
    void handleOptions(const std::string &option, bool optionSetting);
    bool autoSave   {true}; ///<Autosave to last savefile
    bool autoGrid   {true}; ///<Autoprint the grid
    bool autoExit   {false};///<Exit the game on win automatically
    bool easySlide  {true}; ///<Slide a tile without having to type "slide" first

    void handleDebug (const std::string &inputtedLine);


    /* Grid Management */
    std::string defaultSaveFile {""};///<The savefile to use for autoSaving, found during CommandUI::saveGame or CommandUI::loadGame
    void saveGame(const std::string &saveFile, const Grid15::Grid &grid);
    void loadGame(const std::string &saveFile, Grid15::Grid &grid);

    void swapTile(const std::int64_t tile, Grid15::Grid &grid);


    /* Command Handling */
    bool wantsToExit {false};///<Whether the user wants to exit or not; used internally to exit CommandUI::start
    enum class command;

    static const std::unordered_map<std::string, CommandUI::command> commandMap;
};

#endif //COMMANDHANDLER_H

