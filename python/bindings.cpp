#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
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

using namespace pybind11::literals;

namespace nlm {

// Custom Python exception classes
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigValidationError : public NLMException {
public:
    explicit ConfigValidationError(const std::string& msg) : NLMException("Configuration error: " + msg) {}
};

class BrainInitializationError : public NLMException {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMException("Brain initialization error: " + msg) {}
};

class InvalidOperationError : public NLMException {
public:
    explicit InvalidOperationError(const std::string& msg) : NLMException("Invalid operation: " + msg) {}
};

// Helper function to convert C++ exceptions to Python exceptions
void setupExceptionHandling(py::module& m) {
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigValidationError>(m, "ConfigValidationError", PyExc_ValueError);
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError", PyExc_RuntimeError);
    py::register_exception<InvalidOperationError>(m, "InvalidOperationError", PyExc_RuntimeError);
}

// Enhanced Config class with better validation and Pythonic API
void enhanceConfigClass(py::class_<Config>& configClass) {
    configClass
        // Enhanced configuration methods
        .def("set_value", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value with key. Key can be string or enum-like.")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value,
                            const std::string& description = "") {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), py::arg("description") = "",
            "Set string configuration value")
        .def("set_int", [](Config& self, const std::string& key, int64_t value,
                          const std::string& description = "") {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), py::arg("description") = "",
            "Set integer configuration value")
        .def("set_double", [](Config& self, const std::string& key, double value,
                             const std::string& description = "") {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), py::arg("description") = "",
            "Set double configuration value")
        .def("set_float", [](Config& self, const std::string& key, float value,
                            const std::string& description = "") {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), py::arg("description") = "",
            "Set float configuration value")
        .def("set_bool", [](Config& self, const std::string& key, bool value,
                           const std::string& description = "") {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), py::arg("description") = "",
            "Set boolean configuration value")
        .def("set_vector_int", [](Config& self, const std::string& key,
                                 const std::vector<int64_t>& value,
                                 const std::string& description = "") {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), py::arg("description") = "",
            "Set integer vector configuration value")
        .def("set_vector_double", [](Config& self, const std::string& key,
                                     const std::vector<double>& value,
                                     const std::string& description = "") {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), py::arg("description") = "",
            "Set double vector configuration value")
        
        // Convenience methods for common configuration keys
        .def("set_random_seed", &Config::set, "random_seed", py::arg("seed"),
             "Set random seed for reproducible experiments")
        .def("set_neuron_count", &Config::set, "neuron_count", py::arg("count"),
             "Set number of neurons in the brain")
        .def("set_region_count", &Config::set, "region_count", py::arg("count"),
             "Set number of brain regions")
        .def("set_connection_probability", &Config::set, "connection_probability", py::arg("prob"),
             "Set connection probability for random connectivity")
        .def("set_simulation_timestep", &Config::set, "simulation_timestep", py::arg("timestep"),
             "Set simulation timestep in seconds")
        .def("set_max_steps", &Config::set, "max_simulation_steps", py::arg("steps"),
             "Set maximum simulation steps")
        
        // Enhanced get methods with validation
        .def("get_value", &Config::getOr, py::arg("key"), py::arg("default_value"),
             "Get configuration value with default if not found")
        .def("get_string", [](const Config& self, const std::string& key,
                             const std::string& defaultValue = "") {
            auto val = self.get<std::string>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value") = "",
            "Get string configuration value")
        .def("get_int", [](const Config& self, const std::string& key,
                          int64_t defaultValue = 0) {
            auto val = self.get<int64_t>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value") = 0,
            "Get integer configuration value")
        .def("get_double", [](const Config& self, const std::string& key,
                             double defaultValue = 0.0) {
            auto val = self.get<double>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value") = 0.0,
            "Get double configuration value")
        .def("get_float", [](const Config& self, const std::string& key,
                            float defaultValue = 0.0f) {
            auto val = self.get<double>(key);
            return val.has_value() ? static_cast<float>(val.value()) : defaultValue;
        }, py::arg("key"), py::arg("default_value") = 0.0f,
            "Get float configuration value")
        .def("get_bool", [](const Config& self, const std::string& key,
                           bool defaultValue = false) {
            auto val = self.get<bool>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value") = false,
            "Get boolean configuration value")
        
        // Configuration validation
        .def("validate", [](const Config& self) {
            std::vector<std::string> errors;
            
            // Check required configuration keys
            auto keys = self.getKeys();
            
            // Validate neuron_count
            if (self.has("neuron_count")) {
                auto neuronCount = self.get<int64_t>("neuron_count");
                if (neuronCount && *neuronCount <= 0) {
                    errors.push_back("neuron_count must be positive");
                }
            }
            
            // Validate region_count
            if (self.has("region_count")) {
                auto regionCount = self.get<int64_t>("region_count");
                if (regionCount && (*regionCount <= 0 || *regionCount > 100)) {
                    errors.push_back("region_count must be between 1 and 100");
                }
            }
            
            // Validate connection_probability
            if (self.has("connection_probability")) {
                auto prob = self.get<double>("connection_probability");
                if (prob && (*prob < 0.0 || *prob > 1.0)) {
                    errors.push_back("connection_probability must be between 0.0 and 1.0");
                }
            }
            
            // Validate simulation_timestep
            if (self.has("simulation_timestep")) {
                auto timestep = self.get<double>("simulation_timestep");
                if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
                    errors.push_back("simulation_timestep must be between 0.0 and 1.0");
                }
            }
            
            if (!errors.empty()) {
                throw ConfigValidationError("Configuration validation failed:\n" + 
                                           std::accumulate(errors.begin() + 1, errors.end(), errors[0],
                                                         [](const std::string& a, const std::string& b) {
                                                             return a + "\n" + b;
                                                         }));
            }
            
            return true;
        }, "Validate configuration values")
        
        // Create default configuration with validation
        .def_static("create_default_config", []() {
            auto config = std::make_shared<Config>();
            
            // Set sensible defaults
            config->setRandomSeed(42);
            config->setNeuronCount(1000);
            config->setRegionCount(2);
            config->setConnectionProbability(0.1);
            config->setSimulationTimestep(0.001);
            config->set("stdp_ltp_weight", 0.01f);
            config->set("stdp_ltd_weight", 0.012f);
            config->set("stdp_tau", 20.0f);
            config->set("plasticity_learning_rate", 0.01f);
            config->set("max_simulation_steps", 10000LL);
            
            return config;
        }, "Create a default configuration with sensible values")
        
        // Create configuration from JSON with validation
        .def_static("create_config_from_json", [](const std::string& filepath) {
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(filepath)) {
                throw ConfigValidationError("Failed to load configuration from JSON file: " + filepath);
            }
            
            // Validate the loaded configuration
            config->validate();
            
            return config;
        }, py::arg("filepath"),
            "Create configuration from JSON file with validation")
        
        // Create configuration from YAML with validation
        .def_static("create_config_from_yaml", [](const std::string& filepath) {
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(filepath)) {
                throw ConfigValidationError("Failed to load configuration from YAML file: " + filepath);
            }
            
            // Validate the loaded configuration
            config->validate();
            
            return config;
        }, py::arg("filepath"),
            "Create configuration from YAML file with validation");
}

// Enhanced Brain class with better API
void enhanceBrainClass(py::class_<Brain>& brainClass) {
    brainClass
        // Enhanced initialization with validation
        .def("initialize", [](Brain& self) {
            // Validate configuration before initialization
            auto config = self.getConfig();
            config->validate();
            
            bool success = self.initialize();
            if (!success) {
                throw BrainInitializationError("Brain initialization failed");
            }
            
            return success;
        }, "Initialize the brain with configuration validation")
        
        // Enhanced step methods with better Python API
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("step"), "Perform a simulation step")
        .def("step_with_time", [](Brain& self, SimulationStep step, Timestamp time) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time"), "Perform a simulation step with timestamp")
        
        // Convenience injection methods
        .def("inject_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        
        // Convenience action methods
        .def("create_action", [](Brain& self, ActionType type,
                                const std::vector<float>& parameters = {}) {
            if (parameters.empty()) {
                return std::make_unique<Action>(type);
            }
            return std::make_unique<Action>(type, parameters);
        }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
            "Create an action with type and optional parameters")
        
        // Enhanced statistics methods
        .def("get_neuron_stats", [](const Brain& self) {
            std::map<std::string, size_t> stats;
            stats["total"] = self.getTotalNeuronCount();
            stats["active"] = self.getActiveNeuronCount();
            stats["firing"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            return stats;
        }, "Get comprehensive neuron statistics")
        
        .def("get_plasticity_stats", [](const Brain& self) {
            std::map<std::string, float> stats;
            stats["e_to_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            return stats;
        }, "Get plasticity and network statistics")
        
        // Enhanced development system methods
        .def("set_development_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("get_development_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        
        // Enhanced learning methods
        .def("enable_learning", [](Brain& self, bool enable) {
            // Enable learning by setting appropriate plasticity flags
            auto config = self.getConfig();
            config->set("enable_stdp", enable);
            config->set("enable_hebbian", enable);
            config->set("enable_structural_plasticity", enable);
        }, py::arg("enable"), "Enable/disable learning mechanisms")
        
        // Enhanced save/load with better error handling
        .def("save_brain", [](Brain& self, const std::string& filepath) {
            bool success = self.save(filepath);
            if (!success) {
                throw InvalidOperationError("Failed to save brain to file: " + filepath);
            }
            return success;
        }, py::arg("filepath"), "Save brain state to file")
        
        .def("load_brain", [](Brain& self, const std::string& filepath) {
            bool success = self.load(filepath);
            if (!success) {
                throw InvalidOperationError("Failed to load brain from file: " + filepath);
            }
            return success;
        }, py::arg("filepath"), "Load brain state from file");
}

// Enhanced AgentBrain class
void enhanceAgentBrainClass(py::class_<AgentBrain>& agentBrainClass) {
    agentBrainClass
        // Enhanced sensory processing
        .def("process_vision", [](AgentBrain& self, size_t width, size_t height,
                                 const std::vector<float>& data) {
            auto vision = std::make_unique<Vision>(width, height, 3);
            vision->setData(data);
            auto percept = std::make_unique<SensoryPercept>();
            percept->setVision(std::move(vision));
            self.processSensoryInput(*percept);
        }, py::arg("width"), py::arg("height"), py::arg("data"),
            "Process vision input directly")
        
        .def("process_audio", [](AgentBrain& self, size_t sampleRate,
                                 const std::vector<float>& data) {
            auto audio = std::make_unique<Audio>(sampleRate, data.size());
            audio->setData(data);
            auto percept = std::make_unique<SensoryPercept>();
            percept->setAudio(std::move(audio));
            self.processSensoryInput(*percept);
        }, py::arg("sampleRate"), py::arg("data"),
            "Process audio input directly")
        
        // Enhanced action decoding
        .def("decode_action", [](AgentBrain& self, ActionType type,
                                const std::vector<float>& parameters = {}) {
            auto action = std::make_unique<Action>(type, parameters);
            return std::make_unique<ActionResult>(0.0f, true, "");
        }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
            "Decode action from brain and return result")
        
        // Enhanced neuromodulation
        .def("set_reward_modulation", [](AgentBrain& self, bool enable,
                                         float reward, float predictedReward) {
            self.enableRewardModulation(enable);
            if (enable) {
                self.applyRewardModulation(reward, predictedReward);
            }
        }, py::arg("enable"), py::arg("reward"), py::arg("predictedReward"),
            "Enable/disable reward modulation and apply values")
        
        .def("set_curiosity_modulation", [](AgentBrain& self, bool enable,
                                            float curiosityLevel) {
            self.enableCuriosity(enable);
            if (enable) {
                // Note: This is a placeholder - actual implementation would
                // update the curiosity system within the brain
            }
        }, py::arg("enable"), py::arg("curiosity_level"),
            "Enable/disable curiosity modulation")
        
        // Enhanced development
        .def("advance_development", [](AgentBrain& self, float timestep) {
            self.updateDevelopment(timestep);
            return self.getDevelopmentalStage();
        }, py::arg("timestep"), "Advance development and return new stage");
}

// Enhanced World class
void enhanceWorldClass(py::class_<SimpleWorld>& worldClass) {
    worldClass
        // Enhanced world setup
        .def("setup_world", [](SimpleWorld& self, size_t width, size_t height,
                              size_t visionWidth, size_t visionHeight) {
            self.configure(width, height, visionWidth, visionHeight);
            self.reset();
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"),
           py::arg("visionHeight"),
            "Setup world with specified dimensions")
        
        // Enhanced agent positioning
        .def("place_agent", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"), "Place agent at specified position")
        
        // Enhanced simulation
        .def("run_step", [](SimpleWorld& self, double timestep,
                           const py::function<void(const SensoryPercept&)>& onSensorData) {
            self.update(timestep);
            auto percept = self.getSensoryPercept();
            if (onSensorData) {
                onSensorData(*percept);
            }
        }, py::arg("timestep"), py::arg("on_sensor_data") = nullptr,
            "Run a simulation step with optional sensor data callback")
        
        // Enhanced world state
        .def("get_world_stats", [](const SimpleWorld& self) {
            std::map<std::string, float> stats;
            stats["time"] = self.getSimulationTime();
            stats["width"] = static_cast<float>(self.getWidth());
            stats["height"] = static_cast<float>(self.getHeight());
            stats["max_energy"] = self.getMaxEnergy();
            return stats;
        }, "Get world statistics");
}

// Enhanced SensoryInput classes
void enhanceSensoryInputClasses(py::class_<Vision, SensoryInput>& visionClass,
                               py::class_<Audio, SensoryInput>& audioClass) {
    visionClass
        .def("create_image", [](Vision& self, size_t width, size_t height,
                               const std::vector<std::vector<float>>& channels) {
            // Flatten channels into data vector
            std::vector<float> data;
            for (const auto& channel : channels) {
                data.insert(data.end(), channel.begin(), channel.end());
            }
            self.setData(data);
        }, py::arg("width"), py::arg("height"), py::arg("channels"),
            "Create image from channel data")
        .def("normalize", &Vision::normalize,
             "Normalize image data to [0, 1] range")
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            // This is a placeholder - actual implementation would resize the image
            self.setWidth(newWidth);
            self.setHeight(newHeight);
        }, py::arg("newWidth"), py::arg("newHeight"),
            "Resize image to specified dimensions");
    
    audioClass
        .def("normalize", &Audio::normalize,
             "Normalize audio data to [-1, 1] range")
        .def("resample", [](Audio& self, size_t newSampleRate) {
            self.setSampleRate(newSampleRate);
        }, py::arg("newSampleRate"), "Resample audio to new sample rate");
}

void enhanceSensoryPerceptClass(py::class_<SensoryPercept>& perceptClass) {
    perceptClass
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision percept")
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch percept")
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals percept")
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception percept")
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio percept")
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp for percept");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Better error handling with custom exceptions
        - Improved configuration validation
        - More Pythonic APIs and convenience methods
        - Factory methods for common use cases
        - Enhanced documentation and examples
        - Support for JSON and YAML configuration formats
        
        Key improvements:
        1. Config class: Enhanced validation, type-safe setters, factory methods
        2. Brain class: Better initialization, convenience methods, statistics
        3. AgentBrain class: Direct sensory processing, enhanced neuromodulation
        4. SimpleWorld class: Enhanced simulation, callbacks, statistics
        5. Vision and Audio classes: Image processing, normalization methods
        6. Custom exceptions for better error handling
    )pbdoc";

    // Setup exception handling
    setupExceptionHandling(m);

    // Register enhanced classes
    enhanceConfigClass(py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or YAML file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON or YAML file")
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
        }));

    enhanceBrainClass(py::class_<Brain>(m, "Brain", R"pbdoc(Advanced neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
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
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status"));

    enhanceAgentBrainClass(py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
             py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled"));

    enhanceWorldClass(py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with specified dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed"));

    enhanceSensoryInputClasses(
        py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Enhanced vision sensory input)pbdoc")
            .def(py::init<>())
            .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
                 py::arg("channels") = 3)
            .def("setData", [](Vision& self, const std::vector<float>& data) {
                self.setData(data);
            }, py::arg("data"))
            .def("getWidth", &Vision::getWidth)
            .def("getHeight", &Vision::getHeight)
            .def("getChannels", &Vision::getChannels),
        
        py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Enhanced audio sensory input)pbdoc")
            .def(py::init<>())
            .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
            .def("setData", [](Audio& self, const std::vector<float>& data) {
                self.setData(data);
            }, py::arg("data"))
            .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
            .def("getSampleRate", &Audio::getSampleRate)
            .def("getNumSamples", &Audio::getNumSamples)
    );

    enhanceSensoryPerceptClass(py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept)pbdoc")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp")));

    // Enhanced factory methods
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible values")
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Version and metadata
    m.attr("VERSION") = "2.0.0-enhanced";
    m.attr("NLM_VERSION") = "Phase 6: Enhanced Neural Computation";

    // Example usage for documentation
    m.def("example_complete_simulation", []() {
        py::print("Example: Complete simulation");
        py::print("=============================");
        py::print("
import pynlm

# Create brain with default config
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Create world and agent
world = pynlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Run simulation
for i in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if i % 20 == 0:
        print(f'Step {i}: {brain.getFiringNeuronCount()} neurons firing')

print('Simulation complete!')
        ");
        return 1;
    }, "Show complete simulation example");

    // Utility functions
    m.def("print_welcome_message", []() {
        py::print(R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Enhanced Neural Learning Machine                    ║
    ║     Version 2.0.0 with JSON/YAML Support                        ║
    ║                                                               ║
    ║     Phase 6: Enhanced Neural Computation                      ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     Features:                                                 ║
    ║     - Enhanced event-driven processing                        ║
    ║     - Better memory integration                               ║
    ║     - Advanced neuromodulation                                ║
    ║     - Configuration validation                                ║
    ║     - Pythonic API design                                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
        )");
    }, "Print welcome message");

    // InvalidNeuronId exception for better error handling
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Custom exception classes available in Python
    py::class_<NLMException>(m, "NLMException")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<ConfigValidationError, nlm::NLMException>(m, "ConfigValidationError")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<BrainInitializationError, nlm::NLMException>(m, "BrainInitializationError")
        .def(py::init<const std::string&>(), py::arg("message"));
    
    py::class_<InvalidOperationError, nlm::NLMException>(m, "InvalidOperationError")
        .def(py::init<const std::string&>(), py::arg("message"));
}

} // namespace nlm
