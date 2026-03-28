#include <gtest/gtest.h>
#include <climits>
#include <cstddef>

#include "TorusTopology.h"  // подключить реальный заголовок

/**
 * VIBE CODE ALERT
 */

using namespace ntcap;

// ============================================================================
// 2.1  Конструктор — валидные параметры
// ============================================================================

TEST(TorusTopology, Construct_TypicalDimensions_CreatesSuccessfully)
{
    EXPECT_NO_THROW(TorusTopology(10, 20));
}

TEST(TorusTopology, Construct_SquareGrid_CreatesSuccessfully)
{
    EXPECT_NO_THROW(TorusTopology(5, 5));
}

TEST(TorusTopology, Construct_MinimalDimensions_CreatesSuccessfully)
{
    EXPECT_NO_THROW(TorusTopology(1, 1));
}

TEST(TorusTopology, Construct_WidthOne_CreatesSuccessfully)
{
    EXPECT_NO_THROW(TorusTopology(1, 10));
}

TEST(TorusTopology, Construct_HeightOne_CreatesSuccessfully)
{
    EXPECT_NO_THROW(TorusTopology(10, 1));
}

TEST(TorusTopology, Construct_LargeDimensions_CreatesSuccessfully)
{
    EXPECT_NO_THROW(TorusTopology(SIZE_MAX / 2, SIZE_MAX / 2));
}

// ============================================================================
// 2.2  Конструктор — невалидные параметры
// ============================================================================

TEST(TorusTopology, Construct_ZeroWidth_Throws)
{
    EXPECT_ANY_THROW(TorusTopology(0, 10));
}

TEST(TorusTopology, Construct_ZeroHeight_Throws)
{
    EXPECT_ANY_THROW(TorusTopology(10, 0));
}

TEST(TorusTopology, Construct_ZeroBothDimensions_Throws)
{
    EXPECT_ANY_THROW(TorusTopology(0, 0));
}

// ============================================================================
// Вспомогательная фикстура
// ============================================================================

class TorusTopology10x10 : public ::testing::Test {
protected:
    TorusTopology torus{10, 10};
};

// ============================================================================
// 3.1  Координаты внутри диапазона [0, width) × [0, height)
// ============================================================================

TEST_F(TorusTopology10x10, Normalize_Origin_ReturnsOrigin)
{
    auto result = torus.normalize({0, 0});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 0u);
}

TEST_F(TorusTopology10x10, Normalize_CenterOfField_ReturnsSameCoords)
{
    auto result = torus.normalize({5, 5});
    EXPECT_EQ(result.x, 5u);
    EXPECT_EQ(result.y, 5u);
}

TEST_F(TorusTopology10x10, Normalize_MaxValidCoord_ReturnsSameCoords)
{
    auto result = torus.normalize({9, 9});
    EXPECT_EQ(result.x, 9u);
    EXPECT_EQ(result.y, 9u);
}

TEST(TorusTopology, Normalize_ArbitraryInsideRect7x13_ReturnsSameCoords)
{
    TorusTopology torus(7, 13);
    auto result = torus.normalize({3, 8});
    EXPECT_EQ(result.x, 3u);
    EXPECT_EQ(result.y, 8u);
}

// ============================================================================
// 3.2  Координаты на границе (переход через край)
// ============================================================================

TEST_F(TorusTopology10x10, Normalize_XEqualsWidth_WrapsXToZero)
{
    auto result = torus.normalize({10, 5});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 5u);
}

TEST_F(TorusTopology10x10, Normalize_YEqualsHeight_WrapsYToZero)
{
    auto result = torus.normalize({5, 10});
    EXPECT_EQ(result.x, 5u);
    EXPECT_EQ(result.y, 0u);
}

TEST_F(TorusTopology10x10, Normalize_BothEqualDimensions_WrapsBothToZero)
{
    auto result = torus.normalize({10, 10});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 0u);
}

TEST_F(TorusTopology10x10, Normalize_XAtWidthMinusOne_DoesNotWrap)
{
    auto result = torus.normalize({9, 0});
    EXPECT_EQ(result.x, 9u);
    EXPECT_EQ(result.y, 0u);
}

// ============================================================================
// 3.3  Положительное переполнение (координаты > размеров)
// ============================================================================

TEST_F(TorusTopology10x10, Normalize_XExceedsWidthByHalf_WrapsCorrectly)
{
    auto result = torus.normalize({15, 3});
    EXPECT_EQ(result.x, 5u);
    EXPECT_EQ(result.y, 3u);
}

TEST_F(TorusTopology10x10, Normalize_YExceedsHeightByHalf_WrapsCorrectly)
{
    auto result = torus.normalize({3, 17});
    EXPECT_EQ(result.x, 3u);
    EXPECT_EQ(result.y, 7u);
}

TEST_F(TorusTopology10x10, Normalize_XMultipleFullWraps_WrapsCorrectly)
{
    auto result = torus.normalize({25, 0});
    EXPECT_EQ(result.x, 5u);
    EXPECT_EQ(result.y, 0u);
}

TEST_F(TorusTopology10x10, Normalize_YMultipleFullWraps_WrapsCorrectly)
{
    auto result = torus.normalize({0, 33});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 3u);
}

TEST_F(TorusTopology10x10, Normalize_BothMultipleFullWraps_WrapsCorrectly)
{
    auto result = torus.normalize({23, 37});
    EXPECT_EQ(result.x, 3u);
    EXPECT_EQ(result.y, 7u);
}

TEST_F(TorusTopology10x10, Normalize_XExactMultipleOfWidth_WrapsToZero)
{
    auto result = torus.normalize({20, 0});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 0u);
}

TEST_F(TorusTopology10x10, Normalize_YExactMultipleOfHeight_WrapsToZero)
{
    auto result = torus.normalize({0, 30});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 0u);
}

// ============================================================================
// 3.4  Отрицательные координаты
// ============================================================================

TEST_F(TorusTopology10x10, Normalize_XNegativeOne_WrapsToWidthMinusOne)
{
    auto result = torus.normalize({-1, 5});
    EXPECT_EQ(result.x, 9u);
    EXPECT_EQ(result.y, 5u);
}

TEST_F(TorusTopology10x10, Normalize_YNegativeOne_WrapsToHeightMinusOne)
{
    auto result = torus.normalize({5, -1});
    EXPECT_EQ(result.x, 5u);
    EXPECT_EQ(result.y, 9u);
}

TEST_F(TorusTopology10x10, Normalize_BothNegativeOne_WrapsBothToMax)
{
    auto result = torus.normalize({-1, -1});
    EXPECT_EQ(result.x, 9u);
    EXPECT_EQ(result.y, 9u);
}

TEST_F(TorusTopology10x10, Normalize_XNegativeWidth_WrapsToZero)
{
    auto result = torus.normalize({-10, 0});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 0u);
}

TEST_F(TorusTopology10x10, Normalize_YNegativeHeight_WrapsToZero)
{
    auto result = torus.normalize({0, -10});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 0u);
}

TEST_F(TorusTopology10x10, Normalize_BothNegativeMultipleWraps_WrapsCorrectly)
{
    auto result = torus.normalize({-23, -37});
    EXPECT_EQ(result.x, 7u);
    EXPECT_EQ(result.y, 3u);
}

TEST_F(TorusTopology10x10, Normalize_BothNegativeExactMultiple_WrapsToZero)
{
    auto result = torus.normalize({-10, -10});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 0u);
}

TEST_F(TorusTopology10x10, Normalize_BothLargeNegativeMultiple_WrapsToZero)
{
    auto result = torus.normalize({-1000, -1000});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 0u);
}

TEST_F(TorusTopology10x10, Normalize_IntMinValues_WrapsCorrectly)
{
    // INT_MIN % 10 зависит от реализации, вычисляем ожидание вручную:
    // Для корректной модульной арифметики на торе:
    //   normalized = ((value % dim) + dim) % dim
    constexpr int mn = INT_MIN;
    const size_t expected_x = static_cast<size_t>(((mn % 10) + 10) % 10);
    const size_t expected_y = static_cast<size_t>(((mn % 10) + 10) % 10);

    auto result = torus.normalize({mn, mn});
    EXPECT_EQ(result.x, expected_x);
    EXPECT_EQ(result.y, expected_y);
}

// ============================================================================
// 3.5  Вырожденные размеры тора
// ============================================================================

TEST(TorusTopology, Normalize_1x1ZeroCoords_ReturnsZero)
{
    TorusTopology torus(1, 1);
    auto result = torus.normalize({0, 0});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 0u);
}

TEST(TorusTopology, Normalize_1x1ArbitraryCoords_ReturnsZero)
{
    TorusTopology torus(1, 1);
    auto result = torus.normalize({42, -17});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 0u);
}

TEST(TorusTopology, Normalize_1xN_XAlwaysZero)
{
    TorusTopology torus(1, 10);
    auto result = torus.normalize({5, 3});
    EXPECT_EQ(result.x, 0u);
    EXPECT_EQ(result.y, 3u);
}

TEST(TorusTopology, Normalize_Nx1_YAlwaysZero)
{
    TorusTopology torus(10, 1);
    auto result = torus.normalize({3, 5});
    EXPECT_EQ(result.x, 3u);
    EXPECT_EQ(result.y, 0u);
}

// ============================================================================
// 3.6  Асимметричные размеры
// ============================================================================

TEST(TorusTopology, Normalize_Rect7x13PositiveOverflow_WrapsCorrectly)
{
    TorusTopology torus(7, 13);
    auto result = torus.normalize({10, 15});
    EXPECT_EQ(result.x, 3u);
    EXPECT_EQ(result.y, 2u);
}

TEST(TorusTopology, Normalize_Rect7x13NegativeCoords_WrapsCorrectly)
{
    TorusTopology torus(7, 13);
    auto result = torus.normalize({-3, -5});
    EXPECT_EQ(result.x, 4u);
    EXPECT_EQ(result.y, 8u);
}
