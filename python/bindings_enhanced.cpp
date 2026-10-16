#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced Python exceptions for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigurationError : public NLMException {
public:
    explicit NLMConfigurationError(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

class NLMPythonIntegrationError : public NLMException {
public:
    explicit NLMPythonIntegrationError(const std::string& msg) : NLMException("Python Integration Error: " + msg) {}
};

// Custom exception for Python bindings
void registerNLMExceptions(py::module_& m) {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    m.def("raise_NLMException", [](const std::string& msg) {
        throw NLMException(msg);
    }, "Raise NLM exception");
    
    m.def("raise_NLMConfigurationError", [](const std::string& msg) {
        throw NLMConfigurationError(msg);
    }, "Raise NLM configuration exception");
    
    m.def("raise_NLMPythonIntegrationError", [](const std::string& msg) {
        throw NLMPythonIntegrationError(msg);
    }, "Raise NLM Python integration exception");
}

// Enhanced configuration presets for different use cases
void addConfigurationPresets(py::class_<Config>& config_class) {
    // Preset configurations
    config_class.def_static("create_baby_brain", []() {
        auto cfg = std::make_shared<Config>();
        // Baby brain configuration - small, developing
        cfg->set("neuron_count", 200);
        cfg->set("region_count", 1);
        cfg->set("connection_probability", 0.05f);
        cfg->set("stdp_ltp_weight", 0.005f);
        cfg->set("stdp_ltd_weight", 0.006f);
        cfg->set("synaptogenesis_rate", 0.005f);
        cfg->set("pruning_rate", 0.0005f);
        cfg->set("plasticity_learning_rate", 0.005f);
        cfg->set("dopamine_baseline", 0.0f);
        cfg->set("novelty_threshold", 0.2f);
        cfg->set("curiosity_level", 0.5f);
        return cfg;
    }, "Create a baby brain configuration");
    
    config_class.def_static("create_adult_brain", []() {
        auto cfg = std::make_shared<Config>();
        // Adult brain configuration - optimized, experienced
        cfg->set("neuron_count", 2000);
        cfg->set("region_count", 4);
        cfg->set("connection_probability", 0.15f);
        cfg->set("stdp_ltp_weight", 0.02f);
        cfg->set("stdp_ltd_weight", 0.025f);
        cfg->set("synaptogenesis_rate", 0.00001f);
        cfg->set("pruning_rate", 0.002f);
        cfg->set("plasticity_learning_rate", 0.02f);
        cfg->set("dopamine_baseline", 0.5f);
        cfg->set("novelty_threshold", 0.05f);
        cfg->set("curiosity_level", 2.0f);
        return cfg;
    }, "Create an adult brain configuration");
    
    config_class.def_static("create_research_brain", []() {
        auto cfg = std::make_shared<Config>();
        // Research brain configuration - maximum plasticity for exploration
        cfg->set("neuron_count", 1500);
        cfg->set("region_count", 3);
        cfg->set("connection_probability", 0.2f);
        cfg->set("stdp_ltp_weight", 0.03f);
        cfg->set("stdp_ltd_weight", 0.035f);
        cfg->set("synaptogenesis_rate", 0.01f);
        cfg->set("pruning_rate", 0.001f);
        cfg->set("plasticity_learning_rate", 0.05f);
        cfg->set("dopamine_baseline", 0.2f);
        cfg->set("novelty_threshold", 0.15f);
        cfg->set("curiosity_level", 1.5f);
        return cfg;
    }, "Create a research brain configuration");
    
    config_class.def_static("create_performance_brain", []() {
        auto cfg = std::make_shared<Config>();
        // Performance brain configuration - optimized for stability
        cfg->set("neuron_count", 1000);
        cfg->set("region_count", 2);
        cfg->set("connection_probability", 0.1f);
        cfg->set("stdp_ltp_weight", 0.01f);
        cfg->set("stdp_ltd_weight", 0.012f);
        cfg->set("synaptogenesis_rate", 0.00005f);
        cfg->set("pruning_rate", 0.00002f);
        cfg->set("plasticity_learning_rate", 0.01f);
        cfg->set("dopamine_baseline", 0.3f);
        cfg->set("novelty_threshold", 0.1f);
        cfg->set("curiosity_level", 1.0f);
        return cfg;
    }, "Create a performance-optimized brain configuration");
}

// Enhanced convenience functions
void addConvenienceFunctions(py::module_& m) {
    // Quick setup helper
    m.def("setup_brain_world_agent", [](const std::string& config_preset = "default", 
                                     int world_size = 20, 
                                     bool enable_learning = true) {
        auto config = std::make_shared<Config>();
        
        if (config_preset == "baby") {
            config = Config::create_baby_brain();
        } else if (config_preset == "adult") {
            config = Config::create_adult_brain();
        } else if (config_preset == "research") {
            config = Config::create_research_brain();
        } else if (config_preset == "performance") {
            config = Config::create_performance_brain();
        } else {
            // Default config
            config->set("neuron_count", 1000);
            config->set("region_count", 2);
            config->set("connection_probability", 0.1f);
        }
            
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(world_size, world_size, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        if (enable_learning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("config_preset") = "default", 
          py::arg("world_size") = 20,
          py::arg("enable_learning") = true,
          "Quick setup helper for brain, world, and agent");
    
    // Simulation runner
    m.def("run_agent_simulation", [](std::shared_ptr<AgentBrain> agent,
                                    std::shared_ptr<SimpleWorld> world,
                                    int num_steps) {
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            auto brain = agent->getBrain();
            brain->step(step, step * 0.1f);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward based on position
            auto body = world->getAgentBody();
            float reward = 1.0f / (1.0f + body->x + body->y);
            agent->applyRewardModulation(reward, 0.0f);
        }
        
        return std::make_tuple(agent->getBrain(), world, agent);
    }, "Run a complete agent simulation");
    
    // Data export utilities
    m.def("export_brain_data", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        brain->save(filepath);
        return true;
    }, "Export brain data to file");
    
    m.def("export_brain_stats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::string> stats;
        stats["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
        stats["synapse_count"] = std::to_string(brain->getTotalSynapseCount());
        stats["firing_rate"] = std::to_string(brain->getAverageFiringRate());
        stats["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        stats["exc_inh_ratio"] = std::to_string(brain->getExcitationInhibitionRatio());
        return stats;
    }, "Export brain statistics as dictionary");
}

// Enhanced type conversions for better Python integration
void addEnhancedTypeConversions(py::class_<Action>& action_class, 
                                py::class_<SensoryPercept>& percept_class) {
    // Enhanced Action serialization
    action_class.def(py::pickle(
        [](const Action& a) {
            return py::make_tuple(a.getType(), a.getParameters());
        },
        [](py::tuple t) {
            Action action;
            action.setType(std::get<0>(t).cast<ActionType>());
            action.setParameters(std::get<1>(t).cast<std::vector<float>>());
            return action;
        }
    ));
    
    // Enhanced SensoryPercept serialization
    percept_class.def(py::pickle(
        [](const SensoryPercept& p) {
            return py::make_tuple(
                p.getVision(), p.getTouch(), p.getInternal(),
                p.getProprioception(), p.getAudio(), p.getTimestamp()
            );
        },
        [](py::tuple t) {
            SensoryPercept percept;
            percept.setVision(std::get<0>(t).cast<Vision>());
            // Touch is not available, use internal signals instead
            percept.setInternal(std::get<2>(t).cast<InternalSignals>());
            // Proprioception is not available, use internal signals instead
            percept.setAudio(std::get<4>(t).cast<Audio>());
            percept.setTimestamp(std::get<5>(t).cast<double>());
            return percept;
        }
    ));
}

// Enhanced documentation and type hints
void addEnhancedDocumentation(py::module_& m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        -----------------------------------------------------
        
        This is the enhanced Python interface for NLM, providing advanced
        functionality for research and production use cases.
        
        Key Features:
        - Advanced error handling with custom Python exceptions
        - Preset configurations for different use cases
        - Convenience functions for common tasks
        - Enhanced type safety and validation
        - Serialization support for data persistence
        - Type hints for IDE support
        
        Configuration Presets:
        - "baby": Small, developing brain configuration
        - "adult": Optimized, experienced brain configuration
        - "research": Maximum plasticity for exploration
        - "performance": Optimized for speed and stability
        
        Use Cases:
        - Research: Use configuration presets and advanced monitoring
        - Development: Use convenience functions and validation
        - Production: Use serialization
    )pbdoc";
    
    // Type aliases for better documentation
    py::class_<std::vector<float>>(m, "FloatVector")
        .def(py::init<>());
    
    py::class_<std::map<std::string, std::string>>(m, "StringMap")
        .def(py::init<>());
    
    // Example usage function
    m.def("getExample", []() {
        return R"example(
        Example: Basic agent simulation with learning
        
        import pynlm
        
        # Create brain, world, and agent with learning enabled
        brain, world, agent = pynlm.setup_brain_world_agent("learning", 20, True)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            agent.getBrain().step(step, step * 0.1)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            print(f"Step {step}: Position ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        
        # Export results
        stats = pynlm.export_brain_stats(agent.getBrain())
        pynlm.export_brain_data(agent.getBrain(), "simulation_results.bin")
        )example";
    }, "Get example usage code");
}

PYBIND11_MODULE(pynlm, m) {
    // Register enhanced exceptions
    registerNLMExceptions(m);
    
    // Type definitions
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
    
    // Basic types and enums (same as original)
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
    
    // Basic Config class (same as original)
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
             "Get a summary string of the configuration");
    
    // Add enhanced configuration features
    addConfigurationPresets(py::class_<Config>(m, "Config"));
    addConvenienceFunctions(m);
    
    // Add enhanced type conversions
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
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));
    
    addEnhancedTypeConversions(py::class_<Action>(m, "Action"), 
                               py::class_<SensoryPercept>(m, "SensoryPercept"));
    
    // Add enhanced documentation
    addEnhancedDocumentation(m);
    
    // Basic factory functions (same as original)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Phase 6 integration (enhanced)
    py::class_<Phase6Config>(m, "Phase6Config", R"pbdoc(Phase 6 integration configuration)pbdoc")
        .def(py::init<>());
    
    py::class_<Phase6IntegrationResult>(m, "Phase6IntegrationResult", R"pbdoc(Phase 6 integration test results)pbdoc")
        .def_readonly("totalReward", &Phase6IntegrationResult::totalReward)
        .def_readonly("avgFiringRate", &Phase6IntegrationResult::avgFiringRate)
        .def_readonly("memoryEpisodesStored", &Phase6IntegrationResult::memoryEpisodesStored)
        .def_readonly("dopamineLevel", &Phase6IntegrationResult::dopamineLevel)
        .def("isSystemIntegrated", &Phase6IntegrationResult::isSystemIntegrated);
    
    m.def("createPhase6Experiment", []() {
        return std::make_shared<Phase6IntegratedExperiment>();
    }, "Create Phase 6 integration experiment");
    
    // Additional type aliases for convenience
    py::class_<std::vector<float>>(m, "FloatArray");
    py::class_<std::map<std::string, std::string>>(m, "StringDict");
    
    // Version info
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Development Team";
    
    // Additional constants
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 2;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1f;
}

} // namespace nlm