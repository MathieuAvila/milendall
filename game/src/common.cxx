#include "common.hxx"

std::shared_ptr<spdlog::logger> getConsole(std::string name)
{
    auto result = spdlog::stdout_color_mt(name);
    spdlog::get(name)->set_level(
#ifdef DEBUG
    spdlog::level::debug
#else
    spdlog::level::info
#endif
    );
    return result;
};
