#include <nitro/jiffy/jiffy.hpp>

#include <nitro/except/raise.hpp>
#include <nitro/format/format.hpp>

extern "C"
{
#include <sys/time.h>
#include <time.h>
}

static_assert(std::is_same<std::tm, struct tm>::value,
              "std::tm must be the same type as struct tm. It ain't though :(");

extern long timezone;

namespace nitro
{
namespace jiffy
{

    Jiffy::Jiffy(std::chrono::system_clock::time_point tp) : tp_(tp)
    {
        tp_ += std::chrono::seconds(timezone);
    }

    Jiffy::Jiffy(const std::string& date) : Jiffy(date, "%Y-%m-%dT%H:%M:%S%z")
    {
    }

    Jiffy::Jiffy(const std::string& date, const std::string& format) : update_needed_(false)
    {
        clear();

        auto res = strptime(date.c_str(), format.c_str(), &tm_data_);

        if (res == nullptr || res != date.c_str() + date.size())
        {
            nitro::except::raise("Couldn't parse time string '{}'"_nf % date);
        }

        std::time_t tp = timelocal(&tm_data_);
        if (tp == -1)
        {
            nitro::except::raise("Couldn't convert given time point into timestamp");
        }

        tp_ = std::chrono::system_clock::from_time_t(tp);
    }
} // namespace jiffy
} // namespace nitro
