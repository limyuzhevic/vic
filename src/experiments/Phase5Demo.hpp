#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../memory/WorkingMemory.hpp"
#include "../memory/EpisodicMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"

namespace nlm {

// Forward declarations for types used in the demo
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class Brain;
class AgentBrain;
class SimpleWorld;
class NeuralPrediction;
class WorkingMemory;
class EpisodicMemory;
class ConceptFormation;
class NeuralPlanner;

// Forward declarations for Action types (defined elsewhere)
enum class ActionType;
struct Goal;
struct Action;
struct MotorCommand;
struct ActionResult;
struct SensoryPercept;
struct Concept;

class Phase5Demo {
public:
    Phase5Demo();
    ~Phase5Demo();
    
    // Disable copying, enable moving
    Phase5Demo(const Phase5Demo&) = delete;
    Phase5Demo& operator=(const Phase5Demo&) = delete;
    Phase5Demo(Phase5Demo&&) noexcept = default;
    Phase5Demo& operator=(Phase5Demo&&) noexcept = default;
    
    // Run the complete Phase 5 demonstration
    void run();
    
    // Get configuration
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Get system components
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<AgentBrain> getAgentBrain() const { return agentBrain_; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    
private:
    // System components
    std::shared_ptr<Config> config_;
    std::shared_ptr<SimulationClock> clock_;
    std::shared_ptr<RandomGenerator> random_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<NeuralPrediction> prediction_;
    std::shared_ptr<WorkingMemory> workingMemory_;
    std::shared_ptr<EpisodicMemory> episodicMemory_;
    std::shared_ptr<ConceptFormation> conceptFormation_;
    std::shared_ptr<NeuralPlanner> neuralPlanner_;
    std::shared_ptr<SimpleWorld> world_;
    
    // Demonstration methods
    void createSystem();
    void configureComponents();
    void initializeSystem();
    void runPredictionDemo();
    void runMemoryDemo();
    void runCognitionDemo();
    void runActionDemo();
    void showResults();
    
    // Helper methods
    std::string actionTypeToString(ActionType type) const;
};

} // namespace nlm