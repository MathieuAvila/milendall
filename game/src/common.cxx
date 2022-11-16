#include "common.hxx"

std::shared_ptr<spdlog::logger> getConsole(std::string name)
{
    auto result = spdlog::stdout_color_mt(name);
    /*spdlog::get(name)->set_level(
#ifdef DEBUG
    spdlog::level::debug
#else
    spdlog::level::info
#endif
    );*/
    return result;
};

void setDebugConsole()
{
    spdlog::apply_all(
        [&](std::shared_ptr<spdlog::logger> l)
        {
            l->info("Apply debug");
            l->set_level(spdlog::level::level_enum::debug);
        });
}