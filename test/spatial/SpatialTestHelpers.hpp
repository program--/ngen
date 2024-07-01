#pragma once

#include <gtest/gtest.h>

#include <spatial/Geometry.hpp>

#include <boost/geometry/io/wkt/write.hpp>
#include <boost/geometry/algorithms/equals.hpp>

namespace ngen {

inline void PrintTo(const Geometry& geom, std::ostream* os)
{
    *os << boost::geometry::wkt(geom);
}

template<typename G1, typename G2>
testing::AssertionResult ExpectGeomEq(const char* g1_expr, const char* g2_expr, G1&& g1, G2&& g2)
{
    if (boost::geometry::equals(std::forward<G1>(g1), std::forward<G2>(g2))) {
        return testing::AssertionSuccess();
    }

    return testing::AssertionFailure()
        << "Expression: " << g1_expr << " != " << g2_expr << '\n'
        << "       LHS: " << boost::geometry::wkt(g1) << '\n'
        << "       RHS: " << boost::geometry::wkt(g2) << '\n';
           
}

#define EXPECT_GEOM_EQ(a, b) \
    EXPECT_PRED_FORMAT2(ngen::ExpectGeomEq, a, b)

} // namespace ngen
