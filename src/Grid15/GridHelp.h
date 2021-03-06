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
#ifndef GRIDHELP_H
#define GRIDHELP_H


#include "Grid15/Grid.h"

#include <cstdint>
#include <string>
#include <array>


namespace Grid15
{
    /** \brief Helps with movement of tiles, saving/loading, and other management features for a Grid
     *
     * \bug Because of the way initilizer lists are in C++, and because this issue as noticed long into development, Y coordinates are switched with X in GridHelp.
     * \bug This effects the order of function parameters, loops (i == y, j == x ) and arrays in both this library and other 15Slide code that uses it.
     * \bug See https://github.com/JZJisawesome/15Slide/issues/22 for details and possible changes to this.
     *
     * \author John Jekel
     * \date 2017-2018
     */
    namespace GridHelp
    {
        bool validMove(const std::uint8_t tileNum, const Grid &grid);
        bool validMove(const std::uint8_t tileY, const std::uint8_t tileX, const Grid &grid);
        bool hasWon(const Grid &grid);

        void safeCopy(const Grid::gridArray_t &newGrid, Grid &grid);
        void safeCopy(const Grid &newGrid, Grid &grid);

        bool validGridArray(const Grid::gridArray_t &grid);
        bool validGrid(const Grid &grid);

        bool validIndex(const Grid::gridArray_t &grid, const Grid::index_t &index);

        void swapTile(const std::uint8_t tileY, const std::uint8_t tileX, Grid &grid);
        void swapTile(const std::uint8_t tileNum, Grid &grid);

        Grid::gridArray_t generateRandomGridArray();
        Grid generateRandomGrid();
        bool solvableGrid(const Grid &grid);
        bool solvableGrid(const Grid::gridArray_t &gridArray);

        void save(const std::string &saveFile, const Grid15::Grid &grid);
        void load(const std::string &saveFile, Grid &grid);

        void reIndex(Grid &grid);
    }
}
#endif //GRIDHELP_H
