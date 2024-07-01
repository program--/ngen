#pragma once

#include <spatial/Geometry.hpp>
#include <spatial/Feature.hpp>
#include <spatial/FeatureCollection.hpp>

#include <boost/container/flat_map.hpp>
#include <boost/geometry/srs/projection.hpp>

#include <boost/geometry/algorithms/for_each.hpp>

namespace ngen {

namespace dpar = boost::geometry::srs::dpar; // epsg_to_parameters
using srs_type = dpar::parameters<double>; // epsg_to_parameters

srs_type epsg_to_parameters(std::size_t srid);

template<typename G>
void project_inplace(G& geom, std::size_t from_srid, std::size_t to_srid)
{
    boost::geometry::for_each_point(geom, [from_srid, to_srid](Point& p) {
        project_inplace(p, from_srid, to_srid);
    });
}

template<>
void project_inplace(Point& geom, std::size_t from_srid, std::size_t to_srid);

template<>
void project_inplace(Geometry& geom, std::size_t from_srid, std::size_t to_srid);

void project_inplace(Feature& feature, std::size_t new_srid);
void project_inplace(FeatureCollection& fc, std::size_t new_srid);

template<typename G>
G project(G&& geom, std::size_t from_srid, std::size_t to_srid)
{
    G new_geom = std::forward<G>(geom);
    project_inplace(new_geom, from_srid, to_srid);
    return new_geom;
}

template<typename F>
F project(F&& feature, std::size_t new_srid)
{
    F new_feat = std::forward<F>(feature);
    project_inplace(new_feat, new_srid);
    return new_feat;
}

}
