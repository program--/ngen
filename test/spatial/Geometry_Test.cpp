#include <gtest/gtest.h>

#include <spatial/Geometry.hpp>

#include <boost/geometry/algorithms/equals.hpp>

#include "SpatialTestHelpers.hpp"

TEST(Spatial, Point) {
    ngen::Geometry point = ngen::Point{102.0, 0.5};
    EXPECT_FALSE(point.empty());
    EXPECT_EQ(point.type(), ngen::GeometryType::POINT);
    auto bbox = point.envelope();
    EXPECT_GEOM_EQ(point.downcast<ngen::Point>(), bbox.min_corner());
    EXPECT_GEOM_EQ(point.downcast<ngen::Point>(), bbox.max_corner());
    
    ngen::Geometry empty{};
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.type(), ngen::GeometryType::EMPTY);
    empty.swap(point);
    EXPECT_FALSE(empty.empty());
    EXPECT_NE(empty.type(), ngen::GeometryType::EMPTY);
    EXPECT_GEOM_EQ(ngen::Point(102.0, 0.5), empty.downcast<ngen::Point>());
    EXPECT_TRUE(point.empty());
    EXPECT_EQ(point.type(), ngen::GeometryType::EMPTY);
}

TEST(Spatial, LineString) {
    ngen::Geometry linestring = ngen::LineString{
        {102.0, 0.5},
        {103.0, 0.6},
        {104.0, 0.7},
        {105.0, 0.8}
    };

    EXPECT_FALSE(linestring.empty());
    EXPECT_EQ(linestring.type(), ngen::GeometryType::LINESTRING);

    auto bbox = linestring.envelope();
    
    EXPECT_GEOM_EQ(ngen::Point(102.0, 0.5), bbox.min_corner());
    EXPECT_GEOM_EQ(ngen::Point(105.0, 0.8), bbox.max_corner());

    ngen::Geometry empty{};
    empty.swap(linestring);
    EXPECT_FALSE(empty.empty());
    EXPECT_TRUE(linestring.empty());
    EXPECT_EQ(linestring.type(), ngen::GeometryType::EMPTY);
}
