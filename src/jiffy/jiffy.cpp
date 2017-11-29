#include <nitro/jiffy/jiffy.hpp>

#include <nitro/except/raise.hpp>

#include <iomanip>
#include <regex>
#include <sstream>
#include <type_traits>

#include <cstring>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define USE_CXX_STD_SYSTEM_TIME
#endif

extern "C" {
#ifndef USE_CXX_STD_SYSTEM_TIME
#include <sys/time.h>
#endif
#include <time.h>
}

static_assert(std::is_same<std::tm, struct tm>::value,
              "std::tm must be the same type as struct tm. It ain't though :(");

namespace nitro
{
namespace jiffy
{

    Jiffy::Jiffy()
#ifdef USE_CXX_STD_SYSTEM_TIME
    : Jiffy(std::chrono::system_clock::now())
    {
    }
#else
    {
        clear();

        timeval tv;

        int result = gettimeofday(&tv, nullptr);

        if (result != 0)
        {
            nitro::except::raise("Couldn't get time of day.");
        }

        // localtime_r isn't required to behave like tzset was called, so we do it
        tzset();

        auto res = localtime_r(&tv.tv_sec, &tm_data_);

        if (res == nullptr)
        {
            nitro::except::raise("Couldn't get localtime for time point.");
        }

        fraction_ = std::chrono::microseconds(tv.tv_usec);
    }
#endif

    Jiffy::Jiffy(std::chrono::system_clock::time_point tp)
    {
        clear();

        auto time = std::chrono::system_clock::to_time_t(tp);

#ifdef USE_CXX_STD_SYSTEM_TIME

        auto res = std::localtime(&time);

        if (res == nullptr)
        {
            nitro::except::raise("Couldn't get localtime for time point.");
        }

        tm_data_ = *res;

#else
        // localtime_r isn't required to behave like tzset was called, so we do it
        tzset();

        auto res = localtime_r(&time, &tm_data_);

        if (res == nullptr)
        {
            nitro::except::raise("Couldn't get localtime for time point.");
        }
#endif
    }

    Jiffy::Jiffy(const std::string& date, const std::string& format)
    {
        clear();
#ifdef USE_CXX_STD_SYSTEM_TIME
        std::stringstream s;

        s << date;
        s >> std::get_time(&tm_data_, format.c_str());

        if (s.fail())
        {
            nitro::except::raise("Couldn't parse time string '", date, "'");
        }
#else
        auto res = strptime(date.c_str(), format.c_str(), &tm_data_);

        if (res == nullptr)
        {
            nitro::except::raise("Couldn't parse time string '", date, "'");
        }
#endif
    }

    std::string Jiffy::format(std::string fmt) const
    {
        if (fmt.find("%f") != std::string::npos)
        {
            std::stringstream s;

            s << std::setfill('0') << std::setw(6) << fraction_.count();

            fmt = std::regex_replace(fmt, std::regex("%f"), s.str());
        }
        // as fmt == "%p" could result in an empty string, we need to
        // force at least on character in the output format.
        // So if it fits, size will be at least one.
        fmt += '\a';
        std::string buffer;
        std::size_t size_in = std::max(std::size_t(200), fmt.size());
        std::size_t size;

        do
        {
            size_in *= 1.6;
            buffer.resize(size_in);

            size = strftime(&buffer[0], buffer.size(), fmt.data(), &tm_data_);
        } while (size == 0);

        // remove the trailing additional character
        buffer.resize(size - 1);

        return buffer;
    }

    void Jiffy::clear()
    {
        std::memset(&tm_data_, 0, sizeof(tm_data_));
    }
}
}
