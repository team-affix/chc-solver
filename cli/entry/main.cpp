#include <CLI/CLI.hpp>
#include "../hpp/ridge_command_handler.hpp"
#include "../hpp/horizon_command_handler.hpp"

int main(int argc, char** argv) {
    CLI::App app{"Atlas CHC Solver"};
    app.require_subcommand(1);

    // --- ridge subcommand ---
    struct {
        std::string file;
        std::vector<std::string> goals;
        size_t max_resolutions          = 1000;
        size_t iterations_per_avoidance = 10;
        double exploration_constant     = 1.41;
        uint64_t seed                   = 0;
        size_t steps                    = 100;
    } ridge_opts;

    auto* ridge_sub = app.add_subcommand("ridge", "Run the Ridge solver");
    ridge_sub->add_option("file", ridge_opts.file, "CHC input file")->required();
    ridge_sub->add_option("-g,--goal", ridge_opts.goals, "Goal expression(s) to prove")->required();
    ridge_sub->add_option("--max-resolutions", ridge_opts.max_resolutions, "Max resolutions");
    ridge_sub->add_option("--iterations-per-avoidance", ridge_opts.iterations_per_avoidance, "Iterations per avoidance");
    ridge_sub->add_option("--exploration-constant", ridge_opts.exploration_constant, "MCTS exploration constant");
    ridge_sub->add_option("--seed", ridge_opts.seed, "RNG seed");
    ridge_sub->add_option("--steps", ridge_opts.steps, "Solver step limit");
    ridge_sub->callback([&]() {
        ridge_command_handler h(ridge_opts.file, ridge_opts.goals,
                                ridge_opts.max_resolutions,
                                ridge_opts.iterations_per_avoidance,
                                ridge_opts.exploration_constant,
                                ridge_opts.seed, ridge_opts.steps);
        h();
    });

    // --- horizon subcommand ---
    struct {
        std::string file;
        std::vector<std::string> goals;
        size_t max_resolutions      = 1000;
        double exploration_constant = 1.41;
        uint64_t seed               = 0;
        size_t steps                = 100;
    } horizon_opts;

    auto* horizon_sub = app.add_subcommand("horizon", "Run the Horizon solver");
    horizon_sub->add_option("file", horizon_opts.file, "CHC input file")->required();
    horizon_sub->add_option("-g,--goal", horizon_opts.goals, "Goal expression(s) to prove")->required();
    horizon_sub->add_option("--max-resolutions", horizon_opts.max_resolutions, "Max resolutions");
    horizon_sub->add_option("--exploration-constant", horizon_opts.exploration_constant, "MCTS exploration constant");
    horizon_sub->add_option("--seed", horizon_opts.seed, "RNG seed");
    horizon_sub->add_option("--steps", horizon_opts.steps, "Solver step limit");
    horizon_sub->callback([&]() {
        horizon_command_handler h(horizon_opts.file, horizon_opts.goals,
                                  horizon_opts.max_resolutions,
                                  horizon_opts.exploration_constant,
                                  horizon_opts.seed, horizon_opts.steps);
        h();
    });

    CLI11_PARSE(app, argc, argv);
}
