#include <gtest/gtest.h>

#include "TableRule.h"

using namespace ntcap;

TEST(TableRule, LifeVonNeumannAllCombinations) {
    // Von Neumann neighbourhood: 4 neighbors, 2 cell states → 10 entries
    // Index = cell_state * 5 + neighbor_count
    //
    // Game of Life rules:
    //   Dead  cell + 3 live neighbors        → born  (1)
    //   Live  cell + 2 or 3 live neighbors   → alive (1)
    //   Everything else                      → dead  (0)
    //
    // Table: {0, 0, 0, 1, 0,  0, 0, 1, 1, 0}

    constexpr TableRule<10> life(
        0, 0, 0, 1, 0,   // dead cell:  0..4 neighbors
        0, 0, 1, 1, 0    // live cell:  0..4 neighbors
    );

    // Dead cell (state=0) with 0 neighbors → stays dead
    EXPECT_EQ(life(0), 0);
    // Dead cell with 1 neighbor → stays dead
    EXPECT_EQ(life(1), 0);
    // Dead cell with 2 neighbors → stays dead
    EXPECT_EQ(life(2), 0);
    // Dead cell with 3 neighbors → birth
    EXPECT_EQ(life(3), 1);
    // Dead cell with 4 neighbors → stays dead
    EXPECT_EQ(life(4), 0);

    // Live cell (state=1) with 0 neighbors → dies (underpopulation)
    EXPECT_EQ(life(5), 0);
    // Live cell with 1 neighbor → dies (underpopulation)
    EXPECT_EQ(life(6), 0);
    // Live cell with 2 neighbors → survives
    EXPECT_EQ(life(7), 1);
    // Live cell with 3 neighbors → survives
    EXPECT_EQ(life(8), 1);
    // Live cell with 4 neighbors → dies (overpopulation)
    EXPECT_EQ(life(9), 0);
}

