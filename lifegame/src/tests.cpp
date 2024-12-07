#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "components/Grid.h"
#include "components/AliveCell.h"
#include "components/DeadCell.h"

using namespace GameOfLife;

TEST_CASE("Grid Initialization", "[Grid]") {
    Grid grid(10, 10);
    REQUIRE(grid.getWidth() == 10);
    REQUIRE(grid.getHeight() == 10);
}

TEST_CASE("Cell Toggle", "[Grid]") {
    Grid grid(5, 5);
    REQUIRE(dynamic_cast<DeadCell*>(grid.getCell(2, 2)) != nullptr);
    grid.toggleCellState(2, 2);
    REQUIRE(dynamic_cast<AliveCell*>(grid.getCell(2, 2)) != nullptr);
    grid.toggleCellState(2, 2);
    REQUIRE(dynamic_cast<DeadCell*>(grid.getCell(2, 2)) != nullptr);
}
