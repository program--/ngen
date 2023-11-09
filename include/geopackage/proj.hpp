#ifndef NGEN_GEOPACKAGE_PROJ_HPP
#define NGEN_GEOPACKAGE_PROJ_HPP

#include <boost/geometry/srs/projection.hpp>

namespace ngen {
namespace srs {

namespace bg = boost::geometry;

struct epsg
{
    using srs_type = bg::srs::dpar::parameters<double>;

    enum {
        wgs84        = 4326,
        conus_albers = 5070,
        mercator     = 3857
    };

    static srs_type get(uint32_t srid);

  private:
    static const srs_type epsg5070_;
    static const srs_type epsg3857_;
};

} // namespace srs
} // namespace ngen

#endif // NGEN_GEOPACKAGE_PROJ_HPP
