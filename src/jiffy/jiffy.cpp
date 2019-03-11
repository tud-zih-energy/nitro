#include <nitro/jiffy/jiffy.hpp>

#include <nitro/except/raise.hpp>

#include <iomanip>
#include <ratio>
#include <regex>
#include <sstream>
#include <type_traits>

#include <cstring>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define USE_CXX_STD_SYSTEM_TIME
#endif

extern "C"
{
#ifndef USE_CXX_STD_SYSTEM_TIME
#include <sys/time.h>
#endif
#include <time.h>
}

extern long timezone;

static_assert(std::is_same<std::tm, struct tm>::value,
              "std::tm must be the same type as struct tm. It ain't though :(");

namespace nitro
{
namespace jiffy
{
    Jiffy::Jiffy() : Jiffy(std::chrono::system_clock::now())
    {
    }

    Jiffy::Jiffy(std::chrono::system_clock::time_point tp) : tp_(tp)
    {
        fraction_ = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()) %
                    std::micro::den;
    }

    Jiffy::Jiffy(const std::string& date, const std::string& format)
    : update_needed_(false), fraction_(0)
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

        if (res == nullptr || res != date.c_str() + date.size())
        {
            nitro::except::raise("Couldn't parse time string '", date, "'");
        }
#endif
        auto a = timelocal(&tm_data_);
        auto b = timegm(&tm_data_);
        auto c = mktime(&tm_data_);

        auto offset = timezone - tm_data_.tm_gmtoff;
        std::time_t tp = timegm(&tm_data_) + offset;
        if (tp == -1)
        {
            nitro::except::raise("Couldn't convert given time point into timestamp");
        }

        tp_ = std::chrono::system_clock::from_time_t(tp);
    }

    std::string Jiffy::format(std::string fmt) const
    {
        update_cached_tm();

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

    void Jiffy::update_cached_tm() const
    {
        if (update_needed_)
        {
            update_needed_ = false;

            clear();

            auto time = std::chrono::system_clock::to_time_t(tp_);
            auto res = std::localtime(&time);

            if (res == nullptr)
            {
                nitro::except::raise("Couldn't get localtime for time point.");
            }

            tm_data_ = *res;
        }
    }

    int Jiffy::year() const
    {
        update_cached_tm();
        return tm_data_.tm_year + 1900;
    }

    int Jiffy::month() const
    {
        update_cached_tm();
        return tm_data_.tm_mon + 1;
    }

    int Jiffy::day() const
    {
        update_cached_tm();
        return tm_data_.tm_mday;
    }

    int Jiffy::hour() const
    {
        update_cached_tm();
        return tm_data_.tm_hour;
    }

    int Jiffy::minute() const
    {
        update_cached_tm();
        return tm_data_.tm_min;
    }

    int Jiffy::second() const
    {
        update_cached_tm();
        return tm_data_.tm_sec;
    }

    int Jiffy::microsecond() const
    {
        // this is in the range [0, 1e6), so it's fine
        return static_cast<int>(fraction_.count());
    }

    Jiffy::operator std::tm() const
    {
        update_cached_tm();
        return tm_data_;
    }

    Jiffy::operator std::chrono::system_clock::time_point() const
    {
        return tp_;
    }

    void Jiffy::clear() const
    {
        std::memset(&tm_data_, 0, sizeof(tm_data_));
    }
} // namespace jiffy
} // namespace nitro
