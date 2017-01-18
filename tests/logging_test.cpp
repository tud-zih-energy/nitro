//#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#ifdef NITRO_LOG_MIN_SEVERITY
#undef NITRO_LOG_MIN_SEVERITY
#endif
#define NITRO_LOG_MIN_SEVERITY info

#include <nitro/log/attribute/hostname.hpp>
#include <nitro/log/attribute/severity.hpp>
#include <nitro/log/filter/severity_filter.hpp>
#include <nitro/log/log.hpp>
#include <nitro/log/sink/stdout.hpp>

namespace detail
{

typedef nitro::log::record<nitro::log::tag_attribute, nitro::log::message_attribute,
                           nitro::log::hostname_attribute, nitro::log::severity_attribute>
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

        s << r.hostname() << "][" << r.severity() << "]: " << r.message();

        return s.str();
    }
};

template <typename Record>
using log_filter = nitro::log::filter::severity_filter<Record>;
}

typedef nitro::log::logger<detail::record, detail::log_formater, nitro::log::sink::stdout,
                           detail::log_filter>
    logging;

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
