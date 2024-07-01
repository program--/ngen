#include <spatial/io/geojson.hpp>

// Ensure Boost.JSON is header only
// https://github.com/boostorg/json#header-only
#include <boost/json/src.hpp>

#include <boost/core/make_span.hpp>

#define NGEN_GEOJSON_ASSERT(...) ngen::geojson_assert(__VA_ARGS__, value)

namespace ngen {

inline void geojson_assert(bool cond, const char* text, const boost::json::value& value)
{
    #ifndef NGDEBUG
    if (!cond) throw invalid_geojson_exception{text, value};
    #endif
}

FeatureCollection tag_invoke(
    boost::json::value_to_tag<FeatureCollection> /*unused*/,
    const boost::json::value& value
)
{
    NGEN_GEOJSON_ASSERT(
        value.is_object(),
        "GeoJSON FeatureCollection input must be an object."
    );

    const auto& object       = value.as_object();
    const auto* type         = object.if_contains("type");
    const auto* features_ptr = object.if_contains("features");

    NGEN_GEOJSON_ASSERT(
        type != nullptr,
        "GeoJSON FeatureCollection input does not contain the property `type`."
    );

    NGEN_GEOJSON_ASSERT(
        type->is_string() && type->as_string() == "FeatureCollection",
        "GeoJSON FeatureCollection input does not have valid type of 'FeatureCollection'."
    );

    NGEN_GEOJSON_ASSERT(
        features_ptr != nullptr,
        "GeoJSON FeatureCollection input does not contain the property `features`."
    );

    NGEN_GEOJSON_ASSERT(
        features_ptr->is_array(),
        "GeoJSON FeatureCollection input does not have a valid `features` array property."
    );

    const boost::json::array& features = features_ptr->as_array();
    FeatureCollection collection;
    collection.reserve(features.size());
    for (const auto& feature : features) {
        collection.emplace(boost::json::value_to<Feature>(feature));
    }
    
    return collection;
}

Feature tag_invoke(
    boost::json::value_to_tag<Feature> /*unused*/,
    const boost::json::value& value
)
{
    NGEN_GEOJSON_ASSERT(
        value.is_object(),
        "GeoJSON Feature input must be an object."
    );

    const auto& object = value.as_object();
    const auto* id_ptr = object.if_contains("id");
    const auto* geometry_ptr = object.if_contains("geometry");
    const auto* properties_ptr = object.if_contains("properties");

    NGEN_GEOJSON_ASSERT(
        geometry_ptr != nullptr,
        "GeoJSON Feature input must have property `geometry`."
    );

    if (properties_ptr != nullptr) {
        NGEN_GEOJSON_ASSERT(
            properties_ptr->is_object(),
            "GeoJSON Feature input has an invalid `properties` property."
        );
    }

    if (id_ptr != nullptr) {
        NGEN_GEOJSON_ASSERT(
            id_ptr->is_string(),
            "GeoJSON Feature input has invalid `id` string property."
        );
    }

    Geometry geometry = boost::json::value_to<Geometry>(*geometry_ptr);

    return {
        /*geometry=*/std::move(geometry),
        /*properties=*/(properties_ptr == nullptr ? boost::json::object{} : properties_ptr->as_object()),
        /*srid=*/4326,
        /*id=*/(id_ptr == nullptr ? "" : id_ptr->as_string())
    };
}

GeometryType tag_invoke(
    boost::json::value_to_tag<GeometryType> /*unused*/,
    const boost::json::value& value
)
{
    NGEN_GEOJSON_ASSERT(value.is_string(), "GeoJSON Geometry property `type` must be a string.");

    const auto& s = value.as_string();

    if (s == "Point")              return GeometryType::POINT;
    if (s == "LineString")         return GeometryType::LINESTRING;
    if (s == "Polygon")            return GeometryType::POLYGON;
    if (s == "MultiPoint")         return GeometryType::MULTIPOINT;
    if (s == "MultiLineString")    return GeometryType::MULTILINESTRING;
    if (s == "MultiPolygon")       return GeometryType::MULTIPOLYGON;
    if (s == "GeometryCollection") return GeometryType::GEOMETRYCOLLECTION;

    throw invalid_geojson_exception{"GeoJSON Geometry contains invalid `type` property.", value};
}

ngen::Geometry tag_invoke(
    boost::json::value_to_tag<ngen::Geometry> /*unused*/,
    const boost::json::value& value
)
{
    NGEN_GEOJSON_ASSERT(
        value.is_object(),
        "GeoJSON Geometry input must be an object."
    );

    const auto& object = value.as_object();
    const auto* type = object.if_contains("type");

    NGEN_GEOJSON_ASSERT(
        type != nullptr,
        "GeoJSON Geometry input must have property `type`."
    );

    ngen::GeometryType gtype = boost::json::value_to<ngen::GeometryType>(*type);

    const boost::json::value* data = (
        gtype == ngen::GeometryType::GEOMETRYCOLLECTION
            ? object.if_contains("geometries")
            : object.if_contains("coordinates")
    );

    NGEN_GEOJSON_ASSERT(
        data != nullptr && data->is_array(),
        gtype == ngen::GeometryType::GEOMETRYCOLLECTION
            ? "GeoJSON Geometry with type 'GeometryCollection' must have a `geometries` array property."
            : "GeoJSON Geometry must have a `coordinates` array property."
    );

    switch(gtype) {
        case ngen::GeometryType::POINT:
            return boost::json::value_to<ngen::Point>(*data);
        case ngen::GeometryType::LINESTRING:
            return boost::json::value_to<ngen::LineString>(*data);
        case ngen::GeometryType::POLYGON:
            return boost::json::value_to<ngen::Polygon>(*data);
        case ngen::GeometryType::MULTIPOINT:
            return boost::json::value_to<ngen::MultiPoint>(*data);
        case ngen::GeometryType::MULTILINESTRING:
            return boost::json::value_to<ngen::MultiLineString>(*data);
        case ngen::GeometryType::MULTIPOLYGON:
            return boost::json::value_to<ngen::MultiPolygon>(*data);
        case ngen::GeometryType::GEOMETRYCOLLECTION:
            return boost::json::value_to<ngen::GeometryCollection>(*data);
        default:
            throw ngen::invalid_geojson_exception{"GeoJSON Geometry contains invalid `type` property", value};
    }
}

} // namespace ngen

namespace boost {
namespace geometry {
namespace model {

ngen::Point tag_invoke(
    boost::json::value_to_tag<ngen::Point> /*unused*/,
    const boost::json::value& value
)
{
    const auto& coordinates = value.as_array();

    NGEN_GEOJSON_ASSERT(
        coordinates[0].is_double() && coordinates[1].is_double(),
        "GeoJSON Point `coordinates` must be an array of doubles"
    );

    return { coordinates[0].as_double(), coordinates[1].as_double() };
}

ngen::LineString tag_invoke(
    boost::json::value_to_tag<ngen::LineString> /*unused*/,
    const boost::json::value& value
)
{
    const auto& coordinates = value.as_array();

    ngen::LineString line;
    line.reserve(coordinates.size());
    for (const auto& ele : coordinates) {
        line.push_back(boost::json::value_to<ngen::Point>(ele));
    }

    return line;
}

ngen::Polygon tag_invoke(
    boost::json::value_to_tag<ngen::Polygon> /*unused*/,
    const boost::json::value& value
)
{
    const auto& coordinates = value.as_array();
    const auto& outer = coordinates[0].as_array();
    
    ngen::Polygon polygon;
    polygon.outer().reserve(outer.size());
    for (const auto& ele : outer) {
        polygon.outer().push_back(boost::json::value_to<ngen::Point>(ele));
    }

    const auto& inner = boost::make_span(coordinates.begin() + 1, coordinates.end());
    polygon.inners().resize(inner.size());
    auto new_ring = polygon.inners().begin();
    for (const auto& ele : inner) {
        const auto& inner_ring = ele.as_array();
        new_ring->reserve(inner_ring.size());
        for (const auto& coordinate : inner_ring) {
            new_ring->push_back(boost::json::value_to<ngen::Point>(coordinate));
        }
    
        new_ring++;
    }

    return polygon;
}

template<typename M> __attribute__((always_inline))
M construct_multi_from_geojson(const boost::json::value& value)
{
    M multi;

    const auto& elements = value.as_array();
    multi.reserve(elements.size());
    for (const auto& ele : elements) {
        multi.push_back(boost::json::value_to<typename M::value_type>(ele));
    }

    return multi;
}

ngen::MultiPoint tag_invoke(
    boost::json::value_to_tag<ngen::MultiPoint> /*unused*/,
    const boost::json::value& value
)
{
    return construct_multi_from_geojson<ngen::MultiPoint>(value);
}

ngen::MultiLineString tag_invoke(
    boost::json::value_to_tag<ngen::MultiLineString> /*unused*/,
    const boost::json::value& value
)
{
    return construct_multi_from_geojson<ngen::MultiLineString>(value);
}

ngen::MultiPolygon tag_invoke(
    boost::json::value_to_tag<ngen::MultiPolygon> /*unused*/,
    const boost::json::value& value
)
{
    return construct_multi_from_geojson<ngen::MultiPolygon>(value);
}

ngen::GeometryCollection tag_invoke(
    boost::json::value_to_tag<ngen::GeometryCollection> /*unused*/,
    const boost::json::value& value
)
{
    return construct_multi_from_geojson<ngen::GeometryCollection>(value);
}

} // namespace model
} // namespace geometry
} // namespace boost

#undef NGEN_GEOJSON_ASSERT
