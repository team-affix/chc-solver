#include "../../hpp/infrastructure/sequencer.hpp"

sequencer::sequencer(trail& t) :
    trail_ref(t), index(0) {
}

uint32_t sequencer::operator()() {
    trail_ref.log([this]{--index;});
    return index++;
}
