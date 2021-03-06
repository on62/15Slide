/* MIT License
 *
 * Copyright (c) 2017-2018 John Jekel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/


#include "Grid15/GridHelp.h"

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <algorithm>
#include <random>
#include <chrono>
#include <exception>


namespace Grid15
{
    namespace GridHelp
    {
        /** \brief Swaps the tile at the given coordinates with the no tile of a Grid
         *
         * \param tileY The y coordinate
         * \param tileX The x coordinate
         * \param grid The Grid to change
         * \throw std::invalid_argument The tile being moved is invalid or not next to the no tile
         */
        void swapTile(const std::uint8_t tileY, const std::uint8_t tileX, Grid &grid)
        {
            if (validMove(tileY, tileX, grid))
                swapTile(grid.gridArray[tileY][tileX], grid);//find tile to swap and pass grid to change
            else
                throw std::invalid_argument {"tileX or tileY invalid!"};
        }

        /** \brief Swaps the tile given with the no tile of a Grid
         *
         * \param tileNum The tile to swap
         * \param grid The Grid to change
         * \throw std::invalid_argument The tile being moved is invalid, not next to the no tile or the Grid is invalid
         */
        void swapTile(const std::uint8_t tileNum, Grid &grid)
        {
            //original location of tile
            const std::uint8_t tileY {grid.index[tileNum][0]};
            const std::uint8_t tileX {grid.index[tileNum][1]};

            if (validMove(tileY, tileX, grid))
            {
                //original location of blank tile
                std::uint8_t oldNoTileY = {grid.index[Grid::NO_TILE][0]};
                std::uint8_t oldNoTileX = {grid.index[Grid::NO_TILE][1]};

                grid.gridArray[grid.index[Grid::NO_TILE][0]][grid.index[Grid::NO_TILE][1]] = {tileNum};//moves tile

                //updates location of moved tile in index
                grid.index[tileNum][0] = {oldNoTileY};
                grid.index[tileNum][1] = {oldNoTileX};

                grid.gridArray[tileY][tileX] = {Grid::NO_TILE};//moves noTile

                //updates location of noTile in index
                grid.index[Grid::NO_TILE][0] = {tileY};
                grid.index[Grid::NO_TILE][1] = {tileX};
            }
            else
                throw std::invalid_argument {"tileNum or Grid invalid!"};//not a valid move
        }

        /** \brief Checks if the tile movement will be valid between a tile and the no tile of a Grid
         *
         * \param tileNum The tile to check
         * \param grid The Grid to use
         * \return If swapping the tile would work (true) or not (false)
         * \throw std::invalid_argument If the Grid is not valid
         */
        bool validMove(const std::uint8_t tileNum, const Grid& grid)
        {
            if (tileNum <= Grid::TILE_MAX/* && tileNum >= Grid::TILE_MIN*/)//unsigned integer is always greater than 0
                return validMove(grid.index[tileNum][0], grid.index[tileNum][1], grid);
            else
                return false;
        }

        /** \brief Checks if the tile movement will be valid between a tile and the no tile of a Grid
         *
         * \param tileY The tile's y coordinate
         * \param tileX The tile's x coordinate
         * \param grid The Grid to use
         * \return If swapping the tile would work (true) or not (false)
         * \throw std::invalid_argument If the Grid array is not valid
         */
        bool validMove(const std::uint8_t tileY, const std::uint8_t tileX, const Grid& grid)
        {
            if (validGridArray(grid.gridArray))
            {
                if (tileY > Grid::Y_MAX || tileX > Grid::X_MAX)//not out off array boundries
                    return false;//TODO should throw an exception for this
                else if (tileY == grid.index[Grid::NO_TILE][0] && tileX == grid.index[Grid::NO_TILE][1])//not no tile itself
                    return false;
                else if (tileY == grid.index[Grid::NO_TILE][0] && ((tileX == grid.index[Grid::NO_TILE][1] - 1) || (tileX == grid.index[Grid::NO_TILE][1] + 1)))//same colum, a row beside
                    return true;
                else if (tileX == grid.index[Grid::NO_TILE][1] && ((tileY == grid.index[Grid::NO_TILE][0] - 1) || (tileY == grid.index[Grid::NO_TILE][0] + 1)))//same row, a colum beside
                    return true;
                else
                    return false;
            }
            else
                throw std::invalid_argument {"Grid invalid!"};
        }

        /** \brief Checks if a Grid is won
         *
         * \param grid The Grid to use
         * \return If Grid::grid is equal to Grid::GOAL_GRID
         * \throw std::invalid_argument If the grid array is not valid
         */
        bool hasWon(const Grid& grid)
        {
            if (validGridArray(grid.gridArray))
                return std::equal(std::begin(Grid::GOAL_GRID), std::end(Grid::GOAL_GRID), std::begin(grid.gridArray));//check if grid  is equal to Grid::GOAL_GRID
            else
                throw std::invalid_argument {"Grid invalid!"};
        }

        /** \brief Checks if a grid array is valid
         *
         * \param grid The grid array to check
         * \return If the grid array is valid (true) or not (false)
         */
        bool validGridArray(const Grid::gridArray_t &grid)
        {
            std::array<std::uint8_t, 16> numCount {0};//start at 0//FIXME SET ALL TO 0 (MAYBE NEEDED)

            for (std::uint_fast32_t i {0}; i < 4; ++i)
                for (std::uint_fast32_t j {0}; j < 4; ++j)
                {
                    if (grid[i][j] > Grid::TILE_MAX)
                        return false;//if too high a number (out of bounds)
                }


            for (std::uint_fast32_t i {0}; i < 4; ++i)
                for (std::uint_fast32_t j {0}; j < 4; ++j)
                    numCount[grid[i][j]] += 1;//increment each number to see how many of each

            for (std::uint_fast32_t i {0}; i < 16; ++i)
            {
                if (!(numCount[i] == 1))
                    return false;//if exactly 1 of each number
            }

            return true;//everything is good
        }

        /** \brief Checks if an index is valid.
         *
         * \param grid The grid array to compare the index to
         * \param index The index
         * \return If the index is valid (true) or not (false)
         * \throw std::invalid_argument If the gridArray (not the index) is invalid
         */
        bool validIndex(const Grid::gridArray_t &grid, const Grid::index_t &index)
        {
            Grid tempGrid {grid};
            reIndex(tempGrid);//we can rely on this to throw std::invalid_argument

            return std::equal(std::begin(tempGrid.index), std::end(tempGrid.index), std::begin(index));
        }

        /** \brief Copies a Grid. Better than Grid::setGrid because it ensures validation and also creates a new index
         *
         * \param newGrid The new grid array to use
         * \param grid The Grid to copy to
         * \throw std::invalid_argument If the new Grid is invalid
         */
        void safeCopy(const Grid::gridArray_t &newGrid, Grid &grid)
        {
            if (validGridArray(newGrid))//fixme rely on validGridArray in index only
            {
                grid.setGridArray(newGrid);

                reIndex(grid);//also checks validGridArray; unavoidable for now but inefficient
            }
            else
                throw std::invalid_argument {"Grid invalid!"};
        }

        /** \brief Copies a Grid. Better than Grid::setGrid because it ensures validation of the grid array and the index
         *
         * \param newGrid The new grid to
         * \param grid The Grid to copy to
         * \throw std::invalid_argument If the new Grid is invalid
         */
        void safeCopy(const Grid &newGrid, Grid &grid)
        {
            if (validGrid(newGrid))
                grid.setGrid(newGrid);
            else
                throw std::invalid_argument {"Grid invalid!"};
        }

        /** \brief Checks if a Grid is valid, including grid array and index
         *
         * \param grid The Grid to check
         * \return If the Grid is valid (true) or not (false)
         */
        bool validGrid(const Grid& grid)
        {
            return validGridArray(grid.gridArray) && validIndex(grid.gridArray, grid.index);//fixme validGridArray is used in valid index already (unnnessary, but again unavoidable)
        }

        /** \brief Creates a new, random, and solvable grid array
         *
         * \return A new grid array
         */
        Grid15::Grid::gridArray_t generateRandomGridArray()
        {
            //FIXME get rid of single dimentional intermediary array
            //we have to have it because otherwise std::shuffle only shuffles rows themselves, not inside or in between
            std::array<std::uint8_t, 16> tempGrid
            {
                1,  2,  3,  4,
                5,  6,  7,  8,
                9,  10, 11, 12,
                13, 14, 15, Grid::NO_TILE
            };

            Grid::gridArray_t multiDimentional {};//a normal gridArray

            //random number generator with seed
            auto rd = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());

            do
            {
                std::shuffle(std::begin(tempGrid), std::end(tempGrid), rd);//shuffle grid randomly with rd

                //copy 2d to 3d array
                for (std::uint_fast32_t i {0}; i < 4; ++i)
                    for (std::uint_fast32_t j {0}; j < 4; ++j)
                        multiDimentional[i][j] = {tempGrid[(i * 4) + j]};

            }
            while (!solvableGrid(multiDimentional));//check if grid is solvable

            return multiDimentional;
        }

        /** \brief Creates a new, random, and solvable Grid
         *
         * \return A new Grid
         */
        Grid generateRandomGrid()
        {
            Grid newGrid {generateRandomGridArray()};//set the new grid array here

            reIndex(newGrid);//creates a new index

            return newGrid;
        }

        /** \brief Checks if a grid is solvable or not.
         *
         * This function never contained any licenceless code: it was always 15Slide source, so the false comment that was here has been removed
         *
         * \param gridArray The grid array to check
         * \return If the grid is solvable or not
         * \throw std::invalid_argument If the new Grid is invalid (index and/or grid array)
         */
        bool solvableGrid(const Grid::gridArray_t &gridArray)
        {
            //create a new grid with an index
            Grid tempGrid {gridArray};
            reIndex(tempGrid);

            return solvableGrid(tempGrid);
        }

        /** \brief Checks if a grid is solvable or not.
         *
         * This function is a rewrite of the previous, unlicenced code, which I wanted to remove because I am worried about legal problems.
         * I wrote it with information from https://www.cs.bham.ac.uk/~mdr/teaching/modules04/java2/TilesSolvability.html, from scratch to be clear.
         *
         * \param grid The Grid to check
         * \return If the Grid is solvable (index and/or grid array) or not
         * \throw std::invalid_argument If the new Grid is invalid (index and/or grid array)
         */
        bool solvableGrid(const Grid &grid)
        {
            if (validGrid(grid))
            {
                //calculate number of inversions
                std::array<std::uint8_t, 16> linearGrid {};
                std::uint_fast32_t numberOfInversions {0};

                //copy to 1 dimentional array
                for (std::uint_fast32_t i {0}; i < 4; ++i)
                    for (std::uint_fast32_t j {0}; j < 4; ++j)
                        linearGrid[(i * 4) + j] = {grid.gridArray[i][j]};

                //look for inversions by comparing pairs of tiles
                //Starts with 1st and 2nd then 1st and 3rd up to 1st and 16th, then repeats with 2nd and 3rd and so on
                for (std::uint_fast32_t i {0}; i < 15; ++i)//last number cannot have any inversions so we might as well skip it
                {
                    for (std::uint_fast32_t j {1}; j < (16 - i); ++j)//start at the tile after i until the end of the grid
                    {
                        if ((linearGrid[i] != Grid::NO_TILE) && (linearGrid[i + j] != Grid::NO_TILE))//if none of the two compared tiles are the no tile
                            if ((linearGrid[i] > linearGrid[i + j]))//if first tile is greater than other tile
                                ++numberOfInversions;
                    }
                }

                //find if inversions are even
                bool evenInversions {(numberOfInversions % 2) == 0};

                //find if no tile y coordinate is on an even row from the bottom
                //this uses [0][0] as y coordinate, onece flipped coordinate issue is fixed this should be changed to [0][1]
                //works for now
                bool noTileOnEvenRow {((4 - grid.index[0][0]) % 2) == 0};

                return evenInversions != noTileOnEvenRow;
            }
            else
                throw std::invalid_argument {"Grid invalid!"};
        }

        /** \brief Saves a Grid to disk (not the index)
         *
         * \param saveFile The save file
         * \param grid The Grid to save
         * \throw std::ios_base::failure From std::ifstream; if a file operation goes wrong
         * \throw std::invalid_argument If the Grid is not valid (ignores index)
         */
        void save(const std::string& saveFile, const Grid& grid)
        {
            if (validGridArray(grid.gridArray))
            {
                std::ofstream saveFileStream {};
                saveFileStream.exceptions(saveFileStream.failbit | saveFileStream.badbit);//to throw exceptions if something goes wrong
                saveFileStream.open(saveFile);

                std::string saveFileBuffer {};//to avoid to many write calls and therefor speed up the process

                //write grid to buffer
                for (std::uint_fast32_t i {0}; i < 4; ++i)
                    for (std::uint_fast32_t j {0}; j < 4; ++j)
                        saveFileBuffer += {std::to_string(static_cast<int> (grid.gridArray[i][j])) + " "};

                saveFileStream << saveFileBuffer;//store grid to file with one write
                saveFileStream.close();
            }
            else
                throw std::invalid_argument {"Grid invalid!"};
        }

        /** \brief Loads a Grid from disk (and recreates the index)
         *
         * \param saveFile The save file
         * \param grid The Grid to load to
         * \throw std::ios_base::failure From std::ifstream; if a file operation goes wrong
         * \throw std::invalid_argument If the newly loaded Grid is not valid
         */
        void load(const std::string& saveFile, Grid& grid)
        {
            Grid::gridArray_t newGridArray {};//new grid array to store grid

            std::ifstream saveFileStream {};
            saveFileStream.exceptions(saveFileStream.failbit | saveFileStream.badbit);//to throw exceptions if something goes wrong
            saveFileStream.open(saveFile);

            short temp {};

            for (std::uint_fast32_t i {0}; i < 4; ++i)
                for (std::uint_fast32_t j {0}; j < 4; ++j)
                {
                    //FIXME extraction right into newGrid even though uint8_t = unsigned char (extract number not character)
                    saveFileStream >> temp;
                    newGridArray[i][j] = {static_cast<std::uint8_t> (temp)};
                }

            saveFileStream.close();

            safeCopy(newGridArray, grid);//this creates a new index along the way, and throws an exception if the grid is invalid
        }

        /** \brief Reads the grid array of a Grid and updates its index
         *
         * \param grid The Grid to change
         * \throw std::invalid_argument If the grid array is not valid
         */
        void reIndex(Grid& grid)
        {
            if (validGridArray(grid.gridArray))
            {
                for (std::uint_fast32_t i {0}; i < 4; ++i)
                    for (std::uint_fast32_t j {0}; j < 4; ++j)
                    {
                        grid.index[grid.gridArray[i][j]][0] = {static_cast<std::uint8_t> (i)};//find tile's y coordinate and copy to index
                        grid.index[grid.gridArray[i][j]][1] = {static_cast<std::uint8_t> (j)};//find tile's x coordinate and copy to index
                    }
            }
            else
                throw std::invalid_argument {"Grid invalid!"};
        }
    }
}
