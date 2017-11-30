#ifndef NITRO_LOG_MIN_SEVERITY
#error "NITRO_LOG_MIN_SEVERITY should be set by the build system, but isn't!"
#endif

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#ifdef NITRO_LOG_MIN_SEVERITY
#undef NITRO_LOG_MIN_SEVERITY
#endif
#define NITRO_LOG_MIN_SEVERITY info

#include <nitro/log/attribute/severity.hpp>
#include <nitro/log/filter/severity_filter.hpp>
#include <nitro/log/log.hpp>
#include <nitro/log/sink/sequence.hpp>
#include <nitro/log/sink/stderr.hpp>
#include <nitro/log/sink/syslog.hpp>

namespace detail
{

using Sink = nitro::log::sink::sequence<nitro::log::sink::Syslog, nitro::log::sink::StdErr>;

typedef nitro::log::record<nitro::log::tag_attribute, nitro::log::message_attribute,
                           nitro::log::severity_attribute>
    record;

template <typename Record>
class log_formater
{
public:
    std::string format(Record& r)
    {
        std::stringstream s;

        s << "[";

        if (!r.tag().empty())
        {
            s << r.tag() << "][";
        }

        s << r.severity() << "]: " << r.message() << "\n";

        return s.str();
    }
};

template <typename Record>
using log_filter = nitro::log::filter::severity_filter<Record>;
}

using logging =
    nitro::log::logger<detail::record, detail::log_formater, detail::Sink, detail::log_filter>;

TEST_CASE("Without tag works", "[log]")
{
    SECTION("As one statement")
    {
        logging::fatal() << "Test";
        logging::error() << "Test";
        logging::warn() << "Test";
        logging::info() << "Test";
        logging::debug() << "Test";
        logging::trace() << "Test";
    }

    SECTION("As multiple statements")
    {
        {
            auto log = logging::fatal();
            log << "test";
        }

        {
            auto log = logging::error();
            log << "test";
        }

        {
            auto log = logging::warn();
            log << "test";
        }

        {
            auto log = logging::info();
            log << "test";
        }

        {
            auto log = logging::debug();
            log << "test";
        }

        {
            auto log = logging::trace();
            log << "test";
        }
    }
}

TEST_CASE("With tag works", "[log]")
{
    SECTION("As one statement")
    {
        logging::fatal("test tag") << "Test";
        logging::error("test tag") << "Test";
        logging::warn("test tag") << "Test";
        logging::info("test tag") << "Test";
        logging::debug("test tag") << "Test";
        logging::trace("test tag") << "Test";
    }

    SECTION("As multiple statements")
    {
        {
            auto log = logging::fatal("test tag");
            log << "test";
        }

        {
            auto log = logging::error("test tag");
            log << "test";
        }

        {
            auto log = logging::warn("test tag");
            log << "test";
        }

        {
            auto log = logging::info("test tag");
            log << "test";
        }

        {
            auto log = logging::debug("test tag");
            log << "test";
        }

        {
            auto log = logging::trace("test tag");
            log << "test";
        }
    }
}

TEST_CASE("Logging lambdas works", "[log]")
{
    SECTION("As one statement")
    {
        logging::fatal() << []() { return "test"; };
        logging::error() << []() { return "test"; };
        logging::warn() << []() { return "test"; };
        logging::info() << []() { return "test"; };
        logging::debug() << []() { return "test"; };
        logging::trace() << []() { return "test"; };
    }

    SECTION("As multiple statements")
    {
        {
            auto log = logging::fatal();
            log << []() { return "test"; };
        }

        {
            auto log = logging::error();
            log << []() { return "test"; };
        }

        {
            auto log = logging::warn();
            log << []() { return "test"; };
        }

        {
            auto log = logging::info();
            log << []() { return "test"; };
        }

        {
            auto log = logging::debug();
            log << []() { return "test"; };
        }

        {
            auto log = logging::trace();
            log << []() { return "test"; };
        }
    }

    SECTION("Evaluation of the lambda is lazy")
    {
        int i = 0;

        logging::fatal() << [&i]() {
            ++i;
            return "test";
        };

        CHECK(i == 1);

        logging::error() << [&i]() {
            ++i;
            return "test";
        };

        CHECK(i == 2);

        logging::warn() << [&i]() {
            ++i;
            return "test";
        };

        CHECK(i == 3);

        logging::info() << [&i]() {
            ++i;
            return "test";
        };

        CHECK(i == 4);

        logging::debug() << [&i]() {
            ++i;
            return "test";
        };

        CHECK(i == 4);

        logging::trace() << [&i]() {
            ++i;
            return "test";
        };

        CHECK(i == 4);
    }
}
