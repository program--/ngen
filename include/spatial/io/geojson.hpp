#pragma once

#include <spatial/FeatureCollection.hpp>
#include <spatial/Feature.hpp>
#include <spatial/Geometry.hpp>

#include <boost/json.hpp>
#include <boost/geometry/core/exception.hpp>

namespace ngen {

struct invalid_geojson_exception
  : public std::runtime_error
{
    invalid_geojson_exception(std::string what, boost::json::value input)
      : std::runtime_error(std::move(what) + "\nInput:\n" + boost::json::value_to<std::string>(std::move(input)))
    {}
};

template<typename Tp>
Tp read_geojson(boost::json::value json)
{
    return boost::json::value_to<Tp>(std::move(json));
}

template<typename Tp>
Tp read_geojson(boost::core::string_view json)
{
    return read_geojson<Tp>(boost::json::parse(json));
}

FeatureCollection tag_invoke(
    boost::json::value_to_tag<FeatureCollection> /*unused*/,
    const boost::json::value& value
);

Feature tag_invoke(
    boost::json::value_to_tag<Feature> /*unused*/,
    const boost::json::value& value
);

GeometryType tag_invoke(
    boost::json::value_to_tag<GeometryType> /*unused*/,
    const boost::json::value& value
);

ngen::Geometry tag_invoke(
    boost::json::value_to_tag<ngen::Geometry> /*unused*/,
    const boost::json::value& value
);

} // namespace ngen


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// The tag_invoke overloads below must be placed within
// the `boost::geometry::model` namespace due to two things:
//
//    (1) ngen::Point, ngen::LineString etc. are type aliases
//        to Boost.Geometry types. So, they are not actual
//        types defined within the `ngen` namespace, but rather
//        types in the `boost::geometry::model` namespace that have
//        been brought into the `ngen` scope.
//
//        i.e.
//        
//        std::is_same<
//            ngen::Point,
//            boost::geometry::model::d2::point_xy<double>
//        >::value == true
//
//    (2) Boost.JSON uses ADL to perform tag dispatch, so
//        if we define these overloads in the `ngen` namespace,
//        we'll get a compilation error due to the functions
//        not being resolvable in the types' (actual) namespace.
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

namespace boost {
namespace geometry {
namespace model {

ngen::Point tag_invoke(
    boost::json::value_to_tag<ngen::Point> /*unused*/,
    const boost::json::value& value
);

ngen::LineString tag_invoke(
    boost::json::value_to_tag<ngen::LineString> /*unused*/,
    const boost::json::value& value
);

ngen::Polygon tag_invoke(
    boost::json::value_to_tag<ngen::Polygon> /*unused*/,
    const boost::json::value& value
);

ngen::MultiPoint tag_invoke(
    boost::json::value_to_tag<ngen::MultiPoint> /*unused*/,
    const boost::json::value& value
);

ngen::MultiLineString tag_invoke(
    boost::json::value_to_tag<ngen::MultiLineString> /*unused*/,
    const boost::json::value& value
);

ngen::MultiPolygon tag_invoke(
    boost::json::value_to_tag<ngen::MultiPolygon> /*unused*/,
    const boost::json::value& value
);

ngen::GeometryCollection tag_invoke(
    boost::json::value_to_tag<ngen::GeometryCollection> /*unused*/,
    const boost::json::value& value
);

} // namespace model
} // namespace geometry
} // namespace boost
