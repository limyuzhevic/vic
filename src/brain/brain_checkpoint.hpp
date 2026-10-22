#pragma once

#include "Brain.hpp"

namespace nlm {

// Save/load checkpoint methods
bool Brain::save(const std::string& filepath) const;
bool Brain::load(const std::string& filepath);

} // namespace nlm
