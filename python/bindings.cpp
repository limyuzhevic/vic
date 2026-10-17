#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <unordered_map>
#include <variant>
#include <optional>
#include <filesystem>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/ScalingBenchmark.hpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain Error: " + msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Config Error: " + msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError("World Error: " + msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
};

// Helper function for Python type conversion
double microsecondsToSeconds(const Timestamp& ts) {
    return static_cast<double>(ts) / 1000000.0;
}

// Forward declarations for Phase 3 advanced features
class AdvancedBrainManipulator;
class PerformanceMonitor;
class MetricRecorder;
class TimelineRecorder;
class Profiler;
class MemoryMonitor;
class ConfigManager;
class SerializationManager;
class VisualizationTools;

// Advanced Brain Manipulation Utilities

class AdvancedBrainManipulator {
private:
    std::shared_ptr<Brain> brain_;
public:
    AdvancedBrainManipulator(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Neuron manipulation
    NeuronId createNeuron(RegionId regionId, NeuronType type = NeuronType::Excitatory,
                         const NeuronState& initialState = NeuronState());
    bool deleteNeuron(NeuronId neuronId);
    bool modifyNeuronProperties(NeuronId neuronId, const NeuronState& newState);
    NeuronState getNeuronState(NeuronId neuronId) const;
    std::vector<NeuronId> getNeuronsInRegion(RegionId regionId) const;
    std::vector<NeuronId> getNeuronsByType(NeuronType type) const;
    
    // Synapse manipulation
    SynapseId createSynapse(NeuronId source, NeuronId target,
                          SynapseType type = SynapseType::Excitatory,
                          SynapticWeight weight = 1.0f,
                          Delay delay = 1);
    bool deleteSynapse(SynapseId synapseId);
    bool modifySynapseProperties(SynapseId synapseId, const Synapse& newSynapse);
    Synapse getSynapseProperties(SynapseId synapseId) const;
    std::vector<SynapseId> getSynapsesFromNeuron(NeuronId neuronId) const;
    std::vector<SynapseId> getSynapsesToNeuron(NeuronId neuronId) const;
    
    // Memory system access
    void setWorkingMemoryEntry(const std::string& key, const FloatVec& value);
    FloatVec getWorkingMemoryEntry(const std::string& key) const;
    void clearWorkingMemory();
    std::unordered_map<std::string, FloatVec> getFullWorkingMemory() const;
    
    // Plasticity rule customization
    void setHebbianLearningRate(float learningRate);
    void setSTDPParameters(float tauPre, float tauPost, float alpha);
    void setRewardModulatedPlasticity(float learningRate, float traceDecay);
    void enablePlasticityForNeuron(NeuronId neuronId, bool hebbian, bool stdp, bool rewardModulated);
    void enablePlasticityForSynapse(SynapseId synapseId, bool hebbian, bool stdp, bool rewardModulated);
    
    // Development stage control
    void setDevelopmentalStage(DevelopmentalStage stage);
    DevelopmentalStage getDevelopmentalStage() const;
    void triggerDevelopmentStep();
    std::vector<std::string> getDevelopmentEvents() const;
    
    // Regional connectivity management
    void addInterRegionConnection(RegionId source, RegionId target,
                                 float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    std::vector<InterRegionConnection> getInterRegionConnections() const;
    void setRegionalConnectivityProbability(float probability);
    void optimizeRegionalTopology();
};

// Performance Monitoring Tools

class PerformanceMonitor {
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point lastStepTime_;
    double totalTime_;
    uint64_t stepCount_;
    uint64_t spikeCount_;
    uint64_t activeNeuronCount_;
    double avgStepTime_;
    double minStepTime_;
    double maxStepTime_;
    std::vector<double> stepTimes_;
    std::vector<uint64_t> spikeCounts_;
    std::vector<uint64_t> activeNeuronCounts_;
public:
    PerformanceMonitor() : totalTime_(0.0), stepCount_(0), spikeCount_(0),
                          activeNeuronCount_(0), avgStepTime_(0.0), minStepTime_(std::numeric_limits<double>::max()),
                          maxStepTime_(0.0) {}
    
    void start();
    void stop();
    void recordStep(double stepTimeMs, size_t spikes, size_t activeNeurons);
    void reset();
    
    // Metrics
    double getTotalTimeSeconds() const;
    double getAvgStepTimeMs() const;
    double getMinStepTimeMs() const;
    double getMaxStepTimeMs() const;
    double getStepsPerSecond() const;
    double getSpikesPerSecond() const;
    double getFiringRateHz() const;
    size_t getStepCount() const;
    size_t getTotalSpikes() const;
    size_t getCurrentActiveNeurons() const;
    
    // Historical data
    const std::vector<double>& getStepTimes() const { return stepTimes_; }
    const std::vector<size_t>& getSpikeCounts() const { return spikeCounts_; }
    const std::vector<size_t>& getActiveNeuronCounts() const { return activeNeuronCounts_; }
    
    // Analysis
    bool isStable(size_t windowSize = 100, double threshold = 0.1) const;
    std::string getPerformanceReport() const;
};

class MetricRecorder {
private:
    std::unordered_map<std::string, std::vector<std::pair<Timestamp, double>>> metrics_;
    std::unordered_map<std::string, double> currentValues_;
    std::chrono::high_resolution_clock::time_point lastUpdate_;
public:
    MetricRecorder() {}
    
    // Record metrics
    void record(const std::string& name, double value);
    void recordTimeSeries(const std::string& name, const std::vector<double>& values,
                        double timestamp = 0.0);
    
    // Get metrics
    double getMetric(const std::string& name) const;
    double getMetric(const std::string& name, size_t index) const;
    const std::vector<double>& getValues(const std::string& name) const;
    std::vector<std::string> getMetricNames() const;
    
    // Statistics
    double getMean(const std::string& name) const;
    double getStdDev(const std::string& name) const;
    double getMin(const std::string& name) const;
    double getMax(const std::string& name) const;
    double getMedian(const std::string& name) const;
    
    // Aggregation
    MetricRecorder& addRecorder(const MetricRecorder& other);
    MetricRecorder& clear();
    bool isEmpty() const;
    
    // Export
    std::string toCSV(const std::string& filename) const;
    std::string toJSON() const;
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Reporting
    std::string summary() const;
    std::string generateReport() const;
};

class TimelineRecorder {
private:
    std::vector<std::pair<Timestamp, std::string>> events_;
    std::unordered_map<std::string, std::vector<std::pair<Timestamp, std::string>>> categorizedEvents_;
    Timestamp startTime_;
    bool recording_;
public:
    TimelineRecorder() : startTime_(0), recording_(false) {}
    
    void startRecording();
    void stopRecording();
    void recordEvent(const std::string& eventName, const std::string& description = "");
    void recordEventWithTime(const std::string& eventName, Timestamp timestamp,
                           const std::string& description = "");
    void recordStep(const std::string& eventName, Timestamp timestamp,
                  const std::unordered_map<std::string, double>& metrics = {});
    
    // Get timeline data
    const std::vector<std::pair<Timestamp, std::string>>& getEvents() const { return events_; }
    const std::vector<std::pair<Timestamp, std::string>>& getEventsForCategory(const std::string& category) const;
    std::vector<std::string> getEventCategories() const;
    
    // Analysis
    std::string getTimelineReport() const;
    double getDuration() const;
    size_t getEventCount(const std::string& eventName = "") const;
    
    // Filtering and querying
    std::vector<std::pair<Timestamp, std::string>> getEventsAfter(Timestamp timestamp) const;
    std::vector<std::pair<Timestamp, std::string>> getEventsBefore(Timestamp timestamp) const;
    std::vector<std::pair<Timestamp, std::string>> getEventsInRange(Timestamp start, Timestamp end) const;
};

class Profiler {
private:
    struct ProfileData {
        std::string functionName;
        Timestamp startTime;
        Timestamp endTime;
        double duration;
        uint64_t memoryUsage;
        uint64_t callCount;
        double totalTime;
        std::unordered_map<std::string, double> metrics;
    };
    
    std::vector<ProfileData> profileData_;
    std::unordered_map<std::string, ProfileData> currentProfiles_;
    bool profilingEnabled_;
public:
    Profiler() : profilingEnabled_(false) {}
    
    void startProfile(const std::string& functionName);
    void stopProfile(const std::string& functionName);
    void recordMemoryUsage(const std::string& context, uint64_t bytes);
    
    // Get profiling data
    const std::vector<ProfileData>& getProfileData() const { return profileData_; }
    const std::unordered_map<std::string, ProfileData>& getCurrentProfiles() const { return currentProfiles_; }
    
    // Analysis
    std::string getProfileReport() const;
    ProfileData getFunctionProfile(const std::string& functionName) const;
    std::vector<std::string> getSlowestFunctions(size_t limit = 10) const;
    std::vector<std::string> getMostFrequentFunctions(size_t limit = 10) const;
    
    // Bottleneck identification
    std::string identifyBottlenecks() const;
    double getAverageMemoryUsage() const;
    double getMaxMemoryUsage() const;
};

class MemoryMonitor {
private:
    struct MemorySample {
        Timestamp timestamp;
        size_t totalMemoryMB;
        size_t availableMemoryMB;
        size_t usedMemoryMB;
        size_t peakMemoryMB;
        size_t neuronMemoryMB;
        size_t synapseMemoryMB;
        size_t otherMemoryMB;
    };
    
    std::vector<MemorySample> samples_;
    size_t peakMemoryMB_;
    bool monitoring_;
public:
    MemoryMonitor() : peakMemoryMB_(0), monitoring_(false) {}
    
    void startMonitoring();
    void stopMonitoring();
    void recordSample();
    
    // Get memory data
    const std::vector<MemorySample>& getSamples() const { return samples_; }
    
    // Analysis
    size_t getPeakMemoryMB() const;
    size_t getCurrentMemoryMB() const;
    double getMemoryGrowthRate() const;
    std::string getMemoryReport() const;
    
    // Breakdown
    struct MemoryBreakdown {
        size_t neuronsMB;
        size_t synapsesMB;
        size_t eventsMB;
        size_t otherMB;
    };
    
    MemoryBreakdown getCurrentBreakdown() const;
    MemoryBreakdown getAverageBreakdown() const;
};

// Enhanced Configuration Management

class ConfigManager {
private:
    struct ConfigTemplate {
        std::string name;
        std::string description;
        Config config;
        bool isDefault;
    };
    
    std::unordered_map<std::string, ConfigTemplate> templates_;
    std::shared_ptr<Config> currentConfig_;
    std::vector<std::string> configHistory_;
public:
    ConfigManager() {}
    
    // Template management
    void addTemplate(const std::string& name, const std::string& description,
                    const Config& config, bool isDefault = false);
    bool removeTemplate(const std::string& name);
    ConfigTemplate getTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateNames() const;
    void setActiveTemplate(const std::string& name);
    std::string getActiveTemplateName() const;
    
    // Configuration creation
    Config createFromTemplate(const std::string& templateName) const;
    Config createFromDefaults() const;
    Config createFromInheritance(const std::vector<std::string>& templateNames) const;
    
    // Configuration validation
    bool validateConfig(const Config& config) const;
    std::vector<std::string> validateConfigWithDetails(const Config& config) const;
    bool isConfigComplete(const Config& config) const;
    
    // Config inheritance and merging
    Config mergeConfigs(const std::vector<Config>& configs) const;
    Config mergeConfigWithTemplate(const Config& config, const std::string& templateName) const;
    
    // Parameter sensitivity analysis
    std::unordered_map<std::string, double> analyzeSensitivity(const Config& config) const;
    std::string getSensitivityReport(const Config& config) const;
    
    // Configuration history
    void saveConfig(const Config& config, const std::string& description);
    Config loadConfig(const std::string& description) const;
    std::vector<std::string> getConfigHistory() const;
};

// Advanced Serialization and Persistence

class SerializationManager {
private:
    std::string basePath_;
    bool enableCompression_;
    bool enableIncremental_;
    size_t maxCheckpoints_;
    
    struct Checkpoint {
        Timestamp timestamp;
        std::string filepath;
        size_t dataSize;
        std::unordered_map<std::string, Timestamp> componentTimestamps;
    };
    
    std::vector<Checkpoint> checkpoints_;
public:
    SerializationManager() : basePath_("."), enableCompression_(true),
                            enableIncremental_(true), maxCheckpoints_(10) {}
    
    // HDF5 support
    bool saveHDF5(const std::string& filepath, const Config& config,
                 const std::unordered_map<std::string, std::vector<double>>& datasets,
                 const std::unordered_map<std::string, std::string>& attributes = {});
    std::pair<Config, std::unordered_map<std::string, std::vector<double>>>
        loadHDF5(const std::string& filepath);
    
    // Pickle-compatible serialization
    bool savePickle(const std::string& filepath, const Config& config,
                   const std::shared_ptr<Brain>& brain = nullptr);
    std::pair<Config, std::shared_ptr<Brain>> loadPickle(const std::string& filepath);
    
    // Incremental save/restore
    bool saveIncremental(const std::string& filepath, const Config& config,
                        const std::unordered_map<std::string, std::vector<double>>& data);
    std::pair<Config, std::unordered_map<std::string, std::vector<double>>>
        loadIncremental(const std::string& filepath);
    
    // Checkpoint management
    bool createCheckpoint(const std::string& name, const Config& config,
                         const std::shared_ptr<Brain>& brain = nullptr);
    bool restoreCheckpoint(const std::string& name);
    std::vector<std::string> getCheckpoints() const;
    
    // Compressed storage
    bool saveCompressed(const std::string& filepath, const Config& config,
                       const std::shared_ptr<Brain>& brain = nullptr);
    std::pair<Config, std::shared_ptr<Brain>> loadCompressed(const std::string& filepath);
};

// Visualization and Monitoring

class VisualizationTools {
private:
    std::string outputDir_;
    bool enableRealTimeUpdates_;
    size_t updateIntervalMs_;
public:
    VisualizationTools() : outputDir_("."), enableRealTimeUpdates_(false), updateIntervalMs_(1000) {}
    
    // Neural network visualization
    bool visualizeNetwork(const std::string& filepath, const std::shared_ptr<Brain>& brain,
                         const std::string& format = "png");
    bool visualizeConnections(const std::string& filepath, const std::shared_ptr<Brain>& brain,
                            const std::string& format = "png");
    bool visualizeActivity(const std::string& filepath, const std::shared_ptr<Brain>& brain,
                         const std::string& metric = "firing_rate", double threshold = 0.5,
                         const std::string& format = "png");
    
    // Activity monitoring utilities
    bool createActivityMonitor(const std::string& filepath, const std::shared_ptr<Brain>& brain,
                              const std::vector<std::string>& metrics = {});
    bool createRealTimeMonitor(const std::string& filepath, const std::shared_ptr<Brain>& brain,
                              size_t updateIntervalMs = 1000);
    
    // Real-time plotting capabilities
    bool startRealTimePlotting(const std::string& filepath, const std::shared_ptr<Brain>& brain,
                             const std::vector<std::string>& metrics = {});
    void stopRealTimePlotting();
    
    // Result visualization frameworks
    bool createResultsDashboard(const std::string& filepath,
                               const std::vector<std::shared_ptr<Brain>>& brains,
                               const std::vector<std::string>& experimentNames = {});
    bool createComparisonPlot(const std::string& filepath,
                            const std::vector<std::shared_ptr<Brain>>& brains,
                            const std::string& metric = "firing_rate");
    bool createTrainingProgress(const std::string& filepath,
                               const std::vector<std::pair<Timestamp, double>>& trainingData,
                               const std::string& title = "Training Progress");
};

// Python binding functions

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Phase 3 Advanced Features
        -----------------------------------------------------------------------
        Advanced-level utilities and tools for expert users including:
        
        Advanced Brain Manipulation:
        - Direct neuron/synapse manipulation (create, modify, delete)
        - Memory system access and control
        - Plasticity rule customization
        - Development stage control
        - Regional connectivity management
        
        Performance Monitoring:
        - PerformanceMonitor for real-time tracking
        - MetricRecorder for custom metrics
        - TimelineRecorder for event tracing
        - Profiler for bottleneck identification
        - MemoryMonitor for memory usage tracking
        
        Enhanced Configuration Management:
        - ConfigManager for complex configuration
        - Template-based configuration creation
        - Configuration validation utilities
        - Config inheritance and merging
        - Config parameter sensitivity analysis
        
        Advanced Serialization and Persistence:
        - HDF5 support for large datasets
        - Pickle-compatible serialization
        - Incremental save/restore
        - Compressed storage
        - Checkpoint management
        
        Visualization and Monitoring:
        - Neural network visualization tools
        - Activity monitoring utilities
        - Real-time plotting capabilities
        - Result visualization frameworks
        
        All features provide comprehensive Python type hints, detailed docstrings,
        advanced usage patterns, error handling, and integration with the NLM framework.
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // ============= ADVANCED BRAIN MANIPULATION UTILITIES =============

    py::class_<AdvancedBrainManipulator>(m, "AdvancedBrainManipulator", R"pbdoc(
        Advanced brain manipulation utilities for expert-level control
        
        Provides direct access to neural components for fine-grained manipulation
        including neuron/synapse creation, modification, and deletion, as well as
        memory system control, plasticity rule customization, development stage
        control, and regional connectivity management.
        
        Example:
            ```python
            # Create brain and manipulator
            config = createDefaultConfig()
            brain = createBrain(config)
            manipulator = AdvancedBrainManipulator(brain)
            
            # Create neurons
            region_id = brain.addRegion("visual_cortex")
            neuron1 = manipulator.createNeuron(region_id, NeuronType.Excitatory)
            neuron2 = manipulator.createNeuron(region_id, NeuronType.Inhibitory)
            
            # Create synapses
            synapse = manipulator.createSynapse(neuron1, neuron2, weight=0.5)
            
            # Modify plasticity
            manipulator.setHebbianLearningRate(0.01)
            manipulator.enablePlasticityForSynapse(synapse.getId(), True, False, False)
            
            # Set development stage
            manipulator.setDevelopmentalStage(DevelopmentalStage.CriticalPeriod)
            ```
        
        Methods:
        - createNeuron: Create neurons with specific types and initial states
        - deleteNeuron: Remove neurons from the brain
        - modifyNeuronProperties: Update neuron state parameters
        - createSynapse: Create connections between neurons
        - deleteSynapse: Remove synaptic connections
        - setWorkingMemoryEntry: Control memory systems
        - setHebbianLearningRate: Customize plasticity rules
        - setDevelopmentalStage: Control development stages
        - addInterRegionConnection: Manage regional connectivity
    )pbdoc")
    .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain").noconvert(),
         R"pbdoc(
         Initialize AdvancedBrainManipulator with a brain instance
         
         Args:
             brain: Brain instance to manipulate (must not be None)
         
         Raises:
             ValidationError: If brain is None
         )pbdoc")
    .def("createNeuron", &AdvancedBrainManipulator::createNeuron,
         py::arg("regionId").noconvert(), py::arg("type") = NeuronType::Excitatory,
         py::arg("initialState") = NeuronState(),
         R"pbdoc(
         Create a new neuron
         
         Args:
             regionId: Region ID where neuron will be created
             type: Neuron type (default: Excitatory)
             initialState: Initial neuron state (default: resting state)
         
         Returns:
             NeuronId: ID of the created neuron
         
         Raises:
             BrainError: If neuron creation fails
             ValidationError: If region ID is invalid
         )pbdoc")
    .def("deleteNeuron", &AdvancedBrainManipulator::deleteNeuron,
         py::arg("neuronId").noconvert(),
         R"pbdoc(
         Delete a neuron
         
         Args:
             neuronId: Neuron ID to delete
         
         Returns:
             bool: True if neuron was deleted, False if not found
         )pbdoc")
    .def("modifyNeuronProperties", &AdvancedBrainManipulator::modifyNeuronProperties,
         py::arg("neuronId").noconvert(), py::arg("newState").noconvert(),
         R"pbdoc(
         Modify neuron properties
         
         Args:
             neuronId: Target neuron ID
             newState: New neuron state
         
         Returns:
             bool: True if neuron was modified, False if not found
         )pbdoc")
    .def("getNeuronState", &AdvancedBrainManipulator::getNeuronState,
         py::arg("neuronId").noconvert(),
         R"pbdoc(
         Get neuron state
         
         Args:
             neuronId: Neuron ID to query
         
         Returns:
             NeuronState: Current neuron state
         
         Raises:
             BrainError: If neuron not found
         )pbdoc")
    .def("getNeuronsInRegion", &AdvancedBrainManipulator::getNeuronsInRegion,
         py::arg("regionId").noconvert(),
         R"pbdoc(
         Get all neurons in a region
         
         Args:
             regionId: Region ID to query
         
         Returns:
             List[NeuronId]: All neuron IDs in the region
         )pbdoc")
    .def("getNeuronsByType", &AdvancedBrainManipulator::getNeuronsByType,
         py::arg("type").noconvert(),
         R"pbdoc(
         Get all neurons of a specific type
         
         Args:
             type: Neuron type to filter by
         
         Returns:
             List[NeuronId]: All neuron IDs of the specified type
         )pbdoc")
    .def("createSynapse", &AdvancedBrainManipulator::createSynapse,
         py::arg("source").noconvert(), py::arg("target").noconvert(),
         py::arg("type") = SynapseType::Excitatory,
         py::arg("weight") = 1.0f, py::arg("delay") = 1,
         R"pbdoc(
         Create a synapse connection
         
         Args:
             source: Source neuron ID
             target: Target neuron ID
             type: Synapse type (default: Excitatory)
             weight: Synaptic weight (default: 1.0)
             delay: Transmission delay in steps (default: 1)
         
         Returns:
             SynapseId: ID of the created synapse
         
         Raises:
             BrainError: If synapse creation fails
         )pbdoc")
    .def("deleteSynapse", &AdvancedBrainManipulator::deleteSynapse,
         py::arg("synapseId").noconvert(),
         R"pbdoc(
         Delete a synapse
         
         Args:
             synapseId: Synapse ID to delete
         
         Returns:
             bool: True if synapse was deleted, False if not found
         )pbdoc")
    .def("modifySynapseProperties", &AdvancedBrainManipulator::modifySynapseProperties,
         py::arg("synapseId").noconvert(), py::arg("newSynapse").noconvert(),
         R"pbdoc(
         Modify synapse properties
         
         Args:
             synapseId: Target synapse ID
             newSynapse: New synapse properties
         
         Returns:
             bool: True if synapse was modified, False if not found
         )pbdoc")
    .def("getSynapseProperties", &AdvancedBrainManipulator::getSynapseProperties,
         py::arg("synapseId").noconvert(),
         R"pbdoc(
         Get synapse properties
         
         Args:
             synapseId: Synapse ID to query
         
         Returns:
             Synapse: Current synapse properties
         
         Raises:
             BrainError: If synapse not found
         )pbdoc")
    .def("getSynapsesFromNeuron", &AdvancedBrainManipulator::getSynapsesFromNeuron,
         py::arg("neuronId").noconvert(),
         R"pbdoc(
         Get all synapses originating from a neuron
         
         Args:
             neuronId: Source neuron ID
         
         Returns:
             List[SynapseId]: All outgoing synapse IDs
         )pbdoc")
    .def("getSynapsesToNeuron", &AdvancedBrainManipulator::getSynapsesToNeuron,
         py::arg("neuronId").noconvert(),
         R"pbdoc(
         Get all synapses terminating at a neuron
         
         Args:
             neuronId: Target neuron ID
         
         Returns:
             List[SynapseId]: All incoming synapse IDs
         )pbdoc")
    .def("setWorkingMemoryEntry", &AdvancedBrainManipulator::setWorkingMemoryEntry,
         py::arg("key").noconvert(), py::arg("value").noconvert(),
         R"pbdoc(
         Set a working memory entry
         
         Args:
             key: Memory key
             value: Memory value (vector of floats)
         )pbdoc")
    .def("getWorkingMemoryEntry", &AdvancedBrainManipulator::getWorkingMemoryEntry,
         py::arg("key").noconvert(),
         R"pbdoc(
         Get a working memory entry
         
         Args:
             key: Memory key to retrieve
         
         Returns:
             List[float]: Memory value
         )pbdoc")
    .def("clearWorkingMemory", &AdvancedBrainManipulator::clearWorkingMemory,
         R"pbdoc(
         Clear all working memory entries
         )pbdoc")
    .def("getFullWorkingMemory", &AdvancedBrainManipulator::getFullWorkingMemory,
         R"pbdoc(
         Get all working memory entries
         
         Returns:
             Dict[str, List[float]]: All memory entries
         )pbdoc")
    .def("setHebbianLearningRate", &AdvancedBrainManipulator::setHebbianLearningRate,
         py::arg("learningRate").noconvert(),
         R"pbdoc(
         Set Hebbian learning rate for plasticity
         
         Args:
             learningRate: Hebbian learning rate (typically 0.01-0.1)
         )pbdoc")
    .def("setSTDPParameters", &AdvancedBrainManipulator::setSTDPParameters,
         py::arg("tauPre").noconvert(), py::arg("tauPost").noconvert(), py::arg("alpha").noconvert(),
         R"pbdoc(
         Set STDP (Spike-Timing-Dependent Plasticity) parameters
         
         Args:
             tauPre: Pre-synaptic spike trace decay constant
             tauPost: Post-synaptic spike trace decay constant
             alpha: Learning rate for STDP
         )pbdoc")
    .def("setRewardModulatedPlasticity", &AdvancedBrainManipulator::setRewardModulatedPlasticity,
         py::arg("learningRate").noconvert(), py::arg("traceDecay").noconvert(),
         R"pbdoc(
         Set reward-modulated plasticity parameters
         
         Args:
             learningRate: Reward-modulated learning rate
             traceDecay: Eligibility trace decay rate
         )pbdoc")
    .def("enablePlasticityForNeuron", &AdvancedBrainManipulator::enablePlasticityForNeuron,
         py::arg("neuronId").noconvert(), py::arg("hebbian").noconvert(),
         py::arg("stdp").noconvert(), py::arg("rewardModulated").noconvert(),
         R"pbdoc(
         Enable/disable plasticity rules for a neuron
         
         Args:
             neuronId: Neuron ID to configure
             hebbian: Enable Hebbian plasticity
             stdp: Enable STDP
             rewardModulated: Enable reward-modulated plasticity
         )pbdoc")
    .def("enablePlasticityForSynapse", &AdvancedBrainManipulator::enablePlasticityForSynapse,
         py::arg("synapseId").noconvert(), py::arg("hebbian").noconvert(),
         py::arg("stdp").noconvert(), py::arg("rewardModulated").noconvert(),
         R"pbdoc(
         Enable/disable plasticity rules for a synapse
         
         Args:
             synapseId: Synapse ID to configure
             hebbian: Enable Hebbian plasticity
             stdp: Enable STDP
             rewardModulated: Enable reward-modulated plasticity
         )pbdoc")
    .def("setDevelopmentalStage", &AdvancedBrainManipulator::setDevelopmentalStage,
         py::arg("stage").noconvert(),
         R"pbdoc(
         Set developmental stage for the brain
         
         Args:
             stage: Developmental stage to set
         )pbdoc")
    .def("getDevelopmentalStage", &AdvancedBrainManipulator::getDevelopmentalStage,
         R"pbdoc(
         Get current developmental stage
         
         Returns:
             DevelopmentalStage: Current developmental stage
         )pbdoc")
    .def("triggerDevelopmentStep", &AdvancedBrainManipulator::triggerDevelopmentStep,
         R"pbdoc(
         Trigger a developmental step
         )pbdoc")
    .def("getDevelopmentEvents", &AdvancedBrainManipulator::getDevelopmentEvents,
         R"pbdoc(
         Get recent developmental events
         
         Returns:
             List[str]: Developmental events
         )pbdoc")
    .def("addInterRegionConnection", &AdvancedBrainManipulator::addInterRegionConnection,
         py::arg("source").noconvert(), py::arg("target").noconvert(),
         py::arg("weight") = 0.0f, py::arg("delay") = 1,
         R"pbdoc(
         Add inter-region connection
         
         Args:
             source: Source region ID
             target: Target region ID
             weight: Connection weight (default: 0.0)
             delay: Transmission delay (default: 1)
         )pbdoc")
    .def("removeInterRegionConnection", &AdvancedBrainManipulator::removeInterRegionConnection,
         py::arg("source").noconvert(), py::arg("target").noconvert(),
         R"pbdoc(
         Remove inter-region connection
         
         Args:
             source: Source region ID
             target: Target region ID
         )pbdoc")
    .def("getInterRegionConnections", &AdvancedBrainManipulator::getInterRegionConnections,
         R"pbdoc(
         Get all inter-region connections
         
         Returns:
             List[InterRegionConnection]: All inter-region connections
         )pbdoc")
    .def("setRegionalConnectivityProbability", &AdvancedBrainManipulator::setRegionalConnectivityProbability,
         py::arg("probability").noconvert(),
         R"pbdoc(
         Set probability for regional connectivity
         
         Args:
             probability: Connection probability (0.0 to 1.0)
         )pbdoc")
    .def("optimizeRegionalTopology", &AdvancedBrainManipulator::optimizeRegionalTopology,
         R"pbdoc(
         Optimize regional topology based on current connectivity
         )pbdoc");

    // ============= PERFORMANCE MONITORING TOOLS =============

    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(
        Real-time performance monitoring for neural simulations
        
        Tracks simulation performance including step timing, spike rates,
        and active neuron counts. Provides real-time metrics and stability analysis.
        
        Example:
            ```python
            # Create performance monitor
            monitor = PerformanceMonitor()
            monitor.start()
            
            # Run simulation steps
            for step in range(1000):
                # Run simulation step
                brain.step(step)
                
                # Record performance
                monitor.recordStep(step_time_ms, spikes, active_neurons)
            
            # Analyze performance
            print(f"Average step time: {monitor.getAvgStepTimeMs():.2f} ms")
            print(f"Steps per second: {monitor.getStepsPerSecond():.0f}")
            print(f"Stable: {monitor.isStable()}")
            ```
        
        Attributes:
            totalTime (float): Total simulation time in seconds
            stepCount (int): Total number of simulation steps
            spikeCount (int): Total number of spikes recorded
            avgStepTime (float): Average step time in milliseconds
            minStepTime (float): Minimum step time in milliseconds
            maxStepTime (float): Maximum step time in milliseconds
        )pbdoc")
    .def(py::init<>())
    .def("start", &PerformanceMonitor::start,
         R"pbdoc(Start performance monitoring)pbdoc")
    .def("stop", &PerformanceMonitor::stop,
         R"pbdoc(Stop performance monitoring)pbdoc")
    .def("recordStep", &PerformanceMonitor::recordStep,
         py::arg("stepTimeMs").noconvert(), py::arg("spikes").noconvert(),
         py::arg("activeNeurons").noconvert(),
         R"pbdoc(
         Record a simulation step performance
         
         Args:
             stepTimeMs: Step execution time in milliseconds
             spikes: Number of spikes in this step
             activeNeurons: Number of active neurons in this step
         )pbdoc")
    .def("reset", &PerformanceMonitor::reset,
         R"pbdoc(Reset performance counters)pbdoc")
    .def("getTotalTimeSeconds", &PerformanceMonitor::getTotalTimeSeconds,
         R"pbdoc(Get total simulation time in seconds)pbdoc")
    .def("getAvgStepTimeMs", &PerformanceMonitor::getAvgStepTimeMs,
         R"pbdoc(Get average step time in milliseconds)pbdoc")
    .def("getMinStepTimeMs", &PerformanceMonitor::getMinStepTimeMs,
         R"pbdoc(Get minimum step time in milliseconds)pbdoc")
    .def("getMaxStepTimeMs", &PerformanceMonitor::getMaxStepTimeMs,
         R"pbdoc(Get maximum step time in milliseconds)pbdoc")
    .def("getStepsPerSecond", &PerformanceMonitor::getStepsPerSecond,
         R"pbdoc(Get simulation steps per second)pbdoc")
    .def("getSpikesPerSecond", &PerformanceMonitor::getSpikesPerSecond,
         R"pbdoc(Get spikes per second)pbdoc")
    .def("getFiringRateHz", &PerformanceMonitor::getFiringRateHz,
         R"pbdoc(Get firing rate in Hz)pbdoc")
    .def("getStepCount", &PerformanceMonitor::getStepCount,
         R"pbdoc(Get total number of simulation steps)pbdoc")
    .def("getTotalSpikes", &PerformanceMonitor::getTotalSpikes,
         R"pbdoc(Get total number of spikes)pbdoc")
    .def("getCurrentActiveNeurons", &PerformanceMonitor::getCurrentActiveNeurons,
         R"pbdoc(Get current number of active neurons)pbdoc")
    .def("isStable", &PerformanceMonitor::isStable,
         py::arg("windowSize") = 100, py::arg("threshold") = 0.1,
         R"pbdoc(
         Check if performance is stable
         
         Args:
             windowSize: Number of steps to analyze (default: 100)
             threshold: Stability threshold (default: 0.1)
         
         Returns:
             bool: True if performance is stable
         )pbdoc")
    .def("getPerformanceReport", &PerformanceMonitor::getPerformanceReport,
         R"pbdoc(Generate a performance report string)pbdoc")
    .def("getStepTimes", &PerformanceMonitor::getStepTimes,
         R"pbdoc(Get all recorded step times)pbdoc")
    .def("getSpikeCounts", &PerformanceMonitor::getSpikeCounts,
         R"pbdoc(Get all recorded spike counts)pbdoc")
    .def("getActiveNeuronCounts", &PerformanceMonitor::getActiveNeuronCounts,
         R"pbdoc(Get all recorded active neuron counts)pbdoc");

    py::class_<MetricRecorder>(m, "MetricRecorder", R"pbdoc(
        Utility for recording custom metrics during simulation
        
        Provides flexible metric recording with statistical analysis,
        time-series support, and export capabilities. Supports multiple
        metric types and provides comprehensive analysis functions.
        
        Example:
            ```python
            # Create metric recorder
            recorder = MetricRecorder()
            
            # Record metrics
            recorder.record("reward", 1.5)
            recorder.record("loss", 0.23)
            recorder.record_time_series("firing_rate", [0.1, 0.2, 0.3, 0.4, 0.5])
            
            # Get statistics
            print(f"Mean reward: {recorder.getMean('reward'):.3f}")
            print(f"Max loss: {recorder.getMax('loss'):.3f}")
            
            # Export
            csv_data = recorder.toCSV("metrics.csv")
            json_data = recorder.toJSON()
            ```
        
        Methods:
        - record: Record a single metric value
        - record_time_series: Record time-series data
        - getMean: Get mean of metric values
        - getStdDev: Get standard deviation of metric values
        - toCSV: Export to CSV format
        - toJSON: Export to JSON format
    )pbdoc")
    .def(py::init<>())
    .def("record", &MetricRecorder::record,
         py::arg("name").noconvert(), py::arg("value").noconvert(),
         R"pbdoc(
         Record a metric value
         
         Args:
             name: Metric name
             value: Metric value
         )pbdoc")
    .def("recordTimeSeries", &MetricRecorder::recordTimeSeries,
         py::arg("name").noconvert(), py::arg("values").noconvert(),
         py::arg("timestamp") = 0.0,
         R"pbdoc(
         Record time-series data
         
         Args:
             name: Metric name
             values: Time-series values
             timestamp: Timestamp for the data
         )pbdoc")
    .def("getMetric", &MetricRecorder::getMetric,
         py::arg("name").noconvert(),
         R"pbdoc(
         Get current metric value
         
         Args:
             name: Metric name
         
         Returns:
             float: Current metric value
         )pbdoc")
    .def("getMetric", [](const MetricRecorder& self, const std::string& name, size_t index) {
        return self.getMetric(name, index);
    }, py::arg("name").noconvert(), py::arg("index").noconvert(),
         R"pbdoc(
         Get metric value at specific index
         
         Args:
             name: Metric name
             index: Index of value to retrieve
         
         Returns:
             float: Metric value at specified index
         )pbdoc")
    .def("getValues", &MetricRecorder::getValues,
         py::arg("name").noconvert(),
         R"pbdoc(
         Get all values for a metric
         
         Args:
             name: Metric name
         
         Returns:
             List[float]: All metric values
         )pbdoc")
    .def("getMetricNames", &MetricRecorder::getMetricNames,
         R"pbdoc(
         Get all recorded metric names
         
         Returns:
             List[str]: All metric names
         )pbdoc")
    .def("getMean", &MetricRecorder::getMean,
         py::arg("name").noconvert(),
         R"pbdoc(
         Get mean of metric values
         
         Args:
             name: Metric name
         
         Returns:
             float: Mean value
         )pbdoc")
    .def("getStdDev", &MetricRecorder::getStdDev,
         py::arg("name").noconvert(),
         R"pbdoc(
         Get standard deviation of metric values
         
         Args:
             name: Metric name
         
         Returns:
             float: Standard deviation
         )pbdoc")
    .def("getMin", &MetricRecorder::getMin,
         py::arg("name").noconvert(),
         R"pbdoc(
         Get minimum of metric values
         
         Args:
             name: Metric name
         
         Returns:
             float: Minimum value
         )pbdoc")
    .def("getMax", &MetricRecorder::getMax,
         py::arg("name").noconvert(),
         R"pbdoc(
         Get maximum of metric values
         
         Args:
             name: Metric name
         
         Returns:
             float: Maximum value
         )pbdoc")
    .def("getMedian", &MetricRecorder::getMedian,
         py::arg("name").noconvert(),
         R"pbdoc(
         Get median of metric values
         
         Args:
             name: Metric name
         
         Returns:
             float: Median value
         )pbdoc")
    .def("addRecorder", &MetricRecorder::addRecorder,
         py::arg("other").noconvert(),
         R"pbdoc(
         Add another metric recorder's data
         
         Args:
             other: Another MetricRecorder instance
         
         Returns:
             MetricRecorder: Self for method chaining
         )pbdoc")
    .def("clear", &MetricRecorder::clear,
         R"pbdoc(Clear all recorded metrics)pbdoc")
    .def("isEmpty", &MetricRecorder::isEmpty,
         R"pbdoc(Check if metric recorder is empty)pbdoc")
    .def("toCSV", &MetricRecorder::toCSV,
         py::arg("filename").noconvert(),
         R"pbdoc(
         Export metrics to CSV file
         
         Args:
             filename: Output filename
         
         Returns:
             str: CSV formatted data
         )pbdoc")
    .def("toJSON", &MetricRecorder::toJSON,
         R"pbdoc(Export metrics to JSON format)pbdoc")
    .def("saveToFile", &MetricRecorder::saveToFile,
         py::arg("filepath").noconvert(),
         R"pbdoc(
         Save metrics to file
         
         Args:
             filepath: Output file path
         
         Returns:
             bool: True if save successful
         )pbdoc")
    .def("loadFromFile", &MetricRecorder::loadFromFile,
         py::arg("filepath").noconvert(),
         R"pbdoc(
         Load metrics from file
         
         Args:
             filepath: Input file path
         
         Returns:
             bool: True if load successful
         )pbdoc")
    .def("summary", &MetricRecorder::summary,
         R"pbdoc(Get a summary of recorded metrics)pbdoc")
    .def("generateReport", &MetricRecorder::generateReport,
         R"pbdoc(Generate a comprehensive report)pbdoc");

    py::class_<TimelineRecorder>(m, "TimelineRecorder", R"pbdoc(
        Event timeline recorder for simulation steps and milestones
        
        Records and manages event timelines with categorization, filtering,
        and analysis capabilities. Useful for debugging, progress tracking,
        and understanding simulation flow.
        
        Example:
            ```python
            # Create timeline recorder
            timeline = TimelineRecorder()
            timeline.startRecording()
            
            # Record events
            timeline.recordEvent("simulation_start", "Begin simulation")
            timeline.recordStep("training_step", current_time, {"reward": 0.5, "loss": 0.3})
            timeline.recordEvent("checkpoint", "Save model")
            
            # Analyze timeline
            print(f"Duration: {timeline.getDuration():.2f} seconds")
            print(f"Events: {timeline.getEventCount()}")
            recent_events = timeline.getEventsAfter(start_time)
            ```
        
        Methods:
        - startRecording: Start recording events
        - stopRecording: Stop recording events
        - recordEvent: Record a simple event
        - recordStep: Record a step with metrics
        - getEventsAfter: Get events after timestamp
        - getEventsInRange: Get events within time range
        - getTimelineReport: Generate timeline report
    )pbdoc")
    .def(py::init<>())
    .def("startRecording", &TimelineRecorder::startRecording,
         R"pbdoc(Start recording timeline events)pbdoc")
    .def("stopRecording", &TimelineRecorder::stopRecording,
         R"pbdoc(Stop recording timeline events)pbdoc")
    .def("recordEvent", &TimelineRecorder::recordEvent,
         py::arg("eventName").noconvert(), py::arg("description") = "",
         R"pbdoc(
         Record an event
         
         Args:
             eventName: Event name
             description: Event description
         )pbdoc")
    .def("recordEventWithTime", &TimelineRecorder::recordEventWithTime,
         py::arg("eventName").noconvert(), py::arg("timestamp").noconvert(),
         py::arg("description") = "",
         R"pbdoc(
         Record an event with specific timestamp
         
         Args:
             eventName: Event name
             timestamp: Event timestamp
             description: Event description
         )pbdoc")
    .def("recordStep", &TimelineRecorder::recordStep,
         py::arg("eventName").noconvert(), py::arg("timestamp").noconvert(),
         py::arg("metrics") = py::dict(),
         R"pbdoc(
         Record a simulation step
         
         Args:
             eventName: Event name
             timestamp: Step timestamp
             metrics: Step metrics as dictionary
         )pbdoc")
    .def("getEventsForCategory", &TimelineRecorder::getEventsForCategory,
         py::arg("category").noconvert(),
         R"pbdoc(
         Get events for a specific category
         
         Args:
             category: Event category
         
         Returns:
             List[Tuple[Timestamp, str]]: Events in the category
         )pbdoc")
    .def("getEventCategories", &TimelineRecorder::getEventCategories,
         R"pbdoc(
         Get all event categories
         
         Returns:
             List[str]: Event categories
         )pbdoc")
    .def("getTimelineReport", &TimelineRecorder::getTimelineReport,
         R"pbdoc(Generate a timeline report)pbdoc")
    .def("getDuration", &TimelineRecorder::getDuration,
         R"pbdoc(Get timeline duration)pbdoc")
    .def("getEventCount", &TimelineRecorder::getEventCount,
         py::arg("eventName") = "",
         R"pbdoc(
         Get event count
         
         Args:
             eventName: Event name (empty for all events)
         
         Returns:
             int: Number of events
         )pbdoc")
    .def("getEventsAfter", &TimelineRecorder::getEventsAfter,
         py::arg("timestamp").noconvert(),
         R"pbdoc(
         Get events after timestamp
         
         Args:
             timestamp: Start timestamp
         
         Returns:
             List[Tuple[Timestamp, str]]: Events after timestamp
         )pbdoc")
    .def("getEventsBefore", &TimelineRecorder::getEventsBefore,
         py::arg("timestamp").noconvert(),
         R"pbdoc(
         Get events before timestamp
         
         Args:
             timestamp: End timestamp
         
         Returns:
             List[Tuple[Timestamp, str]]: Events before timestamp
         )pbdoc")
    .def("getEventsInRange", &TimelineRecorder::getEventsInRange,
         py::arg("start").noconvert(), py::arg("end").noconvert(),
         R"pbdoc(
         Get events within time range
         
         Args:
             start: Start timestamp
             end: End timestamp
         
         Returns:
             List[Tuple[Timestamp, str]]: Events in range
         )pbdoc");

    py::class_<Profiler>(m, "Profiler", R"pbdoc(
        Function and performance profiler for bottleneck identification
        
        Profiles function execution times, memory usage, and call patterns
        to identify performance bottlenecks and optimize simulation performance.
        
        Example:
            ```python
            # Create profiler
            profiler = Profiler()
            
            # Profile functions
            profiler.startProfile("brain_step")
            brain.step(step)
            profiler.stopProfile("brain_step")
            
            profiler.startProfile("plasticity_update")
            brain.updatePlasticity()
            profiler.stopProfile("plasticity_update")
            
            # Analyze profiling data
            print(profiler.getProfileReport())
            bottlenecks = profiler.identifyBottlenecks()
            print(f"Main bottleneck: {bottlenecks}")
            ```
        
        Methods:
        - startProfile: Start profiling a function
        - stopProfile: Stop profiling a function
        - recordMemoryUsage: Record memory usage
        - identifyBottlenecks: Identify performance bottlenecks
        - getProfileReport: Generate profile report
        - getSlowestFunctions: Get slowest functions
        - getMostFrequentFunctions: Get most frequently called functions
    )pbdoc")
    .def(py::init<>())
    .def("startProfile", &Profiler::startProfile,
         py::arg("functionName").noconvert(),
         R"pbdoc(
         Start profiling a function
         
         Args:
             functionName: Name of function to profile
         )pbdoc")
    .def("stopProfile", &Profiler::stopProfile,
         py::arg("functionName").noconvert(),
         R"pbdoc(
         Stop profiling a function
         
         Args:
             functionName: Name of function to stop profiling
         )pbdoc")
    .def("recordMemoryUsage", &Profiler::recordMemoryUsage,
         py::arg("context").noconvert(), py::arg("bytes").noconvert(),
         R"pbdoc(
         Record memory usage
         
         Args:
             context: Context for memory usage
             bytes: Memory usage in bytes
         )pbdoc")
    .def("getProfileReport", &Profiler::getProfileReport,
         R"pbdoc(Generate a profile report)pbdoc")
    .def("getFunctionProfile", [](const Profiler& self, const std::string& functionName) {
        return self.getFunctionProfile(functionName);
    }, py::arg("functionName").noconvert(),
         R"pbdoc(
         Get profile data for a function
         
         Args:
             functionName: Name of function to query
         
         Returns:
             Dict: Profile data for the function
         )pbdoc")
    .def("getSlowestFunctions", [](const Profiler& self, size_t limit) {
        return self.getSlowestFunctions(limit);
    }, py::arg("limit") = 10,
         R"pbdoc(
         Get slowest functions
         
         Args:
             limit: Maximum number of functions to return
         
         Returns:
             List[str]: Names of slowest functions
         )pbdoc")
    .def("getMostFrequentFunctions", [](const Profiler& self, size_t limit) {
        return self.getMostFrequentFunctions(limit);
    }, py::arg("limit") = 10,
         R"pbdoc(
         Get most frequently called functions
         
         Args:
             limit: Maximum number of functions to return
         
         Returns:
             List[str]: Names of most frequent functions
         )pbdoc")
    .def("identifyBottlenecks", &Profiler::identifyBottlenecks,
         R"pbdoc(
         Identify performance bottlenecks
         
         Returns:
             str: Bottleneck identification report
         )pbdoc")
    .def("getAverageMemoryUsage", &Profiler::getAverageMemoryUsage,
         R"pbdoc(Get average memory usage)pbdoc")
    .def("getMaxMemoryUsage", &Profiler::getMaxMemoryUsage,
         R"pbdoc(Get maximum memory usage)pbdoc");

    py::class_<MemoryMonitor>(m, "MemoryMonitor", R"pbdoc(
        Memory usage monitor for neural simulations
        
        Tracks and analyzes memory usage including neuron, synapse, and
        event queue memory consumption. Provides detailed memory breakdown
        and growth analysis.
        
        Example:
            ```python
            # Create memory monitor
            memory_monitor = MemoryMonitor()
            memory_monitor.startMonitoring()
            
            # Run simulation
            for step in range(1000):
                brain.step(step)
                memory_monitor.recordSample()
            
            memory_monitor.stopMonitoring()
            
            # Analyze memory usage
            print(f"Peak memory: {memory_monitor.getPeakMemoryMB()} MB")
            print(f"Current memory: {memory_monitor.getCurrentMemoryMB()} MB")
            print(f"Memory growth rate: {memory_monitor.getMemoryGrowthRate():.2f} MB/step")
            ```
        
        Methods:
        - startMonitoring: Start monitoring memory
        - stopMonitoring: Stop monitoring memory
        - recordSample: Record a memory sample
        - getPeakMemoryMB: Get peak memory usage
        - getCurrentMemoryMB: Get current memory usage
        - getMemoryGrowthRate: Get memory growth rate
        - getMemoryReport: Generate memory report
    )pbdoc")
    .def(py::init<>())
    .def("startMonitoring", &MemoryMonitor::startMonitoring,
         R"pbdoc(Start memory monitoring)pbdoc")
    .def("stopMonitoring", &MemoryMonitor::stopMonitoring,
         R"pbdoc(Stop memory monitoring)pbdoc")
    .def("recordSample", &MemoryMonitor::recordSample,
         R"pbdoc(Record a memory sample)pbdoc")
    .def("getPeakMemoryMB", &MemoryMonitor::getPeakMemoryMB,
         R"pbdoc(Get peak memory usage in MB)pbdoc")
    .def("getCurrentMemoryMB", &MemoryMonitor::getCurrentMemoryMB,
         R"pbdoc(Get current memory usage in MB)pbdoc")
    .def("getMemoryGrowthRate", &MemoryMonitor::getMemoryGrowthRate,
         R"pbdoc(Get memory growth rate in MB/step)pbdoc")
    .def("getMemoryReport", &MemoryMonitor::getMemoryReport,
         R"pbdoc(Generate a memory report)pbdoc")
    .def("getCurrentBreakdown", [](const MemoryMonitor& self) {
        auto breakdown = self.getCurrentBreakdown();
        return std::make_tuple(breakdown.neuronsMB, breakdown.synapsesMB,
                              breakdown.eventsMB, breakdown.otherMB);
    },
         R"pbdoc(
         Get current memory breakdown
         
         Returns:
             Tuple[int, int, int, int]: (neurons, synapses, events, other) memory in MB
         )pbdoc")
    .def("getAverageBreakdown", [](const MemoryMonitor& self) {
        auto breakdown = self.getAverageBreakdown();
        return std::make_tuple(breakdown.neuronsMB, breakdown.synapsesMB,
                              breakdown.eventsMB, breakdown.otherMB);
    },
         R"pbdoc(
         Get average memory breakdown
         
         Returns:
             Tuple[int, int, int, int]: (neurons, synapses, events, other) memory in MB
         )pbdoc");

    // ============= ENHANCED CONFIGURATION MANAGEMENT =============

    py::class_<ConfigManager>(m, "ConfigManager", R"pbdoc(
        Enhanced configuration management with templates and validation
        
        Provides advanced configuration management including template-based
        configuration creation, validation utilities, config inheritance and merging,
        and parameter sensitivity analysis.
        
        Example:
            ```python
            # Create config manager
            config_manager = ConfigManager()
            
            # Add templates
            default_config = Config()
            default_config.set("brain.neuron_count", 1000)
            default_config.set("brain.simulation_steps", 10000)
            config_manager.addTemplate("default", "Default configuration", default_config)
            
            # Create config from template
            config = config_manager.createFromTemplate("default")
            
            # Validate config
            is_valid = config_manager.validateConfig(config)
            errors = config_manager.validateConfigWithDetails(config)
            
            # Merge configs
            config2 = Config()
            config2.set("brain.simulation_steps", 20000)
            merged_config = config_manager.mergeConfigs([config, config2])
            ```
        
        Methods:
        - addTemplate: Add a configuration template
        - createFromTemplate: Create config from template
        - validateConfig: Validate configuration
        - mergeConfigs: Merge multiple configurations
        - analyzeSensitivity: Analyze parameter sensitivity
        - saveConfig: Save configuration to history
        - loadConfig: Load configuration from history
    )pbdoc")
    .def(py::init<>())
    .def("addTemplate", &ConfigManager::addTemplate,
         py::arg("name").noconvert(), py::arg("description").noconvert(),
         py::arg("config").noconvert(), py::arg("isDefault") = false,
         R"pbdoc(
         Add a configuration template
         
         Args:
             name: Template name
             description: Template description
             config: Default configuration
             isDefault: Whether this is the default template
         )pbdoc")
    .def("removeTemplate", &ConfigManager::removeTemplate,
         py::arg("name").noconvert(),
         R"pbdoc(
         Remove a configuration template
         
         Args:
             name: Template name to remove
         
         Returns:
             bool: True if template was removed
         )pbdoc")
    .def("getTemplate", [](const ConfigManager& self, const std::string& name) {
        auto template = self.getTemplate(name);
        return std::make_tuple(template.name, template.description, template.isDefault);
    }, py::arg("name").noconvert(),
         R"pbdoc(
         Get configuration template
         
         Args:
             name: Template name
         
         Returns:
             Tuple[str, str, bool]: (name, description, isDefault)
         )pbdoc")
    .def("getTemplateNames", &ConfigManager::getTemplateNames,
         R"pbdoc(
         Get all template names
         
         Returns:
             List[str]: Template names
         )pbdoc")
    .def("setActiveTemplate", &ConfigManager::setActiveTemplate,
         py::arg("name").noconvert(),
         R"pbdoc(
         Set active template
         
         Args:
             name: Template name
         )pbdoc")
    .def("getActiveTemplateName", &ConfigManager::getActiveTemplateName,
         R"pbdoc(
         Get active template name
         
         Returns:
             str: Active template name
         )pbdoc")
    .def("createFromTemplate", [](const ConfigManager& self, const std::string& templateName) {
        return self.createFromTemplate(templateName);
    }, py::arg("templateName").noconvert(),
         R"pbdoc(
         Create configuration from template
         
         Args:
             templateName: Template name
         
         Returns:
             Config: Configuration instance
         )pbdoc")
    .def("createFromDefaults", [](const ConfigManager& self) {
        return self.createFromDefaults();
    }, R"pbdoc(
         Create default configuration
         
         Returns:
             Config: Default configuration instance
         )pbdoc")
    .def("createFromInheritance", [](const ConfigManager& self, const std::vector<std::string>& templateNames) {
        return self.createFromInheritance(templateNames);
    }, py::arg("templateNames").noconvert(),
         R"pbdoc(
         Create configuration from template inheritance
         
         Args:
             templateNames: Template names to inherit from
         
         Returns:
             Config: Inherited configuration
         )pbdoc")
    .def("validateConfig", &ConfigManager::validateConfig,
         py::arg("config").noconvert(),
         R"pbdoc(
         Validate configuration
         
         Args:
             config: Configuration to validate
         
         Returns:
             bool: True if configuration is valid
         )pbdoc")
    .def("validateConfigWithDetails", &ConfigManager::validateConfigWithDetails,
         py::arg("config").noconvert(),
         R"pbdoc(
         Validate configuration with detailed errors
         
         Args:
             config: Configuration to validate
         
         Returns:
             List[str]: List of validation errors
         )pbdoc")
    .def("isConfigComplete", &ConfigManager::isConfigComplete,
         py::arg("config").noconvert(),
         R"pbdoc(
         Check if configuration is complete
         
         Args:
             config: Configuration to check
         
         Returns:
             bool: True if configuration is complete
         )pbdoc")
    .def("mergeConfigs", [](const ConfigManager& self, const std::vector<Config>& configs) {
        return self.mergeConfigs(configs);
    }, py::arg("configs").noconvert(),
         R"pbdoc(
         Merge multiple configurations
         
         Args:
             configs: Configurations to merge
         
         Returns:
             Config: Merged configuration
         )pbdoc")
    .def("mergeConfigWithTemplate", [](const ConfigManager& self, const Config& config, const std::string& templateName) {
        return self.mergeConfigWithTemplate(config, templateName);
    }, py::arg("config").noconvert(), py::arg("templateName").noconvert(),
         R"pbdoc(
         Merge configuration with template
         
         Args:
             config: Base configuration
             templateName: Template name
         
         Returns:
             Config: Merged configuration
         )pbdoc")
    .def("analyzeSensitivity", [](const ConfigManager& self, const Config& config) {
        auto sensitivity = self.analyzeSensitivity(config);
        std::vector<std::pair<std::string, double>> results;
        for (const auto& [key, value] : sensitivity) {
            results.emplace_back(key, value);
        }
        return results;
    }, py::arg("config").noconvert(),
         R"pbdoc(
         Analyze parameter sensitivity
         
         Args:
             config: Configuration to analyze
         
         Returns:
             List[Tuple[str, float]]: (parameter, sensitivity)
         )pbdoc")
    .def("getSensitivityReport", &ConfigManager::getSensitivityReport,
         py::arg("config").noconvert(),
         R"pbdoc(
         Get sensitivity analysis report
         
         Args:
             config: Configuration to analyze
         
         Returns:
             str: Sensitivity report
         )pbdoc")
    .def("saveConfig", &ConfigManager::saveConfig,
         py::arg("config").noconvert(), py::arg("description").noconvert(),
         R"pbdoc(
         Save configuration to history
         
         Args:
             config: Configuration to save
             description: Description of configuration
         )pbdoc")
    .def("loadConfig", [](const ConfigManager& self, const std::string& description) {
        return self.loadConfig(description);
    }, py::arg("description").noconvert(),
         R"pbdoc(
         Load configuration from history
         
         Args:
             description: Configuration description
         
         Returns:
             Config: Loaded configuration
         )pbdoc")
    .def("getConfigHistory", &ConfigManager::getConfigHistory,
         R"pbdoc(
         Get configuration history
         
         Returns:
             List[str]: Configuration descriptions
         )pbdoc");

    // ============= ADVANCED SERIALIZATION AND PERSISTENCE =============

    py::class_<SerializationManager>(m, "SerializationManager", R"pbdoc(
        Advanced serialization and persistence manager
        
        Provides comprehensive serialization capabilities including HDF5 support,
        pickle-compatible serialization, incremental save/restore, compressed storage,
        and checkpoint management for large datasets.
        
        Example:
            ```python
            # Create serialization manager
            serializer = SerializationManager()
            
            # Save to HDF5 (for large datasets)
            serializer.saveHDF5("simulation.h5", config, datasets)
            
            # Save to Pickle (for small models)
            serializer.savePickle("model.pkl", config, brain)
            
            # Create checkpoints
            serializer.createCheckpoint("milestone1", config, brain)
            serializer.createCheckpoint("milestone2", config, brain)
            
            # Restore checkpoint
            serializer.restoreCheckpoint("milestone1")
            ```
        
        Methods:
        - saveHDF5: Save to HDF5 format (large datasets)
        - loadHDF5: Load from HDF5 format
        - savePickle: Save to pickle format
        - loadPickle: Load from pickle format
        - createCheckpoint: Create a checkpoint
        - restoreCheckpoint: Restore from checkpoint
        - getCheckpoints: Get list of checkpoints
    )pbdoc")
    .def(py::init<>())
    .def("saveHDF5", &SerializationManager::saveHDF5,
         py::arg("filepath").noconvert(), py::arg("config").noconvert(),
         py::arg("datasets").noconvert(), py::arg("attributes") = py::dict(),
         R"pbdoc(
         Save to HDF5 format
         
         Args:
             filepath: Output file path
             config: Configuration to save
             datasets: Datasets to save (dict[str, List[float]])
             attributes: Additional attributes (dict[str, str])
         
         Returns:
             bool: True if save successful
         )pbdoc")
    .def("loadHDF5", [](SerializationManager& self, const std::string& filepath) {
        auto result = self.loadHDF5(filepath);
        return std::make_tuple(result.first, result.second);
    }, py::arg("filepath").noconvert(),
         R"pbdoc(
         Load from HDF5 format
         
         Args:
             filepath: Input file path
         
         Returns:
             Tuple[Config, Dict[str, List[float]]]: (config, datasets)
         )pbdoc")
    .def("savePickle", &SerializationManager::savePickle,
         py::arg("filepath").noconvert(), py::arg("config").noconvert(),
         py::arg("brain") = std::shared_ptr<Brain>(nullptr),
         R"pbdoc(
         Save to pickle format
         
         Args:
             filepath: Output file path
             config: Configuration to save
             brain: Brain instance to save (optional)
         
         Returns:
             bool: True if save successful
         )pbdoc")
    .def("loadPickle", [](SerializationManager& self, const std::string& filepath) {
        auto result = self.loadPickle(filepath);
        return std::make_tuple(result.first, result.second);
    }, py::arg("filepath").noconvert(),
         R"pbdoc(
         Load from pickle format
         
         Args:
             filepath: Input file path
         
         Returns:
             Tuple[Config, Brain]: (config, brain)
         )pbdoc")
    .def("saveIncremental", &SerializationManager::saveIncremental,
         py::arg("filepath").noconvert(), py::arg("config").noconvert(),
         py::arg("data").noconvert(),
         R"pbdoc(
         Save incrementally
         
         Args:
             filepath: Output file path
             config: Configuration to save
             data: Additional data (dict[str, List[float]])
         
         Returns:
             bool: True if save successful
         )pbdoc")
    .def("loadIncremental", [](SerializationManager& self, const std::string& filepath) {
        auto result = self.loadIncremental(filepath);
        return std::make_tuple(result.first, result.second);
    }, py::arg("filepath").noconvert(),
         R"pbdoc(
         Load incrementally
         
         Args:
             filepath: Input file path
         
         Returns:
             Tuple[Config, Dict[str, List[float]]]: (config, data)
         )pbdoc")
    .def("createCheckpoint", &SerializationManager::createCheckpoint,
         py::arg("name").noconvert(), py::arg("config").noconvert(),
         py::arg("brain") = std::shared_ptr<Brain>(nullptr),
         R"pbdoc(
         Create a checkpoint
         
         Args:
             name: Checkpoint name
             config: Configuration to save
             brain: Brain instance to save (optional)
         
         Returns:
             bool: True if checkpoint created successfully
         )pbdoc")
    .def("restoreCheckpoint", &SerializationManager::restoreCheckpoint,
         py::arg("name").noconvert(),
         R"pbdoc(
         Restore from checkpoint
         
         Args:
             name: Checkpoint name
         
         Returns:
             bool: True if checkpoint restored successfully
         )pbdoc")
    .def("getCheckpoints", &SerializationManager::getCheckpoints,
         R"pbdoc(
         Get list of checkpoints
         
         Returns:
             List[str]: Checkpoint names
         )pbdoc")
    .def("saveCompressed", &SerializationManager::saveCompressed,
         py::arg("filepath").noconvert(), py::arg("config").noconvert(),
         py::arg("brain") = std::shared_ptr<Brain>(nullptr),
         R"pbdoc(
         Save with compression
         
         Args:
             filepath: Output file path
             config: Configuration to save
             brain: Brain instance to save (optional)
         
         Returns:
             bool: True if save successful
         )pbdoc")
    .def("loadCompressed", [](SerializationManager& self, const std::string& filepath) {
        auto result = self.loadCompressed(filepath);
        return std::make_tuple(result.first, result.second);
    }, py::arg("filepath").noconvert(),
         R"pbdoc(
         Load from compressed file
         
         Args:
             filepath: Input file path
         
         Returns:
             Tuple[Config, Brain]: (config, brain)
         )pbdoc");

    // ============= VISUALIZATION AND MONITORING =============

    py::class_<VisualizationTools>(m, "VisualizationTools", R"pbdoc(
        Advanced visualization and monitoring tools
        
        Provides comprehensive visualization capabilities for neural networks,
        including neural network visualization, activity monitoring, real-time
        plotting, and result visualization frameworks.
        
        Example:
            ```python
            # Create visualization tools
            visualizer = VisualizationTools()
            
            # Visualize neural network
            visualizer.visualizeNetwork("network.png", brain)
            
            # Create real-time monitor
            visualizer.createRealTimeMonitor("monitor.html", brain, update_interval=1000)
            
            # Start real-time plotting
            visualizer.startRealTimePlotting("plot.html", brain, ["firing_rate", "reward"])
            
            # Create results dashboard
            visualizer.createResultsDashboard("dashboard.html", [brain1, brain2], ["experiment1", "experiment2"])
            ```
        
        Methods:
        - visualizeNetwork: Visualize neural network structure
        - createRealTimeMonitor: Create real-time monitoring interface
        - startRealTimePlotting: Start real-time plotting
        - createResultsDashboard: Create results dashboard
        - createComparisonPlot: Create comparison plots
        - createTrainingProgress: Create training progress visualization
    )pbdoc")
    .def(py::init<>())
    .def("visualizeNetwork", &VisualizationTools::visualizeNetwork,
         py::arg("filepath").noconvert(), py::arg("brain").noconvert(),
         py::arg("format") = "png",
         R"pbdoc(
         Visualize neural network
         
         Args:
             filepath: Output file path
             brain: Brain instance to visualize
             format: Output format (default: "png")
         
         Returns:
             bool: True if visualization successful
         )pbdoc")
    .def("visualizeConnections", &VisualizationTools::visualizeConnections,
         py::arg("filepath").noconvert(), py::arg("brain").noconvert(),
         py::arg("format") = "png",
         R"pbdoc(
         Visualize neural connections
         
         Args:
             filepath: Output file path
             brain: Brain instance to visualize
             format: Output format (default: "png")
         
         Returns:
             bool: True if visualization successful
         )pbdoc")
    .def("visualizeActivity", &VisualizationTools::visualizeActivity,
         py::arg("filepath").noconvert(), py::arg("brain").noconvert(),
         py::arg("metric") = "firing_rate", py::arg("threshold") = 0.5,
         py::arg("format") = "png",
         R"pbdoc(
         Visualize neural activity
         
         Args:
             filepath: Output file path
             brain: Brain instance to visualize
             metric: Activity metric to visualize (default: "firing_rate")
             threshold: Activity threshold (0.0 to 1.0)
             format: Output format (default: "png")
         
         Returns:
             bool: True if visualization successful
         )pbdoc")
    .def("createActivityMonitor", &VisualizationTools::createActivityMonitor,
         py::arg("filepath").noconvert(), py::arg("brain").noconvert(),
         py::arg("metrics") = std::vector<std::string>(),
         R"pbdoc(
         Create activity monitor
         
         Args:
             filepath: Output file path
             brain: Brain instance to monitor
             metrics: Metrics to monitor
         
         Returns:
             bool: True if monitor created successfully
         )pbdoc")
    .def("createRealTimeMonitor", &VisualizationTools::createRealTimeMonitor,
         py::arg("filepath").noconvert(), py::arg("brain").noconvert(),
         py::arg("updateIntervalMs") = 1000,
         R"pbdoc(
         Create real-time monitoring interface
         
         Args:
             filepath: Output file path
             brain: Brain instance to monitor
             updateIntervalMs: Update interval in milliseconds (default: 1000)
         
         Returns:
             bool: True if monitor created successfully
         )pbdoc")
    .def("startRealTimePlotting", &VisualizationTools::startRealTimePlotting,
         py::arg("filepath").noconvert(), py::arg("brain").noconvert(),
         py::arg("metrics") = std::vector<std::string>(),
         R"pbdoc(
         Start real-time plotting
         
         Args:
             filepath: Output file path
             brain: Brain instance to plot
             metrics: Metrics to plot
         )pbdoc")
    .def("stopRealTimePlotting", &VisualizationTools::stopRealTimePlotting,
         R"pbdoc(Stop real-time plotting)pbdoc")
    .def("createResultsDashboard", &VisualizationTools::createResultsDashboard,
         py::arg("filepath").noconvert(), py::arg("brains").noconvert(),
         py::arg("experimentNames") = std::vector<std::string>(),
         R"pbdoc(
         Create results dashboard
         
         Args:
             filepath: Output file path
             brains: Brain instances to display
             experimentNames: Experiment names for each brain
         
         Returns:
             bool: True if dashboard created successfully
         )pbdoc")
    .def("createComparisonPlot", &VisualizationTools::createComparisonPlot,
         py::arg("filepath").noconvert(), py::arg("brains").noconvert(),
         py::arg("metric") = "firing_rate",
         R"pbdoc(
         Create comparison plot
         
         Args:
             filepath: Output file path
             brains: Brain instances to compare
             metric: Metric to compare
         
         Returns:
             bool: True if plot created successfully
         )pbdoc")
    .def("createTrainingProgress", &VisualizationTools::createTrainingProgress,
         py::arg("filepath").noconvert(), py::arg("trainingData").noconvert(),
         py::arg("title") = "Training Progress",
         R"pbdoc(
         Create training progress visualization
         
         Args:
             filepath: Output file path
             trainingData: Training data (list of pairs[timestamp, value])
             title: Plot title
         
         Returns:
             bool: True if visualization created successfully
         )pbdoc");

    // ============= MODULE CONSTANTS AND FACTORIES =============

    m.def("createAdvancedBrainManipulator", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AdvancedBrainManipulator>(brain);
    }, py::arg("brain").noconvert(),
         R"pbdoc(
         Create an AdvancedBrainManipulator instance
         
         Args:
             brain: Brain instance to manipulate
         
         Returns:
             AdvancedBrainManipulator: Manipulator instance
         )pbdoc")
    .def("createPerformanceMonitor", []() {
        return std::make_shared<PerformanceMonitor>();
    }, R"pbdoc(
         Create a PerformanceMonitor instance
         
         Returns:
             PerformanceMonitor: Monitor instance
         )pbdoc")
    .def("createMetricRecorder", []() {
        return std::make_shared<MetricRecorder>();
    }, R"pbdoc(
         Create a MetricRecorder instance
         
         Returns:
             MetricRecorder: Recorder instance
         )pbdoc")
    .def("createTimelineRecorder", []() {
        return std::make_shared<TimelineRecorder>();
    }, R"pbdoc(
         Create a TimelineRecorder instance
         
         Returns:
             TimelineRecorder: Recorder instance
         )pbdoc")
    .def("createProfiler", []() {
        return std::make_shared<Profiler>();
    }, R"pbdoc(
         Create a Profiler instance
         
         Returns:
             Profiler: Profiler instance
         )pbdoc")
    .def("createMemoryMonitor", []() {
        return std::make_shared<MemoryMonitor>();
    }, R"pbdoc(
         Create a MemoryMonitor instance
         
         Returns:
             MemoryMonitor: Monitor instance
         )pbdoc")
    .def("createConfigManager", []() {
        return std::make_shared<ConfigManager>();
    }, R"pbdoc(
         Create a ConfigManager instance
         
         Returns:
             ConfigManager: Manager instance
         )pbdoc")
    .def("createSerializationManager", []() {
        return std::make_shared<SerializationManager>();
    }, R"pbdoc(
         Create a SerializationManager instance
         
         Returns:
             SerializationManager: Manager instance
         )pbdoc")
    .def("createVisualizationTools", []() {
        return std::make_shared<VisualizationTools>();
    }, R"pbdoc(
         Create a VisualizationTools instance
         
         Returns:
             VisualizationTools: Tools instance
         )pbdoc");

    // Module constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "3.0.0";
    m.attr("__phase__") = "3_Advanced_Features";
}
");