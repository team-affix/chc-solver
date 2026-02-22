#include "../hpp/var_context.hpp"

var_context::var_context(trail& trail_ref)
    : trail_ref(trail_ref), variable_count(0) {

}

uint32_t var_context::next() {
    trail_ref.log([this]{--variable_count;});
    return variable_count++;
}
