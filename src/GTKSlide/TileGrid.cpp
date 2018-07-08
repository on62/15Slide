//Copyright 2018 John Jekel
//See https://github.com/JZJisawesome/15Slide/blob/master/LICENSE for the terms
#include "GTKSlide/TileGrid.h"

#include <gtkmm.h>

#include <iostream>
#include <utility>


#include "ProgramStuff.h"
#include "Grid15/Grid.h"
#include "Grid15/GridHelp.h"

namespace GTKSlide
{
TileGrid::TileGrid(Gtk::Window &parent, std::shared_ptr<Grid15::Grid> &newGridPtr)
{
    parentPtr = {&parent};

    gridPtr = newGridPtr;
    setupGrid(gridPtr);
}

TileGrid::~TileGrid() {}

void TileGrid::setupGrid(std::shared_ptr<Grid15::Grid> &newGridPtr)
{
    //create the first row of tiles
    for (std::uint_fast32_t j {0}; j < 4; ++j)
    {
        gridButtons[0][j].signal_clicked().connect
        (
            sigc::bind<std::pair<int,int>> (sigc::mem_fun(*this, &TileGrid::on_tile_clicked), std::make_pair(0, j))//bind the tile coordinates
        );

        add(gridButtons[0][j]);
    }

    //attach remaning tiles underneath first row
    for (std::uint_fast32_t i {1}; i < 4; ++i)
        for (std::uint_fast32_t j {0}; j < 4; ++j)
        {
            gridButtons[i][j].signal_clicked().connect
            (
                sigc::bind<std::pair<int,int>> (sigc::mem_fun(*this, &TileGrid::on_tile_clicked), std::make_pair(i, j))//bind the tile coordinates
            );

            attach_next_to(gridButtons[i][j], gridButtons[i - 1][j], Gtk::POS_BOTTOM, 1, 1);
        }

    lableTiles();
}

void TileGrid::on_tile_clicked(std::pair<int,int> &coordinates)
{
    if (gridPtr)
    {
        int &x = coordinates.first;
        int &y = coordinates.second;

        if constexpr (ProgramStuff::Build::DEBUG)
        {
            std::clog << std::boolalpha;
            std::clog << "(debug)Coordinates (" << x << ", " << y << ") was pressed, with tile number " << static_cast<int> ((*gridPtr).gridArray[x][y]) << ". Valid move: " << Grid15::GridHelp::validMove(x, y, *gridPtr) << "\n";
        }

        if (Grid15::GridHelp::validMove(x, y, *gridPtr))
        {
            if constexpr (ProgramStuff::Build::DEBUG)
                std::clog << "(debug)Swapping tile... ";

            Grid15::GridHelp::swapTile(x, y, *gridPtr);

            lableTiles();//fixme: just relable the 2 tiles instead

            if constexpr (ProgramStuff::Build::DEBUG)
                std::clog << "Won: " << Grid15::GridHelp::hasWon(*gridPtr) << "\n";

            if (Grid15::GridHelp::hasWon(*gridPtr))//maybe make seperate class
            {
                Gtk::MessageDialog wonDialog("YOU WON!!!" "\xf0\x9f\x8f\x86");//second string is a trophy
                wonDialog.set_title("YOU WON!!!");

                if constexpr (ProgramStuff::CHEAT_MODE)
                    wonDialog.set_secondary_text("BUT YOU CHEATED (CHEAT_MODE = true)");
                else
                    wonDialog.set_secondary_text("Great Work!!!");

                wonDialog.set_transient_for(*parentPtr);
                wonDialog.show_all();
                wonDialog.present();
                wonDialog.run();
            }

            if (saveOnSlide)
            {
                Grid15::GridHelp::save(saveFile, *gridPtr);

                if constexpr (ProgramStuff::Build::DEBUG)
                    std::clog << "(debug)Auto-saved the game" << "\n";
            }
        }


        if constexpr (ProgramStuff::Build::DEBUG)
        {
            std::clog << std::noboolalpha;
            std::clog.flush();//sometimes nothing prints until clog is flushed
        }

    }
}

void TileGrid::lableTiles()
{
    if (gridPtr)
    {
        for (std::uint_fast32_t i {0}; i < 4; ++i)
            for (std::uint_fast32_t j {0}; j < 4; ++j)
            {
                if ((*gridPtr).gridArray[i][j] != 0)
                    gridButtons[i][j].set_label(std::to_string((*gridPtr).gridArray[i][j]));//set the lable to the tile number
                else
                    gridButtons[i][j].set_label("◉");//special character for the no tile
            }
    }
}
}
