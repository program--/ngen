#include "spatial/Geometry.hpp"
#include <spatial/io/wkb.hpp>

#include <boost/geometry/algorithms/num_points.hpp>

#include <utilities/EndianCopy.hpp>

namespace ngen {

struct WKBReader
{
    std::size_t global_offset{};

    template<typename T>
    T read_wkb(
        boost::span<const std::uint8_t> data,
        boost::endian::order order = boost::endian::order::native
    );

    #define WKB_READER_TMPL_PROTO(TYPE)           \
        template<>                                \
        TYPE read_wkb(                            \
            boost::span<const std::uint8_t> data, \
            boost::endian::order order            \
        )

    WKB_READER_TMPL_PROTO(boost::endian::order);
    WKB_READER_TMPL_PROTO(std::uint32_t);
    WKB_READER_TMPL_PROTO(Point);
    WKB_READER_TMPL_PROTO(LineString);
    WKB_READER_TMPL_PROTO(Polygon);
    WKB_READER_TMPL_PROTO(MultiPoint);
    WKB_READER_TMPL_PROTO(MultiLineString);
    WKB_READER_TMPL_PROTO(MultiPolygon);
    WKB_READER_TMPL_PROTO(GeometryCollection);
    WKB_READER_TMPL_PROTO(Geometry);

    #undef WKBREADER_INSTANTIATE_TMPL
};

#define WKB_READER_TMPL_DEFN(TYPE)            \
    template<>                                \
    TYPE WKBReader::read_wkb(                 \
        boost::span<const std::uint8_t> data, \
        boost::endian::order order            \
    )

WKB_READER_TMPL_DEFN(boost::endian::order)
{
    global_offset++;
    return data[0] == 0x01 ? boost::endian::order::little : boost::endian::order::big;
}

WKB_READER_TMPL_DEFN(std::uint32_t)
{
    global_offset += sizeof(std::uint32_t);
    return copy_from<std::uint32_t>(data.subspan(0, sizeof(std::uint32_t)), order);
}

WKB_READER_TMPL_DEFN(Point)
{
    global_offset += sizeof(double) * 2;
    return {
        /*x=*/copy_from<double>(data.subspan(0, sizeof(double)), order), 
        /*y=*/copy_from<double>(data.subspan(sizeof(double), sizeof(double)), order)
    };
}

WKB_READER_TMPL_DEFN(LineString)
{
    std::size_t local_offset = global_offset;
    uint32_t count = read_wkb<std::uint32_t>(data, order);

    LineString linestring;
    linestring.reserve(count);
    for (auto i = 0; i < count; ++i) {
        linestring.push_back(
            read_wkb<Point>(data.subspan(global_offset - local_offset, sizeof(double) * 2), order)
        );
    }

    return linestring;
}

WKB_READER_TMPL_DEFN(Polygon)
{
    std::size_t local_offset = global_offset;
    uint32_t count = read_wkb<uint32_t>(data, order);
    
    Polygon polygon;
    auto outer = read_wkb<LineString>(data.subspan(global_offset - local_offset), order);
    polygon.outer().swap(outer);
    
    if (count > 1) {
        count--; // decrement outer
        polygon.inners().resize(count);
        for (std::size_t i = 0; i < count; ++i) {
            auto inner = read_wkb<LineString>(data.subspan(global_offset - local_offset), order);
            polygon.inners()[i].swap(inner);
        }
    }

    return polygon;
}

WKB_READER_TMPL_DEFN(MultiPoint)
{
    std::size_t local_offset = global_offset;
    std::uint32_t count = read_wkb<std::uint32_t>(data, order);

    MultiPoint multi;
    multi.reserve(count);

    for (auto i = 0; i < count; ++i) {
        order = read_wkb<boost::endian::order>(
            data.subspan(global_offset - local_offset)
        );

        std::uint32_t type = read_wkb<std::uint32_t>(
            data.subspan(global_offset - local_offset),
            order
        );

        auto geom = read_wkb<Point>(
            data.subspan(global_offset - local_offset),
            order
        );
        multi.push_back(std::move(geom));
    }

    return multi;
}

WKB_READER_TMPL_DEFN(MultiLineString)
{
    std::size_t local_offset = global_offset;
    std::uint32_t count = read_wkb<std::uint32_t>(data, order);

    MultiLineString multi;
    multi.reserve(count);

    for (auto i = 0; i < count; ++i) {
        order = read_wkb<boost::endian::order>(
            data.subspan(global_offset - local_offset)
        );

        std::uint32_t type = read_wkb<std::uint32_t>(
            data.subspan(global_offset - local_offset),
            order
        );

        auto geom = read_wkb<LineString>(
            data.subspan(global_offset - local_offset),
            order
        );
    
        multi.push_back(std::move(geom));
    }

    return multi;
}

WKB_READER_TMPL_DEFN(MultiPolygon)
{
    std::size_t local_offset = global_offset;
    std::uint32_t count = read_wkb<std::uint32_t>(data, order);

    MultiPolygon multi;
    multi.reserve(count);

    for (auto i = 0; i < count; ++i) {
        order = read_wkb<boost::endian::order>(
            data.subspan(global_offset - local_offset)
        );

        std::uint32_t type = read_wkb<std::uint32_t>(
            data.subspan(global_offset - local_offset),
            order
        );

        auto geom = read_wkb<Polygon>(
            data.subspan(global_offset - local_offset),
            order
        );

        multi.push_back(std::move(geom));
    }

    return multi;
}

WKB_READER_TMPL_DEFN(GeometryCollection)
{
    std::size_t local_offset = global_offset;
    std::uint32_t count = read_wkb<std::uint32_t>(data, order);

    GeometryCollection multi;
    multi.reserve(count);
    for (auto i = 0; i < count; ++i) {
        auto geom = read_wkb<Geometry>(
            data.subspan(global_offset - local_offset),
            order
        );

        multi.push_back(std::move(geom));
    }

    return multi;
}

WKB_READER_TMPL_DEFN(Geometry)
{
    std::size_t local_offset = global_offset;
    order = read_wkb<boost::endian::order>(
        data.subspan(global_offset - local_offset)
    );

    auto type = read_wkb<std::uint32_t>(
        data.subspan(global_offset - local_offset),
        order
    );

    auto subspan = data.subspan(global_offset - local_offset);
    switch(static_cast<GeometryType>(type)) {
        case GeometryType::POINT:
            return read_wkb<Point>(subspan, order);
        case GeometryType::LINESTRING:
            return read_wkb<LineString>(subspan, order);
        case GeometryType::POLYGON:
            return read_wkb<Polygon>(subspan, order);
        case GeometryType::MULTIPOINT:
            return read_wkb<MultiPoint>(subspan, order);
        case GeometryType::MULTILINESTRING:
            return read_wkb<MultiLineString>(subspan, order);
        case GeometryType::MULTIPOLYGON:
            return read_wkb<MultiPolygon>(subspan, order);
        case GeometryType::GEOMETRYCOLLECTION:
            return read_wkb<GeometryCollection>(subspan, order);
        default:
            throw invalid_wkb_exception{"failed to read input wkb; type " + std::to_string(type) + " not implemented"};
    }
}

// Public
Geometry read_wkb(boost::span<const std::uint8_t> data)
{
    return WKBReader{}.read_wkb<Geometry>(data);
}

} // namespace ngen
