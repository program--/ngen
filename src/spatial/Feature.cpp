#include <spatial/Feature.hpp>

#include <boost/geometry/srs/projection.hpp>
#include <boost/geometry/srs/transformation.hpp>

namespace ngen {

Feature::Feature()
  : props_()
  , geom_(Point{NAN, NAN})
  , srid_(4326)
  , id_()
{}

Feature::Feature(Geometry geometry, boost::json::object properties, size_type srid, boost::json::string id)
  : props_(std::move(properties))
  , geom_(std::move(geometry))
  , srid_(srid)
  , id_(std::move(id))
{}

GeometryType Feature::geometry_type() const noexcept
{
    return geom_.type();
}

Box Feature::envelope() const noexcept
{
    return geom_.envelope();
}

boost::core::string_view Feature::id() const noexcept
{
    return id_;
}

Feature::size_type Feature::srid() const noexcept
{
    return srid_;
}

boost::json::object& Feature::properties() noexcept
{
    return props_;
}

const boost::json::object& Feature::properties() const noexcept
{
    return props_;
}

ngen::Geometry& Feature::geometry() noexcept
{
    return geom_;
}

const ngen::Geometry& Feature::geometry() const noexcept
{
    return geom_;
}

void Feature::promote_id(boost::core::string_view property)
{
    auto new_id_prop = props_.if_contains(property);
    if (new_id_prop != nullptr) {
        auto new_id = new_id_prop->if_string();
        if (new_id != nullptr) {
            id_ = *new_id;
        }
    }
}

void Feature::add_upstream(Feature& feature)
{
    // add `feature` as upstream link to `this`
    upstream_.emplace_back(&feature);

    // add `this` as downstream link to `feature`
    feature.downstream_.emplace_back(this);
}

void Feature::remove_upstream(Feature& feature)
{
    // Remove `this` from `feature`'s downstream links
    const auto this_pos = std::find(
        feature.downstream_.begin(),
        feature.downstream_.end(),
        this
    );

    if (this_pos != feature.downstream_.end()) {
        feature.downstream_.erase(this_pos);
    }

    // Remove `feature` from `this`'s upstream liinks
    const auto feat_pos = std::find(
        upstream_.begin(),
        upstream_.end(),
        &feature
    );

    if (feat_pos != upstream_.end()) {
        upstream_.erase(feat_pos);
    }
}

boost::span<const Feature *const> Feature::upstream() const noexcept
{
    return upstream_;
}

void Feature::add_downstream(Feature& feature)
{
    feature.add_upstream(*this);
}

void Feature::remove_downstream(Feature& feature)
{
    feature.remove_upstream(*this);
}

boost::span<const Feature* const> Feature::downstream() const noexcept
{
    return downstream_;
}

bool Feature::is_root() const noexcept
{
    return upstream_.size() == 0;
}

bool Feature::is_leaf() const noexcept
{
     return downstream_.size() == 0;
}

} // namespace ngen
