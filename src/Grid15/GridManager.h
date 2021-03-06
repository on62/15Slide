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
#ifndef GRIDMANAGER_H
#define GRIDMANAGER_H


#include <cstdint>
#include <string>
#include <array>


namespace Grid15
{
    /** \class GridManager
     *
     * \brief Manages storage, initialization, movement of tiles, and saving/loading of a grid. DONT USE: IT'S DECPRECATED
     *
     * \author John Jekel
     * \date 2017-2018
     *
     * \deprecated Use Grid and GridHelp instead
     */
    class [[deprecated]] GridManager
    {
        public:
            /* Constructors */
            GridManager();
            GridManager(const std::string &saveFile);
            GridManager(const std::array<std::array<std::uint8_t, 4>, 4> &newGrid);
            GridManager(const std::uint8_t newGrid[4][4]);


            /* Tile Movement */
            static constexpr std::size_t NO_TILE   {0};    ///<The number representing the lack of a tile
            static constexpr std::size_t TILE_MAX  {15};   ///<The maximum tile number
            static constexpr std::size_t TILE_MIN  {0};    ///<The mininum tile number
            static constexpr std::size_t X_MAX     {3};    ///<The maximum x coordinate
            static constexpr std::size_t X_MIN     {0};    ///<The mininum x coordinate
            static constexpr std::size_t Y_MAX     {3};    ///<The maximum y coordinate
            static constexpr std::size_t Y_MIN     {0};    ///<The mininum x coordinate

            void swapTile(const std::uint8_t tileX, const std::uint8_t tileY);//make use exceptions
            void swapTile(const std::uint8_t tileNum);//make use exceptions

            std::uint8_t getTile(const std::uint8_t tileX, const std::uint8_t tileY);
            std::uint8_t getX(const std::uint8_t tileNum);
            std::uint8_t getY(const std::uint8_t tileNum);

            bool validMove(const std::int64_t tileNum);
            bool validMove(const std::int64_t tileX, const std::int64_t tileY);
            bool hasWon();


            /* Grid Management */
            ///The grid GridManager::gameGrid must be for GridManager::hasWon to be true
            static constexpr std::uint8_t GOAL_GRID[4][4]
            {
                {1,  2,  3,  4},
                {5,  6,  7,  8},
                {9,  10, 11, 12},
                {13, 14, 15, GridManager::NO_TILE}
            };

            static bool validGrid(const std::array<std::array<std::uint8_t, 4>, 4> &grid);
            static bool validGrid(const std::uint8_t grid[4][4]);

            void setGrid(const std::array<std::array<std::uint8_t, 4>, 4> &newGrid);
            void setGrid(const std::uint8_t newGrid[4][4]);
            static std::array<std::array<std::uint8_t, 4>, 4> generateRandomGrid();

            std::array<std::array<std::uint8_t, 4>, 4> getGrid();


            /* Grid Storage */
            void save(const std::string &saveFile);
            void load(const std::string &saveFile);

        private:
            /* Grid Management */
            //the grid
            //0 is no tile, 1 to 15 are the other tiles
            std::array<std::array<std::uint8_t, 4>, 4> gameGrid {};///<The grid

            //coordinates of tiles on the grid
            //0 is no tile, 1 to 15 are the other tiles
            std::uint8_t index[16][2] {};///<The index to keep track of tile locations
            //std::array<std::array<std::uint8_t, GridManager::TILE_MAX + 1>, 2> index;//for some reason this causes a stack smash
            //eg. index[5][1] is the y coordinate of the tile 5 on the game grid

            void reIndex();
    };
}
#endif //GRIDMANAGER_H
