#include "../hpp/sequencer.hpp"

sequencer::sequencer(trail& t)
    : index(t, 0) {

}

size_t sequencer::operator()() {
    size_t result = index.get();
    index.mutate(
        [](size_t& v) { ++v; },
        [](size_t& v) { --v; }
    );
    return result;
}
