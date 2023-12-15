#include <geometry/backends/boost/boost_linestring.hpp>

namespace ngen {
namespace spatial {
namespace backend {

boost_linestring::boost_linestring() = default;

boost_linestring::~boost_linestring() = default;

boost_linestring::boost_linestring(std::initializer_list<boost_point> pts)
  : data_(pts){};

auto boost_linestring::size() const noexcept -> size_type
{
    return data_.size();
}

auto boost_linestring::get(size_type n) -> reference
{
    return data_.at(n);
}

auto boost_linestring::get(size_type n) const -> const_reference
{
    return data_.at(n);
}

void boost_linestring::set(size_type n, const ngen::spatial::point& pt)
{
    // TODO: This might be expensive
    auto casted = dynamic_cast<const_reference>(pt);
    get(n) = casted;
}

void boost_linestring::resize(size_type n)
{
    data_.resize(n);
}

auto boost_linestring::front() noexcept -> reference
{
    return data_.front();
}

auto boost_linestring::back() noexcept -> reference
{
    return data_.back();
}

auto boost_linestring::front() const noexcept -> const_reference
{
    return data_.front();
}

auto boost_linestring::back() const noexcept -> const_reference
{
    return data_.back();
}

void boost_linestring::swap(linestring& other) noexcept
{
    linestring* ptr = &other;

    auto line = dynamic_cast<boost_linestring*>(ptr);
    if (line != nullptr) {
        data_.swap(line->data_);
        return;
    }
}

void boost_linestring::clone(linestring& other) noexcept
{
    linestring* ptr = &other;

    auto line = dynamic_cast<boost_linestring*>(ptr);
    if (line != nullptr) {
        data_ = line->data_;
    } else {
        const auto size_ = ptr->size();
        data_.resize(ptr->size());
        for (size_type i = 0; i < size_; i++) {
            set(i, ptr->get(i));
        }
    }
}

} // namespace backend
} // namespace spatial
} // namespace ngen
