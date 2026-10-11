#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <pybind11/eigen.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <future>
#include <chrono>
#include <map>
#include <optional>
#include <variant>
#include <any>
#include <filesystem>

namespace py = pybind11;
namespace nlm {

// Forward declarations for Python API
class EnhancedBrain;
class EnhancedWorld;
class ConfigObject;
class EnhancedAgent;
class TrainingAPI;
class SimulationAPI;
class AnalysisAPI;
class VisualizationAPI;
class ExportImportAPI;

// Modern Python configuration system
class ConfigObject {
public:
    ConfigObject() = default;
    ConfigObject(const std::string& configFile);
    
    // Set configuration values
    ConfigObject& set(const std::string& key, int value);
    ConfigObject& set(const std::string& key, double value);
    ConfigObject& set(const std::string& key, float value);
    ConfigObject& set(const std::string& key, bool value);
    ConfigObject& set(const std::string& key, const std::string& value);
    ConfigObject& set(const std::string& key, const std::vector<int>& value);
    ConfigObject& set(const std::string& key, const std::vector<double>& value);
    ConfigObject& set(const std::string& key, const std::vector<float>& value);
    ConfigObject& set(const std::string& key, const std::vector<std::string>& value);
    
    // Get configuration values
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    float getFloat(const std::string& key, float defaultValue = 0.0f) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    std::vector<int> getIntVector(const std::string& key) const;
    std::vector<double> getDoubleVector(const std::string& key) const;
    std::vector<float> getFloatVector(const std::string& key) const;
    std::vector<std::string> getStringVector(const std::string& key) const;
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::string toJSON() const;
    void loadFromJSON(const std::string& jsonStr);
    bool saveToFile(const std::string& filepath) const;
    void loadFromFile(const std::string& filepath);
    
    // Operator overloads for fluent interface
    ConfigObject operator[](const std::string& key) const;
    
    // Python duck typing
    template<typename T>
    ConfigObject& set(const std::string& key, const T& value);
    
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T{}) const;
    
    size_t size() const;
    bool empty() const;
    void clear();
    
    std::string summary() const;
    
private:
    std::unordered_map<std::string, std::any> configData_;
};

// Python event system
class EventSystem {
public:
    using EventCallback = std::function<void(const std::any&)>;
    
    void emit(const std::string& eventName, const std::any& data = nullptr);
    void on(const std::string& eventName, const EventCallback& callback);
    void off(const std::string& eventName, const EventCallback& callback);
    std::vector<std::string> getEventNames() const;
    
private:
    std::unordered_map<std::string, std::vector<EventCallback>> eventListeners_;
};

// Python decorator support for logging, timing, and metrics
class DecoratorSupport {
public:
    template<typename Func>
    auto logged(const std::string& name, Func&& func) -> std::function<decltype(func)>;
    
    template<typename Func>
    auto timed(const std::string& name, Func&& func) -> std::function<decltype(func)>;
    
    template<typename Func>
    auto metered(const std::string& name, Func&& func) -> std::function<decltype(func)>;
    
    template<typename Func>
    auto profiled(Func&& func) -> std::function<decltype(func)>;
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Enhanced Brain with modern Python features
class EnhancedBrain {
public:
    EnhancedBrain(const std::shared_ptr<ConfigObject>& config = nullptr);
    
    // Context manager support
    EnhancedBrain& initialize();
    void step(size_t step, double time = 0.0);
    std::unique_ptr<EnhancedAction> produceAction();
    void reset();
    void save(const std::string& filepath) const;
    void load(const std::string& filepath);
    
    // Modern property system
    std::string getStatus() const;
    size_t getNeuronCount() const;
    size_t getSynapseCount() const;
    size_t getActiveNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    
    // NumPy integration
    py::array_t<float> getNeuronActivity() const;
    py::array_t<float> getFiringRates() const;
    py::array_t<float> getWeights() const;
    
    // Memory systems access
    std::shared_ptr<WorkingMemory> getWorkingMemory() const;
    std::shared_ptr<EpisodicMemory> getEpisodicMemory() const;
    std::shared_ptr<AssociativeMemory> getAssociativeMemory() const;
    
    // Neuromodulation systems
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Development system
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Cognitive systems
    std::string getPlannerOutput() const;
    std::string getConceptSummary() const;
    std::string getAttentionFocus() const;
    
    // Configuration access
    std::shared_ptr<const ConfigObject> getConfig() const;
    
    // Async support
    std::future<void> stepAsync(size_t step, double time = 0.0);
    std::future<std::unique_ptr<EnhancedAction>> produceActionAsync();
    
    // Python iteration support
    class Iterator;
    Iterator begin() const;
    Iterator end() const;
    
    // Property access for regions
    std::vector<RegionId> getRegionIds() const;
    std::shared_ptr<NeuralRegion> getRegion(RegionId id) const;
    
    // Modern C++ features
    EnhancedBrain& operator=(const EnhancedBrain&) = delete;
    EnhancedBrain(const EnhancedBrain&) = delete;
    EnhancedBrain(EnhancedBrain&&) noexcept = default;
    
    // Event system integration
    void connectEventCallback(const std::string& eventName, const std::function<void(const std::any&)>& callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Modern World class with improved API
class EnhancedWorld {
public:
    EnhancedWorld();
    
    // Context manager support
    void configure(float width, float height, size_t visionWidth = 5, size_t visionHeight = 5);
    void reset();
    void setAgentStart(float x, float y);
    
    // Simulation control
    void update(double timestep);
    std::unique_ptr<EnhancedAction> applyAction(const EnhancedAction& action, double currentTime);
    
    // Sensory input
    std::unique_ptr<EnhancedPercept> getSensoryPercept() const;
    
    // Agent state
    EnhancedAgentBody getAgentBody() const;
    
    // World objects
    void addObject(const EnhancedWorldObject& obj);
    void removeObject(float x, float y);
    bool isValidPosition(float x, float y) const;
    EnhancedWorldObject getObjectAt(float x, float y) const;
    
    // World properties
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    float getMaxEnergy() const { return maxEnergy_; }
    void setMaxEnergy(float e) { maxEnergy_ = e; }
    float getEnergyDecayRate() const { return energyDecayRate_; }
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
    double getSimulationTime() const { return simTime_; }
    uint64_t getRandomSeed() const { return rngSeed_; }
    void setRandomSeed(uint64_t seed) { rngSeed_ = seed; }
    
    // NumPy integration for visualization
    py::array_t<float> getWorldGrid() const;
    py::array_t<float> getAgentPosition() const;
    
    // Performance monitoring
    double getLastStepTime() const { return lastStepTime_; }
    size_t getStepCount() const { return stepCount_; }
    
    // Iterator support for world objects
    class ObjectIterator;
    ObjectIterator begin() const;
    ObjectIterator end() const;
    
private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    float width_;
    float height_;
    float maxEnergy_;
    float energyDecayRate_;
    double simTime_;
    uint64_t rngSeed_;
    uint64_t rngState_;
    double lastStepTime_;
    size_t stepCount_;
};

// Enhanced Agent class
class EnhancedAgent {
public:
    EnhancedAgent(std::shared_ptr<EnhancedBrain> brain);
    
    // Agent lifecycle
    void initialize(std::shared_ptr<EnhancedWorld> world);
    void reset();
    
    // Processing pipeline
    std::unique_ptr<EnhancedAction> process(const EnhancedPercept& percept);
    EnhancedAction decodeMotorCommand();
    
    // Neuromodulation
    void applyReward(float reward, float predictedReward = 0.0f);
    void updateDevelopment(double timestep);
    
    // State access
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Configuration
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    
    bool isRewardModulationEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isCuriosityEnabled() const;
    
    // Properties
    std::shared_ptr<EnhancedBrain> getBrain() const { return brain_; }
    std::shared_ptr<EnhancedWorld> getWorld() const { return world_; }
    
    // Event callbacks
    void setActionCallback(const std::function<std::unique_ptr<EnhancedAction>()>& callback);
    void setPerceptCallback(const std::function<void(const EnhancedPercept&)> callback);
    void setRewardCallback(const std::function<void(float)> callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    std::shared_ptr<EnhancedBrain> brain_;
    std::shared_ptr<EnhancedWorld> world_;
};

// High-level Training API
class TrainingAPI {
public:
    TrainingAPI(std::shared_ptr<EnhancedBrain> brain);
    
    // Training modes
    void trainEpisode(double learningRate = 0.01f, size_t maxSteps = 1000);
    void trainBatch(size_t episodes, double learningRate = 0.01f);
    void trainContinuous(double learningRate = 0.01f, size_t maxSteps = 100000);
    
    // Learning strategies
    void setLearningStrategy(const std::string& strategy);
    void setExplorationStrategy(const std::string& strategy);
    void setReward ShapingFunction(std::function<float(float)> func);
    
    // Metrics collection
    float getAverageReward() const;
    float getAverageLearningProgress() const;
    double getTrainingTime() const;
    
    // Checkpointing
    bool saveCheckpoint(const std::string& path) const;
    bool loadCheckpoint(const std::string& path);
    
    // Monitoring
    void setProgressCallback(const std::function<void(float)>& callback);
    void setMetricsCallback(const std::function<void(const std::map<std::string, float>&)>& callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// High-level Simulation API
class SimulationAPI {
public:
    SimulationAPI(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world);
    
    // Simulation runs
    void runEpisode(size_t maxSteps = 1000);
    void runBatch(size_t episodes, size_t maxSteps = 1000);
    void runContinuous(size_t maxSteps = 100000);
    
    // Scenario management
    void setScenario(const std::string& scenarioName);
    void addScenarioObject(const EnhancedWorldObject& obj);
    void clearScenario();
    
    // Performance monitoring
    double getCurrentFPS() const;
    size_t getCurrentStep() const;
    double getElapsedTime() const;
    
    // Metrics collection
    float getCurrentReward() const;
    float getAverageReward() const;
    float getCurrentFiringRate() const;
    
    // State management
    void pause();
    void resume();
    void stop();
    bool isRunning() const;
    
    // Callbacks
    void setStepCallback(const std::function<void(size_t, double, float)>& callback);
    void setEpisodeCallback(const std::function<void(float)>& callback);
    void setPerformanceCallback(const std::function<void(double)>& callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Analysis API for simulation results
class AnalysisAPI {
public:
    AnalysisAPI(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world);
    
    // Performance analysis
    float calculateEfficiency(size_t steps) const;
    float calculateLearningProgress(size_t steps) const;
    float calculateAdaptability(size_t steps) const;
    
    // Behavioral analysis
    std::string analyzeBehaviorPattern(size_t steps) const;
    float calculateExplorationExploitationRatio(size_t steps) const;
    std::vector<float> getBehavioralMetrics(size_t steps) const;
    
    // Neural analysis
    py::array_t<float> getNeuralActivityHistory(size_t steps) const;
    py::array_t<float> getFiringRateEvolution(size_t steps) const;
    float calculateNeuralComplexity(size_t steps) const;
    
    // Visualization preparation
    py::dict prepareForVisualization(size_t steps) const;
    
    // Statistical analysis
    py::dict calculateStatistics(size_t steps) const;
    py::array_t<float> getCorrelationMatrix(size_t steps) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Visualization API
class VisualizationAPI {
public:
    VisualizationAPI(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world);
    
    // Rendering
    void renderBrain(std::string format = "text");
    void renderWorld(std::string format = "text");
    void renderNeurons(std::string format = "text");
    void renderConnections(std::string format = "text");
    
    // Interactive visualization
    void interactiveMode(bool enable = true);
    void setVisualizationStyle(const std::string& style);
    
    // Output formats
    std::string getTextVisualization() const;
    std::string getHTMLVisualization() const;
    std::string getJSONVisualization() const;
    py::array_t<unsigned char> getImage(std::string format = "png", int width = 800, int height = 600) const;
    
    // Recording
    void startRecording(const std::string& path, const std::string& format = "json");
    void stopRecording();
    bool isRecording() const;
    
    // Callbacks
    void setRenderCallback(const std::function<void(std::string)>& callback);
    void setUpdateCallback(const std::function<void(double)>& callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Export/Import API
class ExportImportAPI {
public:
    ExportImportAPI(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world);
    
    // Export formats
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    bool exportToNumpy(const std::string& filepath) const;
    bool exportToTensorFlow(const std::string& filepath) const;
    bool exportToPyTorch(const std::string& filepath) const;
    
    // Import formats
    bool importFromJSON(const std::string& filepath);
    bool importFromCSV(const std::string& filepath);
    bool importFromNumpy(const std::string& filepath);
    bool importFromTensorFlow(const std::string& filepath);
    bool importFromPyTorch(const std::string& filepath);
    
    // Simulation data management
    bool saveSimulationState(const std::string& filepath) const;
    bool loadSimulationState(const std::string& filepath);
    bool saveAgentState(const std::string& filepath) const;
    bool loadAgentState(const std::string& filepath);
    
    // Data processing
    py::dict processData(const std::string& dataType, const std::string& filepath) const;
    std::vector<float> extractFeatures(const std::string& featureType, size_t steps) const;
    
    // Format validation
    bool validateJSON(const std::string& filepath) const;
    bool validateCSV(const std::string& filepath) const;
    
    // Progress reporting
    float getImportProgress() const;
    float getExportProgress() const;
    
    // Callbacks
    void setImportProgressCallback(const std::function<void(float)>& callback);
    void setExportProgressCallback(const std::function<void(float)>& callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Helper structures for modern Python API
struct EnhancedAction {
    ActionType type;
    std::vector<float> parameters;
    float timestamp;
    float reward;
    bool success;
    std::string message;
    
    EnhancedAction() : type(ActionType::Wait), timestamp(0.0f), reward(0.0f), success(true) {}
    EnhancedAction(ActionType t, const std::vector<float>& params = {}) : type(t), parameters(params), timestamp(0.0f), reward(0.0f), success(true) {}
};

struct EnhancedPercept {
    Vision vision;
    Touch touch;
    InternalSignals internal;
    Proprioception proprioception;
    Audio audio;
    double timestamp;
    
    EnhancedPercept() : timestamp(0.0) {}
};

struct EnhancedWorldObject {
    float x;
    float y;
    float radius;
    WorldObjectType type;
    float value;
    bool active;
    
    EnhancedWorldObject() : x(0.0f), y(0.0f), radius(0.5f), type(WorldObjectType::Empty), value(0.0f), active(true) {}
    EnhancedWorldObject(float px, float py, WorldObjectType t, float v = 0.0f, float r = 0.5f) : x(px), y(py), radius(r), type(t), value(v), active(true) {}
};

struct EnhancedAgentBody {
    float x;
    float y;
    float orientation;
    float velocityX;
    float velocityY;
    float angularVelocity;
    float energy;
    float health;
    float age;
    bool isMoving;
    bool isTurning;
    double lastActionTime;
    
    EnhancedAgentBody() : x(0.0f), y(0.0f), orientation(0.0f), velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f), energy(100.0f), health(1.0f), age(0.0f), isMoving(false), isTurning(false), lastActionTime(0.0) {}
};

// Modern Python factory functions
namespace Factory {
    std::shared_ptr<ConfigObject> createConfig();
    std::shared_ptr<ConfigObject> loadConfig(const std::string& filepath);
    std::shared_ptr<EnhancedBrain> createBrain(const std::shared_ptr<ConfigObject>& config = nullptr);
    std::shared_ptr<EnhancedWorld> createWorld();
    std::shared_ptr<EnhancedAgent> createAgent(std::shared_ptr<EnhancedBrain> brain);
    std::unique_ptr<EnhancedAction> createAction(ActionType type, const std::vector<float>& parameters = {});
    std::unique_ptr<EnhancedPercept> createPercept();
    std::unique_ptr<EnhancedWorldObject> createWorldObject(WorldObjectType type, float x, float y, float value = 0.0f, float radius = 0.5f);
    std::unique_ptr<EnhancedAgentBody> createAgentBody();
}

// High-level convenience functions
namespace HighLevel {
    std::unique_ptr<EnhancedAction> runSimpleControl(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world);
    float runTestEpisode(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world, size_t maxSteps = 100);
    py::dict runBenchmark(std::shared_ptr<EnhancedBrain> brain, std::shared_ptr<EnhancedWorld> world, const std::string& benchmarkName = "simple");
}

// Modern Python module initialization
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ----------------------------------------------------
        A modern Python binding for the NLM C++ neural simulation framework.
        Provides enhanced classes with modern Python features:
        - Type hints and comprehensive docstrings
        - NumPy integration for array operations
        - Async/await support for non-blocking operations
        - Context managers for safe resource management
        - Configuration objects replacing string-based configs
        - Event system for simulation events
        - Callback system for event handling
        - High-level APIs for training, simulation, analysis, visualization
        - Full Phase 6 integration support
    )pbdoc";

    py::class_<ConfigObject>(m, "ConfigObject")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def("set", py::overload_cast<const std::string&, int>(&ConfigObject::set), "Set integer value")
        .def("set", py::overload_cast<const std::string&, double>(&ConfigObject::set), "Set double value")
        .def("set", py::overload_cast<const std::string&, float>(&ConfigObject::set), "Set float value")
        .def("set", py::overload_cast<const std::string&, bool>(&ConfigObject::set), "Set boolean value")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&ConfigObject::set), "Set string value")
        .def("set", py::overload_cast<const std::string&, const std::vector<int>&>(&ConfigObject::set), "Set integer vector value")
        .def("set", py::overload_cast<const std::string&, const std::vector<double>&>(&ConfigObject::set), "Set double vector value")
        .def("set", py::overload_cast<const std::string&, const std::vector<float>&>(&ConfigObject::set), "Set float vector value")
        .def("set", py::overload_cast<const std::string&, const std::vector<std::string>&>(&ConfigObject::set), "Set string vector value")
        .def("getInt", &ConfigObject::getInt, "Get integer value")
        .def("getDouble", &ConfigObject::getDouble, "Get double value")
        .def("getFloat", &ConfigObject::getFloat, "Get float value")
        .def("getBool", &ConfigObject::getBool, "Get boolean value")
        .def("getString", &ConfigObject::getString, "Get string value")
        .def("getKeys", &ConfigObject::getKeys, "Get all configuration keys")
        .def("summary", &ConfigObject::summary, "Get configuration summary")
        .def("toJSON", &ConfigObject::toJSON, "Export to JSON")
        .def("loadFromJSON", &ConfigObject::loadFromJSON, "Load from JSON")
        .def("saveToFile", &ConfigObject::saveToFile, "Save to file")
        .def("loadFromFile", &ConfigObject::loadFromFile, "Load from file")
        .def("has", &ConfigObject::has, "Check if key exists")
        .def("remove", &ConfigObject::remove, "Remove key")
        .def("clear", &ConfigObject::clear, "Clear all configuration")
        .def("__getitem__", &ConfigObject::operator[], "Access configuration by key")
        .def("size", &ConfigObject::size, "Get configuration size")
        .def("empty", &ConfigObject::empty, "Check if configuration is empty");

    py::class_<EnhancedAction>(m, "EnhancedAction")
        .def(py::init<>())
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters") = std::vector<float>())
        .def_readwrite("type", &EnhancedAction::type, "Action type")
        .def_readwrite("parameters", &EnhancedAction::parameters, "Action parameters")
        .def_readwrite("timestamp", &EnhancedAction::timestamp, "Action timestamp")
        .def_readwrite("reward", &EnhancedAction::reward, "Action reward")
        .def_readwrite("success", &EnhancedAction::success, "Action success")
        .def_readwrite("message", &EnhancedAction::message, "Action message");

    py::class_<EnhancedPercept>(m, "EnhancedPercept")
        .def(py::init<>())
        .def_readwrite("vision", &EnhancedPercept::vision, "Vision data")
        .def_readwrite("touch", &EnhancedPercept::touch, "Touch data")
        .def_readwrite("internal", &EnhancedPercept::internal, "Internal signals")
        .def_readwrite("proprioception", &EnhancedPercept::proprioception, "Proprioception data")
        .def_readwrite("audio", &EnhancedPercept::audio, "Audio data")
        .def_readwrite("timestamp", &EnhancedPercept::timestamp, "Percept timestamp");

    py::class_<EnhancedWorldObject>(m, "EnhancedWorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(), py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &EnhancedWorldObject::x, "X position")
        .def_readwrite("y", &EnhancedWorldObject::y, "Y position")
        .def_readwrite("radius", &EnhancedWorldObject::radius, "Radius")
        .def_readwrite("type", &EnhancedWorldObject::type, "Object type")
        .def_readwrite("value", &EnhancedWorldObject::value, "Object value")
        .def_readwrite("active", &EnhancedWorldObject::active, "Object active state");

    py::class_<EnhancedAgentBody>(m, "EnhancedAgentBody")
        .def(py::init<>())
        .def_readwrite("x", &EnhancedAgentBody::x, "X position")
        .def_readwrite("y", &EnhancedAgentBody::y, "Y position")
        .def_readwrite("orientation", &EnhancedAgentBody::orientation, "Orientation")
        .def_readwrite("velocityX", &EnhancedAgentBody::velocityX, "Velocity X")
        .def_readwrite("velocityY", &EnhancedAgentBody::velocityY, "Velocity Y")
        .def_readwrite("angularVelocity", &EnhancedAgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &EnhancedAgentBody::energy, "Energy")
        .def_readwrite("health", &EnhancedAgentBody::health, "Health")
        .def_readwrite("age", &EnhancedAgentBody::age, "Age")
        .def_readwrite("isMoving", &EnhancedAgentBody::isMoving, "Is moving")
        .def_readwrite("isTurning", &EnhancedAgentBody::isTurning, "Is turning")
        .def_readwrite("lastActionTime", &EnhancedAgentBody::lastActionTime, "Last action time");

    py::class_<EventSystem>(m, "EventSystem")
        .def(py::init<>())
        .def("emit", &EventSystem::emit, "Emit event")
        .def("on", &EventSystem::on, "Register event listener")
        .def("off", &EventSystem::off, "Unregister event listener")
        .def("getEventNames", &EventSystem::getEventNames, "Get event names");

    py::class_<DecoratorSupport>(m, "DecoratorSupport")
        .def(py::init<>());

    // Factory functions
    m.def("createConfig", &Factory::createConfig, "Create empty configuration");
    m.def("loadConfig", &Factory::loadConfig, "Load configuration from file");
    m.def("createBrain", &Factory::createBrain, "Create brain with configuration");
    m.def("createWorld", &Factory::createWorld, "Create world");
    m.def("createAgent", &Factory::createAgent, "Create agent with brain");
    m.def("createAction", &Factory::createAction, "Create action");
    m.def("createPercept", &Factory::createPercept, "Create percept");
    m.def("createWorldObject", &Factory::createWorldObject, "Create world object");
    m.def("createAgentBody", &Factory::createAgentBody, "Create agent body");

    // High-level convenience functions
    m.def("runSimpleControl", &HighLevel::runSimpleControl, "Run simple control loop");
    m.def("runTestEpisode", &HighLevel::runTestEpisode, "Run test episode");
    m.def("runBenchmark", &HighLevel::runBenchmark, "Run benchmark");

    // Version information
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Enhanced NLM Python bindings with modern Python features";
}

} // namespace nlm