#include "ProgramStuff.h"

#include "CommandUI.h"
#include "Grid15/Grid.h"
#include "Grid15/GridHelp.h"

#include "termcolor/termcolor.hpp"


#include <cstdint>

#include <iostream>
#include <cstdio>

#include <string>
#include <sstream>

#include <array>

#include <exception>


/* Command Handeling*/
///Commands
enum class CommandUI::command
{
    invalid,
    help,
    demo,
    newgame,
    slide,
    print,
    save,
    load,
    options,
    enable,
    disable,
    about,
    exit,
    debug
};

///A map from std::string to CommandUI::command
const std::unordered_map<std::string, CommandUI::command> CommandUI::commandMap
{
    {"help",    CommandUI::command::help},
    {"demo",    CommandUI::command::demo},
    {"newgame", CommandUI::command::newgame},
    {"slide",   CommandUI::command::slide},
    {"print",   CommandUI::command::print},
    {"save",    CommandUI::command::save},
    {"load",    CommandUI::command::load},
    {"options", CommandUI::command::options},
    {"enable",  CommandUI::command::enable},
    {"disable", CommandUI::command::disable},
    {"about",   CommandUI::command::about},
    {"exit",    CommandUI::command::exit},
    {"debug",   CommandUI::command::debug}
};

/** \brief The main loop for the CommandUI
 *
 * \param grid A Grid to use for the program
 */
void CommandUI::start(Grid15::Grid &grid)
{
    std::string inputedLine {};

    while(!wantsToExit)
    {
        std::cout << termcolor::reverse;
        if constexpr (ProgramStuff::OS::WINDOWS)
            std::cout << "slide>";
        else
            std::cout << "slide»";
        std::cout << termcolor::reset;

        std::getline(std::cin, inputedLine);

        handleCommand(inputedLine, grid);
    }

    wantsToExit = {false};//reset the variable
}

/** \brief Handles a command from the user and does something acordingly
 *
 * \param inputedLine The line to parse
 * \param grid The Grid to change
 */
void CommandUI::handleCommand(const std::string &inputedLine, Grid15::Grid &grid)
{
    std::stringstream argsStream{inputedLine};
    std::int64_t tile {0};

    constexpr auto invalidSyntaxError
    {
        []()
        {
            std::cerr << termcolor::bold << termcolor::red;
            std::cerr << "Sorry, but that syntax you used is not valid. ";
            std::cerr << termcolor::reset;
            std::cerr << "Try typing \"help\" for proper usage." << "\n";
        }
    };


    if (easySlide && argsStream >> tile)//if command is a tile and easySlide on
    {
        //FIXME check for other arguments and give an error if there is
        swapTile(tile, grid);
    }
    else
    {
        std::string input {};

        argsStream.clear();//reset bad and fail bits
        argsStream >> input;//get the first command

        CommandUI::command parsedCommand {CommandUI::command::invalid};

        //make sure command is valid
        try
        {
            parsedCommand = {CommandUI::commandMap.at(input)};
        }
        catch (std::out_of_range &e) {} //parsedCommand is set to invalid at initilization

        //switch for command
        switch (parsedCommand)
        {
        case CommandUI::command::help:

            CommandUI::displayHelp();
            break;

        case CommandUI::command::demo:

            CommandUI::runDemo();
            break;

        case CommandUI::command::newgame:

            if constexpr (ProgramStuff::CHEAT_MODE)
            {
                std::cout << termcolor::on_red;
                std::cout << "BUT WHY? YOU WERE SO CLOSE!" << "\n";
                std::cout << termcolor::reset;
            }

            Grid15::GridHelp::copyGrid(Grid15::GridHelp::generateRandomGrid(), grid);

            std::cout << std::endl;
            printGrid(grid);
            std::cout << std::endl;

            if constexpr (ProgramStuff::Build::DEBUG)
                std::clog << "(debug)Reseting defaultSaveFile" << "\n";

            defaultSaveFile = {""};
            break;

        case CommandUI::command::slide:

            if (argsStream >> tile)
                Grid15::GridHelp::swapTile(tile, grid);
            else
                invalidSyntaxError();
            break;

        case CommandUI::command::print:

            if (argsStream >> input)
            {
                std::cout << std::endl;

                if (input == "grid")
                    printGrid(grid);
                else if (input == "goal")
                    printGrid(Grid15::Grid::GOAL_GRID);
                else
                {
                    std::cerr << termcolor::bold << termcolor::red;
                    std::cerr << "Sorry, but \"" << input << "\" is not a valid grid. ";
                    std::cerr << termcolor::reset;
                    std::cerr << "Try \"grid\" or \"goal.\"";
                }

                std::cout << std::endl;
            }
            else
                invalidSyntaxError();
            break;

        case CommandUI::command::save:

            if (argsStream >> input)
                saveGame(input, grid);
            else
                invalidSyntaxError();
            break;

        case CommandUI::command::load:

            if (argsStream >> input)
                loadGame(input, grid);
            else
                invalidSyntaxError();
            break;

        case CommandUI::command::options:

            CommandUI::displayOptions();
            break;

        case CommandUI::command::enable:

            if (argsStream >> input)
                handleOptions(input, true);
            else
                invalidSyntaxError();
            break;

        case CommandUI::command::disable:

            if (argsStream >> input)
                handleOptions(input, false);
            else
                invalidSyntaxError();
            break;

        case CommandUI::command::about:

            CommandUI::displayAbout();
            break;

        case CommandUI::command::exit:

            wantsToExit = true;//if user wants to exit
            break;

        case CommandUI::command::debug:

            std::getline(argsStream, input);
            handleDebug(input);
            break;

        case CommandUI::command::invalid://this might be triggered
        default://this not really used, but to make compiler warnings stop

            std::cerr << termcolor::bold << termcolor::red;
            std::cerr << "Sorry, but \"" << input << "\" is not a valid command. ";
            std::cerr << termcolor::reset;
            std::cerr << "Try typing \"help\" for a list." << "\n";
        }
    }
}

/* Tutorials, Settings and Information */
/// \brief The 15Slide demo
void CommandUI::runDemo()
{
    using namespace std;

    cout << endl;
    cout << termcolor::reverse << termcolor::bold;
    cout << "15Slide" << "\n";
    cout << termcolor::reset;
    cout << endl;

    cout << "Welcome to 15Slide, a slidy-tile game. The object is to make the grid like this:" << "\n";
    cout << endl;

    printGrid(Grid15::Grid::GOAL_GRID);
    cout << endl;

    cout << "Move the tiles agacient to the ";
    cout << termcolor::on_blue << termcolor::white;
    if constexpr (ProgramStuff::USE_UTF8_TERMINAL)
        cout << "X";
    else
        cout << "◉◉◉";
    cout << termcolor::reset;
    cout << " by typing a number and pressing enter." << "\n";
    cout << "You can save or load a game by typing \"save\" or \"load\" and the file name." << "\n";
    cout << "Start over by typing \"newgame.\"" << "\n";
    cout << "To learn about more awesome commands, type \"help.\"" << "\n";
    cout << endl;

    cout << termcolor::blink;
    cout << "ENJOY 15Slide!!!";
    if constexpr (ProgramStuff::USE_UTF8_TERMINAL)
        cout << "\xF0\x9F\x99\x82";//smile
    cout << termcolor::reset;
    cout << endl;
}

/// \brief List of commands
void CommandUI::displayHelp()
{
    using namespace std;

    cout << endl;
    cout << termcolor::underline;
    cout << "Commands" << "\n";
    cout << termcolor::reset;
    cout << endl;

    cout << "help\t\tDisplays a list of valid commands" << "\n";
    cout << "demo\t\tWalks you through the game" << "\n";
    cout << endl;

    cout << "newgame\t\tEnds the current game and starts a new one" << "\n";
    cout << "slide\tnum\tSlides the tile with the number given" << "\n";
    cout << "print\tstr\tPrints the current \"grid\" or the \"goal\" grid" << "\n";
    cout << endl;

    cout << "save\tstr\tSaves the game to the specified file" << "\n";
    cout << "load\tstr\tLoads the game from the specified file" << "\n";
    cout << endl;

    cout << "options\t\tDisplays a list of valid options" << "\n";
    cout << "enable\tstr\tEnables the specified option" << "\n";
    cout << "disable\tstr\tDisables the specified option" << "\n";
    cout << endl;

    cout << "about\t\tCool stuff about 15Slide and credits" << "\n";
    cout << "exit\t\tExit 15Slide" << "\n";
    cout << endl;
}

/// \brief About 15Slide
void CommandUI::displayAbout()
{
    using namespace std;

    cout << endl;
    cout << termcolor::reverse << termcolor::bold;
    cout << "15Slide" << "\n";
    cout << termcolor::reset;
    cout << endl;

    cout << "15Slide is a fun slidy-tile game developed by JZJisawesome in 2017, written in the C++ programming language." << "\n";
    cout << endl;

    cout << "Thanks to the Termcolor project for colour and fonts: ";
    cout << "http://termcolor.readthedocs.io/" << "\n";
    cout << endl;

    cout << termcolor::underline;
    cout << "Build Info";
    cout << termcolor::reset << "\n";
    cout << endl;

    cout << termcolor::reverse << termcolor::bold;
    cout << "15Slide";
    cout << termcolor::reset;
    cout << " Version " << ProgramStuff::Build::SLIDE_VERSION << "-";
    if constexpr (ProgramStuff::Build::DEBUG)
        cout << "debug";
    else//release build
        cout << "release";
    cout << "\n";
    cout << "Built on " << ProgramStuff::Build::DATE << " at " << ProgramStuff::Build::TIME << " for " << ProgramStuff::OS::STRING << "\n";
    cout << "Compiler: " << ProgramStuff::Build::COMPILER_STRING << ", Version: " << ProgramStuff::Build::COMPILER_VERSION << "\n";
    cout << "C++ Version: " << ProgramStuff::Build::CPP_VERSION << "\n";
    if constexpr (ProgramStuff::CHEAT_MODE)
    {
        std::cout << termcolor::on_red;
        std::cout << "CHEATING BUILD" << "\n";
        std::cout << termcolor::reset;
    }
    cout << endl;
}

/// \brief List of options
void CommandUI::displayOptions()
{
    using namespace std;

    cout << endl;
    cout << termcolor::underline;
    cout << "Options" << "\n";
    cout << termcolor::reset;
    cout << endl;

    cout << "autoSave bool\tIf enabled, automatically saves the game to the last savefile. Enabled by default." << "\n";
    cout << "autoGrid bool\tIf enabled, autoprints the grid after certain commands. Enabled by default." << "\n";
    cout << "autoExit bool\tIf enabled, automatically exits the game after you win. Disabled by default." << "\n";
    cout << "easySlide bool\tAllows you to just type in a number rather than \"slide\" first. Enabled by default." << "\n";
    cout << endl;
}

/** \brief Handles an option string and sets the aproperiate option
 *
 * \param option The option string to set
 * \param optionSetting Turn it on or off
 */
void CommandUI::handleOptions(const std::string &option, bool optionSetting)
{
    if      (option == "autoSave")
        autoSave = optionSetting;
    else if (option == "autoGrid")
        autoGrid = optionSetting;
    else if (option == "autoExit")
        autoExit = optionSetting;
    else if (option == "easySlide")
        easySlide  = optionSetting;
    else
    {
        std::cerr << termcolor::bold << termcolor::red;
        std::cerr << "Sorry, but \"" << option << "\" is not a valid option. ";
        std::cerr << termcolor::reset;
        std::cerr << "Try typing \"options\" for a list." << "\n";
    }
}

/** \brief Handles the debug commands. Only avaliable if ProgramStuff::Build::DEBUG equals true
 *
 * \param inputtedLine The arguments to parse
 * \throw std::exception If the user uses the "crash" command
 */
void CommandUI::handleDebug(const std::string& inputtedLine)
{
    if constexpr (ProgramStuff::Build::DEBUG)
    {
        std::string input {};

        std::stringstream debugStream{inputtedLine};
        debugStream >> input;

        if (input == "help")
        {
            using namespace std;

            cout << endl;
            cout << termcolor::underline;
            cout << "(debug)Debug Commands" << "\n";
            cout << termcolor::reset;
            cout << endl;

            cout << "debug help\tDisplays a list of valid debugging commands" << "\n";
            cout << endl;

            cout << "debug crash\tThrows an instance of std::exception, terminating 15Slide" << "\n";
            cout << endl;
        }
        else if (input == "crash")
        {
            std::cout << "(debug)Throwing an instance of std::runtime_error" << "\n";
            throw std::runtime_error {"User induced crash by command \"debug crash\""};
        }
        else
        {
            using namespace std;

            cerr << termcolor::bold << termcolor::red;
            cerr << "(debug)Sorry, but \"" << input << "\" is not a valid debug command. ";
            cerr << termcolor::reset;
            cerr << "Try typing \"debug help\" for a list." << "\n";
        }
    }
    else//release build
        std::cout << "Sorry, but \"debug\" is not avaliable in release builds. See \"about.\"" << "\n";
}

/* Grid Management */
/** \brief Prints a grid array
 *
 * \param grid The grid array to print
 */
void CommandUI::printGrid(const Grid15::Grid::gridArray_t gridArray)
{
    //we use printf here for speed, except for colour which uses ostream :(

    if constexpr (ProgramStuff::USE_UTF8_TERMINAL)
    {
        std::printf("┏━━━┳━━━┳━━━┳━━━┓\n");

        for (std::uint_fast32_t i {0}; i < 4; ++i)
        {
            std::printf("┃");
            for (std::uint_fast32_t j {0}; j < 4; ++j)
            {
                if (gridArray[i][j] == Grid15::Grid::NO_TILE)
                {
                    //no tile is represented by ◉◉◉
                    std::cout << termcolor::on_blue << termcolor::white;
                    std::printf("◉◉◉");
                    std::cout << termcolor::reset;
                }
                else
                {
                    if (gridArray[i][j] <= 9)//extra space for small numbers
                        std::printf(" ");

                    std::printf(" %d", static_cast<int> (gridArray[i][j]));
                }

                std::printf("┃");
            }

            std::printf("\n");

            if ((i <= 2))//all except last line
                std::printf("┣━━━╋━━━╋━━━╋━━━┫\n");//grid and its new line
        }

        std::printf("┗━━━┻━━━┻━━━┻━━━┛\n");
    }
    else
    {
        for (std::uint_fast32_t i {0}; i < 4; ++i)
        {
            for (std::uint_fast32_t j {0}; j < 4; ++j)
            {
                if (gridArray[i][j] == Grid15::Grid::NO_TILE)
                {
                    //no tile is represented by an X
                    std::cout << termcolor::on_blue << termcolor::white;
                    std::printf("X");
                    std::cout << termcolor::reset;
                }
                else
                    std::printf("%d", static_cast<int> (gridArray[i][j]));

                if (gridArray[i][j] <= 9)
                    std::printf("   ");
                else
                    std::printf("  ");
            }
            std::printf("\n");
        }
    }
}

/** \brief Prints a Grid
 *
 * \param grid The Grid to print
 */
void CommandUI::printGrid(const Grid15::Grid &grid)
{
    printGrid(grid.gridArray);
}

/** \brief Saves the game
 *
 * \param saveFile The file to save to
 * \param grid The Grid to save
 */
void CommandUI::saveGame(const std::string &saveFile, const Grid15::Grid &grid)
{
    std::cout << "Saving game to " + saveFile + "..." << std::endl;

    try
    {
        Grid15::GridHelp::save(saveFile, grid);

        std::cout << termcolor::green;
        std::cout << "Save Complete!";
        std::cout << termcolor::reset << std::endl;

        if constexpr (ProgramStuff::Build::DEBUG)
            std::clog << "(debug)Set defaultSaveFile to " << saveFile << "\n";

        defaultSaveFile = {saveFile};
    }
    catch (std::ios_base::failure &e)
    {
        std::cerr << termcolor::bold << termcolor::red;
        std::cerr << "Something went wrong while writing. ";
        std::cerr << termcolor::reset;
        std::cerr << "Try a diffrent file name/location, or change permissions to allow writing.";
        std::cerr << std::endl;
    }
}

/** \brief Loads the game
 *
 * \param saveFile The file to load from
 * \param grid The Grid to load to
 */
void CommandUI::loadGame(const std::string &saveFile, Grid15::Grid &grid)
{
    std::cout << "Loading game from " + saveFile + "..." << std::endl;

    try
    {
        Grid15::GridHelp::load(saveFile, grid);

        std::cout << termcolor::green;
        std::cout << "Load Sucessfull!";
        std::cout << termcolor::reset << std::endl;

        if constexpr (ProgramStuff::Build::DEBUG)
            std::clog << "(debug)Set defaultSaveFile to " << saveFile << "\n";

        defaultSaveFile = {saveFile};
    }
    catch (std::ios_base::failure &e)
    {
        std::cerr << termcolor::bold << termcolor::red;
        std::cerr << "Something went wrong while reading. ";
        std::cerr << termcolor::reset;

        std::cerr << "Try a diffrent file name/location.";
        std::cerr << std::endl;
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << termcolor::bold << termcolor::red;
        std::cerr << "Something went wrong while importing the grid. ";
        std::cerr << termcolor::reset;

        std::cerr << "The file was read sucessfully, but the grid may be corrupted.";
        std::cerr << std::endl;
    }
}

/** \brief Swaps a tile
 *
 * \param tile The tile to swap
 * \param grid The Grid to use
 */
void CommandUI::swapTile(const std::int64_t tile, Grid15::Grid &grid)
{
    if (Grid15::GridHelp::validMove(tile, grid))//instead of a try catch block
    {
        Grid15::GridHelp::swapTile(tile, grid);

        if (Grid15::GridHelp::hasWon(grid) && autoExit)
            wantsToExit = true;

        if (autoSave && (defaultSaveFile != ""))
        {
            try
            {
                Grid15::GridHelp::save(defaultSaveFile, grid);//silent save (dosent use saveGame)

                if constexpr (ProgramStuff::Build::DEBUG)
                    std::clog << "(debug)Autosaved the game" << "\n";
            }
            catch (std::ios_base::failure &e)
            {
                std::cerr << "Warning: The autosave failed. Try saving to a new location using \"save,\" or change permissions." << "\n";
            }
        }
    }
    else
    {
        std::cout << termcolor::bold << termcolor::red;
        std::cout << "Sorry, but \"" << tile << "\" is not a valid tile. ";
        std::cout << termcolor::reset;
        std::cout << "Please try again." << "\n";
    }

    if (autoGrid)
    {
        std::cout << std::endl;
        printGrid(grid);
        std::cout << std::endl;
    }

    if (Grid15::GridHelp::hasWon(grid))
    {
        std::cout << std::endl;
        std::cout << termcolor::green << termcolor::bold << termcolor::blink;
        std::cout << "YOU WON!!!";
        if constexpr (!ProgramStuff::OS::WINDOWS)
            std::cout << "\xf0\x9f\x8f\x86";//trophy
        std::cout << "\n";
        std::cout << termcolor::reset;
        std::cout << std::endl;

        if constexpr (ProgramStuff::CHEAT_MODE)
        {
            std::cout << termcolor::on_red;
            std::cout << "BUT YOU CHEATED (CHEAT_MODE = true)" << "\n";
            std::cout << termcolor::reset;
        }
    }
}