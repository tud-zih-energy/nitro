#ifndef INCLUDE_NITRO_LOG_SINK_STDOUT_OMP_HPP
#define INCLUDE_NITRO_LOG_SINK_STDOUT_OMP_HPP

#include <string>
#include <iostream>

namespace nitro { namespace log { namespace sink {

    class stdout_omp
    {
    public:

        void sink(std::string formatted_record)
        {
            #pragma omp critical
            {
                std::cout << formatted_record << std::endl;
            }
        }
    };


}}} //namespace nitro::log::sink

#endif //INCLUDE_NITRO_LOG_SINK_STDOUT_HPP
