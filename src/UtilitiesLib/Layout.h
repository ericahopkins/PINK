/**
 * @file   UtilitiesLib/Layout.h
 * @date   Aug 30, 2018
 * @author Bernd Doser, HITS gGmbH
 */

#pragma once

#include <iostream>

namespace pink {

template <int dim>
struct Cartesian {};

struct Hexagonal {};

//! SOM layout type
enum class Layout {
    QUADRATIC,
    HEXAGONAL
};

//! Pretty printing of SOM layout type
inline std::ostream& operator << (std::ostream& os, Layout layout)
{
    if (layout == Layout::QUADRATIC) os << "quadratic";
    else if (layout == Layout::HEXAGONAL) os << "hexagonal";
    else os << "undefined";
    return os;
}

} // namespace pink
