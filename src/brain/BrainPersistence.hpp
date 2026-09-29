#pragma once

#include "Brain.hpp"

namespace nlm {

class BrainPersistence {
public:
    // Save and load methods
    static bool save(const Brain& brain, const std::string& filepath);
    static bool load(Brain& brain, const std::string& filepath);
};

} // namespace nlm