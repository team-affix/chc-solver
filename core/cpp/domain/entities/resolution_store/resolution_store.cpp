#include "../../../../hpp/domain/entities/resolution_store/resolution_store.hpp"
#include "../../../../hpp/infrastructure/locator.hpp"

resolution_store::resolution_store() :
    resolutions(locator::locate<trail>()) {
}

void resolution_store::insert(const resolution_lineage* rl) {
    resolutions.insert(rl);
}

const std::unordered_set<const resolution_lineage*>& resolution_store::get() const {
    return resolutions.get();
}
