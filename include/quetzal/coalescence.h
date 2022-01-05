// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

#include "coalescence/policies/merger.h"
#include "coalescence/policies/distance_to_parent.h"
#include "coalescence/container/Tree.h"
#include "coalescence/container/Forest.h"
#include "coalescence/occupancy_spectrum/in_memoized_distribution.h"
#include "coalescence/occupancy_spectrum/on_the_fly.h"

namespace quetzal
{
  ///
  /// @brief Generic components for simulation of coalescent trees
  ///
  namespace coalescence
  {
    ///
    /// @brief Generic algorithms for manipulating abstract coalescent trees
    ///
    namespace algorithm
    {}
    ///
    /// @brief Generic data structures for manipulating abstract coalescent trees
    ///
    namespace container
    {}
  }
}
