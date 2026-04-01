#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstdint>

#include "PlanesCsvConverter.h"
#include "Planes.h"

using namespace ntcap;

// ============================================================================
// Helpers
// ============================================================================

namespace {

/// Детерминистично заполняет Planes значениями (planeIdx * 97 + cellIdx * 31 + 17) % 256
Planes makeDeterministicPlanes(size_t width, size_t height, size_t numPlanes) {
    Planes planes(width, height, numPlanes);
    const size_t planeSize = width * height;
    for (size_t p = 0; p < numPlanes; ++p) {
        for (size_t c = 0; c < planeSize; ++c) {
            planes.data()[p * planeSize + c] =
                static_cast<uint8_t>((p * 97 + c * 31 + 17) % 256);
        }
    }
    return planes;
}

/// Строит CSV-строку из заданных параметров и лямбды-генератора значений
std::string buildCsv(size_t width, size_t height, size_t numPlanes,
                     auto valueFn, const std::string& lineEnding = "\n") {
    std::ostringstream out;
    out << width << "," << height << "," << numPlanes << lineEnding;
    const size_t planeSize = width * height;
    for (size_t p = 0; p < numPlanes; ++p) {
        if (p > 0) {
            out << lineEnding;  // пустая строка-разделитель
        }
        for (size_t row = 0; row < height; ++row) {
            for (size_t col = 0; col < width; ++col) {
                if (col > 0) out << ",";
                size_t cellIdx = row * width + col;
                out << valueFn(p, cellIdx);
            }
            out << lineEnding;
        }
    }
    return out.str();
}

/// Сравнивает два Planes поэлементно
void expectPlanesEqual(const Planes& actual, const Planes& expected) {
    ASSERT_EQ(actual.width(), expected.width());
    ASSERT_EQ(actual.height(), expected.height());
    ASSERT_EQ(actual.numPlanes(), expected.numPlanes());
    ASSERT_EQ(actual.view().size(), expected.view().size());

    const size_t numPlanes = expected.view().size() / expected.size();
    for (size_t p = 0; p < numPlanes; ++p) {
        for (size_t c = 0; c < expected.size(); ++c) {
            EXPECT_EQ(actual(p, c), expected(p, c))
                << "Mismatch at plane=" << p << " cell=" << c;
        }
    }
}

} // anonymous namespace

// ============================================================================
// 1. Serialize + Deserialize (round-trip)
// ============================================================================

TEST(PlanesCsvConverter,
     SerializeDeserialize_OnePlane1200x999_RoundTripPreservesData) {
    const auto original = makeDeterministicPlanes(1200, 999, 1);

    const std::string csv = PlanesCsvConverter::serialize(original);
    const Planes restored = PlanesCsvConverter::deserialize(csv);

    expectPlanesEqual(restored, original);
}

TEST(PlanesCsvConverter,
     SerializeDeserialize_TwoPlanes500x700_RoundTripPreservesData) {
    const auto original = makeDeterministicPlanes(500, 700, 2);

    const std::string csv = PlanesCsvConverter::serialize(original);
    const Planes restored = PlanesCsvConverter::deserialize(csv);

    expectPlanesEqual(restored, original);
}

TEST(PlanesCsvConverter,
     SerializeDeserialize_ThreePlanes200x172_RoundTripPreservesData) {
    const auto original = makeDeterministicPlanes(200, 172, 3);

    const std::string csv = PlanesCsvConverter::serialize(original);
    const Planes restored = PlanesCsvConverter::deserialize(csv);

    expectPlanesEqual(restored, original);
}

// ============================================================================
// 2. Deserialize
// ============================================================================

TEST(PlanesCsvConverter,
     Deserialize_ValidConstantCsv20x15OnePlane_CorrectValues) {
    constexpr size_t W = 20, H = 15, N = 1;

    auto valueFn = [](size_t p, size_t c) -> int {
        return static_cast<int>((p * 53 + c * 7 + 3) % 256);
    };

    const std::string csv = buildCsv(W, H, N, valueFn);

    const Planes planes = PlanesCsvConverter::deserialize(csv);

    ASSERT_EQ(planes.width(), W);
    ASSERT_EQ(planes.height(), H);
    ASSERT_EQ(planes.numPlanes(), N);

    for (size_t c = 0; c < W * H; ++c) {
        EXPECT_EQ(planes(0, c), static_cast<uint8_t>(valueFn(0, c)))
            << "Mismatch at cell=" << c;
    }
}

TEST(PlanesCsvConverter,
     Deserialize_ValidConstantCsvWindowsLineEndings15x16_CorrectValues) {
    constexpr size_t W = 15, H = 16, N = 1;

    auto valueFn = [](size_t p, size_t c) -> int {
        return static_cast<int>((p * 41 + c * 13 + 5) % 256);
    };

    const std::string csv = buildCsv(W, H, N, valueFn, "\r\n");

    const Planes planes = PlanesCsvConverter::deserialize(csv);

    ASSERT_EQ(planes.width(), W);
    ASSERT_EQ(planes.height(), H);
    ASSERT_EQ(planes.numPlanes(), N);

    for (size_t c = 0; c < W * H; ++c) {
        EXPECT_EQ(planes(0, c), static_cast<uint8_t>(valueFn(0, c)))
            << "Mismatch at cell=" << c;
    }
}

TEST(PlanesCsvConverter,
     Deserialize_ValueEquals300_ThrowsException) {
    // 1 plane 10×10, все нули, кроме одной ячейки = 300
    constexpr size_t W = 10, H = 10, N = 1;

    auto valueFn = [](size_t /*p*/, size_t c) -> int {
        // последняя ячейка = 300
        return (c == W * H - 1) ? 300 : 0;
    };

    const std::string csv = buildCsv(W, H, N, valueFn);

    EXPECT_THROW(PlanesCsvConverter::deserialize(csv), PlanesCsvConverterException);
}

TEST(PlanesCsvConverter,
     Deserialize_ValueEqualsNegativeOne_ThrowsException) {
    constexpr size_t W = 10, H = 10, N = 1;

    auto valueFn = [](size_t /*p*/, size_t c) -> int {
        return (c == 0) ? -1 : 0;
    };

    const std::string csv = buildCsv(W, H, N, valueFn);

    EXPECT_THROW(PlanesCsvConverter::deserialize(csv), PlanesCsvConverterException);
}

TEST(PlanesCsvConverter,
     Deserialize_InsufficientData_ThrowsException) {
    // Заголовок заявляет 10×10×1 = 100 значений, но даём только 2 строки
    const std::string csv =
        "10,10,1\n"
        "0,0,0,0,0,0,0,0,0,0\n"
        "0,0,0,0,0,0,0,0,0,0\n";

    EXPECT_THROW(PlanesCsvConverter::deserialize(csv), PlanesCsvConverterException);
}

TEST(PlanesCsvConverter,
     Deserialize_ExcessData_ThrowsException) {
    // Заголовок заявляет 2×2×1 = 4 значения, но даём 3 строки по 2
    const std::string csv =
        "2,2,1\n"
        "0,0\n"
        "0,0\n"
        "0,0\n";

    EXPECT_THROW(PlanesCsvConverter::deserialize(csv), PlanesCsvConverterException);
}

TEST(PlanesCsvConverter,
     Deserialize_EmptyCsv_ThrowsException) {
    const std::string csv;

    EXPECT_THROW(PlanesCsvConverter::deserialize(csv), PlanesCsvConverterException);
}

TEST(PlanesCsvConverter,
     Deserialize_InvalidHeader_ThrowsException) {
    const std::string csv = "abc,def,ghi\n1,2\n3,4\n";

    EXPECT_THROW(PlanesCsvConverter::deserialize(csv), PlanesCsvConverterException);
}

TEST(PlanesCsvConverter,
     Deserialize_NoEmptyLinesBetweenPlanes_CorrectValues) {
    // 2×3, 2 planes — без пустой строки-разделителя между плоскостями
    const std::string csv =
        "2,3,2\n"
        "10,20\n"
        "30,40\n"
        "50,60\n"
        "70,80\n"
        "90,100\n"
        "110,120\n";

    const Planes planes = PlanesCsvConverter::deserialize(csv);

    ASSERT_EQ(planes.width(), 2u);
    ASSERT_EQ(planes.height(), 3u);
    ASSERT_EQ(planes.numPlanes(), 2u);

    // plane 0
    EXPECT_EQ(planes(0, 0), 10);
    EXPECT_EQ(planes(0, 1), 20);
    EXPECT_EQ(planes(0, 2), 30);
    EXPECT_EQ(planes(0, 3), 40);
    EXPECT_EQ(planes(0, 4), 50);
    EXPECT_EQ(planes(0, 5), 60);

    // plane 1
    EXPECT_EQ(planes(1, 0), 70);
    EXPECT_EQ(planes(1, 1), 80);
    EXPECT_EQ(planes(1, 2), 90);
    EXPECT_EQ(planes(1, 3), 100);
    EXPECT_EQ(planes(1, 4), 110);
    EXPECT_EQ(planes(1, 5), 120);
}

TEST(PlanesCsvConverter,
     Deserialize_WithAndWithoutEmptyLines_SameResult) {
    // Один и тот же набор данных в двух форматах
    const std::string csvWithSeparator =
        "2,2,2\n"
        "1,2\n"
        "3,4\n"
        "\n"
        "5,6\n"
        "7,8\n";

    const std::string csvWithoutSeparator =
        "2,2,2\n"
        "1,2\n"
        "3,4\n"
        "5,6\n"
        "7,8\n";

    const Planes withSep    = PlanesCsvConverter::deserialize(csvWithSeparator);
    const Planes withoutSep = PlanesCsvConverter::deserialize(csvWithoutSeparator);

    ASSERT_EQ(withSep.width(), 2u);
    ASSERT_EQ(withSep.width(),  withoutSep.width());
    
    ASSERT_EQ(withSep.height(), 2u);
    ASSERT_EQ(withSep.height(), withoutSep.height());

    ASSERT_EQ(withSep.numPlanes(), 2u);
    ASSERT_EQ(withSep.numPlanes(), withoutSep.numPlanes());

    const size_t numPlanes = 2;
    for (size_t p = 0; p < numPlanes; ++p) {
        for (size_t c = 0; c < withSep.size(); ++c) {
            EXPECT_EQ(withSep(p, c), withoutSep(p, c))
                << "Mismatch at plane=" << p << " cell=" << c;
        }
    }
}

TEST(PlanesCsvConverter,
     Deserialize_TrailingEmptyLine_CorrectValues) {
    const std::string csvNoTrailing =
        "2,2,1\n"
        "10,20\n"
        "30,40\n";

    const std::string csvWithTrailing =
        "2,2,1\n"
        "10,20\n"
        "30,40\n"
        "\n";

    const Planes noTrailing   = PlanesCsvConverter::deserialize(csvNoTrailing);
    const Planes withTrailing = PlanesCsvConverter::deserialize(csvWithTrailing);

    ASSERT_EQ(withTrailing.width(), 2u);
    ASSERT_EQ(withTrailing.width(),  noTrailing.width());

    ASSERT_EQ(withTrailing.height(), 2u);
    ASSERT_EQ(withTrailing.height(), noTrailing.height());

    ASSERT_EQ(withTrailing.numPlanes(), 1u);
    ASSERT_EQ(withTrailing.numPlanes(), noTrailing.numPlanes());

    for (size_t c = 0; c < noTrailing.size(); ++c) {
        EXPECT_EQ(withTrailing(0, c), noTrailing(0, c))
            << "Mismatch at cell=" << c;
    }
}
