#pragma once

#include "date.h"
#include <map>

std::map<date::month, int> month_map = {
    {date::January, 31},
    {date::February, 28},
    {date::March, 31},
    {date::April, 30}
};