#ifndef HORIZON_COMMAND_HANDLER_HPP
#define HORIZON_COMMAND_HANDLER_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

struct horizon_command_handler {
    horizon_command_handler(
        const std::string& file,
        const std::vector<std::string>& goals,
        size_t max_resolutions,
        double exploration_constant,
        uint64_t seed,
        size_t steps
    );
    void operator()();
private:
    std::string file;
    std::vector<std::string> goals;
    size_t max_resolutions;
    double exploration_constant;
    uint64_t seed;
    size_t steps;
};

#endif
