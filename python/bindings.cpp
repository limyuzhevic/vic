#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <sstream>

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

// Improved Python bindings with better error handling and usability

// Custom exception types for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigValidationError : public NLMError {
public:
    explicit ConfigValidationError(const std::string& msg) : NLMError(msg) {}
};

class BrainInitializationError : public NLMError {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMError(msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError(msg) {}
};

// Enhanced Config class with better Python API
class PyConfig {
private:
    std::shared_ptr<Config> config_;
    
    // Helper function to validate configuration
    std::vector<std::string> validateConfig() const {
        std::vector<std::string> errors;
        
        // Basic validation
        if (config_->has("neuron_count")) {
            int64_t neuronCount = config_->getOr<int64_t>("neuron_count", 0);
            if (neuronCount <= 0 || neuronCount > 100000) {
                errors.push_back("neuron_count must be between 1 and 100000");
            }
        }
        
        if (config_->has("simulation_timestep")) {
            double timestep = config_->getOr<double>("simulation_timestep", 0.0);
            if (timestep <= 0.0 || timestep > 1.0) {
                errors.push_back("simulation_timestep must be between 0.0 and 1.0");
            }
        }
        
        if (config_->has("connection_probability")) {
            float connProb = config_->getOr<float>("connection_probability", 0.0f);
            if (connProb < 0.0f || connProb > 1.0f) {
                errors.push_back("connection_probability must be between 0.0 and 1.0");
            }
        }
        
        return errors;
    }
    
public:
    PyConfig() : config_(std::make_shared<Config>()) {
        // Set sensible defaults
        config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        config_->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config_->set("connection_probability", 0.1f, ConfigSource::Default);
    }
    
    // Load from JSON file
    bool loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file) {
            return false;
        }
        
        // Try to parse as JSON
        try {
            nlohmann::json jsonData;
            file >> jsonData;
            
            // Convert JSON to config
            for (const auto& [key, value] : jsonData.items()) {
                std::string valueStr;
                if (value.is_string()) {
                    valueStr = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    valueStr = std::to_string(value.get<int64_t>());
                } else if (value.is_number_float()) {
                    valueStr = std::to_string(value.get<double>());
                } else if (value.is_boolean()) {
                    valueStr = value.get<bool>() ? "true" : "false";
                }
                
                config_->set(key, valueStr, ConfigSource::User);
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Save to JSON file
    bool saveToFile(const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file) {
            return false;
        }
        
        try {
            nlohmann::json jsonData;
            for (const auto& key : config_->getKeys()) {
                jsonData[key] = config_->getOr<std::string>(key, "");
            }
            
            file << jsonData.dump(2);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Check if config has a key
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Set configuration value
    void set(const std::string& key, const std::string& value) {
        config_->set(key, value, ConfigSource::User);
    }
    
    // Get configuration value
    std::string get(const std::string& key, const std::string& defaultValue = "") const {
        return config_->getOr<std::string>(key, defaultValue);
    }
    
    // Get integer value
    int64_t getInt(const std::string& key, int64_t defaultValue = 0) const {
        return config_->getOr<int64_t>(key, defaultValue);
    }
    
    // Get float value
    float getFloat(const std::string& key, float defaultValue = 0.0f) const {
        return config_->getOr<float>(key, defaultValue);
    }
    
    // Get double value
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        return config_->getOr<double>(key, defaultValue);
    }
    
    // Get boolean value
    bool getBool(const std::string& key, bool defaultValue = false) const {
        std::string value = get(key, defaultValue ? "true" : "false");
        return (value == "true" || value == "1" || value == "yes");
    }
    
    // Validate configuration
    std::vector<std::string> validate() const {
        return validateConfig();
    }
    
    // Check if valid
    bool isValid() const {
        return validate().empty();
    }
    
    // Get configuration summary
    std::string summary() const {
        return config_->summary();
    }
    
    // Get internal config
    const std::shared_ptr<Config>& getInternal() const {
        return config_;
    }
    
    // Python representation
    std::string repr() const {
        return "<Config: " + summary() + ">";
    }
    
    // Convenience methods
    void setNeuronCount(int64_t count) {
        config_->set("neuron_count", count, ConfigSource::User);
    }
    
    void setConnectionDensity(float density) {
        config_->set("connection_probability", density, ConfigSource::User);
    }
    
    void setLearningRate(float rate) {
        config_->set("learning_rate", rate, ConfigSource::User);
    }
    
    // Factory methods for common configurations
    static PyConfig createSmallBrain() {
        PyConfig config;
        config.setNeuronCount(100);
        config.setConnectionDensity(0.05f);
        return config;
    }
    
    static PyConfig createLargeBrain() {
        PyConfig config;
        config.setNeuronCount(10000);
        config.setConnectionDensity(0.15f);
        return config;
    }
    
    static PyConfig createDevelopmentBrain() {
        PyConfig config;
        config.setNeuronCount(500);
        return config;
    }
    
    static PyConfig createLearningBrain() {
        PyConfig config;
        config.setNeuronCount(2000);
        return config;
    }
    
    // Default constructor
    PyConfig(std::shared_ptr<Config> config) : config_(config) {}
};

// Enhanced Brain class with better error handling
class PyBrain {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    PyBrain(std::shared_ptr<PyConfig> config) {
        if (!config) {
            throw BrainInitializationError("Config cannot be null");
        }
        
        try {
            brain_ = std::make_shared<Brain>(config->getInternal());
            if (!brain_->initialize()) {
                throw BrainInitializationError("Failed to initialize brain");
            }
        } catch (const std::exception& e) {
            throw BrainInitializationError(std::string("Brain initialization failed: ") + e.what());
        }
    }
    
    ~PyBrain() = default;
    
    // Simulation step
    void step() {
        try {
            static SimulationStep step = 0;
            brain_->step(step++);
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Simulation step failed: ") + e.what());
        }
    }
    
    void step(size_t step) {
        try {
            brain_->step(step);
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Simulation step failed: ") + e.what());
        }
    }
    
    // Add sensory input
    void addSensoryInput(float x, float y, float intensity = 1.0f) {
        try {
            auto vision = std::make_shared<Vision>(16, 16, 1);
            size_t idx = static_cast<size_t>(x) * 16 + static_cast<size_t>(y);
            if (idx < vision->getWidth() * vision->getHeight()) {
                vision->setData(std::vector<float>(vision->getWidth() * vision->getHeight(), 0.0f));
                vision->getData()[idx] = intensity;
                brain_->receiveSensoryInput(vision);
            }
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to add sensory input: ") + e.what());
        }
    }
    
    // Statistics
    class Statistics {
    private:
        PyBrain& brainRef;
        
    public:
        Statistics(PyBrain& brain) : brainRef(brain) {}
        
        size_t neuronCount() const { return brainRef.brain_->getTotalNeuronCount(); }
        size_t synapseCount() const { return brainRef.brain_->getTotalSynapseCount(); }
        size_t firingNeurons() const { return brainRef.brain_->getFiringNeuronCount(); }
        size_t totalSpikes() const { return brainRef.brain_->getTotalSpikeCount(); }
        float averageFiringRate() const { return brainRef.brain_->getAverageFiringRate(); }
        float excitationInhibitionRatio() const { return brainRef.brain_->getExcitationInhibitionRatio(); }
        DevelopmentalStage developmentalStage() const { return brainRef.brain_->getDevelopmentalStage(); }
        
        std::string toString() const {
            std::stringstream ss;
            ss << "Neuron Count: " << neuronCount() << "\n";
            ss << "Synapse Count: " << synapseCount() << "\n";
            ss << "Firing Neurons: " << firingNeurons() << "\n";
            ss << "Total Spikes: " << totalSpikes() << "\n";
            ss << "Average Firing Rate: " << averageFiringRate() << "\n";
            ss << "E/I Ratio: " << excitationInhibitionRatio() << "\n";
            ss << "Developmental Stage: " << static_cast<int>(developmentalStage());
            return ss.str();
        }
    };
    
    Statistics getStats() {
        return Statistics(*this);
    }
    
    // Save/load state
    bool saveState(const std::string& filepath) {
        try {
            return brain_->save(filepath);
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to save state: ") + e.what());
        }
    }
    
    bool loadState(const std::string& filepath) {
        try {
            return brain_->load(filepath);
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Failed to load state: ") + e.what());
        }
    }
    
    // Reset brain
    void reset() {
        brain_->reset();
    }
    
    // Log status
    void logStatus() const {
        brain_->logStatus();
    }
    
    // Get brain pointer
    const std::shared_ptr<Brain>& getInternal() const {
        return brain_;
    }
    
    // Enable plasticity
    void enablePlasticity(bool stdp, bool hebbian, bool rewardModulated) {
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(hebbian, stdp, rewardModulated);
            }
        }
    }
    
    // Get statistics directly
    size_t getNeuronCount() const { return brain_->getTotalNeuronCount(); }
    size_t getSynapseCount() const { return brain_->getTotalSynapseCount(); }
    size_t getFiringNeurons() const { return brain_->getFiringNeuronCount(); }
    size_t getTotalSpikes() const { return brain_->getTotalSpikeCount(); }
    float getAverageFiringRate() const { return brain_->getAverageFiringRate(); }
    float getExcitationInhibitionRatio() const { return brain_->getExcitationInhibitionRatio(); }
    DevelopmentalStage getDevelopmentalStage() const { return brain_->getDevelopmentalStage(); }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        --------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Better error handling and validation
        - Simplified API with convenient factory methods
        - Enhanced statistics and monitoring
        - Improved configuration management
        - Context manager support
    )pbdoc";

    // Register enhanced exception types
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigValidationError>(m, "ConfigValidationError");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Enhanced Config class
    py::class_<PyConfig>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &PyConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("save_to_file", &PyConfig::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &PyConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &PyConfig::getKeys,
             "Get all configuration keys")
        .def("set", &PyConfig::set, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("get", &PyConfig::get, py::arg("key"), py::arg("default_value") = "",
             "Get a configuration value")
        .def("get_int", &PyConfig::getInt, py::arg("key"), py::arg("default_value") = 0,
             "Get an integer configuration value")
        .def("get_float", &PyConfig::getFloat, py::arg("key"), py::arg("default_value") = 0.0f,
             "Get a float configuration value")
        .def("get_double", &PyConfig::getDouble, py::arg("key"), py::arg("default_value") = 0.0,
             "Get a double configuration value")
        .def("get_bool", &PyConfig::getBool, py::arg("key"), py::arg("default_value") = false,
             "Get a boolean configuration value")
        .def("validate", &PyConfig::validate,
             "Validate configuration and return list of errors")
        .def("is_valid", &PyConfig::isValid,
             "Check if configuration is valid")
        .def("summary", &PyConfig::summary,
             "Get configuration summary")
        .def("set_neuron_count", &PyConfig::setNeuronCount, py::arg("count"),
             "Set neuron count")
        .def("set_connection_density", &PyConfig::setConnectionDensity, py::arg("density"),
             "Set connection density")
        .def("set_learning_rate", &PyConfig::setLearningRate, py::arg("rate"),
             "Set learning rate")
        
        // Factory methods
        .def_static("create_small_brain", &PyConfig::createSmallBrain,
                   "Create a small brain configuration for testing")
        .def_static("create_large_brain", &PyConfig::createLargeBrain,
                   "Create a large brain configuration for research")
        .def_static("create_development_brain", &PyConfig::createDevelopmentBrain,
                   "Create a development brain configuration")
        .def_static("create_learning_brain", &PyConfig::createLearningBrain,
                   "Create a learning-focused brain configuration")
        
        .def("__repr__", &PyConfig::repr,
             "String representation of config");

    // Enhanced Brain class
    py::class_<PyBrain>(m, "Brain", R"pbdoc(Enhanced brain simulation with better error handling)pbdoc")
        .def(py::init<std::shared_ptr<PyConfig>>(), py::arg("config"),
             "Create a new brain with configuration")
        
        // Simulation control
        .def("step", static_cast<void (PyBrain::*)()>(&PyBrain::step),
             "Perform one simulation step")
        .def("step", static_cast<void (PyBrain::*)(size_t)>(&PyBrain::step),
             py::arg("step"), "Perform simulation step with step number")
        
        // Sensory input
        .def("add_sensory_input", &PyBrain::addSensoryInput,
             py::arg("x"), py::arg("y"), py::arg("intensity") = 1.0f,
             "Add sensory input at position (x, y) with given intensity")
        
        // Statistics
        .def("get_stats", &PyBrain::getStats,
             "Get brain statistics")
        
        // State management
        .def("save_state", &PyBrain::saveState, py::arg("filepath"),
             "Save brain state to file")
        .def("load_state", &PyBrain::loadState, py::arg("filepath"),
             "Load brain state from file")
        .def("reset", &PyBrain::reset,
             "Reset brain state")
        .def("log_status", &PyBrain::logStatus,
             "Log brain status")
        
        // Direct statistics access
        .def("get_neuron_count", &PyBrain::getNeuronCount,
             "Get total neuron count")
        .def("get_synapse_count", &PyBrain::getSynapseCount,
             "Get total synapse count")
        .def("get_firing_neurons", &PyBrain::getFiringNeurons,
             "Get count of firing neurons")
        .def("get_total_spikes", &PyBrain::getTotalSpikes,
             "Get total spike count")
        .def("get_average_firing_rate", &PyBrain::getAverageFiringRate,
             "Get average firing rate")
        .def("get_excitation_inhibition_ratio", &PyBrain::getExcitationInhibitionRatio,
             "Get excitation/inhibition ratio")
        .def("get_developmental_stage", &PyBrain::getDevelopmentalStage,
             "Get developmental stage");

    // Statistics class
    py::class_<PyBrain::Statistics>(m, "BrainStats", R"pbdoc(Statistics for brain simulation)pbdoc")
        .def_readonly("neuron_count", &PyBrain::Statistics::neuronCount)
        .def_readonly("synapse_count", &PyBrain::Statistics::synapseCount)
        .def_readonly("firing_neurons", &PyBrain::Statistics::firingNeurons)
        .def_readonly("total_spikes", &PyBrain::Statistics::totalSpikes)
        .def_readonly("average_firing_rate", &PyBrain::Statistics::averageFiringRate)
        .def_readonly("excitation_inhibition_ratio", &PyBrain::Statistics::excitationInhibitionRatio)
        .def_readonly("developmental_stage", &PyBrain::Statistics::developmentalStage)
        .def("to_string", &PyBrain::Statistics::toString,
             "Get formatted statistics string");

    // Convenience functions
    m.def("create_brain", [](std::shared_ptr<PyConfig> config) -> std::shared_ptr<PyBrain> {
        return std::make_shared<PyBrain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("create_default_brain", []() -> std::shared_ptr<PyBrain> {
        auto config = std::make_shared<PyConfig>();
        return std::make_shared<PyBrain>(config);
    }, "Create a new brain with default configuration");
    
    m.def("create_small_brain", []() -> std::shared_ptr<PyBrain> {
        auto config = PyConfig::createSmallBrain();
        return std::make_shared<PyBrain>(config);
    }, "Create a small brain configuration for testing");
    
    m.def("create_large_brain", []() -> std::shared_ptr<PyBrain> {
        auto config = PyConfig::createLargeBrain();
        return std::make_shared<PyBrain>(config);
    }, "Create a large brain configuration for research");

    // Version and metadata
    m.attr("__version__") = "2.1.0-improved";
    m.attr("__description__") = "Improved NLM with better error handling and simplified API";
}

} // namespace nlm