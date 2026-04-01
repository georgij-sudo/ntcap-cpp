#include <gtest/gtest.h>
#include "MooreN.h"

using namespace ntcap;

// ============================================================
// 1.1 Значения по умолчанию
// ============================================================
TEST(MooreNBuilder, Build_NoSettersCalled_AllFieldsZero)
{
    auto n = MooreNBuilder{}.build();
    EXPECT_EQ(n.center(),    0);
    EXPECT_EQ(n.north(),     0);
    EXPECT_EQ(n.northEast(), 0);
    EXPECT_EQ(n.east(),      0);
    EXPECT_EQ(n.southEast(), 0);
    EXPECT_EQ(n.south(),     0);
    EXPECT_EQ(n.southWest(), 0);
    EXPECT_EQ(n.west(),      0);
    EXPECT_EQ(n.northWest(), 0);
}

// ============================================================
// 1.2 Установка отдельных полей
// ============================================================
TEST(MooreNBuilder, Center_SetValue_ReturnsSameValue)
{
    auto n = MooreNBuilder{}.center(42).build();
    EXPECT_EQ(n.center(),    42);
    EXPECT_EQ(n.north(),      0);
    EXPECT_EQ(n.northEast(),  0);
    EXPECT_EQ(n.east(),       0);
    EXPECT_EQ(n.southEast(),  0);
    EXPECT_EQ(n.south(),      0);
    EXPECT_EQ(n.southWest(),  0);
    EXPECT_EQ(n.west(),       0);
    EXPECT_EQ(n.northWest(),  0);
}

TEST(MooreNBuilder, North_SetValue_ReturnsSameValue)
{
    auto n = MooreNBuilder{}.north(10).build();
    EXPECT_EQ(n.center(),     0);
    EXPECT_EQ(n.north(),     10);
    EXPECT_EQ(n.northEast(),  0);
    EXPECT_EQ(n.east(),       0);
    EXPECT_EQ(n.southEast(),  0);
    EXPECT_EQ(n.south(),      0);
    EXPECT_EQ(n.southWest(),  0);
    EXPECT_EQ(n.west(),       0);
    EXPECT_EQ(n.northWest(),  0);
}

TEST(MooreNBuilder, NorthEast_SetValue_ReturnsSameValue)
{
    auto n = MooreNBuilder{}.northEast(15).build();
    EXPECT_EQ(n.center(),     0);
    EXPECT_EQ(n.north(),      0);
    EXPECT_EQ(n.northEast(), 15);
    EXPECT_EQ(n.east(),       0);
    EXPECT_EQ(n.southEast(),  0);
    EXPECT_EQ(n.south(),      0);
    EXPECT_EQ(n.southWest(),  0);
    EXPECT_EQ(n.west(),       0);
    EXPECT_EQ(n.northWest(),  0);
}

TEST(MooreNBuilder, East_SetValue_ReturnsSameValue)
{
    auto n = MooreNBuilder{}.east(20).build();
    EXPECT_EQ(n.center(),     0);
    EXPECT_EQ(n.north(),      0);
    EXPECT_EQ(n.northEast(),  0);
    EXPECT_EQ(n.east(),      20);
    EXPECT_EQ(n.southEast(),  0);
    EXPECT_EQ(n.south(),      0);
    EXPECT_EQ(n.southWest(),  0);
    EXPECT_EQ(n.west(),       0);
    EXPECT_EQ(n.northWest(),  0);
}

TEST(MooreNBuilder, SouthEast_SetValue_ReturnsSameValue)
{
    auto n = MooreNBuilder{}.southEast(25).build();
    EXPECT_EQ(n.center(),     0);
    EXPECT_EQ(n.north(),      0);
    EXPECT_EQ(n.northEast(),  0);
    EXPECT_EQ(n.east(),       0);
    EXPECT_EQ(n.southEast(), 25);
    EXPECT_EQ(n.south(),      0);
    EXPECT_EQ(n.southWest(),  0);
    EXPECT_EQ(n.west(),       0);
    EXPECT_EQ(n.northWest(),  0);
}

TEST(MooreNBuilder, South_SetValue_ReturnsSameValue)
{
    auto n = MooreNBuilder{}.south(30).build();
    EXPECT_EQ(n.center(),     0);
    EXPECT_EQ(n.north(),      0);
    EXPECT_EQ(n.northEast(),  0);
    EXPECT_EQ(n.east(),       0);
    EXPECT_EQ(n.southEast(),  0);
    EXPECT_EQ(n.south(),     30);
    EXPECT_EQ(n.southWest(),  0);
    EXPECT_EQ(n.west(),       0);
    EXPECT_EQ(n.northWest(),  0);
}

TEST(MooreNBuilder, SouthWest_SetValue_ReturnsSameValue)
{
    auto n = MooreNBuilder{}.southWest(35).build();
    EXPECT_EQ(n.center(),     0);
    EXPECT_EQ(n.north(),      0);
    EXPECT_EQ(n.northEast(),  0);
    EXPECT_EQ(n.east(),       0);
    EXPECT_EQ(n.southEast(),  0);
    EXPECT_EQ(n.south(),      0);
    EXPECT_EQ(n.southWest(), 35);
    EXPECT_EQ(n.west(),       0);
    EXPECT_EQ(n.northWest(),  0);
}

TEST(MooreNBuilder, West_SetValue_ReturnsSameValue)
{
    auto n = MooreNBuilder{}.west(40).build();
    EXPECT_EQ(n.center(),     0);
    EXPECT_EQ(n.north(),      0);
    EXPECT_EQ(n.northEast(),  0);
    EXPECT_EQ(n.east(),       0);
    EXPECT_EQ(n.southEast(),  0);
    EXPECT_EQ(n.south(),      0);
    EXPECT_EQ(n.southWest(),  0);
    EXPECT_EQ(n.west(),      40);
    EXPECT_EQ(n.northWest(),  0);
}

TEST(MooreNBuilder, NorthWest_SetValue_ReturnsSameValue)
{
    auto n = MooreNBuilder{}.northWest(45).build();
    EXPECT_EQ(n.center(),     0);
    EXPECT_EQ(n.north(),      0);
    EXPECT_EQ(n.northEast(),  0);
    EXPECT_EQ(n.east(),       0);
    EXPECT_EQ(n.southEast(),  0);
    EXPECT_EQ(n.south(),      0);
    EXPECT_EQ(n.southWest(),  0);
    EXPECT_EQ(n.west(),       0);
    EXPECT_EQ(n.northWest(), 45);
}

// ============================================================
// 1.3 Установка всех полей
// ============================================================
TEST(MooreNBuilder, Build_AllFieldsSet_AllReturnCorrectValues)
{
    MooreNBuilder b;
    b.center(11);
    b.north(22);
    b.northEast(33);
    b.east(44);
    b.southEast(55);
    b.south(66);
    b.southWest(77);
    b.west(88);
    b.northWest(99);
    auto n = b.build();

    EXPECT_EQ(n.center(),    11);
    EXPECT_EQ(n.north(),     22);
    EXPECT_EQ(n.northEast(), 33);
    EXPECT_EQ(n.east(),      44);
    EXPECT_EQ(n.southEast(), 55);
    EXPECT_EQ(n.south(),     66);
    EXPECT_EQ(n.southWest(), 77);
    EXPECT_EQ(n.west(),      88);
    EXPECT_EQ(n.northWest(), 99);
}

// ============================================================
// 1.4 Цепочечный вызов (fluent API)
// ============================================================
TEST(MooreNBuilder, Chaining_AllSetters_ReturnsCorrectObject)
{
    auto n = MooreNBuilder{}
        .center(1)
        .north(2)
        .northEast(3)
        .east(4)
        .southEast(5)
        .south(6)
        .southWest(7)
        .west(8)
        .northWest(9)
        .build();

    EXPECT_EQ(n.center(),    1);
    EXPECT_EQ(n.north(),     2);
    EXPECT_EQ(n.northEast(), 3);
    EXPECT_EQ(n.east(),      4);
    EXPECT_EQ(n.southEast(), 5);
    EXPECT_EQ(n.south(),     6);
    EXPECT_EQ(n.southWest(), 7);
    EXPECT_EQ(n.west(),      8);
    EXPECT_EQ(n.northWest(), 9);
}

// ============================================================
// 1.5 Перезапись значений
// ============================================================
TEST(MooreNBuilder, Center_SetTwice_LastValueWins)
{
    auto n = MooreNBuilder{}.center(10).center(99).build();
    EXPECT_EQ(n.center(), 99);
}

TEST(MooreNBuilder, North_SetTwice_LastValueWins)
{
    auto n = MooreNBuilder{}.north(10).north(99).build();
    EXPECT_EQ(n.north(), 99);
}

TEST(MooreNBuilder, NorthEast_SetTwice_LastValueWins)
{
    auto n = MooreNBuilder{}.northEast(10).northEast(99).build();
    EXPECT_EQ(n.northEast(), 99);
}

TEST(MooreNBuilder, East_SetTwice_LastValueWins)
{
    auto n = MooreNBuilder{}.east(10).east(99).build();
    EXPECT_EQ(n.east(), 99);
}

TEST(MooreNBuilder, SouthEast_SetTwice_LastValueWins)
{
    auto n = MooreNBuilder{}.southEast(10).southEast(99).build();
    EXPECT_EQ(n.southEast(), 99);
}

TEST(MooreNBuilder, South_SetTwice_LastValueWins)
{
    auto n = MooreNBuilder{}.south(10).south(99).build();
    EXPECT_EQ(n.south(), 99);
}

TEST(MooreNBuilder, SouthWest_SetTwice_LastValueWins)
{
    auto n = MooreNBuilder{}.southWest(10).southWest(99).build();
    EXPECT_EQ(n.southWest(), 99);
}

TEST(MooreNBuilder, West_SetTwice_LastValueWins)
{
    auto n = MooreNBuilder{}.west(10).west(99).build();
    EXPECT_EQ(n.west(), 99);
}

TEST(MooreNBuilder, NorthWest_SetTwice_LastValueWins)
{
    auto n = MooreNBuilder{}.northWest(10).northWest(99).build();
    EXPECT_EQ(n.northWest(), 99);
}

// ============================================================
// 1.6 Повторное использование билдера
// ============================================================
TEST(MooreNBuilder, Build_CalledTwiceWithoutReset_SecondObjectSameValues)
{
    MooreNBuilder b;
    b.center(1).north(2).northEast(3).east(4).southEast(5)
     .south(6).southWest(7).west(8).northWest(9);

    auto n1 = b.build();
    auto n2 = b.build();

    EXPECT_EQ(n1.center(),    n2.center());
    EXPECT_EQ(n1.north(),     n2.north());
    EXPECT_EQ(n1.northEast(), n2.northEast());
    EXPECT_EQ(n1.east(),      n2.east());
    EXPECT_EQ(n1.southEast(), n2.southEast());
    EXPECT_EQ(n1.south(),     n2.south());
    EXPECT_EQ(n1.southWest(), n2.southWest());
    EXPECT_EQ(n1.west(),      n2.west());
    EXPECT_EQ(n1.northWest(), n2.northWest());
}

TEST(MooreNBuilder, Build_ModifyBetweenBuilds_SecondObjectReflectsChange)
{
    MooreNBuilder b;
    b.center(1).north(2).northEast(3).east(4).southEast(5)
     .south(6).southWest(7).west(8).northWest(9);

    auto n1 = b.build();
    b.center(99);
    auto n2 = b.build();

    EXPECT_EQ(n1.center(),   1);
    EXPECT_EQ(n2.center(),  99);
    EXPECT_EQ(n1.north(),     n2.north());
    EXPECT_EQ(n1.northEast(), n2.northEast());
    EXPECT_EQ(n1.east(),      n2.east());
    EXPECT_EQ(n1.southEast(), n2.southEast());
    EXPECT_EQ(n1.south(),     n2.south());
    EXPECT_EQ(n1.southWest(), n2.southWest());
    EXPECT_EQ(n1.west(),      n2.west());
    EXPECT_EQ(n1.northWest(), n2.northWest());
}

// ============================================================
// 1.7 Граничные значения uint8_t
// ============================================================
TEST(MooreNBuilder, Build_AllFieldsMinValue_AllReturnZero)
{
    auto n = MooreNBuilder{}
        .center(0).north(0).northEast(0).east(0).southEast(0)
        .south(0).southWest(0).west(0).northWest(0)
        .build();

    EXPECT_EQ(n.center(),    0);
    EXPECT_EQ(n.north(),     0);
    EXPECT_EQ(n.northEast(), 0);
    EXPECT_EQ(n.east(),      0);
    EXPECT_EQ(n.southEast(), 0);
    EXPECT_EQ(n.south(),     0);
    EXPECT_EQ(n.southWest(), 0);
    EXPECT_EQ(n.west(),      0);
    EXPECT_EQ(n.northWest(), 0);
}

TEST(MooreNBuilder, Build_AllFieldsMaxValue_AllReturn255)
{
    auto n = MooreNBuilder{}
        .center(255).north(255).northEast(255).east(255).southEast(255)
        .south(255).southWest(255).west(255).northWest(255)
        .build();

    EXPECT_EQ(n.center(),    255);
    EXPECT_EQ(n.north(),     255);
    EXPECT_EQ(n.northEast(), 255);
    EXPECT_EQ(n.east(),      255);
    EXPECT_EQ(n.southEast(), 255);
    EXPECT_EQ(n.south(),     255);
    EXPECT_EQ(n.southWest(), 255);
    EXPECT_EQ(n.west(),      255);
    EXPECT_EQ(n.northWest(), 255);
}

TEST(MooreNBuilder, Build_MixMinMaxValues_CorrectValues)
{
    auto n = MooreNBuilder{}
        .center(0).north(255).northEast(0).east(255).southEast(0)
        .south(255).southWest(0).west(255).northWest(0)
        .build();

    EXPECT_EQ(n.center(),      0);
    EXPECT_EQ(n.north(),     255);
    EXPECT_EQ(n.northEast(),   0);
    EXPECT_EQ(n.east(),      255);
    EXPECT_EQ(n.southEast(),   0);
    EXPECT_EQ(n.south(),     255);
    EXPECT_EQ(n.southWest(),   0);
    EXPECT_EQ(n.west(),      255);
    EXPECT_EQ(n.northWest(),   0);
}

// ============================================================
// 2.1 Независимость полей
// ============================================================
TEST(MooreN, Accessors_UniqueValuesPerField_EachAccessorReturnsOwnValue)
{
    auto n = MooreNBuilder{}
        .center(1)
        .north(2)
        .northEast(3)
        .east(4)
        .southEast(5)
        .south(6)
        .southWest(7)
        .west(8)
        .northWest(9)
        .build();

    EXPECT_EQ(n.center(),    1);
    EXPECT_EQ(n.north(),     2);
    EXPECT_EQ(n.northEast(), 3);
    EXPECT_EQ(n.east(),      4);
    EXPECT_EQ(n.southEast(), 5);
    EXPECT_EQ(n.south(),     6);
    EXPECT_EQ(n.southWest(), 7);
    EXPECT_EQ(n.west(),      8);
    EXPECT_EQ(n.northWest(), 9);
}

// ============================================================
// 2.2 Идемпотентность геттеров
// ============================================================
TEST(MooreN, Center_CalledMultipleTimes_ReturnsSameValue)
{
    auto n = MooreNBuilder{}.center(77).build();
    auto first  = n.center();
    auto second = n.center();
    auto third  = n.center();
    EXPECT_EQ(first,  77);
    EXPECT_EQ(second, 77);
    EXPECT_EQ(third,  77);
}

// ============================================================
// 3. MooreN::count() — представительные конфигурации 0..9
// ============================================================
//
// Полный перебор всех 2^9 = 512 конфигураций избыточен.
// Покрываем:
//   — каждое значение count от 0 до 9;
//   — для count == 1 все 9 одиночных полей (изоляция);
//   — для count == 8 все 9 «всё кроме одного» (изоляция);
//   — для промежуточных значений — по несколько характерных
//     комбинаций (кардинальные, диагональные, смешанные).
// ============================================================

struct MooreCountParam
{
    std::string name;
    uint8_t center;
    uint8_t north;
    uint8_t northEast;
    uint8_t east;
    uint8_t southEast;
    uint8_t south;
    uint8_t southWest;
    uint8_t west;
    uint8_t northWest;
    uint8_t expected;
};

class MooreNCount : public ::testing::TestWithParam<MooreCountParam> {};

TEST_P(MooreNCount, Count_GivenConfiguration_ReturnsExpected)
{
    auto [name, c, n, ne, e, se, s, sw, w, nw, expected] = GetParam();
    auto neighborhood = MooreNBuilder{}
        .center(c)
        .north(n)
        .northEast(ne)
        .east(e)
        .southEast(se)
        .south(s)
        .southWest(sw)
        .west(w)
        .northWest(nw)
        .build();
    EXPECT_EQ(neighborhood.count(), expected);
}

INSTANTIATE_TEST_SUITE_P(
    AllConfigurations,
    MooreNCount,
    ::testing::Values(
        // ===================== count == 0 (1) =====================
        //                                       C  N  NE  E  SE  S  SW  W  NW exp
        MooreCountParam{"AllZeros",              0, 0,  0, 0,  0, 0,  0, 0,  0, 0},

        // ===================== count == 1 (9) =====================
        MooreCountParam{"OnlyCenter",            1, 0,  0, 0,  0, 0,  0, 0,  0, 1},
        MooreCountParam{"OnlyNorth",             0, 1,  0, 0,  0, 0,  0, 0,  0, 1},
        MooreCountParam{"OnlyNorthEast",         0, 0,  1, 0,  0, 0,  0, 0,  0, 1},
        MooreCountParam{"OnlyEast",              0, 0,  0, 1,  0, 0,  0, 0,  0, 1},
        MooreCountParam{"OnlySouthEast",         0, 0,  0, 0,  1, 0,  0, 0,  0, 1},
        MooreCountParam{"OnlySouth",             0, 0,  0, 0,  0, 1,  0, 0,  0, 1},
        MooreCountParam{"OnlySouthWest",         0, 0,  0, 0,  0, 0,  1, 0,  0, 1},
        MooreCountParam{"OnlyWest",              0, 0,  0, 0,  0, 0,  0, 1,  0, 1},
        MooreCountParam{"OnlyNorthWest",         0, 0,  0, 0,  0, 0,  0, 0,  1, 1},

        // ===================== count == 2 (выборка) ===============
        MooreCountParam{"CenterNorth",           1, 1,  0, 0,  0, 0,  0, 0,  0, 2},
        MooreCountParam{"NorthEastCorners",      0, 0,  1, 0,  0, 0,  0, 0,  1, 2},
        MooreCountParam{"EastWest",              0, 0,  0, 1,  0, 0,  0, 1,  0, 2},
        MooreCountParam{"SouthEastSouthWest",    0, 0,  0, 0,  1, 0,  1, 0,  0, 2},
        MooreCountParam{"NorthSouth",            0, 1,  0, 0,  0, 1,  0, 0,  0, 2},

        // ===================== count == 3 (выборка) ===============
        MooreCountParam{"CardinalNES",           0, 1,  0, 1,  0, 1,  0, 0,  0, 3},
        MooreCountParam{"DiagNEastSEastSWest",   0, 0,  1, 0,  1, 0,  1, 0,  0, 3},
        MooreCountParam{"CenterNorthNorthEast",  1, 1,  1, 0,  0, 0,  0, 0,  0, 3},

        // ===================== count == 4 (выборка) ===============
        MooreCountParam{"CardinalNESW",          0, 1,  0, 1,  0, 1,  0, 1,  0, 4},
        MooreCountParam{"AllDiagonals",          0, 0,  1, 0,  1, 0,  1, 0,  1, 4},
        MooreCountParam{"CenterAndCardinalNES",  1, 1,  0, 1,  0, 1,  0, 0,  0, 4},

        // ===================== count == 5 (выборка) ===============
        MooreCountParam{"CenterAndCardinals",    1, 1,  0, 1,  0, 1,  0, 1,  0, 5},
        MooreCountParam{"CenterAndDiagonals",    1, 0,  1, 0,  1, 0,  1, 0,  1, 5},
        MooreCountParam{"NorthRingHalf",         0, 1,  1, 1,  0, 0,  0, 1,  1, 5},

        // ===================== count == 6 (выборка) ===============
        MooreCountParam{"AllExceptDiags_CE",     1, 1,  0, 1,  0, 1,  1, 1,  0, 6},
        MooreCountParam{"CenterCardinalsDiagNE", 1, 1,  1, 1,  0, 1,  0, 1,  0, 6},
        MooreCountParam{"SouthHalfPlusCenter",   1, 0,  0, 1,  1, 1,  1, 1,  0, 6},

        // ===================== count == 7 (выборка) ===============
        MooreCountParam{"AllExceptSEastNWest",   1, 1,  1, 1,  0, 1,  1, 1,  0, 7},
        MooreCountParam{"AllExceptNorthEast",    1, 1,  0, 1,  1, 1,  1, 1,  0, 7},
        MooreCountParam{"AllExceptCenterSouth",  0, 1,  1, 1,  1, 0,  1, 1,  1, 7},

        // ===================== count == 8 (9) =====================
        MooreCountParam{"AllExceptCenter",       0, 1,  1, 1,  1, 1,  1, 1,  1, 8},
        MooreCountParam{"AllExceptNorth",        1, 0,  1, 1,  1, 1,  1, 1,  1, 8},
        MooreCountParam{"AllExceptNorthEast8",   1, 1,  0, 1,  1, 1,  1, 1,  1, 8},
        MooreCountParam{"AllExceptEast",         1, 1,  1, 0,  1, 1,  1, 1,  1, 8},
        MooreCountParam{"AllExceptSouthEast",    1, 1,  1, 1,  0, 1,  1, 1,  1, 8},
        MooreCountParam{"AllExceptSouth",        1, 1,  1, 1,  1, 0,  1, 1,  1, 8},
        MooreCountParam{"AllExceptSouthWest",    1, 1,  1, 1,  1, 1,  0, 1,  1, 8},
        MooreCountParam{"AllExceptWest",         1, 1,  1, 1,  1, 1,  1, 0,  1, 8},
        MooreCountParam{"AllExceptNorthWest",    1, 1,  1, 1,  1, 1,  1, 1,  0, 8},

        // ===================== count == 9 (1) =====================
        MooreCountParam{"AllNonZero",            1, 1,  1, 1,  1, 1,  1, 1,  1, 9},

        // ===================== граничное значение =================
        MooreCountParam{"AllFieldsMax",        255,255,255,255,255,255,255,255,255, 9}
    ),
    [](const auto& info) { return info.param.name; }
);

// ============================================================
// 4.1 Тривиальные предикаты
// ============================================================
TEST(MooreN, CountIf_AlwaysTruePredicate_ReturnsNine)
{
    auto n = MooreNBuilder{}
        .center(10).north(20).northEast(30).east(40).southEast(50)
        .south(60).southWest(70).west(80).northWest(90)
        .build();
    EXPECT_EQ(n.countIf([](uint8_t) { return true; }), 9);
}

TEST(MooreN, CountIf_AlwaysFalsePredicate_ReturnsZero)
{
    auto n = MooreNBuilder{}
        .center(10).north(20).northEast(30).east(40).southEast(50)
        .south(60).southWest(70).west(80).northWest(90)
        .build();
    EXPECT_EQ(n.countIf([](uint8_t) { return false; }), 0);
}

// ============================================================
// 4.2 Фильтрация по значению
// ============================================================
TEST(MooreN, CountIf_GreaterThanThreshold_ReturnsMatchCount)
{
    auto n = MooreNBuilder{}
        .center(5).north(10).northEast(15).east(20).southEast(25)
        .south(30).southWest(35).west(40).northWest(45)
        .build();
    // > 15: east(20), southEast(25), south(30), southWest(35), west(40), northWest(45)
    EXPECT_EQ(n.countIf([](uint8_t v) { return v > 15; }), 6);
}

TEST(MooreN, CountIf_EqualToValue_ReturnsExactMatchCount)
{
    auto n = MooreNBuilder{}
        .center(5).north(5).northEast(0).east(5).southEast(5)
        .south(0).southWest(5).west(5).northWest(0)
        .build();
    EXPECT_EQ(n.countIf([](uint8_t v) { return v == 5; }), 6);
}

TEST(MooreN, CountIf_NoneMatch_ReturnsZero)
{
    auto n = MooreNBuilder{}
        .center(0).north(0).northEast(0).east(0).southEast(0)
        .south(0).southWest(0).west(0).northWest(0)
        .build();
    EXPECT_EQ(n.countIf([](uint8_t v) { return v > 0; }), 0);
}

TEST(MooreN, CountIf_AllMatch_ReturnsNine)
{
    auto n = MooreNBuilder{}
        .center(10).north(10).northEast(10).east(10).southEast(10)
        .south(10).southWest(10).west(10).northWest(10)
        .build();
    EXPECT_EQ(n.countIf([](uint8_t v) { return v == 10; }), 9);
}

// ============================================================
// 4.3 Center участвует в countIf()
// ============================================================
TEST(MooreN, CountIf_OnlyCenterMatchesPredicate_ReturnsOne)
{
    auto n = MooreNBuilder{}
        .center(99).north(0).northEast(0).east(0).southEast(0)
        .south(0).southWest(0).west(0).northWest(0)
        .build();
    EXPECT_EQ(n.countIf([](uint8_t v) { return v == 99; }), 1);
}

TEST(MooreN, CountIf_CenterDoesntMatchNeighborsDo_ReturnsEight)
{
    auto n = MooreNBuilder{}
        .center(0).north(99).northEast(99).east(99).southEast(99)
        .south(99).southWest(99).west(99).northWest(99)
        .build();
    EXPECT_EQ(n.countIf([](uint8_t v) { return v == 99; }), 8);
}

// ============================================================
// 4.4 Согласованность count() и countIf()
// ============================================================
TEST(MooreN, CountIf_NonZeroPredicate_EqualsCount)
{
    auto n = MooreNBuilder{}
        .center(7).north(0).northEast(3).east(0).southEast(12)
        .south(0).southWest(1).west(0).northWest(5)
        .build();
    auto countIfResult = n.countIf([](uint8_t v) { return v != 0; });
    EXPECT_EQ(countIfResult, n.count());
}

// ============================================================
// 4.5 Различные типы предикатов
// ============================================================

// --- лямбда ---
TEST(MooreN, CountIf_LambdaPredicate_Works)
{
    auto n = MooreNBuilder{}
        .center(1).north(2).northEast(3).east(4).southEast(5)
        .south(6).southWest(7).west(8).northWest(9)
        .build();
    auto pred = [](uint8_t v) { return v >= 5; };
    // 5,6,7,8,9
    EXPECT_EQ(n.countIf(pred), 5);
}

// --- указатель на свободную функцию ---
static bool isMooreEven(uint8_t v) { return v % 2 == 0; }

TEST(MooreN, CountIf_FunctionPointerPredicate_Works)
{
    auto n = MooreNBuilder{}
        .center(1).north(2).northEast(3).east(4).southEast(5)
        .south(6).southWest(7).west(8).northWest(9)
        .build();
    // even: 2,4,6,8
    EXPECT_EQ(n.countIf(isMooreEven), 4);
}

// --- функтор ---
struct MooreGreaterThan
{
    uint8_t threshold;
    bool operator()(uint8_t v) const { return v > threshold; }
};

TEST(MooreN, CountIf_FunctorPredicate_Works)
{
    auto n = MooreNBuilder{}
        .center(1).north(2).northEast(3).east(4).southEast(5)
        .south(6).southWest(7).west(8).northWest(9)
        .build();
    // > 6: 7,8,9
    EXPECT_EQ(n.countIf(MooreGreaterThan{6}), 3);
}