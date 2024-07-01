#pragma once

#include <cstdint>
#include <cassert>
#include <boost/endian.hpp>
#include <boost/core/span.hpp>

namespace ngen {

template<typename T>
void to_native_inplace(boost::endian::order order, T& x)
{
    if (order == boost::endian::order::little) {
        boost::endian::little_to_native_inplace(x);
    } else {
        boost::endian::big_to_native_inplace(x);
    }
}

/**
 * @brief
 * Copies bytes from @param{src} to @param{dst},
 * converts the endianness to native,
 * and increments @param{index} by the number of bytes
 * used to store @tparam{S}
 * 
 * @tparam S a primitive type
 * @param src a vector of bytes
 * @param index an integral type tracking the starting position of @param{dst}'s memory
 * @param dst output primitive
 * @param order endianness value (0x01 == Little; 0x00 == Big)
 */
template<typename S>
S copy_from(boost::span<const uint8_t> src, boost::endian::order order)
{
    assert(src.size_bytes() >= (src.data() + sizeof(S)) - src.data());

    S dst{};
    std::memcpy(&dst, src.data(), sizeof(S));
    to_native_inplace(order, dst);
    return dst;
}

// boost::endian doesn't support using primitive doubles
// see: https://github.com/boostorg/endian/issues/36
template<>
inline double copy_from<double>(boost::span<const uint8_t> src, boost::endian::order order)
{
    static_assert(sizeof(uint64_t) == sizeof(double), "sizeof(uint64_t) is not the same as sizeof(double)!");

    double dst{};
    uint64_t tmp = copy_from<uint64_t>(src, order);
    std::memcpy(&dst, &tmp, sizeof(double));
    return dst;
}

} // namespace ngen
