// Forward declarations
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

// Episode data structure for replay and history tracking
struct EpisodeData {
    size_t episodeId = 0;
    std::vector<float> rewards;
    std::vector<std::string> actions;
    std::vector<std::vector<float>> states;
    std::vector<double> timesteps;
    double totalReward = 0.0;
    size_t steps = 0;
    
    struct StepData {
        size_t step;
        size_t neuronCount;
        size_t synapseCount;
        size_t firingCount;
        float avgFiringRate;
    };
    std::vector<StepData> stepData_;
    
    std::string toJson() const {
        // Simple JSON representation
        std::string json = "{\"episodeId\":" + std::to_string(episodeId) +
                         "\"rewards\":" + std::to_string(rewards.size()) +
                         "\"totalReward\":" + std::to_string(totalReward) +
                         "\"steps\":" + std::to_string(steps) + "}";
        return json;
    }
};
