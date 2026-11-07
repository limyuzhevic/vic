#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace nlm {

class PerformanceMonitor {
public:
    std::shared_ptr<Brain> brain;
    uint64_t steps_performed = 0;
    uint64_t total_spikes = 0;
    uint64_t total_steps = 0;
    double start_time = 0.0;
    std::chrono::steady_clock::time_point start_point;
    
    PerformanceMonitor(std::shared_ptr<Brain> b) : brain(b) {
        start_point = std::chrono::steady_clock::now();
        start_time = brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
    }
    
    void start() {
        start_point = std::chrono::steady_clock::now();
    }
    
    double stop() {
        auto end_point = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_point - start_point);
        return duration.count() / 1000.0; // Convert to seconds
    }
    
    void recordStep(SimulationStep step) {
        steps_performed++;
        total_steps = step;
        if (brain) {
            total_spikes = brain->getTotalSpikeCount();
        }
    }
    
    double getStepsPerSecond(double elapsed_time) {
        if (elapsed_time > 0.0) {
            return steps_performed / elapsed_time;
        }
        return 0.0;
    }
    
    double getSpikesPerSecond(double elapsed_time) {
        if (elapsed_time > 0.0) {
            return static_cast<double>(total_spikes) / elapsed_time;
        }
        return 0.0;
    }
    
    size_t getTotalSteps() const { return total_steps; }
    size_t getTotalSpikes() const { return total_spikes; }
    size_t getStepsPerformed() const { return steps_performed; }
    
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Performance Summary:" << std::endl;
        ss << "  Total steps: " << total_steps << std::endl;
        ss << "  Steps performed: " << steps_performed << std::endl;
        ss << "  Total spikes: " << total_spikes << std::endl;
        return ss.str();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");
    py::register_exception<std::runtime_error>(m, "ConfigurationError", "Error in configuration");
    py::register_exception<std::runtime_error>(m, "InitializationError", "Error during initialization");
    py::register_exception<std::runtime_error>(m, "StepError", "Error during simulation step");
    
    // Vision helper methods for easier usage
    m.def("create_vision", [](size_t width, size_t height, size_t channels) {
        auto vision = std::make_shared<Vision>(width, height, channels);
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
       "Create a Vision sensory input with specified dimensions");
    
    m.def("create_vision_from_data", [](size_t width, size_t height, 
                                       const std::vector<float>& data) {
        auto vision = std::make_shared<Vision>(width, height, 3);
        vision->setData(data);
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("data"),
       "Create a Vision sensory input from data array");
    
    // Audio helper methods
    m.def("create_audio", [](size_t sample_rate, size_t num_samples) {
        auto audio = std::make_shared<Audio>(sample_rate, num_samples);
        return audio;
    }, py::arg("sample_rate"), py::arg("num_samples"),
       "Create an Audio sensory input with specified parameters");
    
    m.def("create_audio_from_data", [](size_t sample_rate,
                                       const std::vector<float>& data) {
        auto audio = std::make_shared<Audio>(sample_rate, data.size());
        audio->setData(data);
        audio->setSampleRate(sample_rate);
        return audio;
    }, py::arg("sample_rate"), py::arg("data"),
       "Create an Audio sensory input from data array");
    
    // World helper methods
    m.def("create_world", &createSimpleWorld, "Create a simple world instance");
    
    m.def("create_agent_brain", &createAgentBrain, py::arg("brain"),
          "Create an agent brain interface");
    
    // Convenience functions for common setups
    m.def("create_test_config", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        return config;
    }, "Create a test configuration with typical parameters");
    
    m.def("create_development_config", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("development_synaptogenesis_rate", 0.001f, ConfigSource::Default);
        config->set("development_pruning_rate", 0.0001f, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        return config;
    }, "Create a development configuration with learning focus");
    
    m.def("create_minimal_config", []() {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        return config;
    }, "Create a minimal configuration for quick testing");
    
    // Enhanced action creation helpers
    m.def("create_action", [](ActionType type) {
        return std::make_shared<Action>(type);
    }, py::arg("type"), "Create an action with specified type");
    
    m.def("create_action_with_params", [](ActionType type, 
                                          const std::vector<float>& parameters) {
        return std::make_shared<Action>(type, parameters);
    }, py::arg("type"), py::arg("parameters"), "Create an action with type and parameters");
    
    // Sensory input helper methods
    m.def("create_sensory_percept", &createSensoryPercept, 
          "Create a sensory percept instance");
    
    // Configuration convenience methods
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             "Set double configuration value")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             "Set float configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("getInt", [](Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"),
             "Get integer configuration value")
        .def("getDouble", [](Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"),
             "Get double configuration value")
        .def("getFloat", [](Config& self, const std::string& key) {
            auto value = self.get<float>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"),
             "Get float configuration value")
        .def("getBool", [](Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"),
             "Get boolean configuration value")
        .def("getString", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"),
             "Get string configuration value")
        .def("getIntOr", [](Config& self, const std::string& key, int default_value) {
            return self.getOr<int>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
             "Get integer configuration value or default")
        .def("getDoubleOr", [](Config& self, const std::string& key, double default_value) {
            return self.getOr<double>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
             "Get double configuration value or default")
        .def("getFloatOr", [](Config& self, const std::string& key, float default_value) {
            return self.getOr<float>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
             "Get float configuration value or default")
        .def("getBoolOr", [](Config& self, const std::string& key, bool default_value) {
            return self.getOr<bool>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
             "Get boolean configuration value or default")
        .def("getStringOr", [](Config& self, const std::string& key, const std::string& default_value) {
            return self.getOr<std::string>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
             "Get string configuration value or default")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
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
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Performance monitoring and profiling capabilities
    m.def("performance_monitor", [](std::shared_ptr<Brain> brain) {
        class PerformanceMonitor {
        public:
            std::shared_ptr<Brain> brain;
            uint64_t steps_performed = 0;
            uint64_t total_spikes = 0;
            uint64_t total_steps = 0;
            double start_time = 0.0;
            std::chrono::steady_clock::time_point start_point;
            
            PerformanceMonitor(std::shared_ptr<Brain> b) : brain(b) {
                start_point = std::chrono::steady_clock::now();
                start_time = brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
            }
            
            void start() {
                start_point = std::chrono::steady_clock::now();
            }
            
            double stop() {
                auto end_point = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_point - start_point);
                return duration.count() / 1000.0; // Convert to seconds
            }
            
            void recordStep(SimulationStep step) {
                steps_performed++;
                total_steps = step;
                if (brain) {
                    total_spikes = brain->getTotalSpikeCount();
                }
            }
            
            double getStepsPerSecond(double elapsed_time) {
                if (elapsed_time > 0.0) {
                    return steps_performed / elapsed_time;
                }
                return 0.0;
            }
            
            double getSpikesPerSecond(double elapsed_time) {
                if (elapsed_time > 0.0) {
                    return static_cast<double>(total_spikes) / elapsed_time;
                }
                return 0.0;
            }
            
            size_t getTotalSteps() const { return total_steps; }
            size_t getTotalSpikes() const { return total_spikes; }
            size_t getStepsPerformed() const { return steps_performed; }
            
            std::string getSummary() const {
                std::stringstream ss;
                ss << "Performance Summary:" << std::endl;
                ss << "  Total steps: " << total_steps << std::endl;
                ss << "  Steps performed: " << steps_performed << std::endl;
                ss << "  Total spikes: " << total_spikes << std::endl;
                return ss.str();
            }
        };
        
        return std::make_shared<PerformanceMonitor>(brain);
    }, py::arg("brain"),
         "Create a performance monitor for a brain instance");
    
    m.def("profile_brain", [](std::shared_ptr<Brain> brain, SimulationStep steps) {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        double elapsed_time = duration.count() /  prostitution_ms; // Convert to seconds
        size_t total_spikes = brain->getTotalSpikeCount();
        
        std::cout << "Performance Profile:" << std::endl;
        std::cout << "  Total steps: " << steps << std::endl;
        std::cout << "  Elapsed time: " << elapsed_time << " seconds" << std::endl;
        std::cout << "  Steps per second: " << (steps * 1000.0 / duration.count()) << std::endl;
        std::cout << "  Total spikes: " << total_spikes << std::endl;
        std::cout << "  Spikes per second: " << (total_spikes * 1000.0 / duration.count()) << std::endl;
        
        return std::make_pair(elapsed_time, total_spikes);
    }, py::arg("brain"), py::arg("steps"),
         "Run a simulation with timing and profiling");
    
    // AgentBrain performance monitoring
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Performance monitoring methods for AgentBrain
        .def("getPerformanceStats", [](AgentBrain& self) {
            std::stringstream ss;
            ss << "AgentBrain Performance Stats:" << std::endl;
            ss << "  Developmental stage: " << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            ss << "  Neuromodulation level: " << self.getNeuromodulationLevel() << std::endl;
            ss << "  Curiosity level: " << self.getCuriosityLevel() << std::endl;
            ss << "  Novelty level: " << self.getNoveltyLevel() << std::endl;
            ss << "  Prediction error: " << self.getPredictionError() << std::endl;
            return ss.str();
        }, "Get performance statistics for agent brain");
    
    // World performance monitoring
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Performance monitoring methods for SimpleWorld
        .def("getWorldPerformanceStats", [](SimpleWorld& self) {
            std::stringstream ss;
            ss << "World Performance Stats:" << std::endl;
            ss << "  Width: " << self.getWidth() << std::endl;
            ss << "  Height: " << self.getHeight() << std::endl;
            ss << "  Max energy: " << self.getMaxEnergy() << std::endl;
    // Performance monitoring capabilities
    m.def("profile_brain_performance", [](std::shared_ptr<Brain> brain, SimulationStep steps) {
        auto start = std::chrono::steady_clock::now();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        double elapsed_time = duration.count() / 1000.0; // Convert to seconds
        size_t total_spikes = brain->getTotalSpikeCount();
        
        std::cout << "Performance Profile:" << std::endl;
        std::cout << "  Total steps: " << steps << std::endl;
        std::cout << "  Elapsed time: " << elapsed_time << " seconds" << std::endl;
        std::cout << "  Steps per second: " << (steps * 1000.0 / duration.count()) << std::endl;
        std::cout << "  Total spikes: " << total_spikes << std::endl;
        std::cout << "  Spikes per second: " << (total_spikes * 1000.0 / duration.count()) << std::endl;
        
        return std::make_pair(elapsed_time, total_spikes);
    }, py::arg("brain"), py::arg("steps"),
         "Run a simulation with timing and profiling");
    
    // Performance monitoring class
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring for brain simulations)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("start", &PerformanceMonitor::start, "Start timing performance measurement")
        .def("stop", &PerformanceMonitor::stop, "Stop timing and return elapsed time in seconds")
        .def("recordStep", &PerformanceMonitor::recordStep, py::arg("step"), "Record a simulation step")
        .def("getStepsPerSecond", &PerformanceMonitor::getStepsPerSecond, py::arg("elapsed_time"), 
             "Get steps per second given elapsed time")
        .def("getSpikesPerSecond", &PerformanceMonitor::getSpikesPerSecond, py::arg("elapsed_time"),
             "Get spikes per second given elapsed time")
        .def("getTotalSteps", &PerformanceMonitor::getTotalSteps, "Get total number of steps")
        .def("getTotalSpikes", &PerformanceMonitor::getTotalSpikes, "Get total number of spikes")
        .def("getStepsPerformed", &PerformanceMonitor::getStepsPerformed, "Get number of steps recorded")
        .def("getSummary", &PerformanceMonitor::getSummary, "Get performance summary as string");
    
    m.def("performance_monitor", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<PerformanceMonitor>(brain);
    }, py::arg("brain"),
         "Create a performance monitor for a brain instance");
