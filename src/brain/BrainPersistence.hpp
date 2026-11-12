// BrainPersistence.hpp - Save/load/checkpoint logic
#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>

namespace nlm {

class BrainPersistence {
public:
    BrainPersistence(Brain* brain);
    ~BrainPersistence();
    
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
private:
    Brain* brain_;
};

} // namespace nlm
