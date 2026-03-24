/**
 * Game and pieces for the game.
 */
#ifndef BOARD_H
#define BOARD_H

/// Imports can be added as needed.
#include <string>
#include <unordered_map>
#include <vector>

#include "candy.h"

/// Default board dimensions
const int DEFAULT_BOARD_WIDTH = 10;
const int DEFAULT_BOARD_HEIGHT = 10;

/// Number of candy pieces of the same type needed to explode a horizontal, vertical or diagonal line
const int SHORTEST_EXPLOSION_LINE = 3;

/**
 * Square game board state with r/w access to individual cells.
 */
class Board
{
public:
    /// Initialize a board of the given dimensions.
    Board(int width = DEFAULT_BOARD_WIDTH, int height = DEFAULT_BOARD_HEIGHT);

    /// Free all resources managed by the board
    ~Board();

    /**
     * Determine whether the candy at the given coordinates should explode
     * given the current board state, i.e., is part of a (h/v/d) line of SHORTEST_EXPLOSION_LINE or more candies of the same type.
     * @param x 0-indexed, left to right, x coordinate of the cell
     * @param y 0-indexed, top to bottom, y coordinate of the cell
     * @return true if the candy at the given coordinates should explode, false otherwise.
     */
    bool shouldExplode(int x, int y) const;

    /**
     * Explode all candies that should explode (lines of candies)
     * and then let candies above drop down to fill the gaps.
     * After dropping, repeat until no more candies can explode.
     * @return a (potentially empty) vector of all exploded candies.
     */
    std::vector<Candy*> explodeAndDrop();

    /**
     * Save a serialized representation of the board to a file, which can be later loaded
     * with Board::load.
     * @param output_path path to the output file where the representation is to be saved.
     * @return true if the dump was successful, false otherwise.
     */
    bool dump(const std::string& output_path) const;

    /**
     * Load a serialized representation of the board from a file previously saved.
     * Any existing board state is discarded.
     * @param input_path path to the input file from which the representation is to be loaded.
     * @return true if the load was successful, false otherwise.
     */
    bool load(const std::string& input_path);

    /**
     * Get a reference to the candy piece at the given coordinates if there is one. 
     * 
     * @param x 0-indexed, left to right, x coordinate of the cell
     * @param y 0-indexed, top to bottom, y coordinate of the cell
     * @return a pointer to the candy at the given coordinates, 
     *   if the coordinates are valid and the cell is not empty; nullptr otherwise.
     */
    Candy* getCell(int x, int y) const;

    /**
     * Set the cell at the given coordinates to the given candy.
     * @param candy the candy to place in the cell.
     * @param x 0-indexed, left to right, x coordinate of the cell.
     * @param y 0-indexed, top to bottom, y coordinate of the cell.
     */
    void setCell(Candy* candy, int x, int y);

    /// Get the board width
    int getWidth() const;

    /// Get the board height
    int getHeight() const;

private:

    static const int MAX_SIZE = 20;
    int m_width;
    int m_height;
    Candy m_storage[MAX_SIZE][MAX_SIZE];
    Candy* m_board[MAX_SIZE][MAX_SIZE];
};

#endif
