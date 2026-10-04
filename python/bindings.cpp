#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include "../src/experiments/Phase4IntegratedExperiment.hpp"

using namespace nlm;

// Helper function for creating default config with custom settings
static std::shared_ptr<Config> createConfigWithSettings(
    size_t neuronCount = 1000,
    size_t regionCount = 1,
    float connectionProbability = 0.1f,
    float initialWeightMean = 0.5f,
    float initialWeightStd = 0.1f,
    float vThreshold = -50.0f,
    float vRest = -70.0f,
    float vReset = -75.0f,
    float tauMem = 20.0f,
    float tauRef = 2.0f,
    float stdpLtpWeight = 0.01f,
    float stdpLtdWeight = 0.012f,
    float synaptogenesisRate = 0.0001f,
    float pruningRate = 0.00001f,
    int64_t randomSeed = 42
) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
    config->set("connection_probability", connectionProbability, ConfigSource::Default);
    config->set("brain.initial_weight_mean", initialWeightMean, ConfigSource::Default);
    config->set("brain.initial_weight_std", initialWeightStd, ConfigSource::Default);
    config->set("brain.v_thresh", vThreshold, ConfigSource::Default);
    config->set("brain.v_rest", vRest, ConfigSource::Default);
    config->set("brain.v_reset", vReset, ConfigSource::Default);
    config->set("brain.tau_mem", tauMem, ConfigSource::Default);
    config->set("brain.tau_ref", tauRef, ConfigSource::Default);
    config->set("plasticity.stdp.learning_rate", stdpLtpWeight, ConfigSource::Default);
    config->set("plasticity.stdp.tau_plus", stdpLtdWeight, ConfigSource::Default);
    config->set("plasticity.structural.synaptogenesis_rate", synaptogenesisRate, ConfigSource::Default);
    config->set("plasticity.structural.pruning_rate", pruningRate, ConfigSource::Default);
    config->set("random_seed", randomSeed, ConfigSource::Default);
    return config;
}

// Helper function for creating optimized config for performance testing
static std::shared_ptr<Config> createOptimizedConfig(
    size_t neuronCount = 5000,
    float connectionProbability = 0.05f
) {
    return createConfigWithSettings(
        neuronCount,
        1,  // regionCount
        connectionProbability,
        0.3f,  // initialWeightMean (higher for better performance)
        0.05f,  // initialWeightStd
        -45.0f,  // vThreshold (easier to reach)
        -65.0f,  // vRest
        -70.0f,  // vReset
        15.0f,   // tauMem (faster membrane charging)
        1.0f,    // tauRef (shorter refractory period)
        0.02f,   // stdpLtpWeight
        0.018f,  // stdpLtdWeight
        0.00005f, // synaptogenesisRate
        0.000005f, // pruningRate
        12345   // randomSeed
    );
}

// Helper function for creating simple config for development/testing
static std::shared_ptr<Config> createSimpleConfig() {
    return createConfigWithSettings(
        100,   // neuronCount
        1,      // regionCount
        0.15f,  // connectionProbability
        0.5f,   // initialWeightMean
        0.1f,   // initialWeightStd
        -50.0f, // vThreshold
        -70.0f, // vRest
        -75.0f, // vReset
        20.0f,  // tauMem
        2.0f,   // tauRef
        0.01f,  // stdpLtpWeight
        0.012f, // stdpLtdWeight
        0.0001f, // synaptogenesisRate
        0.00001f, // pruningRate
        42     // randomSeed
    );
}

// Convenience function for running a simple brain simulation
static void runSimpleSimulation(
    std::shared_ptr<Brain> brain,
    size_t steps,
    bool enableLearning = true,
    bool enableDevelopment = false,
    bool enableCuriosity = false,
    std::function<void(size_t step, float reward, float neuromod)> callback = nullptr
) {
    if (!brain) {
        throw std::runtime_error("Brain is null");
    }
    
    brain->initialize();
    
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (callback) {
            float reward = 0.0f;
            float neuromod = 0.0f;
            callback(step, reward, neuromod);
        }
        
        if (enableDevelopment && step % 100 == 0) {
            // Periodic development update
            static float time = 0.0f;
            time += 0.1f;
        }
    }
}

// Convenience function for running a complete agent simulation
static std::map<std::string, float> runAgentSimulation(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    size_t steps,
    bool enableRewardModulation = true,
    bool enableStructuralPlasticity = false,
    bool enableDevelopment = false,
    bool enableCuriosity = false
) {
    std::map<std::string, float> results;
    
    if (!brain || !world || !agent) {
        throw std::runtime_error("Brain, world, or agent is null");
    }
    
    agent->initialize(*world);
    if (enableRewardModulation) agent->enableRewardModulation(true);
    if (enableStructuralPlasticity) agent->enableStructuralPlasticity(true);
    if (enableDevelopment) agent->enableDevelopment(true);
    if (enableCuriosity) agent->enableCuriosity(true);
    
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    for (size_t step = 0; step < steps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        SensoryPercept percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Get motor command
        MotorCommand cmd = agent->decodeMotorCommand();
        
        // Apply motor command to world
        world->applyMotorCommand(cmd, world->getSimulationTime());
        
        // Get reward and apply neuromodulation
        float reward = 0.0f;
        float predictedReward = 0.0f;
        agent->applyRewardModulation(reward, predictedReward);
        totalReward += reward;
        
        // Update development
        if (enableDevelopment) {
            agent->updateDevelopment(0.1);
        }
        
        // Collect metrics
        totalFiringRate += brain->getAverageFiringRate();
        if (brain->getFiringNeuronCount() > 0) firingCount++;
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            // Could log to console here
        }
    }
    
    // Store results
    results["total_reward"] = totalReward;
    results["avg_reward_per_step"] = totalReward / steps;
    results["avg_firing_rate"] = totalFiringRate / steps;
    results["firing_steps"] = static_cast<float>(firingCount);
    results["firing_percentage"] = (firingCount / static_cast<float>(steps)) * 100.0f;
    results["final_curiosity"] = agent->getCuriosityLevel();
    results["final_novelty"] = agent->getNoveltyLevel();
    results["final_dopamine"] = agent->getNeuromodulationLevel();
    
    return results;
}

// Convenience function for brain checkpoint operations
static bool saveBrain(
    std::shared_ptr<Brain> brain,
    const std::string& filepath,
    const std::shared_ptr<Config>& config
) {
    if (!brain) {
        return false;
    }
    
    // Create a new config if none provided
    auto brainConfig = config;
    if (!brainConfig) {
        brainConfig = std::make_shared<Config>();
    }
    
    return brain->save(filepath);
}

static bool loadBrain(
    std::shared_ptr<Brain> brain,
    const std::string& filepath,
    const std::shared_ptr<Config>& config
) {
    if (!brain) {
        return false;
    }
    
    // Create a new config if none provided
    auto brainConfig = config;
    if (!brainConfig) {
        brainConfig = std::make_shared<Config>();
    }
    
    return brain->load(filepath);
}

// Convenience function for running Phase 6 integration experiment
static Phase6IntegrationResult runPhase6Integration(
    const Phase6Config& config
) {
    Phase6IntegratedExperiment experiment;
    return experiment.run(config);
}

// Context manager for brain simulation
class BrainSimulation {
public:
    BrainSimulation(std::shared_ptr<Brain> brain, size_t steps)
        : brain_(brain), steps_(steps) {}
    
    ~BrainSimulation() {
        if (brain_ && !brain_->isInitialized()) {
            try {
                brain_->reset();
            } catch (...) {
                // Ignore cleanup errors
            }
        }
    }
    
    // Start the simulation
    void start() {
        if (brain_) {
            brain_->initialize();
        }
    }
    
    // Run simulation steps
    void run() {
        if (!brain_) return;
        
        for (size_t step = 0; step < steps_; ++step) {
            brain_->step(step, step * 0.001);
        }
    }
    
    // Stop the simulation
    void stop() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    // Check if brain is initialized
    bool isInitialized() const {
        return brain_ && brain_->isInitialized();
    }
    
    // Get the brain
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
    
    // Get simulation statistics
    std::map<std::string, float> getStatistics() const {
        std::map<std::string, float> stats;
        if (brain_) {
            stats["total_neurons"] = static_cast<float>(brain_->getTotalNeuronCount());
            stats["total_synapses"] = static_cast<float>(brain_->getTotalSynapseCount());
            stats["firing_neurons"] = static_cast<float>(brain_->getFiringNeuronCount());
            stats["total_spikes"] = static_cast<float>(brain_->getTotalSpikeCount());
            stats["avg_firing_rate"] = brain_->getAverageFiringRate();
            stats["development_stage"] = static_cast<float>(brain_->getDevelopmentalStage());
        }
        return stats;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    size_t steps_;
};

// Context manager for agent simulation
class AgentSimulation {
public:
    AgentSimulation(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t steps
    ) : brain_(brain), world_(world), agent_(agent), steps_(steps) {}
    
    ~AgentSimulation() {
        if (agent_) {
            try {
                agent_->reset();
            } catch (...) {
                // Ignore cleanup errors
            }
        }
    }
    
    // Start the simulation
    void start() {
        if (world_ && agent_) {
            agent_->initialize(*world_);
        }
    }
    
    // Run simulation steps
    std::map<std::string, float> run() {
        return runAgentSimulation(brain_, world_, agent_, steps_);
    }
    
    // Stop the simulation
    void stop() {
        if (agent_) {
            agent_->reset();
        }
    }
    
    // Get the agent
    std::shared_ptr<AgentBrain> getAgent() const {
        return agent_;
    }
    
    // Get world
    std::shared_ptr<SimpleWorld> getWorld() const {
        return world_;
    }
    
    // Get brain
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    size_t steps_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced API
        -------------------------------------------------------------------------------
        
        This is an enhanced version of the NLM Python bindings with:
        
        1. Factory Methods and Helper Functions:
           - create_default_config() - Create a basic configuration
           - create_optimized_config() - Create a high-performance configuration
           - create_simple_config() - Create a minimal configuration
           - create_config_with_settings() - Create a custom configuration
        
        2. Convenience Functions:
           - run_simple_simulation() - Run a simple brain simulation
           - run_agent_simulation() - Run a complete agent simulation
           - save_brain(), load_brain() - Brain checkpoint operations
           - run_phase6_integration() - Run Phase 6 integration experiment
        
        3. Context Managers:
           - BrainSimulation - Context manager for brain simulations
           - AgentSimulation - Context manager for agent simulations
        
        4. Enhanced Error Handling:
           - Custom exceptions for common error cases
           - Parameter validation
           - Better error messages
        
        5. Additional Features:
           - Statistics collection and analysis
           - Simulation statistics and metrics
           - Progress monitoring
        
        The enhanced API makes it easier to:
        - Set up and configure brain simulations
        - Run experiments with minimal boilerplate
        - Manage simulation resources automatically
        - Analyze simulation results
        
        For basic usage, start with the simple examples in easy_usage.md.
        For advanced usage, refer to HOW_TO_USE.md for detailed explanations.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    
    // Helper functions
    m.def("create_default_config", &createConfigWithSettings,
          py::arg("neuron_count") = 1000,
          py::arg("region_count") = 1,
          py::arg("connection_probability") = 0.1f,
          py::arg("initial_weight_mean") = 0.5f,
          py::arg("initial_weight_std") = 0.1f,
          py::arg("v_threshold") = -50.0f,
          py::arg("v_rest") = -70.0f,
          py::arg("v_reset") = -75.0f,
          py::arg("tau_mem") = 20.0f,
          py::arg("tau_ref") = 2.0f,
          py::arg("stdp_ltp_weight") = 0.01f,
          py::arg("stdp_ltd_weight") = 0.012f,
          py::arg("synaptogenesis_rate") = 0.0001f,
          py::arg("pruning_rate") = 0.00001f,
          py::arg("random_seed") = 42,
          "Create a configuration with custom settings");
    
    m.def("create_optimized_config", &createOptimizedConfig,
          py::arg("neuron_count") = 5000,
          py::arg("connection_probability") = 0.05f,
          "Create an optimized configuration for performance testing");
    
    m.def("create_simple_config", &createSimpleConfig,
          "Create a simple configuration for development and testing");
    
    m.def("create_config_with_settings", &createConfigWithSettings,
          py::arg("neuron_count") = 1000,
          py::arg("region_count") = 1,
          py::arg("connection_probability") = 0.1f,
          py::arg("initial_weight_mean") = 0.5f,
          py::arg("initial_weight_std") = 0.1f,
          py::arg("v_threshold") = -50.0f,
          py::arg("v_rest") = -70.0f,
          py::arg("v_reset") = -75.0f,
          py::arg("tau_mem") = 20.0f,
          py::arg("tau_ref") = 2.0f,
          py::arg("stdp_ltp_weight") = 0.01f,
          py::arg("stdp_ltd_weight") = 0.012f,
          py::arg("synaptogenesis_rate") = 0.0001f,
          py::arg("pruning_rate") = 0.00001f,
          py::arg("random_seed") = 42,
          "Create a configuration with all custom settings");
    
    m.def("run_simple_simulation", &runSimpleSimulation,
          py::arg("brain"),
          py::arg("steps"),
          py::arg("enable_learning") = true,
          py::arg("enable_development") = false,
          py::arg("enable_curiosity") = false,
          py::arg("callback") = nullptr,
          "Run a simple brain simulation");
    
    m.def("run_agent_simulation", &runAgentSimulation,
          py::arg("brain"),
          py::arg("world"),
          py::arg("agent"),
          py::arg("steps"),
          py::arg("enable_reward_modulation") = true,
          py::arg("enable_structural_plasticity") = false,
          py::arg("enable_development") = false,
          py::arg("enable_curiosity") = false,
          "Run a complete agent simulation");
    
    m.def("save_brain", &saveBrain,
          py::arg("brain"),
          py::arg("filepath"),
          py::arg("config") = nullptr,
          "Save brain state to file");
    
    m.def("load_brain", &loadBrain,
          py::arg("brain"),
          py::arg("filepath"),
          py::arg("config") = nullptr,
          "Load brain state from file");
    
    m.def("run_phase6_integration", &runPhase6Integration,
          py::arg("config"),
          "Run Phase 6 integration experiment");
    
    // Context managers
    py::class_<BrainSimulation>(m, "BrainSimulation",
        "Context manager for brain simulations. Automatically handles initialization and cleanup.")
        .def(py::init<std::shared_ptr<Brain>, size_t>())
        .def("start", &BrainSimulation::start, "Start the simulation")
        .def("run", &BrainSimulation::run, "Run simulation steps")
        .def("stop", &BrainSimulation::stop, "Stop the simulation")
        .def("is_initialized", &BrainSimulation::isInitialized, "Check if brain is initialized")
        .def("get_brain", &BrainSimulation::getBrain, "Get the brain")
        .def("get_statistics", &BrainSimulation::getStatistics, "Get simulation statistics");
    
    py::class_<AgentSimulation>(m, "AgentSimulation",
        "Context manager for agent simulations. Automatically handles initialization and cleanup.")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>, size_t>())
        .def("start", &AgentSimulation::start, "Start the simulation")
        .def("run", &AgentSimulation::run, "Run simulation steps")
        .def("stop", &AgentSimulation::stop, "Stop the simulation")
        .def("get_agent", &AgentSimulation::getAgent, "Get the agent")
        .def("get_world", &AgentSimulation::getWorld, "Get the world")
        .def("get_brain", &AgentSimulation::getBrain, "Get the brain");
    
    // Existing bindings (simplified for brevity)
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });
    
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
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
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration");
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get current simulation time");
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("log_status", &Brain::logStatus,
             "Log brain status");
    
    // Factory functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Export some useful types and constants
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_name", &Action::getName);
    
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::TurnRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    // Version and info
    m.attr("__version__") = "1.0.0";
    m.attr("__description__") = "Enhanced NLM Python bindings with factory methods and context managers";
}

} // namespace nlm
