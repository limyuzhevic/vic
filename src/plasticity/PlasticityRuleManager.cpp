#pragma once

#include "PlasticityRuleManager.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <string>

namespace nlm {

// PlasticityRuleManager implementation

void PlasticityRuleManager::addRule(const std::string& name, std::shared_ptr<PlasticityRule> rule, int priority) {
    rules_[name] = rule;
    priority_[name] = priority;
    enabled_[name] = true;
    ruleStats_[name] = RuleStats{};
}

void PlasticityRuleManager::removeRule(const std::string& name) {
    rules_.erase(name);
    priority_.erase(name);
    enabled_.erase(name);
    ruleStats_.erase(name);
}

void PlasticityRuleManager::enableRule(const std::string& name, bool enabled) {
    if (enabled_.find(name) != enabled_.end()) {
        enabled_[name] = enabled;
    }
}

bool PlasticityRuleManager::isRuleEnabled(const std::string& name) const {
    auto it = enabled_.find(name);
    return it != enabled_.end() && it->second;
}

std::vector<std::string> PlasticityRuleManager::getEnabledRules() const {
    std::vector<std::string> enabledNames;
    for (const auto& pair : enabled_) {
        if (pair.second) {
            enabledNames.push_back(pair.first);
        }
    }
    // Sort by priority (descending)
    std::sort(enabledNames.begin(), enabledNames.end(),
              [this](const std::string& a, const std::string& b) {
                  return priority_.at(a) > priority_.at(b);
              });
    return enabledNames;
}

void PlasticityRuleManager::clearAllRules() {
    rules_.clear();
    priority_.clear();
    enabled_.clear();
    ruleStats_.clear();
}

void PlasticityRuleManager::setLearningRate(float rate) {
    globalLearningRate_ = std::max(0.0f, std::min(rate, 1.0f));
}

float PlasticityRuleManager::getLearningRate() const {
    return globalLearningRate_;
}

void PlasticityRuleManager::configureRule(const std::string& name, const std::map<std::string, float>& params) {
    if (rules_.find(name) != rules_.end()) {
        // Apply parameters to the rule (simplified - in real implementation,
        // different rules would handle different parameters)
        for (const auto& param : params) {
            // This would be implemented based on the specific rule type
            ruleStats_[name].parameters[param.first] = param.second;
        }
    }
}

std::map<std::string, float> PlasticityRuleManager::getRuleStats() const {
    return ruleStats_;
}

void PlasticityRuleManager::updateAllRules(Synapse* synapse,
                                          const std::vector<Timestamp>& preSpikes,
                                          const std::vector<Timestamp>& postSpikes,
                                          TimestepDuration dt) {
    if (!synapse) return;

    // Sort rules by priority (descending)
    std::vector<std::string> ruleNames;
    for (const auto& pair : rules_) {
        if (enabled_[pair.first]) {
            ruleNames.push_back(pair.first);
        }
    }

    std::sort(ruleNames.begin(), ruleNames.end(),
              [this](const std::string& a, const std::string& b) {
                  return priority_.at(a) > priority_.at(b);
              });

    // Apply each enabled rule
    for (const std::string& ruleName : ruleNames) {
        auto it = rules_.find(ruleName);
        if (it != rules_.end()) {
            auto& rule = it->second;

            try {
                // Record start time for performance tracking
                auto startTime = std::chrono::high_resolution_clock::now();

                // Update the rule
                rule->update(synapse, preSpikes, postSpikes, dt);

                // Record end time and update stats
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

                // Update rule statistics
                auto& stats = ruleStats_[ruleName];
                stats.updateCount++;
                stats.totalUpdateTime += duration.count();

                // Check if weight was actually changed
                float oldWeight = synapse->getWeight();
                rule->applyWeightChange(synapse, 0.0f);  // Test with zero change
                float newWeight = synapse->getWeight();
                synapse->setWeight(oldWeight);  // Restore original weight

                if (std::abs(newWeight - oldWeight) > 1e-6f) {
                    stats.successfulUpdates++;
                }

            } catch (const std::exception& e) {
                // Log error but continue with other rules
                std::cerr << "Error applying rule " << ruleName << ": " << e.what() << std::endl;
                auto& stats = ruleStats_[ruleName];
                stats.errorCount++;
            }
        }
    }
}

void PlasticityRuleManager::applyLearningSignal(float learningSignal) {
    // Scale global learning rate by learning signal
    float effectiveLearningRate = globalLearningRate_ * std::abs(learningSignal);

    // Apply learning signal to all rules (simplified - rules would typically
    // handle their own learning signal processing)
    for (auto& pair : rules_) {
        if (enabled_[pair.first]) {
            // Update the rule's learning rate based on the learning signal
            // This is a simplified approach - real implementation would be
            // more sophisticated and rule-specific
            auto& rule = pair.second;

            // Try to cast to a rule that accepts learning rate configuration
            // This is a dynamic cast example - in practice, you'd have
            // better abstraction for rule configuration
        }
    }
}

std::vector<std::pair<std::string, float>> PlasticityRuleManager::getTopPerformingRules(size_t count) const {
    std::vector<std::pair<std::string, float>> performanceList;

    for (const auto& pair : ruleStats_) {
        float efficiency = 0.0f;
        if (pair.second.updateCount > 0) {
            efficiency = static_cast<float>(pair.second.successfulUpdates) / pair.second.updateCount;
            // Add weight efficiency based on actual weight changes
            efficiency *= (1.0f - std::exp(-pair.second.averageWeightChange));
        }
        performanceList.emplace_back(pair.first, efficiency);
    }

    // Sort by efficiency (descending)
    std::sort(performanceList.begin(), performanceList.end(),
              [](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b) {
                  return a.second > b.second;
              });

    // Return top N rules
    if (count > performanceList.size()) {
        count = performanceList.size();
    }

    return std::vector<std::pair<std::string, float>>(performanceList.begin(), performanceList.begin() + count);
}

std::string PlasticityRuleManager::getRuleSummary() const {
    std::stringstream ss;

    ss << "Plasticity Rule Manager Summary:" << std::endl;
    ss << "  Total Rules: " << rules_.size() << std::endl;
    ss << "  Enabled Rules: " << std::count_if(enabled_.begin(), enabled_.end(),
                                              [](const auto& pair) { return pair.second; }) << std::endl;
    ss << "  Global Learning Rate: " << globalLearningRate_ << std::endl;

    ss << "\nRule Details:" << std::endl;
    for (const auto& pair : rules_) {
        const std::string& name = pair.first;
        const auto& rule = pair.second;
        const auto& stats = ruleStats_.at(name);

        ss << "  - " << name << ":" << std::endl;
        ss << "    Enabled: " << (enabled_.at(name) ? "Yes" : "No") << std::endl;
        ss << "    Priority: " << priority_.at(name) << std::endl;
        ss << "    Rule Name: " << rule->getName() << std::endl;
        ss << "    Updates Applied: " << stats.updateCount << std::endl;
        ss << "    Successful Updates: " << stats.successfulUpdates << std::endl;
        ss << "    Error Count: " << stats.errorCount << std::endl;
        if (stats.updateCount > 0) {
            ss << "    Average Update Time: " << (stats.totalUpdateTime / stats.updateCount) << " μs" << std::endl;
            ss << "    Efficiency: " << (static_cast<float>(stats.successfulUpdates) / stats.updateCount * 100.0f) << "%" << std::endl;
        }
    }

    return ss.str();
}

void PlasticityRuleManager::resetStatistics() {
    for (auto& pair : ruleStats_) {
        auto& stats = pair.second;
        stats.updateCount = 0;
        stats.successfulUpdates = 0;
        stats.errorCount = 0;
        stats.totalUpdateTime = 0;
        stats.averageWeightChange = 0.0f;
        stats.parameters.clear();
    }
}

// NeuralNetworkPlasticityManager implementation

NeuralNetworkPlasticityManager::NeuralNetworkPlasticityManager(std::shared_ptr<Brain> brain)
    : brain_(brain), currentLearningPhase_(LearningPhase::EXPLORATION), learningRate_(0.01f) {
    if (brain_) {
        // Try to get plasticity rules from the brain if available
        // In a real implementation, the brain would provide access to its plasticity system
    }
}

void NeuralNetworkPlasticityManager::createLearningPhase(const std::string& phaseName) {
    if (phaseName == "exploration") {
        currentLearningPhase_ = LearningPhase::EXPLORATION;
        // Set exploration-specific parameters
        explorationRate_ = 0.3f;
        curiosityThreshold_ = 0.5f;
        noveltyThreshold_ = 0.7f;

    } else if (phaseName == "exploitation") {
        currentLearningPhase_ = LearningPhase::EXPLOITATION;
        explorationRate_ = 0.05f;
        curiosityThreshold_ = 0.1f;
        noveltyThreshold_ = 0.2f;

    } else if (phaseName == "consolidation") {
        currentLearningPhase_ = LearningPhase::CONSOLIDATION;
        explorationRate_ = 0.1f;
        curiosityThreshold_ = 0.3f;
        noveltyThreshold_ = 0.4f;

    } else {
        std::cerr << "Unknown learning phase: " << phaseName << std::endl;
    }
}

void NeuralNetworkPlasticityManager::updatePlasticityBasedOnExperience(const BrainActivity& activity) {
    // Adjust plasticity based on current learning phase and activity
    float phaseFactor = 1.0f;

    switch (currentLearningPhase_) {
        case LearningPhase::EXPLORATION:
            phaseFactor = 1.5f;  // Higher plasticity during exploration
            break;
        case LearningPhase::EXPLOITATION:
            phaseFactor = 0.5f;  // Lower plasticity during exploitation
            break;
        case LearningPhase::CONSOLIDATION:
            phaseFactor = 0.8f;  // Moderate plasticity during consolidation
            break;
    }

    // Adjust learning rate based on phase and activity
    float activityLevel = activity.getAverageFiringRate();
    float activityFactor = std::min(1.0f, activityLevel / 0.1f);  // Normalized activity level

    float effectiveLearningRate = learningRate_ * phaseFactor * (1.0f + activityFactor * 0.5f);

    // Apply to all plasticity rules
    for (auto& pair : plasticityRules_) {
        if (enabledRules_.find(pair.first) != enabledRules_.end() && enabledRules_.at(pair.first)) {
            pair.second->setLearningRate(effectiveLearningRate);
        }
    }
}

void NeuralNetworkPlasticityManager::createPlasticityNetwork(const std::string& networkName,
                                                           const NeuralNetworkTopology& topology) {
    networkName_ = networkName;
    networkTopology_ = topology;

    // Create plasticity rules based on topology
    plasticityRules_.clear();
    enabledRules_.clear();

    if (topology.type == "layered") {
        // Create layered network plasticity
        for (size_t i = 0; i < topology.layers.size(); ++i) {
            std::string layerName = "layer_" + std::to_string(i);

            auto stdp = std::make_shared<STDP>();
            stdp->configure(0.01f, 0.012f, 20.0f);
            plasticityRules_[layerName] = stdp;
            enabledRules_[layerName] = true;

            if (i > 0) {  // Add Hebbian between layers
                std::string prevLayerName = "layer_" + std::to_string(i - 1);
                auto hebbian = std::make_shared<Hebbian>();
                hebbian->setLearningRate(0.005f);
                plasticityRules_[prevLayerName + "_to_" + layerName] = hebbian;
                enabledRules_[prevLayerName + "_to_" + layerName] = true;
            }
        }
    } else if (topology.type == "modular") {
        // Create modular network plasticity
        for (const auto& module : topology.modules) {
            auto stdp = std::make_shared<STDP>();
            stdp->configure(0.008f, 0.01f, 25.0f);
            plasticityRules_[module] = stdp;
            enabledRules_[module] = true;
        }
    } else if (topology.type == "spiking") {
        // Create spiking network plasticity with specialized rules
        auto stdp = std::make_shared<STDP>();
        stdp->configure(0.02f, 0.018f, 15.0f);
        plasticityRules_["spiking_core"] = stdp;
        enabledRules_["spiking_core"] = true;

        auto hebbian = std::make_shared<Hebbian>();
        hebbian->setLearningRate(0.01f);
        plasticityRules_["spiking_associative"] = hebbian;
        enabledRules_["spiking_associative"] = true;
    }
}

void NeuralNetworkPlasticityManager::applyPlasticityToSynapse(SynapseId synapseId,
                                                            const std::vector<Timestamp>& preSpikes,
                                                            const std::vector<Timestamp>& postSpikes,
                                                            TimestepDuration dt) {
    if (!brain_) return;

    // Find the synapse in the brain (simplified - in reality this would
    // be more efficient with direct access)
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto& synapse : population->getSynapses()) {
                if (synapse->getId() == synapseId) {
                    // Apply all enabled plasticity rules
                    for (const auto& pair : plasticityRules_) {
                        if (enabledRules_.find(pair.first) != enabledRules_.end() &&
                            enabledRules_.at(pair.first)) {
                            pair.second->update(synapse.get(), preSpikes, postSpikes, dt);
                        }
                    }
                    return;
                }
            }
        }
    }
}

void NeuralNetworkPlasticityManager::trainPlasticityNetwork(const BrainTrainingData& trainingData) {
    // Train the plasticity rules using the provided training data
    size_t totalSteps = trainingData.getTotalSteps();
    size_t currentStep = 0;

    while (currentStep < totalSteps) {
        // Get training sample
        auto sample = trainingData.getSample(currentStep);

        // Apply plasticity based on the sample
        for (const auto& pair : plasticityRules_) {
            if (enabledRules_.find(pair.first) != enabledRules_.end() &&
                enabledRules_.at(pair.first)) {
                pair.second->update(sample.synapse.get(), sample.preSpikes,
                                   sample.postSpikes, sample.dt);
            }
        }

        currentStep++;
    }
}

std::map<std::string, float> NeuralNetworkPlasticityManager::getPlasticityStatistics() const {
    std::map<std::string, float> stats;

    for (const auto& pair : plasticityRules_) {
        if (enabledRules_.find(pair.first) != enabledRules_.end() &&
            enabledRules_.at(pair.first)) {

            auto stdpRule = std::dynamic_pointer_cast<STDP>(pair.second);
            auto hebbianRule = std::dynamic_pointer_cast<Hebbian>(pair.second);

            if (stdpRule) {
                stats[pair.first + "_LTP"] = stdpRule->getLTPWeight();
                stats[pair.first + "_LTD"] = stdpRule->getLTDWeight();
                stats[pair.first + "_tau"] = stdpRule->getTimeConstant();
            } else if (hebbianRule) {
                stats[pair.first + "_learning_rate"] = hebbianRule->getLearningRate();
            }
        }
    }

    return stats;
}

void NeuralNetworkPlasticityManager::exportPlasticityConfiguration(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    file << "Neural Network Plasticity Configuration" << std::endl;
    file << "=====================================" << std::endl << std::endl;

    file << "Network Name: " << networkName_ << std::endl;
    file << "Learning Phase: " << static_cast<int>(currentLearningPhase_) << std::endl;
    file << "Global Learning Rate: " << learningRate_ << std::endl;
    file << "Topology Type: " << networkTopology_.type << std::endl << std::endl;

    file << "Plasticity Rules:" << std::endl;
    for (const auto& pair : plasticityRules_) {
        if (enabledRules_.find(pair.first) != enabledRules_.end() &&
            enabledRules_.at(pair.first)) {

            file << "  " << pair.first << ": " << pair.second->getName() << std::endl;

            auto stdpRule = std::dynamic_pointer_cast<STDP>(pair.second);
            auto hebbianRule = std::dynamic_pointer_cast<Hebbian>(pair.second);

            if (stdpRule) {
                file << "    LTP Weight: " << stdpRule->getLTPWeight() << std::endl;
                file << "    LTD Weight: " << stdpRule->getLTDWeight() << std::endl;
                file << "    Time Constant: " << stdpRule->getTimeConstant() << std::endl;
            } else if (hebbianRule) {
                file << "    Learning Rate: " << hebbianRule->getLearningRate() << std::endl;
            }

            file << std::endl;
        }
    }

    file.close();
}

} // namespace nlm
