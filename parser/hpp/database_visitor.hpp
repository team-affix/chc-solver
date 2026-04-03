#ifndef DATABASE_VISITOR_HPP
#define DATABASE_VISITOR_HPP

#include "../generated/CHCBaseVisitor.h"
#include "../../core/hpp/expr.hpp"
#include "../../core/hpp/sequencer.hpp"

struct database_visitor : public CHCBaseVisitor {
    database_visitor(expr_pool&, sequencer&);
    antlrcpp::Any visitDatabase(CHCParser::DatabaseContext*) override;
#ifndef DEBUG
private:
#endif
    expr_pool& pool;
    sequencer& seq;
};

#endif
