#pragma once

// Advanced neuroscience features for NLM
// Provides custom neuromodulator implementations, novelty and curiosity computation,
// prediction error analysis, and memory consolidation and replay

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <chrono>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

namespace nlm {
namespace python {

// Neuromodulator configuration
struct NeuromodulatorConfig {
    std::string name;
    std::string type; // "dopamine", "curiosity", "novelty", "prediction_error", "custom"
    double baselineLevel;
    double sensitivity;
    double gain;
    double decayRate;
    double timeConstant;
    std::map<std::string, std::variant<int, float, bool, std::string>> parameters;
    bool isEnabled;
    std::chrono::system_clock::time_point lastUpdate;
    double currentLevel;
    std::vector<double> recentActivity;
    std::map<std::string, double> effectWeights;
    std::string activationFunction;
    std::string decayFunction;
    std::function<double(double)> customActivation;
    std::function<double(double)> customDecay;
};

// Novelty and curiosity computation
struct NoveltyResult {
    double noveltyScore;
    double surpriseValue;
    double predictionError;
    std::vector<double> noveltyProfile;
    std::chrono::system_clock::time_point computedAt;
    std::string computationMethod;
    std::map<std::string, double> componentScores;
    bool isSignificant;
    double confidence;
};

struct CuriosityResult {
    double curiosityScore;
    double explorationDrive;
    double exploitationDrive;
    double intrinsicMotivation;
    std::vector<double> curiosityProfile;
    std::chrono::system_clock::time_point computedAt;
    std::string computationMethod;
    std::map<std::string, double> componentScores;
    bool isHighCuriosity;
    double confidence;
};

struct PredictionErrorResult {
    double predictionError;
    double predictionAccuracy;
    double predictionConfidence;
    std::vector<double> temporalErrorProfile;
    std::chrono::system_clock::time_point computedAt;
    std::string computationMethod;
    std::map<std::string, double> componentScores;
    bool isLargeError;
    double errorSignificance;
};

// Memory consolidation and replay
struct MemoryRecord {
    std::vector<float> memoryTrace;
    double importance;
    std::chrono::system_clock::time_point recordedAt;
    std::string memoryType; // "episodic", "semantic", "procedural"
    std::map<std::string, std::variant<int, float, bool, std::string>> metadata;
    bool isConsolidated;
    std::vector<std::vector<float>> replayTrajectory;
    double replayStrength;
};

struct ConsolidationResult {
    double consolidationScore;
    std::vector<size_t> consolidatedMemoryIndices;
    std::vector<size_t> replayedMemoryIndices;
    std::chrono::system_clock::time_point consolidationTime;
    double timeSpentConsolidating;
    bool success;
    std::string consolidationMethod;
    std::map<std::string, double> consolidationMetrics;
};

struct ReplayTrajectory {
    std::vector<std::vector<float>> states;
    std::vector<double> rewards;
    std::vector<int> actions;
    std::vector<double> predictions;
    std::chrono::system_clock::time_point startTime;
    double duration;
    double replayStrength;
    bool isActive;
    std::string replayType; // "forward", "backward", "simultaneous"
};

// Advanced neuroscience system
class AdvancedNeuroscience {
public:
    // Custom neuromodulator implementations
    static bool registerCustomNeuromodulator(const NeuromodulatorConfig& config);
    static bool registerCustomNeuromodulator(const std::string& name,
                                             const std::string& type,
                                             double baselineLevel,
                                             double sensitivity,
                                             const std::map<std::string, std::variant<int, float, bool, std::string>>& parameters = {});
    static bool unregisterNeuromodulator(const std::string& name);
    static NeuromodulatorConfig getNeuromodulatorConfig(const std::string& name);
    static std::vector<NeuromodulatorConfig> getAllNeuromodulatorConfigs();
    static bool enableNeuromodulator(const std::string& name, bool enable = true);
    static bool isNeuromodulatorEnabled(const std::string& name);
    static double computeNeuromodulationSignal(const std::string& name, double input);
    static double computeNeuromodulationSignal(const NeuromodulatorConfig& config, double input);
    static std::map<std::string, double> getAllNeuromodulatorLevels(std::shared_ptr<Brain> brain);
    static std::map<std::string, double> getNeuromodulatorDynamics(const std::string& name, 
                                                                 size_t historySize = 100);
    static bool setNeuromodulatorParameter(const std::string& name,
                                          const std::string& parameter,
                                          const std::variant<int, float, bool, std::string>& value);
    static std::map<std::string, std::variant<int, float, bool, std::string>> getNeuromodulatorParameters(const std::string& name);
    static void updateNeuromodulators(std::shared_ptr<Brain> brain, double timeStep);
    static std::string generateNeuromodulatorReport(std::shared_ptr<Brain> brain);
    
    // Novelty and curiosity computation
    static NoveltyResult computeNoveltyScore(std::shared_ptr<Brain> brain,
                                            const std::vector<float>& input,
                                            const std::string& method = "prediction_error");
    static CuriosityResult computeCuriosityScore(std::shared_ptr<Brain> brain,
                                                const std::vector<float>& state,
                                                const std::string& method = "information_gain");
    static double computeNovelty(const std::vector<float>& state1, const std::vector<float>& state2);
    static double computeSurprise(const std::vector<float>& prediction, const std::vector<float>& actual);
    static double computeInformationGain(const std::vector<float>& state, const std::vector<float>& action);
    static std::vector<double> computeNoveltyProfile(std::shared_ptr<Brain> brain,
                                                     size_t historySize = 1000,
                                                     const std::string& method = "prediction_error");
    static std::vector<double> computeCuriosityProfile(std::shared_ptr<Brain> brain,
                                                      size_t historySize = 1000,
                                                      const std::string& method = "information_gain");
    static void updateNoveltyLearning(std::shared_ptr<Brain> brain,
                                     const std::vector<float>& prediction,
                                     const std::vector<float>& actual);
    static void updateCuriosityLearning(std::shared_ptr<Brain> brain,
                                        const std::vector<float>& state,
                                        double rewardPredictionError);
    static bool isNoveltyDetectionEnabled(std::shared_ptr<Brain> brain);
    static bool isCuriosityEnabled(std::shared_ptr<Brain> brain);
    static void enableNoveltyDetection(std::shared_ptr<Brain> brain, bool enable = true);
    static void enableCuriosity(std::shared_ptr<Brain> brain, bool enable = true);
    static double getCurrentNoveltyLevel(std::shared_ptr<Brain> brain);
    static double getCurrentCuriosityLevel(std::shared_ptr<Brain> brain);
    static std::string generateNoveltyReport(NoveltyResult result);
    static std::string generateCuriosityReport(CuriosityResult result);
    
    // Prediction error analysis
    static PredictionErrorResult computePredictionError(std::shared_ptr<Brain> brain,
                                                        const std::vector<float>& prediction,
                                                        const std::vector<float>& actual,
                                                        const std::string& method = "absolute");
    static double computeTemporalDifferenceError(const std::vector<double>& rewards,
                                                 const std::vector<double>& values,
                                                 size_t timeIndex);
    static std::vector<double> computeTemporalDifferenceError(const std::vector<double>& rewards,
                                                             const std::vector<double>& values);
    static std::vector<double> computePredictionErrorTimeSeries(std::shared_ptr<Brain> brain,
                                                                size_t historySize = 1000);
    static std::vector<double> computePredictionAccuracyTimeSeries(std::shared_ptr<Brain> brain,
                                                                    size_t historySize = 1000);
    static std::map<std::string, double> analyzePredictionErrorPatterns(const std::vector<double>& errors);
    static double calculatePredictionErrorSignificance(const std::vector<double>& errors);
    static bool isPredictionErrorLearningEnabled(std::shared_ptr<Brain> brain);
    static void enablePredictionErrorLearning(std::shared_ptr<Brain> brain, bool enable = true);
    static double getCurrentPredictionError(std::shared_ptr<Brain> brain);
    static double getPredictionErrorConfidence(std::shared_ptr<Brain> brain);
    static std::string generatePredictionErrorReport(PredictionErrorResult result);
    
    // Memory consolidation and replay
    static bool recordMemory(std::shared_ptr<Brain> brain,
                            const std::vector<float>& memoryTrace,
                            double importance,
                            const std::string& memoryType = "episodic",
                            const std::map<std::string, std::variant<int, float, bool, std::string>>& metadata = {});
    static ConsolidationResult triggerMemoryConsolidation(std::shared_ptr<Brain> brain,
                                                          double consolidationThreshold = 0.8,
                                                          const std::string& consolidationMethod = "replay");
    static std::vector<MemoryRecord> getConsolidationCandidates(std::shared_ptr<Brain> brain,
                                                               size_t maxCandidates = 100);
    static ReplayTrajectory createReplayTrajectory(std::shared_ptr<Brain> brain,
                                                 const std::vector<size_t>& memoryIndices,
                                                 const std::string& replayType = "forward",
                                                 double replayStrength = 1.0);
    static bool startMemoryReplay(std::shared_ptr<Brain> brain, const ReplayTrajectory& trajectory);
    static bool stopMemoryReplay(std::shared_ptr<Brain> brain);
    static bool isMemoryReplayActive(std::shared_ptr<Brain> brain);
    static void replayMemories(std::shared_ptr<Brain> brain,
                              const std::vector<std::vector<float>>& memories,
                              double replayStrength = 1.0,
                              const std::string& replayType = "forward");
    static std::vector<std::vector<float>> getConsolidatedMemories(std::shared_ptr<Brain> brain,
                                                                  size_t maxMemories = 100);
    static bool clearConsolidatedMemories(std::shared_ptr<Brain> brain);
    static std::vector<ReplayTrajectory> getActiveReplayTrajectories(std::shared_ptr<Brain> brain);
    static bool removeReplayTrajectory(std::shared_ptr<Brain> brain, const std::string& trajectoryId);
    static size_t getMemoryRecordCount(std::shared_ptr<Brain> brain);
    static size_t getConsolidatedMemoryCount(std::shared_ptr<Brain> brain);
    static double getMemoryConsolidationProgress(std::shared_ptr<Brain> brain);
    static std::string generateMemoryReport(std::shared_ptr<Brain> brain);
    
    // Advanced neuroscience utilities
    static std::vector<float> normalizeVector(const std::vector<float>& data);
    static double calculateEuclideanDistance(const std::vector<float>& vec1, const std::vector<float>& vec2);
    static double calculateCosineSimilarity(const std::vector<float>& vec1, const std::vector<float>& vec2);
    static double calculatePearsonCorrelation(const std::vector<float>& vec1, const std::vector<float>& vec2);
    static double computeKLDivergence(const std::vector<float>& distribution1, const std::vector<float>& distribution2);
    static double computeMutualInformation(const std::vector<float>& var1, const std::vector<float>& var2);
    static std::vector<float> computeShannonEntropy(const std::vector<float>& distribution);
    static std::vector<float> computeMSE(const std::vector<float>& predictions, const std::vector<float>& targets);
    static std::vector<float> computeRMSE(const std::vector<float>& predictions, const std::vector<float>& targets);
    static std::vector<float> computeMAE(const std::vector<float>& predictions, const std::vector<float>& targets);
    static double calculateAnomalyScore(const std::vector<float>& data, double threshold = 2.0);
    static std::vector<size_t> detectAnomalies(const std::vector<float>& data, double threshold = 2.0);
    static std::vector<float> smoothData(const std::vector<float>& data, size_t windowSize = 5);
    static std::vector<float> filterData(const std::vector<float>& data, const std::vector<float>& kernel);
    static std::vector<float> applySlidingWindow(const std::vector<float>& data, size_t windowSize);
    
    // Neuroscience system configuration
    static void setNeuroscienceConfig(const std::map<std::string, std::variant<int, float, bool, std::string>>& config);
    static std::map<std::string, std::variant<int, float, bool, std::string>> getNeuroscienceConfig();
    static void resetNeuromodulators(std::shared_ptr<Brain> brain);
    static void resetMemorySystem(std::shared_ptr<Brain> brain);
    static void resetPredictionSystem(std::shared_ptr<Brain> brain);
    static void clearNeuroscienceData(std::shared_ptr<Brain> brain);
    static std::string generateNeuroscienceReport(std::shared_ptr<Brain> brain);
    
    // Neuroscience system control
    static void pauseNeuroscienceProcessing(std::shared_ptr<Brain> brain);
    static void resumeNeuroscienceProcessing(std::shared_ptr<Brain> brain);
    static bool isNeuroscienceProcessingPaused(std::shared_ptr<Brain> brain);
    static void setNeuroscienceProcessingInterval(double intervalSeconds);
    static double getNeuroscienceProcessingInterval();
    static size_t getCurrentProcessingQueueSize(std::shared_ptr<Brain> brain);
    static double getNeuroscienceProcessingLoad(std::shared_ptr<Brain> brain);
    
    // Singleton instance management
    static AdvancedNeuroscience& getInstance();
    
private:
    AdvancedNeuroscience() = default;
    ~AdvancedNeuroscience() = default;
    AdvancedNeuroscience(const AdvancedNeuroscience&) = delete;
    AdvancedNeuroscience& operator=(const AdvancedNeuroscience&) = delete;
    
    static std::map<std::string, NeuromodulatorConfig> registeredNeuromodulators_;
    static std::deque<MemoryRecord> memoryRecords_;
    static std::map<std::string, ReplayTrajectory> activeReplayTrajectories_;
    static std::mutex mutex_;
    static double processingInterval_;
    static bool processingPaused_;
};