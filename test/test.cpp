#include <filesystem>
#include "board.h"
#include "candy.h"
#include "controller.h"
#include "game.h"
#include "util.h"

static bool test_board_container() {
    Candy c(CandyType::TYPE_ORANGE);
    Board b(10, 10);
    b.setCell(&c, 0, 0);
    bool pass = (b.getCell(0, 0) == &c);
    std::cout << (pass ? "test_board_container: PASSED\n" : "test_board_container: FAILED\n");
    return pass;
}

static bool test_dump_load_board() {
    Candy c(CandyType::TYPE_ORANGE);
    Board b(10, 10);
    b.setCell(&c, 0, 0);
    
    Board b2(10, 10);
    bool pass = true;
    if (!b.dump(getDataDirPath() + "dump_board.txt")) pass = false;
    else if (!b2.load(getDataDirPath() + "dump_board.txt")) pass = false;
    else if (b2.getCell(0, 0) == nullptr || b2.getCell(0, 0)->getType() != c.getType()) pass = false;
    
    if (std::filesystem::exists(getDataDirPath() + "dump_board.txt")) {
        std::filesystem::remove(getDataDirPath() + "dump_board.txt");
    }
    std::cout << (pass ? "test_dump_load_board: PASSED\n" : "test_dump_load_board: FAILED\n");
    return pass;
}

static bool test_dump_load_game() {
    Game g;
    Controller cont;
    g.update(cont);
    bool pass = true;
    if (!g.dump(getDataDirPath() + "dump_game.txt")) pass = false;
    else {
        Game g2;
        if (!g2.load(getDataDirPath() + "dump_game.txt")) pass = false;
        else if (g != g2) pass = false;
    }
    if (std::filesystem::exists(getDataDirPath() + "dump_game.txt")) {
        std::filesystem::remove(getDataDirPath() + "dump_game.txt");
    }
    std::cout << (pass ? "test_dump_load_game: PASSED\n" : "test_dump_load_game: FAILED\n");
    return pass;
}

static bool test_constructor_empty() {
    Board b(5, 5);
    bool pass = true;
    if (b.getWidth() != 5) pass = false;
    if (b.getHeight() != 5) pass = false;
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (b.getCell(x, y) != nullptr) pass = false;
        }
    }
    std::cout << (pass ? "test_constructor_empty: PASSED\n" : "test_constructor_empty: FAILED\n");
    return pass;
}

static bool test_load_nonexistent() {
    Board b(10, 10);
    bool pass = (b.load("fichero_inventado_xyz.txt") == false);
    std::cout << (pass ? "test_load_nonexistent: PASSED\n" : "test_load_nonexistent: FAILED\n");
    return pass;
}

static bool test_no_false_positives() {
    Board b(10, 10);
    Candy c1(CandyType::TYPE_RED);
    Candy c2(CandyType::TYPE_RED);
    b.setCell(&c1, 0, 0);
    b.setCell(&c2, 1, 0);
    bool pass = (!b.shouldExplode(0, 0) && !b.shouldExplode(1, 0));
    std::cout << (pass ? "test_no_false_positives: PASSED\n" : "test_no_false_positives: FAILED\n");
    return pass;
}

static bool test_explode_horizontal() {
    Board b(10, 10);
    Candy c1(CandyType::TYPE_BLUE);
    Candy c2(CandyType::TYPE_BLUE);
    Candy c3(CandyType::TYPE_BLUE);
    b.setCell(&c1, 0, 0);
    b.setCell(&c2, 1, 0);
    b.setCell(&c3, 2, 0);
    bool pass = (b.shouldExplode(0, 0) && b.shouldExplode(1, 0) && b.shouldExplode(2, 0));
    std::cout << (pass ? "test_explode_horizontal: PASSED\n" : "test_explode_horizontal: FAILED\n");
    return pass;
}

static bool test_explode_vertical() {
    Board b(10, 10);
    Candy c1(CandyType::TYPE_GREEN);
    Candy c2(CandyType::TYPE_GREEN);
    Candy c3(CandyType::TYPE_GREEN);
    b.setCell(&c1, 2, 0);
    b.setCell(&c2, 2, 1);
    b.setCell(&c3, 2, 2);
    bool pass = (b.shouldExplode(2, 0) && b.shouldExplode(2, 1) && b.shouldExplode(2, 2));
    std::cout << (pass ? "test_explode_vertical: PASSED\n" : "test_explode_vertical: FAILED\n");
    return pass;
}

static bool test_chain_reaction() {
    Board b(5, 3);
    Candy r1(CandyType::TYPE_RED);
    Candy r2(CandyType::TYPE_RED);
    Candy r3(CandyType::TYPE_RED);
    Candy g1(CandyType::TYPE_GREEN);
    Candy g2(CandyType::TYPE_GREEN);
    Candy g3(CandyType::TYPE_GREEN);
    
    b.setCell(&r1, 0, 2);
    b.setCell(&r2, 1, 2);
    b.setCell(&r3, 2, 2);
    
    b.setCell(&g1, 2, 1);
    b.setCell(&g2, 3, 2);
    b.setCell(&g3, 4, 2);
    
    auto exploded = b.explodeAndDrop();
    bool pass = (exploded.size() == 6);
    
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (b.getCell(x, y) != nullptr) pass = false;
        }
    }
    std::cout << (pass ? "test_chain_reaction: PASSED\n" : "test_chain_reaction: FAILED\n");
    return pass;
}

bool test()
{
    bool all_passed = true;
    if (!test_board_container()) all_passed = false;
    if (!test_dump_load_board()) all_passed = false;
    if (!test_dump_load_game()) all_passed = false;
    if (!test_constructor_empty()) all_passed = false;
    if (!test_load_nonexistent()) all_passed = false;
    if (!test_no_false_positives()) all_passed = false;
    if (!test_explode_horizontal()) all_passed = false;
    if (!test_explode_vertical()) all_passed = false;
    if (!test_chain_reaction()) all_passed = false;
    return all_passed;
}
