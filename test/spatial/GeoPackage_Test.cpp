#include <gtest/gtest.h>

#include <spatial/io/geopackage.hpp>

#include <utilities/FileChecker.h>

#include "SpatialTestHelpers.hpp"

#include <NGenConfig.h>

TEST(Spatial, GeoPackage)
{
    #if !NGEN_WITH_SQLITE
    GTEST_SKIP() << "GeoPackage support not enabled.";
    #endif

    auto path = utils::FileChecker::find_first_readable({
        "test/data/geopackage/example.gpkg",
        "../test/data/geopackage/example.gpkg",
        "../../test/data/geopackage/example.gpkg"
    });

    if (path.empty()) {
        FAIL() << "can't find test/data/geopackage/example.gpkg";
    }

    auto gpkg = ngen::read_geopackage(path, "test");
    EXPECT_TRUE(gpkg.contains("First"));
    EXPECT_TRUE(gpkg.contains("Second"));
    auto bbox = gpkg.envelope();
    EXPECT_GEOM_EQ(ngen::Point(102.0, 0.0), bbox.min_corner());
    EXPECT_GEOM_EQ(ngen::Point(105.0, 1.0), bbox.max_corner());
    EXPECT_EQ(gpkg.size(), 2);

    auto& first = gpkg.at("First");
    EXPECT_EQ(first.id(), "First");
    EXPECT_EQ(first.geometry_type(), ngen::GeometryType::POINT);
    EXPECT_GEOM_EQ(ngen::Point(102.0, 0.5), first.geometry().downcast<ngen::Point>());

    ASSERT_ANY_THROW(gpkg.at("Third"));
}

TEST(Spatial, GeoPackageSubset)
{
    #if !NGEN_WITH_SQLITE
    GTEST_SKIP() << "GeoPackage support not enabled.";
    #endif
    
    auto path = utils::FileChecker::find_first_readable({
        "test/data/geopackage/example.gpkg",
        "../test/data/geopackage/example.gpkg",
        "../../test/data/geopackage/example.gpkg"
    });

    if (path.empty()) {
        FAIL() << "can't find test/data/geopackage/example.gpkg";
    }

    auto gpkg = ngen::read_geopackage(path, "test", {{ "First" }});
    EXPECT_TRUE(gpkg.contains("First"));
    EXPECT_FALSE(gpkg.contains("Second"));
    EXPECT_EQ(gpkg.size(), 1);
}
