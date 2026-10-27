// Additional Python bindings for improved NLM API convenience
// This file adds enhanced functionality to the pynlm module

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions for enhanced Python API

// Configuration factory functions
std::shared_ptr<Config> createConfigWithDefaults() {
    auto config = std::make_shared<Config>();
    // Set reasonable defaults for easy use
    config->set("brain.neuron_count", static_cast<int64_t>(1000));
    config->set("brain.synapse_density", 0.1f);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("neuromod.dopamine.scale", 1.0f);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    return config;
}

std::shared_ptr<Config> createConfigForLearning() {
    auto config = std::make_shared<Config>();
    // Optimized for learning scenarios
    config->set("brain.neuron_count", static_cast<int64_t>(2000));
    config->set("brain.synapse_density", 0.15f);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.stdp.learning_rate", 0.001f);
    config->set("plasticity.hebbian.enable", true);
    config->set("neuromod.dopamine.scale", 1.5f);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    config->set("neuromod.reward.enable", true);
    return config;
}

std::shared_ptr<Config> createConfigForQuickTest() {
    auto config = std::make_shared<Config>();
    // Minimal configuration for quick testing
    config->set("brain.neuron_count", static_cast<int64_t>(100));
    config->set("brain.synapse_density", 0.05f);
    config->set("plasticity.stdp.enable", false);
    config->set("plasticity.hebbian.enable", false);
    return config;
}

// Enhanced Brain methods with Python-friendly signatures
void brainStepWithTime(Brain& brain, SimulationStep step, Timestamp time) {
    brain.step(step, time);
}

void brainInjectSensoryPattern(Brain& brain, const SensoryInput& input) {
    brain.receiveSensoryInput(input);
}

std::vector<NeuronId> brainGetActiveNeurons(const Brain& brain) {
    std::vector<NeuronId> activeNeurons;
    auto* spikeSystem = brain.getSpikeSystem();
    if (spikeSystem) {
        const auto& spikeHistory = spikeSystem->getSpikeHistory();
        for (const auto& spike : spikeHistory) {
            activeNeurons.push_back(spike.source);
        }
    }
    return activeNeurons;
}

float brainGetLearningRate(const Brain& brain) {
    return brain.getAverageFiringRate() * 0.01f; // Simple heuristic
}

// Enhanced AgentBrain methods with better error handling
void agentProcessPerceptWithValidation(AgentBrain& agent, const SensoryPercept& percept) {
    if (!agent.getBrain()) {
        throw std::runtime_error("Agent brain not initialized");
    }
    agent.processSensoryInput(percept);
}

Action agentGetActionWithDefaults(AgentBrain& agent) {
    auto action = agent.decodeMotorCommand();
    // If action has default values, ensure they're set
    if (action.getType() == ActionType::Custom) {
        // Set some reasonable defaults for custom actions
        std::vector<float> params = action.getParameters();
        if (params.empty()) {
            params = {0.0f, 0.0f, 0.0f}; // X, Y, intensity
            action.setParameters(params);
        }
    }
    return action;
}

// Enhanced world methods
void worldSetRandomSeed(SimpleWorld& world, uint64_t seed) {
    world.setRandomSeed(seed);
    // Also set the internal RNG state for consistency
}

void worldAddResource(SimpleWorld& world, float x, float y, float value) {
    WorldObject obj(x, y, WorldObjectType::Resource, value);
    world.addObject(obj);
}

void worldAddHazard(SimpleWorld& world, float x, float y, float damage) {
    WorldObject obj(x, y, WorldObjectType::Hazard, damage);
    world.addObject(obj);
}

void worldAddWall(SimpleWorld& world, float x, float y, float radius) {
    WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
    world.addObject(obj);
}

// Convenience functions for common scenarios
struct SimpleWorldSetup {
    float width = 20.0f;
    float height = 20.0f;
    size_t visionWidth = 8;
    size_t visionHeight = 8;
    float maxEnergy = 100.0f;
    float energyDecayRate = 0.01f;
};

SimpleWorld createWorldWithSetup(const SimpleWorldSetup& setup) {
    SimpleWorld world;
    world.configure(setup.width, setup.height, setup.visionWidth, setup.visionHeight);
    world.setMaxEnergy(setup.maxEnergy);
    world.setEnergyDecayRate(setup.energyDecayRate);
    return world;
}

// Experiment runner helper
struct AgentSimulationParams {
    uint64_t numSteps = 1000;
    bool enableRewardModulation = true;
    bool enableCuriosity = true;
    bool enableDevelopment = true;
    bool enableStructuralPlasticity = true;
    float timestep = 0.1f;
};

void runAgentSimulation(const AgentSimulationParams& params,
                       Brain& brain,
                       SimpleWorld& world,
                       AgentBrain& agent) {
    // Enable learning systems
    agent.enableRewardModulation(params.enableRewardModulation);
    agent.enableCuriosity(params.enableCuriosity);
    agent.enableDevelopment(params.enableDevelopment);
    agent.enableStructuralPlasticity(params.enableStructuralPlasticity);
    
    // Run simulation
    for (uint64_t step = 0; step < params.numSteps; ++step) {
        world.update(params.timestep);
        
        const SensoryPercept& percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        brain.step(step, step * params.timestep);
        
        Action action = agent.decodeMotorCommand();
        ActionResult result = world.applyMotorCommand(action, world.getSimulationTime());
        
        if (params.enableRewardModulation) {
            agent.applyRewardModulation(result.reward, 0.0f);
        }
        
        if (params.enableDevelopment) {
            agent.updateDevelopment(params.timestep);
        }
    }
}

// Data collection utilities
struct BrainMetrics {
    size_t neuronCount;
    size_t synapseCount;
    size_t firingNeurons;
    size_t totalSpikes;
    float averageFiringRate;
    float excitationInhibitionRatio;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    float developmentalStage;
    uint64_t simulationSteps;
};

BrainMetrics collectBrainMetrics(const Brain& brain, const AgentBrain& agent, uint64_t steps) {
    BrainMetrics metrics;
    metrics.neuronCount = brain.getTotalNeuronCount();
    metrics.synapseCount = brain.getTotalSynapseCount();
    metrics.firingNeurons = brain.getFiringNeuronCount();
    metrics.totalSpikes = brain.getTotalSpikeCount();
    metrics.averageFiringRate = brain.getAverageFiringRate();
    metrics.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
    metrics.curiosityLevel = agent.getCuriosityLevel();
    metrics.noveltyLevel = agent.getNoveltyLevel();
    metrics.predictionError = agent.getPredictionError();
    metrics.developmentalStage = static_cast<float>(brain.getDevelopmentalStage());
    metrics.simulationSteps = steps;
    return metrics;
}

void printMetrics(const BrainMetrics& metrics) {
    std::cout << "=== BRAIN METRICS ===" << std::endl;
    std::cout << "Neurons: " << metrics.neuronCount << std::endl;
    std::cout << "Synapses: " << metrics.synapseCount << std::endl;
    std::cout << "Currently Firing: " << metrics.firingNeurons << std::endl;
    std::cout << "Total Spikes: " << metrics.totalSpikes << std::endl;
    std::cout << "Average Firing Rate: " << metrics.averageFiringRate << " Hz" << std::endl;
    std::cout << "E/I Ratio: " << metrics.excitationInhibitionRatio << std::endl;
    std::cout << "Curiosity Level: " << metrics.curiosityLevel << std::endl;
    std::cout << "Novelty Level: " << metrics.noveltyLevel << std::endl;
    std::cout << "Prediction Error: " << metrics.predictionError << std::endl;
    std::cout << "Developmental Stage: " << metrics.developmentalStage << std::endl;
    std::cout << "Simulation Steps: " << metrics.simulationSteps << std::endl;
}

// Configuration file utilities
bool saveConfigToJSON(const Config& config, const std::string& filepath) {
    // Note: Config.saveToFile() uses JSON format
    return config.saveToFile(filepath);
}

bool loadConfigFromJSON(Config& config, const std::string& filepath) {
    return config.loadFromFile(filepath);
}

// Validation functions
bool validateBrainConfig(const Config& config) {
    bool valid = true;
    
    // Check required parameters
    if (!config.has("brain.neuron_count")) {
        std::cerr << "Warning: brain.neuron_count not set" << std::endl;
        valid = false;
    }
    
    if (!config.has("brain.synapse_density")) {
        std::cerr << "Warning: brain.synapse_density not set" << std::endl;
        valid = false;
    }
    
    // Validate ranges
    auto neuronCountOpt = config.get<int64_t>("brain.neuron_count");
    if (neuronCountOpt && *neuronCountOpt <= 0) {
        std::cerr << "Error: brain.neuron_count must be positive" << std::endl;
        valid = false;
    }
    
    auto densityOpt = config.get<double>("brain.synapse_density");
    if (densityOpt && (*densityOpt < 0.0 || *densityOpt > 1.0)) {
        std::cerr << "Error: brain.synapse_density must be between 0 and 1" << std::endl;
        valid = false;
    }
    
    return valid;
}

} // namespace nlm

// Enhanced module definitions
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides enhanced convenience functions and improved API
        for easier and more powerful NLM usage.
    )pbdoc";

    // Import the main pynlm module
    py::module_ pynlm = py::module_::import("pynlm");

    // Add enhanced classes and functions
    
    // Enhanced Config class
    py::class_<Config>(m, "EnhancedConfig", pynlm.attr("Config"))
        .def(py::init<>())
        .def("createForLearning", &createConfigForLearning, "Create config optimized for learning")
        .def("createForQuickTest", &createConfigForQuickTest, "Create config for quick testing")
        .def("validate", &validateBrainConfig, "Validate configuration settings")
        .def("saveJSON", &saveConfigToJSON, py::arg("filepath"), "Save config to JSON file")
        .def("loadJSON", &loadConfigFromJSON, py::arg("filepath"), "Load config from JSON file");

    // Enhanced Brain class
    py::class_<Brain>(m, "EnhancedBrain", pynlm.attr("Brain"))
        .def("stepWithTime", &brainStepWithTime, py::arg("step"), py::arg("time"),
             "Perform brain step with timestamp")
        .def("injectSensory", &brainInjectSensoryPattern, py::arg("input"),
             "Inject sensory input into brain")
        .def("getActiveNeurons", &brainGetActiveNeurons, "Get list of recently active neurons")
        .def("getLearningRate", &brainGetLearningRate, "Get estimated learning rate");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "EnhancedAgentBrain", pynlm.attr("AgentBrain"))
        .def("processPercept", &agentProcessPerceptWithValidation, py::arg("percept"),
             "Process sensory percept with validation")
        .def("getAction", &agentGetActionWithDefaults, "Get action with default values");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "EnhancedSimpleWorld", pynlm.attr("SimpleWorld"))
        .def("setRandomSeed", &worldSetRandomSeed, py::arg("seed"), "Set random seed for reproducibility")
        .def("addResource", &worldAddResource, py::arg("x"), py::arg("y"), py::arg("value"), "Add resource object")
        .def("addHazard", &worldAddHazard, py::arg("x"), py::arg("y"), py::arg("damage"), "Add hazard object")
        .def("addWall", &worldAddWall, py::arg("x"), py::arg("y"), py::arg("radius"), "Add wall object");

    // Factory functions
    m.def("createWorldWithSetup", &createWorldWithSetup, py::arg("setup"),
          "Create world with predefined setup parameters");
    
    m.def("runAgentSimulation", &runAgentSimulation, py::arg("params"), py::arg("brain"),
          py::arg("world"), py::arg("agent"), "Run complete agent simulation with parameters");

    // Data collection utilities
    py::class_<BrainMetrics>(m, "BrainMetrics")
        .def(py::init<>())
        .def_readwrite("neuronCount", &BrainMetrics::neuronCount)
        .def_readwrite("synapseCount", &BrainMetrics::synapseCount)
        .def_readwrite("firingNeurons", &BrainMetrics::firingNeurons)
        .def_readwrite("totalSpikes", &BrainMetrics::totalSpikes)
        .def_readwrite("averageFiringRate", &BrainMetrics::averageFiringRate)
        .def_readwrite("excitationInhibitionRatio", &BrainMetrics::excitationInhibitionRatio)
        .def_readwrite("curiosityLevel", &BrainMetrics::curiosityLevel)
        .def_readwrite("noveltyLevel", &BrainMetrics::noveltyLevel)
        .def_readwrite("predictionError", &BrainMetrics::predictionError)
        .def_readwrite("developmentalStage", &BrainMetrics::developmentalStage)
        .def_readwrite("simulationSteps", &BrainMetrics::simulationSteps);

    m.def("collectBrainMetrics", &collectBrainMetrics, py::arg("brain"), py::arg("agent"), py::arg("steps"),
          "Collect comprehensive metrics from brain simulation");
    
    m.def("printMetrics", &printMetrics, py::arg("metrics"), "Print brain metrics to console");

    // Configuration setup
    m.def("createConfigForLearning", &createConfigForLearning, "Create config optimized for learning");
    m.def("createConfigForQuickTest", &createConfigForQuickTest, "Create config for quick testing");

    // Utility functions
    m.def("validateBrainConfig", &validateBrainConfig, py::arg("config"), "Validate brain configuration");

    // Exception types
    py::register_exception<std::runtime_error>(m, "NLMError", pynlm.attr("RuntimeError"));

    // Version info
    m.attr("__version__") = "enhanced-0.1.0";
    
}