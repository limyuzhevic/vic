#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>

// Error handling utilities
namespace py = pybind11;
namespace nlm {

// Forward declarations for error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain Error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

// Enhanced factory functions
py::object createResearchConfig(const std::string& experimentType = "standard",
                               size_t neuronCount = 1000,
                               bool enableLearning = true,
                               bool enableMemory = true) {
    // Default config
    auto config = std::make_shared<Config>();
    
    config->set("neuron_count", static_cast<int64_t>(neuronCount));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.1f);
    config->set("simulation_timestep", 0.001);
    
    // Experiment type specific settings
    if (experimentType == "social") {
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.08f);
        config->set("social_coefficient", 0.3f);
        config->set("empathy_rate", 0.05f);
        config->set("imitation_probability", 0.4f);
    } else if (experimentType == "curiosity") {
        config->set("curiosity_threshold", 0.5f);
        config->set("exploration_bonus", 0.3f);
        config->set("intrinsic_reward_scale", 0.2f);
    } else if (experimentType == "development") {
        config->set("plasticity_decay_rate", 0.95f);
        config->set("critical_period_multiplier", 1.5f);
        config->set("maturation_threshold", 100);
    }
    
    // Plasticity settings
    config->set("stdp_ltp_weight", 0.02f);
    config->set("stdp_ltd_weight", 0.018f);
    config->set("synaptogenesis_rate", enableLearning ? 0.0001f : 0.0f);
    config->set("pruning_rate", enableLearning ? 0.00001f : 0.0f);
    
    // Enable features
    config->set("enable_stdp", true);
    config->set("enable_hebbian", true);
    config->set("enable_structural_plasticity", enableLearning);
    config->set("enable_development", enableLearning);
    config->set("enable_reward_modulation", enableLearning);
    config->set("enable_curiosity", true);
    config->set("enable_novelty", true);
    config->set("enable_prediction", true);
    config->set("enable_working_memory", enableMemory);
    config->set("enable_episodic_memory", enableMemory);
    config->set("enable_associative_memory", enableMemory);
    
    return py::cast(config);
}

py::object createBrainWithConfig(const py::object& configObj) {
    try {
        auto config = std::dynamic_pointer_cast<Config>(configObj.cast<std::shared_ptr<Config>>());
        if (!config) {
            throw ConfigurationError("Invalid configuration object");
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainError("Failed to initialize brain");
        }
        
        return py::cast(brain);
    } catch (const py::cast_error& e) {
        throw ConfigurationError("Invalid configuration object passed to createBrain");
    }
}

py::object createAdvancedExperimentSetup(size_t worldWidth = 16,
                                         size_t worldHeight = 16,
                                         size_t visionWidth = 8,
                                         size_t visionHeight = 8,
                                         size_t numAgents = 1) {
    auto config = createResearchConfig("standard", 1000, true, true);
    
    auto brain = std::make_shared<Brain>(config.cast<std::shared_ptr<Config>>());
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->initialize(worldWidth, worldHeight, visionWidth, visionHeight);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    
    // Enable advanced features
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    auto setup = py::dict(
        "config" = config,
        "brain" = brain,
        "world" = world,
        "agent" = agent,
        "components" = py::dict(
            "brain_count" = 1,
            "world_count" = 1,
            "agent_count" = numAgents,
            "total_neurons" = brain->getTotalNeuronCount()
        )
    );
    
    return setup;
}

py::dict analyzeBrainState(const py::object& brainObj, size_t maxNeurons = 10) {
    auto brain = brainObj.cast<std::shared_ptr<Brain>>();
    
    py::dict analysis;
    
    // Basic neuron stats
    analysis["neuron_stats"] = py::dict(
        "total" = brain->getTotalNeuronCount(),
        "active" = brain->getActiveNeuronCount(),
        "firing" = brain->getFiringNeuronCount(),
        "excitatory_ratio" = brain->getExcitatoryInhibitionRatio(),
        "average_firing_rate" = brain->getAverageFiringRate(),
        "total_spikes" = brain->getTotalSpikeCount()
    );
    
    // Memory stats
    py::dict memoryStats;
    if (brain->getWorkingMemory()) {
        memoryStats["working_memory_traces"] = brain->getWorkingMemory()->getActiveTraces();
    }
    if (brain->getEpisodicMemory()) {
        memoryStats["episodic_episodes"] = brain->getEpisodicMemory()->getEpisodeCount();
    }
    analysis["memory_stats"] = memoryStats;
    
    // Neuromodulation stats
    py::dict neuromodStats;
    try {
        neuromodStats["dopamine_level"] = brain->getNeuromodulationLevel();
    } catch (...) {
        neuromodStats["dopamine_level"] = 0.0f;
    }
    
    try {
        neuromodStats["curiosity_level"] = brain->getCuriosityLevel();
    } catch (...) {
        neuromodStats["curiosity_level"] = 0.0f;
    }
    
    try {
        neuromodStats["novelty_level"] = brain->getNoveltyLevel();
    } catch (...) {
        neuromodStats["novelty_level"] = 0.0f;
    }
    
    try {
        neuromodStats["prediction_error"] = brain->getPredictionError();
    } catch (...) {
        neuromodStats["prediction_error"] = 0.0f;
    }
    
    analysis["neuromodulation"] = neuromodStats;
    
    // Developmental stats
    py::dict devStats;
    devStats["stage"] = brain->getDevelopmentalStage();
    try {
        devStats["plasticity_rate"] = brain->getPlasticityRate();
    } catch (...) {
        devStats["plasticity_rate"] = 1.0f;
    }
    analysis["developmental"] = devStats;
    
    return analysis;
}

// Python function wrappers
py::object runLearningSimulation(py::object setup,
                                 size_t steps = 1000,
                                 bool enableCheckpointing = false,
                                 const std::string& checkpointPath = "./nlm_checkpoint.bin") {
    auto brain = setup["brain"].cast<std::shared_ptr<Brain>>();
    auto world = setup["world"].cast<std::shared_ptr<SimpleWorld>>();
    auto agent = setup["agent"].cast<std::shared_ptr<AgentBrain>>();
    
    py::dict results;
    results["steps_completed"] = 0;
    results["total_reward"] = 0.0f;
    results["firing_rates"] = std::vector<float>();
    results["brain_states"] = std::vector<py::dict>();
    results["actions_taken"] = 0;
    results["learning_events"] = 0;
    results["avg_firing_rate"] = 0.0f;
    results["memory_growth"] = 0.0f;
    
    for (size_t step = 0; step < steps; ++step) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        
        agent->processSensoryInput(percept);
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Collect metrics
        results["firing_rates"].cast<std::vector<float>>().push_back(
            brain->getAverageFiringRate());
        
        // Get detailed brain state periodically
        if (step % 100 == 0) {
            results["brain_states"].cast<std::vector<py::dict>>().push_back(
                analyzeBrainState(brain, 5));
        }
        
        results["actions_taken"] = results["actions_taken"].cast<int>() + 1;
        
        // Check for learning events
        try {
            if (agent->getPredictionError() > 0.1) {
                results["learning_events"] = results["learning_events"].cast<int>() + 1;
            }
        } catch (...) {}
        
        // Apply reward modulation (using current reward estimate)
        agent->applyRewardModulation(0.0f, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Save checkpoint periodically
        if (enableCheckpointing && step % 500 == 0) {
            brain->save(checkpointPath);
        }
    }
    
    // Calculate averages
    auto& firingRates = results["firing_rates"].cast<std::vector<float>>();
    if (!firingRates.empty()) {
        float sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f);
        results["avg_firing_rate"] = sum / firingRates.size();
    }
    
    results["steps_completed"] = steps;
    results["final_state"] = analyzeBrainState(brain);
    
    return results;
}

py::dict benchmarkPerformance(py::object setup,
                              const std::vector<double>& durations = std::vector<double>{0.1, 0.5, 1.0, 2.0}) {
    auto brain = setup["brain"].cast<std::shared_ptr<Brain>>();
    auto world = setup["world"].cast<std::shared_ptr<SimpleWorld>>();
    auto agent = setup["agent"].cast<std::shared_ptr<AgentBrain>>();
    
    py::dict results;
    results["benchmarks"] = std::vector<py::dict>();
    results["performance_scaling"] = py::dict();
    results["bottleneck_analysis"] = py::dict();
    
    for (double duration : durations) {
        size_t steps = static_cast<size_t>(duration / 0.1);  // Assuming 0.1s timestep
        
        auto startWall = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->applyRewardModulation(0.0f, 0.0f);
            agent->updateDevelopment(0.1);
        }
        
        auto endWall = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(endWall - startWall).count();
        
        size_t stepsPerSecond = (elapsed > 0) ? static_cast<size_t>(steps / elapsed) : 0;
        
        py::dict benchmark;
        benchmark["duration_sec"] = duration;
        benchmark["steps_completed"] = steps;
        benchmark["elapsed_time"] = elapsed;
        benchmark["steps_per_second"] = stepsPerSecond;
        benchmark["firing_rate_avg"] = brain->getAverageFiringRate();
        benchmark["neurons_active"] = brain->getFiringNeuronCount();
        benchmark["total_spikes"] = brain->getTotalSpikeCount();
        
        results["benchmarks"].cast<std::vector<py::dict>>().push_back(benchmark);
    }
    
    // Analyze performance scaling
    if (results["benchmarks"].cast<std::vector<py::dict>>().size() > 1) {
        auto& benchmarks = results["benchmarks"].cast<std::vector<py::dict>>();
        auto& first = benchmarks[0];
        auto& last = benchmarks.back();
        
        double timeScaling = (first["steps_per_second"].cast<size_t>() > 0) ?
            (last["steps_per_second"].cast<size_t>() / first["steps_per_second"].cast<size_t>()) : 0.0;
        
        results["performance_scaling"]["time_scaling"] = timeScaling;
        results["performance_scaling"]["memory_scaling"] = 
            (last["neurons_active"].cast<size_t>() / first["neurons_active"].cast<size_t>());
        results["performance_scaling"]["efficiency_scaling"] = 
            (last["firing_rate_avg"].cast<float>() / first["firing_rate_avg"].cast<float>());
    }
    
    // Identify bottlenecks
    double maxTimePerStep = 0.0;
    auto& benchmarks = results["benchmarks"].cast<std::vector<py::dict>>();
    for (auto& bench : benchmarks) {
        if (bench["steps_completed"].cast<size_t>() > 0) {
            double timePerStep = bench["elapsed_time"].cast<double>() / 
                                bench["steps_completed"].cast<size_t>();
            maxTimePerStep = std::max(maxTimePerStep, timePerStep);
        }
    }
    
    results["bottleneck_analysis"]["max_time_per_step_ms"] = maxTimePerStep * 1000;
    results["bottleneck_analysis"]["recommended_max_steps"] = 
        (maxTimePerStep > 0) ? static_cast<size_t>(1.0 / maxTimePerStep) : 0;
    results["bottleneck_analysis"]["performance_rating"] = 
        (maxTimePerStep < 0.001) ? "GOOD" : "NEEDS_OPTIMIZATION";
    
    return results;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ----------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Improved error handling with custom exception types
        - Advanced factory functions for research setups
        - Brain state analysis utilities
        - Simulation management tools
        - Performance benchmarking capabilities
        
        Key improvements over basic bindings:
        * Better error messages and exception types
        * Research-oriented configuration presets
        * Comprehensive brain state analysis
        * Advanced experiment setup utilities
        * Performance monitoring and benchmarking
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Original basic bindings (kept for backward compatibility)
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

    // [Additional basic class bindings would continue here for backward compatibility]
    
    // Enhanced factory functions
    m.def("create_research_config", &createResearchConfig,
          py::arg("experimentType") = "standard",
          py::arg("neuronCount") = 1000,
          py::arg("enableLearning") = true,
          py::arg("enableMemory") = true,
          "Create a research configuration for different experiment types.");

    m.def("create_brain_with_config", &createBrainWithConfig,
          py::arg("config"),
          "Create a brain with the specified configuration.");

    m.def("create_advanced_experiment_setup", &createAdvancedExperimentSetup,
          py::arg("worldWidth") = 16,
          py::arg("worldHeight") = 16,
          py::arg("visionWidth") = 8,
          py::arg("visionHeight") = 8,
          py::arg("numAgents") = 1,
          "Create an advanced experiment setup with world, brain, and agent.");

    m.def("analyze_brain_state", &analyzeBrainState,
          py::arg("brain"),
          py::arg("maxNeurons") = 10,
          "Analyze and provide detailed statistics about brain state.");

    m.def("run_learning_simulation", &runLearningSimulation,
          py::arg("setup"),
          py::arg("steps") = 1000,
          py::arg("enableCheckpointing") = false,
          py::arg("checkpointPath") = "./nlm_checkpoint.bin",
          "Run a complete learning simulation with the given setup.");

    m.def("benchmark_performance", &benchmarkPerformance,
          py::arg("setup"),
          py::arg("durations") = std::vector<double>{0.1, 0.5, 1.0, 2.0},
          "Benchmark performance of NLM components.");

    // Configuration class methods
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
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("set_value", [](Config& self, const std::string& key, const py::object& value) {
            // Handle different types
            if (py::isinstance<py::int_(config)>(value)) {
                self.set(key, value.cast<int64_t>(), ConfigSource::User);
            } else if (py::isinstance<py::float>(value)) {
                self.set(key, value.cast<float>(), ConfigSource::User);
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>(), ConfigSource::User);
            } else if (py::isinstance<py::bool>(value)) {
                self.set(key, value.cast<bool>(), ConfigSource::User);
            } else {
                throw ConfigurationError("Unsupported type for configuration value");
            }
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (supports int, float, string, bool)");

    // Brain class methods (enhanced)
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
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
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
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitatoryInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status");

    // AgentBrain class methods (enhanced)
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Enable/Disable methods
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled);

    // World and other core classes (minimal bindings for now)
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight);

    // Factory functions (original)
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->initialize();
        return world;
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Special values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}
