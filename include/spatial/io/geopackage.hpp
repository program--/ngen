#pragma once

#include <spatial/FeatureCollection.hpp>

namespace ngen {

FeatureCollection read_geopackage(
    boost::core::string_view path,
    std::string layer,
    boost::span<const std::string> ids = {}
);

} // namespace ngen
