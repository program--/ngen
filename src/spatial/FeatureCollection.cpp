#include <spatial/FeatureCollection.hpp>

namespace ngen {

Feature& FeatureCollection::at(boost::core::string_view id)
{
    auto it = this->find(id);
    if (it == this->end()) {
        throw std::out_of_range{"Feature with ID `" + std::string{id} + "` not found"};
    }

    return *it;
}

void FeatureCollection::swap(FeatureCollection& other) noexcept
{
    this->base_type::swap(other);
}

void FeatureCollection::merge(FeatureCollection& other)
{
    this->base_type::merge(other);
}

const Box& FeatureCollection::envelope() const noexcept
{
    if (bounds_ == nullptr) {
        auto min = Point{INFINITY, INFINITY};
        auto max = Point{-INFINITY, -INFINITY};
    
        for (auto& feature : *this) {
            const Box feature_bbox = feature.envelope();
            const double feat_xmin = feature_bbox.min_corner().x();
            const double feat_ymin = feature_bbox.min_corner().y();
            const double feat_xmax = feature_bbox.max_corner().x();
            const double feat_ymax = feature_bbox.max_corner().y();

            if (feat_xmin < min.x()) {
                min.x(feat_xmin);
            }

            if (feat_ymin < min.y()) {
                min.y(feat_ymin);
            }

            if (feat_xmax > max.x()) {
                max.x(feat_xmax);
            }

            if (feat_ymax > max.y()) {
                max.y(feat_ymax);
            }
        }

        bounds_ = std::make_unique<Box>(min, max);           
    }

    return *bounds_;
}

bool FeatureCollection::try_link(boost::core::string_view key) noexcept
{
    bool any_unresolved = false;
    for (auto& feature : *this) {
        const auto* fkey = feature.properties().if_contains(key);
        if (fkey == nullptr) {
            any_unresolved = true;
            continue;
        }
        
        auto downstream_link = this->find(fkey->as_string());
        if (downstream_link == this->end()) {
            any_unresolved = true;
            continue;
        }

        feature.add_downstream(*downstream_link);
    }

    return !any_unresolved;
}

} // namespace ngen
