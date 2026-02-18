# CDCL in CHC

## Overview of the problem

In our CHC solver, we use CDCL to quickly shave off large chunks of the state space when searching for solutions.
The use of an automatic propagation scheme like that of CDCL, DPLL, or others is required to achieve refutational completeness for finite domains (one requirement of this solver).

We start our journey into implementing CDCL by asking: How would we implement the implication graph used in CDCL? What are the nodes/edges? There are two potential answers available.

1. Nodes are metavariable assignments at decision levels and edges are the clauses which force unit propagation of other vars
2. Nodes are clause fulfillments (assigning a clause a rule by which to be fulfilled) and edges have no labels, as they are not actually required by the CDCL algorithm.

In this system, we have chosen option 2 as it is the most straightforward to implement in my opinion, and would be the occam's razor choice for somebody wanting to base this off of a prolog style system. This is because, each clause must be fulfilled. We simply find all candidates which could fulfill a given clause, and choose one of them. We do not need to do analysis of what variables are present in the clause and what values each variable could assume between all clauses in the database simultaneously which might be required for option (1).

## Breaking the problem down

In order to be able to build the implication graph, we must be able to accurately track which clause fulfillments caused other clause fulfillments to be made unit. This means that we must find the medium through which these things are forced to happen.

**To assist in this derivation, we recognize the following tautology:**

> Every clause C in the constraint store has some number of candidate rules whose heads unify with C. A clause fulfillment is made unit when all but one candidate are eliminated.

Using the above definition, we can see that a unit propagation is forced by prior fulfillments _iff_ said prior fulfillments resulted in the elimination of all other candidates.

How to track why a candidate is eliminated?

> A candidate G for a clause C is eliminated when C is made so that it can no longer unify with the head of G.

And, the clause can only be changed by unifications of variables involved in the clause. NOTE: (very useful observation): in our system, we can say that the only things which ever cause unifications are clause fulfillments. This closes the causality to be describable as a graph where the nodes are clause fulfillments.

In order to implement the reasoning system for "why a candidate's head no longer unifies with the clause", we must implement a unification graph system.

## Unification Graph

We implement a unification graph where the edges are causes, nodes are expressions, and the edges' presence between nodes signifies that those two expressions are made equal (unified). In order to realize why any two nodes in the graph were made equal (even indirectly), it just matters that we walk a path between the nodes and collect reasons from the edges that we traverse. The set of reasons why two quantities are made equal (from a single path) is called a _causal set_.

There could be several independently sufficient reasons why any two nodes are made equal, as there could be several paths between the nodes.

### Priors

I recognize that this problem is very similar to the problem of congruence closure, however, we will potentially end up with massive causal sets that are not very optimal if we naively use the union-find approach to explaining why two things are made equal.

### Algorithm for Unification

We maintain a unification graph, which is really constituted by many disconnected subgraphs. Any two subgraphs can be unified, which may result in a _collision_. A collision occurs when two subgraphs cannot unify as a result of a fundamental difference between the heads of the expressions in the two graphs being joined, or if any _decomposition_ produces a collision.

Generally, this the algorithm for detecting if/why a collision occurs as a result of unification:

Given nodes A and B,

1. Search for the closest instantiated node **CIN** to A inside of A's graph. (NOTE instantiated means NON-var)
2. Search for the closest instantiated node **CIN** to B inside of B's graph.
3. Construct the edge merging the two graphs into one.
4. Determine if the _cin(A)_ and _cin(B)_ have heads which collide. If so, return the complete causal set from the path between _cin(A)_ and _cin(B)_.
5. If the two do not collide and there are children (in the case of a composite structure like cons cells) make a recursive call to unify() algorithm on the children.
6. If there are no children, then no collision was produced. In the case that no collision is produced, an empty causal set is returned from unification.
