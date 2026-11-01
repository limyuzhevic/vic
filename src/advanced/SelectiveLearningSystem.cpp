#include "SelectiveLearningSystem.hpp"
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

SelectiveAttention::SelectiveAttention()
    : attentionDecayRate_(0.95f) {}

SelectiveAttention::~SelectiveAttention() = default;

void SelectiveAttention::addPattern(const std::string& patternName, const std::vector<float>& pattern, float priority) {
    patterns_[patternName] = std::make_pair(pattern, priority);
}

std::string SelectiveAttention::getRelevantPattern(const std::vector<float>& input) const {
    if (patterns_.empty()) return std::string();
    
    std::string bestPattern = std::string();
    float bestRelevance = 0.0f;
    
    for (const auto& pattern : patterns_) {
        float relevance = calculatePatternSimilarity(input, pattern.second.first);
        relevance *= pattern.second.second; // Apply priority
        
        if (relevance > bestRelevance) {
            bestRelevance = relevance;
            bestPattern = pattern.first;
        }
    }
    
    currentFocus_ = bestPattern;
    return bestPattern;
}

void SelectiveAttention::updatePatternRelevance(const std::string& patternName, float feedback) {
    auto it = patterns_.find(patternName);
    if (it != patterns_.end()) {
        // Update priority based on feedback
        it->second.second += feedback * (1.0f - it->second.second);
        
        // Decay other patterns
        for (auto& otherPattern : patterns_) {
            if (otherPattern.first != patternName) {
                otherPattern.second.second *= attentionDecayRate_;
            }
        }
    }
}

std::vector<std::pair<std::string, float>> SelectiveAttention::getPatternRelevance() const {
    std::vector<std::pair<std::string, float>> relevanceList;
    for (const auto& pattern : patterns_) {
        relevanceList.push_back(std::make_pair(pattern.first, pattern.second.second));
    }
    
    // Sort by relevance (descending)
    std::sort(relevanceList.begin(), relevanceList.end(),
              [](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b) {
                  return a.second > b.second;
              });
    
    return relevanceList;
}

void SelectiveAttention::clearPatterns() {
    patterns_.clear();
    currentFocus_ = std::string();
}

float SelectiveAttention::calculatePatternSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size() || pattern1.empty()) return 0.0f;
    
    float totalDifference = 0.0f;
    for (size_t i = 0; i < pattern1.size(); ++i) {
        totalDifference += std::abs(pattern1[i] - pattern2[i]);
    }
    
    return 1.0f - (totalDifference / static_cast<float>(pattern1.size()));
}

PatternBasedLearner::PatternBasedLearner()
    : learningRate_(0.1f), decayRate_(0.99f) {}

PatternBasedLearner::~PatternBasedLearner() = default;

void PatternBasedLearner::learnPattern(const std::vector<float>& pattern, const std::string& response, float strength) {
    patternResponses_[pattern][response] += strength;
}

std::string PatternBasedLearner::getBestResponse(const std::vector<float>& pattern) const {
    if (patternResponses_.empty()) return std::string();
    
    std::string bestResponse = std::string();
    float bestStrength = 0.0f;
    
    for (const auto& patternEntry : patternResponses_) {
        if (patternEntry.first.size() == pattern.size()) {
            float similarity = 0.0f;
            for (size_t i = 0; i < pattern.size(); ++i) {
                similarity += std::abs(pattern[i] - patternEntry.first[i]);
            }
            similarity = 1.0f - (similarity / static_cast<float>(pattern.size()));
            
            for (const auto& response : patternEntry.second) {
                float adjustedStrength = response.second * similarity;
                if (adjustedStrength > bestStrength) {
                    bestStrength = adjustedStrength;
                    bestResponse = response.first;
                }
            }
        }
    }
    
    return bestResponse;
}

std::map<std::vector<float>, std::string> PatternBasedLearner::getLearnedPatterns() const {
    std::map<std::vector<float>, std::string> learnedPatterns;
    for (const auto& patternEntry : patternResponses_) {
        std::string bestResponse = std::string();
        float bestStrength = 0.0f;
        
        for (const auto& response : patternEntry.second) {
            if (response.second > bestStrength) {
                bestStrength = response.second;
                bestResponse = response.first;
            }
        }
        
        if (!bestResponse.empty()) {
            learnedPatterns[patternEntry.first] = bestResponse;
        }
    }
    
    return learnedPatterns;
}

void PatternBasedLearner::updatePatternLearning(const std::vector<float>& pattern, float reward) {
    auto it = patternResponses_.find(pattern);
    if (it != patternResponses_.end()) {
        // Update based on reward
        float rewardFactor = std::min(std::max(reward, 0.0f), 2.0f); // Clamp to [0, 2]
        
        for (auto& response : it->second) {
            response.second *= rewardFactor;
        }
        
        // Apply decay
        for (auto& response : it->second) {
            response.second *= decayRate_;
        }
    }
}

PatternBasedLearner::PatternStats PatternBasedLearner::getPatternStats(const std::vector<float>& pattern) const {
    PatternStats stats;
    auto it = patternResponses_.find(pattern);
    if (it != patternResponses_.end()) {
        // Calculate accuracy (simplified)
        float totalStrength = 0.0f;
        float correctStrength = 0.0f;
        
        for (const auto& response : it->second) {
            totalStrength += response.second;
            // Assuming the response is "correct" if it matches the pattern name
            if (response.first == "correct") {
                correctStrength += response.second;
            }
        }
        
        stats.accuracy = totalStrength > 0.0f ? correctStrength / totalStrength : 0.0f;
        stats.sampleCount = it->second.size();
        
        // Calculate consistency (simplified)
        float strengthVariance = 0.0f;
        float strengthMean = totalStrength / static_cast<float>(it->second.size());
        
        for (const auto& response : it->second) {
            float diff = response.second - strengthMean;
            strengthVariance += diff * diff;
        }
        
        strengthVariance /= static_cast<float>(it->second.size());
        stats.consistency = 1.0f - std::sqrt(strengthVariance);
        
        // Calculate learning progress (simplified)
        stats.learningProgress = stats.accuracy * stats.consistency;
    }
    
    return stats;
}

AdvancedRewardModulation::AdvancedRewardModulation()
    : explorationRate_(0.1f) {}

AdvancedRewardModulation::~AdvancedRewardModulation() = default;

void AdvancedRewardModulation::addRewardRule(const std::string& ruleName, std::function<float(float, const std::vector<float>&)> rewardFunction) {
    rewardRules_[ruleName] = rewardFunction;
}

float AdvancedRewardModulation::calculateReward(float baseReward, const std::vector<float>& context) const {
    if (rewardRules_.empty()) return baseReward;
    
    float totalReward = baseReward;
    float explorationBonus = 0.0f;
    
    for (const auto& rule : rewardRules_) {
        float ruleReward = rule.second(baseReward, context);
        totalReward += ruleReward;
        
        // Add exploration bonus based on rule type
        if (rule.first == "exploration" || rule.first == "novelty") {
            explorationBonus += ruleReward;
        }
    }
    
    // Apply exploration rate
    totalReward = totalReward * (1.0f - explorationRate_) + explorationBonus * explorationRate_;
    
    return totalReward;
}

std::vector<std::string> AdvancedRewardModulation::getActiveRules() const {
    std::vector<std::string> activeRules;
    for (const auto& rule : rewardRules_) {
        activeRules.push_back(rule.first);
    }
    return activeRules;
}

void AdvancedRewardModulation::updateRewardRule(const std::string& ruleName, float error, const std::vector<float>& context) {
    auto it = rewardRules_.find(ruleName);
    if (it != rewardRules_.end()) {
        // Update rule statistics based on error
        RewardRuleStats& stats = ruleStats_[ruleName];
        stats.accuracy = 1.0f - std::min(error, 1.0f);
        
        // Calculate precision, recall, F1 score (simplified)
        stats.precision = stats.accuracy * 0.8f;
        stats.recall = stats.accuracy * 0.9f;
        
        if (stats.precision + stats.recall > 0.0f) {
            stats.f1Score = 2.0f * (stats.precision * stats.recall) / (stats.precision + stats.recall);
        } else {
            stats.f1Score = 0.0f;
        }
    }
}

AdvancedRewardModulation::RewardRuleStats AdvancedRewardModulation::getRuleStats(const std::string& ruleName) const {
    auto it = ruleStats_.find(ruleName);
    if (it != ruleStats_.end()) {
        return it->second;
    }
    
    return RewardRuleStats();
}

EnhancedDevelopmentalSystem::EnhancedDevelopmentalSystem()
    : plasticityDecayRate_(0.98f) {
    startTime_ = std::chrono::system_clock::now();
}

EnhancedDevelopmentalSystem::~EnhancedDevelopmentalSystem() = default;

void EnhancedDevelopmentalSystem::setDevelopmentalStage(DevelopmentalStage stage, const std::map<std::string, float>& parameters) {
    stageParameters_[stage] = parameters;
}

std::map<std::string, float> EnhancedDevelopmentalSystem::getDevelopmentalParameters() const {
    if (stageParameters_.empty()) return std::map<std::string, float>();
    
    // Return parameters for the current stage (simplified)
    auto it = stageParameters_.find(DevelopmentalStage::Adult);
    if (it != stageParameters_.end()) {
        return it->second;
    }
    
    return std::map<std::string, float>();
}

void EnhancedDevelopmentalSystem::updateDevelopment(const std::vector<float>& experience) {
    // Record developmental trajectory
    auto currentTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime_);
    
    DevelopmentalStage currentStage = DevelopmentalStage::Adult; // Simplified
    developmentalTrajectory_.push_back(std::make_pair(currentStage, duration.count()));
    
    // Update stage parameters based on experience
    for (auto& stageParams : stageParameters_) {
        for (auto& param : stageParams.second) {
            // Apply experience-based updates
            param.second += experience.size() > 0 ? experience[0] * 0.01f : 0.0f;
            param.second *= plasticityDecayRate_;
        }
    }
}

std::vector<std::pair<DevelopmentalStage, double>> EnhancedDevelopmentalSystem::getDevelopmentalTrajectory() const {
    return developmentalTrajectory_;
}

float EnhancedDevelopmentalSystem::calculateDevelopmentalProgress() const {
    if (developmentalTrajectory_.empty()) return 0.0f;
    
    // Calculate progress based on trajectory length and time
    double totalTime = 0.0;
    for (const auto& point : developmentalTrajectory_) {
        totalTime += point.second;
    }
    
    return static_cast<float>(developmentalTrajectory_.size()) / (1.0f + static_cast<float>(totalTime));
}

BehavioralConsistencyAnalyzer::BehavioralConsistencyAnalyzer()
    : consistencyThreshold_(0.8f) {}

BehavioralConsistencyAnalyzer::~BehavioralConsistencyAnalyzer() = default;

void BehavioralConsistencyAnalyzer::recordBehavior(const std::string& behavior, const std::vector<float>& context, float outcome) {
    behaviorRecordings_[behavior].push_back(std::make_pair(context, outcome));
    behavioralContexts_[behavior] = context;
}

float BehavioralConsistencyAnalyzer::calculateConsistency(const std::string& behavior) const {
    auto it = behaviorRecordings_.find(behavior);
    if (it == behaviorRecordings_.end() || it->second.empty()) return 0.0f;
    
    // Calculate consistency based on outcome variance
    float meanOutcome = 0.0f;
    for (const auto& recording : it->second) {
        meanOutcome += recording.second;
    }
    meanOutcome /= static_cast<float>(it->second.size());
    
    float variance = 0.0f;
    for (const auto& recording : it->second) {
        float diff = recording.second - meanOutcome;
        variance += diff * diff;
    }
    
    variance /= static_cast<float>(it->second.size());
    
    return 1.0f - std::sqrt(variance);
}

std::map<std::string, std::vector<std::vector<float>>> BehavioralConsistencyAnalyzer::getBehavioralPatterns() const {
    std::map<std::string, std::vector<std::vector<float>>> patterns;
    for (const auto& behavior : behaviorRecordings_) {
        for (const auto& recording : behavior.second) {
            patterns[behavior.first].push_back(recording.first);
        }
    }
    
    return patterns;
}

std::vector<std::string> BehavioralConsistencyAnalyzer::detectBehaviorDrifts() const {
    std::vector<std::string> drifts;
    for (const auto& behavior : behaviorRecordings_) {
        float consistency = calculateConsistency(behavior.first);
        if (consistency < consistencyThreshold_) {
            drifts.push_back(behavior.first);
        }
    }
    
    return drifts;
}

BehavioralConsistencyAnalyzer::ConsistencyStats BehavioralConsistencyAnalyzer::getConsistencyStats() const {
    ConsistencyStats stats;
    if (behaviorRecordings_.empty()) return stats;
    
    float totalConsistency = 0.0f;
    float totalWithinBehavior = 0.0f;
    float totalAcrossContext = 0.0f;
    float totalTemporal = 0.0f;
    
    for (const auto& behavior : behaviorRecordings_) {
        float consistency = calculateConsistency(behavior.first);
        totalConsistency += consistency;
        
        // Calculate within-behavior consistency
        float withinConsistency = 0.0f;
        if (behavior.second.size() >= 2) {
            for (size_t i = 0; i < behavior.second.size() - 1; ++i) {
                float similarity = 0.0f;
                for (size_t j = 0; j < behavior.second[i].first.size() && j < behavior.second[i+1].first.size(); ++j) {
                    similarity += std::abs(behavior.second[i].first[j] - behavior.second[i+1].first[j]);
                }
                similarity = 1.0f - (similarity / static_cast<float>(behavior.second[i].first.size()));
                withinConsistency += similarity;
            }
            withinConsistency /= static_cast<float>(behavior.second.size() - 1);
        }
        totalWithinBehavior += withinConsistency;
        
        // Calculate across-context consistency
        float acrossConsistency = 0.0f;
        if (behavior.second.size() >= 2) {
            for (size_t i = 1; i < behavior.second.size(); ++i) {
                float outcomeDiff = std::abs(behavior.second[i].second - behavior.second[i-1].second);
                acrossConsistency += 1.0f - outcomeDiff;
            }
            acrossConsistency /= static_cast<float>(behavior.second.size() - 1);
        }
        totalAcrossContext += acrossConsistency;
        
        // Calculate temporal consistency (simplified)
        totalTemporal += consistency;
    }
    
    size_t behaviorCount = behaviorRecordings_.size();
    stats.overallConsistency = totalConsistency / static_cast<float>(behaviorCount);
    stats.withinBehaviorConsistency = totalWithinBehavior / static_cast<float>(behaviorCount);
    stats.acrossContextConsistency = totalAcrossContext / static_cast<float>(behaviorCount);
    stats.temporalConsistency = totalTemporal / static_cast<float>(behaviorCount);
    
    return stats;
}

AdvancedLearningAnalytics::AdvancedLearningAnalytics()
    : learningThreshold_(0.01f) {}

AdvancedLearningAnalytics::~AdvancedLearningAnalytics() = default;

void AdvancedLearningAnalytics::recordLearningEvent(const std::string& eventType, float value, const std::vector<float>& context) {
    learningEvents_.push_back(std::make_pair(std::chrono::system_clock::now(), value));
    
    // Update metrics
    learningMetrics_.push_back(std::make_pair(eventType, value));
}

float AdvancedLearningAnalytics::calculateLearningEfficiency() const {
    if (learningEvents_.empty()) return 0.0f;
    
    // Calculate learning efficiency as the rate of learning improvement
    float totalLearning = 0.0f;
    float timeElapsed = 0.0f;
    
    if (learningEvents_.size() >= 2) {
        auto firstTime = learningEvents_.front().first;
        auto lastTime = learningEvents_.back().first;
        timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(lastTime - firstTime).count();
        
        if (timeElapsed > 0.0) {
            float firstValue = learningEvents_.front().second;
            float lastValue = learningEvents_.back().second;
            totalLearning = lastValue - firstValue;
        }
    }
    
    return timeElapsed > 0.0f ? totalLearning / timeElapsed : 0.0f;
}

std::vector<std::pair<double, float>> AdvancedLearningAnalytics::getLearningTrends() const {
    // Simple trend analysis (last 10% of events)
    if (learningEvents_.empty()) return std::vector<std::pair<double, float>>();
    
    size_t startIndex = std::max(static_cast<size_t>(0), learningEvents_.size() / 10);
    std::vector<std::pair<double, float>> trends;
    
    for (size_t i = startIndex; i < learningEvents_.size(); ++i) {
        auto timePoint = learningEvents_[i].first;
        float value = learningEvents_[i].second;
        auto timeSinceStart = std::chrono::duration_cast<std::chrono::seconds>(timePoint - learningEvents_.front().first).count();
        trends.push_back(std::make_pair(timeSinceStart, value));
    }
    
    return trends;
}

std::vector<double> AdvancedLearningAnalytics::detectLearningPlateaus() const {
    std::vector<double> plateaus;
    if (learningEvents_.size() < 10) return plateaus;
    
    // Simple plateau detection based on small changes in learning over time
    float windowSize = static_cast<float>(learningEvents_.size()) / 10.0f;
    float minChange = learningThreshold_;
    
    for (size_t i = 0; i < learningEvents_.size() - static_cast<size_t>(windowSize); ++i) {
        size_t windowEnd = std::min(static_cast<size_t>(i + windowSize), learningEvents_.size());
        
        float windowStartValue = learningEvents_[i].second;
        float windowEndValue = learningEvents_[windowEnd - 1].second;
        
        if (std::abs(windowEndValue - windowStartValue) < minChange) {
            double timePoint = std::chrono::duration_cast<std::chrono::seconds>(
                learningEvents_[windowEnd - 1].first - learningEvents_.front().first).count();
            plateaus.push_back(timePoint);
        }
    }
    
    return plateaus;
}

AdvancedLearningAnalytics::AdvancedAnalyticsMetrics AdvancedLearningAnalytics::getAdvancedMetrics() const {
    AdvancedAnalyticsMetrics metrics;
    
    if (learningMetrics_.empty()) return metrics;
    
    // Calculate various learning metrics
    float totalEvents = static_cast<float>(learningMetrics_.size());
    float successfulEvents = 0.0f;
    float novelEvents = 0.0f;
    
    for (const auto& event : learningMetrics_) {
        if (event.second > 0.0f) {
            successfulEvents += 1.0f;
        }
        
        if (event.first == "novel" || event.first == "exploration") {
            novelEvents += 1.0f;
        }
    }
    
    metrics.learningRate = successfulEvents / totalEvents;
    metrics.retentionRate = novelEvents / totalEvents;
    metrics.transferRate = metrics.learningRate * 0.8f; // Simplified
    metrics.generalizationAbility = metrics.learningRate * metrics.retentionRate;
    
    // Calculate exploration-exploitation balance
    float explorationEvents = 0.0f;
    float exploitationEvents = 0.0f;
    
    for (const auto& event : learningMetrics_) {
        if (event.first == "exploration" || event.first == "novel") {
            explorationEvents += 1.0f;
        } else {
            exploitationEvents += 1.0f;
        }
    }
    
    metrics.explorationExploitationBalance = 
        (explorationEvents + 1.0f) / (exploitationEvents + 1.0f); // Avoid division by zero
    
    return metrics;
}

SelectiveLearningSystem::SelectiveLearningSystem()
    : explorationRate_(0.1f), learningThreshold_(0.01f) {
    enabledFeatures_["attention"] = true;
    enabledFeatures_["pattern_learning"] = true;
    enabledFeatures_["reward_modulation"] = true;
    enabledFeatures_["development"] = true;
    enabledFeatures_["consistency"] = true;
    enabledFeatures_["analytics"] = true;
}

SelectiveLearningSystem::~SelectiveLearningSystem() = default;

bool SelectiveLearningSystem::initialize() {
    try {
        // Initialize subsystems
        attentionSystem_.clearPatterns();
        patternLearner_.reset();
        rewardModulator_.clearRules();
        developmentalSystem_.reset();
        consistencyAnalyzer_.reset();
        learningAnalytics_.reset();
        
        return true;
    } catch (...) {
        return false;
    }
}

void SelectiveLearningSystem::processExperience(const std::vector<float>& sensoryInput,
                                               const std::string& action,
                                               float reward,
                                               const std::vector<float>& context) {
    // Extract features from experience
    std::vector<float> features = extractContextualFeatures(sensoryInput, action, reward);
    
    // Update attention system
    if (enabledFeatures_["attention"]) {
        std::string relevantPattern = attentionSystem_.getRelevantPattern(features);
        attentionSystem_.updatePatternRelevance(relevantPattern, reward);
    }
    
    // Learn pattern associations
    if (enabledFeatures_["pattern_learning"]) {
        patternLearner_.learnPattern(features, action, reward);
        patternLearner_.updatePatternLearning(features, reward);
    }
    
    // Apply reward modulation
    if (enabledFeatures_["reward_modulation"]) {
        float modulatedReward = rewardModulator_.calculateReward(reward, context);
        rewardModulator_.updateRewardRule("main", reward - modulatedReward, context);
    }
    
    // Update development
    if (enabledFeatures_["development"]) {
        std::vector<float> developmentalExperience = {reward, 0.0f, 0.0f, 0.0f, 0.0f}; // Simplified
        developmentalSystem_.updateDevelopment(developmentalExperience);
    }
    
    // Record behavioral consistency
    if (enabledFeatures_["consistency"]) {
        consistencyAnalyzer_.recordBehavior(action, features, reward);
    }
    
    // Record learning analytics
    if (enabledFeatures_["analytics"]) {
        learningAnalytics_.recordLearningEvent("experience", reward, features);
    }
}

std::string SelectiveLearningSystem::getBestAction(const std::vector<float>& context) const {
    // Get the best response based on learned patterns
    std::string bestAction = patternLearner_.getBestResponse(context);
    
    if (bestAction.empty()) {
        // If no learned pattern, return a default action
        bestAction = "explore";
    }
    
    return bestAction;
}

SelectiveLearningSystem::SelectiveLearningStats SelectiveLearningSystem::getStats() const {
    SelectiveLearningStats stats;
    
    // Calculate pattern discrimination accuracy
    auto learnedPatterns = patternLearner_.getLearnedPatterns();
    if (!learnedPatterns.empty()) {
        float correctPredictions = 0.0f;
        for (const auto& pattern : learnedPatterns) {
            if (pattern.second == "correct") {
                correctPredictions += 1.0f;
            }
        }
        stats.patternDiscriminationAccuracy = correctPredictions / static_cast<float>(learnedPatterns.size());
    }
    
    // Calculate response selectivity
    auto patternStats = patternLearner_.getPatternStats(std::vector<float>()); // Simplified
    stats.responseSelectivity = patternStats.consistency;
    
    // Calculate learning progress
    stats.learningProgress = learningAnalytics_.calculateLearningEfficiency();
    
    // Calculate behavioral consistency
    auto consistencyStats = consistencyAnalyzer_.getConsistencyStats();
    stats.behavioralConsistency = consistencyStats.overallConsistency;
    
    // Calculate attention focus
    auto patternRelevance = attentionSystem_.getPatternRelevance();
    if (!patternRelevance.empty()) {
        stats.attentionFocus = patternRelevance.front().second;
    }
    
    return stats;
}

void SelectiveLearningSystem::enableFeature(const std::string& featureName, bool enabled) {
    enabledFeatures_[featureName] = enabled;
}

bool SelectiveLearningSystem::isFeatureEnabled(const std::string& featureName) const {
    auto it = enabledFeatures_.find(featureName);
    if (it != enabledFeatures_.end()) {
        return it->second;
    }
    return false;
}

std::map<std::string, bool> SelectiveLearningSystem::getFeatureStatus() const {
    return enabledFeatures_;
}

void SelectiveLearningSystem::reset() {
    // Reset all subsystems
    attentionSystem_.clearPatterns();
    patternLearner_.reset();
    rewardModulator_.clearRules();
    developmentalSystem_.reset();
    consistencyAnalyzer_.reset();
    learningAnalytics_.reset();
    
    // Reset feature status
    for (auto& feature : enabledFeatures_) {
        feature.second = true;
    }
}

std::vector<float> SelectiveLearningSystem::extractContextualFeatures(const std::vector<float>& sensoryInput,
                                                                     const std::string& action,
                                                                     float reward) const {
    std::vector<float> features;
    
    // Add sensory input features
    features.insert(features.end(), sensoryInput.begin(), sensoryInput.end());
    
    // Add action encoding
    float actionCode = 0.0f;
    if (action == "move_forward") actionCode = 1.0f;
    else if (action == "move_backward") actionCode = 2.0f;
    else if (action == "turn_left") actionCode = 3.0f;
    else if (action == "turn_right") actionCode = 4.0f;
    else if (action == "interact") actionCode = 5.0f;
    else if (action == "wait") actionCode = 6.0f;
    
    features.push_back(actionCode);
    
    // Add reward features
    features.push_back(reward);
    
    // Add normalized features (simplified)
    for (auto& feature : features) {
        feature = feature / 10.0f; // Normalize to [0, 1]
    }
    
    return features;
}

} // namespace nlm
