#include "game.h"
#include <random>
#include "graphics.h"
#include "candy.h"
#include <fstream>

Game::Game() : m_frameCounter(0), m_score(0), m_gameOver(false)
{
}

Game::~Game()
{
}

void Game::update(const Controller& controller)
{
    // Implement your code here
}

void Game::render(GraphicManager& graphics)
{
    // Implement your code here
    
    // Note: the following code exhibits the main graphic library features
    // Board: border [draw rectangles] and a single piece of candy
    const int board_size = 10;
    const int board_padding = 3;
    graphics.drawRectangle(
        CANDY_IMAGE_HEIGHT * board_padding, CANDY_IMAGE_HEIGHT * board_padding,
        CANDY_IMAGE_WIDTH * board_size,
        CANDY_IMAGE_HEIGHT * board_size,
        5, 150, 150, 150);
    // Board: place a candy piece
    graphics.drawImage(Candy(CandyType::TYPE_PURPLE).getResourceName(),
        CANDY_IMAGE_WIDTH * 3,
        CANDY_IMAGE_HEIGHT * 3);
    // Title [draw images]
    graphics.drawImage("img/logo_small.png", 10, 10);
    // Score and footer [draw text]
    graphics.drawText("Movement: [Up] [Down] [Left] [Right]  --  "
                      "Buttons: [Q] [W] [E]  --  Exit [ESC]",
                      25, 700, 20, 100, 100, 100);
    graphics.drawText("Score: ", 450, 10, 70, 125, 200, 125);
}

void Game::run()
{
    const int screen_width = 750;
    const int screen_height = 750;
    const int bg_red = 255;
    const int bg_green = 255;
    const int bg_blue = 255;
    runGraphicGame(*this, screen_width, screen_height, bg_red, bg_green, bg_blue);
}

bool Game::dump(const std::string& output_path) const
{
    if (!m_board.dump(output_path)) return false;
    std::ofstream out(output_path, std::ios::app);
    if (!out) return false;
    out << m_frameCounter << " " << m_score << " " << m_gameOver << "\n";
    return true;
}

bool Game::load(const std::string& input_path)
{
    if (!m_board.load(input_path)) return false;
    
    std::ifstream in(input_path);
    if (!in) return false;
    
    int w, h;
    if (!(in >> w >> h)) return false;
    
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int dummy;
            in >> dummy;
        }
    }
    
    in >> m_frameCounter >> m_score >> m_gameOver;
    return true;
}

bool Game::operator==(const Game& other) const
{
    if (m_frameCounter != other.m_frameCounter ||
        m_score != other.m_score ||
        m_gameOver != other.m_gameOver) {
        return false;
    }
    
    if (m_board.getWidth() != other.m_board.getWidth() ||
        m_board.getHeight() != other.m_board.getHeight()) {
        return false;
    }
    
    for (int y = 0; y < m_board.getHeight(); ++y) {
        for (int x = 0; x < m_board.getWidth(); ++x) {
            Candy* c1 = m_board.getCell(x, y);
            Candy* c2 = other.m_board.getCell(x, y);
            
            if (c1 == nullptr && c2 == nullptr) continue;
            if (c1 == nullptr || c2 == nullptr) return false;
            if (c1->getType() != c2->getType()) return false;
        }
    }
    
    return true;
}