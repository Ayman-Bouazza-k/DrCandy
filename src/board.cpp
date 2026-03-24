#include "board.h"
#include <iostream>
#include <fstream>
#include <algorithm>

Board::Board(int width, int height) : m_width(width), m_height(height)
{
    for (int y = 0; y < MAX_SIZE; ++y) {
        for (int x = 0; x < MAX_SIZE; ++x) {
            m_board[x][y] = nullptr;
        }
    }
}

Board::~Board()
{
    for (int y = 0; y < MAX_SIZE; ++y) {
        for (int x = 0; x < MAX_SIZE; ++x) {
            m_board[x][y] = nullptr;
        }
    }
}

Candy* Board::getCell(int x, int y) const
{
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        return m_board[x][y];
    }
    return nullptr;
}

void Board::setCell(Candy* candy, int x, int y)
{
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_board[x][y] = candy;
    }
}

int Board::getWidth() const
{
    return m_width;
}

int Board::getHeight() const
{
    return m_height;
}

bool Board::shouldExplode(int x, int y) const
{
    Candy* center = getCell(x, y);
    if (center == nullptr) return false;
    
    CandyType type = center->getType();
    
    // Check horizontal
    int count = 1;
    for (int i = 1; x - i >= 0 && getCell(x - i, y) && getCell(x - i, y)->getType() == type; ++i) count++;
    for (int i = 1; x + i < m_width && getCell(x + i, y) && getCell(x + i, y)->getType() == type; ++i) count++;
    if (count >= SHORTEST_EXPLOSION_LINE) return true;
    
    // Check vertical
    count = 1;
    for (int i = 1; y - i >= 0 && getCell(x, y - i) && getCell(x, y - i)->getType() == type; ++i) count++;
    for (int i = 1; y + i < m_height && getCell(x, y + i) && getCell(x, y + i)->getType() == type; ++i) count++;
    if (count >= SHORTEST_EXPLOSION_LINE) return true;
    
    // Check diagonal \ (top-left to bottom-right)
    count = 1;
    for (int i = 1; x - i >= 0 && y - i >= 0 && getCell(x - i, y - i) && getCell(x - i, y - i)->getType() == type; ++i) count++;
    for (int i = 1; x + i < m_width && y + i < m_height && getCell(x + i, y + i) && getCell(x + i, y + i)->getType() == type; ++i) count++;
    if (count >= SHORTEST_EXPLOSION_LINE) return true;
    
    // Check diagonal / (bottom-left to top-right)
    count = 1;
    for (int i = 1; x - i >= 0 && y + i < m_height && getCell(x - i, y + i) && getCell(x - i, y + i)->getType() == type; ++i) count++;
    for (int i = 1; x + i < m_width && y - i >= 0 && getCell(x + i, y - i) && getCell(x + i, y - i)->getType() == type; ++i) count++;
    if (count >= SHORTEST_EXPLOSION_LINE) return true;
    
    return false;
}

std::vector<Candy*> Board::explodeAndDrop()
{
    std::vector<Candy*> all_exploded;
    bool exploded_in_pass = true;
    
    while (exploded_in_pass) {
        exploded_in_pass = false;
        std::vector<std::pair<int, int>> to_explode;
        
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                if (shouldExplode(x, y)) {
                    to_explode.push_back({x, y});
                }
            }
        }
        
        if (!to_explode.empty()) {
            exploded_in_pass = true;
            
            for (auto p : to_explode) {
                Candy* c = m_board[p.first][p.second];
                if (c) {
                    all_exploded.push_back(c);
                    m_board[p.first][p.second] = nullptr;
                }
            }
            
            for (int x = 0; x < m_width; ++x) {
                int write_y = m_height - 1;
                for (int y = m_height - 1; y >= 0; --y) {
                    Candy* c = m_board[x][y];
                    if (c != nullptr) {
                        if (write_y != y) {
                            m_board[x][write_y] = c;
                            m_board[x][y] = nullptr;
                        }
                        write_y--;
                    }
                }
            }
        }
    }
    
    return all_exploded;
}

bool Board::dump(const std::string& output_path) const
{
    std::ofstream out(output_path);
    if (!out) return false;
    
    out << m_width << " " << m_height << "\n";
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            Candy* c = m_board[x][y];
            if (c) {
                out << static_cast<int>(c->getType()) << " ";
            } else {
                out << "-1 ";
            }
        }
        out << "\n";
    }
    return true;
}

bool Board::load(const std::string& input_path)
{
    std::ifstream in(input_path);
    if (!in) return false;
    
    int new_width, new_height;
    if (!(in >> new_width >> new_height)) return false;
    
    for (int y = 0; y < MAX_SIZE; ++y) {
        for (int x = 0; x < MAX_SIZE; ++x) {
            m_board[x][y] = nullptr;
        }
    }
    
    m_width = new_width;
    m_height = new_height;
    
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int type_val;
            if (in >> type_val) {
                if (type_val != -1) {
                    m_storage[x][y] = Candy(static_cast<CandyType>(type_val));
                    m_board[x][y] = &m_storage[x][y];
                } else {
                    m_board[x][y] = nullptr;
                }
            }
        }
    }
    
    return true;
}
