#pragma once

#include "Grid.hpp"

#include <cstdint>

#include <boost/core/span.hpp>

namespace ngen {

struct Variable
{
    enum class dtype { i32, i64, f32, f64 };

    Variable() = default;

    Variable(
      void* data,
      dtype type,
      std::size_t align,
      std::size_t size,
      const Grid* grid
    )
      : data_(data)
      , type_(type)
      , align_(align)
      , size_(size)
      , grid_(grid)
    {}


    dtype type() const noexcept
    {
        return type_;
    }

    template<typename Tp>
    boost::span<Tp> unsafe_values()
    {
        return { static_cast<Tp*>(data_), size_ };
    }

    template<typename Tp>
    boost::span<const Tp> unsafe_values() const
    {
        return { static_cast<const Tp*>(data_), size_ };
    }

    template<typename Tp>
    boost::span<Tp> values()
    {
        assert_type<Tp>();
        return unsafe_values<Tp>();
    }

    template<typename Tp>
    boost::span<const Tp> values() const
    {
        assert_type<Tp>();
        return unsafe_values<Tp>();
    }

    const Grid* grid() const
    {
        return grid_;
    }

    std::size_t align() const noexcept
    {
        return align_;
    }

    std::size_t size() const noexcept
    {
        return size_;
    }

  private:

    template<typename Tp>
    constexpr dtype cxx_to_dtype();

    template<> constexpr dtype cxx_to_dtype<std::int32_t>() { return dtype::i32; }
    template<> constexpr dtype cxx_to_dtype<std::int64_t>() { return dtype::i64; }
    template<> constexpr dtype cxx_to_dtype<float>()        { return dtype::f32; }
    template<> constexpr dtype cxx_to_dtype<double>()       { return dtype::f64; }
    
    template<typename Tp>
    void assert_type()
    {
        assert(cxx_to_dtype<Tp>() == type_);
    }

    void*       data_;
    dtype       type_;
    std::size_t align_;
    std::size_t size_;
    const Grid* grid_;
};

std::ostream& operator<<(std::ostream& stream, const Variable& variable);

} // namespace ngen
