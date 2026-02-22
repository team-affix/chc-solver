#ifndef VAR_CONTEXT_HPP
#define VAR_CONTEXT_HPP

#include <cstdint>
#include "trail.hpp"

struct var_context {
    var_context(trail&);
    uint32_t next();
#ifndef DEBUG
private:
#endif
    trail& trail_ref;
    uint32_t variable_count;
};

#endif
