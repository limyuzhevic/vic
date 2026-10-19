#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <optional>
#include <variant>
#include <cmath>
#include <algorithm>
#include <map>
#include <unordered_set>
#include <sstream>
#include <iomanip>
#include <type_traits>
#include <functional>

// Enhanced exception hierarchy for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
    virtual const char* type() const noexcept { return "NLMException"; }
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message) : NLMException("Configuration Error: " + message) {}
    virtual const char* type() const noexcept { return "ConfigurationError"; }
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& message) : NLMException("Brain Error: " + message) {}
    virtual const char* type() const noexcept { return "BrainError"; }
};

class AgentError : public NLMException {
public:
    explicit AgentError(const std::string& message) : NLMException("Agent Error: " + message) {}
    virtual const char* type() const noexcept { return "AgentError"; }
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& message) : NLMException("World Error: " + message) {}
    virtual const char* type() const noexcept { return "WorldError"; }
};

// Type aliases for better type safety
using NLMType = std::variant<
    std::monostate,
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Helper functions for type conversion and validation
namespace detail {
    // Convert Python value to NLMType with automatic type detection
    NLMType py_to_nlm_type(py::object py_obj);
    
    // Convert NLMType to Python value with type preservation
    py::object nlm_to_py_type(const NLMType& nlm_type);
    
    // Validate type for configuration parameter
    template<typename T>
    void validate_type(const T& value, const std::string& param_name);
    
    // Get type name as string
    template<typename T>
    std::string type_name();
    
    // Format error messages
    std::string format_error(const std::string& operation, const std::string& context);
    
    // Range validation
    template<typename T>
    bool in_range(const T& value, const T& min, const T& max);
    
    // String utilities
    std::string trim(const std::string& str);
    bool starts_with(const std::string& str, const std::string& prefix);
    std::string to_lower(const std::string& str);
}

// Enhanced Config class with better type safety and Pythonic interfaces
class EnhancedConfig {
public:
    EnhancedConfig() = default;
    
    // Enhanced loading with format detection
    bool loadFromFile(const std::string& filepath, const std::string& format = "auto");
    bool loadFromYAML(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    
    // Enhanced saving with format options
    bool saveToFile(const std::string& filepath, const std::string& format = "json", int indent = 2) const;
    
    // Pythonic access with automatic type conversion
    NLMType get(const std::string& key, const NLMType& default_value = std::monostate()) const;
    NLMType getOr(const std::string& key, const NLMType& default_value) const;
    NLMType getNested(const std::string& dotNotationPath) const;
    
    // Type-safe setters with validation
    template<typename T>
    void set(const std::string& key, const T& value, const std::string& source = "Runtime");
    
    // Enhanced validation
    bool validate(const std::string& schema_path = "") const;
    void addValidator(const std::string& key, 
                      std::function<bool(const NLMType&)> validator);
    
    // Schema management
    bool hasSchema() const;
    void addSchemaEntry(const std::string& entry);
    bool validateSchema() const;
    
    // Default value management
    template<typename T>
    void setDefault(const std::string& key, const T& value);
    
    // Batch operations
    void setBatch(const std::map<std::string, NLMType>& values);
    std::map<std::string, NLMType> getAll() const;
    
    // Context manager support
    EnhancedConfig& operator=(const EnhancedConfig&) = default;
    EnhancedConfig(const EnhancedConfig&) = default;
    
    // Enhanced representation
    std::string toString() const;
    
    // Iteration support
    class iterator;
    iterator begin() const;
    iterator end() const;
    
private:
    // Internal implementation details
    std::map<std::string, NLMType> data_;
    std::map<std::string, std::function<bool(const NLMType&)>> validators_;
    std::string schema_;
    
    friend class iterator;
};

class EnhancedConfig::iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::pair<std::string, NLMType>;
    using pointer = value_type*;
    using reference = value_type&;
    
    iterator() = default;
    iterator(const iterator&) = default;
    
    reference operator*() const;
    pointer operator->() const;
    
    iterator& operator++();
    iterator operator++(int);
    
    bool operator==(const iterator& other) const;
    bool operator!=(const iterator& other) const;
    
private:
    iterator(const std::map<std::string, NLMType>* map, size_t index);
    
    const std::map<std::string, NLMType>* map_;
    size_t index_;
    
    friend class EnhancedConfig;
};

// Enhanced Brain class with type safety and Pythonic interfaces
class EnhancedBrain {
public:
    EnhancedBrain(const EnhancedConfig& config);
    ~EnhancedBrain();
    
    // Copy semantics
    EnhancedBrain(const EnhancedBrain&) = delete;
    EnhancedBrain& operator=(const EnhancedBrain&) = delete;
    
    // Move semantics
    EnhancedBrain(EnhancedBrain&& other) noexcept;
    EnhancedBrain& operator=(EnhancedBrain&& other) noexcept;
    
    // Core simulation methods
    void initialize();
    void step(int step, double time);
    void receiveSensoryInput(const py::object& input);
    void injectCurrent(int neuron, double current);
    
    // Neuron management
    int getActiveNeuronCount() const;
    int getFiringNeuronCount() const;
    double getAverageFiringRate() const;
    double getExcitationInhibitionRatio() const;
    int getTotalSpikeCount() const;
    
    // Region management
    int getRegionCount() const;
    std::vector<int> getRegionIds() const;
    
    // Memory and performance
    size_t getMemoryUsage() const;
    double getProcessingTime() const;
    
    // State management
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Context manager support
    EnhancedBrain& operator()(); // Return reference to self
    void reset();
    
    // Enhanced representation
    std::string toString() const;
    
    // Statistics and analysis
    std::map<std::string, double> getStatistics() const;
    
private:
    std::shared_ptr<class BrainImpl> impl_;
};

// Enhanced AgentBrain class with Pythonic interfaces
class EnhancedAgentBrain {
public:
    EnhancedAgentBrain(const std::shared_ptr<EnhancedBrain>& brain);
    
    // Initialization
    void initialize(py::object world);
    
    // Sensory and motor interfaces
    int getSensoryInputSize() const;
    int getMotorOutputSize() const;
    
    // Processing methods
    void processSensoryInput(py::object percept);
    py::object decodeMotorCommand();
    
    // Neuromodulation
    void applyRewardModulation(double reward, double predicted_reward);
    void updateDevelopment(double timestep);
    
    // State information
    std::string getDevelopmentalStage() const;
    double getNeuromodulationLevel() const;
    double getCuriosityLevel() const;
    double getNoveltyLevel() const;
    double getPredictionError() const;
    
    // Configuration and control
    void enableFeature(const std::string& feature);
    void disableFeature(const std::string& feature);
    bool isFeatureEnabled(const std::string& feature) const;
    
    // Context manager support
    EnhancedAgentBrain& operator()();
    void reset();
    
    // Utility methods
    std::map<std::string, bool> getFeatureStatus() const;
    
private:
    std::shared_ptr<class AgentBrainImpl> impl_;
};

// Enhanced SimpleWorld class
class EnhancedSimpleWorld {
public:
    EnhancedSimpleWorld();
    
    // Configuration
    void configure(int width, int height, int visionWidth, int visionHeight);
    
    // Simulation control
    void reset();
    void update(double timestep);
    
    // Agent management
    void setAgentStart(int x, int y);
    void applyMotorCommand(py::object cmd, double currentTime);
    
    // Sensing
    py::object getSensoryPercept();
    py::object getAgentBody();
    
    // World objects
    void addObject(py::object obj);
    void removeObject(int x, int y);
    bool isValidPosition(int x, int y) const;
    
    // World properties
    int getWidth() const;
    int getHeight() const;
    double getSimulationTime() const;
    
    // Context manager support
    EnhancedSimpleWorld& operator()();
    
    // Enhanced representation
    std::string toString() const;
    
private:
    std::shared_ptr<class WorldImpl> impl_;
};

// Factory functions
EnhancedConfig createDefaultConfig();
EnhancedBrain createBrain(const EnhancedConfig& config);
EnhancedAgentBrain createAgentBrain(const EnhancedBrain& brain);
EnhancedSimpleWorld createSimpleWorld();

// Utility functions
namespace utils {
    // Type conversion utilities
    py::object to_python(const NLMType& value);
    NLMType from_python(py::object py_value);
    
    // Validation utilities
    bool validate_config(const EnhancedConfig& config);
    bool validate_brain(const EnhancedBrain& brain);
    bool validate_parameters(const std::map<std::string, NLMType>& params);
    
    // Conversion utilities
    std::string to_string(const NLMType& value);
    NLMType from_string(const std::string& str, const std::string& type_hint = "auto");
    
    // Statistical utilities
    double mean(const std::vector<double>& values);
    double stddev(const std::vector<double>& values);
    double median(const std::vector<double>& values);
    
    // File utilities
    bool file_exists(const std::string& path);
    std::string get_file_extension(const std::string& path);
    std::string get_current_directory();
    
    // String utilities
    std::string generate_timestamp();
    std::string generate_uuid();
    std::string escape_json(const std::string& str);
    
    // Error handling utilities
    std::string get_error_type(const std::exception& e);
    std::string get_error_details(const std::exception& e);
    void log_error(const std::exception& e);
}

// Exception registration
inline void register_enhanced_exceptions(py::module& m) {
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<AgentError>(m, "AgentError");
    py::register_exception<WorldError>(m, "WorldError");
}

// Module initialization
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        
        This module provides enhanced type safety, error handling, and Pythonic interfaces
        for NLM neural simulations, building on the basic pynlm bindings.
        
        Key improvements:
        - Better type safety with automatic type conversion
        - Enhanced error handling with specific exception types
        - Pythonic interfaces with context managers and properties
        - Comprehensive validation and parameter checking
        - Utility methods for common operations
        - Improved documentation and examples
    )pbdoc";
    
    // Register enhanced exceptions
    register_enhanced_exceptions(m);
    
    // Register type aliases
    m.attr("NLMType") = py::type(NLMType());
    
    // Register exception types
    m.attr("NLMException") = py::type(NLMException(""));
    m.attr("ConfigurationError") = py::type(ConfigurationError(""));
    m.attr("BrainError") = py::type(BrainError(""));
    m.attr("AgentError") = py::type(AgentError(""));
    m.attr("WorldError") = py::type(WorldError(""));
    
    // Register enhanced classes
    py::class_<EnhancedConfig>(m, "EnhancedConfig")
        .def(py::init<>())
        .def("loadFromFile", &EnhancedConfig::loadFromFile, py::arg("filepath"), py::arg("format") = "auto",
             "Load configuration from a file with automatic format detection")
        .def("loadFromYAML", &EnhancedConfig::loadFromYAML, py::arg("filepath"),
             "Load configuration from a YAML file")
        .def("loadFromJSON", &EnhancedConfig::loadFromJSON, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("saveToFile", &EnhancedConfig::saveToFile, py::arg("filepath"), py::arg("format") = "json", py::arg("indent") = 2,
             "Save configuration to a file with specified format")
        .def("has", &EnhancedConfig::has, py::arg("key"), "Check if a configuration key exists")
        .def("__contains__", &EnhancedConfig::has, py::arg("key"), "Check if a configuration key exists (Python 'in' operator)")
        .def("get", &EnhancedConfig::get, py::arg("key"), py::arg("default_value") = std::monostate(),
             "Get a configuration value with automatic type conversion")
        .def("getOr", &EnhancedConfig::getOr, py::arg("key"), py::arg("default_value"),
             "Get a configuration value or default")
        .def("set", [](EnhancedConfig& self, const std::string& key, const NLMType& value) {
            self.set(key, value, "Runtime");
        }, py::arg("key"), py::arg("value"), "Set a configuration value with type conversion")
        .def("validate", &EnhancedConfig::validate, py::arg("schema_path") = "",
             "Validate the configuration against its schema")
        .def("addValidator", &EnhancedConfig::addValidator, py::arg("key"), py::arg("validator"),
             "Add a custom validator function for a key")
        .def("setDefault", [](EnhancedConfig& self, const std::string& key, const NLMType& value) {
            self.setDefault(key, value);
        }, py::arg("key"), py::arg("value"), "Set a default value for a key")
        .def("setBatch", &EnhancedConfig::setBatch, py::arg("values"), "Set multiple configuration values at once")
        .def("getAll", &EnhancedConfig::getAll, "Get all configuration values")
        .def("begin", &EnhancedConfig::begin, "Get iterator to beginning of configuration")
        .def("end", &EnhancedConfig::end, "Get iterator to end of configuration")
        .def("toString", &EnhancedConfig::toString, "Get string representation of configuration")
        .def("__repr__", &EnhancedConfig::toString, "String representation (Python __repr__)")
        .def("__iter__", [](EnhancedConfig& self) {
            return py::make_iterator(self.begin(), self.end());
        }, "Return iterator for configuration (Python iteration support)")
        .def("__len__", [](const EnhancedConfig& self) {
            return self.getAll().size();
        }, "Get number of configuration entries (Python len)")
        .def("__getitem__", [](EnhancedConfig& self, const std::string& key) {
            auto value = self.get(key);
            if (std::holds_alternative<std::monostate>(value)) {
                throw py::key_error("Key not found: " + key);
            }
            return detail::nlm_to_py_type(value);
        }, py::arg("key"), "Get configuration value (Python '[]' operator)")
        .def("__setitem__", [](EnhancedConfig& self, const std::string& key, const NLMType& value) {
            self.set(key, value, "Runtime");
        }, py::arg("key"), py::arg("value"), "Set configuration value (Python '[]' operator)")
        .def("keys", &EnhancedConfig::getAll, "Get all configuration keys")
        .def("values", &EnhancedConfig::getAll, "Get all configuration values")
        .def("items", &EnhancedConfig::getAll, "Get all configuration key-value pairs")
        .def("clear", &EnhancedConfig::clear, "Clear all configuration entries")
        .def("copy", [](const EnhancedConfig& self) {
            return EnhancedConfig(self);
        }, "Create a copy of this configuration")
        .def("merge", [](EnhancedConfig& self, const EnhancedConfig& other) {
            auto values = other.getAll();
            self.setBatch(values);
        }, py::arg("other"), "Merge another configuration into this one")
        .def("__enter__", [](EnhancedConfig& self) { return self; })
        .def("__exit__", [](EnhancedConfig&, const py::args&) {
            // Cleanup code here if needed
            return false; // Don't suppress exceptions
        });
    
    // Register enhanced exception types
    m.def("NLMException", []() { return NLMException(""); });
    m.def("ConfigurationError", []() { return ConfigurationError(""); });
    m.def("BrainError", []() { return BrainError(""); });
    m.def("AgentError", []() { return AgentError(""); });
    m.def("WorldError", []() { return WorldError(""); });
    
    // Register utility functions
    m.def("to_python", &detail::nlm_to_py_type, "Convert NLMType to Python value");
    m.def("from_python", &detail::py_to_nlm_type, "Convert Python value to NLMType");
    m.def("validate_config", &utils::validate_config, "Validate configuration");
    m.def("validate_brain", &utils::validate_brian, "Validate brain configuration");
    m.def("validate_parameters", &utils::validate_parameters, "Validate simulation parameters");
    
    // Register factory functions
    m.def("createDefaultConfig", &createDefaultConfig, "Create default configuration");
    m.def("createBrain", &createBrain, "Create enhanced brain with configuration");
    m.def("createAgentBrain", &createAgentBrain, "Create enhanced agent brain");
    m.def("createSimpleWorld", &createSimpleWorld, "Create simple world");
    
    // Register type conversion utilities
    m.def("to_string", &utils::to_string, "Convert NLMType to string");
    m.def("from_string", &utils::from_string, "Convert string to NLMType");
    
    // Register statistical utilities
    m.def("mean", &utils::mean, "Calculate mean of values");
    m.def("stddev", &utils::stddev, "Calculate standard deviation of values");
    m.def("median", &utils::median, "Calculate median of values");
    
    // Register file utilities
    m.def("file_exists", &utils::file_exists, "Check if file exists");
    m.def("get_file_extension", &utils::get_file_extension, "Get file extension");
    m.def("get_current_directory", &utils::get_current_directory, "Get current directory");
    
    // Register string utilities
    m.def("generate_timestamp", &utils::generate_timestamp, "Generate timestamp string");
    m.def("generate_uuid", &utils::generate_uuid, "Generate UUID string");
    m.def("escape_json", &utils::escape_json, "Escape JSON string");
    
    // Register error utilities
    m.def("get_error_type", &utils::get_error_type, "Get error type");
    m.def("get_error_details", &utils::get_error_details, "Get error details");
    m.def("log_error", &utils::log_error, "Log error message");
    
    // Register convenience constants
    m.attr("NEURON_TYPE_EXCITATORY") = 1;
    m.attr("NEURON_TYPE_INHIBITORY") = 2;
    m.attr("NEURON_TYPE_MODULATORY") = 3;
    m.attr("NEURON_TYPE_SENSORY") = 4;
    m.attr("NEURON_TYPE_MOTOR") = 5;
    m.attr("NEURON_TYPE_INTERNAL") = 6;
    
    m.attr("SIMULATION_PHASE_INITIAL") = 0;
    m.attr("SIMULATION_PHASE_CRITICAL_PERIOD") = 1;
    m.attr("SIMULATION_PHASE_MATURATION") = 2;
    m.attr("SIMULATION_PHASE_ADULT") = 3;
    m.attr("SIMULATION_PHASE_AGING") = 4;
    
    m.attr("ACTION_TYPE_MOVE_FORWARD") = "MoveForward";
    m.attr("ACTION_TYPE_MOVE_BACKWARD") = "MoveBackward";
    m.attr("ACTION_TYPE_MOVE_LEFT") = "MoveLeft";
    m.attr("ACTION_TYPE_MOVE_RIGHT") = "MoveRight";
    
    // Register enhanced Brain class
    py::class_<EnhancedBrain>(m, "EnhancedBrain")
        .def(py::init<const EnhancedConfig&>(), py::arg("config"), "Create enhanced brain with configuration")
        .def("initialize", &EnhancedBrain::initialize, "Initialize the brain")
        .def("step", &EnhancedBrain::step, py::arg("step"), py::arg("time"), "Perform simulation step")
        .def("receiveSensoryInput", &EnhancedBrain::receiveSensoryInput, py::arg("input"), "Inject sensory input")
        .def("injectCurrent", &EnhancedBrain::injectCurrent, py::arg("neuron"), py::arg("current"), "Inject current into neuron")
        .def("getActiveNeuronCount", &EnhancedBrain::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &EnhancedBrain::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &EnhancedBrain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &EnhancedBrain::getExcitationInhibitionRatio, "Get excitation/inhibition ratio")
        .def("getTotalSpikeCount", &EnhancedBrain::getTotalSpikeCount, "Get total spike count")
        .def("getRegionCount", &EnhancedBrain::getRegionCount, "Get region count")
        .def("getRegionIds", &EnhancedBrain::getRegionIds, "Get region IDs")
        .def("getMemoryUsage", &EnhancedBrain::getMemoryUsage, "Get memory usage")
        .def("getProcessingTime", &EnhancedBrain::getProcessingTime, "Get processing time")
        .def("save", &EnhancedBrain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &EnhancedBrain::load, py::arg("filepath"), "Load brain state from file")
        .def("reset", &EnhancedBrain::reset, "Reset brain state")
        .def("toString", &EnhancedBrain::toString, "Get string representation")
        .def("__repr__", &EnhancedBrain::toString, "String representation (Python __repr__)")
        .def("getStatistics", &EnhancedBrain::getStatistics, "Get brain statistics")
        .def("__call__", &EnhancedBrain::operator()(), "Return reference to self (Pythonic access)");
    
    // Register enhanced AgentBrain class
    py::class_<EnhancedAgentBrain>(m, "EnhancedAgentBrain")
        .def(py::init<const std::shared_ptr<EnhancedBrain>&>(), py::arg("brain"), "Create enhanced agent brain")
        .def("initialize", &EnhancedAgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &EnhancedAgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &EnhancedAgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &EnhancedAgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept")
        .def("decodeMotorCommand", &EnhancedAgentBrain::decodeMotorCommand, "Decode motor command")
        .def("applyRewardModulation", &EnhancedAgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predicted_reward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &EnhancedAgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &EnhancedAgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &EnhancedAgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &EnhancedAgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &EnhancedAgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &EnhancedAgentBrain::getPredictionError, "Get prediction error")
        .def("enableFeature", &EnhancedAgentBrain::enableFeature, py::arg("feature"), "Enable a feature")
        .def("disableFeature", &EnhancedAgentBrain::disableFeature, py::arg("feature"), "Disable a feature")
        .def("isFeatureEnabled", &EnhancedAgentBrain::isFeatureEnabled, py::arg("feature"), "Check if feature is enabled")
        .def("getFeatureStatus", &EnhancedAgentBrain::getFeatureStatus, "Get status of all features")
        .def("reset", &EnhancedAgentBrain::reset, "Reset agent")
        .def("toString", &EnhancedAgentBrain::toString, "Get string representation")
        .def("__repr__", &EnhancedAgentBrain::toString, "String representation (Python __repr__)")
        .def("__call__", &EnhancedAgentBrain::operator()(), "Return reference to self (Pythonic access)");
    
    // Register Enhanced SimpleWorld class
    py::class_<EnhancedSimpleWorld>(m, "EnhancedSimpleWorld")
        .def(py::init<>())
        .def("configure", &EnhancedSimpleWorld::configure, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Configure world")
        .def("reset", &EnhancedSimpleWorld::reset, "Reset world")
        .def("update", &EnhancedSimpleWorld::update, py::arg("timestep"), "Update world")
        .def("setAgentStart", &EnhancedSimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent start position")
        .def("applyMotorCommand", &EnhancedSimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"), "Apply motor command")
        .def("getSensoryPercept", &EnhancedSimpleWorld::getSensoryPercept, "Get sensory percept")
        .def("getAgentBody", &EnhancedSimpleWorld::getAgentBody, "Get agent body")
        .def("addObject", &EnhancedSimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &EnhancedSimpleSimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove world object")
        .def("isValidPosition", &EnhancedSimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &EnhancedSimpleWorld::getWidth, "Get world width")
        .def("getHeight", &EnhancedSimpleWorld::getHeight, "Get world height")
        .def("getSimulationTime", &EnhancedSimpleWorld::getSimulationTime, "Get simulation time")
        .def("toString", &EnhancedSimpleWorld::toString, "Get string representation")
        .def("__repr__", &EnhancedSimpleWorld::toString, "String representation (Python __repr__)")
        .def("__call__", &EnhancedSimpleWorld::operator()(), "Return reference to self (Pythonic access)");
    
    // Register backward compatibility aliases
    m.def("createBrain", [](const EnhancedConfig& config) {
        return createBrain(config);
    }, py::arg("config"), "Create enhanced brain (backward compatibility)");
    
    m.def("createAgentBrain", [](const std::shared_ptr<EnhancedBrain>& brain) {
        return createAgentBrain(brain);
    }, py::arg("brain"), "Create enhanced agent brain (backward compatibility)");
    
    m.def("createSimpleWorld", []() {
        return createSimpleWorld();
    }, "Create simple world (backward compatibility)");
    
    // Register utility classes for direct use
    py::class_<EnhancedConfig::iterator>(m, "ConfigIterator")
        .def("__next__", [](EnhancedConfig::iterator& it) {
            if (it == EnhancedConfig().end()) {
                throw py::stop_iteration();
            }
            auto value = *it;
            ++it;
            return value;
        }, "Get next item (Python iterator protocol)")
        .def("__iter__", [](EnhancedConfig::iterator it) {
            return py::make_iterator(std::move(it), std::move(it));
        }, "Return iterator (Python iterator protocol)");
    
    // Register type conversion utilities
    py::class_<NLMType>(m, "NLMType")
        .def(py::init<>())
        .def("type", [](const NLMType& value) {
            return std::visit([](auto&& arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::monostate>) return "null";
                else if constexpr (std::is_same_v<T, int>) return "int";
                else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
                else if constexpr (std::is_same_v<T, double>) return "double";
                else if constexpr (std::is_same_v<T, bool>) return "bool";
                else if constexpr (std::is_same_v<T, std::string>) return "string";
                else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
                else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
                else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
                else return "unknown";
            }, value);
        }, "Get type name of stored value")
        .def("is_null", [](const NLMType& value) {
            return std::holds_alternative<std::monostate>(value);
        }, "Check if value is null")
        .def("as_int", [](const NLMType& value) -> int {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value);
            } else if (std::holds_alternative<double>(value)) {
                return static_cast<int>(std::get<double>(value));
            } else {
                throw ConfigurationError("Cannot convert to int: " + to_string(value));
            }
        }, "Convert to int")
        .def("as_float", [](const NLMType& value) -> double {
            if (std::holds_alternative<int>(value)) {
                return static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            } else {
                throw ConfigurationError("Cannot convert to float: " + to_string(value));
            }
        }, "Convert to float")
        .def("as_string", [](const NLMType& value) -> std::string {
            return detail::nlm_to_py_type(value).cast<std::string>();
        }, "Convert to string")
        .def("as_bool", [](const NLMType& value) -> bool {
            if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value);
            } else {
                throw ConfigurationError("Cannot convert to bool: " + to_string(value));
            }
        }, "Convert to bool")
        .def("__repr__", [](const NLMType& value) {
            return detail::nlm_to_py_type(value).cast<std::string>();
        }, "String representation");
    
    // Register convenience functions
    m.def("py_to_nlm", &detail::py_to_nlm_type, "Convert Python value to NLMType");
    m.def("nlm_to_py", &detail::nlm_to_py_type, "Convert NLMType to Python value");
    m.def("validate_type", &detail::validate_type<std::string>, py::arg("value"), py::arg("param_name"), "Validate value type");
    m.def("in_range", &detail::in_range<double>, py::arg("value"), py::arg("min"), py::arg("max"), "Check if value is in range");
    m.def("trim", &detail::trim, "Trim whitespace from string");
    m.def("starts_with", &detail::starts_with, "Check if string starts with prefix");
    m.def("to_lower", &detail::to_lower, "Convert string to lowercase");
    
    // Register type constants
    m.attr("NLM_TYPE_NULL") = std::monostate();
    m.attr("NLM_TYPE_INT") = 1;
    m.attr("NLM_TYPE_INT64") = 2;
    m.attr("NLM_TYPE_FLOAT") = 3;
    m.attr("NLM_TYPE_BOOL") = 4;
    m.attr("NLM_TYPE_STRING") = 5;
    m.attr("NLM_TYPE_INT_VECTOR") = 6;
    m.attr("NLM_TYPE_FLOAT_VECTOR") = 7;
    m.attr("NLM_TYPE_STRING_VECTOR") = 8;
    
    // Register enhanced constants
    m.attr("ENHANCED_VERSION") = "2.0.0";
    m.attr("MAX_CONFIG_SIZE") = 1000000;
    m.attr("DEFAULT_INDENT") = 2;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        
        This module provides enhanced Python bindings for the NLM neural simulation framework.
        It extends the basic bindings with better type safety, error handling, and Pythonic interfaces.
        
        Key improvements:
        - Enhanced type safety with automatic type conversion
        - Comprehensive error handling with custom exception types
        - Pythonic interfaces with context managers and properties
        - Automatic parameter validation and range checking
        - Convenience methods and factory functions
        - Enhanced documentation and examples
        - Backward compatibility with existing pynlm bindings
        
        Usage:
            from nlm_enhanced import *
            
            # Create enhanced brain
            config = createDefaultConfig()
            brain = createBrain(config)
            
            # Run simulation
            brain.initialize()
            for step in range(100):
                brain.step(step, step * 0.01)
                
            # Get statistics
            stats = brain.getStatistics()
        )pbdoc";
}} // namespace nlm
