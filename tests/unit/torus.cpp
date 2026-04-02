#include <gtest/gtest.h>
#include <climits>
#include <cstddef>
  
#include "TorusTopology.h"

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
// 3.1  WrapX и WrapY для координат в диапазоне [0, width) × [0, height)
// ============================================================================
  
TEST_F(TorusTopology10x10, WrapX_ZeroCoord_ReturnsZero)
{
    auto result = torus.wrapX(0);
    EXPECT_EQ(result, 0u);
}
  
TEST_F(TorusTopology10x10, WrapX_CenterCoord_ReturnsSameCoord)
{
    auto result = torus.wrapX(5);
    EXPECT_EQ(result, 5u);
}
  
TEST_F(TorusTopology10x10, WrapX_MaxValidCoord_ReturnsSameCoord)
{
    auto result = torus.wrapX(9);
    EXPECT_EQ(result, 9u);
}
  
TEST_F(TorusTopology10x10, WrapY_ZeroCoord_ReturnsZero)
{
    auto result = torus.wrapY(0);
    EXPECT_EQ(result, 0u);
}
  
TEST_F(TorusTopology10x10, WrapY_CenterCoord_ReturnsSameCoord)
{
    auto result = torus.wrapY(5);
    EXPECT_EQ(result, 5u);
}
  
TEST_F(TorusTopology10x10, WrapY_MaxValidCoord_ReturnsSameCoord)
{
    auto result = torus.wrapY(9);
    EXPECT_EQ(result, 9u);
}
  
TEST(TorusTopology, WrapX_ArbitraryCoord7x13_ReturnsSameCoord)
{
    TorusTopology torus(7, 13);
    auto result = torus.wrapX(3);
    EXPECT_EQ(result, 3u);
}
  
TEST(TorusTopology, WrapY_ArbitraryCoord7x13_ReturnsSameCoord)
{
    TorusTopology torus(7, 13);
    auto result = torus.wrapY(8);
    EXPECT_EQ(result, 8u);
}
  
// ============================================================================
// 3.2  Граничные случаи: переход через границу (координаты = dim)
// ============================================================================
  
TEST_F(TorusTopology10x10, WrapX_EqualsWidth_WrapsToZero)
{
    auto result = torus.wrapX(10);
    EXPECT_EQ(result, 0u);
}
  
TEST_F(TorusTopology10x10, WrapY_EqualsHeight_WrapsToZero)
{
    auto result = torus.wrapY(10);
    EXPECT_EQ(result, 0u);
}
  
TEST_F(TorusTopology10x10, WrapX_XAtWidthMinusOne_DoesNotWrap)
{
    auto result = torus.wrapX(9);
    EXPECT_EQ(result, 9u);
}
  
TEST_F(TorusTopology10x10, WrapY_YAtHeightMinusOne_DoesNotWrap)
{
    auto result = torus.wrapY(9);
    EXPECT_EQ(result, 9u);
}
  
// ============================================================================
// 3.3  Отрицательные координаты (диапазон [-1, 0))
// ============================================================================
  
TEST_F(TorusTopology10x10, WrapX_NegativeOne_WrapsToWidthMinusOne)
{
    auto result = torus.wrapX(-1);
    EXPECT_EQ(result, 9u);
}
  
TEST_F(TorusTopology10x10, WrapY_NegativeOne_WrapsToHeightMinusOne)
{
    auto result = torus.wrapY(-1);
    EXPECT_EQ(result, 9u);
}
  
// ============================================================================
// 3.4  Вырожденные размеры тора
// ============================================================================
  
TEST(TorusTopology, WrapX_1x1_ZeroCoord_ReturnsZero)
{
    TorusTopology torus(1, 1);
    auto result = torus.wrapX(0);
    EXPECT_EQ(result, 0u);
}
  
TEST(TorusTopology, WrapY_1x1_ZeroCoord_ReturnsZero)
{
    TorusTopology torus(1, 1);
    auto result = torus.wrapY(0);
    EXPECT_EQ(result, 0u);
}
  
TEST(TorusTopology, WrapX_1x1_NegativeOne_WrapsToZero)
{
    TorusTopology torus(1, 1);
    auto result = torus.wrapX(-1);
    EXPECT_EQ(result, 0u);
}
  
TEST(TorusTopology, WrapY_1x1_NegativeOne_WrapsToZero)
{
    TorusTopology torus(1, 1);
    auto result = torus.wrapY(-1);
    EXPECT_EQ(result, 0u);
}
  
TEST(TorusTopology, WrapX_1xN_ValidRange_ReturnsZero)
{
    TorusTopology torus(1, 10);
    
    EXPECT_EQ(torus.wrapX(0), 0u);
    EXPECT_EQ(torus.wrapX(-1), 0u);
    EXPECT_EQ(torus.wrapX(1), 0u);
}
  
TEST(TorusTopology, WrapY_Nx1_ValidRange_ReturnsZero)
{
    TorusTopology torus(10, 1);
    
    EXPECT_EQ(torus.wrapY(0), 0u);
    EXPECT_EQ(torus.wrapY(-1), 0u);
    EXPECT_EQ(torus.wrapY(1), 0u);
}
  
// ============================================================================
// 3.5  Асимметричные размеры
// ============================================================================
  
TEST(TorusTopology, WrapX_Rect7x13_ValidCoord_ReturnsSameCoord)
{
    TorusTopology torus(7, 13);
    
    EXPECT_EQ(torus.wrapX(3), 3u);
    EXPECT_EQ(torus.wrapX(0), 0u);
    EXPECT_EQ(torus.wrapX(6), 6u);
}
  
TEST(TorusTopology, WrapY_Rect7x13_ValidCoord_ReturnsSameCoord)
{
    TorusTopology torus(7, 13);
    
    EXPECT_EQ(torus.wrapY(5), 5u);
    EXPECT_EQ(torus.wrapY(0), 0u);
    EXPECT_EQ(torus.wrapY(12), 12u);
}
  
TEST(TorusTopology, WrapX_Rect7x13_AtBoundary_WrapsToZero)
{
    TorusTopology torus(7, 13);
    EXPECT_EQ(torus.wrapX(7), 0u);
}
  
TEST(TorusTopology, WrapY_Rect7x13_AtBoundary_WrapsToZero)
{
    TorusTopology torus(7, 13);
    EXPECT_EQ(torus.wrapY(13), 0u);
}
  
TEST(TorusTopology, WrapX_Rect7x13_Negative_WrapsCorrectly)
{
    TorusTopology torus(7, 13);
    EXPECT_EQ(torus.wrapX(-1), 6u);
}
  
TEST(TorusTopology, WrapY_Rect7x13_Negative_WrapsCorrectly)
{
    TorusTopology torus(7, 13);
    EXPECT_EQ(torus.wrapY(-1), 12u);
}
  
// ============================================================================
// 3.6  Проверка методов доступа
// ============================================================================
  
TEST(TorusTopology, Width_ReturnsConstructorValue)
{
    TorusTopology torus(25, 15);
    EXPECT_EQ(torus.width(), 25u);
}
  
TEST(TorusTopology, Height_ReturnsConstructorValue)
{
    TorusTopology torus(25, 15);
    EXPECT_EQ(torus.height(), 15u);
}
  
TEST_F(TorusTopology10x10, Width_ReturnsCorrectValue)
{
    EXPECT_EQ(torus.width(), 10u);
}
  
TEST_F(TorusTopology10x10, Height_ReturnsCorrectValue)
{
    EXPECT_EQ(torus.height(), 10u);
}