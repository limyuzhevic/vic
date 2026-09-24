// Brain Integration Implementation - Contains integration between different systems
#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Integration system management
namespace SystemIntegration {

    // Initialize all integrated systems
    void initializeAllSystems(Brain::Impl* impl);
    
    // Register event handlers and callbacks
    void registerEventHandlers(Brain::Impl* impl);
    
    // Clean up integrated systems
    void cleanupIntegratedSystems(Brain::Impl* impl);
    
    // System health check
    bool checkSystemHealth(const Brain::Impl* impl);
    std::string getSystemHealthReport(const Brain::Impl* impl);
    
    // Integration validation
    bool validateIntegration(const Brain::Impl* impl);
    
    // Cross-system communication
    void processCrossSystemCommunication(Brain::Impl* impl, SimulationStep currentStep, const TimestepDuration& timestep);
    
    // Resource management
    void optimizeResourceUsage(Brain::Impl* impl);
    void manageMemoryPools(Brain::Impl* impl);
}

} // namespace nlm