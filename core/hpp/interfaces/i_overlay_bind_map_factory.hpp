#ifndef I_OVERLAY_BIND_MAP_FACTORY_HPP
#define I_OVERLAY_BIND_MAP_FACTORY_HPP

#include "i_factory.hpp"
#include "i_bind_map.hpp"

struct i_overlay_bind_map_factory : i_factory<i_bind_map, i_bind_map&, i_bind_map&> {
    virtual ~i_overlay_bind_map_factory() = default;
};

#endif
