#include <spatial/io/geopackage.hpp>

#include <NGenConfig.h>

#if !NGEN_WITH_SQLITE
ngen::FeatureCollection ngen::read_geopackage(
    boost::core::string_view path,
    std::string layer,
    boost::span<const std::string> ids
) {
    throw std::runtime_error{"GeoPackage support not enabled."};
}
#else

#include <spatial/io/wkb.hpp>

#include <regex>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/range/combine.hpp>

#include "ngen_sqlite/ngen_sqlite.hpp"

#include <utilities/EndianCopy.hpp>

namespace ngen {
// Forward Declaration Start ==================================================
boost::json::value build_property(
    const ngen::sqlite::database::iterator& row,
    const std::string& name,
    int type
);

boost::json::object build_properties(
    const ngen::sqlite::database::iterator& row,
    const std::string& geom_col
);

ngen::Geometry build_geometry(
    const ngen::sqlite::database::iterator& row,
    const std::string& geom_col
);

Feature build_feature(
    const ngen::sqlite::database::iterator& row,
    const std::string& id_col,
    const std::string& geom_col
);

// Forward Declaration End ====================================================

void check_table_name(const std::string& table)
{
    if (boost::algorithm::starts_with(table, "sqlite_")) {
        throw std::runtime_error("table `" + table + "` is not queryable");
    }

    std::regex allowed("[^-A-Za-z0-9_ ]+");
    if (std::regex_match(table, allowed)) {
        throw std::runtime_error("table `" + table + "` contains invalid characters");
    }
}

FeatureCollection read_geopackage(
    boost::core::string_view path,
    std::string layer,
    boost::span<const std::string> ids
)
{
    check_table_name(layer);

    ngen::sqlite::database db{path};

    // Check if layer exists
    if (!db.contains(layer)) {
        // Since the layer doesn't exist, we need to output some additional
        // debug information with the error. In this case, we add ALL the tables
        // available in the GPKG, so that if the user sees this error, then it
        // might've been either a typo or a bad data input, and they can correct.
        std::string errmsg = "[" + std::string(sqlite3_errmsg(db.connection())) + "] " +
                             "table " + layer + " does not exist.\n\tTables: ";

        auto errquery = db.query("SELECT name FROM sqlite_master WHERE type='table'");
        errquery.next();
        while(!errquery.done()) {
            errmsg += errquery.get<std::string>(0);
            errmsg += ", ";
            errquery.next();
        }

        throw std::runtime_error(errmsg);
    }

    // Introspect if the layer is divides to see which ID field is in use
    std::string id_column = "id";
    if (layer == "divides") {
        try {
            //TODO: A bit primitive. Actually introspect the schema somehow? https://www.sqlite.org/c3ref/funclist.html
            auto query_get_first_row = db.query("SELECT divide_id FROM " + layer + " LIMIT 1");
            id_column = "divide_id";
        }
        catch (const std::exception& e){
            #ifndef NGEN_QUIET
            // output debug info on what is read exactly
            std::cout << "WARN: Using legacy ID column \"id\" in layer " << layer << " is DEPRECATED and may stop working at any time." << std::endl;
            #endif
        }
    }

    // Layer exists, getting statement for it
    //
    // this creates a string in the form:
    //     WHERE id IN (?, ?, ?, ...)
    // so that it can be bound by SQLite.
    // This is safer than trying to concatenate
    // the IDs together.
    std::string joined_ids = "";
    if (!ids.empty()) {
        joined_ids = " WHERE " + id_column + " IN (?";
        for (size_t i = 1; i < ids.size(); i++) {
            joined_ids += ", ?";
        }
        joined_ids += ")";
    }

    // Get number of features
    auto query_get_layer_count = db.query("SELECT COUNT(*) FROM " + layer + joined_ids, ids);
    query_get_layer_count.next();
    const int layer_feature_count = query_get_layer_count.get<int>(0);

    #ifndef NGEN_QUIET
    // output debug info on what is read exactly
    std::cout << "Reading " << layer_feature_count << " features from layer " << layer << " using ID column `"<< id_column << "`";
    if (!ids.empty()) {
        std::cout << " (id subset:";
        for (auto& id : ids) {
            std::cout << " " << id;
        }
        std::cout << ")";
    }
    std::cout << std::endl;
    #endif

    // Get layer feature metadata (geometry column name + type)
    auto query_get_layer_geom_meta = db.query("SELECT column_name FROM gpkg_geometry_columns WHERE table_name = ?", layer);
    query_get_layer_geom_meta.next();
    const std::string layer_geometry_column = query_get_layer_geom_meta.get<std::string>(0);

    // Get layer
    auto query_get_layer = db.query("SELECT * FROM " + layer + joined_ids, ids);
    query_get_layer.next();

    // build features out of layer query
    FeatureCollection features;
    features.reserve(layer_feature_count);
    while(!query_get_layer.done()) {
        ngen::Feature feature = build_feature(
            query_get_layer,
            id_column,
            layer_geometry_column
        );

        features.emplace(std::move(feature));
        query_get_layer.next();
    }

    return features;
}

// Helpers ====================================================================

// specialization for boost::json::string
template<>
boost::json::string sqlite::database::iterator::get(int col) const
{
    handle_get_index_(col);
    int size = sqlite3_column_bytes(ptr_(), col);
    const unsigned char* ptr = sqlite3_column_text(ptr_(), col);
    return boost::json::string{ ptr, ptr + size };
}

boost::json::value build_property(
    const ngen::sqlite::database::iterator& row,
    const std::string& name,
    std::int64_t type
)
{
    switch(type) {
        case SQLITE_INTEGER:
            return { name, row.get<int>(name)};
        case SQLITE_FLOAT:
            return { name, row.get<double>(name)};
        case SQLITE_TEXT:
            return { name, row.get<std::string>(name)};
        default:
            return { name, "null" };
    }
}

boost::json::object build_properties(
    const ngen::sqlite::database::iterator& row,
    const std::string& geom_col
)
{
    auto data_cols = row.columns();
    auto data_types = row.types();
    boost::json::object properties{data_cols.size()};

    // Create mapping from column name <==> column type
    for (auto kv : boost::combine(data_cols, data_types))
    {
        properties.emplace(kv.get<0>(), kv.get<1>());
    }

    // Convert column to their actual value
    for (auto& col : properties) {
        if (col.key() != geom_col) {
            col.value() = build_property(row, col.key(), col.value().as_int64());
        }
    }

    return properties;
}

ngen::Geometry build_geometry(
    const ngen::sqlite::database::iterator& row,
    const std::string& geom_col,
    std::uint32_t& srid
)
{
    auto blob = row.get<std::vector<std::uint8_t>>(geom_col);
    if (blob[0] != 'G' || blob[1] != 'P') {
        throw std::runtime_error("expected geopackage WKB, but found invalid format instead");
    }

    std::size_t offset = 3; // skip version

    // flags
    bool is_extended  =  blob[offset] & 0x00100000;
    bool is_empty     =  blob[offset] & 0x00010000;
    uint8_t indicator = (blob[offset] >> 1) & 0x00000111;
    auto order        = static_cast<boost::endian::order>(blob[offset] & 0x00000001);
    offset++;

    if (is_empty) {
        return {};
    }

    srid = ngen::copy_from<std::uint32_t>(
        boost::span<const uint8_t>{blob.data() + offset, sizeof(std::uint32_t)},
        order
    );
    offset += sizeof(std::uint32_t);

    if (indicator > 0 && indicator < 5) {
        // Ignore the bounding box
        offset += sizeof(double) * 4;

        // ensure `index` is at beginning of data
        if (indicator == 2 || indicator == 3) {
            offset += 2 * sizeof(double);
        } else if (indicator == 4) {
            offset += 4 * sizeof(double);
        }
    }
    
    boost::span<const uint8_t> data{
        blob.data() + offset,
        blob.data() + blob.size()
    };

    return ngen::read_wkb(data);
}

Feature build_feature(
    const ngen::sqlite::database::iterator& row,
    const std::string& id_col,
    const std::string& geom_col
)
{
    auto srid  = std::uint32_t{0};
    auto id    = row.get<boost::json::string>(id_col);
    auto props = build_properties(row, geom_col);
    auto geom  = build_geometry(row, geom_col, srid);

    return {
        /*geometry=*/std::move(geom),
        /*properties=*/std::move(props),
        /*srid=*/std::move(srid),
        /*id=*/std::move(id)
    };
}

} // namespace ngen
#endif // #if NGEN_WITH_SQLITE
