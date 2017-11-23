#include <nitro/jiffy/jiffy.hpp>

void print_now()
{
    nitro::jiffy::Jiffy j;

    std::cout << j << std::endl;
}

int main()
{
    for (int i = 0; i < 100; i++)
    {
        print_now();
    }
}
