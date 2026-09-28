#include "CurriculumLearner.hpp"
#include "../core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {

// ============= CURRICULUM LEARNER IMPLEMENTATION =============

CurriculumLearner::CurriculumLearner(size_t initialDifficulty) {
    // Initialize with standard difficulty levels
    difficultyLevels_ = {
        // Easy
        EnvironmentAdaptation{1.0f, 1.0f, 0.001f, 0.01f, 
                             {WorldObjectType::Empty, WorldObjectType::Resource},
                             {100.0f, 1000.0f, 0.1f}},
        
        // Medium
        EnvironmentAdaptation{1.5f, 1.2f, 0.002f, 0.015f,
                             {WorldObjectType::Empty, WorldObjectType::Resource, WorldObjectType::Hazard},
                             {150.0f, 1500.0f, 0.15f}},
        
        // Hard
        EnvironmentAdaptation{2.0f, 1.5f, 0.003f, 0.02f,
                             {WorldObjectType::Empty, WorldObjectType::Resource, WorldObjectType::Hazard, WorldObjectType::Wall},
                             {200.0f, 2000.0f, 0.2f}},
        
        // Expert
        EnvironmentAdaptation{2.5f, 2.0f, 0.005f, 0.025f,
                             {WorldObjectType::Empty, WorldObjectType::Resource, WorldObjectType::Hazard, WorldObjectType::Wall, WorldObjectType::Marker},
                             {250.0f, 2500.0f, 0.3f}},
        
        // Nightmare
        EnvironmentAdaptation{3.0f, 3.0f, 0.01f, 0.05f,
                             {WorldObjectType::Hazard, WorldObjectType::Wall},
                             {300.0f, 3000.0f, 0.5f}}
    };
    
    currentDifficultyIndex_ = std::min(initialDifficulty, difficultyLevels_.size() - 1);
    totalEpisodesCompleted_ = 0;
    overallSuccessRate_ = 0.0;
    averageTrainingTime_ = 0.0;
    curriculumCompleted_ = false;
    
    currentProgress_.currentDifficulty = static_cast<DifficultyLevel>(currentDifficultyIndex_);
    currentProgress_.episodesCompleted = 0;
    currentProgress_.successRate = 0.0;
    currentProgress_.averageReward = 0.0;
    currentProgress_.averageSteps = 0.0;
    currentProgress_.trainingTimePerEpisode = 0.0;
    currentProgress_.performanceScore = 0.0;
    
    NLM_LOG_INFO("CurriculumLearner initialized with difficulty level: " + 
                getDifficultyName(currentDifficultyIndex_));
}

bool CurriculumLearner::advanceCurriculum() {
    if (currentDifficultyIndex_ < getMaxDifficultyLevel()) {
        size_t previousDifficulty = currentDifficultyIndex_;
        currentDifficultyIndex_++;
        currentProgress_.currentDifficulty = static_cast<DifficultyLevel>(currentDifficultyIndex_);
        
        NLM_LOG_INFO("Advanced curriculum from " + getDifficultyName(previousDifficulty) + 
                    " to " + getDifficultyName(currentDifficultyIndex_));
        
        // Call difficulty change callback
        if (difficultyChangeCallback_) {
            difficultyChangeCallback_(static_cast<DifficultyLevel>(previousDifficulty),
                                     currentProgress_.currentDifficulty);
        }
        
        return true;
    } else {
        curriculumCompleted_ = true;
        NLM_LOG_INFO("Curriculum completed - reached maximum difficulty level");
        return false; // Cannot advance further
    }
}

bool CurriculumLearner::resetProgress() {
    currentDifficultyIndex_ = 0;
    currentProgress_.currentDifficulty = DifficultyLevel::EASY;
    currentProgress_.episodesCompleted = 0;
    currentProgress_.successRate = 0.0;
    currentProgress_.averageReward = 0.0;
    currentProgress_.averageSteps = 0.0;
    currentProgress_.trainingTimePerEpisode = 0.0;
    currentProgress_.performanceScore = 0.0;
    
    episodeHistory_.clear();
    difficultyHistory_.clear();
    performanceHistory_.clear();
    
    totalEpisodesCompleted_ = 0;
    overallSuccessRate_ = 0.0;
    averageTrainingTime_ = 0.0;
    curriculumCompleted_ = false;
    
    NLM_LOG_INFO("Curriculum progress reset");
    
    // Call progress callback
    if (progressCallback_) {
        progressCallback_(currentProgress_);
    }
    
    return true;
}

DifficultyLevel CurriculumLearner::getCurrentDifficulty() const {
    return currentProgress_.currentDifficulty;
}

LearningProgress CurriculumLearner::getProgress() const {
    return currentProgress_;
}

bool CurriculumLearner::adaptEnvironment(const std::shared_ptr<Brain>& brain,
                                        const std::shared_ptr<AgentBrain>& agent,
                                        const std::shared_ptr<SimpleWorld>& world,
                                        const std::vector<float>& metrics) {
    if (!world) {
        return false;
    }
    
    EnvironmentAdaptation baseAdaptation = difficultyLevels_[currentDifficultyIndex_];
    EnvironmentAdaptation adapted = computeAdaptedEnvironment(baseAdaptation, metrics);
    
    // Apply environment adaptations
    world->setMaxEnergy(adapted.environmentParameters[0]);
    world->setEnergyDecayRate(adapted.energyDecayRate);
    
    // Add or remove objects based on adaptation
    // (This would require more complex world management in a real implementation)
    
    NLM_LOG_INFO("Adapted environment for difficulty level: " + 
                getDifficultyName(currentDifficultyIndex_) +
                " (difficulty multiplier: " + std::to_string(adapted.difficultyMultiplier) + ")");
    
    // Call adaptation callback
    if (adaptationCallback_) {
        adaptationCallback_(adapted);
    }
    
    return true;
}

bool CurriculumLearner::runEpisodeWithDifficulty(std::shared_ptr<Brain> brain,
                                                std::shared_ptr<AgentBrain> agent,
                                                std::shared_ptr<SimpleWorld> world,
                                                size_t targetDifficulty) {
    if (targetDifficulty >= difficultyLevels_.size()) {
        return false;
    }
    
    // Store original difficulty
    size_t originalDifficulty = currentDifficultyIndex_;
    
    try {
        // Set target difficulty
        currentDifficultyIndex_ = targetDifficulty;
        currentProgress_.currentDifficulty = static_cast<DifficultyLevel>(targetDifficulty);
        
        // Run episode (simplified - would use actual training loop)
        double startTime = getCurrentTime();
        float reward = 0.0f;
        size_t steps = 0;
        
        // Simulate episode run (placeholder)
        reward = simulateEpisodeReward(targetDifficulty);
        steps = calculateExpectedSteps(targetDifficulty);
        
        double duration = getCurrentTime() - startTime;
        
        // Record results
        recordEpisodeResult(reward, steps, duration, static_cast<DifficultyLevel>(targetDifficulty));
        
        return true;
        
    } catch (const std::exception& e) {
        // Restore original difficulty
        currentDifficultyIndex_ = originalDifficulty;
        currentProgress_.currentDifficulty = static_cast<DifficultyLevel>(originalDifficulty);
        
        NLM_LOG_ERROR("Error running episode with difficulty: " + std::string(e.what()));
        return false;
    }
}

double CurriculumLearner::evaluatePerformance(const std::vector<float>& episodeResults) const {
    if (episodeResults.empty()) {
        return 0.0;
    }
    
    // Calculate performance based on episode results
    double sum = std::accumulate(episodeResults.begin(), episodeResults.end(), 0.0);
    return sum / episodeResults.size();
}

DifficultyLevel CurriculumLearner::determineNextDifficulty(const std::vector<float>& difficultyMetrics) const {
    if (difficultyMetrics.empty()) {
        return static_cast<DifficultyLevel>(currentDifficultyIndex_);
    }
    
    double performance = evaluatePerformance(difficultyMetrics);
    
    // Simple progression logic
    if (performance > 0.8) {
        return static_cast<DifficultyLevel>(std::min(currentDifficultyIndex_ + 1, getMaxDifficultyLevel()));
    } else if (performance < 0.4) {
        return static_cast<DifficultyLevel>(std::max(currentDifficultyIndex_ - 1, 0));
    } else {
        return static_cast<DifficultyLevel>(currentDifficultyIndex_);
    }
}

bool CurriculumLearner::shouldIncreaseDifficulty(const std::vector<float>& metrics) const {
    return !metrics.empty() && evaluatePerformance(metrics) > 0.8;
}

bool CurriculumLearner::shouldDecreaseDifficulty(const std::vector<float>& metrics) const {
    return !metrics.empty() && evaluatePerformance(metrics) < 0.4;
}

void CurriculumLearner::setDifficulty(size_t level) {
    if (isValidDifficultyLevel(level)) {
        size_t previousDifficulty = currentDifficultyIndex_;
        currentDifficultyIndex_ = level;
        currentProgress_.currentDifficulty = static_cast<DifficultyLevel>(level);
        
        NLM_LOG_INFO("Set difficulty level to: " + getDifficultyName(level) +
                    " (was " + getDifficultyName(previousDifficulty) + ")");
    } else {
        NLM_LOG_WARN("Invalid difficulty level requested: " + std::to_string(level));
    }
}

bool CurriculumLearner::isDifficultyAchievable(size_t difficultyLevel) const {
    return isValidDifficultyLevel(difficultyLevel);
}

float CurriculumLearner::getDifficultyProgress(size_t difficultyLevel) const {
    if (!isValidDifficultyLevel(difficultyLevel)) {
        return 0.0f;
    }
    
    // Calculate progress based on episodes completed at this difficulty
    size_t episodesAtDifficulty = 0;
    for (const auto& history : episodeHistory_) {
        if (history.first == static_cast<DifficultyLevel>(difficultyLevel)) {
            episodesAtDifficulty++;
        }
    }
    
    return static_cast<float>(episodesAtDifficulty) / 10.0f; // Normalize to 0-1 range
}

void CurriculumLearner::recordEpisodeResult(float reward, size_t steps, double duration,
                                          DifficultyLevel difficulty) {
    // Add to episode history
    episodeHistory_.push_back({difficulty, reward});
    difficultyHistory_.push_back(static_cast<float>(static_cast<int>(difficulty)));
    
    // Calculate performance metrics
    size_t currentDifficultyIndex = static_cast<size_t>(difficulty);
    
    // Update progress
    currentProgress_.episodesCompleted++;
    totalEpisodesCompleted_++;
    
    // Update success rate (reward > 0)
    currentProgress_.successRate = 
        (currentProgress_.successRate * (currentProgress_.episodesCompleted - 1) + 
         (reward > 0.0f ? 1.0 : 0.0)) / currentProgress_.episodesCompleted;
    
    // Update average reward and steps
    currentProgress_.averageReward = 
        (currentProgress_.averageReward * (currentProgress_.episodesCompleted - 1) + reward) /
        currentProgress_.episodesCompleted;
    
    currentProgress_.averageSteps = 
        (currentProgress_.averageSteps * (currentProgress_.episodesCompleted - 1) + static_cast<double>(steps)) /
        currentProgress_.episodesCompleted;
    
    currentProgress_.trainingTimePerEpisode = 
        (currentProgress_.trainingTimePerEpisode * (currentProgress_.episodesCompleted - 1) + duration) /
        currentProgress_.episodesCompleted;
    
    // Update overall success rate
    size_t successfulEpisodes = 0;
    for (const auto& history : episodeHistory_) {
        if (history.second > 0.0f) {
            successfulEpisodes++;
        }
    }
    
    overallSuccessRate_ = static_cast<double>(successfulEpisodes) / episodeHistory_.size();
    
    // Update average training time
    averageTrainingTime_ = 
        (averageTrainingTime_ * (totalEpisodesCompleted_ - 1) + duration) / totalEpisodesCompleted_;
    
    // Update performance score
    currentProgress_.performanceScore = calculatePerformanceScore({reward, static_cast<float>(steps), 
                                                                   static_cast<float>(duration)});
    
    // Check for curriculum completion
    if (currentDifficultyIndex_ >= getMaxDifficultyLevel() && 
        currentProgress_.episodesCompleted >= 50) { // Minimum episodes per difficulty
        curriculumCompleted_ = true;
    }
    
    // Call progress callback
    if (progressCallback_) {
        progressCallback_(currentProgress_);
    }
    
    NLM_LOG_INFO("Recorded episode result - difficulty: " + getDifficultyName(static_cast<int>(difficulty)) +
                ", reward: " + std::to_string(reward) + ", steps: " + std::to_string(steps) +
                ", duration: " + std::to_string(duration) + "s");
}

void CurriculumLearner::updateProgress(const std::vector<float>& metrics) {
    if (metrics.empty()) {
        return;
    }
    
    // Update progress based on metrics
    double performance = evaluatePerformance(metrics);
    currentProgress_.performanceScore = performance;
    
    // Adapt difficulty if needed
    DifficultyLevel nextDifficulty = determineNextDifficulty(metrics);
    
    if (nextDifficulty != currentProgress_.currentDifficulty) {
        size_t nextIndex = static_cast<size_t>(nextDifficulty);
        size_t previousIndex = currentDifficultyIndex_;
        
        currentDifficultyIndex_ = nextIndex;
        currentProgress_.currentDifficulty = nextDifficulty;
        
        NLM_LOG_INFO("Adapted difficulty based on performance: " + 
                    getDifficultyName(previousIndex) + " -> " + getDifficultyName(nextIndex) +
                    " (performance: " + std::to_string(performance) + ")");
    }
}

bool CurriculumLearner::isCurriculumComplete() const {
    return curriculumCompleted_;
}

void CurriculumLearner::configureDifficultyLevel(size_t levelIndex, const EnvironmentAdaptation& adaptation) {
    if (isValidDifficultyLevel(levelIndex)) {
        difficultyLevels_[levelIndex] = adaptation;
        NLM_LOG_INFO("Configured difficulty level " + std::to_string(levelIndex));
    } else {
        NLM_LOG_WARN("Attempted to configure invalid difficulty level: " + std::to_string(levelIndex));
    }
}

EnvironmentAdaptation CurriculumLearner::getDifficultyAdaptation(size_t difficultyLevel) const {
    if (isValidDifficultyLevel(difficultyLevel)) {
        return difficultyLevels_[difficultyLevel];
    }
    return EnvironmentAdaptation(); // Return default
}

std::string CurriculumLearner::getDifficultyName(size_t difficultyLevel) const {
    const std::vector<std::string> difficultyNames = {
        "Easy", "Medium", "Hard", "Expert", "Nightmare"
    };
    
    if (difficultyLevel < difficultyNames.size()) {
        return difficultyNames[difficultyLevel];
    }
    return "Unknown";
}

std::string CurriculumLearner::getProgressReport() const {
    std::stringstream ss;
    ss << "=== Curriculum Learning Progress Report ===\n";
    ss << "Current Difficulty: " << getDifficultyName(currentDifficultyIndex_) << "\n";
    ss << "Total Episodes Completed: " << totalEpisodesCompleted_ << "\n";
    ss << "Current Level Episodes: " << currentProgress_.episodesCompleted << "\n";
    ss << "Overall Success Rate: " << overallSuccessRate_ * 100 << "%\n";
    ss << "Average Reward: " << currentProgress_.averageReward << "\n";
    ss << "Average Steps: " << currentProgress_.averageSteps << "\n";
    ss << "Average Training Time: " << averageTrainingTime_ << "s\n";
    ss << "Current Performance Score: " << currentProgress_.performanceScore << "\n";
    ss << "Curriculum Complete: " << (curriculumCompleted_ ? "Yes" : "No") << "\n";
    ss << "\nDifficulty Level Progress:\n";
    
    for (size_t i = 0; i < difficultyLevels_.size(); ++i) {
        ss << "  Level " << i << " (" << getDifficultyName(i) << "): "
           << getDifficultyProgress(i) * 100 << "% complete\n";
    }
    
    ss << "==========================================\n";
    
    return ss.str();
}

std::vector<float> CurriculumLearner::getDifficultyHistory() const {
    return difficultyHistory_;
}

std::vector<float> CurriculumLearner::getPerformanceHistory() const {
    return performanceHistory_;
}

bool CurriculumLearner::isValidDifficultyLevel(size_t level) const {
    return level < difficultyLevels_.size();
}

double CurriculumLearner::calculatePerformanceScore(const std::vector<float>& metrics) const {
    if (metrics.size() < 3) {
        return 0.0;
    }
    
    float reward = metrics[0];
    float steps = metrics[1];
    float duration = metrics[2];
    
    // Normalize metrics to 0-1 range
    float normalizedReward = std::min(1.0f, reward / 100.0f); // Assume max reward of 100
    float normalizedSteps = std::min(1.0f, steps / 1000.0f); // Assume max steps of 1000
    float normalizedDuration = std::min(1.0f, 1.0f / std::max(0.001f, duration)); // Inverse of duration
    
    // Weighted combination
    return (normalizedReward * 0.4f + normalizedSteps * 0.3f + normalizedDuration * 0.3f);
}

double CurriculumLearner::calculateSuccessRate(const std::vector<float>& results) const {
    if (results.empty()) {
        return 0.0;
    }
    
    size_t successful = 0;
    for (float result : results) {
        if (result > 0.0f) {
            successful++;
        }
    }
    
    return static_cast<double>(successful) / results.size();
}

EnvironmentAdaptation CurriculumLearner::computeAdaptedEnvironment(const EnvironmentAdaptation& base,
                                                                   const std::vector<float>& metrics) const {
    EnvironmentAdaptation adapted = base;
    
    if (metrics.empty()) {
        return adapted;
    }
    
    double performance = evaluatePerformance(metrics);
    
    // Adapt based on performance
    if (performance < 0.4) {
        // Reduce difficulty
        adapted.difficultyMultiplier *= 0.8f;
        adapted.rewardScale *= 1.2f; // Increase reward scale
    } else if (performance > 0.8) {
        // Increase difficulty
        adapted.difficultyMultiplier *= 1.2f;
        adapted.rewardScale *= 0.9f; // Decrease reward scale
    }
    
    return adapted;
}

std::string CurriculumLearner::getCurrentTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    
    std::stringstream ss;
    ss << seconds << "." << std::setfill('0') << std::setw(3) << milliseconds;
    return ss.str();
}

float CurriculumLearner::simulateEpisodeReward(size_t difficultyLevel) const {
    // Simulate episode reward based on difficulty
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    // Lower difficulty gets higher reward
    float baseReward = 1.0f - (static_cast<float>(difficultyLevel) * 0.15f);
    
    // Add stochasticity
    return baseReward + dist(rng_) * 0.2f;
}

size_t CurriculumLearner::calculateExpectedSteps(size_t difficultyLevel) const {
    // Simulate expected steps based on difficulty
    std::uniform_real_distribution<size_t> dist(100, 1000);
    
    // Lower difficulty gets more steps
    return dist(rng_) + difficultyLevel * 50;
}

// Implementation of factory methods
std::unique_ptr<CurriculumLearner> CurriculumLearnerFactory::createStandardCurriculum(
    size_t numDifficultyLevels, const std::string& curriculumType) {
    
    std::unique_ptr<CurriculumLearner> learner;
    
    if (curriculumType == "progressive") {
        learner = std::make_unique<CurriculumLearner>(0);
    } else if (curriculumType == "adaptive") {
        // Would need performance evaluator
        learner = std::make_unique<CurriculumLearner>(0);
    } else if (curriculumType == "structured") {
        learner = std::make_unique<CurriculumLearner>(0);
    } else {
        // Default to progressive
        learner = std::make_unique<CurriculumLearner>(0);
    }
    
    return learner;
}

std::unique_ptr<CurriculumLearner> CurriculumLearnerFactory::createAdaptiveCurriculum(
    std::function<double(const std::vector<float>&)> performanceEvaluator,
    size_t minEpisodesPerLevel) {
    
    // This would need to be implemented with the performance evaluator
    return std::make_unique<CurriculumLearner>(0);
}

std::unique_ptr<CurriculumLearner> CurriculumLearnerFactory::createStructuredCurriculum(
    const std::vector<DifficultyLevel>& difficultyOrder,
    const std::vector<EnvironmentAdaptation>& adaptations) {
    
    std::unique_ptr<CurriculumLearner> learner = std::make_unique<CurriculumLearner>(0);
    
    // Configure the learner with the structured curriculum
    for (size_t i = 0; i < std::min(difficultyOrder.size(), adaptations.size()); ++i) {
        learner->configureDifficultyLevel(i, adaptations[i]);
    }
    
    return learner;
}

std::vector<std::string> CurriculumLearnerFactory::getAvailableCurriculumTypes() {
    return {"progressive", "adaptive", "structured"};
}

EnvironmentAdaptation CurriculumLearnerFactory::createDefaultAdaptation(DifficultyLevel level) {
    EnvironmentAdaptation adaptation;
    
    switch (level) {
        case DifficultyLevel::EASY:
            adaptation.difficultyMultiplier = 1.0f;
            adaptation.rewardScale = 1.0f;
            break;
        case DifficultyLevel::MEDIUM:
            adaptation.difficultyMultiplier = 1.5f;
            adaptation.rewardScale = 1.2f;
            break;
        case DifficultyLevel::HARD:
            adaptation.difficultyMultiplier = 2.0f;
            adaptation.rewardScale = 1.5f;
            break;
        case DifficultyLevel::EXPERT:
            adaptation.difficultyMultiplier = 2.5f;
            adaptation.rewardScale = 2.0f;
            break;
        case DifficultyLevel::NIGHTMARE:
            adaptation.difficultyMultiplier = 3.0f;
            adaptation.rewardScale = 3.0f;
            break;
    }
    
    return adaptation;
}

} // namespace nlm
