#ifndef LEMMA_HPP
#define LEMMA_HPP

#include <set>
#include "defs.hpp"

struct lemma {
    lemma(const resolutions&);
#ifndef DEBUG
private:
#endif
    void remove_ancestors(const resolution_lineage*, std::set<const resolution_lineage*>&);
    resolutions rs;
};

#endif
