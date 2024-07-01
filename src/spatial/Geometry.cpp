#include <spatial/Geometry.hpp>

#include <cmath>

#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/equals.hpp>

namespace ngen {

struct EqualityVisitor : public boost::static_visitor<bool>
{
    template<typename T, typename U>
    bool operator()(const T& a, const U& b) const noexcept
    {
        return false;
    }

    template<typename T>
    bool operator()(const T& a, const T& b) const noexcept
    {
        return boost::geometry::equals(a, b);
    }

    bool operator()(const GeometryCollection& a, const GeometryCollection& b) const noexcept
    {
        if (a.size() != b.size()) {
            return false;
        }
    
        for (auto i = 0; i < a.size(); ++i) {
            if (!boost::apply_visitor(*this, a[i], b[i])) {
                return false;
            }
        }

        return true;
    }
};

Geometry::Geometry()
  : base_type(ngen::Point{NAN, NAN})
{}

GeometryType Geometry::type() const noexcept
{
    if (empty()) {
        return GeometryType::EMPTY;
    }

    // We can safely do this as long as the variant types
    // align with the GeometryType enum. Otherwise, we need
    // to dispatch using `data_.apply_visitor()`
    return static_cast<GeometryType>(this->which() + 1);
}

Box Geometry::envelope() const noexcept
{
    return boost::geometry::return_envelope<Box>(*this);
}

bool Geometry::empty() const noexcept
{
    if (which() != 0 /* POINT */) {
        return false;
    }

    const auto& p = boost::get<const Point>(*this);
    return std::isnan(p.x()) && std::isnan(p.y());
}

void Geometry::swap(Geometry& other) noexcept
{
    // Cast is safe since both types inherit privately and do not
    // store any additional member variables from base_type.
    base_type& lhs = static_cast<base_type&>(*this);
    base_type& rhs = static_cast<base_type&>(other);
    lhs.swap(rhs);
}

bool Geometry::operator==(const Geometry& rhs) const noexcept
{
    return boost::apply_visitor(EqualityVisitor{}, *this, rhs);
}

} // namespace ngen
