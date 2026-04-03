#include "../hpp/database_visitor.hpp"
#include "../../core/hpp/defs.hpp"
#include "../hpp/clause_visitor.hpp"

database_visitor::database_visitor(expr_pool& pool, sequencer& seq)
    : pool(pool), seq(seq) {}

antlrcpp::Any database_visitor::visitDatabase(CHCParser::DatabaseContext* ctx) {
    clause_visitor cv(pool, seq);
    database db;
    for (auto* c : ctx->clause())
        db.push_back(std::any_cast<rule>(cv.visitClause(c)));
    return db;
}
