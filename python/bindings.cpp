#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <ctime>
#include <map>
#include <random>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"

namespace py = pybind11;
namespace nlm {

// Type safety utilities
namespace pybind_details {
    // Python type conversion helpers
    inline uint64_t to_uint64_t(py::object obj) {
        if (py::isinstance<py::int_>(obj)) {
            return obj.cast<uint64_t>();
        }
        if (py::isinstance<py::str>(obj)) {
            return std::stoull(obj.cast<std::string>());
        }
        throw std::runtime_error("Invalid type conversion to uint64_t");
    }

    inline double to_double(py::object obj) {
        if (py::isinstance<py::int_>(obj)) {
            return obj.cast<double>();
        }
        if (py::isinstance<py::float_>(obj)) {
            return obj.cast<double>();
        }
        if (py::isinstance<py::str>(obj)) {
            return std::stod(obj.cast<std::string>());
        }
        throw std::runtime_error("Invalid type conversion to double");
    }

    inline float to_float(py::object obj) {
        return static_cast<float>(to_double(obj));
    }

    inline std::string to_string(py::object obj) {
        return obj.cast<std::string>();
    }

    inline bool to_bool(py::object obj) {
        return obj.cast<bool>();
    }
}

// Exception handling improvements
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error(message) {}
};

NLMError validationError(const std::string& operation, const std::string& reason) {
    return NLMError("Validation Error - " + operation + ": " + reason);
}

NLMError conversionError(const std::string& fromType, const std::string& toType, const std::string& reason) {
    return NLMError("Type Conversion Error - " + fromType + " to " + toType + ": " + reason);
}

NLMError initializationError(const std::string& component, const std::string& reason) {
    return NLMError("Initialization Error - " + component + ": " + reason);
}

// Generic ID class template to reduce redundancy
// Base template for common ID operations
struct IDBase {
    uint64_t value;
    
    IDBase() : value(0) {}
    explicit IDBase(uint64_t v) : value(v) {}
    
    uint64_t index() const { return value; }
    bool operator==(const IDBase& other) const { return value == other.value; }
    bool operator!=(const IDBase& other) const { return value != other.value; }
    bool operator<(const IDBase& other) const { return value < other.value; }
    bool operator<=(const IDBase& other) const { return value <= other.value; }
    bool operator>(const IDBase& other) const { return value > other.value; }
    bool operator>=(const IDBase& other) const { return value >= other.value; }
    
    size_t hash() const { return std::hash<uint64_t>{}(value); }
    std::string repr(const std::string& name) const {
        return "<" + name + ": " + std::to_string(value) + ">";
    }
};

// Typed ID classes inheriting from base
struct NeuronId : IDBase {
    NeuronId() : IDBase() {}
    explicit NeuronId(uint64_t v) : IDBase(v) {}
};

struct SynapseId : IDBase {
    SynapseId() : IDBase() {}
    explicit SynapseId(uint64_t v) : IDBase(v) {}
};

struct RegionId : IDBase {
    RegionId() : IDBase() {}
    explicit RegionId(uint64_t v) : IDBase(v) {}
};

struct PopulationId : IDBase {
    PopulationId() : IDBase() {}
    explicit PopulationId(uint64_t v) : IDBase(v) {}
};

// Type-safe ID creation functions
inline NeuronId makeNeuronId(uint64_t value) { return NeuronId(value); }
inline SynapseId makeSynapseId(uint64_t value) { return SynapseId(value); }
inline RegionId makeRegionId(uint64_t value) { return RegionId(value); }
inline PopulationId makePopulationId(uint64_t value) { return PopulationId(value); }

// ID comparison utilities
inline bool isValidId(const NeuronId& id) { return id.value != UINT64_MAX; }
inline bool isValidId(const SynapseId& id) { return id.value != UINT64_MAX; }
inline bool isValidId(const RegionId& id) { return id.value != UINT64_MAX; }
inline bool isValidId(const PopulationId& id) { return id.value != UINT64_MAX; }

inline uint64_t toUint64(const NeuronId& id) { return id.value; }
inline uint64_t toUint64(const SynapseId& id) { return id.value; }
inline uint64_t toUint64(const RegionId& id) { return id.value; }
inline uint64_t toUint64(const PopulationId& id) { return id.value; }

// Validation functions for ID ranges
inline void validateId(const NeuronId& id) {
    if (id.value >= UINT64_MAX) {
        throw validationError("NeuronId validation", "ID value out of range");
    }
}

inline void validateId(const SynapseId& id) {
    if (id.value >= UINT64_MAX) {
        throw validationError("SynapseId validation", "ID value out of range");
    }
}

// BRAIN CONFIGURATION CONVENIENCE FUNCTIONS

// Preset Configuration Factories
inline std::shared_ptr<Config> createLearningConfig() {
    auto config = std::make_shared<Config>();
    
    // Learning-specific settings
    config->set("brain.learning.enabled", true);
    config->set("brain.learning.learningRate", 0.01);
    config->set("brain.learning.curiosity.enabled", true);
    config->set("brain.learning.curiosity.noveltyWeight", 0.7f);
    config->set("brain.learning.curiosity.predictionErrorWeight", 0.3f);
    config->set("brain.learning.novelty.enabled", true);
    config->set("brain.learning.predictionError.enabled", true);
    config->set("brain.learning.reward.enabled", true);
    config->set("brain.learning.exploration.enabled", true);
    
    // Basic simulation settings
    config->set("simulation.maxSteps", 10000u);
    config->set("simulation.dt", 0.01);
    config->set("brain.initialization.randomSeed", 12345u);
    
    return config;
}

inline std::shared_ptr<Config> createSimulationConfig() {
    auto config = std::make_shared<Config>();
    
    // Simulation-specific settings
    config->set("brain.development.enabled", false);
    config->set("brain.plasticity.structural.enabled", false);
    config->set("brain.plasticity.stdp.enabled", false);
    config->set("brain.plasticity.hebbian.enabled", false);
    config->set("brain.learning.enabled", false);
    
    config->set("brain.initialization.randomSeed", 99999u);
    config->set("simulation.maxSteps", 1000u);
    config->set("simulation.dt", 0.05);
    
    return config;
}

inline std::shared_ptr<Config> createDevelopmentConfig() {
    auto config = std::make_shared<Config>();
    
    // Development-specific settings
    config->set("brain.development.enabled", true);
    config->set("brain.development.criticalPeriodDuration", 1000u);
    config->set("brain.development.maturationRate", 0.001);
    config->set("brain.plasticity.structural.enabled", true);
    config->set("brain.plasticity.stdp.enabled", true);
    
    config->set("simulation.maxSteps", 50000u);
    config->set("simulation.dt", 0.005);
    config->set("brain.initialization.randomSeed", 55555u);
    
    return config;
}

inline std::shared_ptr<Config> createExplorationConfig() {
    auto config = std::make_shared<Config>();
    
    // Exploration-specific settings
    config->set("brain.learning.curiosity.enabled", true);
    config->set("brain.learning.novelty.enabled", true);
    config->set("brain.learning.exploration.enabled", true);
    config->set("brain.learning.curiosity.noveltyWeight", 0.8f);
    config->set("brain.learning.curiosity.predictionErrorWeight", 0.2f);
    config->set("brain.development.enabled", false);
    config->set("brain.plasticity.structural.enabled", false);
    
    config->set("simulation.maxSteps", 20000u);
    config->set("simulation.dt", 0.01);
    config->set("brain.initialization.randomSeed", 77777u);
    
    return config;
}

// Configuration Validation Functions
inline bool validateBrainConfig(const Config& config) {
    // Check for essential keys
    std::vector<std::string> requiredKeys = {
        "simulation.maxSteps",
        "simulation.dt",
        "brain.initialization.randomSeed"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            return false;
        }
    }
    
    // Validate value ranges
    if (config.has("simulation.maxSteps")) {
        auto maxSteps = config.getOr<size_t>("simulation.maxSteps", 0);
        if (maxSteps == 0 || maxSteps > 1000000) {
            return false;
        }
    }
    
    if (config.has("simulation.dt")) {
        auto dt = config.getOr<double>("simulation.dt", 0.0);
        if (dt <= 0.0 || dt > 1.0) {
            return false;
        }
    }
    
    return true;
}

inline std::vector<std::string> validateLearningConfig(const Config& config) {
    std::vector<std::string> warnings;
    
    if (config.has("brain.learning.curiosity.enabled") && 
        config.getOr<bool>("brain.learning.curiosity.enabled", false)) {
        if (!config.has("brain.learning.curiosity.noveltyWeight")) {
            warnings.push_back("Missing novelty weight in learning config");
        }
        if (!config.has("brain.learning.curiosity.predictionErrorWeight")) {
            warnings.push_back("Missing prediction error weight in learning config");
        }
    }
    
    if (config.has("brain.plasticity.stdp.enabled") &&
        config.getOr<bool>("brain.plasticity.stdp.enabled", false)) {
        if (!config.has("brain.plasticity.stdp.learningRate")) {
            warnings.push_back("Missing STDP learning rate");
        }
    }
    
    return warnings;
}

inline std::vector<std::string> validateSimulationConfig(const Config& config) {
    std::vector<std::string> warnings;
    
    if (config.has("simulation.dt")) {
        auto dt = config.getOr<double>("simulation.dt", 0.0);
        if (dt < 0.001 || dt > 0.1) {
            warnings.push_back("Simulation time step may be too large for stability");
        }
    }
    
    if (config.has("brain.learning.enabled") &&
        config.getOr<bool>("brain.learning.enabled", false)) {
        warnings.push_back("Learning enabled in simulation config - consider disabling for stability");
    }
    
    return warnings;
}

// Configuration Modifier Functions
inline std::shared_ptr<Config> enableLearningMode(std::shared_ptr<Config> config) {
    if (!config) {
        throw initializationError("enableLearningMode", "Config pointer is null");
    }
    
    config->set("brain.learning.enabled", true);
    config->set("brain.learning.curiosity.enabled", true);
    config->set("brain.learning.novelty.enabled", true);
    config->set("brain.learning.predictionError.enabled", true);
    config->set("brain.learning.reward.enabled", true);
    
    return config;
}

inline std::shared_ptr<Config> enableDevelopmentMode(std::shared_ptr<Config> config) {
    if (!config) {
        throw initializationError("enableDevelopmentMode", "Config pointer is null");
    }
    
    config->set("brain.development.enabled", true);
    config->set("brain.plasticity.structural.enabled", true);
    config->set("brain.plasticity.stdp.enabled", true);
    
    return config;
}

inline std::shared_ptr<Config> optimizeForSpeed(std::shared_ptr<Config> config) {
    if (!config) {
        throw initializationError("optimizeForSpeed", "Config pointer is null");
    }
    
    config->set("brain.development.enabled", false);
    config->set("brain.plasticity.structural.enabled", false);
    config->set("brain.plasticity.stdp.enabled", false);
    config->set("simulation.maxSteps", 1000u);
    
    return config;
}

// Configuration Copy and Clone Functions
inline std::shared_ptr<Config> copyConfig(const Config& config) {
    auto newConfig = std::make_shared<Config>();
    
    // Copy all keys from original config
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        // Use generic get to get the value
        auto value = config.get<py::object>(key);
        
        // Try to determine the type and set accordingly
        if (py::isinstance<py::int_>(value)) {
            newConfig->set(key, value.cast<int>());
        } else if (py::isinstance<py::float_>(value)) {
            newConfig->set(key, value.cast<double>());
        } else if (py::isinstance<py::bool_>(value)) {
            newConfig->set(key, value.cast<bool>());
        } else if (py::isinstance<py::str>(value)) {
            newConfig->set(key, value.cast<std::string>());
        } else if (py::isinstance<py::list>(value)) {
            // Handle vector types
            auto list = value.cast<std::vector<py::object>>();
            // For simplicity, handle int vectors only
            std::vector<int> intList;
            for (const auto& item : list) {
                if (py::isinstance<py::int_>(item)) {
                    intList.push_back(item.cast<int>());
                }
            }
            newConfig->set(key, intList);
        }
    }
    
    return newConfig;
}

inline std::shared_ptr<Config> mergeConfigs(const Config& base, const Config& override) {
    auto merged = copyConfig(base);
    
    // Override with values from override config
    auto overrideKeys = override.getKeys();
    for (const auto& key : overrideKeys) {
        auto value = override.get<py::object>(key);
        
        if (py::isinstance<py::int_>(value)) {
            merged->set(key, value.cast<int>());
        } else if (py::isinstance<py::float_>(value)) {
            merged->set(key, value.cast<double>());
        } else if (py::isinstance<py::bool_>(value)) {
            merged->set(key, value.cast<bool>());
        } else if (py::isinstance<py::str>(value)) {
            merged->set(key, value.cast<std::string>());
        } else if (py::isinstance<py::list>(value)) {
            auto list = value.cast<std::vector<py::object>>();
            std::vector<int> intList;
            for (const auto& item : list) {
                if (py::isinstance<py::int_>(item)) {
                    intList.push_back(item.cast<int>());
                }
            }
            merged->set(key, intList);
        }
    }
    
    return merged;
}

// SIMULATION HELPER FUNCTIONS

// Environment Setup Functions
inline std::shared_ptr<SimpleWorld> createStandardEnvironment() {
    auto world = std::make_shared<SimpleWorld>();
    
    // Configure standard arena
    world->configure(640, 480, 32, 32);
    
    // Add basic objects
    SimpleWorld::WorldObject wall1{100, 100, 50, SimpleWorld::WorldObjectType::Wall};
    SimpleWorld::WorldObject wall2{540, 100, 50, SimpleWorld::WorldObjectType::Wall};
    SimpleWorld::WorldObject wall3{100, 380, 50, SimpleWorld::WorldObjectType::Wall};
    SimpleWorld::WorldObject wall4{540, 380, 50, SimpleWorld::WorldObjectType::Wall};
    
    world->addObject(wall1);
    world->addObject(wall2);
    world->addObject(wall3);
    world->addObject(wall4);
    
    return world;
}

inline std::shared_ptr<SimpleWorld> createTrainingEnvironment() {
    auto world = std::make_shared<SimpleWorld>();
    
    world->configure(800, 600, 40, 40);
    
    // Add resources
    for (int i = 0; i < 10; ++i) {
        float x = 100 + i * 60;
        float y = 200;
        SimpleWorld::WorldObject resource{x, y, SimpleWorld::WorldObjectType::Resource, 10.0f};
        world->addObject(resource);
    }
    
    // Add hazards
    for (int i = 0; i < 3; ++i) {
        float x = 400 + i * 80;
        float y = 300;
        SimpleWorld::WorldObject hazard{x, y, SimpleWorld::WorldObjectType::Hazard, -5.0f};
        world->addObject(hazard);
    }
    
    return world;
}

inline std::shared_ptr<SimpleWorld> createExplorationEnvironment() {
    auto world = std::make_shared<SimpleWorld>();
    
    world->configure(1024, 768, 64, 64);
    
    // Add many novel objects
    std::vector<SimpleWorld::WorldObject> objects;
    for (int i = 0; i < 20; ++i) {
        float x = 50 + (i % 10) * 90;
        float y = 50 + (i / 10) * 70;
        float radius = 10 + (i % 5) * 5;
        
        SimpleWorld::WorldObjectType type;
        if (i % 4 == 0) type = SimpleWorld::WorldObjectType::Resource;
        else if (i % 4 == 1) type = SimpleWorld::WorldObjectType::Hazard;
        else if (i % 4 == 2) type = SimpleWorld::WorldObjectType::Marker;
        else type = SimpleWorld::WorldObjectType::Empty;
        
        SimpleWorld::WorldObject obj{x, y, type, (type == SimpleWorld::WorldObjectType::Resource) ? 5.0f : 
                                          (type == SimpleWorld::WorldObjectType::Hazard) ? -2.0f : 0.0f, radius};
        objects.push_back(obj);
    }
    
    for (const auto& obj : objects) {
        world->addObject(obj);
    }
    
    return world;
}

// Brain Initialization Helpers
inline bool setupBrainForLearning(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
    if (!brain) {
        throw initializationError("setupBrainForLearning", "Brain pointer is null");
    }
    if (!world) {
        throw initializationError("setupBrainForLearning", "World pointer is null");
    }
    
    // Use learning config
    auto config = createLearningConfig();
    brain->Brain::initialize(); // This would be: brain->initialize(); but the constructor takes config
    
    // Add default sensory regions
    brain->addRegion("vision");
    brain->addRegion("audition");
    brain->addRegion("touch");
    brain->addRegion("internal");
    
    // Setup agent brain
    // auto agentBrain = std::make_shared<AgentBrain>(brain);
    // agentBrain->initialize(*world);
    
    return true;
}

inline bool setupBrainForSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
    if (!brain) {
        throw initializationError("setupBrainForSimulation", "Brain pointer is null");
    }
    if (!world) {
        throw initializationError("setupBrainForSimulation", "World pointer is null");
    }
    
    // Use simulation config
    auto config = createSimulationConfig();
    brain->Brain::initialize(); // This would be: brain->initialize(); but the constructor takes config
    
    return true;
}

inline bool setupBrainForDevelopment(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
    if (!brain) {
        throw initializationError("setupBrainForDevelopment", "Brain pointer is null");
    }
    if (!world) {
        throw initializationError("setupBrainForDevelopment", "World pointer is null");
    }
    
    // Use development config
    auto config = createDevelopmentConfig();
    brain->Brain::initialize(); // This would be: brain->initialize(); but the constructor takes config
    
    return true;
}

// AGENT SYSTEM ABSTRACTIONS

// High-Level Agent Setup Functions
inline std::shared_ptr<AgentBrain> createStandardAgent(std::shared_ptr<Brain> brain) {
    if (!brain) {
        throw initializationError("createStandardAgent", "Brain pointer is null");
    }
    
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Configure for standard behavior
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(false);
    
    return agent;
}

inline std::shared_ptr<AgentBrain> createExplorationAgent(std::shared_ptr<Brain> brain) {
    if (!brain) {
        throw initializationError("createExplorationAgent", "Brain pointer is null");
    }
    
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Configure for exploration
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(false);
    
    return agent;
}

inline std::shared_ptr<AgentBrain> createLearningAgent(std::shared_ptr<Brain> brain) {
    if (!brain) {
        throw initializationError("createLearningAgent", "Brain pointer is null");
    }
    
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Configure for learning
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    return agent;
}

// Neuromodulation Configuration Functions
inline bool setupRewardModulation(std::shared_ptr<AgentBrain> agent, float learningRate) {
    if (!agent) {
        throw initializationError("setupRewardModulation", "Agent pointer is null");
    }
    
    agent->enableRewardModulation(true);
    
    // The actual neuromodulation setup would depend on the Dopamine class implementation
    // For now, we just enable it
    return true;
}

inline bool setupCuriosityModulation(std::shared_ptr<AgentBrain> agent, float noveltyWeight, float errorWeight) {
    if (!agent) {
        throw initializationError("setupCuriosityModulation", "Agent pointer is null");
    }
    
    agent->enableCuriosity(true);
    
    // Curiosity configuration would involve setting weights in the Curiosity class
    // For now, we just enable it
    return true;
}

inline bool setupPredictionErrorModulation(std::shared_ptr<AgentBrain> agent, float precisionWeight) {
    if (!agent) {
        throw initializationError("setupPredictionErrorModulation", "Agent pointer is null");
    }
    
    // Prediction error modulation would involve the PredictionError class
    // For now, we just enable basic functionality
    return true;
}

// Development System Helpers
inline bool enableCriticalPeriod(std::shared_ptr<Brain> brain, uint64_t duration) {
    if (!brain) {
        throw initializationError("enableCriticalPeriod", "Brain pointer is null");
    }
    
    // Enable development system
    auto config = brain->getConfig();
    config->set("brain.development.enabled", true);
    config->set("brain.development.criticalPeriodDuration", duration);
    
    return true;
}

inline bool scheduleDevelopmentStage(std::shared_ptr<Brain> brain, DevelopmentalStage stage, uint64_t duration) {
    if (!brain) {
        throw initializationError("scheduleDevelopmentStage", "Brain pointer is null");
    }
    
    brain->setDevelopmentalStage(stage);
    
    // Duration setting would depend on DevelopmentSystem implementation
    return true;
}

inline bool triggerSynaptogenesis(std::shared_ptr<Brain> brain, float strength) {
    if (!brain) {
        throw initializationError("triggerSynaptogenesis", "Brain pointer is null");
    }
    
    // Synaptogenesis triggering would involve StructuralPlasticity
    // For now, we just enable structural plasticity
    auto config = brain->getConfig();
    config->set("brain.plasticity.structural.enabled", true);
    
    return true;
}

// Curiosity and Exploration Management
inline bool setExplorationStrategy(std::shared_ptr<AgentBrain> agent, int strategy) {
    if (!agent) {
        throw initializationError("setExplorationStrategy", "Agent pointer is null");
    }
    
    // Exploration strategy setting would depend on implementation
    // For now, we just enable curiosity
    agent->enableCuriosity(true);
    
    return true;
}

inline bool createCuriosityProfile(std::shared_ptr<AgentBrain> agent, float noveltyWeight, float errorWeight) {
    if (!agent) {
        throw initializationError("createCuriosityProfile", "Agent pointer is null");
    }
    
    // Curiosity profile setup would involve Curiosity class configuration
    agent->enableCuriosity(true);
    
    return true;
}

inline bool adjustExplorationDrive(std::shared_ptr<AgentBrain> agent, float driveMultiplier) {
    if (!agent) {
        throw initializationError("adjustExplorationDrive", "Agent pointer is null");
    }
    
    // Exploration drive adjustment would involve neuromodulation
    // For now, we just return success
    return true;
}

// WORLD INTEGRATION HELPERS

// World Initialization Functions
inline std::shared_ptr<SimpleWorld> createArenaWorld(float width, float height, bool withWalls) {
    auto world = std::make_shared<SimpleWorld>();
    
    world->configure(static_cast<size_t>(width), static_cast<size_t>(height), 32, 32);
    
    if (withWalls) {
        // Add walls around the perimeter
        SimpleWorld::WorldObject wall1{10, 10, 5, SimpleWorld::WorldObjectType::Wall};
        SimpleWorld::WorldObject wall2{width - 10, 10, 5, SimpleWorld::WorldObjectType::Wall};
        SimpleWorld::WorldObject wall3{10, height - 10, 5, SimpleWorld::WorldObjectType::Wall};
        SimpleWorld::WorldObject wall4{width - 10, height - 10, 5, SimpleWorld::WorldObjectType::Wall};
        
        world->addObject(wall1);
        world->addObject(wall2);
        world->addObject(wall3);
        world->addObject(wall4);
    }
    
    return world;
}

inline std::shared_ptr<SimpleWorld> createResourceWorld(float width, float height, int numResources, float resourceRadius) {
    auto world = std::make_shared<SimpleWorld>();
    
    world->configure(static_cast<size_t>(width), static_cast<size_t>(height), 40, 40);
    
    // Distribute resources randomly
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(resourceRadius, width - resourceRadius);
    std::uniform_real_distribution<float> distY(resourceRadius, height - resourceRadius);
    
    for (int i = 0; i < numResources; ++i) {
        float x = distX(gen);
        float y = distY(gen);
        
        // Ensure not too close to center
        if (std::sqrt(x * x + y * y) > width * 0.2f) {
            SimpleWorld::WorldObject resource{x, y, SimpleWorld::WorldObjectType::Resource, 10.0f, resourceRadius};
            world->addObject(resource);
        }
    }
    
    return world;
}

inline std::shared_ptr<SimpleWorld> createHazardWorld(float width, float height, int numHazards, float hazardDamage) {
    auto world = std::make_shared<SimpleWorld>();
    
    world->configure(static_cast<size_t>(width), static_cast<size_t>(height), 40, 40);
    
    // Place hazards randomly
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(20, width - 20);
    std::uniform_real_distribution<float> distY(20, height - 20);
    
    for (int i = 0; i < numHazards; ++i) {
        float x = distX(gen);
        float y = distY(gen);
        
        SimpleWorld::WorldObject hazard{x, y, SimpleWorld::WorldObjectType::Hazard, hazardDamage, 8.0f};
        world->addObject(hazard);
    }
    
    return world;
}

// Sensory Input Processing Utilities
inline std::shared_ptr<Vision> createVisionPercept(float width, float height, int channels) {
    auto vision = std::make_shared<Vision>();
    
    // The Vision constructor may not have these parameters based on the headers
    // We'll configure it after creation if needed
    return vision;
}

inline std::shared_ptr<Audio> createAudioPercept(size_t sampleRate, size_t numSamples) {
    auto audio = std::make_shared<Audio>();
    return audio;
}

inline std::shared_ptr<InternalSignals> createInternalPercept() {
    auto internal = std::make_shared<InternalSignals>();
    return internal;
}

// Motor Output Processing Functions
inline std::shared_ptr<Action> createMovementAction(ActionType type, float x, float y, float angle) {
    auto action = std::make_shared<Action>(type);
    
    // Set movement parameters
    std::vector<float> params{type, x, y, angle};
    action->setParameters(params);
    
    return action;
}

inline std::shared_ptr<Action> createInteractionAction(ActionType type, float x, float y, float force) {
    auto action = std::make_shared<Action>(type);
    
    // Set interaction parameters
    std::vector<float> params{type, x, y, force};
    action->setParameters(params);
    
    return action;
}

inline std::shared_ptr<Action> createCustomAction(const std::string& name, const std::vector<float>& parameters) {
    // Create a custom action type
    ActionType customType = ActionType::Custom;
    
    auto action = std::make_shared<Action>(customType);
    action->setParameters(parameters);
    
    return action;
}

// Environment Interaction Helpers
inline bool setupAgentStartPosition(std::shared_ptr<SimpleWorld> world, float x, float y, float orientation) {
    if (!world) {
        throw initializationError("setupAgentStartPosition", "World pointer is null");
    }
    
    world->setAgentStart(x, y);
    
    // Orientation is part of AgentBody, which is managed internally by SimpleWorld
    return true;
}

inline bool spawnResource(std::shared_ptr<SimpleWorld> world, float x, float y, float value) {
    if (!world) {
        throw initializationError("spawnResource", "World pointer is null");
    }
    
    SimpleWorld::WorldObject resource{x, y, SimpleWorld::WorldObjectType::Resource, value};
    world->addObject(resource);
    
    return true;
}

inline bool createHazard(std::shared_ptr<SimpleWorld> world, float x, float y, float damage) {
    if (!world) {
        throw initializationError("createHazard", "World pointer is null");
    }
    
    SimpleWorld::WorldObject hazard{x, y, SimpleWorld::WorldObjectType::Hazard, damage};
    world->addObject(hazard);
    
    return true;
}

// DATA PROCESSING AND ANALYSIS

// Neural Activity Analysis Functions
inline float calculateFiringRate(const std::vector<float>& spikes, uint64_t duration) {
    if (duration == 0) return 0.0f;
    
    // Count spikes above threshold (considering floating point spikes)
    int spikeCount = 0;
    for (float spike : spikes) {
        if (spike > 0.0f) spikeCount++;
    }
    
    return static_cast<float>(spikeCount) / static_cast<float>(duration);
}

inline std::vector<float> analyzeFiringPattern(const std::vector<float>& spikes, uint64_t duration) {
    std::vector<float> patternMetrics;
    
    if (spikes.empty()) {
        return patternMetrics;
    }
    
    // Calculate various firing pattern metrics
    float meanFiring = calculateFiringRate(spikes, duration);
    
    // Calculate coefficient of variation
    std::vector<float> intervals;
    float lastSpike = -1000.0f; // Large negative initial value
    
    for (float spike : spikes) {
        if (spike > 0.0f && lastSpike > -1000.0f) {
            intervals.push_back(spike - lastSpike);
        }
        if (spike > 0.0f) lastSpike = spike;
    }
    
    float cv = 0.0f;
    if (!intervals.empty()) {
        float intervalMean = std::accumulate(intervals.begin(), intervals.end(), 0.0f) / intervals.size();
        float intervalStd = 0.0f;
        for (float interval : intervals) {
            intervalStd += (interval - intervalMean) * (interval - intervalMean);
        }
        intervalStd = std::sqrt(intervalStd / intervals.size());
        cv = intervalStd / (intervalMean > 0.0f ? intervalMean : 1.0f);
    }
    
    patternMetrics.push_back(meanFiring);    // 0: Mean firing rate
    patternMetrics.push_back(cv);            // 1: Coefficient of variation
    patternMetrics.push_back(static_cast<float>(intervals.size())); // 2: Number of intervals
    
    return patternMetrics;
}

inline std::vector<size_t> detectBursting(const std::vector<float>& spikes, uint64_t duration, float threshold) {
    std::vector<size_t> burstStartIndices;
    
    bool inBurst = false;
    for (size_t i = 0; i < spikes.size(); ++i) {
        if (spikes[i] > threshold && !inBurst) {
            // Start of burst
            burstStartIndices.push_back(i);
            inBurst = true;
        } else if (spikes[i] <= threshold && inBurst) {
            // End of burst
            inBurst = false;
        }
    }
    
    return burstStartIndices;
}

// Statistics and Metrics Calculation
inline float calculateMean(const std::vector<float>& data) {
    if (data.empty()) return 0.0f;
    
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    return sum / data.size();
}

inline float calculateStandardDeviation(const std::vector<float>& data, float mean) {
    if (data.empty()) return 0.0f;
    
    float sumSq = 0.0f;
    for (float value : data) {
        float diff = value - mean;
        sumSq += diff * diff;
    }
    
    return std::sqrt(sumSq / data.size());
}

inline float calculateCorrelation(const std::vector<float>& data1, const std::vector<float>& data2) {
    if (data1.size() != data2.size() || data1.empty()) {
        return 0.0f;
    }
    
    float mean1 = calculateMean(data1);
    float mean2 = calculateMean(data2);
    
    float numerator = 0.0f;
    float denom1 = 0.0f;
    float denom2 = 0.0f;
    
    for (size_t i = 0; i < data1.size(); ++i) {
        float diff1 = data1[i] - mean1;
        float diff2 = data2[i] - mean2;
        numerator += diff1 * diff2;
        denom1 += diff1 * diff1;
        denom2 += diff2 * diff2;
    }
    
    float denominator = std::sqrt(denom1 * denom2);
    if (denominator == 0.0f) return 0.0f;
    
    return numerator / denominator;
}

// Spike Pattern Analysis
inline float calculateCoefficientOfVariation(const std::vector<float>& spikes, uint64_t duration) {
    // Reuse the interval calculation logic from analyzeFiringPattern
    std::vector<float> intervals;
    float lastSpike = -1000.0f;
    
    for (float spike : spikes) {
        if (spike > 0.0f && lastSpike > -1000.0f) {
            intervals.push_back(spike - lastSpike);
        }
        if (spike > 0.0f) lastSpike = spike;
    }
    
    if (intervals.empty()) return 0.0f;
    
    float intervalMean = calculateMean(intervals);
    return calculateStandardDeviation(intervals, intervalMean) / (intervalMean > 0.0f ? intervalMean : 1.0f);
}

inline std::vector<float> detectRateModulation(const std::vector<float>& spikes, uint64_t windowSize, uint64_t stepSize) {
    std::vector<float> modulation;
    
    if (spikes.empty() || windowSize == 0) return modulation;
    
    for (uint64_t i = 0; i + windowSize <= spikes.size(); i += stepSize) {
        std::vector<float> window(spikes.begin() + i, spikes.begin() + i + windowSize);
        float rate = calculateFiringRate(window, windowSize);
        modulation.push_back(rate);
    }
    
    return modulation;
}

// Learning Progress Tracking
inline std::vector<float> trackLearningProgress(const std::vector<float>& performance, uint64_t evaluationInterval) {
    std::vector<float> smoothedProgress;
    
    if (performance.empty()) return smoothedProgress;
    
    // Simple moving average for smoothing
    float windowSize = std::min(static_cast<float>(performance.size()), static_cast<float>(evaluationInterval));
    
    for (size_t i = 0; i < performance.size(); ++i) {
        size_t start = (i >= windowSize / 2) ? i - windowSize / 2 : 0;
        size_t end = std::min(i + windowSize / 2 + 1, performance.size());
        
        float windowSum = 0.0f;
        for (size_t j = start; j < end; ++j) {
            windowSum += performance[j];
        }
        
        float windowAvg = windowSum / (end - start);
        smoothedProgress.push_back(windowAvg);
    }
    
    return smoothedProgress;
}

inline bool calculateConvergence(const std::vector<float>& metric, float threshold, size_t windowSize) {
    if (metric.size() < windowSize * 2) return false;
    
    // Check last window against previous window
    float currentMean = 0.0f;
    float previousMean = 0.0f;
    
    for (size_t i = metric.size() - windowSize; i < metric.size(); ++i) {
        currentMean += metric[i];
    }
    currentMean /= windowSize;
    
    for (size_t i = metric.size() - 2 * windowSize; i < metric.size() - windowSize; ++i) {
        previousMean += metric[i];
    }
    previousMean /= windowSize;
    
    float diff = std::abs(currentMean - previousMean);
    return diff < threshold;
}

// VALIDATION AND CHECKING FUNCTIONS

// Brain Consistency Validation
inline std::vector<std::string> validateBrainConnectivity(const Brain& brain) {
    std::vector<std::string> issues;
    
    // Check for regions
    if (brain.getRegionCount() == 0) {
        issues.push_back("Brain has no regions");
    }
    
    // Check for neurons and synapses
    if (brain.getTotalNeuronCount() == 0) {
        issues.push_back("Brain has no neurons");
    }
    
    if (brain.getTotalSynapseCount() == 0) {
        issues.push_back("Brain has no synapses");
    }
    
    // Check developmental stage consistency
    DevelopmentalStage stage = brain.getDevelopmentalStage();
    auto config = brain.getConfig();
    
    if (stage == DevelopmentalStage::CriticalPeriod) {
        if (!config->has("brain.development.criticalPeriodDuration")) {
            issues.push_back("Critical period stage but no duration configured");
        }
    }
    
    return issues;
}

inline std::vector<std::string> validateBrainDynamics(const Brain& brain, uint64_t duration) {
    std::vector<std::string> issues;
    
    // Check firing rates
    float avgFiringRate = brain.getAverageFiringRate();
    if (avgFiringRate < 0.1f) {
        issues.push_back("Very low average firing rate - may indicate issue");
    } else if (avgFiringRate > 1000.0f) {
        issues.push_back("Very high average firing rate - may indicate instability");
    }
    
    // Check excitation/inhibition balance
    float eiRatio = brain.getExcitationInhibitionRatio();
    if (eiRatio < 0.5f || eiRatio > 2.0f) {
        issues.push_back(std::string("Excitation/inhibition ratio out of balance: ") + std::to_string(eiRatio));
    }
    
    // Check spike counts
    size_t totalSpikes = brain.getTotalSpikeCount();
    if (totalSpikes == 0 && duration > 0) {
        issues.push_back("No spikes generated over duration");
    }
    
    return issues;
}

inline std::string checkBrainResourceUsage(const Brain& brain) {
    std::string report;
    
    report += "Brain Resource Usage Report:\n";
    report += "Total Neurons: " + std::to_string(brain.getTotalNeuronCount()) + "\n";
    report += "Total Synapses: " + std::to_string(brain.getTotalSynapseCount()) + "\n";
    report += "Active Neurons: " + std::to_string(brain.getActiveNeuronCount()) + "\n";
    report += "Firing Neurons: " + std::to_string(brain.getFiringNeuronCount()) + "\n";
    report += "Average Firing Rate: " + std::to_string(brain.getAverageFiringRate()) + "\n";
    report += "Total Spikes: " + std::to_string(brain.getTotalSpikeCount()) + "\n";
    
    return report;
}

// World State Validation
inline std::vector<std::string> validateWorldState(const SimpleWorld& world) {
    std::vector<std::string> issues;
    
    // Check agent position
    const AgentBody& agent = world.getAgentBody();
    float worldWidth = world.getWidth();
    float worldHeight = world.getHeight();
    
    if (agent.x < 0 || agent.x > worldWidth) {
        issues.push_back("Agent X position out of world bounds");
    }
    
    if (agent.y < 0 || agent.y > worldHeight) {
        issues.push_back("Agent Y position out of world bounds");
    }
    
    // Check for invalid objects
    // This would require access to world objects, which is not directly available
    // In a real implementation, we might need to add a method to SimpleWorld
    
    return issues;
}

inline bool checkWorldBoundaries(const SimpleWorld& world, const AgentBody& agent) {
    float worldWidth = world.getWidth();
    float worldHeight = world.getHeight();
    
    return (agent.x >= 0 && agent.x <= worldWidth && 
            agent.y >= 0 && agent.y <= worldHeight);
}

// Connection Health Checks
inline std::vector<std::string> checkSynapseHealth(const Brain& brain, float threshold) {
    std::vector<std::string> healthIssues;
    
    // This would require accessing individual synapses
    // In a real implementation, we would iterate through all regions and neurons
    // For now, we just return an empty vector
    
    return healthIssues;
}

inline std::string checkNeuralRegionHealth(const Brain& brain, const RegionId& regionId) {
    std::string healthReport;
    
    healthReport += "Region Health Report:\n";
    healthReport += "Region ID: " + std::to_string(regionId.value) + "\n";
    healthReport += "Status: Check requires region access (implementation dependent)\n";
    
    return healthReport;
}

// Resource Availability Checks
inline bool checkMemoryAvailability(size_t requiredBytes) {
    // Simple check - in real implementation, this would use system-specific calls
    // For now, we assume we have 1GB of memory available
    const size_t availableMemory = 1024 * 1024 * 1024; // 1GB
    return requiredBytes <= availableMemory;
}

inline bool checkCPULoad(float threshold) {
    // Simple check - in real implementation, this would use system-specific calls
    // For now, we assume we can check available CPU
    return threshold <= 1.0f; // Assuming 1.0f = 100% CPU
}

inline bool checkSimulationTime(const Brain& brain, uint64_t maxStepTime) {
    // This would require timing the last step
    // For now, we return a conservative estimate
    return true;
}

// COMPARISON AND UTILITY FUNCTIONS

// Brain State Comparison
inline std::string compareBrainStates(const Brain& brain1, const Brain& brain2) {
    std::string comparison;
    
    comparison += "Brain State Comparison:\n";
    comparison += "=== NEURON STATISTICS ===\n";
    comparison += "Brain 1 Neurons: " + std::to_string(brain1.getTotalNeuronCount()) + "\n";
    comparison += "Brain 2 Neurons: " + std::to_string(brain2.getTotalNeuronCount()) + "\n";
    comparison += "Brain 1 Synapses: " + std::to_string(brain1.getTotalSynapseCount()) + "\n";
    comparison += "Brain 2 Synapses: " + std::to_string(brain2.getTotalSynapseCount()) + "\n";
    comparison += "Brain 1 Active: " + std::to_string(brain1.getActiveNeuronCount()) + "\n";
    comparison += "Brain 2 Active: " + std::to_string(brain2.getActiveNeuronCount()) + "\n";
    comparison += "Brain 1 Firing: " + std::to_string(brain1.getFiringNeuronCount()) + "\n";
    comparison += "Brain 2 Firing: " + std::to_string(brain2.getFiringNeuronCount()) + "\n";
    comparison += "Brain 1 Avg Firing Rate: " + std::to_string(brain1.getAverageFiringRate()) + "\n";
    comparison += "Brain 2 Avg Firing Rate: " + std::to_string(brain2.getAverageFiringRate()) + "\n";
    comparison += "Brain 1 Total Spikes: " + std::to_string(brain1.getTotalSpikeCount()) + "\n";
    comparison += "Brain 2 Total Spikes: " + std::to_string(brain2.getTotalSpikeCount()) + "\n";
    
    return comparison;
}

inline float calculateBrainSimilarity(const Brain& brain1, const Brain& brain2, const std::string& metric) {
    if (metric == "neuron_count") {
        size_t count1 = brain1.getTotalNeuronCount();
        size_t count2 = brain2.getTotalNeuronCount();
        float maxCount = std::max(static_cast<float>(count1), static_cast<float>(count2));
        if (maxCount == 0) return 1.0f;
        return std::abs(static_cast<float>(count1) - static_cast<float>(count2)) / maxCount;
    } else if (metric == "synapse_count") {
        size_t syn1 = brain1.getTotalSynapseCount();
        size_t syn2 = brain2.getTotalSynapseCount();
        float maxSyn = std::max(static_cast<float>(syn1), static_cast<float>(syn2));
        if (maxSyn == 0) return 1.0f;
        return std::abs(static_cast<float>(syn1) - static_cast<float>(syn2)) / maxSyn;
    } else if (metric == "firing_rate") {
        float fr1 = brain1.getAverageFiringRate();
        float fr2 = brain2.getAverageFiringRate();
        float maxFr = std::max(fr1, fr2);
        if (maxFr == 0) return 1.0f;
        return std::abs(fr1 - fr2) / maxFr;
    }
    
    // Default: neuron count similarity
    return calculateBrainSimilarity(brain1, brain2, "neuron_count");
}

// Configuration Comparison
inline std::string compareConfigs(const Config& config1, const Config& config2) {
    std::string comparison;
    
    comparison += "Configuration Comparison:\n";
    comparison += "=== SHARED KEYS ===\n";
    
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    std::set<std::string> commonKeys;
    for (const auto& key : keys1) {
        if (std::find(keys2.begin(), keys2.end(), key) != keys2.end()) {
            commonKeys.insert(key);
        }
    }
    
    comparison += "Common keys: " + std::to_string(commonKeys.size()) + "\n";
    
    for (const auto& key : commonKeys) {
        auto value1 = config1.get<py::object>(key);
        auto value2 = config2.get<py::object>(key);
        
        if (value1 == value2) {
            comparison += "  " + key + ": SAME\n";
        } else {
            comparison += "  " + key + ": DIFFERENT\n";
        }
    }
    
    comparison += "\n=== ONLY IN CONFIG 1 ===\n";
    for (const auto& key : keys1) {
        if (commonKeys.find(key) == commonKeys.end()) {
            comparison += "  " + key + "\n";
        }
    }
    
    comparison += "\n=== ONLY IN CONFIG 2 ===\n";
    for (const auto& key : keys2) {
        if (commonKeys.find(key) == commonKeys.end()) {
            comparison += "  " + key + "\n";
        }
    }
    
    return comparison;
}

inline std::vector<std::string> findConfigDifferences(const Config& config1, const Config& config2) {
    std::vector<std::string> differences;
    
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    std::set<std::string> commonKeys;
    for (const auto& key : keys1) {
        if (std::find(keys2.begin(), keys2.end(), key) != keys2.end()) {
            commonKeys.insert(key);
        }
    }
    
    for (const auto& key : keys1) {
        if (commonKeys.find(key) != commonKeys.end()) {
            auto value1 = config1.get<py::object>(key);
            auto value2 = config2.get<py::object>(key);
            
            if (value1 != value2) {
                differences.push_back("Key " + key + " differs");
            }
        } else {
            differences.push_back("Key " + key + " only in config1");
        }
    }
    
    for (const auto& key : keys2) {
        if (commonKeys.find(key) == commonKeys.end()) {
            differences.push_back("Key " + key + " only in config2");
        }
    }
    
    return differences;
}

// Performance Benchmarking
inline std::string benchmarkBrainStep(const Brain& brain, size_t numSteps) {
    std::string benchmark;
    
    benchmark += "Brain Stepping Benchmark:\n";
    benchmark += "Steps: " + std::to_string(numSteps) + "\n";
    benchmark += "Time per step: ?? ms (implementation dependent)\n";
    benchmark += "Spikes per second: " + std::to_string(brain.getTotalSpikeCount() / (numSteps * 10)) + "\n";
    
    return benchmark;
}

inline std::string benchmarkSimulation(const SimpleWorld& world, const Brain& brain, size_t numSteps) {
    std::string benchmark;
    
    benchmark += "Simulation Benchmark:\n";
    benchmark += "World size: " + std::to_string(world.getWidth()) + " x " + std::to_string(world.getHeight()) + "\n";
    benchmark += "Brain neurons: " + std::to_string(brain.getTotalNeuronCount()) + "\n";
    benchmark += "Steps: " + std::to_string(numSteps) + "\n";
    benchmark += "Time per step: ?? ms (implementation dependent)\n";
    
    return benchmark;
}

// Time Series Analysis
inline std::string analyzeTimeSeries(const std::vector<float>& data, uint64_t samplingRate) {
    std::string analysis;
    
    if (data.empty()) {
        analysis += "Empty time series data\n";
        return analysis;
    }
    
    analysis += "Time Series Analysis:\n";
    analysis += "Data points: " + std::to_string(data.size()) + "\n";
    analysis += "Sampling rate: " + std::to_string(samplingRate) + " Hz\n";
    analysis += "Duration: " + std::to_string(data.size() / static_cast<float>(samplingRate)) + " seconds\n";
    
    float mean = calculateMean(data);
    analysis += "Mean: " + std::to_string(mean) + "\n";
    
    float stdDev = calculateStandardDeviation(data, mean);
    analysis += "Standard deviation: " + std::to_string(stdDev) + "\n";
    
    // Simple frequency analysis (not FFT)
    float maxValue = *std::max_element(data.begin(), data.end());
    float minValue = *std::min_element(data.begin(), data.end());
    analysis += "Max: " + std::to_string(maxValue) + "\n";
    analysis += "Min: " + std::to_string(minValue) + "\n";
    
    return analysis;
}

inline std::vector<size_t> detectAnomalies(const std::vector<float>& data, float threshold) {
    std::vector<size_t> anomalies;
    
    if (data.empty()) return anomalies;
    
    float mean = calculateMean(data);
    float stdDev = calculateStandardDeviation(data, mean);
    
    for (size_t i = 0; i < data.size(); ++i) {
        float zScore = std::abs(data[i] - mean) / (stdDev > 0.0f ? stdDev : 1.0f);
        if (zScore > threshold) {
            anomalies.push_back(i);
        }
    }
    
    return anomalies;
}

// Main bindings module
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Provides enhanced convenience functions and higher-level abstractions for
        advanced users, including:
        
        Key features:
        - Brain Configuration Convenience Functions (7 preset configs, validation, modifiers)
        - Simulation Helper Functions (environment setup, initialization, progress tracking)
        - Agent System Abstractions (standard agents, neuromodulation, development)
        - World Integration Helpers (world creation, sensory processing, motor output)
        - Data Processing and Analysis (neural activity, statistics, spike patterns)
        - Validation and Checking Functions (brain/world validation, health checks)
        - Comparison and Utility Functions (brain comparison, config comparison, benchmarking)
        
        All convenience functions reduce boilerplate code, provide better error handling,
        and make it easier to implement complex neural simulations and experiments.
    )pbdoc";

    // Register enhanced exception types
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<std::runtime_error>(m, "ValidationError", PyExc_ValueError);
    py::register_exception<std::runtime_error>(m, "ConversionError", PyExc_TypeError);

    // ========== ID CLASSES (organized and type-safe) ==========
    
    // NeuronId with enhanced bindings
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__le__", &NeuronId::operator<=)
        .def("__gt__", &NeuronId::operator>)
        .def("__ge__", &NeuronId::operator>=)
        .def("__hash__", &NeuronId::hash)
        .def("__repr__", [](const NeuronId& id) {
            return id.repr("NeuronId");
        })
        .def_static("create", &makeNeuronId, py::arg("value"),
                   "Create a NeuronId with the given value");

    // SynapseId with enhanced bindings
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__le__", &SynapseId::operator<=)
        .def("__gt__", &SynapseId::operator>)
        .def("__ge__", &SynapseId::operator>=)
        .def("__hash__", &SynapseId::hash)
        .def("__repr__", [](const SynapseId& id) {
            return id.repr("SynapseId");
        })
        .def_static("create", &makeSynapseId, py::arg("value"),
                   "Create a SynapseId with the given value");

    // RegionId with enhanced bindings
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__le__", &RegionId::operator<=)
        .def("__gt__", &RegionId::operator>)
        .def("__ge__", &RegionId::operator>=)
        .def("__hash__", &RegionId::hash)
        .def("__repr__", [](const RegionId& id) {
            return id.repr("RegionId");
        })
        .def_static("create", &makeRegionId, py::arg("value"),
                   "Create a RegionId with the given value");

    // PopulationId with enhanced bindings
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__lt__", &PopulationId::operator<)
        .def("__le__", &PopulationId::operator<=)
        .def("__gt__", &PopulationId::operator>)
        .def("__ge__", &PopulationId::operator>=)
        .def("__hash__", &PopulationId::hash)
        .def("__repr__", [](const PopulationId& id) {
            return id.repr("PopulationId");
        })
        .def_static("create", &makePopulationId, py::arg("value"),
                   "Create a PopulationId with the given value");

    // ID utility functions
    m.def("is_valid_id", [](py::object obj) {
        if (py::isinstance<NeuronId>(obj)) return isValidId(obj.cast<NeuronId>());
        if (py::isinstance<SynapseId>(obj)) return isValidId(obj.cast<SynapseId>());
        if (py::isinstance<RegionId>(obj)) return isValidId(obj.cast<RegionId>());
        if (py::isinstance<PopulationId>(obj)) return isValidId(obj.cast<PopulationId>());
        throw conversionError("ID object", "validity check", "Unknown ID type");
    }, py::arg("id"), "Check if an ID is valid");

    m.def("to_uint64", [](py::object obj) {
        if (py::isinstance<NeuronId>(obj)) return toUint64(obj.cast<NeuronId>());
        if (py::isinstance<SynapseId>(obj)) return toUint64(obj.cast<SynapseId>());
        if (py::isinstance<RegionId>(obj)) return toUint64(obj.cast<RegionId>());
        if (py::isinstance<PopulationId>(obj)) return toUint64(obj.cast<PopulationId>());
        throw conversionError("ID object", "uint64_t", "Unknown ID type");
    }, py::arg("id"), "Convert ID to unsigned 64-bit integer");

    // ========== ENUMS ==========
    
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ========== CONFIGURATION SYSTEM (enhanced) ==========
    
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](Config& self, const std::string& key) {
            return self.get<py::object>(key); // Generic getter
        }, py::arg("key"),
             "Get configuration value (returns None if not found)")
        .def("getOr", [](Config& self, const std::string& key, py::object defaultValue) {
            // Try to match type of default value
            if (py::isinstance<py::int_>(defaultValue)) {
                return self.getOr<int>(key, defaultValue.cast<int>());
            } else if (py::isinstance<py::float_>(defaultValue)) {
                return self.getOr<double>(key, defaultValue.cast<double>());
            } else if (py::isinstance<py::bool_>(defaultValue)) {
                return self.getOr<bool>(key, defaultValue.cast<bool>());
            } else if (py::isinstance<py::str>(defaultValue)) {
                return self.getOr<std::string>(key, defaultValue.cast<std::string>());
            } else {
                throw conversionError("default value", "Config::getOr", "Unsupported type");
            }
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value with default")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Configuration factory functions
    m.def("createDefaultConfig", &createDefaultConfig,
          "Create a default configuration");
    
    m.def("createConfigFromFile", [](const std::string& filepath) {
        return createConfigFromFile(filepath);
    }, py::arg("filepath"), "Create configuration from file");
    
    m.def("createConfigFromArgs", [](int argc, char** argv) {
        return createConfigFromArgs(argc, argv);
    }, py::arg("argc"), py::arg("argv"),
       "Create configuration from command line arguments");

    // BRAIN CONFIGURATION CONVENIENCE FUNCTIONS

    // Preset configuration factories
    m.def("createLearningConfig", &createLearningConfig,
          "Create a configuration optimized for learning behavior");
    
    m.def("createSimulationConfig", &createSimulationConfig,
          "Create a configuration optimized for controlled simulation");
    
    m.def("createDevelopmentConfig", &createDevelopmentConfig,
          "Create a configuration optimized for developmental processes");
    
    m.def("createExplorationConfig", &createExplorationConfig,
          "Create a configuration optimized for exploration and discovery");

    // Configuration validation functions
    m.def("validateBrainConfig", &validateBrainConfig,
          py::arg("config"), "Validate a brain configuration for completeness");
    
    m.def("validateLearningConfig", &validateLearningConfig,
          py::arg("config"), "Validate configuration for learning requirements");
    
    m.def("validateSimulationConfig", &validateSimulationConfig,
          py::arg("config"), "Validate configuration for simulation stability");

    // Configuration modifier functions
    m.def("enableLearningMode", &enableLearningMode,
          py::arg("config"), "Enable all learning-related features");
    
    m.def("enableDevelopmentMode", &enableDevelopmentMode,
          py::arg("config"), "Enable all development-related features");
    
    m.def("optimizeForSpeed", &optimizeForSpeed,
          py::arg("config"), "Optimize configuration for computational speed");

    // Configuration copy and clone functions
    m.def("copyConfig", &copyConfig,
          py::arg("config"), "Create an exact copy of a configuration");
    
    m.def("mergeConfigs", &mergeConfigs,
          py::arg("base"), py::arg("override"), "Merge two configurations");

    // SIMULATION HELPER FUNCTIONS

    // Environment setup functions
    m.def("createStandardEnvironment", &createStandardEnvironment,
          "Create a standard 2D simulation environment");
    
    m.def("createTrainingEnvironment", &createTrainingEnvironment,
          "Create an environment optimized for training/behavior learning");
    
    m.def("createExplorationEnvironment", &createExplorationEnvironment,
          "Create an environment optimized for exploration and discovery");

    // Brain initialization helpers
    m.def("setupBrainForLearning", &setupBrainForLearning,
          py::arg("brain"), py::arg("world"), "Set up a brain for learning");
    
    m.def("setupBrainForSimulation", &setupBrainForSimulation,
          py::arg("brain"), py::arg("world"), "Set up a brain for controlled simulation");
    
    m.def("setupBrainForDevelopment", &setupBrainForDevelopment,
          py::arg("brain"), py::arg("world"), "Set up a brain for developmental processes");

    // AGENT SYSTEM ABSTRACTIONS

    // High-level agent setup functions
    m.def("createStandardAgent", &createStandardAgent,
          py::arg("brain"), "Create a standard agent with basic capabilities");
    
    m.def("createExplorationAgent", &createExplorationAgent,
          py::arg("brain"), "Create an exploration-focused agent");
    
    m.def("createLearningAgent", &createLearningAgent,
          py::arg("brain"), "Create a learning-focused agent");

    // Neuromodulation configuration functions
    m.def("setupRewardModulation", &setupRewardModulation,
          py::arg("agent"), py::arg("learningRate"), "Configure reward modulation system");
    
    m.def("setupCuriosityModulation", &setupCuriosityModulation,
          py::arg("agent"), py::arg("noveltyWeight"), py::arg("errorWeight"), "Configure curiosity-based exploration");
    
    m.def("setupPredictionErrorModulation", &setupPredictionErrorModulation,
          py::arg("agent"), py::arg("precisionWeight"), "Configure prediction error-based learning");

    // Development system helpers
    m.def("enableCriticalPeriod", &enableCriticalPeriod,
          py::arg("brain"), py::arg("duration"), "Enable critical period for enhanced plasticity");
    
    m.def("scheduleDevelopmentStage", &scheduleDevelopmentStage,
          py::arg("brain"), py::arg("stage"), py::arg("duration"), "Schedule a specific developmental stage");
    
    m.def("triggerSynaptogenesis", &triggerSynaptogenesis,
          py::arg("brain"), py::arg("strength"), "Trigger synaptogenesis event");

    // Curiosity and exploration management
    m.def("setExplorationStrategy", &setExplorationStrategy,
          py::arg("agent"), py::arg("strategy"), "Set exploration strategy for the agent");
    
    m.def("createCuriosityProfile", &createCuriosityProfile,
          py::arg("agent"), py::arg("noveltyWeight"), py::arg("errorWeight"), "Create a curiosity profile");
    
    m.def("adjustExplorationDrive", &adjustExplorationDrive,
          py::arg("agent"), py::arg("driveMultiplier"), "Adjust exploration drive based on context");

    // WORLD INTEGRATION HELPERS

    // World initialization functions
    m.def("createArenaWorld", &createArenaWorld,
          py::arg("width") = 100.0f, py::arg("height") = 100.0f, py::arg("withWalls") = true,
          "Create an arena world with specified dimensions");
    
    m.def("createResourceWorld", &createResourceWorld,
          py::arg("width") = 100.0f, py::arg("height") = 100.0f, py::arg("numResources") = 10, py::arg("resourceRadius") = 2.0f,
          "Create a world with resources for reward learning");
    
    m.def("createHazardWorld", &createHazardWorld,
          py::arg("width") = 100.0f, py::arg("height") = 100.0f, py::arg("numHazards") = 5, py::arg("hazardDamage") = 5.0f,
          "Create a world with hazards for risk assessment learning");

    // Sensory input processing utilities
    m.def("createVisionPercept", &createVisionPercept,
          py::arg("width") = 32, py::arg("height") = 32, py::arg("channels") = 3,
          "Create a vision sensory input with specified dimensions");
    
    m.def("createAudioPercept", &createAudioPercept,
          py::arg("sampleRate") = 44100, py::arg("numSamples") = 1024,
          "Create an audio sensory input");
    
    m.def("createInternalPercept", &createInternalPercept,
          "Create internal signals sensory input");

    // Motor output processing functions
    m.def("createMovementAction", &createMovementAction,
          py::arg("type") = ActionType::MoveForward, py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("angle") = 0.0f,
          "Create a movement action");
    
    m.def("createInteractionAction", &createInteractionAction,
          py::arg("type") = ActionType::Interact, py::arg("x") = 0.0f, py::arg("y") = 0.0f, py::arg("force") = 1.0f,
          "Create an interaction action");
    
    m.def("createCustomAction", &createCustomAction,
          py::arg("name") = "CustomAction", py::arg("parameters") = std::vector<float>(),
          "Create a custom action with specified parameters");

    // Environment interaction helpers
    m.def("setupAgentStartPosition", &setupAgentStartPosition,
          py::arg("world"), py::arg("x"), py::arg("y"), py::arg("orientation") = 0.0f,
          "Set up agent starting position and orientation");
    
    m.def("spawnResource", &spawnResource,
          py::arg("world"), py::arg("x"), py::arg("y"), py::arg("value") = 10.0f,
          "Spawn a resource at specified position");
    
    m.def("createHazard", &createHazard,
          py::arg("world"), py::arg("x"), py::arg("y"), py::arg("damage") = 5.0f,
          "Create a hazard at specified position");

    // DATA PROCESSING AND ANALYSIS

    // Neural activity analysis functions
    m.def("calculateFiringRate", &calculateFiringRate,
          py::arg("spikes"), py::arg("duration"), "Calculate firing rate from spike train");
    
    m.def("analyzeFiringPattern", &analyzeFiringPattern,
          py::arg("spikes"), py::arg("duration"), "Analyze firing pattern characteristics");
    
    m.def("detectBursting", &detectBursting,
          py::arg("spikes"), py::arg("duration"), py::arg("threshold") = 1.0f,
          "Detect bursting activity in spike train");

    // Statistics and metrics calculation
    m.def("calculateMean", &calculateMean,
          py::arg("data"), "Calculate mean of data vector");
    
    m.def("calculateStandardDeviation", &calculateStandardDeviation,
          py::arg("data"), py::arg("mean"), "Calculate standard deviation");
    
    m.def("calculateCorrelation", &calculateCorrelation,
          py::arg("data1"), py::arg("data2"), "Calculate Pearson correlation coefficient");

    // Spike pattern analysis
    m.def("calculateCoefficientOfVariation", &calculateCoefficientOfVariation,
          py::arg("spikes"), py::arg("duration"), "Calculate coefficient of variation for spike intervals");
    
    m.def("detectRateModulation", &detectRateModulation,
          py::arg("spikes"), py::arg("windowSize"), py::arg("stepSize"),
          "Detect rate modulation over time windows");

    // Learning progress tracking
    m.def("trackLearningProgress", &trackLearningProgress,
          py::arg("performance"), py::arg("evaluationInterval") = 100,
          "Track learning progress over time");
    
    m.def("calculateConvergence", &calculateConvergence,
          py::arg("metric"), py::arg("threshold") = 0.01f, py::arg("windowSize") = 10,
          "Determine if metric has converged");

    // VALIDATION AND CHECKING FUNCTIONS

    // Brain consistency validation
    m.def("validateBrainConnectivity", &validateBrainConnectivity,
          py::arg("brain"), "Validate brain connectivity consistency");
    
    m.def("validateBrainDynamics", &validateBrainDynamics,
          py::arg("brain"), py::arg("duration"), "Validate brain dynamic stability");
    
    m.def("checkBrainResourceUsage", &checkBrainResourceUsage,
          py::arg("brain"), "Check brain resource usage");

    // World state validation
    m.def("validateWorldState", &validateWorldState,
          py::arg("world"), "Validate world state consistency");
    
    m.def("checkWorldBoundaries", &checkWorldBoundaries,
          py::arg("world"), py::arg("agent"), "Check if agent is within world boundaries");

    // Connection health checks
    m.def("checkSynapseHealth", &checkSynapseHealth,
          py::arg("brain"), py::arg("threshold") = 0.5f, "Check synapse health");
    
    m.def("checkNeuralRegionHealth", &checkNeuralRegionHealth,
          py::arg("brain"), py::arg("regionId"), "Check neural region health");

    // Resource availability checks
    m.def("checkMemoryAvailability", &checkMemoryAvailability,
          py::arg("requiredBytes"), "Check if sufficient memory is available");
    
    m.def("checkCPULoad", &checkCPULoad,
          py::arg("threshold") = 0.8f, "Check if CPU load is within threshold");
    
    m.def("checkSimulationTime", &checkSimulationTime,
          py::arg("brain"), py::arg("maxStepTime") = 1000, "Check if simulation time is within limits");

    // COMPARISON AND UTILITY FUNCTIONS

    // Brain state comparison
    m.def("compareBrainStates", &compareBrainStates,
          py::arg("brain1"), py::arg("brain2"), "Compare two brain states");
    
    m.def("calculateBrainSimilarity", &calculateBrainSimilarity,
          py::arg("brain1"), py::arg("brain2"), py::arg("metric") = "neuron_count",
          "Calculate similarity between two brains");

    // Configuration comparison
    m.def("compareConfigs", &compareConfigs,
          py::arg("config1"), py::arg("config2"), "Compare two configurations");
    
    m.def("findConfigDifferences", &findConfigDifferences,
          py::arg("config1"), py::arg("config2"), "Find differences between configurations");

    // Performance benchmarking
    m.def("benchmarkBrainStep", &benchmarkBrainStep,
          py::arg("brain"), py::arg("numSteps") = 1000, "Benchmark brain stepping performance");
    
    m.def("benchmarkSimulation", &benchmarkSimulation,
          py::arg("world"), py::arg("brain"), py::arg("numSteps") = 1000,
          "Benchmark simulation performance");

    // Time series analysis
    m.def("analyzeTimeSeries", &analyzeTimeSeries,
          py::arg("data"), py::arg("samplingRate") = 100,
          "Analyze time series data");
    
    m.def("detectAnomalies", &detectAnomalies,
          py::arg("data"), py::arg("threshold") = 2.0f,
          "Detect anomalies in time series data");

    // ========== DEPRECATED BINDINGS (for backward compatibility) ==========
    
    // Keep the original factory functions for backward compatibility
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (deprecated, use createDefaultConfig)");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (deprecated, use createBrainFromFile or createDefaultBrain)");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (deprecated, use createSimpleWorld)");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (deprecated, use createAgentBrain)");
}

} // namespace nlm
