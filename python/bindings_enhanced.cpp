// Add specific error types to bindings.cpp for better error handling
namespace py = pybind11;
namespace nlm {

// Custom exception types for better error handling
class NeuralNetworkError : public std::runtime_error {
public:
    explicit NeuralNetworkError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class MemoryError : public std::runtime_error {
public:
    explicit MemoryError(const std::string& msg) : std::runtime_error(msg) {}
};

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper function for checking parameter ranges
template<typename T>
T clampParameter(const std::string& name, T value, T min, T max) {
    if (value < min) {
        throw ConfigError("Parameter \"" + name + "\" cannot be less than " + std::to_string(min) +
                         ", got " + std::to_string(value));
    }
    if (value > max) {
        throw ConfigError("Parameter \"" + name + "\" cannot be greater than " + std::to_string(max) +
                         ", got " + std::to_string(value));
    }
    return value;
}

// Helper function for getting population by type
std::vector<Neuron*> getPopulationByType(const Brain& brain, NeuronType type) {
    std::vector<Neuron*> neurons;
    for (const auto& region : brain.getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (Neuron* n : pop->getNeurons()) {
                    neurons.push_back(n);
                }
            }
        }
    }
    return neurons;
}

// Helper function for batch current injection
void injectCurrentToNeurons(Brain& brain, NeuronType type, float current) {
    for (const auto& region : brain.getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (Neuron* n : pop->getNeurons()) {
                    if (n) n->injectCurrent(current);
                }
            }
        }
    }
}

// Helper function for statistical analysis
struct BrainStatistics {
    size_t totalNeurons;
    size_t totalSynapses;
    size_t activeNeurons;
    size_t firingNeurons;
    float averageFiringRate;
    float excitationInhibitionRatio;
    float totalSpikeCount;
    double currentTime;
    std::string developmentalStage;
};

BrainStatistics computeBrainStatistics(const Brain& brain) {
    BrainStatistics stats;
    stats.totalNeurons = brain.getTotalNeuronCount();
    stats.totalSynapses = brain.getTotalSynapseCount();
    stats.activeNeurons = brain.getActiveNeuronCount();
    stats.firingNeurons = brain.getFiringNeuronCount();
    stats.averageFiringRate = brain.getAverageFiringRate();
    stats.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
    stats.totalSpikeCount = brain.getTotalSpikeCount();
    stats.currentTime = brain.getSimulationTime();
    stats.developmentalStage = brain.getDevelopmentalStage();
    return stats;
}

// Helper function for learning progress tracking
struct LearningProgress {
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    float neuromodulationLevel;
    float learnedPreference;
    std::string thoughtTrace;
    int stepSinceTurn;
    int repeatCount;
    int reflexBonusCount;
    int reflexSuppressCount;
    int wtaPulseCount;
};

LearningProgress getLearningProgress(const AgentBrain& agent) {
    LearningProgress progress;
    progress.curiosityLevel = agent.getCuriosityLevel();
    progress.noveltyLevel = agent.getNoveltyLevel();
    progress.predictionError = agent.getPredictionError();
    progress.neuromodulationLevel = agent.getNeuromodulationLevel();
    progress.learnedPreference = agent.getLearnedPreference();
    progress.thoughtTrace = agent.getThoughtTrace();
    // Note: need to add these methods to AgentBrain if not present
    // For now, set defaults
    progress.stepSinceTurn = 0;
    progress.repeatCount = 0;
    progress.reflexBonusCount = 0;
    progress.reflexSuppressCount = 0;
    progress.wtaPulseCount = 0;
    return progress;
}

} // namespace nlm