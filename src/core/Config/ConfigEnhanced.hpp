#ifndef PYNLM_BINDINGS_HPP
#define PYNLM_BINDINGS_HPP

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

// Include NLM core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Custom exception hierarchy
namespace py = pybind11;
namespace nlm {

    // Base NLM exception
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

    class WorldError : public NLMError {
    public:
        explicit WorldError(const std::string& msg) : NLMError("World Error: " + msg) {}
    };

    class AgentError : public NLMError {
    public:
        explicit AgentError(const std::string& msg) : NLMError("Agent Error: " + msg) {}
    };

    // Enhanced configuration schema
    struct ConfigParameter {
        std::string name;
        std::string type;
        std::string description;
        std::string group;
        std::vector<std::string> valid_values;
        std::pair<double, double> range = {-1e6, 1e6};
        bool required = false;
        std::string default_value;
    };

    // Helper functions for parameter validation
    template<typename T>
    T convert_to_type(const std::string& value_str);

    template<typename T>
    std::string convert_to_string(const T& value);

    // Parameter validation utilities
    class ParameterValidator {
    public:
        static bool validateParameter(const ConfigParameter& param, const std::string& value_str);
        static std::vector<std::string> validateParameter(const ConfigParameter& param, const std::string& value_str, std::string& converted_value);
        static std::map<std::string, std::string> validateConfig(const std::vector<ConfigParameter>& schema, const std::map<std::string, std::string>& config);
        static std::vector<std::string> getValidValues(const ConfigParameter& param);
        static bool isInRange(const ConfigParameter& param, double value);
    };

    // Enhanced Config with schema support
    class ConfigEnhanced {
    private:
        std::shared_ptr<Config> config;
        std::vector<ConfigParameter> schema;

    public:
        ConfigEnhanced();
        explicit ConfigEnhanced(const std::string& filename);

        // Enhanced configuration methods
        bool setParameter(const std::string& key, const std::string& value);
        bool setParameter(const std::string& key, double value);
        bool setParameter(const std::string& key, int64_t value);
        bool setParameter(const std::string& key, bool value);

        // Validation methods
        bool validate();
        bool isValid() const;
        std::vector<std::string> getValidationErrors() const;
        std::vector<std::string> getValidationWarnings() const;
        std::map<std::string, std::string> validateWithSchema(const std::vector<ConfigParameter>& schema);

        // Schema methods
        void setSchema(const std::vector<ConfigParameter>& schema);
        std::vector<std::string> getGroups() const;
        std::vector<std::string> getParametersInGroup(const std::string& group) const;
        std::string getParameterDescription(const std::string& key) const;
        bool hasParameter(const std::string& key) const;
        std::string getParameterType(const std::string& key) const;

        // Template configuration methods
        static ConfigEnhanced createSimpleBrainConfig(size_t neuron_count = 1000);
        static ConfigEnhanced createAgentLearningConfig(size_t neuron_count = 2000);
        static ConfigEnhanced createResearchConfig(size_t neuron_count = 5000);
        static ConfigEnhanced createProductionConfig(size_t neuron_count = 10000);

        // Original methods (for backward compatibility)
        void loadFromFile(const std::string& filepath);
        void loadFromArgs(int argc, char** argv);
        void saveToFile(const std::string& filepath) const;
        bool has(const std::string& key) const;
        std::vector<std::string> getKeys() const;
        void clear();
        std::string summary() const;
        std::string __repr__() const;

        // Operator overloads
        bool operator==(const ConfigEnhanced& other) const;
        bool operator!=(const ConfigEnhanced& other) const;

        // Conversion
        std::shared_ptr<Config> getOriginalConfig() const { return config; }
    };

    // Enhanced Brain with context manager support
    class BrainEnhanced {
    private:
        std::shared_ptr<Brain> brain;
        bool initialized;
        std::string last_error;

    public:
        BrainEnhanced(const std::shared_ptr<Config>& config);
        BrainEnhanced(const std::shared_ptr<ConfigEnhanced>& config);
        ~BrainEnhanced();

        // Context manager support
        BrainEnhanced& operator=(std::shared_ptr<Brain> other_brain);
        BrainEnhanced& operator=(const BrainEnhanced& other);

        // Core functionality with enhanced error handling
        bool initialize();
        bool initialize(const std::map<std::string, std::string>& config_overrides);
        std::string getLastError() const { return last_error; }
        void clearError() { last_error.clear(); }

        // Step methods with enhanced signatures
        void step(size_t currentStep);
        void step(size_t currentStep, double currentTime);
        void step(const std::string& step_label, size_t step_number, double time = 0.0);

        // Enhanced statistics
        std::map<std::string, double> getStatistics() const;
        std::map<std::string, size_t> getCounts() const;
        std::map<std::string, std::string> getDebugInfo() const;

        // Batch operations
        void stepRange(size_t start_step, size_t end_step, double timestep = 0.001);
        void runSimulation(size_t steps, double timestep = 0.001);

        // Enhanced input/output
        void receiveSensoryInput(const class SensoryInput& input);
        void receiveSensoryInput(const std::vector<float>& data);
        void injectCurrent(NeuronId neuron, MembranePotential current);
        void injectCurrentToNeurons(NeuronType type, MembranePotential current);

        // Enhanced action production
        std::unique_ptr<Action> produceAction(size_t min_firing_threshold = 1);
        std::unique_ptr<Action> produceActionWithContext(const std::map<std::string, std::string>& context);

        // Enhanced state management
        bool save(const std::string& filepath) const;
        bool load(const std::string& filepath);
        void reset();
        void resetToDefaults();

        // Enhanced region management
        RegionId addRegion(const std::string& name = "");
        NeuralRegion* getRegion(RegionId id);
        const NeuralRegion* getRegion(RegionId id) const;
        std::vector<RegionId> findRegionsByType(NeuronType type) const;
        std::vector<RegionId> findRegionsByNeuronCount(size_t min_neurons) const;

        // Enhanced memory access
        NeuralWorkingMemory* getWorkingMemory();
        NeuralEpisodicMemory* getEpisodicMemory();
        NeuralAssociativeMemory* getAssociativeMemory();

        // Enhanced prediction access
        PredictionSystem* getPredictionSystem();

        // Enhanced cognition access
        NeuralPlanner* getPlanner();
        ConceptFormation* getConceptFormation();
        AttentionalSelection* getAttention();

        // Enhanced development access
        DevelopmentSystem* getDevelopmentSystem();
        DevelopmentalStage getDevelopmentalStage() const;
        void setDevelopmentalStage(DevelopmentalStage stage);
        bool advanceDevelopment(double timestep);

        // Enhanced neuromodulation access
        Dopamine* getDopamine();
        Curiosity* getCuriosity();
        Novelty* getNovelty();
        PredictionError* getPredictionError();

        // Enhanced plasticity access
        SpikeSystem* getSpikeSystem();
        STDP* getSTDP();
        Hebbian* getHebbian();
        StructuralPlasticity* getStructuralPlasticity();

        // Original methods (for backward compatibility)
        bool initialize(); // Original signature

        // Property access
        size_t getTotalNeuronCount() const;
        size_t getTotalSynapseCount() const;
        size_t getActiveNeuronCount() const;
        size_t getFiringNeuronCount() const;
        float getAverageFiringRate() const;
        float getExcitationInhibitionRatio() const;
        size_t getTotalSpikeCount() const;
        size_t getPendingSpikeEventCount() const;
        DevelopmentStage getDevelopmentalStage() const; // Original

        // Logging and monitoring
        void logStatus() const;
        std::string getBrainInfo() const;

        // Context manager interface
        bool isInitialized() const { return initialized; }
        void close() { reset(); }

        // Static factory methods
        static std::unique_ptr<BrainEnhanced> create(const std::shared_ptr<Config>& config);
        static std::unique_ptr<BrainEnhanced> create(const std::shared_ptr<ConfigEnhanced>& config);
        static std::unique_ptr<BrainEnhanced> createDefault();

        // Pythonic interfaces
        py::object getRegionPy(RegionId id) {
            NeuralRegion* region = getRegion(id);
            if (!region) {
                throw BrainError("Region not found: " + std::to_string(id.value()));
            }
            return py::cast(region);
        }

        // Iterator support for regions
        class RegionIterator {
        private:
            const std::vector<std::unique_ptr<NeuralRegion>>& regions;
            size_t index;

        public:
            RegionIterator(const std::vector<std::unique_ptr<NeuralRegion>>& regions, size_t index)
                : regions(regions), index(index) {}

            NeuralRegion* operator*() { return regions[index].get(); }
            RegionIterator& operator++() { ++index; return *this; }
            bool operator!=(const RegionIterator& other) const { return index != other.index; }
            RegionIterator begin() const { return RegionIterator(regions, 0); }
            RegionIterator end() const { return RegionIterator(regions, regions.size()); }
        };

        RegionIterator beginRegions() { return RegionIterator(brain->getRegions(), 0); }
        RegionIterator endRegions() { return RegionIterator(brain->getRegions(), brain->getRegions().size()); }

        // Enhanced sensory integration
        void processSensoryInput(const SensoryInput& input, const std::string& modality = "vision");
        std::vector<float> extractFeatures(const SensoryInput& input) const;
        double getSensoryIntensity(const SensoryInput& input) const;

        // Enhanced motor planning
        std::vector<Action> planActions(size_t max_actions = 10) const;
        Action getBestAction(const std::map<std::string, std::string>& preferences = {}) const;
        double evaluateAction(const Action& action, const SensoryInput& context = nullptr) const;
    };

    // Enhanced AgentBrain with better integration
    class AgentBrainEnhanced {
    private:
        std::shared_ptr<AgentBrain> agent;
        std::shared_ptr<BrainEnhanced> brain;
        std::shared_ptr<SimpleWorld> world;
        bool initialized;

    public:
        AgentBrainEnhanced(const std::shared_ptr<Brain>& brain_ptr);
        AgentBrainEnhanced(const std::shared_ptr<BrainEnhanced>& brain_enhanced);

        // Enhanced initialization
        bool initialize(const std::shared_ptr<SimpleWorld>& world_ptr);
        bool initialize(const std::shared_ptr<SimpleWorld>& world_ptr,
                       const std::map<std::string, std::string>& agent_config);

        // Enhanced processing with context
        void processSensoryInput(const SensoryInput& percept,
                                const std::string& context = "");
        std::unique_ptr<Action> decodeMotorCommand(
            const std::map<std::string, std::string>& decision_context = {});
        void applyRewardModulation(double reward, double predicted_reward,
                                   const std::string& learning_context = "");

        // Enhanced development
        void updateDevelopment(double timestep,
                               const std::map<std::string, std::string>& development_factors = {});

        // Enhanced statistics and monitoring
        std::map<std::string, double> getPerformanceMetrics() const;
        std::map<std::string, std::string> getAgentState() const;
        double calculateIntelligenceScore() const;

        // Enhanced subsystem access
        void enableSubsystem(const std::string& subsystem, bool enable = true);
        bool isSubsystemEnabled(const std::string& subsystem) const;
        std::vector<std::string> getEnabledSubsystems() const;
        std::vector<std::string> getAllSubsystems() const;

        // Enhanced learning capabilities
        void enableLearning(bool enable = true);
        void setLearningRate(double rate);
        void setExplorationRate(double rate);
        double getLearningProgress() const;

        // Enhanced state management
        void reset();
        bool save(const std::string& filepath) const;
        bool load(const std::string& filepath);

        // Context manager support
        bool isInitialized() const { return initialized; }
        void close() { reset(); }

        // Conversion
        std::shared_ptr<AgentBrain> getOriginalAgent() const { return agent; }
    };

    // Enhanced SimpleWorld with better simulation control
    class SimpleWorldEnhanced {
    private:
        std::shared_ptr<SimpleWorld> world;
        bool initialized;

    public:
        SimpleWorldEnhanced();
        ~SimpleWorldEnhanced();

        // Enhanced configuration
        void configure(size_t width, size_t height,
                       size_t vision_width, size_t vision_height,
                       const std::map<std::string, std::string>& options = {});

        // Enhanced simulation control
        void runEpisode(size_t steps,
                        const std::map<std::string, std::string>& episode_config = {});
        void runEpisodeWithLogging(size_t steps,
                                   const std::string& log_file = "episode.log");

        // Enhanced world interaction
        std::vector<std::unique_ptr<Action>> getPossibleActions() const;
        std::unique_ptr<Action> getRandomAction() const;
        double calculateReward(const Action& action, const SensoryInput& percept) const;

        // Enhanced world state
        std::map<std::string, std::string> getWorldState() const;
        std::vector<std::string> getObjectsInView() const;
        std::vector<std::string> getAvailableResources() const;

        // Enhanced environment interaction
        bool isValidPosition(double x, double y) const;
        double distanceToNearestObject(double x, double y) const;
        std::string getClosestObjectType(double x, double y) const;

        // Enhanced simulation control
        void setTimeStep(double dt);
        void setMaxEnergy(double energy);
        void setRandomSeed(unsigned int seed);
        void enablePhysics(bool enable = true);

        // Enhanced output and visualization
        std::string getMapString() const;
        std::vector<std::vector<std::string>> getGridMap() const;
        std::string generateVisualizationData() const;

        // Enhanced statistics
        std::map<std::string, double> getSimulationStats() const;
        std::map<std::string, size_t> getWorldMetrics() const;

        // Original methods (for backward compatibility)
        void reset();
        void update(double timestep);
        void setAgentStart(double x, double y);
        void applyMotorCommand(const Action& cmd, double currentTime);
        const SensoryPercept& getSensoryPercept() const;
        const AgentBody& getAgentBody() const;
        void addObject(const WorldObject& obj);
        void removeObject(double x, double y);
        bool isValidPosition(double x, double y) const;
        size_t getWidth() const;
        size_t getHeight() const;
        double getMaxEnergy() const;
        void setMaxEnergy(double e);
        double getEnergyDecayRate() const;
        void setEnergyDecayRate(double r);
        double getSimulationTime() const;
        void setRandomSeed(int seed);
        int getRandomSeed() const;

        // Context manager support
        bool isInitialized() const { return initialized; }
        void close() { reset(); }

        // Conversion
        std::shared_ptr<SimpleWorld> getOriginalWorld() const { return world; }
    };

    // Template configurations
    inline std::vector<ConfigParameter> getDefaultConfigSchema() {
        return std::vector<ConfigParameter>{
            // Brain group
            {"brain.neuron_count", "int64_t", "Total number of neurons", "brain", {"1000", "2000", "5000", "10000"}, {100, 50000}, true, "1000"},
            {"brain.synapse_density", "float", "Synapse connectivity density", "brain", {}, {0.0, 1.0}, false, "0.1"},
            {"brain.connection_probability", "float", "Connection probability between neurons", "brain", {}, {0.0, 1.0}, false, "0.05"},
            {"brain.initial_weight_mean", "float", "Mean initial synaptic weight", "brain", {}, {-1.0, 1.0}, false, "0.5"},
            {"brain.initial_weight_std", "float", "Standard deviation of initial weights", "brain", {}, {0.0, 1.0}, false, "0.1"},
            {"brain.v_thresh", "float", "Neuron threshold potential (mV)", "brain", {}, {-100.0, -20.0}, false, "-50.0"},
            {"brain.v_rest", "float", "Resting potential (mV)", "brain", {}, {-100.0, -50.0}, false, "-70.0"},
            {"brain.v_reset", "float", "Reset potential after spike (mV)", "brain", {}, {-100.0, -50.0}, false, "-75.0"},
            {"brain.tau_mem", "float", "Membrane time constant (ms)", "brain", {}, {1.0, 100.0}, false, "20.0"},
            {"brain.tau_ref", "float", "Refractory period (ms)", "brain", {}, {0.0, 10.0}, false, "2.0"},

            // Plasticity group
            {"plasticity.stdp.enable", "bool", "Enable STDP learning", "plasticity", {"true", "false"}, {}, false, "true"},
            {"plasticity.stdp.learning_rate", "float", "STDP learning rate", "plasticity", {}, {0.0, 0.1}, false, "0.001"},
            {"plasticity.stdp.tau_plus", "float", "STDP potentiation time constant (ms)", "plasticity", {}, {1.0, 100.0}, false, "20.0"},
            {"plasticity.stdp.tau_minus", "float", "STDP depression time constant (ms)", "plasticity", {}, {1.0, 100.0}, false, "20.0"},
            {"plasticity.hebbian.enable", "bool", "Enable Hebbian learning", "plasticity", {"true", "false"}, {}, false, "true"},
            {"plasticity.structural.enable", "bool", "Enable structural plasticity", "plasticity", {"true", "false"}, {}, false, "true"},

            // Neuromodulation group
            {"neuromod.dopamine.scale", "float", "Dopamine modulation scale", "neuromod", {}, {0.0, 10.0}, false, "1.0"},
            {"neuromod.curiosity.enable", "bool", "Enable curiosity-driven exploration", "neuromod", {"true", "false"}, {}, false, "true"},
            {"neuromod.novelty.enable", "bool", "Enable novelty detection", "neuromod", {"true", "false"}, {}, false, "true"},

            // Development group
            {"development.stage_transition_rate", "float", "Rate of developmental stage transitions", "development", {}, {0.0, 0.01}, false, "0.001"},
            {"development.critical_period_length", "float", "Length of critical period (ms)", "development", {}, {100.0, 10000.0}, false, "1000.0"},
            {"development.plasticity_decay_rate", "float", "Rate at which plasticity decreases with age", "development", {}, {0.0, 1.0}, false, "0.1"},
            {"development.excitability_change_rate", "float", "Rate of excitability changes during development", "development", {}, {0.0, 1.0}, false, "0.05"},

            // Simulation parameters
            {"simulation.timestep", "float", "Simulation time step (ms)", "simulation", {}, {0.0, 10.0}, false, "0.001"},
            {"simulation.random_seed", "int64_t", "Random seed for reproducibility", "simulation", {}, {0, 1000000}, false, "42"},
            {"simulation.checkpoint_interval", "size_t", "Interval between checkpoints (steps)", "simulation", {}, {0, 100000}, false, "1000"},
            {"simulation.max_memory_episodes", "size_t", "Maximum number of episodic memory episodes", "simulation", {}, {0, 10000}, false, "1000"},
        };
    }

    // Template configuration implementations
    inline ConfigEnhanced ConfigEnhanced::createSimpleBrainConfig(size_t neuron_count) {
        ConfigEnhanced config;
        config.setParameter("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config.setParameter("plasticity.stdp.enable", true);
        config.setParameter("plasticity.hebbian.enable", true);
        return config;
    }

    inline ConfigEnhanced ConfigEnhanced::createAgentLearningConfig(size_t neuron_count) {
        ConfigEnhanced config;
        config.setParameter("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config.setParameter("plasticity.stdp.enable", true);
        config.setParameter("plasticity.hebbian.enable", true);
        config.setParameter("plasticity.structural.enable", true);
        config.setParameter("neuromod.dopamine.scale", 1.5);
        config.setParameter("neuromod.curiosity.enable", true);
        config.setParameter("development.stage_transition_rate", 0.002);
        return config;
    }

    inline ConfigEnhanced ConfigEnhanced::createResearchConfig(size_t neuron_count) {
        ConfigEnhanced config;
        config.setParameter("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config.setParameter("plasticity.stdp.enable", true);
        config.setParameter("plasticity.stdp.tau_plus", 50.0);
        config.setParameter("plasticity.stdp.tau_minus", 50.0);
        config.setParameter("plasticity.hebbian.enable", true);
        config.setParameter("plasticity.structural.enable", true);
        config.setParameter("neuromod.dopamine.scale", 2.0);
        config.setParameter("neuromod.curiosity.enable", true);
        config.setParameter("neuromod.novelty.enable", true);
        config.setParameter("development.stage_transition_rate", 0.005);
        config.setParameter("development.critical_period_length", 5000.0);
        return config;
    }

    inline ConfigEnhanced ConfigEnhanced::createProductionConfig(size_t neuron_count) {
        ConfigEnhanced config;
        config.setParameter("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config.setParameter("plasticity.stdp.enable", true);
        config.setParameter("plasticity.hebbian.enable", true);
        config.setParameter("plasticity.structural.enable", true);
        config.setParameter("neuromod.dopamine.scale", 1.0);
        config.setParameter("neuromod.curiosity.enable", false); // Lower curiosity for stability
        config.setParameter("development.stage_transition_rate", 0.0005);
        config.setParameter("simulation.random_seed", 42);
        config.setParameter("simulation.checkpoint_interval", 5000);
        return config;
    }

    // Python module initialization with enhanced API
    PYBIND11_MODULE(pynlm, m) {
        m.doc() = R"pbdoc(
            Enhanced NLM (Neural Learning Machine) Python Bindings
            ---------------------------------------------------
            Advanced Python bindings for the NLM C++ neural simulation framework.
            
            Features:
            - Enhanced configuration with schema validation
            - Context managers for automatic resource management
            - Comprehensive error handling with custom exceptions
            - Batch operations for improved performance
            - Pythonic API design with snake_case naming
            - Advanced statistics and monitoring
            - Template configurations for common use cases
        )pbdoc";

        // Register custom exceptions
        py::register_exception<NLMError>(m, "NLMError");
        py::register_exception<ConfigurationError>(m, "ConfigurationError");
        py::register_exception<BrainError>(m, "BrainError");
        py::register_exception<WorldError>(m, "WorldError");
        py::register_exception<AgentError>(m, "AgentError");

        // Register enhanced Config class
        py::class_<ConfigEnhanced>(m, "Config", R"pbdoc(
            Enhanced configuration system with schema validation and parameter groups.
            
            Supports:
            - Parameter validation with type checking
            - Configuration templates for common use cases
            - Schema validation and error reporting
            - Parameter groups (brain, plasticity, neuromodulation, development)
            - Automatic value conversion and range checking
        )pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("set", &ConfigEnhanced::setParameter, py::arg("key"), py::arg("value"),
             "Set a configuration parameter (supports int, float, bool, string)")
        .def("set", [](ConfigEnhanced& self, const std::string& key, double value) {
            return self.setParameter(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("set", [](ConfigEnhanced& self, const std::string& key, int64_t value) {
            return self.setParameter(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("set", [](ConfigEnhanced& self, const std::string& key, bool value) {
            return self.setParameter(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("get", &ConfigEnhanced::has, py::arg("key"), "Check if a configuration parameter exists")
        .def("validate", &ConfigEnhanced::validate, "Validate configuration against schema")
        .def("is_valid", &ConfigEnhanced::isValid, "Check if configuration is valid")
        .def("get_errors", &ConfigEnhanced::getValidationErrors, "Get validation error messages")
        .def("get_warnings", &ConfigEnhanced::getValidationWarnings, "Get validation warning messages")
        .def("get_groups", &ConfigEnhanced::getGroups, "Get all configuration parameter groups")
        .def("get_parameters", &ConfigEnhanced::getParametersInGroup, py::arg("group"), 
             "Get all parameters in a specific group")
        .def("get_description", &ConfigEnhanced::getParameterDescription, py::arg("key"),
             "Get description of a configuration parameter")
        .def("get_type", &ConfigEnhanced::getParameterType, py::arg("key"),
             "Get type of a configuration parameter")
        .def("create_simple_brain", &ConfigEnhanced::createSimpleBrainConfig, py::arg("neuron_count") = 1000,
             "Create a simple brain configuration")
        .def("create_agent_learning", &ConfigEnhanced::createAgentLearningConfig, py::arg("neuron_count") = 2000,
             "Create an agent learning configuration")
        .def("create_research", &ConfigEnhanced::createResearchConfig, py::arg("neuron_count") = 5000,
             "Create a research configuration")
        .def("create_production", &ConfigEnhanced::createProductionConfig, py::arg("neuron_count") = 10000,
             "Create a production configuration")
        .def("load_from_file", &ConfigEnhanced::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("save_to_file", &ConfigEnhanced::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("summary", &ConfigEnhanced::summary, "Get configuration summary")
        .def("__repr__", &ConfigEnhanced::summary)
        .def("__eq__", &ConfigEnhanced::operator==)
        .def("__ne__", &ConfigEnhanced::operator!=);

        // Register enhanced Brain class with context manager support
        py::class_<BrainEnhanced>(m, "Brain", R"pbdoc(
            Enhanced neural simulation brain with context manager support and advanced features.
            
            Features:
            - Automatic resource management with context managers
            - Enhanced error handling and diagnostics
            - Batch operations for improved performance
            - Comprehensive statistics and monitoring
            - Advanced configuration support
            - Rich API for all NLM systems
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def(py::init<std::shared_ptr<ConfigEnhanced>>())
        .def(py::init<>())  // Default constructor
        .def("initialize", &BrainEnhanced::initialize, py::arg("config_overrides") = std::map<std::string, std::string>(),
             "Initialize the brain with optional configuration overrides")
        .def("is_initialized", &BrainEnhanced::isInitialized, "Check if brain is initialized")
        .def("get_last_error", &BrainEnhanced::getLastError, "Get last error message")
        .def("clear_error", &BrainEnhanced::clearError, "Clear last error message")
        .def("step", &BrainEnhanced::step, py::arg("current_step"), py::arg("current_time") = 0.0,
             "Perform a simulation step with optional timestamp")
        .def("step_with_label", &BrainEnhanced::step, py::arg("step_label"), py::arg("step_number"), py::arg("time") = 0.0,
             "Perform a step with a descriptive label")
        .def("step_range", &BrainEnhanced::stepRange, py::arg("start_step"), py::arg("end_step"), py::arg("timestep") = 0.001,
             "Perform a range of steps (batch operation)")
        .def("run_simulation", &BrainEnhanced::runSimulation, py::arg("steps"), py::arg("timestep") = 0.001,
             "Run a complete simulation for specified steps")
        .def("get_statistics", &BrainEnhanced::getStatistics, "Get comprehensive brain statistics")
        .def("get_counts", &BrainEnhanced::getCounts, "Get brain neuron/synapse counts")
        .def("get_debug_info", &BrainEnhanced::getDebugInfo, "Get detailed debug information")
        .def("receive_sensory_input", &BrainEnhanced::receiveSensoryInput, py::arg("input"),
             "Receive sensory input")
        .def("receive_sensory_input", [](BrainEnhanced& self, const std::vector<float>& data) {
            // Create a simple sensory input from data
            auto input = std::make_shared<Vision>(8, 8, 1);
            input->setData(data);
            self.receiveSensoryInput(*input);
        }, py::arg("data"), "Receive sensory input as raw data")
        .def("inject_current", &BrainEnhanced::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &BrainEnhanced::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current to all neurons of a specific type")
        .def("produce_action", &BrainEnhanced::produceAction, py::arg("min_firing_threshold") = 1,
             "Produce motor action based on neural activity")
        .def("produce_action_with_context", &BrainEnhanced::produceActionWithContext, py::arg("context") = std::map<std::string, std::string>(),
             "Produce action with additional context")
        .def("save", &BrainEnhanced::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &BrainEnhanced::load, py::arg("filepath"), "Load brain state from file")
        .def("reset", &BrainEnhanced::reset, "Reset brain to initial state")
        .def("reset_to_defaults", &BrainEnhanced::resetToDefaults, "Reset brain to default state")
        .def("add_region", &BrainEnhanced::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &BrainEnhanced::getRegion, py::arg("id"), "Get a region by ID")
        .def("find_regions_by_type", &BrainEnhanced::findRegionsByType, py::arg("type"),
             "Find regions containing neurons of specific type")
        .def("find_regions_by_neuron_count", &BrainEnhanced::findRegionsByNeuronCount, py::arg("min_neurons"),
             "Find regions with at least specified number of neurons")
        .def("get_working_memory", &BrainEnhanced::getWorkingMemory, "Get working memory system")
        .def("get_episodic_memory", &BrainEnhanced::getEpisodicMemory, "Get episodic memory system")
        .def("get_associative_memory", &BrainEnhanced::getAssociativeMemory, "Get associative memory system")
        .def("get_prediction_system", &BrainEnhanced::getPredictionSystem, "Get prediction system")
        .def("get_planner", &BrainEnhanced::getPlanner, "Get neural planner")
        .def("get_concept_formation", &BrainEnhanced::getConceptFormation, "Get concept formation system")
        .def("get_attention", &BrainEnhanced::getAttention, "Get attentional selection system")
        .def("get_development_system", &BrainEnhanced::getDevelopmentSystem, "Get development system")
        .def("get_developmental_stage", &BrainEnhanced::getDevelopmentalStage, "Get current developmental stage")
        .def("set_developmental_stage", &BrainEnhanced::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("advance_development", &BrainEnhanced::advanceDevelopment, py::arg("timestep"), "Advance development")
        .def("get_dopamine", &BrainEnhanced::getDopamine, "Get dopamine neuromodulator")
        .def("get_curiosity", &BrainEnhanced::getCuriosity, "Get curiosity neuromodulator")
        .def("get_novelty", &BrainEnhanced::getNovelty, "Get novelty neuromodulator")
        .def("get_prediction_error", &BrainEnhanced::getPredictionError, "Get prediction error signal")
        .def("get_spike_system", &BrainEnhanced::getSpikeSystem, "Get spike system")
        .def("get_stdp", &BrainEnhanced::getSTDP, "Get STDP plasticity system")
        .def("get_hebbian", &BrainEnhanced::getHebbian, "Get Hebbian plasticity system")
        .def("get_structural_plasticity", &BrainEnhanced::getStructuralPlasticity, "Get structural plasticity system")
        .def("get_total_neuron_count", &BrainEnhanced::getTotalNeuronCount, "Get total neuron count")
        .def("get_total_synapse_count", &BrainEnhanced::getTotalSynapseCount, "Get total synapse count")
        .def("get_active_neuron_count", &BrainEnhanced::getActiveNeuronCount, "Get active neuron count")
        .def("get_firing_neuron_count", &BrainEnhanced::getFiringNeuronCount, "Get firing neuron count")
        .def("get_average_firing_rate", &BrainEnhanced::getAverageFiringRate, "Get average firing rate")
        .def("get_excitation_inhibition_ratio", &BrainEnhanced::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("get_total_spike_count", &BrainEnhanced::getTotalSpikeCount, "Get total spike count")
        .def("get_pending_spike_event_count", &BrainEnhanced::getPendingSpikeEventCount, "Get pending spike event count")
        .def("log_status", &BrainEnhanced::logStatus, "Log brain status")
        .def("get_brain_info", &BrainEnhanced::getBrainInfo, "Get brain information string")
        .def("close", &BrainEnhanced::close, "Close and cleanup brain resources")
        // Context manager support
        .def("__enter__", [](BrainEnhanced& self) { return &self; })
        .def("__exit__", [](BrainEnhanced& self, py::args) {
            self.close();
        });

        // Register Enhanced AgentBrain class
        py::class_<AgentBrainEnhanced>(m, "AgentBrain", R"pbdoc(
            Enhanced agent brain interface with advanced learning and monitoring capabilities.
            
            Features:
            - Enhanced learning with multiple subsystems
            - Comprehensive performance monitoring
            - Flexible configuration options
            - State management and persistence
            - Subsystem control and status monitoring
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def(py::init<std::shared_ptr<BrainEnhanced>>())
        .def("initialize", &AgentBrainEnhanced::initialize, py::arg("world"), py::arg("agent_config") = std::map<std::string, std::string>(),
             "Initialize agent with world and optional configuration")
        .def("is_initialized", &AgentBrainEnhanced::isInitialized, "Check if agent is initialized")
        .def("process_sensory_input", &AgentBrainEnhanced::processSensoryInput, py::arg("percept"), py::arg("context") = "",
             "Process sensory percept with optional context")
        .def("decode_motor_command", &AgentBrainEnhanced::decodeMotorCommand, py::arg("decision_context") = std::map<std::string, std::string>(),
             "Decode motor command with optional decision context")
        .def("apply_reward_modulation", &AgentBrainEnhanced::applyRewardModulation, py::arg("reward"), py::arg("predicted_reward"), py::arg("learning_context") = "",
             "Apply reward-based learning with context")
        .def("update_development", &AgentBrainEnhanced::updateDevelopment, py::arg("timestep"), py::arg("development_factors") = std::map<std::string, std::string>(),
             "Update development with additional factors")
        .def("get_performance_metrics", &AgentBrainEnhanced::getPerformanceMetrics, "Get agent performance metrics")
        .def("get_agent_state", &AgentBrainEnhanced::getAgentState, "Get complete agent state")
        .def("calculate_intelligence_score", &AgentBrainEnhanced::calculateIntelligenceScore, "Calculate intelligence score")
        .def("enable_subsystem", &AgentBrainEnhanced::enableSubsystem, py::arg("subsystem"), py::arg("enable") = true,
             "Enable or disable a subsystem")
        .def("is_subsystem_enabled", &AgentBrainEnhanced::isSubsystemEnabled, py::arg("subsystem"),
             "Check if a subsystem is enabled")
        .def("get_enabled_subsystems", &AgentBrainEnhanced::getEnabledSubsystems, "Get list of enabled subsystems")
        .def("get_all_subsystems", &AgentBrainEnhanced::getAllSubsystems, "Get list of all available subsystems")
        .def("enable_learning", &AgentBrainEnhanced::enableLearning, py::arg("enable") = true,
             "Enable or disable learning")
        .def("set_learning_rate", &AgentBrainEnhanced::setLearningRate, py::arg("rate"),
             "Set learning rate")
        .def("set_exploration_rate", &AgentBrainEnhanced::setExplorationRate, py::arg("rate"),
             "Set exploration rate")
        .def("get_learning_progress", &AgentBrainEnhanced::getLearningProgress, "Get learning progress")
        .def("reset", &AgentBrainEnhanced::reset, "Reset agent to initial state")
        .def("save", &AgentBrainEnhanced::save, py::arg("filepath"), "Save agent state to file")
        .def("load", &AgentBrainEnhanced::load, py::arg("filepath"), "Load agent state from file")
        .def("close", &AgentBrainEnhanced::close, "Close and cleanup agent resources")
        .def("__enter__", [](AgentBrainEnhanced& self) { return &self; })
        .def("__exit__", [](AgentBrainEnhanced& self, py::args) {
            self.close();
        });

        // Register Enhanced SimpleWorld class
        py::class_<SimpleWorldEnhanced>(m, "SimpleWorld", R"pbdoc(
            Enhanced 2D world simulation environment with advanced simulation control.
            
            Features:
            - Batch simulation execution
            - Enhanced reward calculation
            - Advanced world state management
            - Simulation statistics and metrics
            - Action planning and evaluation
            - Customizable simulation parameters
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorldEnhanced::configure, py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"), py::arg("options") = std::map<std::string, std::string>(),
             "Configure world with additional options")
        .def("run_episode", &SimpleWorldEnhanced::runEpisode, py::arg("steps"), py::arg("episode_config") = std::map<std::string, std::string>(),
             "Run an episode with configuration")
        .def("run_episode_with_logging", &SimpleWorldEnhanced::runEpisodeWithLogging, py::arg("steps"), py::arg("log_file") = "episode.log",
             "Run episode with logging to file")
        .def("get_possible_actions", &SimpleWorldEnhanced::getPossibleActions, "Get all possible actions")
        .def("get_random_action", &SimpleWorldEnhanced::getRandomAction, "Get random action")
        .def("calculate_reward", &SimpleWorldEnhanced::calculateReward, py::arg("action"), py::arg("percept"),
             "Calculate reward for an action in given context")
        .def("get_world_state", &SimpleWorldEnhanced::getWorldState, "Get complete world state")
        .def("get_objects_in_view", &SimpleWorldEnhanced::getObjectsInView, "Get objects visible to agent")
        .def("get_available_resources", &SimpleWorldEnhanced::getAvailableResources, "Get available resources")
        .def("is_valid_position", &SimpleWorldEnhanced::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("distance_to_nearest_object", &SimpleWorldEnhanced::distanceToNearestObject, py::arg("x"), py::arg("y"),
             "Get distance to nearest object from position")
        .def("get_closest_object_type", &SimpleWorldEnhanced::getClosestObjectType, py::arg("x"), py::arg("y"),
             "Get type of closest object to position")
        .def("set_time_step", &SimpleWorldEnhanced::setTimeStep, py::arg("dt"), "Set time step duration")
        .def("set_max_energy", &SimpleWorldEnhanced::setMaxEnergy, py::arg("energy"), "Set maximum energy")
        .def("set_random_seed", &SimpleWorldEnhanced::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("enable_physics", &SimpleWorldEnhanced::enablePhysics, py::arg("enable") = true, "Enable or disable physics")
        .def("get_map_string", &SimpleWorldEnhanced::getMapString, "Get ASCII map representation")
        .def("get_grid_map", &SimpleWorldEnhanced::getGridMap, "Get 2D grid map")
        .def("generate_visualization_data", &SimpleWorldEnhanced::generateVisualizationData, "Generate visualization data")
        .def("get_simulation_stats", &SimpleWorldEnhanced::getSimulationStats, "Get simulation statistics")
        .def("get_world_metrics", &SimpleWorldEnhanced::getWorldMetrics, "Get world metrics")
        .def("close", &SimpleWorldEnhanced::close, "Close and cleanup world resources")
        .def("__enter__", [](SimpleWorldEnhanced& self) { return &self; })
        .def("__exit__", [](SimpleWorldEnhanced& self, py::args) {
            self.close();
        });

        // Create factory functions
        m.def("createBrain", [](const std::shared_ptr<Config>& config) {
            return BrainEnhanced::create(config);
        }, py::arg("config"), "Create enhanced brain with configuration");

        m.def("createBrain", [](const std::shared_ptr<ConfigEnhanced>& config) {
            return BrainEnhanced::create(config);
        }, py::arg("config"), "Create enhanced brain with enhanced configuration");

        m.def("createBrainDefault", []() {
            return BrainEnhanced::createDefault();
        }, "Create enhanced brain with default configuration");

        m.def("createAgentBrain", [](const std::shared_ptr<Brain>& brain) {
            auto agent = std::make_shared<AgentBrainEnhanced>(brain);
            return agent;
        }, py::arg("brain"), "Create enhanced agent brain");

        m.def("createAgentBrain", [](const std::shared_ptr<BrainEnhanced>& brain) {
            auto agent = std::make_shared<AgentBrainEnhanced>(brain);
            return agent;
        }, py::arg("brain"), "Create enhanced agent brain from enhanced brain");

        m.def("createSimpleWorld", []() {
            auto world = std::make_shared<SimpleWorldEnhanced>();
            return world;
        }, "Create enhanced simple world");

        // Error types
        py::class_<NLMError, py::base<std::runtime_error>>(m, "NLMError")
            .def(py::init<const std::string&>());

        py::class_<ConfigurationError, py::base<NLMError>>(m, "ConfigurationError")
            .def(py::init<const std::string&>());

        py::class_<BrainError, py::base<NLMError>>(m, "BrainError")
            .def(py::init<const std::string&>());

        py::class_<WorldError, py::base<NLMError>>(m, "WorldError")
            .def(py::init<const std::string&>());

        py::class_<AgentError, py::base<NLMError>>(m, "AgentError")
            .def(py::init<const std::string&>());

        // Version and info
        m.attr("__version__") = "2.0.0-enhanced";
        m.attr("__description__") = "Enhanced NLM Python bindings with validation and advanced features";

        // Example usage
        m.def("example_simple_brain", []() {
            py::print("Example: Simple Brain Simulation");
            py::print("===============================\n");
            
            auto config = std::make_shared<ConfigEnhanced>();
            config->setParameter("brain.neuron_count", 1000);
            config->setParameter("plasticity.stdp.enable", true);
            
            auto brain = BrainEnhanced::create(config);
            brain->initialize();
            
            py::print("Brain created and initialized successfully!");
            py::print(f"Total neurons: {brain->getTotalNeuronCount()}");
            py::print(f"Total synapses: {brain->getTotalSynapseCount()}");
            
            for (size_t i = 0; i < 100; ++i) {
                brain->step(i);
            }
            
            py::print(f"Total spikes after 100 steps: {brain->getTotalSpikeCount()}");
            py::print("Simulation complete!\n");
        });

        m.def("example_agent_learning", []() {
            py::print("Example: Agent Learning Simulation");
            py::print("==============================\n");
            
            auto config = std::make_shared<ConfigEnhanced>();
            config->setParameter("brain.neuron_count", 2000);
            config->setParameter("neuromod.dopamine.scale", 1.5);
            config->setParameter("neuromod.curiosity.enable", true);
            
            auto brain = BrainEnhanced::create(config);
            brain->initialize();
            
            auto agent = std::make_shared<AgentBrainEnhanced>(brain);
            
            auto world = std::make_shared<SimpleWorldEnhanced>();
            world->configure(20, 20, 8, 8);
            world->reset();
            agent->initialize(world);
            
            py::print("Agent created and initialized successfully!");
            py::print(f"World size: {world->getWidth()}x{world->getHeight()}");
            
            // Run simulation
            for (size_t step = 0; step < 200; ++step) {
                world->update(0.1);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(*action, world->getSimulationTime());
                
                if (step % 50 == 0) {
                    py::print(f"Step {step}: Agent active");
                }
            }
            
            py::print("Agent simulation complete!\n");
        });
    }

} // namespace nlm

#endif // PYNLM_BINDINGS_HPP
