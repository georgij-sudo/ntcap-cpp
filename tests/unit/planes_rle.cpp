#include <gtest/gtest.h>
#include "PlanesRleConverter.h"
#include "Planes.h"
#include <cstring>
#include <sstream>
#include <vector>
#include <cstdint>
#include <algorithm>

using namespace ntcap;

// ─── helpers ────────────────────────────────────────────────────────

namespace {

void fillPattern(Planes& p, size_t planeIdx, uint8_t seed) {
    const size_t sz = p.size();
    uint8_t* dst = p.data() + planeIdx * sz;
    for (size_t i = 0; i < sz; ++i)
        dst[i] = static_cast<uint8_t>((seed + i * 7 + i / 256) & 0xFF);
}

void expectPlanesEqual(const Planes& a, const Planes& b) {
    ASSERT_EQ(a.width(),     b.width());
    ASSERT_EQ(a.height(),    b.height());
    ASSERT_EQ(a.numPlanes(), b.numPlanes());
    const size_t total = a.size() * a.numPlanes();
    ASSERT_EQ(std::memcmp(a.data(), b.data(), total), 0);
}

// little-endian writers for building reference blobs
void pushLE64(std::vector<uint8_t>& v, uint64_t x) {
    for (int i = 0; i < 8; ++i) { v.push_back(x & 0xFF); x >>= 8; }
}
void pushLE32(std::vector<uint8_t>& v, uint32_t x) {
    for (int i = 0; i < 4; ++i) { v.push_back(x & 0xFF); x >>= 8; }
}
void pushRun(std::vector<uint8_t>& v, uint8_t val, uint32_t cnt) {
    v.push_back(val);
    pushLE32(v, cnt);
}
void pushHeader(std::vector<uint8_t>& v,
                uint64_t w, uint64_t h, uint64_t np) {
    v.push_back('P'); v.push_back('R'); v.push_back('L'); v.push_back('E');
    v.push_back(1);
    pushLE64(v, w);
    pushLE64(v, h);
    pushLE64(v, np);
}

} // anon

// ═══════════════════════════════════════════════════════════════════
// 1.  Roundtrip  1 × 511 × 512
// ═══════════════════════════════════════════════════════════════════
TEST(PlanesRleConverter, Roundtrip_Plane1x511x512_DataPreserved) {
    Planes src(511, 512, 1);
    fillPattern(src, 0, 42);

    auto blob = PlanesRleConverter::serialize(src);
    Planes dst = PlanesRleConverter::deserialize(blob);

    expectPlanesEqual(src, dst);
}

// ═══════════════════════════════════════════════════════════════════
// 2.  Roundtrip  2 × 256 × 256
// ═══════════════════════════════════════════════════════════════════
TEST(PlanesRleConverter, Roundtrip_Plane2x256x256_DataPreserved) {
    Planes src(256, 256, 2);
    fillPattern(src, 0, 0);
    fillPattern(src, 1, 99);

    auto blob = PlanesRleConverter::serialize(src);
    Planes dst = PlanesRleConverter::deserialize(blob);

    expectPlanesEqual(src, dst);
}

// ═══════════════════════════════════════════════════════════════════
// 3.  Roundtrip  3 × 111 × 113
// ═══════════════════════════════════════════════════════════════════
TEST(PlanesRleConverter, Roundtrip_Plane3x111x113_DataPreserved) {
    Planes src(111, 113, 3);
    fillPattern(src, 0, 10);
    fillPattern(src, 1, 20);
    fillPattern(src, 2, 30);

    auto blob = PlanesRleConverter::serialize(src);
    Planes dst = PlanesRleConverter::deserialize(blob);

    expectPlanesEqual(src, dst);
}

// ═══════════════════════════════════════════════════════════════════
// 4.  Roundtrip  2 × 100 × 201  — 256 distinct runs in plane 0
// ═══════════════════════════════════════════════════════════════════
TEST(PlanesRleConverter, Roundtrip_PlaneWith256DistinctRuns_DataPreserved) {
    const size_t W = 100, H = 201, NP = 2;
    const size_t planeSize = W * H;            // 20100

    Planes src(W, H, NP);

    // plane 0 — ровно 256 ранов: значения 0..255,
    // длина каждого рана = planeSize/256 (= 78), остаток раскидываем по первым
    {
        uint8_t* d = src.data();
        const size_t base  = planeSize / 256;   // 78
        const size_t extra = planeSize % 256;   // 132
        size_t pos = 0;
        for (size_t r = 0; r < 256; ++r) {
            size_t len = base + (r < extra ? 1 : 0);
            std::memset(d + pos, static_cast<uint8_t>(r), len);
            pos += len;
        }
    }
    // plane 1 — простая заливка
    fillPattern(src, 1, 77);

    auto blob = PlanesRleConverter::serialize(src);
    Planes dst = PlanesRleConverter::deserialize(blob);

    expectPlanesEqual(src, dst);
}

// ═══════════════════════════════════════════════════════════════════
// 5.  Serialize → сравнение с эталонным blob   (2 × 2 × 51)
//     plane0 : все 0x00  → 1 run  (0x00, 102)
//     plane1 : 50× 0xAA + 52× 0xBB  → 2 runs
// ═══════════════════════════════════════════════════════════════════
TEST(PlanesRleConverter, Serialize_Plane2x2x51_MatchesConstant) {
    const size_t W = 2, H = 51, NP = 2;
    const size_t planeSize = W * H;             // 102

    Planes src(W, H, NP);
    // plane 0 — уже нули
    // plane 1
    {
        uint8_t* d = src.data() + planeSize;
        std::memset(d,      0xAA, 50);
        std::memset(d + 50, 0xBB, 52);
    }

    // ── строим эталон ──
    std::vector<uint8_t> expected;
    pushHeader(expected, W, H, NP);
    // plane 0: 1 run
    pushLE32(expected, 1);
    pushRun(expected, 0x00, 102);
    // plane 1: 2 runs
    pushLE32(expected, 2);
    pushRun(expected, 0xAA, 50);
    pushRun(expected, 0xBB, 52);

    auto actual = PlanesRleConverter::serialize(src);

    ASSERT_EQ(actual.size(), expected.size());
    EXPECT_EQ(actual, expected);
}

// ═══════════════════════════════════════════════════════════════════
// 6.  Serialize → эталон   (1 × 128 × 128)
//     первая половина 0xFF, вторая 0x00  → 2 runs
// ═══════════════════════════════════════════════════════════════════
TEST(PlanesRleConverter, Serialize_Plane1x128x128_MatchesConstant) {
    const size_t W = 128, H = 128, NP = 1;
    const size_t planeSize = W * H;             // 16384
    const size_t half = planeSize / 2;          // 8192

    Planes src(W, H, NP);
    std::memset(src.data(),        0xFF, half);
    std::memset(src.data() + half, 0x00, half);

    std::vector<uint8_t> expected;
    pushHeader(expected, W, H, NP);
    // plane 0: 2 runs
    pushLE32(expected, 2);
    pushRun(expected, 0xFF, static_cast<uint32_t>(half));
    pushRun(expected, 0x00, static_cast<uint32_t>(half));

    auto actual = PlanesRleConverter::serialize(src);

    ASSERT_EQ(actual.size(), expected.size());
    EXPECT_EQ(actual, expected);
}

// ═══════════════════════════════════════════════════════════════════
// 7.  Deserialize из эталонного blob   (2 × 51 × 2)
//     plane0 : все 0xAA  → 1 run
//     plane1 : 60× 0xCC + 42× 0xDD  → 2 runs
// ═══════════════════════════════════════════════════════════════════
TEST(PlanesRleConverter, Deserialize_Plane2x51x2_MatchesConstant) {
    const size_t W = 51, H = 2, NP = 2;
    const size_t planeSize = W * H;             // 102

    // ── строим blob ──
    std::vector<uint8_t> blob;
    pushHeader(blob, W, H, NP);
    // plane 0
    pushLE32(blob, 1);
    pushRun(blob, 0xAA, static_cast<uint32_t>(planeSize));
    // plane 1
    pushLE32(blob, 2);
    pushRun(blob, 0xCC, 60);
    pushRun(blob, 0xDD, 42);

    Planes p = PlanesRleConverter::deserialize(blob);

    ASSERT_EQ(p.width(),     W);
    ASSERT_EQ(p.height(),    H);
    ASSERT_EQ(p.numPlanes(), NP);

    // plane 0
    {
        auto v = p.view(0);
        EXPECT_TRUE(std::all_of(v.begin(), v.end(),
                    [](uint8_t b){ return b == 0xAA; }));
    }
    // plane 1
    {
        auto v = p.view(1);
        for (size_t i = 0;  i < 60; ++i)
            EXPECT_EQ(v[i], 0xCC) << "i=" << i;
        for (size_t i = 60; i < planeSize; ++i)
            EXPECT_EQ(v[i], 0xDD) << "i=" << i;
    }
}

// ═══════════════════════════════════════════════════════════════════
// 8.  Deserialize из эталонного blob   (1 × 52 × 99)
//     plane0 : 2574× 0x10 + 2574× 0x20  → 2 runs
// ═══════════════════════════════════════════════════════════════════
TEST(PlanesRleConverter, Deserialize_Plane1x52x99_MatchesConstant) {
    const size_t W = 52, H = 99, NP = 1;
    const size_t planeSize = W * H;             // 5148
    const size_t half = planeSize / 2;          // 2574

    std::vector<uint8_t> blob;
    pushHeader(blob, W, H, NP);
    pushLE32(blob, 2);
    pushRun(blob, 0x10, static_cast<uint32_t>(half));
    pushRun(blob, 0x20, static_cast<uint32_t>(half));

    Planes p = PlanesRleConverter::deserialize(blob);

    ASSERT_EQ(p.width(),     W);
    ASSERT_EQ(p.height(),    H);
    ASSERT_EQ(p.numPlanes(), NP);

    auto v = p.view(0);
    for (size_t i = 0;    i < half;      ++i)
        EXPECT_EQ(v[i], 0x10) << "i=" << i;
    for (size_t i = half;  i < planeSize; ++i)
        EXPECT_EQ(v[i], 0x20) << "i=" << i;
}

// ═══════════════════════════════════════════════════════════════════
//  Негативные тесты — повреждение заголовка
// ═══════════════════════════════════════════════════════════════════

TEST(PlanesRleConverter, Deserialize_EmptyData_Throws) {
    std::vector<uint8_t> blob;
    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

TEST(PlanesRleConverter, Deserialize_TruncatedHeader_Throws) {
    // 28 байт — на 1 меньше минимального заголовка (29)
    std::vector<uint8_t> blob;
    pushHeader(blob, 10, 10, 1);
    blob.pop_back();                              // 28 байт
    ASSERT_EQ(blob.size(), 28u);

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

TEST(PlanesRleConverter, Deserialize_WrongMagic_Throws) {
    std::vector<uint8_t> blob;
    pushHeader(blob, 1, 1, 1);
    // plane 0: 1 run (0x00, 1)
    pushLE32(blob, 1);
    pushRun(blob, 0x00, 1);
    // портим magic
    blob[0] = 'X';

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

TEST(PlanesRleConverter, Deserialize_WrongVersion2_Throws) {
    std::vector<uint8_t> blob;
    pushHeader(blob, 1, 1, 1);
    pushLE32(blob, 1);
    pushRun(blob, 0x00, 1);
    // портим version
    blob[4] = 2;

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

TEST(PlanesRleConverter, Deserialize_VersionZero_Throws) {
    std::vector<uint8_t> blob;
    pushHeader(blob, 1, 1, 1);
    pushLE32(blob, 1);
    pushRun(blob, 0x00, 1);
    blob[4] = 0;

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

// ═══════════════════════════════════════════════════════════════════
//  Негативные тесты — повреждение данных плоскостей
// ═══════════════════════════════════════════════════════════════════

TEST(PlanesRleConverter, Deserialize_TruncatedBeforeNumRuns_Throws) {
    // корректный заголовок, но данных плоскости нет совсем
    std::vector<uint8_t> blob;
    pushHeader(blob, 4, 4, 1);                   // ожидается plane size=16
    // ничего после заголовка

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

TEST(PlanesRleConverter, Deserialize_TruncatedInsideRuns_Throws) {
    // numRuns=2, но в blob данные только для 1 run
    std::vector<uint8_t> blob;
    pushHeader(blob, 5, 5, 1);                   // plane size=25
    pushLE32(blob, 2);                            // numRuns = 2
    pushRun(blob, 0xAA, 10);                      // только 1 run
    // второй run отсутствует

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

TEST(PlanesRleConverter, Deserialize_RunCountSumLessThanPlaneSize_Throws) {
    // plane size = 100, но сумма count = 90
    std::vector<uint8_t> blob;
    pushHeader(blob, 10, 10, 1);
    pushLE32(blob, 2);
    pushRun(blob, 0x01, 50);
    pushRun(blob, 0x02, 40);                      // 50+40=90 < 100

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

TEST(PlanesRleConverter, Deserialize_RunCountSumGreaterThanPlaneSize_Throws) {
    // plane size = 100, но сумма count = 110
    std::vector<uint8_t> blob;
    pushHeader(blob, 10, 10, 1);
    pushLE32(blob, 2);
    pushRun(blob, 0x01, 60);
    pushRun(blob, 0x02, 50);                      // 60+50=110 > 100

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

TEST(PlanesRleConverter, Deserialize_ZeroNumRunsNonZeroPlane_Throws) {
    // plane size = 25, но numRuns = 0
    std::vector<uint8_t> blob;
    pushHeader(blob, 5, 5, 1);
    pushLE32(blob, 0);                            // numRuns = 0, сумма count = 0 ≠ 25

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

TEST(PlanesRleConverter, Deserialize_SecondPlaneTruncated_Throws) {
    // две плоскости: первая корректна, вторая оборвана
    std::vector<uint8_t> blob;
    pushHeader(blob, 3, 3, 2);                    // plane size = 9
    // plane 0 — ОК
    pushLE32(blob, 1);
    pushRun(blob, 0x00, 9);
    // plane 1 — numRuns есть, но run-ы оборваны
    pushLE32(blob, 2);
    pushRun(blob, 0xFF, 5);
    // второй run отсутствует — 5 ≠ 9

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

// ═══════════════════════════════════════════════════════════════════
//  Негативные тесты — граничные размеры
// ═══════════════════════════════════════════════════════════════════

TEST(PlanesRleConverter, Deserialize_WidthZero_Throws) {
    std::vector<uint8_t> blob;
    pushHeader(blob, 0, 100, 1);

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

TEST(PlanesRleConverter, Deserialize_HeightZero_Throws) {
    std::vector<uint8_t> blob;
    pushHeader(blob, 100, 0, 1);

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

TEST(PlanesRleConverter, Deserialize_NumPlanesZero_Throws) {
    std::vector<uint8_t> blob;
    pushHeader(blob, 10, 10, 0);

    EXPECT_THROW(PlanesRleConverter::deserialize(blob), PlanesRleConverterException);
}

// ═══════════════════════════════════════════════════════════════════
//  Негативные тесты — проверка offset в исключении
// ═══════════════════════════════════════════════════════════════════

TEST(PlanesRleConverter, Deserialize_WrongMagic_ExceptionOffsetIsZero) {
    std::vector<uint8_t> blob;
    pushHeader(blob, 1, 1, 1);
    pushLE32(blob, 1);
    pushRun(blob, 0x00, 1);
    blob[0] = 'Z';

    try {
        PlanesRleConverter::deserialize(blob);
        FAIL() << "Expected PlanesRleConverterException";
    } catch (const PlanesRleConverterException& e) {
        EXPECT_EQ(e.offset(), 0u);
        EXPECT_FALSE(std::string(e.what()).empty());
        EXPECT_FALSE(e.message().empty());
    }
}

TEST(PlanesRleConverter, Deserialize_TruncatedInsideRuns_ExceptionOffsetCorrect) {
    // header 29 + numRuns 4 + 1 run 5 = 38 байт в blob
    // ожидается второй run начиная с offset 38
    std::vector<uint8_t> blob;
    pushHeader(blob, 5, 5, 1);                   // 29
    pushLE32(blob, 2);                            // 33
    pushRun(blob, 0xAA, 10);                      // 38
    // второй run отсутствует → ошибка на offset ≥ 38

    try {
        PlanesRleConverter::deserialize(blob);
        FAIL() << "Expected PlanesRleConverterException";
    } catch (const PlanesRleConverterException& e) {
        EXPECT_GE(e.offset(), 38u);
        EXPECT_FALSE(e.message().empty());
    }
}
