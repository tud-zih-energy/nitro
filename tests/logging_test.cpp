#ifndef NITRO_LOG_MIN_SEVERITY
#error "NITRO_LOG_MIN_SEVERITY should be set by the build system, but isn't!"
#endif

#include <catch2/catch.hpp>

#ifdef NITRO_LOG_MIN_SEVERITY
#undef NITRO_LOG_MIN_SEVERITY
#endif
#define NITRO_LOG_MIN_SEVERITY info

#include <nitro/log/attribute/severity.hpp>
#include <nitro/log/attribute/timestamp.hpp>
#include <nitro/log/filter/severity_filter.hpp>
#include <nitro/log/log.hpp>
#include <nitro/log/sink/logfile.hpp>
#include <nitro/log/sink/sequence.hpp>
#include <nitro/log/sink/stderr.hpp>
#include <nitro/log/sink/stdout.hpp>

#include <nitro/format.hpp>

namespace detail
{

using Sink = nitro::log::sink::sequence<nitro::log::sink::StdOut, nitro::log::sink::StdErr,
                                        nitro::log::sink::Logfile>;

typedef nitro::log::record<nitro::log::tag_attribute, nitro::log::message_attribute,
                           nitro::log::severity_attribute,
                           nitro::log::timestamp_clock_attribute<std::chrono::system_clock>>
    record;

template <typename Record>
class log_formater
{
public:
    std::string format(Record& r)
    {
        if (r.tag().empty())
        {
            return nitro::format("[{}][{}]: {}\n") % r.timestamp().time_since_epoch().count() %
                   r.severity() % r.message();
        }
        else
        {
            return nitro::format("[{}][{}][{}]: {}\n") % r.timestamp().time_since_epoch().count() %
                   r.tag() % r.severity() % r.message();
        }
    }
};

template <typename Record>
using log_filter = nitro::log::filter::severity_filter<Record>;
} // namespace detail

using logging =
    nitro::log::logger<detail::record, detail::log_formater, detail::Sink, detail::log_filter>;

struct StaticInit
{
    StaticInit()
    {
        nitro::log::sink::Logfile::log_file() = "test_log.txt";
    }
} init_me;

TEST_CASE("Without tag works", "[log]")
{
    SECTION("As one statement")
    {
        logging::fatal() << "Test 1";
        logging::error() << "Test 2";
        logging::warn() << "Test 3";
        logging::info() << "Test 4";
        logging::debug() << "Test 5";
        logging::trace() << "Test 6";
    }

    SECTION("As multiple statements")
    {
        {
            auto log = logging::fatal();
            log << "test 7";
        }

        {
            auto log = logging::error();
            log << "test 8";
        }

        {
            auto log = logging::warn();
            log << "test 9";
        }

        {
            auto log = logging::info();
            log << "test 10";
        }

        {
            auto log = logging::debug();
            log << "test 11";
        }

        {
            auto log = logging::trace();
            log << "test 12";
        }
    }
}

TEST_CASE("With tag works", "[log]")
{
    SECTION("As one statement")
    {
        logging::fatal("test tag") << "Test 13";
        logging::error("test tag") << "Test 14";
        logging::warn("test tag") << "Test 15";
        logging::info("test tag") << "Test 16";
        logging::debug("test tag") << "Test 17";
        logging::trace("test tag") << "Test 18";
    }

    SECTION("As multiple statements")
    {
        {
            auto log = logging::fatal("test tag");
            log << "test 19";
        }

        {
            auto log = logging::error("test tag");
            log << "test 20";
        }

        {
            auto log = logging::warn("test tag");
            log << "test 21";
        }

        {
            auto log = logging::info("test tag");
            log << "test 22";
        }

        {
            auto log = logging::debug("test tag");
            log << "test 23";
        }

        {
            auto log = logging::trace("test tag");
            log << "test 24";
        }
    }
}

TEST_CASE("Logging lambdas works", "[log]")
{
    SECTION("As one statement")
    {
        logging::fatal() << []() { return "test 25"; };
        logging::error() << []() { return "test 26"; };
        logging::warn() << []() { return "test 27"; };
        logging::info() << []() { return "test 28"; };
        logging::debug() << []() { return "test 29"; };
        logging::trace() << []() { return "test 30"; };
    }

    SECTION("As multiple statements")
    {
        {
            auto log = logging::fatal();
            log << []() { return "test 31"; };
        }

        {
            auto log = logging::error();
            log << []() { return "test 32"; };
        }

        {
            auto log = logging::warn();
            log << []() { return "test 33"; };
        }

        {
            auto log = logging::info();
            log << []() { return "test 34"; };
        }

        {
            auto log = logging::debug();
            log << []() { return "test 35"; };
        }

        {
            auto log = logging::trace();
            log << []() { return "test 36"; };
        }
    }

    SECTION("Evaluation of the lambda is lazy")
    {
        int i = 0;

        logging::fatal() << [&i]() {
            ++i;
            return "test 37";
        };

        CHECK(i == 1);

        logging::error() << [&i]() {
            ++i;
            return "test 38";
        };

        CHECK(i == 2);

        logging::warn() << [&i]() {
            ++i;
            return "test 39";
        };

        CHECK(i == 3);

        logging::info() << [&i]() {
            ++i;
            return "test 40";
        };

        CHECK(i == 4);

        logging::debug() << [&i]() {
            ++i;
            return "test 41";
        };

        CHECK(i == 4);

        logging::trace() << [&i]() {
            ++i;
            return "test 42";
        };

        CHECK(i == 4);
    }
}
