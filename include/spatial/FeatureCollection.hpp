#pragma once

#include "Feature.hpp"

#include <boost/container/flat_set.hpp>

namespace ngen {

class FeatureCollection
  : private boost::container::flat_set<Feature, Feature::compare>
{
    using base_type = boost::container::flat_set<Feature, Feature::compare>;

  public:
    using key_type        = typename base_type::key_type;
    using key_compare     = typename base_type::key_compare;
    using size_type       = typename base_type::size_type;
    using difference_type = typename base_type::difference_type;
    using value_type      = typename base_type::value_type;
    using reference       = typename base_type::reference;
    using const_reference = typename base_type::const_reference;
    using pointer         = typename base_type::pointer;
    using const_pointer   = typename base_type::const_pointer;
    using iterator        = typename base_type::iterator;
    using const_iterator  = typename base_type::const_iterator;

    using base_type::operator=;
    using base_type::size;
    using base_type::begin;
    using base_type::end;
    using base_type::empty;
    using base_type::reserve;
    using base_type::clear;
    using base_type::insert;
    using base_type::emplace;
    using base_type::contains;
    using base_type::find;
    using base_type::adopt_sequence;
    using base_type::extract_sequence;

    //! @throws std::out_of_range when `id` does not exist in the collection.
    reference at(boost::core::string_view id);
    void swap(FeatureCollection& other) noexcept;
    void merge(FeatureCollection& other);
    const Box& envelope() const noexcept;
    bool try_link(boost::core::string_view key) noexcept;

  private:
    mutable std::unique_ptr<Box> bounds_ = nullptr;
};

} // namespace ngen
