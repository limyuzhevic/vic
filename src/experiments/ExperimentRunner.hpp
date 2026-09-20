#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <optional>

namespace nlm {

// Forward declarations
class Brain;
class SimpleWorld;
class Simulation;
class EpisodeData;
class PerformanceMetrics;
class ErrorHandler;
class ConfigValidator;

// Advanced simulation modes
enum class SimulationMode {
    Standard = 0,
    Episode = 1,
    ReinforcementLearning = 2,
    Benchmark = 3
};

// Exploration strategies
enum class ExplorationStrategy {
    EpsilonGreedy = 0,
    Softmax = 1,
    UCT = 2,
    Noise = 3
};

// Metric aggregation methods
enum class MetricAggregation {
    Sum = 0,
    Mean = 1,
    Median = 2,
    Max = 3,
    Min = 4,
    Last = 5
};

// Validation rules
enum class ValidationRule {
    Range = 0,
    Positive = 1,
    NonNegative = 2,
    String = 3,
    Boolean = 4,
    Choice = 5
};

// Advanced experiment types
enum class ExperimentType {
    Exploration = 0,
    Learning = 1,
    Adaptation = 2,
    Benchmark = 3,
    Ablation = 4
};

// Metrics collection and analysis
class Metrics {
public:
    Metrics();
    ~Metrics();
    
    // Record a metric value
    void record(const std::string& name, double value);
    
    // Get metric values
    const std::vector<double>& getValues(const std::string& name) const;
    
    // Get statistics
    double getMean(const std::string& name) const;
    double getStdDev(const std::string& name) const;
    double getMin(const std::string& name) const;
    double getMax(const std::string& name) const;
    
    // Get all metric names
    std::vector<std::string> getMetricNames() const;
    
    // Clear all metrics
    void clear();
    
    // Summary report
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Experiment with enhanced features
class Experiment {
public:
    Experiment();
    explicit Experiment(const std::string& name);
    ~Experiment();
    
    // Experiment metadata
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Experiment type
    void setType(ExperimentType type);
    ExperimentType getType() const;
    
    // Random seed
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    
    // Configuration
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    
    // Metrics
    void recordMetric(const std::string& name, double value);
    void recordMetric(const std::string& name, float value);
    void recordMetric(const std::string& name, int value);
    
    // Get all metrics
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    
    // Get metric value
    double getMetric(const std::string& name) const;
    
    // Check if metric exists
    bool hasMetric(const std::string& name) const;
    
    // Recording step data
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                    size_t firingCount, float avgFiringRate);
    
    // Get step data
    struct StepData {
        SimulationStep step;
        size_t neuronCount;
        size_t synapseCount;
        size_t firingCount;
        float avgFiringRate;
    };
    const std::vector<StepData>& getStepData() const;
    
    // Duration
    SimulationStep getStartStep() const;
    void setStartStep(SimulationStep step);
    SimulationStep getEndStep() const;
    void setEndStep(SimulationStep step);
    
    // Notes
    void addNote(const std::string& note);
    const std::vector<std::string>& getNotes() const;
    
    // Export/import
    bool exportToJson(const std::string& filepath) const;
    bool importFromJson(const std::string& filepath);
    
    // Get experiment summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ExperimentRunner: Runs experiments and collects data
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Create new experiment
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    
    // Run single experiment
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<SimpleWorld> environment,
                      SimulationStep maxSteps);
    
    // Run batch of experiments
    std::vector<std::shared_ptr<Experiment>> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<SimpleWorld> environmentTemplate,
        SimulationStep maxSteps);
    
    // Get all experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    // Get experiment by name
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Set random seed for reproducibility
    void setGlobalSeed(uint64_t seed);
    
    // Save experiment results
    bool saveResults(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
