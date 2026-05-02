#include "../../hpp/infrastructure/var_names.hpp"

var_names::var_names() {
}

const std::string& var_names::name(uint32_t index) const {
    return names.at(index);
}

void var_names::set_name(uint32_t index, const std::string& name) {
    names.insert({index, name});
}
