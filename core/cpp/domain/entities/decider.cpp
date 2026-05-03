#include "../../../hpp/domain/entities/decider.hpp"
#include "../../../hpp/bootstrap/resolver.hpp"

decider::decider() :
    decision_generator(resolver::resolve<i_decision_generator>()),
    decided_producer(resolver::resolve<i_event_producer<decided_event>>()) {
}

void decider::decide() const {
    auto rl = decision_generator.generate();
    decided_producer.produce(decided_event{rl});
}
