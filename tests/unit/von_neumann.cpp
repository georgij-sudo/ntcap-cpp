#include <gtest/gtest.h>

#include "VonNeumannN.h"

using namespace ntcap;

// ============================================================
// 1.1 Значения по умолчанию
// ============================================================

TEST(VonNeumannNBuilder, Build_NoSettersCalled_AllFieldsZero)
{
    auto n = VonNeumannNBuilder{}.build();

    EXPECT_EQ(n.center(), 0);
    EXPECT_EQ(n.north(),  0);
    EXPECT_EQ(n.east(),   0);
    EXPECT_EQ(n.south(),  0);
    EXPECT_EQ(n.west(),   0);
}

// ============================================================
// 1.2 Установка отдельных полей
// ============================================================

TEST(VonNeumannNBuilder, Center_SetValue_ReturnsSameValue)
{
    auto n = VonNeumannNBuilder{}.center(42).build();

    EXPECT_EQ(n.center(), 42);
    EXPECT_EQ(n.north(),   0);
    EXPECT_EQ(n.east(),    0);
    EXPECT_EQ(n.south(),   0);
    EXPECT_EQ(n.west(),    0);
}

TEST(VonNeumannNBuilder, North_SetValue_ReturnsSameValue)
{
    auto n = VonNeumannNBuilder{}.north(10).build();

    EXPECT_EQ(n.center(), 0);
    EXPECT_EQ(n.north(), 10);
    EXPECT_EQ(n.east(),   0);
    EXPECT_EQ(n.south(),  0);
    EXPECT_EQ(n.west(),   0);
}

TEST(VonNeumannNBuilder, East_SetValue_ReturnsSameValue)
{
    auto n = VonNeumannNBuilder{}.east(20).build();

    EXPECT_EQ(n.center(), 0);
    EXPECT_EQ(n.north(),  0);
    EXPECT_EQ(n.east(),  20);
    EXPECT_EQ(n.south(),  0);
    EXPECT_EQ(n.west(),   0);
}

TEST(VonNeumannNBuilder, South_SetValue_ReturnsSameValue)
{
    auto n = VonNeumannNBuilder{}.south(30).build();

    EXPECT_EQ(n.center(), 0);
    EXPECT_EQ(n.north(),  0);
    EXPECT_EQ(n.east(),   0);
    EXPECT_EQ(n.south(), 30);
    EXPECT_EQ(n.west(),   0);
}

TEST(VonNeumannNBuilder, West_SetValue_ReturnsSameValue)
{
    auto n = VonNeumannNBuilder{}.west(40).build();

    EXPECT_EQ(n.center(), 0);
    EXPECT_EQ(n.north(),  0);
    EXPECT_EQ(n.east(),   0);
    EXPECT_EQ(n.south(),  0);
    EXPECT_EQ(n.west(),  40);
}

// ============================================================
// 1.3 Установка всех полей
// ============================================================

TEST(VonNeumannNBuilder, Build_AllFieldsSet_AllReturnCorrectValues)
{
    VonNeumannNBuilder b;
    b.center(11);
    b.north(22);
    b.east(33);
    b.south(44);
    b.west(55);
    auto n = b.build();

    EXPECT_EQ(n.center(), 11);
    EXPECT_EQ(n.north(),  22);
    EXPECT_EQ(n.east(),   33);
    EXPECT_EQ(n.south(),  44);
    EXPECT_EQ(n.west(),   55);
}

// ============================================================
// 1.4 Цепочечный вызов (fluent API)
// ============================================================

TEST(VonNeumannNBuilder, Chaining_AllSetters_ReturnsCorrectObject)
{
    auto n = VonNeumannNBuilder{}
        .center(1)
        .north(2)
        .east(3)
        .south(4)
        .west(5)
        .build();

    EXPECT_EQ(n.center(), 1);
    EXPECT_EQ(n.north(),  2);
    EXPECT_EQ(n.east(),   3);
    EXPECT_EQ(n.south(),  4);
    EXPECT_EQ(n.west(),   5);
}

// ============================================================
// 1.5 Перезапись значений
// ============================================================

TEST(VonNeumannNBuilder, Center_SetTwice_LastValueWins)
{
    auto n = VonNeumannNBuilder{}.center(10).center(99).build();
    EXPECT_EQ(n.center(), 99);
}

TEST(VonNeumannNBuilder, North_SetTwice_LastValueWins)
{
    auto n = VonNeumannNBuilder{}.north(10).north(99).build();
    EXPECT_EQ(n.north(), 99);
}

TEST(VonNeumannNBuilder, East_SetTwice_LastValueWins)
{
    auto n = VonNeumannNBuilder{}.east(10).east(99).build();
    EXPECT_EQ(n.east(), 99);
}

TEST(VonNeumannNBuilder, South_SetTwice_LastValueWins)
{
    auto n = VonNeumannNBuilder{}.south(10).south(99).build();
    EXPECT_EQ(n.south(), 99);
}

TEST(VonNeumannNBuilder, West_SetTwice_LastValueWins)
{
    auto n = VonNeumannNBuilder{}.west(10).west(99).build();
    EXPECT_EQ(n.west(), 99);
}

// ============================================================
// 1.6 Повторное использование билдера
// ============================================================

TEST(VonNeumannNBuilder, Build_CalledTwiceWithoutReset_SecondObjectSameValues)
{
    VonNeumannNBuilder b;
    b.center(1).north(2).east(3).south(4).west(5);

    auto n1 = b.build();
    auto n2 = b.build();

    EXPECT_EQ(n1.center(), n2.center());
    EXPECT_EQ(n1.north(),  n2.north());
    EXPECT_EQ(n1.east(),   n2.east());
    EXPECT_EQ(n1.south(),  n2.south());
    EXPECT_EQ(n1.west(),   n2.west());
}

TEST(VonNeumannNBuilder, Build_ModifyBetweenBuilds_SecondObjectReflectsChange)
{
    VonNeumannNBuilder b;
    b.center(1).north(2).east(3).south(4).west(5);

    auto n1 = b.build();
    b.center(99);
    auto n2 = b.build();

    EXPECT_EQ(n1.center(),  1);
    EXPECT_EQ(n2.center(), 99);

    EXPECT_EQ(n1.north(), n2.north());
    EXPECT_EQ(n1.east(),  n2.east());
    EXPECT_EQ(n1.south(), n2.south());
    EXPECT_EQ(n1.west(),  n2.west());
}

// ============================================================
// 1.7 Граничные значения uint8_t
// ============================================================

TEST(VonNeumannNBuilder, Build_AllFieldsMinValue_AllReturnZero)
{
    auto n = VonNeumannNBuilder{}
        .center(0).north(0).east(0).south(0).west(0)
        .build();

    EXPECT_EQ(n.center(), 0);
    EXPECT_EQ(n.north(),  0);
    EXPECT_EQ(n.east(),   0);
    EXPECT_EQ(n.south(),  0);
    EXPECT_EQ(n.west(),   0);
}

TEST(VonNeumannNBuilder, Build_AllFieldsMaxValue_AllReturn255)
{
    auto n = VonNeumannNBuilder{}
        .center(255).north(255).east(255).south(255).west(255)
        .build();

    EXPECT_EQ(n.center(), 255);
    EXPECT_EQ(n.north(),  255);
    EXPECT_EQ(n.east(),   255);
    EXPECT_EQ(n.south(),  255);
    EXPECT_EQ(n.west(),   255);
}

TEST(VonNeumannNBuilder, Build_MixMinMaxValues_CorrectValues)
{
    auto n = VonNeumannNBuilder{}
        .center(0).north(255).east(0).south(255).west(0)
        .build();

    EXPECT_EQ(n.center(),   0);
    EXPECT_EQ(n.north(),  255);
    EXPECT_EQ(n.east(),     0);
    EXPECT_EQ(n.south(),  255);
    EXPECT_EQ(n.west(),     0);
}

// ============================================================
// 2.1 Независимость полей
// ============================================================

TEST(VonNeumannN, Accessors_UniqueValuesPerField_EachAccessorReturnsOwnValue)
{
    auto n = VonNeumannNBuilder{}
        .center(1)
        .north(2)
        .east(3)
        .south(4)
        .west(5)
        .build();

    EXPECT_EQ(n.center(), 1);
    EXPECT_EQ(n.north(),  2);
    EXPECT_EQ(n.east(),   3);
    EXPECT_EQ(n.south(),  4);
    EXPECT_EQ(n.west(),   5);
}

// ============================================================
// 2.2 Идемпотентность геттеров
// ============================================================

TEST(VonNeumannN, Center_CalledMultipleTimes_ReturnsSameValue)
{
    auto n = VonNeumannNBuilder{}.center(77).build();

    auto first  = n.center();
    auto second = n.center();
    auto third  = n.center();

    EXPECT_EQ(first,  77);
    EXPECT_EQ(second, 77);
    EXPECT_EQ(third,  77);
}

// ============================================================
// 3. VonNeumannN::count() — все 32 конфигурации + граничное
// ============================================================

struct CountParam
{
    std::string name;
    uint8_t center;
    uint8_t north;
    uint8_t east;
    uint8_t south;
    uint8_t west;
    uint8_t expected;
};

class VonNeumannNCount : public ::testing::TestWithParam<CountParam> {};

TEST_P(VonNeumannNCount, Count_GivenConfiguration_ReturnsExpected)
{
    auto [name, c, n, e, s, w, expected] = GetParam();

    auto neighborhood = VonNeumannNBuilder{}
        .center(c)
        .north(n)
        .east(e)
        .south(s)
        .west(w)
        .build();

    EXPECT_EQ(neighborhood.count(), expected);
}

INSTANTIATE_TEST_SUITE_P(
    AllConfigurations,
    VonNeumannNCount,
    ::testing::Values(
        // ===================== count == 0 (1) =====================
        //                                  C  N  E  S  W  exp
        CountParam{"AllZeros",              0, 0, 0, 0, 0, 0},

        // ===================== count == 1 (5) =====================
        CountParam{"OnlyCenter",            1, 0, 0, 0, 0, 1},
        CountParam{"OnlyNorth",             0, 1, 0, 0, 0, 1},
        CountParam{"OnlyEast",              0, 0, 1, 0, 0, 1},
        CountParam{"OnlySouth",             0, 0, 0, 1, 0, 1},
        CountParam{"OnlyWest",              0, 0, 0, 0, 1, 1},

        // ===================== count == 2 (10) ====================
        CountParam{"CenterNorth",           1, 1, 0, 0, 0, 2},
        CountParam{"CenterEast",            1, 0, 1, 0, 0, 2},
        CountParam{"CenterSouth",           1, 0, 0, 1, 0, 2},
        CountParam{"CenterWest",            1, 0, 0, 0, 1, 2},
        CountParam{"NorthEast",             0, 1, 1, 0, 0, 2},
        CountParam{"NorthSouth",            0, 1, 0, 1, 0, 2},
        CountParam{"NorthWest",             0, 1, 0, 0, 1, 2},
        CountParam{"EastSouth",             0, 0, 1, 1, 0, 2},
        CountParam{"EastWest",              0, 0, 1, 0, 1, 2},
        CountParam{"SouthWest",             0, 0, 0, 1, 1, 2},

        // ===================== count == 3 (10) ====================
        CountParam{"CenterNorthEast",       1, 1, 1, 0, 0, 3},
        CountParam{"CenterNorthSouth",      1, 1, 0, 1, 0, 3},
        CountParam{"CenterNorthWest",       1, 1, 0, 0, 1, 3},
        CountParam{"CenterEastSouth",       1, 0, 1, 1, 0, 3},
        CountParam{"CenterEastWest",        1, 0, 1, 0, 1, 3},
        CountParam{"CenterSouthWest",       1, 0, 0, 1, 1, 3},
        CountParam{"NorthEastSouth",        0, 1, 1, 1, 0, 3},
        CountParam{"NorthEastWest",         0, 1, 1, 0, 1, 3},
        CountParam{"NorthSouthWest",        0, 1, 0, 1, 1, 3},
        CountParam{"EastSouthWest",         0, 0, 1, 1, 1, 3},

        // ===================== count == 4 (5) =====================
        CountParam{"AllExceptWest",         1, 1, 1, 1, 0, 4},
        CountParam{"AllExceptSouth",        1, 1, 1, 0, 1, 4},
        CountParam{"AllExceptEast",         1, 1, 0, 1, 1, 4},
        CountParam{"AllExceptNorth",        1, 0, 1, 1, 1, 4},
        CountParam{"AllExceptCenter",       0, 1, 1, 1, 1, 4},

        // ===================== count == 5 (1) =====================
        CountParam{"AllNonZero",            1, 1, 1, 1, 1, 5},

        // ===================== граничное значение =================
        CountParam{"AllFieldsMax",        255,255,255,255,255, 5}
    ),
    [](const auto& info) { return info.param.name; }
);

// ============================================================
// 4.1 Тривиальные предикаты
// ============================================================

TEST(VonNeumannN, CountIf_AlwaysTruePredicate_ReturnsFive)
{
    auto n = VonNeumannNBuilder{}
        .center(10).north(20).east(30).south(40).west(50)
        .build();

    EXPECT_EQ(n.countIf([](uint8_t) { return true; }), 5);
}

TEST(VonNeumannN, CountIf_AlwaysFalsePredicate_ReturnsZero)
{
    auto n = VonNeumannNBuilder{}
        .center(10).north(20).east(30).south(40).west(50)
        .build();

    EXPECT_EQ(n.countIf([](uint8_t) { return false; }), 0);
}

// ============================================================
// 4.2 Фильтрация по значению
// ============================================================

TEST(VonNeumannN, CountIf_GreaterThanThreshold_ReturnsMatchCount)
{
    auto n = VonNeumannNBuilder{}
        .center(5).north(10).east(20).south(30).west(40)
        .build();

    EXPECT_EQ(n.countIf([](uint8_t v) { return v > 15; }), 3);
}

TEST(VonNeumannN, CountIf_EqualToValue_ReturnsExactMatchCount)
{
    auto n = VonNeumannNBuilder{}
        .center(5).north(5).east(0).south(5).west(5)
        .build();

    EXPECT_EQ(n.countIf([](uint8_t v) { return v == 5; }), 4);
}

TEST(VonNeumannN, CountIf_NoneMatch_ReturnsZero)
{
    auto n = VonNeumannNBuilder{}
        .center(0).north(0).east(0).south(0).west(0)
        .build();

    EXPECT_EQ(n.countIf([](uint8_t v) { return v > 0; }), 0);
}

TEST(VonNeumannN, CountIf_AllMatch_ReturnsFive)
{
    auto n = VonNeumannNBuilder{}
        .center(10).north(10).east(10).south(10).west(10)
        .build();

    EXPECT_EQ(n.countIf([](uint8_t v) { return v == 10; }), 5);
}

// ============================================================
// 4.3 Center участвует в countIf()
// ============================================================

TEST(VonNeumannN, CountIf_OnlyCenterMatchesPredicate_ReturnsOne)
{
    auto n = VonNeumannNBuilder{}
        .center(99).north(0).east(0).south(0).west(0)
        .build();

    EXPECT_EQ(n.countIf([](uint8_t v) { return v == 99; }), 1);
}

TEST(VonNeumannN, CountIf_CenterDoesntMatchNeighborsDo_ReturnsFour)
{
    auto n = VonNeumannNBuilder{}
        .center(0).north(99).east(99).south(99).west(99)
        .build();

    EXPECT_EQ(n.countIf([](uint8_t v) { return v == 99; }), 4);
}

// ============================================================
// 4.4 Согласованность count() и countIf()
// ============================================================

TEST(VonNeumannN, CountIf_NonZeroPredicate_EqualsCount)
{
    auto n = VonNeumannNBuilder{}
        .center(7).north(0).east(3).south(0).west(12)
        .build();

    auto countIfResult = n.countIf([](uint8_t v) { return v != 0; });

    EXPECT_EQ(countIfResult, n.count());
}

// ============================================================
// 4.5 Различные типы предикатов
// ============================================================

// --- лямбда ---
TEST(VonNeumannN, CountIf_LambdaPredicate_Works)
{
    auto n = VonNeumannNBuilder{}
        .center(1).north(2).east(3).south(4).west(5)
        .build();

    auto pred = [](uint8_t v) { return v >= 3; };

    EXPECT_EQ(n.countIf(pred), 3);
}

// --- указатель на свободную функцию ---
static bool isEven(uint8_t v) { return v % 2 == 0; }

TEST(VonNeumannN, CountIf_FunctionPointerPredicate_Works)
{
    auto n = VonNeumannNBuilder{}
        .center(1).north(2).east(3).south(4).west(5)
        .build();

    EXPECT_EQ(n.countIf(isEven), 2);
}

// --- функтор ---
struct GreaterThan
{
    uint8_t threshold;
    bool operator()(uint8_t v) const { return v > threshold; }
};

TEST(VonNeumannN, CountIf_FunctorPredicate_Works)
{
    auto n = VonNeumannNBuilder{}
        .center(1).north(2).east(3).south(4).west(5)
        .build();

    EXPECT_EQ(n.countIf(GreaterThan{3}), 2);
}

