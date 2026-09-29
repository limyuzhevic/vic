#pragma once

#include "Brain.hpp"

namespace nlm {

class BrainUtilities {
public:
    // Logging and status
    static void logStatus(const Brain& brain);
    
    // Reset functionality
    static void reset(Brain& brain);
    
    // Helper functions for checking E/I ratio
    static float getExcitationInhibitionRatio(const Brain& brain);
};

} // namespace nlm