#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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
        })
        // Advanced configuration methods
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a boolean configuration value")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set an integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a float configuration value")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a double configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            return self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a string configuration value")
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a boolean configuration value")
        .def("getInt", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get an integer configuration value")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a float configuration value")
        .def("getDouble", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a double configuration value")
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a string configuration value")
        .def("validate", &Config::validate,
             "Validate the current configuration")
        .def("clone", []() {
            return std::make_shared<Config>();
        }, "Create a copy of the configuration")
        .def("merge", [](Config& self, const Config& other) {
            // Merge another config into this one
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                if (other.has(key)) {
                    self.set(key, other.getOr(key, ""));
                }
            }
            return self;
        }, py::arg("other"),
           "Merge another configuration into this one");

    // Add convenience functions for creating configurations
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createOptimizedConfig", [](bool enableMultithreading = true, bool enableSIMD = true) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Performance optimizations
        config->set("brain.neuron_count", 100000, ConfigSource::Default);
        config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
        config->set("performance.enable_multithreading", enableMultithreading, ConfigSource::Default);
        config->set("performance.enable_simd", enableSIMD, ConfigSource::Default);
        config->set("performance.thread_count", std::thread::hardware_concurrency(), ConfigSource::Default);
        
        // Advanced features
        config->set("neuromodulation.enabled", true, ConfigSource::Default);
        config->set("memory.working_memory_capacity", 1000, ConfigSource::Default);
        config->set("memory.episodic_memory_max_episodes", 10000, ConfigSource::Default);
        config->set("prediction.enabled", true, ConfigSource::Default);
        config->set("development.enabled", true, ConfigSource::Default);
        
        return config;
    }, py::arg("enableMultithreading") = true, py::arg("enableSIMD") = true,
       "Create an optimized configuration with performance settings");
    
    m.def("createExperimentConfig", [](const std::string& experimentName) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        if (experimentName == "phase5_lifelong") {
            // Phase 5 lifelong learning configuration
            config->set("brain.neuron_count", 100000, ConfigSource::Default);
            config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
            config->set("development.synapse_formation_steps", 10000, ConfigSource::Default);
            config->set("development.sensory_development_steps", 40000, ConfigSource::Default);
            config->set("development.motor_development_steps", 50000, ConfigSource::Default);
            config->set("development.associative_development_steps", 100000, ConfigSource::Default);
            config->set("development.memory_consolidation_steps", 100000, ConfigSource::Default);
            config->set("development.social_development_steps", 100000, ConfigSource::Default);
            config->set("performance.enable_multithreading", true, ConfigSource::Default);
            config->set("performance.enable_simd", true, ConfigSource::Default);
            config->set("performance.enable_event_driven", true, ConfigSource::Default);
            config->set("memory.enabled", true, ConfigSource::Default);
            config->set("memory.working_memory_capacity", 10000, ConfigSource::Default);
            config->set("memory.episodic_memory_max_episodes", 100000, ConfigSource::Default);
            config->set("neuromodulation.enabled", true, ConfigSource::Default);
            config->set("prediction.enabled", true, ConfigSource::Default);
            config->set("visualization.enabled", false, ConfigSource::Default); // Disable for speed
        } else if (experimentName == "phase5_single") {
            // Single Phase 5 experiment configuration
            config->set("brain.neuron_count", 50000, ConfigSource::Default);
            config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
            config->set("performance.enable_multithreading", true, ConfigSource::Default);
            config->set("performance.enable_simd", true, ConfigSource::Default);
            config->set("performance.enable_event_driven", true, ConfigSource::Default);
        } else if (experimentName == "quick_test") {
            // Quick test configuration
            config->set("brain.neuron_count", 1000, ConfigSource::Default);
            config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
            config->set("performance.enable_multithreading", false, ConfigSource::Default);
            config->set("performance.enable_simd", false, ConfigSource::Default);
            config->set("performance.enable_event_driven", false, ConfigSource::Default);
        }
        
        return config;
    }, py::arg("experimentName"), "Create a configuration optimized for a specific experiment type");

    m.def("getDefaultConfigs", []() {
        py::dict configs;
        configs["default"] = std::make_shared<Config>();
        configs["optimized"] = createOptimizedConfig();
        configs["phase5_lifelong"] = createExperimentConfig("phase5_lifelong");
        configs["phase5_single"] = createExperimentConfig("phase5_single");
        configs["quick_test"] = createExperimentConfig("quick_test");
        return configs;
    }, "Get a dictionary of predefined configuration presets");


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

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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

    // Advanced Python API functions for developers
    
    // Performance monitoring and profiling
    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, uint64_t steps, bool warmup = true) -> py::dict {
        py::dict results;
        
        if (warmup) {
            NLM_LOG_INFO("Warming up brain...")
            for (uint64_t i = 0; i < steps / 10; ++i) {
                brain->step(i, i * 0.001);
            }
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        double stepsPerSecond = steps / duration;
        
        results["steps"] = steps;
        results["duration_seconds"] = duration;
        results["steps_per_second"] = stepsPerSecond;
        results["avg_firing_rate"] = brain->getAverageFiringRate();
        results["total_neurons"] = brain->getTotalNeuronCount();
        results["total_synapses"] = brain->getTotalSynapseCount();
        results["firing_neurons"] = brain->getFiringNeuronCount();
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("warmup") = true,
       "Benchmark brain performance over a given number of steps");
    
    m.def("profileBrainMemory", [](std::shared_ptr<Brain> brain) -> py::dict {
        py::dict results;
        
        results["neuron_count"] = brain->getTotalNeuronCount();
        results["synapse_count"] = brain->getTotalSynapseCount();
        results["firing_neuron_count"] = brain->getFiringNeuronCount();
        results["average_firing_rate"] = brain->getAverageFiringRate();
        results["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Estimate memory usage (rough approximation)
        size_t memoryBytes = sizeof(Brain) + 
                            brain->getTotalNeuronCount() * sizeof(Neuron) +
                            brain->getTotalSynapseCount() * sizeof(Synapse);
        results["estimated_memory_bytes"] = memoryBytes;
        
        return results;
    }, py::arg("brain"), "Get memory usage and statistics for a brain");
    
    // Advanced configuration management
    m.def("validateConfiguration", [](const Config& config) -> py::dict {
        py::dict results;
        results["valid"] = config.validate();
        
        if (!results["valid"].cast<bool>()) {
            results["errors"] = py::list(); // Empty list for errors
        }
        
        return results;
    }, py::arg("config"), "Validate a configuration and return validation results");
    
    m.def("mergeConfigs", [](const Config& config1, const Config& config2) {
        auto result = std::make_shared<Config>(config1);
        auto keys = config2.getKeys();
        for (const auto& key : keys) {
            if (config2.has(key)) {
                result->set(key, config2.getOr(key, ""));
            }
        }
        return result;
    }, py::arg("config1"), py::arg("config2"), "Merge two configurations");
    
    // Experiment utilities
    m.def("createExperimentConfig", [](const std::string& experimentType, const py::dict& customSettings) {
        auto config = std::make_shared<Config>();
        
        if (experimentType == "phase5_lifelong") {
            // Phase 5 lifelong learning configuration
            config->set("brain.neuron_count", 100000, ConfigSource::Default);
            config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
            config->set("development.synapse_formation_steps", 10000, ConfigSource::Default);
            config->set("development.sensory_development_steps", 40000, ConfigSource::Default);
            config->set("development.motor_development_steps", 50000, ConfigSource::Default);
            config->set("development.associative_development_steps", 100000, ConfigSource::Default);
            config->set("development.memory_consolidation_steps", 100000, ConfigSource::Default);
            config->set("development.social_development_steps", 100000, ConfigSource::Default);
            config->set("performance.enable_multithreading", true, ConfigSource::Default);
            config->set("performance.enable_simd", true, ConfigSource::Default);
            config->set("performance.enable_event_driven", true, ConfigSource::Default);
            config->set("memory.enabled", true, ConfigSource::Default);
            config->set("memory.working_memory_capacity", 10000, ConfigSource::Default);
            config->set("memory.episodic_memory_max_episodes", 100000, ConfigSource::Default);
            config->set("neuromodulation.enabled", true, ConfigSource::Default);
            config->set("prediction.enabled", true, ConfigSource::Default);
        } else if (experimentType == "phase5_single") {
            // Single Phase 5 experiment configuration
            config->set("brain.neuron_count", 50000, ConfigSource::Default);
            config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
            config->set("performance.enable_multithreading", true, ConfigSource::Default);
            config->set("performance.enable_simd", true, ConfigSource::Default);
            config->set("performance.enable_event_driven", true, ConfigSource::Default);
        }
        
        // Apply custom settings
        for (auto it = customSettings.begin(); it != customSettings.end(); ++it) {
            std::string key = it->first.cast<std::string>();
            if (it->second.is_same_type(py::int_(0))) {
                config->set(key, it->second.cast<int64_t>(), ConfigSource::Default);
            } else if (it->second.is_same_type(py::float_(0.0))) {
                config->set(key, it->second.cast<float>(), ConfigSource::Default);
            } else if (it->second.is_same_type(py::bool_(true))) {
                config->set(key, it->second.cast<bool>(), ConfigSource::Default);
            } else {
                config->set(key, it->second.cast<std::string>(), ConfigSource::Default);
            }
        }
        
        return config;
    }, py::arg("experimentType"), py::arg("customSettings") = py::dict(),
       "Create a configuration optimized for a specific experiment type");
    
    // Batch operations
    m.def("runBatchBenchmarks", [](const std::vector<std::shared_ptr<Config>>& configs, uint64_t steps) {
        py::list results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            auto brain = std::make_shared<Brain>(configs[i]);
            brain->initialize();
            
            auto startTime = std::chrono::high_resolution_clock::now();
            for (uint64_t step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            py::dict result;
            result["config_index"] = static_cast<int>(i);
            result["config_summary"] = configs[i]->summary();
            result["steps_per_second"] = steps / duration;
            result["total_reward"] = brain->getAverageFiringRate() * steps;
            result["neurons"] = brain->getTotalNeuronCount();
            result["synapses"] = brain->getTotalSynapseCount();
            
            results.append(result);
        }
        
        return results;
    }, py::arg("configs"), py::arg("steps"), "Run benchmarks on multiple configurations");
    
    // Advanced utility functions
    m.def("getSystemInfo", []() {
        py::dict info;
        info["version"] = "NLM Phase 5";
        info["cpp_standard"] = "C++20";
        info["build_type"] = "Release";
        info["threading"] = std::thread::hardware_concurrency();
        info["pybind11_version"] = PYBIND11_VERSION;
        return info;
    }, "Get system information about the NLM installation");
    
    m.def("benchmarkConfiguration", [](const Config& config, uint64_t steps) {
        auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        for (uint64_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        double stepsPerSecond = steps / duration;
        
        py::dict result;
        result["config"] = config.summary();
        result["steps_per_second"] = stepsPerSecond;
        result["avg_firing_rate"] = brain->getAverageFiringRate();
        result["memory_usage"] = brain->getTotalNeuronCount() * 100; // Approximate
        
        return result;
    }, py::arg("config"), py::arg("steps"), "Benchmark a specific configuration");
    
    // Error handling utilities
    m.def("safeBrainStep", [](std::shared_ptr<Brain> brain, uint64_t step, Timestamp time) {
        try {
            brain->step(step, time);
            return py::make_tuple(true, "Step completed successfully");
        } catch (const std::exception& e) {
            return py::make_tuple(false, std::string(e.what()));
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time"), "Safely execute a brain step with error handling");
    
    m.def("safeBrainInitialization", [](std::shared_ptr<Brain> brain) {
        try {
            if (brain->initialize()) {
                return py::make_tuple(true, "Initialization successful");
            } else {
                return py::make_tuple(false, "Initialization failed");
            }
        } catch (const std::exception& e) {
            return py::make_tuple(false, std::string(e.what()));
        }
    }, py::arg("brain"), "Safely initialize a brain with error handling");
    
    // Performance optimization suggestions
    m.def("getPerformanceRecommendations", [](uint64_t neuronCount) {
        py::dict recommendations;
        
        if (neuronCount > 50000) {
            recommendations["use_multithreading"] = true;
            recommendations["use_simd"] = true;
            recommendations["use_event_driven"] = true;
            recommendations["use_sparse_connectivity"] = true;
            recommendations["memory_pool_size"] = neuronCount / 1000 * 100; // MB
        } else if (neuronCount > 10000) {
            recommendations["use_multithreading"] = true;
            recommendations["use_simd"] = true;
            recommendations["use_event_driven"] = false;
            recommendations["use_sparse_connectivity"] = false;
        } else {
            recommendations["use_multithreading"] = false;
            recommendations["use_simd"] = false;
            recommendations["use_event_driven"] = false;
            recommendations["use_sparse_connectivity"] = false;
        }
        
        return recommendations;
    }, py::arg("neuronCount"), "Get performance optimization recommendations for a given neuron count");
    
    // Configuration validation and suggestions
    m.def("validateAndSuggestImprovements", [](const Config& config) {
        py::dict results;
        bool isValid = config.validate();
        results["valid"] = isValid;
        
        py::list suggestions;
        
        if (config.getOr<int64_t>("brain.neuron_count", 0) > 50000) {
            if (!config.has("performance.enable_multithreading") || 
                !config.getOr<bool>("performance.enable_multithreading", false)) {
                suggestions.append("Consider enabling multithreading for large neuron counts");
            }
            if (!config.has("performance.enable_simd") || 
                !config.getOr<bool>("performance.enable_simd", false)) {
                suggestions.append("Consider enabling SIMD optimization for large neuron counts");
            }
        }
        
        if (config.getOr<int64_t>("brain.neuron_count", 0) > 10000) {
            if (!config.has("performance.enable_event_driven") || 
                !config.getOr<bool>("performance.enable_event_driven", false)) {
                suggestions.append("Consider enabling event-driven processing for large networks");
            }
        }
        
        results["suggestions"] = suggestions;
        return results;
    }, py::arg("config"), "Validate configuration and provide improvement suggestions");

    // Add convenience functions for creating configurations
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createOptimizedConfig", [](bool enableMultithreading = true, bool enableSIMD = true) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Performance optimizations
        config->set("brain.neuron_count", 100000, ConfigSource::Default);
        config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
        config->set("performance.enable_multithreading", enableMultithreading, ConfigSource::Default);
        config->set("performance.enable_simd", enableSIMD, ConfigSource::Default);
        config->set("performance.thread_count", std::thread::hardware_concurrency(), ConfigSource::Default);
        
        // Advanced features
        config->set("neuromodulation.enabled", true, ConfigSource::Default);
        config->set("memory.working_memory_capacity", 1000, ConfigSource::Default);
        config->set("memory.episodic_memory_max_episodes", 10000, ConfigSource::Default);
        config->set("prediction.enabled", true, ConfigSource::Default);
        config->set("development.enabled", true, ConfigSource::Default);
        
        return config;
    }, py::arg("enableMultithreading") = true, py::arg("enableSIMD") = true,
       "Create an optimized configuration with performance settings");
    
    m.def("createExperimentConfig", [](const std::string& experimentName) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        if (experimentName == "phase5_lifelong") {
            // Phase 5 lifelong learning configuration
            config->set("brain.neuron_count", 100000, ConfigSource::Default);
            config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
            config->set("development.synapse_formation_steps", 10000, ConfigSource::Default);
            config->set("development.sensory_development_steps", 40000, ConfigSource::Default);
            config->set("development.motor_development_steps", 50000, ConfigSource::Default);
            config->set("development.associative_development_steps", 100000, ConfigSource::Default);
            config->set("development.memory_consolidation_steps", 100000, ConfigSource::Default);
            config->set("development.social_development_steps", 100000, ConfigSource::Default);
            config->set("performance.enable_multithreading", true, ConfigSource::Default);
            config->set("performance.enable_simd", true, ConfigSource::Default);
            config->set("performance.enable_event_driven", true, ConfigSource::Default);
            config->set("memory.enabled", true, ConfigSource::Default);
            config->set("memory.working_memory_capacity", 10000, ConfigSource::Default);
            config->set("memory.episodic_memory_max_episodes", 100000, ConfigSource::Default);
            config->set("neuromodulation.enabled", true, ConfigSource::Default);
            config->set("prediction.enabled", true, ConfigSource::Default);
            config->set("visualization.enabled", false, ConfigSource::Default); // Disable for speed
        } else if (experimentName == "phase5_single") {
            // Single Phase 5 experiment configuration
            config->set("brain.neuron_count", 50000, ConfigSource::Default);
            config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
            config->set("performance.enable_multithreading", true, ConfigSource::Default);
            config->set("performance.enable_simd", true, ConfigSource::Default);
            config->set("performance.enable_event_driven", true, ConfigSource::Default);
        } else if (experimentName == "quick_test") {
            // Quick test configuration
            config->set("brain.neuron_count", 1000, ConfigSource::Default);
            config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
            config->set("performance.enable_multithreading", false, ConfigSource::Default);
            config->set("performance.enable_simd", false, ConfigSource::Default);
            config->set("performance.enable_event_driven", false, ConfigSource::Default);
        }
        
        return config;
    }, py::arg("experimentName"), "Create a configuration optimized for a specific experiment type");

    m.def("getDefaultConfigs", []() {
        py::dict configs;
        configs["default"] = std::make_shared<Config>();
        configs["optimized"] = createOptimizedConfig();
        configs["phase5_lifelong"] = createExperimentConfig("phase5_lifelong");
        configs["phase5_single"] = createExperimentConfig("phase5_single");
        configs["quick_test"] = createExperimentConfig("quick_test");
        return configs;
    }, "Get a dictionary of predefined configuration presets");


    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
