#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Forward declarations
class Brain;
class CheckpointManager;

class BrainPersistence {
public:
    // ===========================================
    // CHECKPOINT PERSISTENCE SYSTEM
    // ===========================================
    
    static bool saveBrainState(Brain* brain, const std::string& filepath);
    static bool loadBrainState(Brain* brain, const std::string& filepath);
    static void resetBrainState(Brain* brain);
    
    // ===========================================
    // CHECKPOINT MANAGEMENT
    // ===========================================
    
    static void configureCheckpointManager(Brain* brain,
                                           const std::string& checkpointDir,
                                           size_t maxCheckpoints,
                                           size_t maxCheckpointAge,
                                           bool compressionEnabled);
    static void updateCheckpointManager(Brain* brain, SimulationStep step, Timestamp time);
    
    // ===========================================
    // PERSISTENCE STATISTICS AND ACCESSORS
    // ===========================================
    
    static void logPersistenceStatus(Brain* brain);
};

} // namespace nlm