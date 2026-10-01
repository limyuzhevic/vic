#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This enhanced Python API provides convenience methods, better error handling,
        and advanced features for easier neural network simulation.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    
    // ========== Enhanced Python API Utilities ==========
    
    // Configuration utility functions
    m.def("create_advanced_config", []() {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults for Python users
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return config;
    }, R"pbdoc(
        Create an advanced configuration with sensible defaults.
        
        Returns:
            Config: A configured NLM configuration object
        )pbdoc");
    
    m.def("create_brain_from_config", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a brain from an existing configuration.
        
        Args:
            config: Configuration object
            
        Returns:
            Brain: A new brain instance
        )pbdoc");
    
    // World configuration helper
    m.def("configure_world", [](std::shared_ptr<SimpleWorld> world, 
                              size_t width, size_t height,
                              size_t visionWidth, size_t visionHeight) {
        world->configure(static_cast<float>(width), static_cast<float>(height),
                        visionWidth, visionHeight);
    }, py::arg("world"), py::arg("width"), py::arg("height"),
        py::arg("visionWidth"), py::arg("visionHeight"), R"pbdoc(
        Configure a simple world with dimensions and vision parameters.
        
        Args:
            world: World object to configure
            width: World width in cells
            height: World height in cells  
            visionWidth: Agent's vision width
            visionHeight: Agent's vision height
        )pbdoc");
    
    // Agent setup helper
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"), R"pbdoc(
        Create and initialize an agent brain.
        
        Args:
            brain: Brain object to control
            world: World environment
            
        Returns:
            AgentBrain: Configured agent interface
        )pbdoc");
    
    // Enhanced simulation utilities
    m.def("run_simulation_step", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   double timestep = 0.1,
                                   bool enableLogging = false) {
        if (enableLogging) {
            NLM_LOG_INFO("Starting simulation step");
        }
        
        // World update
        world->update(timestep);
        
        // Get sensory input
        const SensoryPercept& percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain processing
        brain->step(static_cast<SimulationStep>(world->getSimulationTime()));
        
        // Get action
        MotorCommand action = agent->decodeMotorCommand();
        
        // Apply action
        ActionResult result = world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation if enabled
        if (agent->isRewardModulationEnabled()) {
            float reward = result.reward;
            float predictedReward = agent->getNeuromodulationLevel();
            agent->applyRewardModulation(reward, predictedReward);
        }
        
        // Update development if enabled
        if (agent->isDevelopmentEnabled()) {
            agent->updateDevelopment(timestep);
        }
        
        if (enableLogging) {
            NLM_LOG_INFO("Simulation step completed: " + std::to_string(static_cast<int>(world->getSimulationTime())) + "s");
        }
        
        return result;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
        py::arg("timestep") = 0.1, py::arg("enableLogging") = false, R"pbdoc(
        Run a single simulation step with all components.
        
        Args:
            brain: The neural brain
            world: The environment
            agent: The agent interface
            timestep: Time step for simulation
            enableLogging: Enable logging output
            
        Returns:
            ActionResult: Result of applying the action
        )pbdoc");
    
    // Batch simulation utility
    m.def("run_simulation_loop", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   size_t numSteps,
                                   double timestep = 0.1,
                                   bool enableLogging = false,
                                   std::function<bool(size_t)> shouldStop = nullptr) {
        size_t stepsCompleted = 0;
        bool shouldContinue = !shouldStop;
        
        while (stepsCompleted < numSteps && shouldContinue) {
            auto result = run_simulation_step(brain, world, agent, timestep, enableLogging);
            
            if (shouldStop) {
                shouldContinue = !shouldStop(stepsCompleted);
            }
            
            stepsCompleted++;
        }
        
        if (enableLogging) {
            NLM_LOG_INFO("Simulation completed: " + std::to_string(stepsCompleted) + " steps");
        }
        
        return stepsCompleted;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("numSteps"), py::arg("timestep") = 0.1,
        py::arg("enableLogging") = false, py::arg("shouldStop") = nullptr, R"pbdoc(
        Run a complete simulation loop.
        
        Args:
            brain: The neural brain
            world: The environment
            agent: The agent interface
            numSteps: Number of steps to run
            timestep: Time step for simulation
            enableLogging: Enable logging output
            shouldStop: Optional callback to stop early (returns True to stop)
            
        Returns:
            int: Number of steps completed
        )pbdoc");
    
    // Enhanced brain utilities
    m.def("create_test_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Enable advanced features
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        return std::make_tuple(brain, agent);
    }, R"pbdoc(
        Create a test brain with all advanced features enabled.
        
        Returns:
            tuple: (brain, agent) tuple
        )pbdoc");
    
    // Performance monitoring
    m.def("get_brain_stats", [](std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, double> stats;
        
        stats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());
        stats["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        
        return stats;
    }, R"pbdoc(
        Get comprehensive brain statistics.
        
        Args:
            brain: The brain to analyze
            
        Returns:
            dict: Dictionary of brain statistics
        )pbdoc");
    
    // Enhanced logging
    m.def("set_log_level", [](std::string level) {
        LogLevel logLevel;
        if (level == "debug") logLevel = LogLevel::Debug;
        else if (level == "info") logLevel = LogLevel::Info;
        else if (level == "warning") logLevel = LogLevel::Warning;
        else if (level == "error") logLevel = LogLevel::Error;
        else if (level == "critical") logLevel = LogLevel::Critical;
        else {
            throw py::value_error("Invalid log level: " + level);
        }
        
        Logger::getGlobal().setLevel(logLevel);
    }, py::arg("level"), R"pbdoc(
        Set the global logging level.
        
        Args:
            level: Logging level (debug, info, warning, error, critical)
        )pbdoc");
    
    // Configuration file utilities
    m.def("save_config_to_json", [](std::shared_ptr<Config> config, std::string filepath) {
        bool success = config->saveToFile(filepath);
        if (!success) {
            throw py::runtime_error("Failed to save configuration to " + filepath);
        }
        return true;
    }, py::arg("config"), py::arg("filepath"), R"pbdoc(
        Save configuration to JSON file.
        
        Args:
            config: Configuration to save
            filepath: Output file path
            
        Returns:
            bool: True on success
        )pbdoc");
    
    m.def("load_config_from_json", [](std::string filepath) {
        auto config = std::make_shared<Config>();
        bool success = config->loadFromFile(filepath);
        if (!success) {
            throw py::runtime_error("Failed to load configuration from " + filepath);
        }
        return config;
    }, py::arg("filepath"), R"pbdoc(
        Load configuration from JSON file.
        
        Args:
            filepath: Input file path
            
        Returns:
            Config: Loaded configuration
        )pbdoc");
    
    // Example simulation patterns
    m.def("simple_prediction_pattern", []() {
        py::module_ sys = py::module_::import("sys");
        py::object example = py::str(
            """# Example: Simple Prediction Pattern\n\n"
            "import pynlm\n\n"
            "# Create advanced brain\nbrain = pynlm.create_advanced_config()\nbrain = pynlm.create_brain_from_config(brain)\nbrain.initialize()\n\n"
            "# Create agent and world\nagent = pynlm.create_agent_brain(brain)\nworld = pynlm.createSimpleWorld()\npynlm.configure_world(world, 20, 20, 8, 8)\n\n"
            "# Enable learning\nagent.enableRewardModulation(True)\nagent.enableCuriosity(True)\n\n"
            "# Run simulation\nfor step in range(100):\n    result = pynlm.run_simulation_step(brain, world, agent, 0.1, True)\n    if step % 20 == 0:\n        stats = pynlm.get_brain_stats(brain)\n        print(f\"Step {step}: Firing rate {stats['average_firing_rate']:.3f}\")\n\n"
            "print(\"Simulation complete!\")\n"
        );
        sys.stdout.write(example.cast<std::string>());
    }, R"pbdoc(
        Print an example simulation pattern to stdout.
    )pbdoc");
    
    // Brain and Config class definitions (continuing from earlier code)
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Register all the original classes (continuing from the full file)...
    
    // Add enhanced Python features
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
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Add Python-specific convenience methods
    py::class_<Config>(m, "Config", py::dynamic_attr<>())
        .def("get_int", &Config::getOr<int>, py::arg("key"), py::arg("default_value"),
             "Get integer value with default")
        .def("get_float", &Config::getOr<double>, py::arg("key"), py::arg("default_value"),
             "Get float value with default")
        .def("get_string", &Config::getOr<std::string>, py::arg("key"), py::arg("default_value"),
             "Get string value with default")
        .def("set_int", &Config::set<int>, py::arg("key"), py::arg("value"),
             "Set integer value")
        .def("set_float", &Config::set<double>, py::arg("key"), py::arg("value"),
             "Set float value")
        .def("set_string", &Config::set<std::string>, py::arg("key"), py::arg("value"),
             "Set string value");
    
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
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration");

    // Simplified Sensor and Action classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // Continue with the rest of the class definitions...
    // (For brevity, showing key enhanced classes)

    // SimpleWorld with enhanced Python API
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight);

    // AgentBrain with enhanced Python API
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity");

    // Add convenience functions for common patterns
    m.def("run_simple_episode", [](std::shared_ptr<Brain> brain,
                                 std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent,
                                 size_t numSteps = 100,
                                 double timestep = 0.1) {
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(timestep);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            agent->decodeMotorCommand(); // The agent processes its own percept internally
            // Note: In the real implementation, the agent's decodeMotorCommand would apply the action
        }
        return brain->getAverageFiringRate();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("numSteps") = 100, py::arg("timestep") = 0.1,
        "Run a simple episode with the given components");

    m.def("create_training_scenario", []() {
        // Create a standard training scenario for demonstration
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(300), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50.0f, 50.0f, 16, 16);
        world->setAgentStart(25.0f, 25.0f);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a complete training scenario with all systems enabled");

    // Add type aliases for Python convenience
    py::class_<std::tuple<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>>>(m, "BrainAgentPair");
    
    // Add error types
    py::class_<std::runtime_error>(m, "RuntimeError");
    py::class_<std::invalid_argument>(m, "ValueError");

} // namespace nlm
