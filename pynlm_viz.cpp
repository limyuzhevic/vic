// NLM Advanced Visualization Tools

/**
 * Advanced visualization system for NLM Phase 6 enhanced brain simulation.
 * Provides real-time visualization of neural activity, memory systems, neuromodulation,
 * and other brain components with interactive capabilities and comprehensive
 * analytical tools.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <cmath>
#include <complex>

// Forward declarations
namespace nlm {
    class VisualizationInterface;
    class NeuralActivityVisualizer;
    class MemorySystemVisualizer;
    class NeuromodulationVisualizer;
    class DevelopmentVisualizer;
    class PerformanceMonitor;
    class InteractiveVisualizer;
}

namespace py = pybind11;
namespace nlm {

// Visualization types
enum class VisualizationType {
    2D = 0,
    3D = 1,
    TIME_SERIES = 2,
    SPATIAL = 3,
    CONNECTIVITY = 4,
    SYNAPTIC = 5,
    POPULATION = 6,
    NETWORK = 7,
    MULTIPLEXED = 8
};

// Visualization update frequency
enum class UpdateFrequency {
    REAL_TIME = 0,
    HIGH = 1,
    MEDIUM = 2,
    LOW = 3,
    MINIMAL = 4
};

// Visualization output formats
enum class OutputFormat {
    TEXT = 0,
    SVG = 1,
    PNG = 2,
    PDF = 3,
    HTML = 4,
    JSON = 5,
    VIDEO = 6
};

// Visualization event types
enum class VisualizationEvent {
    UPDATE = 0,
    SELECT = 1,
    ZOOM = 2,
    PAN = 3,
    FILTER = 4,
    ANNOTATE = 5,
    EXPORT = 6,
    IMPORT = 7,
    RESET = 8
};

// Visualization color schemes
enum class ColorScheme {
    DEFAULT = 0,
    NEURAL = 1,
    MEMORY = 2,
    NEUROMODULATION = 3,
    COGNITION = 4,
    DEVELOPMENT = 5,
    SCIENTIFIC = 6,
    ARTISTIC = 7,
    MONOCHROMATIC = 8,
    HIGH_CONTRAST = 9
};

// Visualization data structures
struct NeuralActivityData {
    std::vector<float> firing_rates;
    std::vector<float> membrane_potentials;
    std::vector<float> synaptic_activities;
    std::vector<double> spike_times;
    std::vector<NeuronId> active_neurons;
    std::chrono::steady_clock::time_point timestamp;
    
    NeuralActivityData() : timestamp(std::chrono::steady_clock::now()) {}
};

struct MemoryStateData {
    std::vector<float> working_memory_traces;
    std::vector<float> episodic_activations;
    std::vector<float> associative_strengths;
    std::vector<double> memory_timestamps;
    std::vector<NeuronId> episodic_neurons;
    std::chrono::steady_clock::time_point timestamp;
    
    MemoryStateData() : timestamp(std::chrono::steady_clock::now()) {}
};

struct NeuromodulationData {
    float dopamine_level;
    float curiosity_level;
    float novelty_level;
    float prediction_error_level;
    std::vector<double> neuromodulation_timestamps;
    std::chrono::steady_clock::time_point timestamp;
    
    NeuromodulationData() 
        : dopamine_level(0.0), curiosity_level(0.0), 
          novelty_level(0.0), prediction_error_level(0.0)
        , timestamp(std::chrono::steady_clock::now()) {}
};

struct DevelopmentData {
    DevelopmentalStage developmental_stage;
    float plasticity_modifier;
    float structural_plasticity_rate;
    float synaptogenesis_rate;
    float pruning_rate;
    double developmental_time;
    std::chrono::steady_clock::time_point timestamp;
    
    DevelopmentData()
        : developmental_stage(DevelopmentalStage::Initial),
          plasticity_modifier(1.0), structural_plasticity_rate(0.0001),
          synaptogenesis_rate(0.0001), pruning_rate(0.00001),
          developmental_time(0.0), timestamp(std::chrono::steady_clock::now()) {}
};

struct PerformanceMetricsData {
    double simulation_speed;
    float cpu_usage;
    float memory_usage_mb;
    size_t active_threads;
    size_t pending_events;
    double frame_time_ms;
    std::chrono::steady_clock::time_point timestamp;
    
    PerformanceMetricsData()
        : simulation_speed(0.0), cpu_usage(0.0), memory_usage_mb(0.0),
          active_threads(0), pending_events(0), frame_time_ms(0.0)
        , timestamp(std::chrono::steady_clock::now()) {}
};

struct VisualizationEventData {
    VisualizationEvent event_type;
    std::string event_id;
    std::string target;
    py::dict parameters;
    std::chrono::steady_clock::time_point timestamp;
    
    VisualizationEventData() : timestamp(std::chrono::steady_clock::now()) {}
};

// Base visualization interface
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    // Initialize visualization with configuration
    virtual bool initialize(const std::string& config_file = "") = 0;
    
    // Update visualization with new data
    virtual void update(const NeuralActivityData& neural_data) = 0;
    virtual void update(const MemoryStateData& memory_data) = 0;
    virtual void update(const NeuromodulationData& neuromod_data) = 0;
    virtual void update(const DevelopmentData& dev_data) = 0;
    virtual void update(const PerformanceMetricsData& perf_data) = 0;
    
    // Event handling
    virtual void handleEvent(const VisualizationEventData& event) = 0;
    
    // Configuration
    virtual void setVisualizationType(VisualizationType type) = 0;
    virtual void setColorScheme(ColorScheme scheme) = 0;
    virtual void setUpdateFrequency(UpdateFrequency freq) = 0;
    virtual void setOutputFormat(OutputFormat format) = 0;
    
    // Data filtering and manipulation
    virtual void setDataFilter(const std::string& filter_name, const py::dict& parameters) = 0;
    virtual py::dict getDataFilter() const = 0;
    
    // Interaction
    virtual void setInteractive(bool interactive) = 0;
    virtual bool isInteractive() const = 0;
    virtual void setCanvasSize(int width, int height) = 0;
    virtual std::pair<int, int> getCanvasSize() const = 0;
    
    // Export and import
    virtual bool exportData(const std::string& filepath, OutputFormat format) = 0;
    virtual bool importData(const std::string& filepath) = 0;
    
    // Visualization control
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    
    // Query current state
    virtual std::string getVisualizationInfo() const = 0;
    virtual py::dict getVisualizationState() const = 0;
    virtual size_t getDataPointCount() const = 0;
    
    // Callbacks
    virtual void setUpdateCallback(const std::function<void(const NeuralActivityData&)>& callback) = 0;
    virtual void setEventCallback(const std::function<void(const VisualizationEventData&)>& callback) = 0;
    
    // Static factory methods
    static std::unique_ptr<VisualizationInterface> create2DVisualizer(const std::string& title = "2D Neural Activity");
    static std::unique_ptr<VisualizationInterface> create3DVisualizer(const std::string& title = "3D Brain Model");
    static std::unique_ptr<VisualizationInterface> createTimeSeriesVisualizer(const std::string& title = "Time Series Analysis");
    static std::unique_ptr<VisualizationInterface> createSpatialVisualizer(const std::string& title = "Spatial Distribution");
    static std::unique_ptr<VisualizationInterface> createConnectivityVisualizer(const std::string& title = "Connectivity Matrix");
};

// Neural activity visualizer (2D)
class NeuralActivityVisualizer : public VisualizationInterface {
private:
    struct NeuronData {
        NeuronId id;
        float x;
        float y;
        float firing_rate;
        float membrane_potential;
        float radius;
        std::vector<NeuronId> connected_neurons;
        std::vector<float> connection_strengths;
        bool is_active;
        double last_spike_time;
    };
    
    struct SynapseData {
        NeuronId source_id;
        NeuronId target_id;
        float weight;
        float delay;
        float activity;
        bool is_active;
    };
    
    struct PopulationData {
        PopulationId id;
        NeuronType neuron_type;
        std::vector<NeuronId> neurons;
        float average_firing_rate;
        float coherence;
        float synchrony;
    };
    
    std::vector<NeuronData> neurons_;
    std::vector<SynapseData> synapses_;
    std::vector<PopulationData> populations_;
    
    // Visualization parameters
    float neuron_scale_;
    float connection_threshold_;
    bool show_neurons_;
    bool show_synapses_;
    bool show_populations_;
    bool show_labels_;
    bool show_activity_;
    bool highlight_active_;
    
    // Animation and interaction
    double animation_speed_;
    double time_offset_;
    bool is_animating_;
    
    // Color schemes
    std::vector<std::string> getColorForType(NeuronType type) const;
    std::string getColorForFiringRate(float rate) const;
    std::string getColorForActivity(float activity) const;
    
public:
    NeuralActivityVisualizer() = default;
    ~NeuralActivityVisualizer() override = default;
    
    bool initialize(const std::string& config_file = "") override;
    
    void update(const NeuralActivityData& neural_data) override;
    void update(const MemoryStateData& memory_data) override {} // Not applicable
    void update(const NeuromodulationData& neuromod_data) override {} // Not applicable
    void update(const DevelopmentData& dev_data) override {} // Not applicable
    void update(const PerformanceMetricsData& perf_data) override {} // Not applicable
    
    void handleEvent(const VisualizationEventData& event) override;
    
    void setVisualizationType(VisualizationType type) override;
    void setColorScheme(ColorScheme scheme) override;
    void setUpdateFrequency(UpdateFrequency freq) override;
    void setOutputFormat(OutputFormat format) override;
    
    void setDataFilter(const std::string& filter_name, const py::dict& parameters) override;
    py::dict getDataFilter() const override;
    
    void setInteractive(bool interactive) override;
    bool isInteractive() const override;
    void setCanvasSize(int width, int height) override;
    std::pair<int, int> getCanvasSize() const override;
    
    bool exportData(const std::string& filepath, OutputFormat format) override;
    bool importData(const std::string& filepath) override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override;
    void pause() override;
    void resume() override;
    
    std::string getVisualizationInfo() const override;
    py::dict getVisualizationState() const override;
    size_t getDataPointCount() const override;
    
    void setUpdateCallback(const std::function<void(const NeuralActivityData&)>& callback) override;
    void setEventCallback(const std::function<void(const VisualizationEventData&)>& callback) override;
    
    // Neural activity specific methods
    void clearNeurons();
    void addNeuron(const NeuronId& id, float x, float y, NeuronType type);
    void updateNeuronActivity(const NeuronId& id, float firing_rate, float membrane_potential, bool active);
    void addSynapse(const NeuronId& source, const NeuronId& target, float weight, float delay);
    void updateSynapseActivity(const NeuronId& source, const NeuronId& target, float activity);
    void addPopulation(const PopulationId& id, NeuronType type, const std::vector<NeuronId>& neurons);
    
    // Visualization control
    void setNeuronScale(float scale);
    void setConnectionThreshold(float threshold);
    void toggleShowNeurons(bool show);
    void toggleShowSynapses(bool show);
    void toggleShowPopulations(bool show);
    void toggleShowLabels(bool show);
    void toggleShowActivity(bool show);
    void toggleHighlightActive(bool highlight);
    
    // Animation control
    void setAnimationSpeed(double speed);
    void setTimeOffset(double offset);
    void startAnimation();
    void stopAnimation();
    bool isAnimating() const;
    
    // Query methods
    const std::vector<NeuronData>& getNeurons() const { return neurons_; }
    const std::vector<SynapseData>& getSynapses() const { return synapses_; }
    const std::vector<PopulationData>& getPopulations() const { return populations_; }
    
    // Export visualization data
    std::string exportSVG() const;
    std::string exportJSON() const;
    py::dict exportPythonDict() const;
    
    // Statistics
    float getAverageFiringRate() const;
    float getNetworkDensity() const;
    float getSparsity() const;
    float getClusteringCoefficient() const;
};

// Memory system visualizer
class MemorySystemVisualizer : public VisualizationInterface {
private:
    struct WorkingMemoryTrace {
        NeuronId neuron_id;
        float activation;
        double timestamp;
        float decay_rate;
    };
    
    struct EpisodicMemoryEntry {
        double timestamp;
        float reward;
        std::vector<NeuronId> active_neurons;
        std::vector<float> neuron_activations;
        std::string episode_id;
        std::string summary;
    };
    
    struct AssociativeMemoryPattern {
        std::vector<float> pattern;
        std::vector<NeuronId> associated_neurons;
        float strength;
        double last_accessed;
    };
    
    std::vector<WorkingMemoryTrace> working_memory_traces_;
    std::vector<EpisodicMemoryEntry> episodic_entries_;
    std::vector<AssociativeMemoryPattern> associative_patterns_;
    
    // Visualization parameters
    bool show_working_memory_;
    bool show_episodic_memory_;
    bool show_associative_memory_;
    bool show_connections_;
    bool show_strengths_;
    
    // Time series data
    std::vector<double> working_memory_time_series_;
    std::vector<double> episodic_count_time_series_;
    
public:
    MemorySystemVisualizer() = default;
    ~MemorySystemVisualizer() override = default;
    
    bool initialize(const std::string& config_file = "") override;
    
    void update(const NeuralActivityData& neural_data) override {} // Not applicable
    void update(const MemoryStateData& memory_data) override;
    void update(const NeuromodulationData& neuromod_data) override {} // Not applicable
    void update(const DevelopmentData& dev_data) override {} // Not applicable
    void update(const PerformanceMetricsData& perf_data) override {} // Not applicable
    
    void handleEvent(const VisualizationEventData& event) override;
    
    void setVisualizationType(VisualizationType type) override;
    void setColorScheme(ColorScheme scheme) override;
    void setUpdateFrequency(UpdateFrequency freq) override;
    void setOutputFormat(OutputFormat format) override;
    
    void setDataFilter(const std::string& filter_name, const py::dict& parameters) override;
    py::dict getDataFilter() const override;
    
    void setInteractive(bool interactive) override;
    bool isInteractive() const override;
    void setCanvasSize(int width, int height) override;
    std::pair<int, int> getCanvasSize() const override;
    
    bool exportData(const std::string& filepath, OutputFormat format) override;
    bool importData(const std::string& filepath) override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override;
    void pause() override;
    void resume() override;
    
    std::string getVisualizationInfo() const override;
    py::dict getVisualizationState() const override;
    size_t getDataPointCount() const override;
    
    void setUpdateCallback(const std::function<void(const MemoryStateData&)>& callback) override;
    void setEventCallback(const std::function<void(const VisualizationEventData&)>& callback) override;
    
    // Memory system specific methods
    void clearWorkingMemory();
    void addWorkingMemoryTrace(const NeuronId& neuron_id, float activation, float decay_rate);
    void updateWorkingMemoryTrace(const NeuronId& neuron_id, float activation);
    
    void clearEpisodicMemory();
    void addEpisodicEntry(const EpisodicMemoryEntry& entry);
    void updateEpisodicEntry(const std::string& episode_id, const EpisodicMemoryEntry& entry);
    
    void clearAssociativeMemory();
    void addAssociativePattern(const AssociativeMemoryPattern& pattern);
    void updateAssociativePattern(const std::vector<float>& pattern, float strength);
    
    // Analysis methods
    float getWorkingMemoryLoad() const;
    size_t getEpisodicMemoryCount() const;
    size_t getAssociativeMemoryCount() const;
    float getMemoryConsolidationIndex() const;
    
    // Export visualization data
    std::string exportWorkingMemorySVG() const;
    std::string exportEpisodicMemorySVG() const;
    std::string exportAssociativeMemorySVG() const;
    std::string exportMemoryNetworkSVG() const;
};

// Neuromodulation visualizer
class NeuromodulationVisualizer : public VisualizationInterface {
private:
    struct NeuromodulatorData {
        std::string name;
        float level;
        float target_level;
        float change_rate;
        std::vector<double> history;
        double last_update;
    };
    
    std::vector<NeuromodulatorData> neuromodulators_;
    
    // Visualization parameters
    bool show_levels_;
    bool show_targets_;
    bool show_changes_;
    bool show_history_;
    bool show_correlations_;
    
    // Time series data
    std::vector<double> time_axis_;
    
public:
    NeuromodulationVisualizer() = default;
    ~NeuromodulationVisualizer() override = default;
    
    bool initialize(const std::string& config_file = "") override;
    
    void update(const NeuralActivityData& neural_data) override {} // Not applicable
    void update(const MemoryStateData& memory_data) override {} // Not applicable
    void update(const NeuromodulationData& neuromod_data) override;
    void update(const DevelopmentData& dev_data) override {} // Not applicable
    void update(const PerformanceMetricsData& perf_data) override {} // Not applicable
    
    void handleEvent(const VisualizationEventData& event) override;
    
    void setVisualizationType(VisualizationType type) override;
    void setColorScheme(ColorScheme scheme) override;
    void setUpdateFrequency(UpdateFrequency freq) override;
    void setOutputFormat(OutputFormat format) override;
    
    void setDataFilter(const std::string& filter_name, const py::dict& parameters) override;
    py::dict getDataFilter() const override;
    
    void setInteractive(bool interactive) override;
    bool isInteractive() const override;
    void setCanvasSize(int width, int height) override;
    std::pair<int, int> getCanvasSize() const override;
    
    bool exportData(const std::string& filepath, OutputFormat format) override;
    bool importData(const std::string& filepath) override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override;
    void pause() override;
    void resume() override;
    
    std::string getVisualizationInfo() const override;
    py::dict getVisualizationState() const override;
    size_t getDataPointCount() const override;
    
    void setUpdateCallback(const std::function<void(const NeuromodulationData&)>& callback) override;
    void setEventCallback(const std::function<void(const VisualizationEventData&)>& callback) override;
    
    // Neuromodulation specific methods
    void clearNeuromodulators();
    void addNeuromodulator(const std::string& name, float initial_level, float target_level);
    void updateNeuromodulator(const std::string& name, float level, float change_rate);
    
    // Analysis methods
    float getAverageNeuromodulationLevel() const;
    float getNeuromodulationVariability() const;
    float getCorrelationCoefficient(const std::string& modulator1, const std::string& modulator2) const;
    
    // Export visualization data
    std::string exportNeuromodulationSVG() const;
    std::string exportNeuromodulationJSON() const;
    std::string exportNeuromodulationHTML() const;
};

// Development system visualizer
class DevelopmentVisualizer : public VisualizationInterface {
private:
    struct DevelopmentStageData {
        DevelopmentalStage stage;
        float plasticity_modifier;
        float structural_plasticity_rate;
        float synaptogenesis_rate;
        float pruning_rate;
        double start_time;
        double end_time;
        std::string description;
    };
    
    std::vector<DevelopmentStageData> development_stages_;
    
    // Visualization parameters
    bool show_stages_;
    bool show_plasticity_;
    bool show_structural_changes_;
    bool show_temporal_dynamics_;
    
    // Time series data
    std::vector<double> development_time_axis_;
    
public:
    DevelopmentVisualizer() = default;
    ~DevelopmentVisualizer() override = default;
    
    bool initialize(const std::string& config_file = "") override;
    
    void update(const NeuralActivityData& neural_data) override {} // Not applicable
    void update(const MemoryStateData& memory_data) override {} // Not applicable
    void update(const NeuromodulationData& neuromod_data) override {} // Not applicable
    void update(const DevelopmentData& dev_data) override;
    void update(const PerformanceMetricsData& perf_data) override {} // Not applicable
    
    void handleEvent(const VisualizationEventData& event) override;
    
    void setVisualizationType(VisualizationType type) override;
    void setColorScheme(ColorScheme scheme) override;
    void setUpdateFrequency(UpdateFrequency freq) override;
    void setOutputFormat(OutputFormat format) override;
    
    void setDataFilter(const std::string& filter_name, const py::dict& parameters) override;
    py::dict getDataFilter() const override;
    
    void setInteractive(bool interactive) override;
    bool isInteractive() const override;
    void setCanvasSize(int width, int height) override;
    std::pair<int, int> getCanvasSize() const override;
    
    bool exportData(const std::string& filepath, OutputFormat format) override;
    bool importData(const std::string& filepath) override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override;
    void pause() override;
    void resume() override;
    
    std::string getVisualizationInfo() const override;
    py::dict getVisualizationState() const override;
    size_t getDataPointCount() const override;
    
    void setUpdateCallback(const std::function<void(const DevelopmentData&)>& callback) override;
    void setEventCallback(const std::function<void(const VisualizationEventData&)>& callback) override;
    
    // Development system specific methods
    void clearDevelopmentStages();
    void addDevelopmentStage(const DevelopmentStageData& stage);
    void updateDevelopmentStage(const DevelopmentalStage& stage, const DevelopmentData& data);
    
    // Analysis methods
    DevelopmentalStage getCurrentDevelopmentalStage() const;
    float getCurrentPlasticityModifier() const;
    double getDevelopmentalProgress() const;
    float getAgeRelatedPlasticityChange() const;
    
    // Export visualization data
    std::string exportDevelopmentSVG() const;
    std::string exportDevelopmentTimeline() const;
    std::string exportDevelopmentJSON() const;
};

// Performance monitoring visualizer
class PerformanceMonitor : public VisualizationInterface {
private:
    struct PerformanceMetric {
        std::string name;
        float current_value;
        float min_value;
        float max_value;
        std::vector<double> history;
        double last_update;
        std::string unit;
    };
    
    std::vector<PerformanceMetric> metrics_;
    
    // Visualization parameters
    bool show_histogram_;
    bool show_trend_lines_;
    bool show_alerts_;
    bool show_thresholds_;
    double alert_threshold_;
    
    // Time series data
    std::vector<double> time_axis_;
    
public:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() override = default;
    
    bool initialize(const std::string& config_file = "") override;
    
    void update(const NeuralActivityData& neural_data) override {} // Not applicable
    void update(const MemoryStateData& memory_data) override {} // Not applicable
    void update(const NeuromodulationData& neuromod_data) override {} // Not applicable
    void update(const DevelopmentData& dev_data) override {} // Not applicable
    void update(const PerformanceMetricsData& perf_data) override;
    
    void handleEvent(const VisualizationEventData& event) override;
    
    void setVisualizationType(VisualizationType type) override;
    void setColorScheme(ColorScheme scheme) override;
    void setUpdateFrequency(UpdateFrequency freq) override;
    void setOutputFormat(OutputFormat format) override;
    
    void setDataFilter(const std::string& filter_name, const py::dict& parameters) override;
    py::dict getDataFilter() const override;
    
    void setInteractive(bool interactive) override;
    bool isInteractive() const override;
    void setCanvasSize(int width, int height) override;
    std::pair<int, int> getCanvasSize() const override;
    
    bool exportData(const std::string& filepath, OutputFormat format) override;
    bool importData(const std::string& filepath) override;
    
    void start() override;
    void stop() override;
    bool isRunning() override;
    void pause() override;
    void resume() override;
    
    std::string getVisualizationInfo() const override;
    py::dict getVisualizationState() const override;
    size_t getDataPointCount() const override;
    
    void setUpdateCallback(const std::function<void(const PerformanceMetricsData&)>& callback) override;
    void setEventCallback(const std::function<void(const VisualizationEventData&)>& callback) override;
    
    // Performance monitoring specific methods
    void clearMetrics();
    void addMetric(const std::string& name, float current, float min, float max, const std::string& unit);
    void updateMetric(const std::string& name, float value);
    
    // Analysis methods
    float getAveragePerformance() const;
    float getPeakPerformance() const;
    float getPerformanceTrend() const;
    std::vector<std::string> getAlertMessages() const;
    
    // Export visualization data
    std::string exportPerformanceSVG() const;
    std::string exportPerformanceJSON() const;
    std::string exportPerformanceHTML() const;
};

// Interactive visualization system
class InteractiveVisualizer : public VisualizationInterface {
private:
    std::unique_ptr<VisualizationInterface> primary_visualizer_;
    std::unordered_map<std::string, std::unique_ptr<VisualizationInterface>> secondary_visualizers_;
    
    // Interaction state
    bool is_dragging_;
    bool is_zooming_;
    bool is_panning_;
    bool is_selecting_;
    
    // Selection data
    std::vector<std::string> selected_items_;
    
    // Event handling
    std::function<void(const VisualizationEventData&)> event_callback_;
    std::function<void(const NeuralActivityData&)> neural_callback_;
    std::function<void(const MemoryStateData&)> memory_callback_;
    std::function<void(const NeuromodulationData&)> neuromod_callback_;
    std::function<void(const DevelopmentData&)> dev_callback_;
    std::function<void(const PerformanceMetricsData&)> perf_callback_;
    
public:
    InteractiveVisualizer() = default;
    ~InteractiveVisualizer() override = default;
    
    bool initialize(const std::string& config_file = "") override;
    
    void update(const NeuralActivityData& neural_data) override;
    void update(const MemoryStateData& memory_data) override;
    void update(const NeuromodulationData& neuromod_data) override;
    void update(const DevelopmentData& dev_data) override;
    void update(const PerformanceMetricsData& perf_data) override;
    
    void handleEvent(const VisualizationEventData& event) override;
    
    void setVisualizationType(VisualizationType type) override;
    void setColorScheme(ColorScheme scheme) override;
    void setUpdateFrequency(UpdateFrequency freq) override;
    void setOutputFormat(OutputFormat format) override;
    
    void setDataFilter(const std::string& filter_name, const py::dict& parameters) override;
    py::dict getDataFilter() const override;
    
    void setInteractive(bool interactive) override;
    bool isInteractive() override;
    void setCanvasSize(int width, int height) override;
    std::pair<int, int> getCanvasSize() const override;
    
    bool exportData(const std::string& filepath, OutputFormat format) override;
    bool importData(const std::string& filepath) override;
    
    void start() override;
    void stop() override;
    bool isRunning() override;
    void pause() override;
    void resume() override;
    
    std::string getVisualizationInfo() const override;
    py::dict getVisualizationState() const override;
    size_t getDataPointCount() const override;
    
    void setUpdateCallback(const std::function<void(const NeuralActivityData&)>& callback) override;
    void setEventCallback(const std::function<void(const VisualizationEventData&)>& callback) override;
    
    // Interaction management
    void addSecondaryVisualizer(const std::string& name, std::unique_ptr<VisualizationInterface> visualizer);
    void setPrimaryVisualizer(std::unique_ptr<VisualizationInterface> visualizer);
    VisualizationInterface* getPrimaryVisualizer() const { return primary_visualizer_.get(); }
    VisualizationInterface* getSecondaryVisualizer(const std::string& name) const;
    
    // Multi-view management
    void createMultiView(const std::vector<std::string>& visualizer_names);
    void removeMultiView(const std::string& view_name);
    void updateMultiView(const std::string& view_name);
    
    // Export and import
    std::string exportInteractiveLayout() const;
    bool importInteractiveLayout(const std::string& layout_data);
    
    // Statistics
    std::vector<std::string> getAvailableVisualizers() const;
    bool hasVisualizer(const std::string& name) const;
};

// Visualization factory and utility functions
class VisualizationFactory {
public:
    static std::unique_ptr<VisualizationInterface> createVisualizer(VisualizationType type, const std::string& title = "");
    static std::unique_ptr<VisualizationInterface> createVisualizer(const std::string& type_name, const std::string& title = "");
    
    static std::vector<VisualizationType> getAvailableTypes();
    static std::string getTypeName(VisualizationType type);
    static ColorScheme getDefaultColorScheme(VisualizationType type);
    static UpdateFrequency getDefaultUpdateFrequency(VisualizationType type);
    
    static std::unique_ptr<VisualizationInterface> createIntegratedVisualizer(
        const std::vector<VisualizationType>& types,
        const std::string& layout = "grid"
    );
    
    static std::unique_ptr<VisualizationInterface> createFromConfig(const std::string& config_file);
    
    static py::dict getVisualizationCapabilities();
    static bool isVisualizationSupported(VisualizationType type);
};

// Utility functions
namespace utils {
    std::string generateVisualizationID();
    
    py::dict convertDataToPython(const NeuralActivityData& data);
    py::dict convertDataToPython(const MemoryStateData& data);
    py::dict convertDataToPython(const NeuromodulationData& data);
    py::dict convertDataToPython(const DevelopmentData& data);
    py::dict convertDataToPython(const PerformanceMetricsData& data);
    
    NeuralActivityData convertPythonToNeuralActivity(const py::dict& data);
    MemoryStateData convertPythonToMemoryState(const py::dict& data);
    NeuromodulationData convertPythonToNeuromodulation(const py::dict& data);
    DevelopmentData convertPythonToDevelopment(const py::dict& data);
    PerformanceMetricsData convertPythonToPerformance(const py::dict& data);
    
    std::string visualizationTypeToString(VisualizationType type);
    std::string colorSchemeToString(ColorScheme scheme);
    std::string updateFrequencyToString(UpdateFrequency freq);
    std::string outputFormatToString(OutputFormat format);
    
    VisualizationType stringToVisualizationType(const std::string& str);
    ColorScheme stringToColorScheme(const std::string& str);
    UpdateFrequency stringToUpdateFrequency(const std::string& str);
    OutputFormat stringToOutputFormat(const std::string& str);
    
    bool validateData(const NeuralActivityData& data);
    bool validateData(const MemoryStateData& data);
    bool validateData(const NeuromodulationData& data);
    bool validateData(const DevelopmentData& data);
    bool validateData(const PerformanceMetricsData& data);
    
    std::string getVisualizationInfo(const NeuralActivityData& data);
    std::string getVisualizationInfo(const MemoryStateData& data);
    std::string getVisualizationInfo(const NeuromodulationData& data);
    std::string getVisualizationInfo(const DevelopmentData& data);
    std::string getVisualizationInfo(const PerformanceMetricsData& data);
    
    std::string generateReport(const NeuralActivityData& neural_data,
                               const MemoryStateData& memory_data,
                               const NeuromodulationData& neuromod_data,
                               const DevelopmentData& dev_data,
                               const PerformanceMetricsData& perf_data);
}

// Pybind11 module definitions
PYBIND11_MODULE(pynlm_viz, m) {
    m.doc() = R"pbdoc(
        NLM Advanced Visualization Tools
        -------------------------------
        
        Comprehensive visualization system for NLM Phase 6 brain simulation.
        Provides real-time visualization of neural activity, memory systems,
        neuromodulation, and development with interactive capabilities.
        
        Features:
        - 2D and 3D visualization of neural activity
        - Memory system visualization (working, episodic, associative)
        - Neuromodulation system visualization (dopamine, curiosity, novelty)
        - Development system visualization (plasticity, stages)
        - Performance monitoring and benchmarking visualization
        - Interactive multi-view visualization
        - Export to multiple formats (SVG, PNG, PDF, HTML, JSON)
        - Real-time data updates and animation
        - Custom visualization layouts
        - Event-driven visualization updates
        
        Usage:
        ```python
        import pynlm_viz
        
        # Create neural activity visualizer
        viz = pynlm_viz.VisualizationFactory.createVisualizer(pynlm_viz.VisualizationType.2D)
        viz.setCanvasSize(800, 600)
        
        # Update with neural data
        neural_data = pynlm_viz.NeuralActivityData()
        # ... populate neural data
        viz.update(neural_data)
        
        # Start visualization
        viz.start()
        ```
    )pbdoc";

    // Exception types
    py::register_exception<std::runtime_error>(m, "VisualizationError", PyExc_RuntimeError);

    // Basic type bindings
    py::enum_<VisualizationType>(m, "VisualizationType", R"pbdoc(Visualization type enumeration)pbdoc")
        .value("2D", VisualizationType::2D)
        .value("3D", VisualizationType::3D)
        .value("TIME_SERIES", VisualizationType::TIME_SERIES)
        .value("SPATIAL", VisualizationType::SPATIAL)
        .value("CONNECTIVITY", VisualizationType::CONNECTIVITY)
        .value("SYNAPTIC", VisualizationType::SYNAPTIC)
        .value("POPULATION", VisualizationType::POPULATION)
        .value("NETWORK", VisualizationType::NETWORK)
        .value("MULTIPLEXED", VisualizationType::MULTIPLEXED)
        .export_values();

    py::enum_<UpdateFrequency>(m, "UpdateFrequency", R"pbdoc(Update frequency enumeration)pbdoc")
        .value("REAL_TIME", UpdateFrequency::REAL_TIME)
        .value("HIGH", UpdateFrequency::HIGH)
        .value("MEDIUM", UpdateFrequency::MEDIUM)
        .value("LOW", UpdateFrequency::LOW)
        .value("MINIMAL", UpdateFrequency::MINIMAL)
        .export_values();

    py::enum_<OutputFormat>(m, "OutputFormat", R"pbdoc(Output format enumeration)pbdoc")
        .value("TEXT", OutputFormat::TEXT)
        .value("SVG", OutputFormat::SVG)
        .value("PNG", OutputFormat::PNG)
        .value("PDF", OutputFormat::PDF)
        .value("HTML", OutputFormat::HTML)
        .value("JSON", OutputFormat::JSON)
        .value("VIDEO", OutputFormat::VIDEO)
        .export_values();

    py::enum_<ColorScheme>(m, "ColorScheme", R"pbdoc(Color scheme enumeration)pbdoc")
        .value("DEFAULT", ColorScheme::DEFAULT)
        .value("NEURAL", ColorScheme::NEURAL)
        .value("MEMORY", ColorScheme::MEMORY)
        .value("NEUROMODULATION", ColorScheme::NEUROMODULATION)
        .value("COGNITION", ColorScheme::COGNITION)
        .value("DEVELOPMENT", ColorScheme::DEVELOPMENT)
        .value("SCIENTIFIC", ColorScheme::SCIENTIFIC)
        .value("ARTISTIC", ColorScheme::ARTISTIC)
        .value("MONOCHROMATIC", ColorScheme::MONOCHROMATIC)
        .value("HIGH_CONTRAST", ColorScheme::HIGH_CONTRAST)
        .export_values();

    // Data structure bindings
    py::class_<NeuralActivityData>(m, "NeuralActivityData", R"pbdoc(Neural activity data)")
        .def(py::init<>())
        .def_readwrite("firing_rates", &NeuralActivityData::firing_rates)
        .def_readwrite("membrane_potentials", &NeuralActivityData::membrane_potentials)
        .def_readwrite("synaptic_activities", &NeuralActivityData::synaptic_activities)
        .def_readwrite("spike_times", &NeuralActivityData::spike_times)
        .def_readwrite("active_neurons", &NeuralActivityData::active_neurons);

    py::class_<MemoryStateData>(m, "MemoryStateData", R"pbdoc(Memory state data)")
        .def(py::init<>())
        .def_readwrite("working_memory_traces", &MemoryStateData::working_memory_traces)
        .def_readwrite("episodic_activations", &MemoryStateData::episodic_activations)
        .def_readwrite("associative_strengths", &MemoryStateData::associative_strengths)
        .def_readwrite("episodic_neurons", &MemoryStateData::episodic_neurons);

    py::class_<NeuromodulationData>(m, "NeuromodulationData", R"pbdoc(Neuromodulation data)")
        .def(py::init<>())
        .def_readwrite("dopamine_level", &NeuromodulationData::dopamine_level)
        .def_readwrite("curiosity_level", &NeuromodulationData::curiosity_level)
        .def_readwrite("novelty_level", &NeuromodulationData::novelty_level)
        .def_readwrite("prediction_error_level", &NeuromodulationData::prediction_error_level);

    py::class_<DevelopmentData>(m, "DevelopmentData", R"pbdoc(Development data)")
        .def(py::init<>())
        .def_readwrite("developmental_stage", &DevelopmentData::developmental_stage)
        .def_readwrite("plasticity_modifier", &DevelopmentData::plasticity_modifier)
        .def_readwrite("structural_plasticity_rate", &DevelopmentData::structural_plasticity_rate)
        .def_readwrite("synaptogenesis_rate", &DevelopmentData::synaptogenesis_rate)
        .def_readwrite("pruning_rate", &DevelopmentData::pruning_rate)
        .def_readwrite("developmental_time", &DevelopmentData::developmental_time);

    py::class_<PerformanceMetricsData>(m, "PerformanceMetricsData", R"pbdoc(Performance metrics data)")
        .def(py::init<>())
        .def_readwrite("simulation_speed", &PerformanceMetricsData::simulation_speed)
        .def_readwrite("cpu_usage", &PerformanceMetricsData::cpu_usage)
        .def_readwrite("memory_usage_mb", &PerformanceMetricsData::memory_usage_mb)
        .def_readwrite("active_threads", &PerformanceMetricsData::active_threads)
        .def_readwrite("pending_events", &PerformanceMetricsData::pending_events)
        .def_readwrite("frame_time_ms", &PerformanceMetricsData::frame_time_ms);

    // Event data binding
    py::class_<VisualizationEventData>(m, "VisualizationEventData", R"pbdoc(Visualization event data)")
        .def(py::init<>())
        .def_readwrite("event_type", &VisualizationEventData::event_type)
        .def_readwrite("event_id", &VisualizationEventData::event_id)
        .def_readwrite("target", &VisualizationEventData::target)
        .def_readwrite("parameters", &VisualizationEventData::parameters);

    // Main visualization interface
    py::class_<VisualizationInterface, std::unique_ptr<VisualizationInterface>>(m, "VisualizationInterface", R"pbdoc(Base visualization interface)")
        .def("initialize", &VisualizationInterface::initialize, py::arg("config_file") = "")
        .def("update", static_cast<void (VisualizationInterface::*)(const NeuralActivityData&)>(&VisualizationInterface::update), py::arg("neural_data"))
        .def("update", static_cast<void (VisualizationInterface::*)(const MemoryStateData&)>(&VisualizationInterface::update), py::arg("memory_data"))
        .def("update", static_cast<void (VisualizationInterface::*)(const NeuromodulationData&)>(&VisualizationInterface::update), py::arg("neuromod_data"))
        .def("update", static_cast<void (VisualizationInterface::*)(const DevelopmentData&)>(&VisualizationInterface::update), py::arg("dev_data"))
        .def("update", static_cast<void (VisualizationInterface::*)(const PerformanceMetricsData&)>(&VisualizationInterface::update), py::arg("perf_data"))
        .def("handleEvent", &VisualizationInterface::handleEvent, py::arg("event"))
        .def("setVisualizationType", &VisualizationInterface::setVisualizationType, py::arg("type"))
        .def("setColorScheme", &VisualizationInterface::setColorScheme, py::arg("scheme"))
        .def("setUpdateFrequency", &VisualizationInterface::setUpdateFrequency, py::arg("freq"))
        .def("setOutputFormat", &VisualizationInterface::setOutputFormat, py::arg("format"))
        .def("setDataFilter", &VisualizationInterface::setDataFilter, py::arg("filter_name"), py::arg("parameters"))
        .def("getDataFilter", &VisualizationInterface::getDataFilter)
        .def("setInteractive", &VisualizationInterface::setInteractive, py::arg("interactive"))
        .def("isInteractive", &VisualizationInterface::isInteractive)
        .def("setCanvasSize", &VisualizationInterface::setCanvasSize, py::arg("width"), py::arg("height"))
        .def("getCanvasSize", &VisualizationInterface::getCanvasSize)
        .def("exportData", &VisualizationInterface::exportData, py::arg("filepath"), py::arg("format"))
        .def("importData", &VisualizationInterface::importData, py::arg("filepath"))
        .def("start", &VisualizationInterface::start)
        .def("stop", &VisualizationInterface::stop)
        .def("isRunning", &VisualizationInterface::isRunning)
        .def("pause", &VisualizationInterface::pause)
        .def("resume", &VisualizationInterface::resume)
        .def("getVisualizationInfo", &VisualizationInterface::getVisualizationInfo)
        .def("getVisualizationState", &VisualizationInterface::getVisualizationState)
        .def("getDataPointCount", &VisualizationInterface::getDataPointCount)
        .def("setUpdateCallback", &VisualizationInterface::setUpdateCallback, py::arg("callback"))
        .def("setEventCallback", &VisualizationInterface::setEventCallback, py::arg("callback"));

    // Neural activity visualizer
    py::class_<NeuralActivityVisualizer, std::unique_ptr<NeuralActivityVisualizer>>(m, "NeuralActivityVisualizer", R"pbdoc(Neural activity visualizer)")
        .def(py::init<>())
        .def("clearNeurons", &NeuralActivityVisualizer::clearNeurons)
        .def("addNeuron", &NeuralActivityVisualizer::addNeuron, py::arg("id"), py::arg("x"), py::arg("y"), py::arg("type"))
        .def("updateNeuronActivity", &NeuralActivityVisualizer::updateNeuronActivity, py::arg("id"), py::arg("firing_rate"), py::arg("membrane_potential"), py::arg("active"))
        .def("addSynapse", &NeuralActivityVisualizer::addSynapse, py::arg("source"), py::arg("target"), py::arg("weight"), py::arg("delay"))
        .def("updateSynapseActivity", &NeuralActivityVisualizer::updateSynapseActivity, py::arg("source"), py::arg("target"), py::arg("activity"))
        .def("addPopulation", &NeuralActivityVisualizer::addPopulation, py::arg("id"), py::arg("type"), py::arg("neurons"))
        .def("setNeuronScale", &NeuralActivityVisualizer::setNeuronScale, py::arg("scale"))
        .def("setConnectionThreshold", &NeuralActivityVisualizer::setConnectionThreshold, py::arg("threshold"))
        .def("toggleShowNeurons", &NeuralActivityVisualizer::toggleShowNeurons, py::arg("show"))
        .def("toggleShowSynapses", &NeuralActivityVisualizer::toggleShowSynapses, py::arg("show"))
        .def("toggleShowPopulations", &NeuralActivityVisualizer::toggleShowPopulations, py::arg("show"))
        .def("toggleShowLabels", &NeuralActivityVisualizer::toggleShowLabels, py::arg("show"))
        .def("toggleShowActivity", &NeuralActivityVisualizer::toggleShowActivity, py::arg("show"))
        .def("toggleHighlightActive", &NeuralActivityVisualizer::toggleHighlightActive, py::arg("highlight"))
        .def("setAnimationSpeed", &NeuralActivityVisualizer::setAnimationSpeed, py::arg("speed"))
        .def("setTimeOffset", &NeuralActivityVisualizer::setTimeOffset, py::arg("offset"))
        .def("startAnimation", &NeuralActivityVisualizer::startAnimation)
        .def("stopAnimation", &NeuralActivityVisualizer::stopAnimation)
        .def("isAnimating", &NeuralActivityVisualizer::isAnimating)
        .def("getAverageFiringRate", &NeuralActivityVisualizer::getAverageFiringRate)
        .def("getNetworkDensity", &NeuralActivityVisualizer::getNetworkDensity)
        .def("getSparsity", &NeuralActivityVisualizer::getSparsity)
        .def("getClusteringCoefficient", &NeuralActivityVisualizer::getClusteringCoefficient);

    // Memory system visualizer
    py::class_<MemorySystemVisualizer, std::unique_ptr<MemorySystemVisualizer>>(m, "MemorySystemVisualizer", R"pbdoc(Memory system visualizer)")
        .def(py::init<>())
        .def("clearWorkingMemory", &MemorySystemVisualizer::clearWorkingMemory)
        .def("addWorkingMemoryTrace", &MemorySystemVisualizer::addWorkingMemoryTrace, py::arg("neuron_id"), py::arg("activation"), py::arg("decay_rate"))
        .def("updateWorkingMemoryTrace", &MemorySystemVisualizer::updateWorkingMemoryTrace, py::arg("neuron_id"), py::arg("activation"))
        .def("clearEpisodicMemory", &MemorySystemVisualizer::clearEpisodicMemory)
        .def("addEpisodicEntry", &MemorySystemVisualizer::addEpisodicEntry, py::arg("entry"))
        .def("updateEpisodicEntry", &MemorySystemVisualizer::updateEpisodicEntry, py::arg("episode_id"), py::arg("entry"))
        .def("clearAssociativeMemory", &MemorySystemVisualizer::clearAssociativeMemory)
        .def("addAssociativePattern", &MemorySystemVisualizer::addAssociativePattern, py::arg("pattern"))
        .def("updateAssociativePattern", &MemorySystemVisualizer::updateAssociativePattern, py::arg("pattern"), py::arg("strength"))
        .def("getWorkingMemoryLoad", &MemorySystemVisualizer::getWorkingMemoryLoad)
        .def("getEpisodicMemoryCount", &MemorySystemVisualizer::getEpisodicMemoryCount)
        .def("getAssociativeMemoryCount", &MemorySystemVisualizer::getAssociativeMemoryCount)
        .def("getMemoryConsolidationIndex", &MemorySystemVisualizer::getMemoryConsolidationIndex);

    // Neuromodulation visualizer
    py::class_<NeuromodulationVisualizer, std::unique_ptr<NeuromodulationVisualizer>>(m, "NeuromodulationVisualizer", R"pbdoc(Neuromodulation visualizer)")
        .def(py::init<>())
        .def("clearNeuromodulators", &NeuromodulationVisualizer::clearNeuromodulators)
        .def("addNeuromodulator", &NeuromodulationVisualizer::addNeuromodulator, py::arg("name"), py::arg("initial_level"), py::arg("target_level"))
        .def("updateNeuromodulator", &NeuromodulationVisualizer::updateNeuromodulator, py::arg("name"), py::arg("level"), py::arg("change_rate"))
        .def("getAverageNeuromodulationLevel", &NeuromodulationVisualizer::getAverageNeuromodulationLevel)
        .def("getNeuromodulationVariability", &NeuromodulationVisualizer::getNeuromodulationVariability)
        .def("getCorrelationCoefficient", &NeuromodulationVisualizer::getCorrelationCoefficient, py::arg("modulator1"), py::arg("modulator2"));

    // Development visualizer
    py::class_<DevelopmentVisualizer, std::unique_ptr<DevelopmentVisualizer>>(m, "DevelopmentVisualizer", R"pbdoc(Development visualizer)")
        .def(py::init<>())
        .def("clearDevelopmentStages", &DevelopmentVisualizer::clearDevelopmentStages)
        .def("addDevelopmentStage", &DevelopmentVisualizer::addDevelopmentStage, py::arg("stage"))
        .def("updateDevelopmentStage", &DevelopmentVisualizer::updateDevelopmentStage, py::arg("stage"), py::arg("data"))
        .def("getCurrentDevelopmentalStage", &DevelopmentVisualizer::getCurrentDevelopmentalStage)
        .def("getCurrentPlasticityModifier", &DevelopmentVisualizer::getCurrentPlasticityModifier)
        .def("getDevelopmentalProgress", &DevelopmentVisualizer::getDevelopmentalProgress)
        .def("getAgeRelatedPlasticityChange", &DevelopmentVisualizer::getAgeRelatedPlasticityChange);

    // Performance monitor visualizer
    py::class_<PerformanceMonitor, std::unique_ptr<PerformanceMonitor>>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring visualizer)")
        .def(py::init<>())
        .def("clearMetrics", &PerformanceMonitor::clearMetrics)
        .def("addMetric", &PerformanceMonitor::addMetric, py::arg("name"), py::arg("current"), py::arg("min"), py::arg("max"), py::arg("unit"))
        .def("updateMetric", &PerformanceMonitor::updateMetric, py::arg("name"), py::arg("value"))
        .def("getAveragePerformance", &PerformanceMonitor::getAveragePerformance)
        .def("getPeakPerformance", &PerformanceMonitor::getPeakPerformance)
        .def("getPerformanceTrend", &PerformanceMonitor::getPerformanceTrend)
        .def("getAlertMessages", &PerformanceMonitor::getAlertMessages);

    // Interactive visualizer
    py::class_<InteractiveVisualizer, std::unique_ptr<InteractiveVisualizer>>(m, "InteractiveVisualizer", R"pbdoc(Interactive visualization system)")
        .def(py::init<>())
        .def("addSecondaryVisualizer", &InteractiveVisualizer::addSecondaryVisualizer, py::arg("name"), py::arg("visualizer"))
        .def("setPrimaryVisualizer", &InteractiveVisualizer::setPrimaryVisualizer, py::arg("visualizer"))
        .def("getSecondaryVisualizer", &InteractiveVisualizer::getSecondaryVisualizer, py::arg("name"))
        .def("createMultiView", &InteractiveVisualizer::createMultiView, py::arg("visualizer_names"))
        .def("removeMultiView", &InteractiveVisualizer::removeMultiView, py::arg("view_name"))
        .def("updateMultiView", &InteractiveVisualizer::updateMultiView, py::arg("view_name"))
        .def("getAvailableVisualizers", &InteractiveVisualizer::getAvailableVisualizers)
        .def("hasVisualizer", &InteractiveVisualizer::hasVisualizer, py::arg("name"));

    // Visualization factory
    py::class_<VisualizationFactory>(m, "VisualizationFactory", R"pbdoc(Visualization factory and utility functions)")
        .def("static", "createVisualizer", &VisualizationFactory::createVisualizer, py::arg("type"), py::arg("title") = "")
        .def("static", "createVisualizer", &VisualizationFactory::createVisualizer, py::arg("type_name"), py::arg("title") = "")
        .def("static", "getAvailableTypes", &VisualizationFactory::getAvailableTypes)
        .def("static", "getTypeName", &VisualizationFactory::getTypeName, py::arg("type"))
        .def("static", "getDefaultColorScheme", &VisualizationFactory::getDefaultColorScheme, py::arg("type"))
        .def("static", "getDefaultUpdateFrequency", &VisualizationFactory::getDefaultUpdateFrequency, py::arg("type"))
        .def("static", "createIntegratedVisualizer", &VisualizationFactory::createIntegratedVisualizer, py::arg("types"), py::arg("layout") = "grid")
        .def("static", "createFromConfig", &VisualizationFactory::createFromConfig, py::arg("config_file"))
        .def("static", "getVisualizationCapabilities", &VisualizationFactory::getVisualizationCapabilities)
        .def("static", "isVisualizationSupported", &VisualizationFactory::isVisualizationSupported, py::arg("type"));

    // Utility functions
    m.def("generateVisualizationID", &utils::generateVisualizationID);

    m.def("convertDataToPython", [](const NeuralActivityData& data) { return utils::convertDataToPython(data); });
    m.def("convertDataToPython", [](const MemoryStateData& data) { return utils::convertDataToPython(data); });
    m.def("convertDataToPython", [](const NeuromodulationData& data) { return utils::convertDataToPython(data); });
    m.def("convertDataToPython", [](const DevelopmentData& data) { return utils::convertDataToPython(data); });
    m.def("convertDataToPython", [](const PerformanceMetricsData& data) { return utils::convertDataToPython(data); });

    m.def("convertPythonToNeuralActivity", &utils::convertPythonToNeuralActivity, py::arg("data"));
    m.def("convertPythonToMemoryState", &utils::convertPythonToMemoryState, py::arg("data"));
 m.def("convertPythonToNeuromodulation", &utils::convertPythonToNeuromodulation, py::arg("data"));
    m.def("convertPythonToDevelopment", &utils::convertPythonToDevelopment, py::arg("data"));
    m.def("convertPythonToPerformance", &utils::convertPythonToPerformance, py::arg("data"));

    m.def("visualizationTypeToString", &utils::visualizationTypeToString, py::arg("type"));
    m.def("colorSchemeToString", &utils::colorSchemeToString, py::arg("scheme"));
    m.def("updateFrequencyToString", &utils::updateFrequencyToString, py::arg("freq"));
    m.def("outputFormatToString", &utils::outputFormatToString, py::arg("format"));

    m.def("stringToVisualizationType", &utils::stringToVisualizationType, py::arg("str"));
    m.def("stringToColorScheme", &utils::stringToColorScheme, py::arg("str"));
    m.def("stringToUpdateFrequency", &utils::stringToUpdateFrequency, py::arg("str"));
    m.def("stringToOutputFormat", &utils::stringToOutputFormat, py::arg("str"));

    m.def("validateData", [](const NeuralActivityData& data) { return utils::validateData(data); });
    m.def("validateData", [](const MemoryStateData& data) { return utils::validateData(data); });
    m.def("validateData", [](const NeuromodulationData& data) { return utils::validateData(data); });
    m.def("validateData", [](const DevelopmentData& data) { return utils::validateData(data); });
    m.def("validateData", [](const PerformanceMetricsData& data) { return utils::validateData(data); });

    m.def("getVisualizationInfo", [](const NeuralActivityData& data) { return utils::getVisualizationInfo(data); });
    m.def("getVisualizationInfo", [](const MemoryStateData& data) { return utils::getVisualizationInfo(data); });
    m.def("getVisualizationInfo", [](const NeuromodulationData& data) { return utils::getVisualizationInfo(data); });
    m.def("getVisualizationInfo", [](const DevelopmentData& data) { return utils::getVisualizationInfo(data); });
    m.def("getVisualizationInfo", [](const PerformanceMetricsData& data) { return utils::getVisualizationInfo(data); });

    m.def("generateReport", [](const NeuralActivityData& neural_data,
                              const MemoryStateData& memory_data,
                              const NeuromodulationData& neuromod_data,
                              const DevelopmentData& dev_data,
                              const PerformanceMetricsData& perf_data) {
        return utils::generateReport(neural_data, memory_data, neuromod_data, dev_data, perf_data);
    });

    // Constants and enumerations
    m.attr("NEURAL_ACTIVITY_DATA_TYPE") = py::cast(VisualizationType::2D);
    m.attr("NEUROMODULATION_DATA_TYPE") = py::cast(VisualizationType::NEUROMODULATION);
    m.attr("PERFORMANCE_DATA_TYPE") = py::cast(VisualizationType::TIME_SERIES);

    // Main factory functions
    m.def("create_neural_activity_visualizer", []() {
        return VisualizationFactory::createVisualizer(VisualizationType::2D);
    });

    m.def("create_3d_brain_visualizer", []() {
        return VisualizationFactory::createVisualizer(VisualizationType::3D);
    });

    m.def("create_time_series_visualizer", []() {
        return VisualizationFactory::createVisualizer(VisualizationType::TIME_SERIES);
    });

    m.def("create_spatial_visualizer", []() {
        return VisualizationFactory::createVisualizer(VisualizationType::SPATIAL);
    });

    m.def("create_connectivity_visualizer", []() {
        return VisualizationFactory::createVisualizer(VisualizationType::CONNECTIVITY);
    });

    m.def("create_network_visualizer", []() {
        return VisualizationFactory::createVisualizer(VisualizationType::NETWORK);
    });

    // Example and demo functions
    m.def("run_neural_activity_demo", []() {
        auto viz = VisualizationFactory::createVisualizer(VisualizationType::2D);
        viz->setCanvasSize(800, 600);
        viz->start();
        return true;
    });

    m.def("run_interactive_demo", []() {
        auto viz = VisualizationFactory::createVisualizer(VisualizationType::MULTIPLEXED);
        viz->setCanvasSize(1024, 768);
        viz->setInteractive(true);
        viz->start();
        return true;
    });

    // Utility for getting system capabilities
    m.def("get_visualization_capabilities", []() {
        return VisualizationFactory::getVisualizationCapabilities();
    });

    // Version and info
    m.def("get_version", []() {
        return std::string("NLM Visualization Tools - Phase 6 - Enhanced");
    });

    m.def("get_system_info", []() {
        py::dict info;
        info["version"] = "Phase 6 Enhanced";
        info["description"] = "Advanced visualization system for NLM neural simulation";
        info["features"] = py::list({"2D/3D neural activity", "Memory system visualization", 
                                   "Neuromodulation monitoring", "Development tracking",
                                   "Performance metrics", "Interactive multi-view"});
        info["data_types"] = py::list({"Neural Activity", "Memory State", 
                                     "Neuromodulation", "Development", "Performance"});
        return info;
    });
}

} // namespace nlm
