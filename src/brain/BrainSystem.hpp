#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class RandomGenerator;
class Config;

// Abstract base class for brain systems
class BrainSystem {
public:
    virtual ~BrainSystem() = default;
    
    // Initialize the system with a reference to the brain
    virtual bool initialize(Brain* brain) = 0;
    
    // Update the system with current timestep
    virtual void update(double timestep) = 0;
    
    // Reset system state
    virtual void reset() = 0;
    
    // Return system name for logging
    virtual std::string getName() const = 0;
    
    // Check if system is enabled
    virtual bool isEnabled() const = 0;
protected:
    Brain* brainRef = nullptr;
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
};

// Template Method pattern for step processing
class StepProcessor {
public:
    virtual ~StepProcessor() = default;
    
    // Execute the step with Template Method pattern
    void executeStep(SimulationStep step, Timestamp time) {
        preStep(step, time);
        doStep(step, time);
        postStep(step, time);
    }
    
protected:
    virtual void preStep(SimulationStep step, Timestamp time) = 0;
    virtual void doStep(SimulationStep step, Timestamp time) = 0;
    virtual void postStep(SimulationStep step, Timestamp time) = 0;
};

// Chain of Responsibility handler base class
class SystemHandler {
public:
    virtual ~SystemHandler() = default;
    virtual bool handle(Brain* brain, SimulationStep step, Timestamp time) = 0;
    virtual SystemHandler* setNext(SystemHandler* handler) = 0;
};

// Facade for simplified brain interface
class BrainFacade {
public:
    static BrainFacade& getInstance() {
        static BrainFacade instance;
        return instance;
    }
    
    // Simplified operations that aggregate multiple systems
    void processInput(const class SensoryInput& input);
    void generateOutput();
    
private:
    BrainFacade() = default;
    BrainFacade(const BrainFacade&) = delete;
    BrainFacade& operator=(const BrainFacade&) = delete;
};

// Enum for system priorities and execution order
enum class SystemPriority {
    Low = 0,
    Medium = 1,
    High = 2,
    Critical = 3
};

// Base class for specialized systems
class SpecializedBrainSystem : public BrainSystem {
public:
    SpecializedBrainSystem(SystemPriority priority) : priority(priority) {}
    SystemPriority getPriority() const { return priority; }
    
    // Override priority for specialized systems
    virtual SystemPriority getSystemPriority() const override { return priority; }
    
protected:
    SystemPriority priority;
};

} // namespace nlm
