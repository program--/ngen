#pragma once

#include <cstdint>
#include <stdexcept>

#include <boost/core/span.hpp>

#include <spatial/Geometry.hpp>

namespace ngen {

struct invalid_wkb_exception
  : public std::runtime_error
{
    invalid_wkb_exception(std::string what)
      : std::runtime_error(std::move(what))
    {}
};

ngen::Geometry read_wkb(boost::span<const std::uint8_t> data);

} // namespace ngen
