#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <queue>
#include <functional>

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "TrainingLoop.hpp" // For TrainingExperience

namespace nlm {

// Difficulty levels for curriculum learning
enum class DifficultyLevel {
    EASY,
    MEDIUM,
    HARD,
    EXPERT,
    NIGHTMARE
};

// Performance metrics for curriculum progression
struct LearningProgress {
    double successRate;
    double averageReward;
    double averageSteps;
    double trainingTimePerEpisode;
    size_t episodesCompleted;
    DifficultyLevel currentDifficulty;
    double performanceScore; // Normalized performance metric
    
    LearningProgress() : successRate(0.0), averageReward(0.0), averageSteps(0.0),
                         trainingTimePerEpisode(0.0), episodesCompleted(0),
                         currentDifficulty(DifficultyLevel::EASY), performanceScore(0.0) {}
};

// Environment adaptation rules
struct EnvironmentAdaptation {
    float difficultyMultiplier;
    float rewardScale;
    float timeStepSize;
    float energyDecayRate;
    std::vector<WorldObjectType> allowedObjects;
    std::vector<float> environmentParameters;
    
    EnvironmentAdaptation() : difficultyMultiplier(1.0f), rewardScale(1.0f),
                              timeStepSize(0.001f), energyDecayRate(0.01f),
                              difficultyMultiplier(1.0f) {
        allowedObjects = {WorldObjectType::Empty, WorldObjectType::Resource, WorldObjectType::Hazard};
    }
};

// ============= CURRICULUM LEARNER CLASS IMPLEMENTATION =============

class CurriculumLearner {
public:
    CurriculumLearner(size_t initialDifficulty = 0);
    
    // Core curriculum learning methods
    bool advanceCurriculum();
    bool resetProgress();
    DifficultyLevel getCurrentDifficulty() const;
    LearningProgress getProgress() const;
    
    // Environment adaptation
    bool adaptEnvironment(const std::shared_ptr<Brain>& brain,
                         const std::shared_ptr<AgentBrain>& agent,
                         const std::shared_ptr<SimpleWorld>& world,
                         const std::vector<float>& metrics);
    
    // Episode management
    bool runEpisodeWithDifficulty(std::shared_ptr<Brain> brain,
                                  std::shared_ptr<AgentBrain> agent,
                                  std::shared_ptr<SimpleWorld> world,
                                  size_t targetDifficulty);
    
    // Performance analysis
    double evaluatePerformance(const std::vector<float>& episodeResults) const;
    DifficultyLevel determineNextDifficulty(const std::vector<float>& difficultyMetrics) const;
    bool shouldIncreaseDifficulty(const std::vector<float>& metrics) const;
    bool shouldDecreaseDifficulty(const std::vector<float>& metrics) const;
    
    // Difficulty management
    void setDifficulty(size_t difficultyLevel);
    bool isDifficultyAchievable(size_t difficultyLevel) const;
    float getDifficultyProgress(size_t difficultyLevel) const;
    
    // Progress tracking
    void recordEpisodeResult(float reward, size_t steps, double duration,
                           DifficultyLevel difficulty);
    void updateProgress(const std::vector<float>& metrics);
    bool isCurriculumComplete() const;
    
    // Configuration and utilities
    void configureDifficultyLevel(size_t levelIndex, const EnvironmentAdaptation& adaptation);
    EnvironmentAdaptation getDifficultyAdaptation(size_t difficultyLevel) const;
    std::string getDifficultyName(size_t difficultyLevel) const;
    
    // Statistics and reporting
    std::string getProgressReport() const;
    std::vector<float> getDifficultyHistory() const;
    std::vector<float> getPerformanceHistory() const;
    
    // Callbacks
    CurriculumLearner& setProgressCallback(std::function<void(const LearningProgress&)> callback);
    CurriculumLearner& setDifficultyChangeCallback(std::function<void(DifficultyLevel, DifficultyLevel)> callback);
    CurriculumLearner& setAdaptationCallback(std::function<void(const EnvironmentAdaptation&)> callback);
    
    // Statistics
    size_t getTotalEpisodesCompleted() const { return totalEpisodesCompleted_; }
    double getOverallSuccessRate() const { return overallSuccessRate_; }
    double getAverageTrainingTime() const { return averageTrainingTime_; }
    
protected:
    // Difficulty level management
    bool isValidDifficultyLevel(size_t level) const;
    size_t getMaxDifficultyLevel() const { return difficultyLevels_.size() - 1; }
    
    // Performance calculation
    double calculatePerformanceScore(const std::vector<float>& metrics) const;
    double calculateSuccessRate(const std::vector<float>& results) const;
    
    // Adaptation logic
    EnvironmentAdaptation computeAdaptedEnvironment(const EnvironmentAdaptation& base,
                                                   const std::vector<float>& metrics) const;
    
    // Members
    std::vector<EnvironmentAdaptation> difficultyLevels_;
    LearningProgress currentProgress_;
    
    // Episode tracking
    std::vector<std::pair<DifficultyLevel, float>> episodeHistory_;
    std::vector<float> difficultyHistory_;
    std::vector<float> performanceHistory_;
    
    size_t totalEpisodesCompleted_;
    double overallSuccessRate_;
    double averageTrainingTime_;
    
    // Callbacks
    std::function<void(const LearningProgress&)> progressCallback_;
    std::function<void(DifficultyLevel, DifficultyLevel)> difficultyChangeCallback_;
    std::function<void(const EnvironmentAdaptation&)> adaptationCallback_;
    
    // State
    size_t currentDifficultyIndex_;
    bool curriculumCompleted_;
};

// ============= ADVANCED CURRICULUM LEARNER FACTORY =============

class CurriculumLearnerFactory {
public:
    static std::unique_ptr<CurriculumLearner> createStandardCurriculum(
        size_t numDifficultyLevels = 5,
        const std::string& curriculumType = "progressive");
    
    static std::unique_ptr<CurriculumLearner> createAdaptiveCurriculum(
        std::function<double(const std::vector<float>&)> performanceEvaluator,
        size_t minEpisodesPerLevel = 10);
    
    static std::unique_ptr<CurriculumLearner> createStructuredCurriculum(
        const std::vector<DifficultyLevel>& difficultyOrder,
        const std::vector<EnvironmentAdaptation>& adaptations);
    
    static std::vector<std::string> getAvailableCurriculumTypes();
    static EnvironmentAdaptation createDefaultAdaptation(DifficultyLevel level);
};

} // namespace nlm
