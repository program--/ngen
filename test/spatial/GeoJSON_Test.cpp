#include "spatial/FeatureCollection.hpp"
#include "spatial/Geometry.hpp"
#include <gtest/gtest.h>

#include <spatial/io/geojson.hpp>

#include <boost/geometry/algorithms/equals.hpp>

// Provides:
// - test_feature_document
// - test_nexus_data
// - test_catchment_data
#include "GeoJSON_TestData.hpp"

#include "SpatialTestHelpers.hpp"
#include "spatial/algorithm/project.hpp"

TEST(Spatial, GeoJSON_Feature)
{
    auto document = boost::json::parse(test_feature_document);
    auto feature  = boost::json::value_to<ngen::Feature>(document);

    EXPECT_EQ(feature.geometry_type(), ngen::GeometryType::POLYGON);
    EXPECT_EQ(feature.id(), "38");
    EXPECT_EQ(feature.srid(), 4326);
    EXPECT_EQ(feature.properties().size(), 1);
    EXPECT_EQ(feature.properties()["name"], "North Dakota");

    auto bbox = feature.envelope();
    EXPECT_GEOM_EQ(ngen::Point(-104.05, 45.94), bbox.min_corner());
    EXPECT_GEOM_EQ(ngen::Point(-96.58, 48.99), bbox.max_corner());

    ASSERT_NO_THROW(feature.promote_id("name"));
    EXPECT_EQ(feature.id(), "North Dakota");

    ASSERT_NO_THROW(ngen::project_inplace(feature, 5070));
    bbox = feature.envelope();
    // Using EXPECT_NEAR here because the precision difference is too great for EXPECT_GEOM_EQ. 
    EXPECT_NEAR(-622737.907687213, bbox.min_corner().x(), 1e-5);
    EXPECT_NEAR(2550337.5460196, bbox.min_corner().y(), 1e-5);
    EXPECT_NEAR(-45033.1128094282, bbox.max_corner().x(), 1e-5);
    EXPECT_NEAR(2912372.2294787, bbox.max_corner().y(), 1e-5);

    const ngen::Geometry& geom = feature.geometry();
    
    ASSERT_THROW(geom.downcast<ngen::LineString>(), ngen::bad_geometry_cast);

    const ngen::Polygon& poly = geom.downcast<ngen::Polygon>();
    EXPECT_EQ(poly.outer().size(), 5);
    EXPECT_EQ(poly.inners().size(), 0);
}

TEST(Spatial, GeoJSON_FeatureCollection)
{
    auto document = boost::json::parse(test_nexus_data);
    auto fc = boost::json::value_to<ngen::FeatureCollection>(document);

    EXPECT_EQ(fc.size(), 3);
    
    auto bbox = fc.envelope();
    EXPECT_GEOM_EQ(ngen::Point(-80.73677251799995, 35.15839061111134), bbox.min_corner());
    EXPECT_GEOM_EQ(ngen::Point(-80.71308451799996, 35.18693221111115), bbox.max_corner());

    auto& feature = fc.at("nex-34");
    ASSERT_EQ(feature.id(), "nex-34");
    EXPECT_EQ(feature.geometry_type(), ngen::GeometryType::POINT);
    EXPECT_EQ(feature.properties()["toid"], "cat-34");
}

TEST(Spatial, GeoJSON_Linked)
{
    auto nex_document = boost::json::parse(test_nexus_data);
    auto nex_fc = boost::json::value_to<ngen::FeatureCollection>(nex_document);
    ASSERT_EQ(nex_fc.size(), 3);

    {
        auto cat_document = boost::json::parse(test_catchment_data);
        auto cat_fc = boost::json::value_to<ngen::FeatureCollection>(cat_document);
        ASSERT_EQ(cat_fc.size(), 3);

        nex_fc.merge(cat_fc);
        ASSERT_EQ(nex_fc.size(), 6);

        for (const auto& feat : cat_fc) {
            ASSERT_TRUE(nex_fc.contains(feat.id()));
        }
    } // cat_fc destroyed

    ASSERT_NO_THROW(nex_fc.try_link("toid"));

    const ngen::Feature* const cat = &nex_fc.at("cat-67");
    const ngen::Feature* const nex = &nex_fc.at("nex-68");

    EXPECT_FALSE(cat->is_leaf());
    EXPECT_TRUE(cat->is_root());
    EXPECT_EQ(cat->upstream().size(), 0);
    EXPECT_EQ(cat->downstream().size(), 1);
    EXPECT_EQ(cat->downstream().front(), nex); // Equal pointer addresses

    EXPECT_TRUE(nex->is_leaf());
    EXPECT_FALSE(nex->is_root());
    EXPECT_EQ(nex->downstream().size(), 0);
    EXPECT_EQ(nex->upstream().size(), 1);
    EXPECT_EQ(nex->upstream().front(), cat); // Equal pointer addresses
}
