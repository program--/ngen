#include "geometry_test_suite.hpp"
#include "geometry/backends/boost.hpp"

NGEN_GEOMETRY_TEST(BoostPointTest, ngen::spatial::boost::boost_point, 1, 2)
NGEN_GEOMETRY_TEST(BoostLineStringTest, ngen::spatial::boost::boost_linestring, {{1, 2}, {3, 4}, {5, 6}});
// NGEN_GEOMETRY_TEST(BoostLinearRingTest, ngen::spatial::boost::boost_linearring);
// NGEN_GEOMETRY_TEST(BoostPolygonTest,    ngen::spatial::boost::boost_polygon);
