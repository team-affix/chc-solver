#include <gtest/gtest.h>
#include "../../../core/hpp/infrastructure/goal_candidate_activator_visitor.hpp"
#include "../../../core/hpp/infrastructure/lineage_pool.hpp"
#include "../../../core/hpp/infrastructure/copier.hpp"
#include "../../../core/hpp/infrastructure/var_sequencer.hpp"
#include "../../../core/hpp/infrastructure/expr_pool.hpp"
#include "../../../core/hpp/infrastructure/bind_map.hpp"
#include "../../../core/hpp/infrastructure/bind_map_factory.hpp"
#include "../../../core/hpp/infrastructure/overlay_bind_map_factory.hpp"
#include "../../../core/hpp/infrastructure/unifier_factory.hpp"
#include "../../../core/hpp/infrastructure/mhu_elimination_generator.hpp"
#include "../../../core/hpp/utility/trail.hpp"
#include "../../../core/hpp/interfaces/i_get_goal_expr.hpp"
#include "../../../core/hpp/interfaces/i_activate_candidate_translation_map.hpp"
#include "../../../core/hpp/interfaces/i_candidate_activator.hpp"
#include "../../../core/hpp/interfaces/i_elimination_backlog.hpp"

namespace {

class fixed_goal_expr : public i_get_goal_expr {
public:
    explicit fixed_goal_expr(const expr* e) : e(e) {}
    const expr* get(const goal_lineage*) const override { return e; }

    const expr* e;
};

class recording_translation_activator : public i_activate_candidate_translation_map {
public:
    std::unordered_map<const resolution_lineage*, translation_map> active;

    void activate(const resolution_lineage* rl, translation_map tm) override {
        active[rl] = std::move(tm);
    }
};

class recording_candidate_activator : public i_candidate_activator {
public:
    std::unordered_set<const resolution_lineage*> active;

    void activate(const resolution_lineage* rl) override { active.insert(rl); }
};

class noop_backlog : public i_elimination_backlog {
public:
    void insert(const resolution_lineage*) override {}
    bool contains(const resolution_lineage*) override { return false; }
    void constrain(const resolution_lineage*) override {}
};

} // namespace

class GoalCandidateActivatorVisitorIntegrationTest : public ::testing::Test {
protected:
    trail t;
    expr_pool pool{t};
    var_sequencer vs{t};
    copier cp{vs, pool};
    bind_map common;
    bind_map_factory bmf;
    overlay_bind_map_factory obmf;
    unifier_factory uf;
    lineage_pool lp;
    mhu_elimination_generator mhu{common, pool};

    expr goal_var{expr::var{0}};
    expr head_var{expr::var{0}};
    rule matching{&head_var, {}};

    fixed_goal_expr gge{&goal_var};
    recording_translation_activator actm;
    recording_candidate_activator ca;
    noop_backlog eb;

    goal_lineage* gl = nullptr;

    void SetUp() override {
        gl = const_cast<goal_lineage*>(lp.goal(nullptr, &goal_var));
    }
};

TEST_F(GoalCandidateActivatorVisitorIntegrationTest,
    SuccessfulUnifyActivatesCandidateAndTranslationMap) {
    goal_candidate_activator_visitor visitor{
        gl, cp, common, bmf, obmf, uf, actm, mhu, ca, eb, lp, gge};

    visitor.visit(&matching);

    const resolution_lineage* rl = lp.resolution(gl, &matching);
    EXPECT_TRUE(ca.active.contains(rl));
    EXPECT_TRUE(actm.active.contains(rl));
}

TEST_F(GoalCandidateActivatorVisitorIntegrationTest,
    FailedUnifyDoesNotActivateCandidate) {
    expr goal_f{expr::functor{"f", {}}};
    expr head_g{expr::functor{"g", {}}};
    rule non_matching{&head_g, {}};

    auto* gl_f = const_cast<goal_lineage*>(lp.goal(nullptr, &goal_f));
    fixed_goal_expr gge_f{&goal_f};

    goal_candidate_activator_visitor visitor{
        gl_f, cp, common, bmf, obmf, uf, actm, mhu, ca, eb, lp, gge_f};

    visitor.visit(&non_matching);

    const resolution_lineage* rl = lp.resolution(gl_f, &non_matching);
    EXPECT_FALSE(ca.active.contains(rl));
    EXPECT_FALSE(actm.active.contains(rl));
}
