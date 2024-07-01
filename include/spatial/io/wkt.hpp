#pragma once

#include <spatial/Geometry.hpp>

#include <boost/geometry/io/wkt/read.hpp>
#include <boost/geometry/io/wkt/write.hpp>

namespace ngen {

inline ngen::Geometry read_wkt(const std::string& str)
{
    return boost::geometry::from_wkt<ngen::Geometry>(str);
}

inline std::string write_wkt(ngen::Geometry geom)
{
    return boost::geometry::to_wkt(geom);
}

} // namespace ngen
