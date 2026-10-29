#pragma once

#include "Brain.hpp"

namespace nlm {

class BrainStepManager {
public:
    // Update all neurons with LIF dynamics
    static void updateAllNeurons(Brain* brain, Timestamp currentTime);
    
    // Detect spikes and schedule spike events
    static void detectAndScheduleSpikes(Brain* brain, Timestamp currentTime);
};

} // namespace nlm
