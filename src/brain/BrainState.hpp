#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ========== BRAIN STATE DECLARATIONS ==========

namespace BrainState {
    
    bool saveBrainState(Brain::Impl* impl, const std::string& filepath);
    
    bool loadBrainState(Brain::Impl* impl, const std::string& filepath);
    
    void resetBrain(Brain::Impl* impl);
    
    void collectStatistics(Brain::Impl* impl);
}

} // namespace nlm