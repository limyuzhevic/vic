// This file is now implemented as Dopamine.cpp
// The Reward class has been replaced by the Dopamine class
// which implements real reward prediction error signaling

#include "Dopamine.hpp"

namespace nlm {
    // Dopamine implementation is in src/neuromodulation/Dopamine.cpp
    // This file is kept for backward compatibility
    // The Dopamine class provides reward prediction error and reinforcement learning
    
    // Alias for backward compatibility
    using Reward = Dopamine;
    
} // namespace nlm