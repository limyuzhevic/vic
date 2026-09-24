// Brain Checkpoint System Implementation - Contains checkpoint system management
#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"

namespace nlm {

// Checkpoint system management
namespace CheckpointManagement {

    // Checkpoint system initialization
    void initializeCheckpointSystem(Brain::Impl* impl);
    void configureCheckpointSystem(Brain::Impl* impl);
    
    // Checkpoint operations
    void saveBrainState(Brain::Impl* impl, const std::string& filepath);
    void loadBrainState(Brain::Impl* impl, const std::string& filepath);
    void manageCheckpointBackups(Brain::Impl* impl);
    
    // Checkpoint metadata
    void updateCheckpointMetadata(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    void manageCheckpointCompression(Brain::Impl* impl);
    
    // Checkpoint statistics
    void updateCheckpointStatistics(Brain::Impl* impl);
    std::string getCheckpointStatisticsReport(const Brain::Impl* impl);
    
    // Checkpoint recovery
    bool recoverFromCheckpoint(Brain::Impl* impl, const std::string& filepath);
    void validateCheckpointIntegrity(Brain::Impl* impl, const std::string& filepath);
    
    // Cross-system checkpoint integration
    void integrateCheckpointsWithMemory(Brain::Impl* impl);
    void integrateCheckpointsWithDevelopment(Brain::Impl* impl);
}

} // namespace nlm