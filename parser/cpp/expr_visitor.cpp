#include "../hpp/expr_visitor.hpp"

expr_visitor::expr_visitor(expr_pool& pool, sequencer& seq, std::map<std::string, uint32_t>& var_map)
    : pool(pool), seq(seq), var_map(var_map) {}

antlrcpp::Any expr_visitor::visitExpr(CHCParser::ExprContext* ctx) {
    if (auto* atom = ctx->ATOM())
        return visitAtom(atom);

    if (auto* var = ctx->VARIABLE())
        return visitVar(var);

    // Parenthesized form: children are ( expr* ) or ( expr . expr ).
    auto exprs = ctx->expr();
    if (exprs.size() == 2 && ctx->children[2]->getText() == ".")
        return visitCons(exprs);

    return visitList(exprs);
}

const expr* expr_visitor::visitAtom(antlr4::tree::TerminalNode* node) {
    return pool.atom(node->getText());
}

const expr* expr_visitor::visitVar(antlr4::tree::TerminalNode* node) {
    const std::string& name = node->getText();
    if (name == "_")
        return pool.var(seq());
    auto [it, inserted] = var_map.emplace(name, 0u);
    if (inserted) it->second = seq();
    return pool.var(it->second);
}

const expr* expr_visitor::visitCons(std::vector<CHCParser::ExprContext*>& exprs) {
    return pool.cons(std::any_cast<const expr*>(visit(exprs[0])),
                     std::any_cast<const expr*>(visit(exprs[1])));
}

const expr* expr_visitor::visitList(std::vector<CHCParser::ExprContext*>& exprs) {
    const expr* result = pool.atom("nil");
    for (int i = exprs.size() - 1; i >= 0; i--)
        result = pool.cons(std::any_cast<const expr*>(visit(exprs[i])), result);
    return result;
}
