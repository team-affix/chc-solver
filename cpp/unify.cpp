#include <queue>
#include "../hpp/unify.hpp"

unification_graph::unification_graph(trail& trail_ref) : trail_ref(trail_ref) {

}

causal_set unification_graph::unify(const expr* lhs, const expr* rhs, const causal_set& cause) {
    // 1. Find the CIN of the lhs, and path to it (lPath)
    auto [lCinFound, lPath] = cin_dijkstra(lhs);

    // 2. Find the CIN of the rhs, and path to it (rPath)
    auto [rCinFound, rPath] = cin_dijkstra(rhs);

    // 3. Merge the lhs and rhs with a new edge
    edges[lhs].insert(edge{rhs, cause});
    edges[rhs].insert(edge{lhs, cause});

    // 4. If either CIN is not found, unification succeeds
    if (!lCinFound || !rCinFound)
        return {};

    // 5. Construct the full causal set for why the CINs were unified
    causal_set cinUnifyCause = cause + lPath + rPath;

    // 6. If the CINs heads differ, unification fails.
    
    // 6.1 Check if the CINs have different types
    if (lhs->content.index() != rhs->content.index())
        return cinUnifyCause;

    // 6.2 Both are atoms
    if (std::holds_alternative<expr::atom>(lhs->content) && std::holds_alternative<expr::atom>(rhs->content)) {
        const expr::atom& lAtom = std::get<expr::atom>(lhs->content);
        const expr::atom& rAtom = std::get<expr::atom>(rhs->content);
        if (lAtom.value != rAtom.value)
            return cinUnifyCause;
        return {};
    }

    // 6.3 Both are cons cells
    const expr::cons& lList = std::get<expr::cons>(lhs->content);
    const expr::cons& rList = std::get<expr::cons>(rhs->content);
    
    // 7. Decompose based on the CINs, unifying children
    causal_set result;

    // 7.1 Unify the car of the CINs
    result = unify(lList.lhs, rList.lhs, cinUnifyCause);

    if (!result.empty())
        return result;

    result = unify(lList.rhs, rList.rhs, cinUnifyCause);

    return result;
    
}

std::pair<bool, causal_set> unification_graph::cin_dijkstra(const expr* src) const {
    // 1. Create the priority queue for Dijkstra's algorithm
    std::priority_queue<
        std::pair<causal_set, const expr*>,
        std::vector<std::pair<causal_set, const expr*>>,
        std::greater<std::pair<causal_set, const expr*>> // min-heap
    > pq;

    // 2. Create the visited set to track nodes we have already visited
    std::set<const expr*> visited;

    // 3. Initialize the priority queue with the source
    pq.push({{}, src});

    // 4. Create cin node
    const expr* cinNode = nullptr;
    causal_set  cinCause;

    // 5. Run Dijkstra's algorithm
    while (!pq.empty()) {
        // Get the node with the smallest distance from the source
        auto [causalSet, node] = pq.top();
        pq.pop();

        // If we have already visited this node, skip it
        if (visited.count(node))
            continue;

        // Mark the node as visited
        visited.insert(node);
        
        // If the node is a nonvar, we are done.
        if (!std::holds_alternative<expr::var>(node->content)) {
            cinNode = node;
            cinCause = causalSet;
            break;
        }

        // Add the children of the node to the priority queue
        for (const auto& e : edges.at(node))
            pq.push({causalSet + e.cause, e.dst});
    }

    // 6. If we did not find a CIN, return false
    if (cinNode == nullptr)
        return {false, {}};
    
    return {true, cinCause};

}
