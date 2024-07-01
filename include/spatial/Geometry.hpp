#pragma once

#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/core/tags.hpp>

#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_point.hpp>
#include <boost/geometry/geometries/multi_linestring.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>
#include <boost/geometry/geometries/geometry_collection.hpp>
#include <boost/geometry/geometries/box.hpp>

#include <boost/variant.hpp>

namespace ngen {

struct Geometry;

// Core Geometry Types

using Point = boost::geometry::model::d2::point_xy<double>;
using LineString = boost::geometry::model::linestring<Point>;
using Polygon = boost::geometry::model::polygon<Point>;
using MultiPoint = boost::geometry::model::multi_point<Point>;
using MultiLineString = boost::geometry::model::multi_linestring<LineString>;
using MultiPolygon = boost::geometry::model::multi_polygon<Polygon>;
using GeometryCollection = boost::geometry::model::geometry_collection<Geometry>;

/* Specialized Geometry Types */
using Box  = boost::geometry::model::box<Point>;
using Ring = typename Polygon::ring_type;

enum class GeometryType {
    EMPTY              = 0, // Non-standard
    POINT              = 1,
    LINESTRING         = 2,
    POLYGON            = 3,
    MULTIPOINT         = 4,
    MULTILINESTRING    = 5,
    MULTIPOLYGON       = 6,
    GEOMETRYCOLLECTION = 7
};

inline std::string to_string(GeometryType type) noexcept
{
    switch (type) {
        case GeometryType::EMPTY:
            return "Empty";
        case GeometryType::POINT:
            return "Point";
        case GeometryType::LINESTRING:
            return "LineString";
        case GeometryType::POLYGON:
            return "Polygon";
        case GeometryType::MULTIPOINT:
            return "MultiPoint";
        case GeometryType::MULTILINESTRING:
            return "MultiLineString";
        case GeometryType::MULTIPOLYGON:
            return "MultiPolygon";
        case GeometryType::GEOMETRYCOLLECTION:
            return "GeometryCollection";
    }

    return "<Invalid ngen::GeometryType " + std::to_string(static_cast<int>(type)) + ">";
}

template<typename G>
constexpr GeometryType static_type();

#define NGEN_STATIC_GEOMETRY_TYPE_IMPL(G, E) \
    template<> constexpr GeometryType static_type<G>() { return (E); }

NGEN_STATIC_GEOMETRY_TYPE_IMPL(Point, GeometryType::POINT)
NGEN_STATIC_GEOMETRY_TYPE_IMPL(LineString, GeometryType::LINESTRING)
NGEN_STATIC_GEOMETRY_TYPE_IMPL(Polygon, GeometryType::POLYGON)
NGEN_STATIC_GEOMETRY_TYPE_IMPL(MultiPoint, GeometryType::MULTIPOINT)
NGEN_STATIC_GEOMETRY_TYPE_IMPL(MultiLineString, GeometryType::MULTILINESTRING)
NGEN_STATIC_GEOMETRY_TYPE_IMPL(MultiPolygon, GeometryType::MULTIPOLYGON)
NGEN_STATIC_GEOMETRY_TYPE_IMPL(GeometryCollection, GeometryType::GEOMETRYCOLLECTION)

#undef NGEN_STATIC_GEOMETRY_TYPE_IMPL

// Exception Types
struct bad_geometry_cast : public std::logic_error
{
    bad_geometry_cast(ngen::GeometryType from, ngen::GeometryType to)
      : std::logic_error("attempted to cast from " + to_string(from) + " to " + to_string(to))
    {}
};

// List of geometry types, only used within this header -- undefined at EOF.
#define NGEN_SPATIAL_GEOMETRY_TYPES \
    ngen::Point,                    \
    ngen::LineString,               \
    ngen::Polygon,                  \
    ngen::MultiPoint,               \
    ngen::MultiLineString,          \
    ngen::MultiPolygon,             \
    ngen::GeometryCollection

class Geometry : private boost::variant<NGEN_SPATIAL_GEOMETRY_TYPES> {
    using base_type = boost::variant<NGEN_SPATIAL_GEOMETRY_TYPES>;

  public:
    using base_type::variant;
    using base_type::apply_visitor;

    Geometry();

    GeometryType type() const noexcept;
    Box envelope() const noexcept;
    bool empty() const noexcept;
    void swap(Geometry& rhs) noexcept;

    bool operator==(const Geometry& rhs) const noexcept;

    template<
        typename Tp,
        std::enable_if_t<boost::geometry::util::is_geometry<Tp>::value, bool> = true
    >
    Tp& downcast()
    {
        if (type() != static_type<Tp>()) {
            throw bad_geometry_cast{type(), static_type<Tp>()};
        }
        
        return boost::strict_get<Tp&>(*this);
    }

    template<
        typename Tp,
        std::enable_if_t<boost::geometry::util::is_geometry<Tp>::value, bool> = true
    >
    const Tp& downcast() const
    {
        if (type() != static_type<Tp>()) {
            throw bad_geometry_cast{type(), static_type<Tp>()};
        }
        
        return boost::strict_get<const Tp&>(*this);
    }
};

} // namespace ngen

// ============================================================================
// Boost Concept Assertions and Traits ========================================
// ============================================================================

// Tag specialization for ngen::Geometry to ensure it dispatches
// as a dynamic geometry object.
template<>
struct boost::geometry::traits::tag<ngen::Geometry> {
    using type = boost::geometry::dynamic_geometry_tag;
};

// Geometry type specialization for ngen::Geometry
template<>
struct boost::geometry::traits::geometry_types<ngen::Geometry> {
    using type = boost::geometry::util::type_sequence<NGEN_SPATIAL_GEOMETRY_TYPES>;
};


// Dynamic geometry visit trait specialization
template<>
struct boost::geometry::traits::visit<ngen::Geometry>
{
    template<typename Function>
    struct visitor : public boost::static_visitor<>
    {
        visitor(Function function)
          : function_(function)
        {}

        template<typename GeometryType>
        void operator()(GeometryType&& geometry)
        {
            function_(std::forward<GeometryType>(geometry));
        }

        Function function_;
    };

    template<typename Function, typename GeometryType>
    static void apply(Function function, GeometryType&& geometry)
    {
        visitor<Function> visitor{function};
        boost::apply_visitor(visitor, std::forward<GeometryType>(geometry));
    }
};

// Dynamic geometry visit trait specialization
template<>
struct boost::geometry::traits::visit<ngen::Geometry, ngen::Geometry>
{
    template<typename Function>
    struct visitor : public boost::static_visitor<>
    {
        visitor(Function function)
          : function_(function)
        {}

        template<typename GeometryType1, typename GeometryType2>
        void operator()(GeometryType1&& g1, GeometryType2&& g2)
        {
            function_(
                std::forward<GeometryType1>(g1),
                std::forward<GeometryType2>(g2)
            );
        }

        Function function_;
    };

    template<typename Function, typename GeometryType1, typename GeometryType2>
    static void apply(Function function, GeometryType1&& g1, GeometryType2&& g2)
    {
        visitor<Function> visitor{function};
        boost::apply_visitor(
            visitor,
            std::forward<GeometryType1>(g1),
            std::forward<GeometryType2>(g2)
        );
    }
};

// Concept assertions
BOOST_CONCEPT_ASSERT((boost::geometry::concepts::DynamicGeometry<ngen::Geometry>));
BOOST_CONCEPT_ASSERT((boost::geometry::concepts::ConstDynamicGeometry<ngen::Geometry>));

#undef NGEN_SPATIAL_GEOMETRY_TYPES
