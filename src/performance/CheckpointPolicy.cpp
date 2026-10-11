// CheckpointPolicy.cpp
#include "CheckpointPolicy.hpp"
#include <thread>
#include <future>
#include <shared_mutex>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {
    
    CheckpointPolicy::CheckpointPolicy()
        : rulesCreated_(0)
        , lastUpdateTime_(std::time(nullptr)) {}
    
    CheckpointPolicy::~CheckpointPolicy() {}
    
    void CheckpointPolicy::addRule(const CheckpointRule& rule) {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        rules_[rule.name] = rule;
        ++rulesCreated_;
        lastUpdateTime_ = std::time(nullptr);
        
        // Update statistics
        updateStatistics();
    }
    
    void CheckpointPolicy::removeRule(const std::string& ruleName) {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        auto it = rules_.find(ruleName);
        if (it != rules_.end()) {
            rules_.erase(it);
            lastUpdateTime_ = std::time(nullptr);
            updateStatistics();
        }
    }
    
    void CheckpointPolicy::updateRule(const CheckpointRule& rule) {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        rules_[rule.name] = rule;
        lastUpdateTime_ = std::time(nullptr);
        updateStatistics();
    }
    
    const CheckpointRule* CheckpointPolicy::getRule(const std::string& ruleName) const {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        auto it = rules_.find(ruleName);
        if (it != rules_.end()) {
            return &it->second;
        }
        
        return nullptr;
    }
    
    bool CheckpointPolicy::shouldCreateCheckpoint(
        uint64_t currentStep,
        double currentTime,
        uint64_t currentSize,
        const std::string& event,
        const std::unordered_map<std::string, uint64_t>& state) const {
        
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        // Get enabled rules
        std::vector<const CheckpointRule*> enabledRules;
        for (const auto& pair : rules_) {
            if (pair.second.enabled) {
                enabledRules.push_back(&pair.second);
            }
        }
        
        if (enabledRules.empty()) {
            return false;
        }
        
        // Evaluate each rule
        std::vector<double> scores;
        scores.reserve(enabledRules.size());
        
        for (const CheckpointRule* rule : enabledRules) {
            double score = evaluateRule(rule, currentStep, currentTime, currentSize, event, state);
            scores.push_back(score);
        }
        
        // Find maximum score
        double maxScore = 0.0;
        for (double score : scores) {
            if (score > maxScore) {
                maxScore = score;
            }
        }
        
        // Create checkpoint if score is high enough
        return maxScore >= 0.5; // Threshold for creating checkpoint
    }
    
    void CheckpointPolicy::recordCheckpoint(uint64_t step, double time, uint64_t size, const std::string& name) {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        schedule_.recordCheckpoint(step, time, size, name);
        lastUpdateTime_ = std::time(nullptr);
    }
    
    bool CheckpointPolicy::shouldCleanup(const std::vector<CheckpointMetadata>& checkpoints) const {
        return cleanupRule_.shouldCleanup(checkpoints);
    }
    
    std::vector<std::string> CheckpointPolicy::getRecommendations(
        uint64_t currentStep,
        double currentTime,
        uint64_t currentSize,
        const std::string& event,
        const std::unordered_map<std::string, uint64_t>& state) const {
        
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        std::vector<std::string> recommendations;
        
        // Evaluate each rule
        for (const auto& pair : rules_) {
            const CheckpointRule& rule = pair.second;
            
            if (!rule.enabled) {
                continue;
            }
            
            double score = evaluateRule(&rule, currentStep, currentTime, currentSize, event, state);
            
            // Generate recommendation if score is high
            if (score >= 0.7) {
                std::string recommendation = "rule:" + rule.name + "_score:" + 
                                             std::to_string(score) + "_action:create_checkpoint";
                recommendations.push_back(recommendation);
            }
        }
        
        return recommendations;
    }
    
    CheckpointPolicy::PolicyStatistics CheckpointPolicy::getStatistics() const {
        std::lock_guard<std::mutex> lock(statsMutex_);
        
        PolicyStatistics stats;
        
        stats.totalRules = rules_.size();
        stats.enabledRules = 0;
        stats.disabledRules = 0;
        
        for (const auto& pair : rules_) {
            if (pair.second.enabled) {
                ++stats.enabledRules;
            } else {
                ++stats.disabledRules;
            }
        }
        
        stats.checkpointsCreated = schedule_.totalCheckpoints;
        stats.averageSize = schedule_.totalCheckpoints > 0 ? 
            schedule_.totalSize / schedule_.totalCheckpoints : 0.0;
        stats.frequency = getCheckpointFrequency();
        stats.lastRuleUpdate = lastUpdateTime_;
        
        return stats;
    }
    
    bool CheckpointPolicy::exportRules(const std::string& filepath) const {
        return writeRulesToFile(filepath);
    }
    
    bool CheckpointPolicy::importRules(const std::string& filepath) {
        return readRulesFromFile(filepath);
    }
    
    uint64_t CheckpointPolicy::calculateAdaptiveWeight(
        const CheckpointRule& rule,
        uint64_t currentStep,
        double currentTime,
        uint64_t currentSize,
        const std::string& event,
        const std::unordered_map<std::string, uint64_t>& state) const {
        
        // Default implementation - can be overridden by derived classes
        return rule.weight;
    }
    
    double CheckpointPolicy::evaluateRulePriority(const CheckpointRule& rule) const {
        // Higher priority rules get higher scores
        switch (rule.priority) {
            case CheckpointPriority::Critical:
                return 1.0;
            case CheckpointPriority::High:
                return 0.8;
            case CheckpointPriority::Normal:
                return 0.6;
            case CheckpointPriority::Low:
                return 0.4;
            default:
                return 0.5;
        }
    }
    
    double CheckpointPolicy::evaluateRule(
        const CheckpointRule* rule,
        uint64_t currentStep,
        double currentTime,
        uint64_t currentSize,
        const std::string& event,
        const std::unordered_map<std::string, uint64_t>& state) const {
        
        // Evaluate rule based on strategy
        bool shouldCreate = rule->shouldCreateCheckpoint(
            currentStep, currentTime, currentSize, event, state);
        
        if (!shouldCreate) {
            return 0.0;
        }
        
        // Calculate score based on rule properties
        double baseScore = 0.5;
        
        // Add priority bonus
        baseScore += evaluateRulePriority(*rule) * 0.2;
        
        // Add frequency bonus
        if (rule->interval > 0) {
            double frequencyBonus = std::min(0.3, 1.0 / std::max(1.0, rule->interval / 10000.0));
            baseScore += frequencyBonus * 0.3;
        }
        
        // Add event bonus
        if (!rule->triggerEvents.empty()) {
            baseScore += 0.2;
        }
        
        return std::min(1.0, baseScore);
    }
    
    void CheckpointPolicy::updateStatistics() const {
        // Update statistics
        lastUpdateTime_ = std::time(nullptr);
    }
    
    bool CheckpointPolicy::writeRulesToFile(const std::string& filepath) const {
        try {
            std::ofstream file(filepath, std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Write statistics
            PolicyStatistics stats = getStatistics();
            file.write(reinterpret_cast<const char*>(&stats), sizeof(stats));
            
            // Write rules
            uint64_t ruleCount = rules_.size();
            file.write(reinterpret_cast<const char*>(&ruleCount), sizeof(ruleCount));
            
            for (const auto& pair : rules_) {
                const CheckpointRule& rule = pair.second;
                
                // Serialize rule (simplified)
                // Write rule name length and data
                uint64_t nameLength = rule.name.size();
                file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
                file.write(rule.name.c_str(), nameLength);
                
                // Write other fields as needed
            }
            
            return file.good();
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception writing rules to file: ") + e.what());
            return false;
        }
    }
    
    bool CheckpointPolicy::readRulesFromFile(std::string& filepath) {
        try {
            std::ifstream file(filepath, std::ios::binary);
            if (!file) {
                return false;
            }
            
            // Clear existing rules
            std::lock_guard<std::mutex> lock(statsMutex_);
            rules_.clear();
            
            // Read statistics
            PolicyStatistics stats;
            file.read(reinterpret_cast<char*>(&stats), sizeof(stats));
            
            // Read rule count
            uint64_t ruleCount;
            file.read(reinterpret_cast<char*>(&ruleCount), sizeof(ruleCount));
            
            // Read rules
            for (uint64_t i = 0; i < ruleCount; ++i) {
                // Deserialize rule (simplified)
                uint64_t nameLength;
                file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
                
                std::string name(nameLength, ' ');
                file.read(&name[0], nameLength);
                
                // Create rule
                CheckpointRule rule(name, CheckpointStrategy::None);
                
                // Deserialize other fields as needed
                
                // Add to rules
                rules_[name] = rule;
            }
            
            lastUpdateTime_ = std::time(nullptr);
            return file.good() && !file.bad();
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception reading rules from file: ") + e.what());
            return false;
        }
    }
    
} // namespace nlm