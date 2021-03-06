//
// Created by konch on 4/12/21.
//

#include "burst.hpp"

#include <cassert>
#include <stdexcept>

Burst::Burst(BurstType type, int length)
{
    this->burst_type = type;
    this->length = length;
}

void Burst::update_time(int delta_t)
{
    this->length = length - delta_t;
}