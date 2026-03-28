#include <gtest/gtest.h>
#include "Planes.h"

using namespace ntcap;

// ============================================================
// 1.1 Корректное создание
// ============================================================

TEST(Planes, Constructor_DefaultNumPlanes_CreatesOnePlaneAllZeros)
{
    Planes p(10, 20);

    EXPECT_EQ(p.width(), 10);
    EXPECT_EQ(p.height(), 20);
    EXPECT_EQ(p.size(), 200);
    EXPECT_EQ(p.view().size(), 200);

    for (size_t i = 0; i < p.size(); ++i) {
        EXPECT_EQ(p(0, i), 0) << "cell index: " << i;
    }
}

TEST(Planes, Constructor_MultiplePlanes_CreatesCorrectDimensionsAllZeros)
{
    Planes p(5, 5, 3);

    EXPECT_EQ(p.width(), 5);
    EXPECT_EQ(p.height(), 5);
    EXPECT_EQ(p.size(), 25);
    EXPECT_EQ(p.view().size(), 75);

    for (size_t plane = 0; plane < 3; ++plane) {
        for (size_t cell = 0; cell < 25; ++cell) {
            EXPECT_EQ(p(plane, cell), 0)
                << "plane: " << plane << ", cell: " << cell;
        }
    }
}

TEST(Planes, Constructor_MinimalDimensions_CreatesSingleCell)
{
    Planes p(1, 1, 1);

    EXPECT_EQ(p.width(), 1);
    EXPECT_EQ(p.height(), 1);
    EXPECT_EQ(p.size(), 1);
    EXPECT_EQ(p.view().size(), 1);
    EXPECT_EQ(p(0, 0), 0);
}

TEST(Planes, Constructor_WideRectangle_CorrectDimensions)
{
    Planes p(100, 1, 2);

    EXPECT_EQ(p.width(), 100);
    EXPECT_EQ(p.height(), 1);
    EXPECT_EQ(p.size(), 100);
    EXPECT_EQ(p.view().size(), 200);
}

TEST(Planes, Constructor_TallRectangle_CorrectDimensions)
{
    Planes p(1, 100, 2);

    EXPECT_EQ(p.width(), 1);
    EXPECT_EQ(p.height(), 100);
    EXPECT_EQ(p.size(), 100);
    EXPECT_EQ(p.view().size(), 200);
}

// ============================================================
// 1.2 Граничные и невалидные значения
// ============================================================

TEST(Planes, Constructor_ZeroWidth_ThrowsOrCreatesEmpty)
{
    EXPECT_ANY_THROW(Planes(0, 10, 1));
}

TEST(Planes, Constructor_ZeroHeight_ThrowsOrCreatesEmpty)
{
    EXPECT_ANY_THROW(Planes(10, 0, 1));
}

TEST(Planes, Constructor_ZeroNumPlanes_ThrowsOrCreatesEmpty)
{
    EXPECT_ANY_THROW(Planes(10, 10, 0));
}

// ============================================================
// 1.3 Инициализация нулями
// ============================================================

TEST(Planes, Constructor_MultiplePlanes_AllCellsInitializedToZero)
{
    Planes p(10, 10, 3);

    auto raw = p.view();
    ASSERT_EQ(raw.size(), 300);

    for (size_t i = 0; i < raw.size(); ++i) {
        EXPECT_EQ(raw[i], 0) << "raw index: " << i;
    }
}

// ============================================================
// 2.1 Корректный доступ
// ============================================================

TEST(Planes, CallOperator_FirstCellFirstPlane_ReturnsCorrectValue)
{
    Planes p(5, 5, 2);
    p.data()[0] = 11;

    EXPECT_EQ(p(0, 0), 11);
}

TEST(Planes, CallOperator_LastCellFirstPlane_ReturnsCorrectValue)
{
    Planes p(5, 5, 2);
    p.data()[24] = 77;

    EXPECT_EQ(p(0, 24), 77);
}

TEST(Planes, CallOperator_FirstCellSecondPlane_ReturnsCorrectValue)
{
    Planes p(5, 5, 2);
    p.data()[25] = 42;

    EXPECT_EQ(p(1, 0), 42);
}

TEST(Planes, CallOperator_LastCellLastPlane_ReturnsCorrectValue)
{
    Planes p(5, 5, 3);
    p.data()[74] = 99;

    EXPECT_EQ(p(2, 24), 99);
}

TEST(Planes, CallOperator_DifferentPlanesSameCell_ReturnsDifferentValues)
{
    Planes p(4, 4, 3);

    const size_t planeSize = p.size();
    p.data()[0 * planeSize + 5] = 10;
    p.data()[1 * planeSize + 5] = 20;
    p.data()[2 * planeSize + 5] = 30;

    EXPECT_EQ(p(0, 5), 10);
    EXPECT_EQ(p(1, 5), 20);
    EXPECT_EQ(p(2, 5), 30);
}

TEST(Planes, CallOperator_FullPlaneTraversal_AllCellsMatchRawData)
{
    Planes p(6, 4, 2);

    for (size_t i = 0; i < p.view().size(); ++i) {
        p.data()[i] = static_cast<uint8_t>(i % 256);
    }

    for (size_t plane = 0; plane < 2; ++plane) {
        for (size_t cell = 0; cell < p.size(); ++cell) {
            const size_t rawIndex = plane * p.size() + cell;
            EXPECT_EQ(p(plane, cell), static_cast<uint8_t>(rawIndex % 256))
                << "plane: " << plane << ", cell: " << cell;
        }
    }
}

// ============================================================
// 2.2 Граничные значения (noexcept — не бросает, но UB)
// ============================================================

TEST(Planes, CallOperator_PlaneIndexOutOfRange_NoThrow)
{
    Planes p(5, 5, 2);

    EXPECT_NO_THROW((void)p(2, 0));
}

TEST(Planes, CallOperator_CellIndexOutOfRange_NoThrow)
{
    Planes p(5, 5, 2);

    EXPECT_NO_THROW((void)p(0, 25));
}

// ============================================================
// 3.1 Корректная работа
// ============================================================

TEST(Planes, ViewByIndex_FirstPlane_SpanPointsToBeginning)
{
    Planes p(3, 3, 2);

    auto span = p.view(0);

    EXPECT_EQ(span.size(), 9);
    EXPECT_EQ(span.data(), p.data());
}

TEST(Planes, ViewByIndex_SecondPlane_SpanPointsToOffset)
{
    Planes p(3, 3, 2);

    auto span = p.view(1);

    EXPECT_EQ(span.size(), 9);
    EXPECT_EQ(span.data(), p.data() + 9);
}

TEST(Planes, ViewByIndex_LastPlane_SpanPointsToCorrectOffset)
{
    Planes p(4, 4, 5);

    auto span = p.view(4);

    EXPECT_EQ(span.size(), 16);
    EXPECT_EQ(span.data(), p.data() + 4 * 16);
}

TEST(Planes, ViewByIndex_DataMatchesCallOperator_AllCellsEqual)
{
    Planes p(5, 4, 3);

    for (size_t i = 0; i < p.view().size(); ++i) {
        p.data()[i] = static_cast<uint8_t>((i * 7 + 13) % 256);
    }

    for (size_t plane = 0; plane < 3; ++plane) {
        auto span = p.view(plane);
        for (size_t cell = 0; cell < p.size(); ++cell) {
            EXPECT_EQ(span[cell], p(plane, cell))
                << "plane: " << plane << ", cell: " << cell;
        }
    }
}

TEST(Planes, ViewByIndex_SpanSize_EqualsPlaneSize)
{
    Planes p(7, 3, 1);

    auto span = p.view(0);

    EXPECT_EQ(span.size(), p.size());
    EXPECT_EQ(span.size(), 21);
}

TEST(Planes, ViewByIndex_ModifiedData_ReflectedInSpan)
{
    Planes p(4, 4, 2);

    p.data()[0]  = 0xAA;
    p.data()[15] = 0xBB;
    p.data()[16] = 0xCC;
    p.data()[31] = 0xDD;

    EXPECT_EQ(p.view(0)[0],  0xAA);
    EXPECT_EQ(p.view(0)[15], 0xBB);
    EXPECT_EQ(p.view(1)[0],  0xCC);
    EXPECT_EQ(p.view(1)[15], 0xDD);
}

// ============================================================
// 3.2 Граничные значения
// ============================================================

TEST(Planes, ViewByIndex_PlaneIndexOutOfRange_NoThrow)
{
    Planes p(3, 3, 1);

    EXPECT_NO_THROW((void)p.view(1));
}

TEST(Planes, ViewByIndex_SinglePlaneZeroIndex_ReturnsValidSpan)
{
    Planes p(3, 3, 1);

    auto span = p.view(0);

    EXPECT_EQ(span.size(), 9);
    EXPECT_EQ(span.data(), p.data());
}

// ============================================================
// 4. Метод view() (без параметров)
// ============================================================

TEST(Planes, ViewAll_MultiplePlanes_SizeEqualsPlanesSizeTimesNumPlanes)
{
    Planes p(5, 5, 3);

    auto span = p.view();

    EXPECT_EQ(span.size(), 75);
}

TEST(Planes, ViewAll_PointerMatchesData_SameAddress)
{
    Planes p(5, 5, 3);

    auto span = p.view();

    EXPECT_EQ(span.data(), p.data());
}

TEST(Planes, ViewAll_ContentMatchesConcatenatedPlanes_AllBytesEqual)
{
    Planes p(4, 3, 3);

    for (size_t i = 0; i < p.view().size(); ++i) {
        p.data()[i] = static_cast<uint8_t>((i * 11 + 3) % 256);
    }

    auto raw = p.view();

    for (size_t plane = 0; plane < 3; ++plane) {
        auto planeSpan = p.view(plane);
        for (size_t cell = 0; cell < p.size(); ++cell) {
            const size_t rawIndex = plane * p.size() + cell;
            EXPECT_EQ(raw[rawIndex], planeSpan[cell])
                << "plane: " << plane << ", cell: " << cell;
        }
    }
}

TEST(Planes, ViewAll_SinglePlane_EqualsViewByZeroIndex)
{
    Planes p(5, 5, 1);

    auto all   = p.view();
    auto first = p.view(0);

    EXPECT_EQ(all.data(), first.data());
    EXPECT_EQ(all.size(), first.size());
}

TEST(Planes, ViewAll_ModifiedData_ReflectedInSpan)
{
    Planes p(3, 3, 2);

    p.data()[0]  = 0x11;
    p.data()[8]  = 0x22;
    p.data()[9]  = 0x33;
    p.data()[17] = 0x44;

    auto span = p.view();

    EXPECT_EQ(span[0],  0x11);
    EXPECT_EQ(span[8],  0x22);
    EXPECT_EQ(span[9],  0x33);
    EXPECT_EQ(span[17], 0x44);
}

TEST(Planes, ViewAll_AfterConstruction_AllZeros)
{
    Planes p(6, 6, 4);

    auto span = p.view();

    ASSERT_EQ(span.size(), 144);
    for (size_t i = 0; i < span.size(); ++i) {
        EXPECT_EQ(span[i], 0) << "index: " << i;
    }
}

// ============================================================
// 5.1 const версия data()
// ============================================================

TEST(Planes, DataConst_ValidPlanes_NotNull)
{
    const Planes p(5, 5, 1);

    EXPECT_NE(p.data(), nullptr);
}

TEST(Planes, DataConst_ValidPlanes_MatchesViewData)
{
    const Planes p(5, 5, 2);

    EXPECT_EQ(p.data(), p.view().data());
}

TEST(Planes, DataConst_MultiplePlanes_MatchesEachPlaneView)
{
    const Planes p(4, 4, 3);

    for (size_t plane = 0; plane < 3; ++plane) {
        EXPECT_EQ(p.data() + plane * p.size(), p.view(plane).data())
            << "plane: " << plane;
    }
}

// ============================================================
// 5.2 Мутабельная версия data()
// ============================================================

TEST(Planes, DataMutable_WriteSingleCell_ReflectedInCallOperator)
{
    Planes p(5, 5, 1);

    p.data()[3] = 42;

    EXPECT_EQ(p(0, 3), 42);
}

TEST(Planes, DataMutable_WriteSecondPlane_ReflectedInView)
{
    Planes p(5, 5, 2);

    p.data()[30] = 7;

    EXPECT_EQ(p.view(1)[5], 7);
}

TEST(Planes, DataMutable_OverwriteAllCells_AllReflectedInCallOperator)
{
    Planes p(4, 3, 2);

    const size_t total = p.view().size();
    for (size_t i = 0; i < total; ++i) {
        p.data()[i] = 0xFF;
    }

    for (size_t plane = 0; plane < 2; ++plane) {
        for (size_t cell = 0; cell < p.size(); ++cell) {
            EXPECT_EQ(p(plane, cell), 255)
                << "plane: " << plane << ", cell: " << cell;
        }
    }
}

TEST(Planes, DataMutable_WritePattern_ReflectedInViewAll)
{
    Planes p(3, 3, 3);

    const size_t total = p.view().size();
    for (size_t i = 0; i < total; ++i) {
        p.data()[i] = static_cast<uint8_t>(i % 256);
    }

    auto span = p.view();
    for (size_t i = 0; i < total; ++i) {
        EXPECT_EQ(span[i], static_cast<uint8_t>(i % 256))
            << "index: " << i;
    }
}

TEST(Planes, DataMutable_ConstAndMutablePointToSameMemory_SameAddress)
{
    Planes p(5, 5, 1);

    uint8_t*       mutablePtr = p.data();
    const uint8_t* constPtr   = std::as_const(p).data();

    EXPECT_EQ(static_cast<const uint8_t*>(mutablePtr), constPtr);
}

// ============================================================
// 6. Геттеры width(), height(), size()
// ============================================================

TEST(Planes, Width_RectangularPlane_ReturnsCorrectValue)
{
    Planes p(7, 3, 2);

    EXPECT_EQ(p.width(), 7);
}

TEST(Planes, Height_RectangularPlane_ReturnsCorrectValue)
{
    Planes p(7, 3, 2);

    EXPECT_EQ(p.height(), 3);
}

TEST(Planes, Size_RectangularPlane_EqualsWidthTimesHeight)
{
    Planes p(7, 3, 2);

    EXPECT_EQ(p.size(), 21);
    EXPECT_EQ(p.size(), p.width() * p.height());
}

TEST(Planes, Size_DifferentNumPlanes_DoesNotDependOnPlaneCount)
{
    Planes p1(4, 4, 1);
    Planes p2(4, 4, 5);
    Planes p3(4, 4, 10);

    EXPECT_EQ(p1.size(), 16);
    EXPECT_EQ(p2.size(), 16);
    EXPECT_EQ(p3.size(), 16);
}

TEST(Planes, Getters_SquarePlane_AllConsistent)
{
    Planes p(8, 8, 1);

    EXPECT_EQ(p.width(), 8);
    EXPECT_EQ(p.height(), 8);
    EXPECT_EQ(p.size(), 64);
    EXPECT_EQ(p.size(), p.width() * p.height());
}

TEST(Planes, Getters_MinimalDimensions_AllReturnOne)
{
    Planes p(1, 1, 1);

    EXPECT_EQ(p.width(), 1);
    EXPECT_EQ(p.height(), 1);
    EXPECT_EQ(p.size(), 1);
}

TEST(Planes, Getters_WidePlane_WidthGreaterThanHeight)
{
    Planes p(100, 1, 3);

    EXPECT_EQ(p.width(), 100);
    EXPECT_EQ(p.height(), 1);
    EXPECT_EQ(p.size(), 100);
}

TEST(Planes, Getters_TallPlane_HeightGreaterThanWidth)
{
    Planes p(1, 100, 3);

    EXPECT_EQ(p.width(), 1);
    EXPECT_EQ(p.height(), 100);
    EXPECT_EQ(p.size(), 100);
}

TEST(Planes, Getters_MultipleInstances_IndependentValues)
{
    Planes a(3, 7, 2);
    Planes b(10, 20, 4);

    EXPECT_EQ(a.width(), 3);
    EXPECT_EQ(a.height(), 7);
    EXPECT_EQ(a.size(), 21);

    EXPECT_EQ(b.width(), 10);
    EXPECT_EQ(b.height(), 20);
    EXPECT_EQ(b.size(), 200);
}

// ============================================================
// 7. Согласованность данных между методами
// ============================================================

TEST(Planes, Consistency_ViewAllSize_EqualsSizeTimesNumPlanes)
{
    constexpr size_t numPlanes = 4;
    Planes p(6, 5, numPlanes);

    EXPECT_EQ(p.view().size(), p.size() * numPlanes);
}

TEST(Planes, Consistency_ViewAllSize_DifferentConfigurations_AlwaysCorrect)
{
    struct Config { size_t w; size_t h; size_t n; };
    const Config configs[] = {
        {1, 1, 1}, {3, 7, 2}, {10, 10, 5}, {100, 1, 3}, {1, 100, 8}
    };

    for (const auto& [w, h, n] : configs) {
        Planes p(w, h, n);
        EXPECT_EQ(p.view().size(), p.size() * n)
            << "w=" << w << ", h=" << h << ", n=" << n;
    }
}

TEST(Planes, Consistency_ViewByIndexSize_EqualsPlaneSizeForAll)
{
    constexpr size_t numPlanes = 5;
    Planes p(4, 3, numPlanes);

    for (size_t i = 0; i < numPlanes; ++i) {
        EXPECT_EQ(p.view(i).size(), p.size())
            << "plane: " << i;
    }
}

TEST(Planes, Consistency_ViewByIndexOffset_EqualsDataPlusIndexTimesSize)
{
    constexpr size_t numPlanes = 4;
    Planes p(5, 6, numPlanes);

    for (size_t i = 0; i < numPlanes; ++i) {
        EXPECT_EQ(p.view(i).data(), p.data() + i * p.size())
            << "plane: " << i;
    }
}

TEST(Planes, Consistency_ViewByIndexElement_EqualsCallOperator)
{
    constexpr size_t numPlanes = 3;
    Planes p(5, 4, numPlanes);

    for (size_t i = 0; i < p.view().size(); ++i) {
        p.data()[i] = static_cast<uint8_t>((i * 13 + 7) % 256);
    }

    for (size_t plane = 0; plane < numPlanes; ++plane) {
        auto span = p.view(plane);
        for (size_t cell = 0; cell < p.size(); ++cell) {
            EXPECT_EQ(span[cell], p(plane, cell))
                << "plane: " << plane << ", cell: " << cell;
        }
    }
}

TEST(Planes, Consistency_ViewAllElement_EqualsCallOperatorLinearIndex)
{
    constexpr size_t numPlanes = 3;
    Planes p(4, 5, numPlanes);

    for (size_t i = 0; i < p.view().size(); ++i) {
        p.data()[i] = static_cast<uint8_t>((i * 17 + 11) % 256);
    }

    auto raw = p.view();
    for (size_t plane = 0; plane < numPlanes; ++plane) {
        for (size_t cell = 0; cell < p.size(); ++cell) {
            const size_t linearIndex = plane * p.size() + cell;
            EXPECT_EQ(raw[linearIndex], p(plane, cell))
                << "plane: " << plane << ", cell: " << cell;
        }
    }
}

TEST(Planes, Consistency_AllAccessMethods_ReturnSameValue)
{
    constexpr size_t numPlanes = 3;
    Planes p(5, 5, numPlanes);

    for (size_t i = 0; i < p.view().size(); ++i) {
        p.data()[i] = static_cast<uint8_t>((i * 31 + 5) % 256);
    }

    for (size_t plane = 0; plane < numPlanes; ++plane) {
        for (size_t cell = 0; cell < p.size(); ++cell) {
            const size_t rawIndex          = plane * p.size() + cell;
            const uint8_t viaOperator      = p(plane, cell);
            const uint8_t viaViewByIndex   = p.view(plane)[cell];
            const uint8_t viaViewAll       = p.view()[rawIndex];
            const uint8_t viaData          = p.data()[rawIndex];

            EXPECT_EQ(viaOperator, viaViewByIndex)
                << "plane: " << plane << ", cell: " << cell;
            EXPECT_EQ(viaOperator, viaViewAll)
                << "plane: " << plane << ", cell: " << cell;
            EXPECT_EQ(viaOperator, viaData)
                << "plane: " << plane << ", cell: " << cell;
        }
    }
}

