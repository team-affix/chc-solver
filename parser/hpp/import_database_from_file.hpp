#ifndef IMPORT_DATABASE_FROM_FILE_HPP
#define IMPORT_DATABASE_FROM_FILE_HPP

#include <string>
#include "../../core/hpp/defs.hpp"
#include "../../core/hpp/expr.hpp"
#include "../../core/hpp/sequencer.hpp"

database import_database_from_file(const std::string& path, expr_pool&, sequencer&);

#endif
