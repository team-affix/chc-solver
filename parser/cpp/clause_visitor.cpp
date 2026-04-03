#include "../hpp/clause_visitor.hpp"

clause_visitor::clause_visitor(expr_pool& pool, sequencer& seq)
    : pool(pool), seq(seq) {}

antlrcpp::Any clause_visitor::visitClause(CHCParser::ClauseContext* ctx) {
    std::map<std::string, uint32_t> var_map;
    expr_visitor ev(pool, seq, var_map);

    const expr* head = std::any_cast<const expr*>(ev.visitExpr(ctx->expr()));

    // In case of facts
    auto* b = ctx->body();
    if (!b) return rule{head, {}};

    // In case of non-fact rules
    std::vector<const expr*> body;
    for (auto* e : b->expr())
        body.push_back(std::any_cast<const expr*>(ev.visitExpr(e)));

    return rule{head, body};
}
