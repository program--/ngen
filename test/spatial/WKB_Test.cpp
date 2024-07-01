
#include "gtest/gtest.h"
#include <gtest/gtest.h>

#include <spatial/io/wkb.hpp>
#include <spatial/io/wkt.hpp>

#include <boost/geometry/algorithms/equals.hpp>

#include "WKB_TestData.hpp"
#include "spatial/Geometry.hpp"

#include "SpatialTestHelpers.hpp"

struct WKBTestParam {
    const char*                name;
    const char*                wkt;
    boost::span<const uint8_t> wkb;
};

class WKBTest
  : public testing::TestWithParam<WKBTestParam>
{};

TEST_P(WKBTest, ReadWKB)
{
    const auto& param = GetParam();
    auto actual       = ngen::read_wkb(param.wkb);
    auto expected     = ngen::read_wkt(param.wkt);
    EXPECT_EQ(actual, expected);
}

INSTANTIATE_TEST_SUITE_P(
    Spatial,
    WKBTest,
    testing::Values(
        WKBTestParam{"PointLittleEndian", wkt_point, wkb_point_little_endian},
        WKBTestParam{"PointBigEndian", wkt_point, wkb_point_big_endian},
        WKBTestParam{"LineString", wkt_linestring, wkb_linestring},
        WKBTestParam{"Polygon", wkt_polygon, wkb_polygon},
        WKBTestParam{"PolygonWithHoles", wkt_polygon_with_holes, wkb_polygon_with_holes},
        WKBTestParam{"MultiPoint", wkt_multipoint, wkb_multipoint},
        WKBTestParam{"MultiLineString", wkt_multilinestring, wkb_multilinestring},
        WKBTestParam{"MultiPolygon", wkt_multipolygon, wkb_multipolygon},
        WKBTestParam{"GeometryCollection", wkt_geometrycollection, wkb_geometrycollection}
    ),
    [](const testing::TestParamInfo<WKBTest::ParamType>& info) -> std::string {
        return info.param.name;
    }
);

