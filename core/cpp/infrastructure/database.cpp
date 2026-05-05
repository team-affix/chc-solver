#include "../../hpp/infrastructure/database.hpp"

database::database(const std::vector<rule>& db) :
    db(db) {
}

const rule& database::at(size_t index) const {
    return db.at(index);
}

size_t database::size() const {
    return db.size();
}
