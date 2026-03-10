// #ifndef A01_HPP
// #define A01_HPP

// #include <optional>
// #include "a01_defs.hpp"
// #include "a01_goal_adder.hpp"
// #include "a01_goal_resolver.hpp"
// #include "a01_head_elimination_detector.hpp"
// #include "a01_cdcl_elimination_detector.hpp"
// #include "normalizer.hpp"
// #include "solution_detector.hpp"
// #include "conflict_detector.hpp"
// #include "unit_propagation_detector.hpp"

// struct a01 {
//     a01(
//         const a01_database&,
//         const std::set<const expr*>&,
//         trail&,
//         sequencer&,
//         expr_pool&,
//         bind_map&
//     );
//     std::optional<a01_resolution_store> operator()();
// #ifndef DEBUG
// private:
// #endif
//     const a01_database& db;
//     trail& t;
//     sequencer& vars;
//     expr_pool& ep;
//     bind_map& bm;

//     lineage_pool lp;

//     a01_avoidance_store as;
// };

// #endif
