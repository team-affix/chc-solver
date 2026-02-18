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
    edges.at(lhs).insert(edge{rhs, cause});
    edges.at(rhs).insert(edge{lhs, cause});

    // 4. If either CIN is not found, unification succeeds
    if (!lCinFound || !rCinFound)
        return {};

    // 5. Construct the full causal set for why the CINs were unified
    causal_set cinUnifyCause = cause;
    cinUnifyCause.insert(lPath.begin(), lPath.end());
    cinUnifyCause.insert(rPath.begin(), rPath.end());

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
        std::pair<size_t, const expr*>,
        std::vector<std::pair<size_t, const expr*>>,
        std::greater<std::pair<size_t, const expr*>> // min-heap
    > pq;

    // 2. Create the visited set to track nodes we have already visited
    std::set<const expr*> visited;

    // 3. Create the memory-efficient backedge type (src, cause)
    using backedge = std::pair<const expr*, const causal_set*>;

    // 3. Create the trace map to track current path
    std::map<const expr*, backedge> backTrace;

    // 4. Initialize the priority queue with the source
    pq.push({0, src});

    // 5. Create cin node
    const expr* cinNode = nullptr;

    // 5. Run Dijkstra's algorithm
    while (!pq.empty()) {
        // Get the node with the smallest distance from the source
        auto [dist, node] = pq.top();
        pq.pop();

        // If we have already visited this node, skip it
        if (visited.count(node))
            continue;

        // Mark the node as visited
        visited.insert(node);
        
        // If the node is a nonvar, we are done.
        if (!std::holds_alternative<expr::var>(node->content)) {
            cinNode = node;
            break;
        }

        // Add the children of the node to the priority queue
        for (const auto& e : edges.at(node)) {
            pq.push({dist + e.cause.size(), e.dst});
            backTrace[e.dst] = {node, &e.cause};
        }
    }

    // 6. If we did not find a CIN, return false
    if (cinNode == nullptr)
        return {false, {}};

    // 7. Construct the causal set for the path to the CIN
    causal_set cinCause;

    const expr* currentNode = cinNode;
    
    while (currentNode != src) {
        const backedge& e = backTrace[currentNode];
        cinCause.insert(e.second->begin(), e.second->end());
        currentNode = e.first;
    }
    
    return {true, cinCause};

}
