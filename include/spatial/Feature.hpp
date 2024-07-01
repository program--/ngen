#pragma once

#include "Geometry.hpp"

#include <boost/json.hpp>
#include <boost/core/span.hpp>
#include <boost/container/small_vector.hpp>

namespace ngen {

struct Feature
{
    using map_type        = boost::json::object;
    using key_type        = map_type::key_type;
    using mapped_type     = map_type::mapped_type;
    using value_type      = map_type::value_type;
    using size_type       = map_type::size_type;
    using reference       = mapped_type&;
    using const_reference = const mapped_type&;
    using iterator        = map_type::iterator;
    using const_iterator  = map_type::const_iterator;

    static constexpr size_type default_srid = 4326;

    struct compare;

    Feature();

    Feature(
        Geometry geometry,
        boost::json::object properties,
        size_type srid = default_srid,
        boost::json::string id = ""
    );

    virtual ~Feature() = default;

    GeometryType geometry_type() const noexcept;
    Box envelope() const noexcept;
    boost::core::string_view id() const noexcept;
    size_type srid() const noexcept;
    boost::json::object& properties() noexcept;
    const boost::json::object& properties() const noexcept;
    ngen::Geometry& geometry() noexcept;
    const ngen::Geometry& geometry() const noexcept;
    void promote_id(boost::core::string_view property);
    void add_upstream(Feature& feature);
    void remove_upstream(Feature& feature);
    boost::span<const Feature* const> upstream() const noexcept;
    void add_downstream(Feature& feature);
    void remove_downstream(Feature& feature);
    boost::span<const Feature* const> downstream() const noexcept;
    bool is_root() const noexcept;
    bool is_leaf() const noexcept;

  protected:
    boost::json::string id_; // uses small string optimization

  private:
    static constexpr auto num_preallocated_links = 4;
    using link_vector = boost::container::small_vector<
        Feature*,
        num_preallocated_links
    >;

    boost::json::object props_;
    size_type           srid_;
    Geometry            geom_;
    link_vector         upstream_{};
    link_vector         downstream_{};
};

struct Feature::compare
{
    // Enable transparent comparator. See overload (2)
    // of https://en.cppreference.com/w/cpp/container/set/count.
    // (or the specific proposal N3465/N3657)
    //
    // Note: boost::container::flat_set also allows transparent comparators.
    using is_transparent = void;

    bool operator()(const Feature& lhs, const Feature& rhs) const noexcept
    {
        return lhs.id_ < rhs.id_;
    }

    bool operator()(const Feature& lhs, boost::core::string_view rhs) const noexcept
    {
        return lhs.id_ < rhs;
    }

    bool operator()(boost::core::string_view lhs, const Feature& rhs) const noexcept
    {
        return lhs < rhs.id_;
    }
};

} // namespace ngen
