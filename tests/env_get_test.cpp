#include <nitro/env/get.hpp>

#include <cassert>
#include <iostream>

int main()
{
    auto set = nitro::env::get("TEST_1");

    assert(set == "THIS_WAS_SET");

    auto not_set = nitro::env::get("TEST_2", "THIS_WAS_NOT_SET");

    assert(set == "THIS_WAS_NOT_SET");

    return 0;
}
