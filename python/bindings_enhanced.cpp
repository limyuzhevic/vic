// Enhanced Python bindings for NLM - User-friendly API improvements
// This file enhances the original bindings.cpp with convenience functions,
// better error handling, and a more Pythonic interface.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

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

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error("NLM Error: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration: " + msg) {}
};

class BrainInitializationError : public NLMError {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMError("Brain Initialization: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation: " + msg) {}
};

// Enhanced configuration with Pythonic API
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        
        A user-friendly Python interface for the NLM neural simulation framework.
        
        Key improvements:
        - High-level convenience functions for common operations
        - Better error handling with specific exception types
        - Pythonic parameter naming and defaults
        - Batch processing utilities
        - Enhanced analysis and export capabilities
        - Factory methods for different scenarios
        
        Quick Start:
        ```python
        from nlm import createDefaultConfig, createBrain, quickSimulation
        
        # Create and run a simple brain
        config = createDefaultConfig()
        brain = createBrain(config)
        quickSimulation(brain, steps=100, input_strength=30.0)
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // ========== ENHANCED CONFIGURATION ==========
    
    py::class_<Config, std::shared_ptr<Config>>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"), "Load from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to JSON file")
        // Enhanced Pythonic setters
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set float value")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set integer value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set double value")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set string value")
        // Enhanced getters with defaults
        .def("getFloat", &Config::getOr<float>, py::arg("key"), py::arg("defaultValue"),
             "Get float value with default")
        .def("getInt", &Config::getOr<int64_t>, py::arg("key"), py::arg("defaultValue"),
             "Get integer value with default")
        .def("getDouble", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get double value with default")
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get string value with default")
        .def("has", &Config::has, py::arg("key"), "Check if key exists")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration")
        .def("summary", &Config::summary, "Get configuration summary")
        .def("exportAsDict", [](const Config& self) {
            std::map<std::string, std::string> result;
            for (const auto& key : self.getKeys()) {
                try {
                    float fval = self.getOr<float>(key, 0.0f);
                    result[key] = std::to_string(fval);
                } catch (...) {
                    try {
                        int64_t ival = self.getOr<int64_t>(key, 0);
                        result[key] = std::to_string(ival);
                    } catch (...) {
                        result[key] = "[complex]";
                    }
                }
            }
            return result;
        }, "Export configuration as Python dictionary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ========== BRAIN FACTORIES ==========
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Apply Phase 2 defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return config;
    }, "Create configuration with Phase 2 optimized defaults");

    m.def("createLearningConfig", [](float learning_rate, float connection_prob) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("plasticity_learning_rate", learning_rate, ConfigSource::Default);
        config->set("connection_probability", connection_prob, ConfigSource::Default);
        return config;
    }, py::arg("learning_rate") = 0.01, py::arg("connection_probability") = 0.15,
         "Create configuration optimized for learning");

    m.def("createMinimalConfig", [](size_t neuron_count) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        return config;
    }, py::arg("neuron_count") = 500, "Create minimal configuration");

    m.def("createConfigFromEnv", []() {
        auto config = std::make_shared<Config>();
        
        // Read from environment variables
        const char* neuron_count = std::getenv("NLM_NEURON_COUNT");
        if (neuron_count) {
            config->set("neuron_count", std::stoi(neuron_count), ConfigSource::Runtime);
        }
        
        const char* region_count = std::getenv("NLM_REGION_COUNT");
        if (region_count) {
            config->set("region_count", std::stoi(region_count), ConfigSource::Runtime);
        }
        
        return config;
    }, "Create configuration from environment variables");

    // ========== WORLD FACTORIES ==========
    
    m.def("createWorld", [](size_t width, size_t height, 
                           size_t vision_width = 16, 
                           size_t vision_height = 16) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
         py::arg("vision_width") = 16, py::arg("vision_height") = 16,
         "Create a world with specified dimensions");

    // ========== BRAIN FACTORIES ==========
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a brain with configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface");

    // ========== CONVENIENCE FUNCTIONS ==========
    
    m.def("quickSimulation", 
          [](std::shared_ptr<Brain> brain, size_t steps, float input_strength) {
              if (!brain->initialize()) {
                  throw BrainInitializationError("Failed to initialize brain");
              }
              
              if (input_strength > 0.0f) {
                  brain->injectCurrentToNeurons(NeuronType::Sensory, input_strength);
              }
              
              for (nlm::SimulationStep step = 0; step < steps; ++step) {
                  brain->step(step, step * 0.001);
              }
          },
          py::arg("brain"), py::arg("steps") = 100, py::arg("input_strength") = 0.0f,
          "Run a quick simulation with optional sensory input");

    m.def("runBasicTest", [](std::shared_ptr<Brain> brain) {
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain for basic test");
        }
        
        // Run basic connectivity test
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            throw SimulationError("No region found for basic test");
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            throw SimulationError("No neurons found for basic test");
        }
        
        size_t initial_spikes = brain->getTotalSpikeCount();
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return brain->getTotalSpikeCount() - initial_spikes;
    }, py::arg("brain"), "Run basic connectivity test and return spike count");

    m.def("analyzeLearning", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain for learning analysis");
        }
        
        // Record initial state
        size_t initial_synapse_count = brain->getTotalSynapseCount();
        std::vector<float> initial_weights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initial_weights.push_back(syn->getWeight());
            }
        }
        
        // Enable plasticity and run learning
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply input patterns
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            if (auto* region = brain->getRegion(RegionId(1))) {
                auto neurons = region->getAllNeurons();
                size_t sensory_count = 0;
                for (auto* neuron : neurons) {
                    if (neuron->getType() == NeuronType::Sensory && sensory_count < 20) {
                        brain->injectCurrent(neuron->getId(), 30.0f);
                        sensory_count++;
                    }
                }
            }
            brain->step(step, step * 0.001);
        }
        
        // Record final state
        size_t final_synapse_count = brain->getTotalSynapseCount();
        std::vector<float> final_weights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                final_weights.push_back(syn->getWeight());
            }
        }
        
        // Calculate learning metrics
        bool learning_occurred = false;
        float weight_change = 0.0f;
        
        if (!initial_weights.empty() && !final_weights.empty()) {
            float initial_sum = std::accumulate(initial_weights.begin(), initial_weights.end(), 0.0f);
            float final_sum = std::accumulate(final_weights.begin(), final_weights.end(), 0.0f);
            weight_change = final_sum - initial_sum;
            learning_occurred = std::abs(weight_change) > 0.001f;
        }
        
        return std::make_tuple(learning_occurred, weight_change, 
                              initial_synapse_count, final_synapse_count);
    }, py::arg("brain"), py::arg("steps") = 1000,
         "Run learning experiment and return metrics (learning, weight_change, initial_synapses, final_synapses)");

    m.def("createTextPercept", [](const std::string& text, 
                                 const std::vector<float>& vision = std::vector<float>(),
                                 const std::vector<float>& touch = std::vector<float>()) {
        auto percept = SensoryPercept();
        percept.setVision(vision);
        percept.setTouch(touch);
        // Encode text into neurons
        auto text_pattern = AgentBrain::encodeText(text);
        // Create a simple vision pattern that encodes the text
        std::vector<float> vision_with_text(vision.size(), 1.0f);
        for (size_t idx : text_pattern) {
            if (idx < vision.size()) {
                vision_with_text[idx] = 0.1f;  // Mark text neurons
            }
        }
        percept.setVision(vision_with_text);
        return percept;
    }, py::arg("text"), py::arg("vision") = std::vector<float>(), py::arg("touch") = std::vector<float>(),
         "Create a sensory percept with text encoding");

    m.def("createSensoryPercept", 
          [](const std::vector<float>& vision, 
             const std::vector<float>& touch,
             const std::vector<float>& internal = std::vector<float>(),
             const std::vector<float>& proprioception = std::vector<float>()) {
              auto percept = SensoryPercept();
              percept.setVision(vision);
              percept.setTouch(touch);
              percept.setInternal(internal);
              percept.setProprioception(proprioception);
              return percept;
          }, py::arg("vision"), py::arg("touch"), py::arg("internal") = std::vector<float>(),
             py::arg("proprioception") = std::vector<float>(),
          "Create a sensory percept from vector inputs");

    // ========== UTILITY FUNCTIONS ==========
    
    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        return stats;
    }, py::arg("brain"), "Get brain statistics as dictionary");

    m.def("compareBrains", 
          [](const std::vector<std::shared_ptr<Brain>>& brains,
             const std::vector<std::string>& labels) {
              if (brains.size() != labels.size()) {
                  throw ConfigurationError("Brains and labels must have same length");
              }
              
              std::map<std::string, std::map<std::string, double>> comparison;
              for (size_t i = 0; i < brains.size(); ++i) {
                  auto stats = getBrainStats(brains[i]);
                  comparison[labels[i]] = stats;
              }
              return comparison;
          }, py::arg("brains"), py::arg("labels"), "Compare multiple brains");

    m.def("exportBrainState", 
          [](std::shared_ptr<Brain> brain, const std::string& format) {
              if (format == "json") {
                  std::string result = "{\n";
                  result += "  \"total_neurons\": " + std::to_string(brain->getTotalNeuronCount()) + ",\n";
                  result += "  \"total_synapses\": " + std::to_string(brain->getTotalSynapseCount()) + ",\n";
                  result += "  \"active_neurons\": " + std::to_string(brain->getActiveNeuronCount()) + ",\n";
                  result += "  \"firing_rate\": " + std::to_string(brain->getAverageFiringRate()) + ",\n";
                  result += "  \"total_spikes\": " + std::to_string(brain->getTotalSpikeCount()) + "\n";
                  result += "}";
                  return result;
              } else if (format == "csv") {
                  std::string result = "metric,value\n";
                  result += "total_neurons," + std::to_string(brain->getTotalNeuronCount()) + "\n";
                  result += "total_synapses," + std::to_string(brain->getTotalSynapseCount()) + "\n";
                  result += "active_neurons," + std::to_string(brain->getActiveNeuronCount()) + "\n";
                  result += "firing_rate," + std::to_string(brain->getAverageFiringRate()) + "\n";
                  result += "total_spikes," + std::to_string(brain->getTotalSpikeCount()) + "\n";
                  return result;
              } else {
                  throw ConfigurationError("Unsupported format: " + format);
              }
          }, py::arg("brain"), py::arg("format") = "json", "Export brain state in specified format");

    m.def("batchTrain", 
          [](const std::vector<std::shared_ptr<Brain>>& brains,
             const std::vector<float>& rewards,
             size_t steps) {
              if (brains.size() != rewards.size()) {
                  throw ConfigurationError("Brains and rewards must have same length");
              }
              
              for (size_t i = 0; i < brains.size(); ++i) {
                  auto& brain = brains[i];
                  if (!brain->initialize()) {
                      throw BrainInitializationError("Failed to initialize brain in batch");
                  }
                  
                  for (nlm::SimulationStep step = 0; step < steps; ++step) {
                      brain->step(step);
                  }
              }
          }, py::arg("brains"), py::arg("rewards"), py::arg("steps"), "Batch train multiple brains");

    m.def("createDemoBrain", []() {
        auto config = createDefaultConfig();
        auto brain = createBrain(config);
        auto world = createWorld();
        auto agent = createAgentBrain(brain);
        
        // Initialize and set up a simple demo
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a demo brain with world and agent");

    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config->has("neuron_count")) errors.push_back("Missing neuron_count");
        if (!config->has("region_count")) errors.push_back("Missing region_count");
        if (!config->has("connection_probability")) errors.push_back("Missing connection_probability");
        
        if (errors.empty()) {
            return "Configuration is valid";
        } else {
            throw ConfigurationError("Validation failed: " + 
                                   (errors.size() > 1 ? "Multiple issues" : errors[0]));
        }
    }, py::arg("config"), "Validate configuration");

    m.def("getVersion", []() { return "NLM Python Bindings v2.0 (Enhanced)"; },
          "Get version of enhanced bindings");

    m.def("listAvailableBrains", []() {
        return std::vector<std::string>{
            "basic", "learning", "complex", "demo"
        };
    }, "List available brain types");

    m.def("createFromTemplate", 
          [](const std::string& template_name) {
              auto config = createDefaultConfig();
              auto brain = createBrain(config);
              
              if (template_name == "sensorimotor") {
                  config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
                  config->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
              } else if (template_name == "cognitive") {
                  config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
                  config->set("region_count", static_cast<int64_t>(3), ConfigSource::Runtime);
              }
              
              return brain;
          }, py::arg("template_name"), "Create brain from template");

    // ========== Original Classes (for backward compatibility) ==========
    
    // NeuronId
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Neuron ID)pbdoc")
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

    // SynapseId
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Synapse ID)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=);

    // RegionId
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Region ID)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=);

    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Population ID)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType")
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

    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) { self.setData(data); })
        .def("getData", &Vision::getData)
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) { self.setData(data); })
        .def("getData", &Audio::getData)
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
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

    // SimpleWorld class
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("current_time"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept)
        .def("getAgentBody", &SimpleWorld::getAgentBody)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // AgentBrain class
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"))
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predicted_reward"))
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"))
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset)
        .def("getBrain", &AgentBrain::getBrain)
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("encodeText", &AgentBrain::encodeText, py::arg("text"), "Encode text into pattern (Phase 6)")
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates)
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities)
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace);

    // Brain class
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize brain")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("current_step"), "Step simulation")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step), py::arg("current_step"), py::arg("current_time"), "Step with time")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"), "Receive sensory input")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"), "Inject current into neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"), "Inject to neuron type")
        .def("produceAction", &Brain::produceAction, "Produce action")
        .def("reset", &Brain::reset, "Reset brain")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add neural region")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion), py::arg("id"), "Get region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get region count")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neurons")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapses")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spikes")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, "Get configuration")
        .def("logStatus", &Brain::logStatus, "Log status");

    // Configuration helpers
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return config;
    }, "Create default configuration");

    // Module metadata
    m.attr("version") = "2.0.0 (Enhanced)";
    m.attr("description") = "Enhanced NLM Python bindings with user-friendly API";
}