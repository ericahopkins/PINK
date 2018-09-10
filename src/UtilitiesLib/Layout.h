/**
 * @file   UtilitiesLib/Layout.h
 * @date   Aug 30, 2018
 * @author Bernd Doser, HITS gGmbH
 */

#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <numeric>

namespace pink {

//! n-dimensional cartesian layout for SOM
template <uint8_t dim, typename T>
class Cartesian
{
public:

    typedef T value_type;

    Cartesian(std::array<uint32_t, dim> length = {0})
     : length(length),
	   data(std::accumulate(length.begin(), length.end(), 1, std::multiplies<uint32_t>()))
    {}

    T& get(std::array<uint32_t, dim> position)
    {
    	size_t p = 0;
    	for (uint8_t i = 0; i != dim; ++i) p += position[i] * i;
        return data[p];
    }

private:

    std::array<uint32_t, dim> length;

    std::vector<T> data;

};

//! Hexagonal layout for SOM, only supported for 2-dimensional
template <typename T>
class Hexagonal
{
public:

    typedef T value_type;

    Hexagonal(uint32_t length)
     : length(length),
	   data(get_size(length))
    {}

    T& get(uint32_t position)
    {
        return data[position];
    }

private:

    //! Return number of elements for a regular hexagon
    size_t get_size(uint32_t length) const
    {
    	uint32_t radius = (length - 1)/2;
        return length * length - radius * (radius + 1);
    }

    uint32_t length;

    std::vector<T> data;

};

//! SOM layout type
enum class Layout {
	CARTESIAN,
    HEXAGONAL
};

//! Pretty printing of SOM layout type
inline std::ostream& operator << (std::ostream& os, Layout layout)
{
    if (layout == Layout::CARTESIAN) os << "cartesian";
    else if (layout == Layout::HEXAGONAL) os << "hexagonal";
    else os << "undefined";
    return os;
}

} // namespace pink
