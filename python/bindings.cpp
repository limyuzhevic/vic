#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

// Include all necessary headers for comprehensive bindings
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
#include "../src/agent/SelfModel.hpp"
#include "../src/agent/SocialLearning.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/performance/CheckpointSystem.hpp"

// Helper functions for Pythonic conversions
namespace nlm::python {

    // Helper function to convert Python list to std::vector<float>
    inline std::vector<float> toFloatVector(const py::sequence& seq) {
        std::vector<float> result;
        result.reserve(seq.size());
        for (auto item : seq) {
            result.push_back(item.cast<float>());
        }
        return result;
    }
    
    // Helper function to convert Python list to std::vector<std::string>
    inline std::vector<std::string> toStringVector(const py::sequence& seq) {
        std::vector<std::string> result;
        result.reserve(seq.size());
        for (auto item : seq) {
            result.push_back(item.cast<std::string>());
        }
        return result;
    }
    
    // Helper function to convert Python list to std::vector<int>
    inline std::vector<int> toIntVector(const py::sequence& seq) {
        std::vector<int> result;
        result.reserve(seq.size());
        for (auto item : seq) {
            result.push_back(item.cast<int>());
        }
        return result;
    }
    
    // Helper function to convert Python dict to ConfigValue
    inline ConfigValue dictToConfigValue(const py::dict& dict) {
        if (dict.size() == 0) return ConfigValue();
        
        // Check for special keys that indicate type information
        if (dict.contains("__type__")) {
            std::string type = dict["__type__"].cast<std::string>();
            if (type == "int") {
                return dict["value"].cast<int>();
            } else if (type == "int64_t") {
                return dict["value"].cast<int64_t>();
            } else if (type == "double") {
                return dict["value"].cast<double>();
            } else if (type == "bool") {
                return dict["value"].cast<bool>();
            } else if (type == "string") {
                return dict["value"].cast<std::string>();
            } else if (type == "vector_int") {
                return toIntVector(dict["value"].cast<py::sequence>());
            } else if (type == "vector_double") {
                return toFloatVector(dict["value"].cast<py::sequence>());
            } else if (type == "vector_string") {
                return toStringVector(dict["value"].cast<py::sequence>());
            }
        }
        
        // Default to string for backward compatibility
        return dict["value"].cast<std::string>();
    }
    
    // Helper function to convert ConfigValue to Python object
    inline py::object configValueToPy(const ConfigValue& value) {
        return std::visit([](const auto& v) -> py::object {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, int>) {
                return py::cast(v);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return py::cast(v);
            } else if constexpr (std::is_same_v<T, double>) {
                return py::cast(v);
            } else if constexpr (std::is_same_v<T, bool>) {
                return py::cast(v);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return py::cast(v);
            } else if constexpr (std::is_same_v<T, std::vector<int>>()) {
                return py::cast(v);
            } else if constexpr (std::is_same_v<T, std::vector<double>>()) {
                return py::cast(v);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>()) {
                return py::cast(v);
            }
        }, value);
    }
}

// Helper functions for pybind11
namespace nlm::bindings {

    // Simple Python wrappers for Config methods
    class PyConfig {
    public:
        std::shared_ptr<Config> config;
        
        PyConfig() : config(std::make_shared<Config>()) {}
        PyConfig(std::shared_ptr<Config> cfg) : config(cfg) {}
        
        // Pythonic API methods
        void set(const std::string& key, const py::object& value, ConfigSource source = ConfigSource::Runtime) {
            ConfigValue configValue;
            
            // Convert Python value to ConfigValue based on type
            if (value.is_none()) {
                configValue = std::string();
            } else if (py::isinstance<py::bool_>(value)) {
                configValue = value.cast<bool>();
            } else if (py::isinstance<py::int_>(value)) {
                // Check if it fits in int64_t
                int64_t intVal = value.cast<int64_t>();
                if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
                    configValue = static_cast<int>(intVal);
                } else {
                    configValue = intVal;
                }
            } else if (py::isinstance<py::float_>(value)) {
                configValue = value.cast<double>();
            } else if (py::isinstance<py::str>(value)) {
                configValue = value.cast<std::string>();
            } else if (py::isinstance<py::list>(value)) {
                // Determine list element type
                py::list listVal = value.cast<py::list>();
                if (listVal.size() > 0) {
                    py::object firstItem = listVal[0];
                    if (py::isinstance<py::int_>(firstItem)) {
                        std::vector<int> intList;
                        for (auto item : listVal) {
                            intList.push_back(item.cast<int>());
                        }
                        configValue = intList;
                    } else if (py::isinstance<py::float_>(firstItem)) {
                        std::vector<double> doubleList;
                        for (auto item : listVal) {
                            doubleList.push_back(item.cast<double>());
                        }
                        configValue = doubleList;
                    } else if (py::isinstance<py::str>(firstItem)) {
                        std::vector<std::string> stringList;
                        for (auto item : listVal) {
                            stringList.push_back(item.cast<std::string>());
                        }
                        configValue = stringList;
                    }
                }
            }
            
            config->set(key, configValue, source);
        }
        
        // Get method with Pythonic return type
        py::object get(const std::string& key) const {
            std::optional<ConfigValue> value = config->get<ConfigValue>(key);
            if (value.has_value()) {
                return configValueToPy(value.value());
            }
            return py::none();
        }
        
        // Get with default
        py::object getOr(const std::string& key, const py::object& defaultValue) const {
            // Convert defaultValue to ConfigValue for comparison
            std::optional<ConfigValue> result = config->getOr<ConfigValue>(key, ConfigValue());
            
            if (!result.has_value()) {
                return defaultValue;
            }
            
            return configValueToPy(result.value());
        }
        
        // More Pythonic property-style access
        py::object operator[](const std::string& key) {
            return get(key);
        }
        
        void __setitem__(const std::string& key, const py::object& value) {
            set(key, value);
        }
        
        bool __contains__(const std::string& key) const {
            return config->has(key);
        }
        
        std::vector<std::string> keys() const {
            return config->getKeys();
        }
        
        std::string summary() const {
            return config->summary();
        }
        
        // File operations with better error handling
        bool loadFromFile(const std::string& filepath) {
            return config->loadFromFile(filepath);
        }
        
        bool saveToFile(const std::string& filepath) const {
            return config->saveToFile(filepath);
        }
        
        // Convenience methods
        int getInt(const std::string& key, int defaultValue = 0) const {
            return config->getOr<int>(key, defaultValue);
        }
        
        double getDouble(const std::string& key, double defaultValue = 0.0) const {
            return config->getOr<double>(key, defaultValue);
        }
        
        bool getBool(const std::string& key, bool defaultValue = false) const {
            return config->getOr<bool>(key, defaultValue);
        }
        
        std::string getString(const std::string& key, const std::string& defaultValue = "") const {
            return config->getOr<std::string>(key, defaultValue);
        }
        
        std::vector<int> getIntVector(const std::string& key) const {
            return config->getOr<std::vector<int>>(key, std::vector<int>());
        }
        
        std::vector<double> getDoubleVector(const std::string& key) const {
            return config->getOr<std::vector<double>>(key, std::vector<double>());
        }
        
        std::vector<std::string> getStringVector(const std::string& key) const {
            return config->getOr<std::vector<std::string>>(key, std::vector<std::string>());
        }
        
        // Check if a key exists
        bool has(const std::string& key) const {
            return config->has(key);
        }
        
        // Remove a key
        void remove(const std::string& key) {
            config->remove(key);
        }
        
        // Clear all configuration
        void clear() {
            config->clear();
        }
        
        // Static methods
        static PyConfig createDefaultConfig() {
            return PyConfig(std::make_shared<Config>());
        }
    };
    
    // Simple Python wrappers for Brain methods
    class PyBrain {
    public:
        std::shared_ptr<Brain> brain;
        
        PyBrain(std::shared_ptr<Config> config) : brain(std::make_shared<Brain>(config)) {}
        PyBrain(std::shared_ptr<Brain> b) : brain(b) {}
        
        // Initialization
        bool initialize() {
            return brain->initialize();
        }
        
        // Step methods
        void step(int currentStep) {
            brain->step(static_cast<SimulationStep>(currentStep));
        }
        
        void stepWithTime(int currentStep, double currentTime) {
            brain->step(static_cast<SimulationStep>(currentStep), static_cast<Timestamp>(currentTime));
        }
        
        // State modification
        void injectCurrent(NeuronId neuron, double current) {
            brain->injectCurrent(neuron, static_cast<MembranePotential>(current));
        }
        
        void injectCurrentToNeurons(NeuronType type, double current) {
            brain->injectCurrentToNeurons(type, static_cast<MembranePotential>(current));
        }
        
        // Action production
        py::object produceAction() {
            auto action = brain->produceAction();
            if (action) {
                return action;  // Return as pybind11 object
            }
            return py::none();
        }
        
        // Statistics methods
        size_t getTotalNeuronCount() const {
            return brain->getTotalNeuronCount();
        }
        
        size_t getTotalSynapseCount() const {
            return brain->getTotalSynapseCount();
        }
        
        size_t getActiveNeuronCount() const {
            return brain->getActiveNeuronCount();
        }
        
        size_t getFiringNeuronCount() const {
            return brain->getFiringNeuronCount();
        }
        
        double getAverageFiringRate() const {
            return brain->getAverageFiringRate();
        }
        
        double getExcitationInhibitionRatio() const {
            return brain->getExcitationInhibitionRatio();
        }
        
        size_t getTotalSpikeCount() const {
            return brain->getTotalSpikeCount();
        }
        
        size_t getPendingSpikeEventCount() const {
            return brain->getPendingSpikeEventCount();
        }
        
        // Development
        DevelopmentalStage getDevelopmentalStage() const {
            return brain->getDevelopmentalStage();
        }
        
        void setDevelopmentalStage(DevelopmentalStage stage) {
            brain->setDevelopmentalStage(stage);
        }
        
        double getLearnedPreference() const {
            return brain->getLearnedPreference();
        }
        
        void setLearnedPreference(double preference) {
            brain->setLearnedPreference(preference);
        }
        
        // Region management
        RegionId addRegion(const std::string& name = "") {
            return brain->addRegion(name);
        }
        
        NeuralRegion* getRegion(RegionId id) {
            return brain->getRegion(id);
        }
        
        const NeuralRegion* getRegion(RegionId id) const {
            return brain->getRegion(id);
        }
        
        size_t getRegionCount() const {
            return brain->getRegionCount();
        }
        
        std::vector<RegionId> getRegionIds() const {
            return brain->getRegionIds();
        }
        
        // Working memory access
        NeuralWorkingMemory* getWorkingMemory() {
            return brain->getWorkingMemory();
        }
        
        NeuralEpisodicMemory* getEpisodicMemory() {
            return brain->getEpisodicMemory();
        }
        
        NeuralAssociativeMemory* getAssociativeMemory() {
            return brain->getAssociativeMemory();
        }
        
        // Prediction system
        PredictionSystem* getPredictionSystem() {
            return brain->getPredictionSystem();
        }
        
        // Cognition systems
        NeuralPlanner* getPlanner() {
            return brain->getPlanner();
        }
        
        ConceptFormation* getConceptFormation() {
            return brain->getConceptFormation();
        }
        
        AttentionalSelection* getAttention() {
            return brain->getAttention();
        }
        
        // Development system
        DevelopmentSystem* getDevelopmentSystem() {
            return brain->getDevelopmentSystem();
        }
        
        // Neuromodulation systems
        Dopamine* getDopamine() {
            return brain->getDopamine();
        }
        
        Curiosity* getCuriosity() {
            return brain->getCuriosity();
        }
        
        Novelty* getNovelty() {
            return brain->getNovelty();
        }
        
        PredictionError* getPredictionErrorSignal() {
            return brain->getPredictionErrorSignal();
        }
        
        // Logging and status
        void logStatus() const {
            brain->logStatus();
        }
        
        // Save and load
        bool save(const std::string& filepath) const {
            return brain->save(filepath);
        }
        
        bool load(const std::string& filepath) {
            return brain->load(filepath);
        }
        
        // Reset
        void reset() {
            brain->reset();
        }
        
        // Get configuration
        std::shared_ptr<const Config> getConfig() const {
            return brain->getConfig();
        }
        
        // Get random generator
        RandomGenerator* getRandomGenerator() {
            return brain->getRandomGenerator();
        }
    };
    
    // Simple Python wrappers for AgentBrain methods
    class PyAgentBrain {
    public:
        std::shared_ptr<AgentBrain> agentBrain;
        
        PyAgentBrain(std::shared_ptr<Brain> brain) : agentBrain(std::make_shared<AgentBrain>(brain)) {}
        PyAgentBrain(std::shared_ptr<AgentBrain> agent) : agentBrain(agent) {}
        
        // Initialization
        void initialize(const SimpleWorld& world) {
            agentBrain->initialize(world);
        }
        
        // Input processing
        size_t getSensoryInputSize() const {
            return agentBrain->getSensoryInputSize();
        }
        
        size_t getMotorOutputSize() const {
            return agentBrain->getMotorOutputSize();
        }
        
        void processSensoryInput(const SensoryPercept& percept) {
            agentBrain->processSensoryInput(percept);
        }
        
        // Motor command
        MotorCommand decodeMotorCommand() {
            return agentBrain->decodeMotorCommand();
        }
        
        // Neuromodulation
        void applyRewardModulation(double reward, double predictedReward) {
            agentBrain->applyRewardModulation(reward, predictedReward);
        }
        
        // Development
        void updateDevelopment(double timestep) {
            agentBrain->updateDevelopment(timestep);
        }
        
        DevelopmentalStage getDevelopmentalStage() const {
            return agentBrain->getDevelopmentalStage();
        }
        
        // Neuromodulation levels
        double getNeuromodulationLevel() const {
            return agentBrain->getNeuromodulationLevel();
        }
        
        double getCuriosityLevel() const {
            return agentBrain->getCuriosityLevel();
        }
        
        double getNoveltyLevel() const {
            return agentBrain->getNoveltyLevel();
        }
        
        double getPredictionError() const {
            return agentBrain->getPredictionError();
        }
        
        // Cognitive features
        std::string getThoughtTrace() const {
            return agentBrain->getThoughtTrace();
        }
        
        double getLearnedPreference() const {
            return agentBrain->getLearnedPreference();
        }
        
        // Configuration flags
        void enableRewardModulation(bool enable) {
            agentBrain->enableRewardModulation(enable);
        }
        
        void enableStructuralPlasticity(bool enable) {
            agentBrain->enableStructuralPlasticity(enable);
        }
        
        void enableDevelopment(bool enable) {
            agentBrain->enableDevelopment(enable);
        }
        
        void enableCuriosity(bool enable) {
            agentBrain->enableCuriosity(enable);
        }
        
        bool isRewardModulationEnabled() const {
            return agentBrain->isRewardModulationEnabled();
        }
        
        bool isStructuralPlasticityEnabled() const {
            return agentBrain->isStructuralPlasticityEnabled();
        }
        
        bool isDevelopmentEnabled() const {
            return agentBrain->isDevelopmentEnabled();
        }
        
        bool isCuriosityEnabled() const {
            return agentBrain->isCuriosityEnabled();
        }
        
        // Diagnostic counters
        int getReflexBonusCount() const {
            return agentBrain->getReflexBonusCount();
        }
        
        int getReflexSuppressCount() const {
            return agentBrain->getReflexSuppressCount();
        }
        
        int getWtaPulseCount() const {
            return agentBrain->getWtaPulseCount();
        }
        
        int getPlannerOverrideCount() const {
            return agentBrain->getPlannerOverrideCount();
        }
        
        int getPlannerAgreementCount() const {
            return agentBrain->getPlannerAgreementCount();
        }
        
        int getPlannerDecisionCount() const {
            return agentBrain->getPlannerDecisionCount();
        }
        
        int getConceptBiasCount() const {
            return agentBrain->getConceptBiasCount();
        }
        
        int getSelfBiasCount() const {
            return agentBrain->getSelfBiasCount();
        }
        
        int getSocialBiasCount() const {
            return agentBrain->getSocialBiasCount();
        }
        
        size_t getConceptCount() const {
            return agentBrain->getConceptCount();
        }
        
        double getSelfModelConfidence() const {
            return agentBrain->getSelfModelConfidence();
        }
        
        size_t getSocialObservationCount() const {
            return agentBrain->getSocialObservationCount();
        }
        
        // Text encoding
        static std::vector<size_t> encodeText(const std::string& text) {
            return AgentBrain::encodeText(text);
        }
        
        std::vector<float> getInternalFiringRates() const {
            return agentBrain->getInternalFiringRates();
        }
        
        // Social learning
        void observeOtherAgent(MotorCommand observed, 
                             const std::vector<float>& beforeVision,
                             const std::vector<float>& afterVision) {
            agentBrain->observeOtherAgent(observed, beforeVision, afterVision);
        }
        
        // Reset
        void reset() {
            agentBrain->reset();
        }
        
        // Get brain pointer
        Brain* getBrain() {
            return agentBrain->getBrain();
        }
        
        std::shared_ptr<Brain> getBrainShared() {
            return agentBrain->getBrain();
        }
    };
    
    // Simple Python wrappers for SimpleWorld methods
    class PySimpleWorld {
    public:
        std::shared_ptr<SimpleWorld> world;
        
        PySimpleWorld() : world(std::make_shared<SimpleWorld>()) {}
        PySimpleWorld(std::shared_ptr<SimpleWorld> w) : world(w) {}
        
        // Configuration
        void configure(float width, float height, size_t visionWidth, size_t visionHeight) {
            world->configure(width, height, visionWidth, visionHeight);
        }
        
        // Reset
        void reset() {
            world->reset();
        }
        
        // Agent control
        void setAgentStart(float x, float y) {
            world->setAgentStart(x, y);
        }
        
        // Simulation
        void update(double timestep) {
            world->update(timestep);
        }
        
        ActionResult applyMotorCommand(MotorCommand cmd, double currentTime) {
            return world->applyMotorCommand(cmd, currentTime);
        }
        
        // Accessors
        const SensoryPercept& getSensoryPercept() const {
            return world->getSensoryPercept();
        }
        
        const AgentBody& getAgentBody() const {
            return world->getAgentBody();
        }
        
        // Object management
        void addObject(const WorldObject& obj) {
            world->addObject(obj);
        }
        
        void removeObject(float x, float y) {
            world->removeObject(x, y);
        }
        
        bool isValidPosition(float x, float y) const {
            return world->isValidPosition(x, y);
        }
        
        WorldObject* getObjectAt(float x, float y) {
            return world->getObjectAt(x, y);
        }
        
        // Dimensions and settings
        float getWidth() const {
            return world->getWidth();
        }
        
        float getHeight() const {
            return world->getHeight();
        }
        
        size_t getVisionWidth() const {
            return world->getVisionWidth();
        }
        
        size_t getVisionHeight() const {
            return world->getVisionHeight();
        }
        
        float getMaxEnergy() const {
            return world->getMaxEnergy();
        }
        
        void setMaxEnergy(float e) {
            world->setMaxEnergy(e);
        }
        
        float getEnergyDecayRate() const {
            return world->getEnergyDecayRate();
        }
        
        void setEnergyDecayRate(float r) {
            world->setEnergyDecayRate(r);
        }
        
        double getSimulationTime() const {
            return world->getSimulationTime();
        }
        
        uint64_t getRandomSeed() const {
            return world->getRandomSeed();
        }
        
        void setRandomSeed(uint64_t seed) {
            world->setRandomSeed(seed);
        }
        
        // Create new world with same configuration
        std::shared_ptr<SimpleWorld> clone() const {
            auto newWorld = std::make_shared<SimpleWorld>();
            newWorld->configure(world->getWidth(), world->getHeight(),
                               world->getVisionWidth(), world->getVisionHeight());
            newWorld->setAgentStart(world->getAgentBody().x, world->getAgentBody().y);
            newWorld->setMaxEnergy(world->getMaxEnergy());
            newWorld->setEnergyDecayRate(world->getEnergyDecayRate());
            newWorld->setRandomSeed(world->getRandomSeed());
            return newWorld;
        }
    };

    // Create Python modules with improved APIs
    void addConfigModule(py::module_& m) {
        py::class_<PyConfig>(m, "Config", "Pythonic configuration wrapper for NLM")
            .def(py::init<>(), "Create a new configuration")
            .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Wrap existing config")
            
            // Pythonic property-style access
            .def("__getitem__", &PyConfig::operator[], "Get configuration value using []")
            .def("__setitem__", &PyConfig::operator[], "Set configuration value using []")
            .def("__contains__", &PyConfig::operator[], "Check if key exists in config")
            
            // File operations
            .def("loadFromFile", &PyConfig::loadFromFile, py::arg("filepath"), "Load configuration from JSON file")
            .def("saveToFile", &PyConfig::saveToFile, py::arg("filepath"), "Save configuration to JSON file")
            
            // Value access with type safety
            .def("getInt", &PyConfig::getInt, py::arg("key"), py::arg("defaultValue") = 0,
                 "Get integer value from configuration")
            .def("getDouble", &PyConfig::getDouble, py::arg("key"), py::arg("defaultValue") = 0.0,
                 "Get floating-point value from configuration")
            .def("getBool", &PyConfig::getBool, py::arg("key"), py::arg("defaultValue") = false,
                 "Get boolean value from configuration")
            .def("getString", &PyConfig::getString, py::arg("key"), py::arg("defaultValue") = "",
                 "Get string value from configuration")
            .def("getIntVector", &PyConfig::getIntVector, py::arg("key"),
                 "Get vector of integers from configuration")
            .def("getDoubleVector", &PyConfig::getDoubleVector, py::arg("key"),
                 "Get vector of doubles from configuration")
            .def("getStringVector", &PyConfig::getStringVector, py::arg("key"),
                 "Get vector of strings from configuration")
            
            // Dynamic value access
            .def("get", &PyConfig::get, py::arg("key"), "Get configuration value (type inference)")
            .def("getOr", &PyConfig::getOr, py::arg("key"), py::arg("defaultValue"), "Get value with default")
            
            // Utility methods
            .def("has", &PyConfig::has, py::arg("key"), "Check if key exists")
            .def("remove", &PyConfig::remove, py::arg("key"), "Remove key from configuration")
            .def("keys", &PyConfig::keys, "Get all configuration keys")
            .def("clear", &PyConfig::clear, "Clear all configuration entries")
            .def("summary", &PyConfig::summary, "Get configuration summary")
            
            // Configuration source
            .def("set", &PyConfig::set, py::arg("key"), py::arg("value"),
                 "Set configuration value with automatic type detection")
            
            .def_static("createDefaultConfig", &PyConfig::createDefaultConfig,
                       "Create a default configuration");
    }
    
    void addBrainModule(py::module_& m) {
        py::class_<PyBrain>(m, "Brain", "Enhanced brain simulation class")
            .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with config")
            .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Wrap existing brain")
            
            // Initialization
            .def("initialize", &PyBrain::initialize, "Initialize brain with configuration")
            .def("isInitialized", &PyBrain::initialize, "Check if brain is initialized (alias)")
            
            // Simulation steps
            .def("step", &PyBrain::step, py::arg("currentStep"), "Perform a simulation step")
            .def("stepWithTime", &PyBrain::stepWithTime,
                 py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
            
            // Input processing
            .def("injectCurrent", &PyBrain::injectCurrent,
                 py::arg("neuron"), py::arg("current"), "Inject current into specific neuron")
            .def("injectCurrentToNeurons", &PyBrain::injectCurrentToNeurons,
                 py::arg("type"), py::arg("current"), "Inject current into all neurons of specific type")
            .def("receiveSensoryInput", &PyBrain::receiveSensoryInput,
                 py::arg("input"), "Receive sensory input from environment")
            
            // Action production
            .def("produceAction", &PyBrain::produceAction, "Produce motor action based on neural activity")
            
            // Statistics
            .def("getTotalNeuronCount", &PyBrain::getTotalNeuronCount, "Get total neuron count")
            .def("getTotalSynapseCount", &PyBrain::getTotalSynapseCount, "Get total synapse count")
            .def("getActiveNeuronCount", &PyBrain::getActiveNeuronCount, "Get active neuron count")
            .def("getFiringNeuronCount", &PyBrain::getFiringNeuronCount, "Get firing neuron count")
            .def("getAverageFiringRate", &PyBrain::getAverageFiringRate, "Get average firing rate")
            .def("getExcitationInhibitionRatio", &PyBrain::getExcitationInhibitionRatio,
                 "Get excitation/inhibition balance ratio")
            .def("getTotalSpikeCount", &PyBrain::getTotalSpikeCount, "Get total spike count")
            .def("getPendingSpikeEventCount", &PyBrain::getPendingSpikeEventCount,
                 "Get pending spike event count")
            
            // Development
            .def("getDevelopmentalStage", &PyBrain::getDevelopmentalStage, "Get developmental stage")
            .def("setDevelopmentalStage", &PyBrain::setDevelopmentalStage,
                 py::arg("stage"), "Set developmental stage")
            .def("getLearnedPreference", &PyBrain::getLearnedPreference, "Get learned preference trait")
            .def("setLearnedPreference", &PyBrain::setLearnedPreference,
                 py::arg("preference"), "Set learned preference trait")
            
            // Memory systems
            .def("getWorkingMemory", &PyBrain::getWorkingMemory, "Get working memory system")
            .def("getEpisodicMemory", &PyBrain::getEpisodicMemory, "Get episodic memory system")
            .def("getAssociativeMemory", &PyBrain::getAssociativeMemory, "Get associative memory system")
            
            // Prediction system
            .def("getPredictionSystem", &PyBrain::getPredictionSystem, "Get prediction system")
            
            // Cognition systems
            .def("getPlanner", &PyBrain::getPlanner, "Get neural planner")
            .def("getConceptFormation", &PyBrain::getConceptFormation, "Get concept formation system")
            .def("getAttention", &PyBrain::getAttention, "Get attentional selection system")
            
            // Development system
            .def("getDevelopmentSystem", &PyBrain::getDevelopmentSystem, "Get development system")
            
            // Neuromodulation systems
            .def("getDopamine", &PyBrain::getDopamine, "Get dopamine neuromodulator")
            .def("getCuriosity", &PyBrain::getCuriosity, "Get curiosity neuromodulator")
            .def("getNovelty", &PyBrain::getNovelty, "Get novelty neuromodulator")
            .def("getPredictionErrorSignal", &PyBrain::getPredictionErrorSignal,
                 "Get prediction error signal")
            
            // Region management
            .def("addRegion", &PyBrain::addRegion, py::arg("name") = "",
                 "Add a new neural region")
            .def("getRegion", &PyBrain::getRegion, py::arg("id"),
                 "Get a region by ID")
            .def("getRegionCount", &PyBrain::getRegionCount, "Get number of regions")
            .def("getRegionIds", &PyBrain::getRegionIds, "Get all region IDs")
            
            // Configuration
            .def("getConfig", &PyBrain::getConfig, "Get brain configuration")
            
            // Random generator
            .def("getRandomGenerator", &PyBrain::getRandomGenerator, "Get random generator")
            
            // Logging
            .def("logStatus", &PyBrain::logStatus, "Log brain status")
            
            // Save and load
            .def("save", &PyBrain::save, py::arg("filepath"), "Save brain state to file")
            .def("load", &PyBrain::load, py::arg("filepath"), "Load brain state from file")
            
            // Reset
            .def("reset", &PyBrain::reset, "Reset brain state")
            
            // Clone method
            .def("clone", &PyBrain::clone, "Create a copy of the brain")
            
            // Utility methods
            .def("__repr__", [](const PyBrain& b) {
                return "<Brain: " + std::to_string(b.getTotalNeuronCount()) + " neurons, " +
                       std::to_string(b.getTotalSynapseCount()) + " synapses>";
            }, "String representation");
    }
    
    void addWorldModule(py::module_& m) {
        py::class_<PySimpleWorld>(m, "SimpleWorld", "Simple 2D world simulation")
            .def(py::init<>(), "Create a new simple world")
            .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"), "Wrap existing world")
            
            // Configuration
            .def("configure", &PySimpleWorld::configure,
                 py::arg("width"), py::arg("height"),
                 py::arg("visionWidth"), py::arg("visionHeight"),
                 "Configure world dimensions and vision")
            
            // Reset
            .def("reset", &PySimpleWorld::reset, "Reset world to initial state")
            
            // Agent setup
            .def("setAgentStart", &PySimpleWorld::setAgentStart,
                 py::arg("x"), py::arg("y"), "Set agent starting position")
            
            // Simulation
            .def("update", &PySimpleWorld::update, py::arg("timestep"), "Update world state")
            .def("step", &PySimpleWorld::update, py::arg("timestep"), "Alias for update()")
            
            // Actions
            .def("applyMotorCommand", &PySimpleWorld::applyMotorCommand,
                 py::arg("cmd"), py::arg("currentTime"), "Apply motor command")
            
            // Sensory access
            .def("getSensoryPercept", &PySimpleWorld::getSensoryPercept,
                 "Get current sensory percept")
            .def("getVision", &PySimpleWorld::getSensoryPercept,
                 "Get vision sensory data (alias)")
            
            // Agent state
            .def("getAgentBody", &PySimpleWorld::getAgentBody, "Get agent body state")
            
            // Object management
            .def("addObject", &PySimpleWorld::addObject, py::arg("obj"), "Add object to world")
            .def("removeObject", &PySimpleWorld::removeObject,
                 py::arg("x"), py::arg("y"), "Remove object at position")
            .def("isValidPosition", &PySimpleWorld::isValidPosition,
                 py::arg("x"), py::arg("y"), "Check if position is valid")
            .def("getObjectAt", &PySimpleWorld::getObjectAt,
                 py::arg("x"), py::arg("y"), "Get object at position")
            
            // Dimensions
            .def("getWidth", &PySimpleWorld::getWidth, "Get world width")
            .def("getHeight", &PySimpleWorld::getHeight, "Get world height")
            .def("getVisionWidth", &PySimpleWorld::getVisionWidth, "Get vision width")
            .def("getVisionHeight", &PySimpleWorld::getVisionHeight, "Get vision height")
            
            // Energy system
            .def("getMaxEnergy", &PySimpleWorld::getMaxEnergy, "Get maximum energy")
            .def("setMaxEnergy", &PySimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
            .def("getEnergyDecayRate", &PySimpleWorld::getEnergyDecayRate, "Get energy decay rate")
            .def("setEnergyDecayRate", &PySimpleWorld::setEnergyDecayRate,
                 py::arg("r"), "Set energy decay rate")
            
            // Time
            .def("getSimulationTime", &PySimpleWorld::getSimulationTime, "Get simulation time")
            
            // Random seed
            .def("setRandomSeed", &PySimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
            .def("getRandomSeed", &PySimpleWorld::getRandomSeed, "Get random seed")
            
            // Utility
            .def("clone", &PySimpleWorld::clone, "Create a copy of the world")
            
            .def("__repr__", [](const PySimpleWorld& w) {
                return "<SimpleWorld: " + std::to_string(w.getWidth()) + "x" +
                       std::to_string(w.getHeight()) + " meters>";
            }, "String representation");
    }
    
    void addAgentModule(py::module_& m) {
        py::class_<PyAgentBrain>(m, "AgentBrain", "Agent-brain interface for NLM simulation")
            .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain with brain")
            .def(py::init<std::shared_ptr<AgentBrain>>(), py::arg("agentBrain"), "Wrap existing agent brain")
            
            // Initialization
            .def("initialize", &PyAgentBrain::initialize,
                 py::arg("world"), "Initialize with world")
            
            // Input/output sizes
            .def("getSensoryInputSize", &PyAgentBrain::getSensoryInputSize,
                 "Get expected sensory input size")
            .def("getMotorOutputSize", &PyAgentBrain::getMotorOutputSize,
                 "Get expected motor output size")
            
            // Processing
            .def("processSensoryInput", &PyAgentBrain::processSensoryInput,
                 py::arg("percept"), "Process sensory percept and inject into brain")
            .def("decodeMotorCommand", &PyAgentBrain::decodeMotorCommand,
                 "Decode brain motor activity into motor command")
            
            // Neuromodulation
            .def("applyRewardModulation", &PyAgentBrain::applyRewardModulation,
                 py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
            
            // Development
            .def("updateDevelopment", &PyAgentBrain::updateDevelopment,
                 py::arg("timestep"), "Update development system")
            
            // State
            .def("getDevelopmentalStage", &PyAgentBrain::getDevelopmentalStage,
                 "Get current developmental stage")
            
            // Neuromodulation levels
            .def("getNeuromodulationLevel", &PyAgentBrain::getNeuromodulationLevel,
                 "Get current neuromodulation level")
            .def("getCuriosityLevel", &PyAgentBrain::getCuriosityLevel,
                 "Get curiosity level")
            .def("getNoveltyLevel", &PyAgentBrain::getNoveltyLevel,
                 "Get novelty level")
            .def("getPredictionError", &PyAgentBrain::getPredictionError,
                 "Get prediction error")
            
            // Cognitive features
            .def("getThoughtTrace", &PyAgentBrain::getThoughtTrace,
                 "Get thought trace (perception -> planning -> action)")
            .def("getLearnedPreference", &PyAgentBrain::getLearnedPreference,
                 "Get learned preference trait")
            
            // Configuration flags
            .def("enableRewardModulation", &PyAgentBrain::enableRewardModulation,
                 py::arg("enable"), "Enable reward modulation")
            .def("enableStructuralPlasticity", &PyAgentBrain::enableStructuralPlasticity,
                 py::arg("enable"), "Enable structural plasticity")
            .def("enableDevelopment", &PyAgentBrain::enableDevelopment,
                 py::arg("enable"), "Enable development")
            .def("enableCuriosity", &PyAgentBrain::enableCuriosity,
                 py::arg("enable"), "Enable curiosity")
            
            // Status checks
            .def("isRewardModulationEnabled", &PyAgentBrain::isRewardModulationEnabled,
                 "Check if reward modulation is enabled")
            .def("isStructuralPlasticityEnabled", &PyAgentBrain::isStructuralPlasticityEnabled,
                 "Check if structural plasticity is enabled")
            .def("isDevelopmentEnabled", &PyAgentBrain::isDevelopmentEnabled,
                 "Check if development is enabled")
            .def("isCuriosityEnabled", &PyAgentBrain::isCuriosityEnabled,
                 "Check if curiosity is enabled")
            
            // Diagnostic counters
            .def("getReflexBonusCount", &PyAgentBrain::getReflexBonusCount,
                 "Get vision-gated reflex bonus count")
            .def("getReflexSuppressCount", &PyAgentBrain::getReflexSuppressCount,
                 "Get vision-gated hazard suppression count")
            .def("getWtaPulseCount", &PyAgentBrain::getWtaPulseCount,
                 "Get winner-take-all pulse count")
            .def("getPlannerOverrideCount", &PyAgentBrain::getPlannerOverrideCount,
                 "Get planner override count")
            .def("getPlannerAgreementCount", &PyAgentBrain::getPlannerAgreementCount,
                 "Get planner agreement count")
            .def("getPlannerDecisionCount", &PyAgentBrain::getPlannerDecisionCount,
                 "Get planner decision count")
            .def("getConceptBiasCount", &PyAgentBrain::getConceptBiasCount,
                 "Get concept bias count")
            .def("getSelfBiasCount", &PyAgentBrain::getSelfBiasCount,
                 "Get self-bias count")
            .def("getSocialBiasCount", &PyAgentBrain::getSocialBiasCount,
                 "Get social bias count")
            .def("getConceptCount", &PyAgentBrain::getConceptCount,
                 "Get concept count")
            .def("getSelfModelConfidence", &PyAgentBrain::getSelfModelConfidence,
                 "Get self-model confidence")
            .def("getSocialObservationCount", &PyAgentBrain::getSocialObservationCount,
                 "Get social observation count")
            
            // Text encoding
            .def_static("encodeText", &PyAgentBrain::encodeText, py::arg("text"),
                 "Encode text into neural pattern (Phase 6 feature)")
            .def("getInternalFiringRates", &PyAgentBrain::getInternalFiringRates,
                 "Get internal firing rates for text recall")
            
            // Social learning
            .def("observeOtherAgent", &PyAgentBrain::observeOtherAgent,
                 py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"),
                 "Observe other agent's actions and vision")
            
            // Reset
            .def("reset", &PyAgentBrain::reset, "Reset agent for new episode")
            
            // Brain access
            .def("getBrain", &PyAgentBrain::getBrain,
                 "Get underlying brain pointer")
            .def("getBrain", &PyAgentBrain::getBrainShared,
                 "Get underlying brain (shared pointer)")
            
            // Utility
            .def("__repr__", [](const PyAgentBrain& a) {
                auto brain = a.getBrainShared();
                if (brain) {
                    return "<AgentBrain: " + std::to_string(brain->getTotalNeuronCount()) +
                           " neurons>";
                }
                return "<AgentBrain: uninitialized>";
            }, "String representation");
    }
    
    void addUtilityFunctions(py::module_& m) {
        // CreateDefaultConfig
        m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration");
        
        // CreateBrain
        m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration");
        
        // CreateSimpleWorld
        m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, "Create a new simple world");
        
        // CreateAgentBrain
        m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a new agent brain interface");
        
        // Invalid ID constants (for backward compatibility)
        m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
        m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
        m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
        m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
        
        // Utility functions
        m.def("toFloatVector", &nlm::python::toFloatVector,
              py::arg("seq"), "Convert Python sequence to std::vector<float>");
        
        m.def("toStringVector", &nlm::python::toStringVector,
              py::arg("seq"), "Convert Python sequence to std::vector<std::string>");
        
        m.def("toIntVector", &nlm::python::toIntVector,
              py::arg("seq"), "Convert Python sequence to std::vector<int>");
        
        // Version
        m.attr("__version__") = "0.1.0";
        
        // Welcome message
        m.def("welcome", []() {
            std::cout << "Welcome to pynlm - Python bindings for NLM (熙然) neural simulation framework!" << std::endl;
            std::cout << "See easy_usage.md for beginner-friendly examples." << std::endl;
            std::cout << "See HOW_TO_USE.md for detailed API documentation." << std::endl;
        }, "Print welcome message with usage guidance");
    }
}

// Main module definition with improved Pythonic API
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides an intuitive Python API for creating and simulating
        virtual brains with learning capabilities, sensory-motor integration,
        and adaptive behavior.
        
        Key Features:
        • Pythonic configuration management
        • Enhanced brain simulation with detailed statistics
        • Agent-brain interface for world interaction
        • Simplified world simulation
        • Text encoding and cognitive features (Phase 6)
        • Social learning capabilities
        
        Usage Examples:
        
        >>> import pynlm
        >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
        >>> brain.initialize()
        >>> for i in range(100):
        ...     brain.step(i)
        
        See easy_usage.md for beginner-friendly tutorials and
        HOW_TO_USE.md for comprehensive API documentation.
    )pbdoc";
    
    // Add version
    m.attr("__version__") = "0.1.0";
    
    // Add utility functions first
    nlm::bindings::addUtilityFunctions(m);
    
    // Add main module components
    nlm::bindings::addConfigModule(m);
    nlm::bindings::addBrainModule(m);
    nlm::bindings::addWorldModule(m);
    nlm::bindings::addAgentModule(m);
    
    // Add original class bindings for backward compatibility
    // (This section would include the original bindings from the original file)
    
    // Add type mappings for better Python integration
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}
}

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
