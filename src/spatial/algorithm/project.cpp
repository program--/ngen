#include <spatial/algorithm/project.hpp>

#include <boost/geometry/srs/transformation.hpp>

namespace ngen {

using GeographicPoint = boost::geometry::model::d2::point_xy<
    double,
    boost::geometry::cs::geographic<boost::geometry::degree>
>;

// Only EPSG codes we need
boost::container::flat_map<
    std::size_t,
    boost::geometry::srs::dpar::parameters<double>
> extra_epsg_definitions = {
    {3857, srs_type(dpar::proj_merc)(dpar::units_m)(dpar::no_defs)(dpar::a, 6378137)(dpar::b, 6378137)(dpar::lat_ts, 0)(dpar::lon_0, 0)(dpar::x_0, 0)(dpar::y_0, 0)(dpar::k, 1)},
    {4326, srs_type(dpar::proj_longlat)(dpar::ellps_wgs84)(dpar::datum_wgs84)(dpar::no_defs)},
    {5070, srs_type(dpar::proj_aea)(dpar::ellps_grs80)(dpar::towgs84, {0,0,0,0,0,0,0})(dpar::lat_0, 23)(dpar::lon_0, -96)(dpar::lat_1, 29.5)(dpar::lat_2, 45.5)(dpar::x_0, 0)(dpar::y_0, 0)}
};

// similar to boost::geometry::projections::detail::epsg_to_parameters
srs_type epsg_to_parameters(std::size_t srid)
{
    const auto pos = extra_epsg_definitions.find(srid);
    if (pos == extra_epsg_definitions.end()) {
        throw std::runtime_error("SRID " + std::to_string(srid) + " is not supported. Project the input data to EPSG:5070 or EPSG:4326.");
    }

    return pos->second;
}

struct ProjectionVisitor : public boost::static_visitor<void>
{
    ProjectionVisitor(std::size_t from, std::size_t to)
      : from_(from)
      , to_(to)
    {}

    template<typename Tp>
    void operator()(Tp& geom) const
    {
        project_inplace(geom, from_, to_);
    }

    void operator()(GeometryCollection& geom) const
    {
        for (Geometry& g : geom) {
            boost::apply_visitor(*this, g);
        }
    }

  private:
    std::size_t from_;
    std::size_t to_;
};

template<>
void project_inplace(Point& geom, std::size_t from_srid, std::size_t to_srid)
{
    if (from_srid == to_srid) {
        return;
    }

    // TODO: Make it so this doesn't need to be constructed each time
    // TODO: Possibly use grid storage?
    auto from = epsg_to_parameters(from_srid);
    auto to   = epsg_to_parameters(to_srid);
    auto tr   = boost::geometry::srs::transformation<>{from, to};

    if (from_srid == 4326 && to_srid != 4326) {
        GeographicPoint lhs = { geom.x(), geom.y() };
        tr.forward(lhs, geom);
    } else if (from_srid != 4326 && to_srid == 4326) {
        GeographicPoint rhs;
        tr.forward(geom, rhs);
        geom = { rhs.x(), rhs.y() };
    } else { // from_srid != 4326 && to_srid != 4326 && from_srid != to_srd
        Point rhs;
        tr.forward(geom, rhs);
        geom = { rhs.x(), rhs.y() };
    }
}

template<>
void project_inplace(Geometry& geom, std::size_t from_srid, std::size_t to_srid)
{
    boost::apply_visitor(ProjectionVisitor{from_srid, to_srid}, geom);
}

void project_inplace(Feature& feature, std::size_t new_srid)
{
    project_inplace(feature.geometry(), feature.srid(), new_srid);
}

void project_inplace(FeatureCollection& fc, std::size_t new_srid)
{
    for (Feature& feature : fc) {
        project_inplace(feature, new_srid);
    }
}

} // namespace ngen
