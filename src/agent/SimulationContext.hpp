#pragma once

#include "SimpleWorld.hpp"
#include "AgentBrain.hpp"
#include "Brain.hpp"

namespace nlm {

/**
 * SimulationContext - Context manager for simulation resources
 * 
 * Provides Pythonic context manager interface for simulations.
 * Ensures proper cleanup of resources and consistent state management.
 */
class SimulationContext {
public:
    SimulationContext(std::shared_ptr<SimpleWorld> world, 
                     std::shared_ptr<AgentBrain> agent,
                     std::shared_ptr<Brain> brain);
    
    ~SimulationContext();
    
    // Context manager interface
    void enter();
    void exit();
    
    // Run simulation for specified steps
    void run(size_t steps);
    
    // Get current simulation state
    std::string getStatus() const;
    
    // Step execution
    void step(size_t stepNumber);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
