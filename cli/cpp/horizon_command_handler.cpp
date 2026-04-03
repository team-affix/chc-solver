#include "../hpp/horizon_command_handler.hpp"
#include "../../core/hpp/trail.hpp"
#include "../../core/hpp/expr.hpp"
#include "../../core/hpp/sequencer.hpp"
#include "../../core/hpp/bind_map.hpp"
#include "../../core/hpp/defs.hpp"
#include "../../core/hpp/horizon.hpp"
#include "../../parser/hpp/import_database_from_file.hpp"
#include "../../parser/hpp/expr_visitor.hpp"
#include "../../parser/generated/CHCLexer.h"
#include "../../parser/generated/CHCParser.h"
#include <iostream>
#include <map>
#include <random>

horizon_command_handler::horizon_command_handler(
    const std::string& file,
    const std::vector<std::string>& goals,
    size_t max_resolutions,
    double exploration_constant,
    uint64_t seed,
    size_t steps
) :
    file(file),
    goals(goals),
    max_resolutions(max_resolutions),
    exploration_constant(exploration_constant),
    seed(seed),
    steps(steps)
{}

void horizon_command_handler::operator()() {
    trail t;
    expr_pool pool(t);
    sequencer seq(t);
    bind_map bm(t);
    std::mt19937 rng(seed);

    database db = import_database_from_file(file, pool, seq);

    ::goals gl;
    for (const auto& goal_str : goals) {
        std::map<std::string, uint32_t> var_map;
        expr_visitor ev(pool, seq, var_map);
        antlr4::ANTLRInputStream stream(goal_str);
        CHCLexer lexer(&stream);
        antlr4::CommonTokenStream tokens(&lexer);
        CHCParser parser(&tokens);
        gl.push_back(std::any_cast<const expr*>(ev.visitExpr(parser.expr())));
    }

    horizon solver(db, gl, t, seq, bm,
                   max_resolutions, exploration_constant, rng);
    std::optional<resolutions> res;
    bool sat = solver(steps, res);
    std::cout << (sat ? "SAT" : "UNSAT") << "\n";
}
