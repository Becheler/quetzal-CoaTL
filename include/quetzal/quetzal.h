// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/***********************************************************************                                                                         *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***************************************************************************/

// Header file including all coalescence module

#include "coalescence.h"

#include "random.h"

#include "demography.h"

#include "geography.h"

#include "expressive.h"
#include "genetics.h"

#include "fuzzy_transfer_distance.h"

#include "simulator/DiscreteTimeWrightFisher.h"
#include "simulator/ForwardBackwardSpatiallyExplicit.h"
#include "simulator/utils.h"

#include "utils.h"
#include "version.h"

///
/// @brief Generic components for simulating integrated distribution, demographic and coalescence models.
///
namespace quetzal
{}
