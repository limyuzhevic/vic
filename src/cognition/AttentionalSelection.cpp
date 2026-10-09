#include "AttentionalSelection.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>

namespace nlm {

struct AttentionalSelection::Impl {
    // Neural population activity levels
    std::vector<float> populationActivity;
    
    // Lateral inhibition connections between neurons
    std::vector<std::pair<NeuronId, NeuronId>> lateralInhibitions;
    
    // Recurrent connections for winner amplification
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections;
    
    // Attention dynamics state
    std::vector<float> attentionTrace;
    std::queue<NeuronId> attentionQueue;
    
    // Competition results
    std::vector<std::pair<NeuronId, float>> competitionResults;
    
    // Time constants for different processes
    float inhibitionTimeConstant;
    float excitationTimeConstant;
    float decayTimeConstant;
    
    Impl() : inhibitionTimeConstant(0.1f), excitationTimeConstant(0.2f), decayTimeConstant(0.05f) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f)
    , attentionDuration_(1.0f)
    , currentAttentionLevel_(0.0f)
    , attentionStability_(0.0f)
    , attentionSwitchTimer_(0.0f)
    , attentionStartStep_(0)
    , isSustainedAttention_(false)
    , integratedWithWorkingMemory_(false)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    
    // Initialize attentional weights based on brain structure
    if (brain_) {
        size_t totalNeurons = 0;
        for (const auto& region : brain_->getRegions()) {
            totalNeurons += region->getNeuronCount();
        }
        attentionalWeights_.resize(totalNeurons, 1.0f / totalNeurons);
        attentionalSalience_.resize(totalNeurons, 0.0f);
    }
    
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(
    const std::vector<NeuronId>& competitors,
    float globalInhibition,
    float dt) {
    winners_.clear();
    attentionalWeights_.clear();
    attentionalWeights_.resize(competitors.size(), 0.0f);
    pImpl->competitionResults.clear();
    
    if (competitors.empty()) {
        return winners_;
    }
    
    // Reset lateral inhibition state
    pImpl->lateralInhibitions.clear();
    
    // Create lateral inhibition connections between competitors
    for (size_t i = 0; i < competitors.size(); ++i) {
        for (size_t j = i + 1; j < competitors.size(); ++j) {
            pImpl->lateralInhibitions.emplace_back(competitors[i], competitors[j]);
            pImpl->lateralInhibitions.emplace_back(competitors[j], competitors[i]);
        }
    }
    
    // Run competitive dynamics
    runCompetitiveDynamics(competitors, globalInhibition, dt);
    
    // Apply lateral inhibition
    applyLateralInhibition(competitors, dt);
    
    // Amplify winners through recurrent connections
    amplifyWinners(winners_, dt);
    
    // Normalize attentional weights
    normalizeWeights(attentionalWeights_);
    
    // Update attentional salience
    for (size_t i = 0; i < competitors.size(); ++i) {
        attentionalSalience_[competitors[i].value] = attentionalWeights_[i];
    }
    
    return winners_;
}

void AttentionalSelection::runCompetitiveDynamics(
    const std::vector<NeuronId>& competitors,
    float globalInhibition,
    float dt) {
    
    // Compute initial neural activities based on current state
    pImpl->populationActivity.clear();
    pImpl->populationActivity.resize(competitors.size(), 0.0f);
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        float activity = computeNeuralActivity(competitors[i], competitors);
        
        // Apply top-down and bottom-up biases
        auto biasIt = topDownBias_.find(competitors[i].value);
        float topDown = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        auto salienceIt = bottomUpSalience_.find(competitors[i].value);
        float salience = (salienceIt != bottomUpSalience_.end()) ? salienceIt->second : 0.0f;
        
        activity = activity + topDown * 0.3f + salience * 0.5f;
        
        // Apply global inhibition
        activity = std::max(0.0f, activity - globalInhibition);
        
        pImpl->populationActivity[i] = activity;
    }
    
    // Competitive normalization (softmax-like)
    float maxActivity = *std::max_element(pImpl->populationActivity.begin(), 
                                          pImpl->populationActivity.end());
    
    if (maxActivity > 0.0f) {
        float sumExp = 0.0f;
        std::vector<float> expActivities;
        
        for (float activity : pImpl->populationActivity) {
            float expActivity = std::exp(activity * 10.0f); // Temperature parameter
            expActivities.push_back(expActivity);
            sumExp += expActivity;
        }
        
        if (sumExp > 0.0f) {
            for (size_t i = 0; i < competitors.size(); ++i) {
                attentionalWeights_[i] = expActivities[i] / sumExp;
            }
        }
    }
    
    // Select winners based on threshold
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (attentionalWeights_[i] >= competitionThreshold_) {
            winners_.push_back(competitors[i]);
            pImpl->competitionResults.emplace_back(competitors[i], attentionalWeights_[i]);
        }
    }
}

void AttentionalSelection::applyLateralInhibition(
    const std::vector<NeuronId>& competitors,
    float dt) {
    
    // Stronger lateral inhibition for non-winners
    float inhibitionStrength = inhibitionStrength_ * dt * 10.0f;
    
    for (const auto& conn : pImpl->lateralInhibitions) {
        // Check if source is a competitor
        bool isCompetitor = std::find(competitors.begin(), competitors.end(), conn.first) != competitors.end();
        bool isWinner = std::find(winners_.begin(), winners_.end(), conn.first) != winners_.end();
        
        if (isCompetitor && !isWinner) {
            // Apply strong inhibition to non-winners
            if (brain_) {
                brain_->injectCurrent(conn.first, -inhibitionStrength);
            }
        }
        
        // Winners get weak inhibition from other winners (balanced competition)
        if (isWinner && !isCompetitor) {
            if (brain_) {
                brain_->injectCurrent(conn.first, -inhibitionStrength * 0.5f);
            }
        }
    }
}

void AttentionalSelection::amplifyWinners(const std::vector<NeuronId>& winners, float dt) {
    
    float amplificationStrength = excitationStrength_ * dt * 5.0f;
    
    // Create recurrent connections for winner amplification
    pImpl->recurrentConnections.clear();
    
    // Each winner gets recurrent excitation from others
    for (size_t i = 0; i < winners.size(); ++i) {
        for (size_t j = 0; j < winners.size(); ++j) {
            if (i != j) {
                pImpl->recurrentConnections.emplace_back(winners[i], winners[j]);
            }
        }
    }
    
    // Apply recurrent excitation
    for (const auto& conn : pImpl->recurrentConnections) {
        if (brain_) {
            brain_->injectCurrent(conn.second, amplificationStrength * 0.8f);
        }
    }
    
    // Primary winner gets additional boost
    if (!winners.empty() && brain_) {
        brain_->injectCurrent(winners[0], amplificationStrength * 1.5f);
    }
}

float AttentionalSelection::computeNeuralActivity(
    NeuronId neuron,
    const std::vector<NeuronId>& competitors) const {
    
    float baseActivity = 0.0f;
    
    // Get current neuron activation from brain
    if (brain_) {
        // Simplified: use neuron ID to approximate current activity
        float neuronIdHash = std::hash<uint64_t>{}(neuron.value);
        baseActivity = (neuronIdHash % 1000) / 1000.0f; // Pseudorandom activity 0-1
    }
    
    // Modulate by attentional history
    if (!attentionalWeights_.empty()) {
        // This would normally access actual attentional history
        // For now, return base activity
    }
    
    // Ensure activity is in valid range
    return std::max(0.0f, std::min(1.0f, baseActivity));
}

void AttentionalSelection::normalizeWeights(std::vector<float>& weights) const {
    if (weights.empty()) return;
    
    float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
    if (sum > 0.0f) {
        for (float& weight : weights) {
            weight /= sum;
        }
    }
}

NeuronId AttentionalSelection::findWinner(
    const std::vector<NeuronId>& competitors,
    const std::vector<float>& activities) const {
    
    float maxActivity = -1.0f;
    NeuronId winner;
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activities[i] > maxActivity) {
            maxActivity = activities[i];
            winner = competitors[i];
        }
    }
    
    return winner;
}

void AttentionalSelection::updateMaps(float dt) {
    
    // Decay top-down biases
    for (auto& pair : topDownBias_) {
        pair.second *= std::pow(0.99f, dt * 10.0f); // Time constant 0.1
    }
    
    // Decay bottom-up salience
    for (auto& pair : bottomUpSalience_) {
        pair.second *= std::pow(0.95f, dt * 10.0f); // Time constant 0.2
    }
    
    // Remove very small biases
    topDownBias_.erase(std::remove_if(topDownBias_.begin(), topDownBias_.end(),
                                     [](const auto& pair) { return pair.second < 0.001f; }),
                      topDownBias_.end());
    
    bottomUpSalience_.erase(std::remove_if(bottomUpSalience_.begin(), bottomUpSalience_.end(),
                                          [](const auto& pair) { return pair.second < 0.001f; }),
                           bottomUpSalience_.end());
}

void AttentionalSelection::checkAttentionSwitching(float dt) {
    
    attentionSwitchTimer_ += dt;
    
    // Switch attention based on novelty or reward prediction error
    float novelty = 0.0f;
    float rewardPredictionError = 0.0f;
    
    // Simplified novelty detection based on salience variance
    if (!attentionalSalience_.empty()) {
        float meanSalience = std::accumulate(attentionalSalience_.begin(),
                                            attentionalSalience_.end(), 0.0f) /
                                           attentionalSalience_.size();
        
        float variance = 0.0f;
        for (float salience : attentionalSalience_) {
            variance += (salience - meanSalience) * (salience - meanSalience);
        }
        variance /= attentionalSalience_.size();
        
        novelty = std::sqrt(variance);
    }
    
    // Switch attention if novelty is high or timer expired
    if (novelty > 0.5f || attentionSwitchTimer_ > attentionDuration_) {
        switchAttention(0.5f);
        attentionSwitchTimer_ = 0.0f;
    }
}

void AttentionalSelection::switchAttention(float noveltyThreshold) {
    
    // Release current attention
    releaseAttention();
    
    // New attentional focus based on novelty
    if (!attentionalSalience_.empty()) {
        // Find most novel or salient neuron
        float maxSalience = 0.0f;
        NeuronId mostNovel;
        
        for (const auto& pair : bottomUpSalience_) {
            if (pair.second > maxSalience) {
                maxSalience = pair.second;
                mostNovel = NeuronId(pair.first);
            }
        }
        
        if (mostNovel != INVALID_NEURON_ID) {
            // Apply top-down bias to maintain focus on novel items
            applyTopDownBias(mostNovel, 0.8f);
            applyBottomUpSalience(mostNovel, 1.0f);
            
            // Set new attentional focus
            sustainAttention(RegionId(mostNovel.value / 1000)); // Approximate region
            
            attentionStartStep_ = 0; // Reset timer
        }
    }
}

void AttentionalSelection::selectBasedOnTask(float dt) {
    
    // This would integrate with task goals and planning
    // For now, maintain current attentional focus
    
    if (isSustainedAttention_ && attentionStartStep_ > 0) {
        // Gradually shift attention based on task demands
        if (attentionStability_ < 0.5f) {
            // Increase stability of current attentional focus
            attentionStability_ = std::min(1.0f, attentionStability_ + dt * 0.1f);
        }
    }
}

void AttentionalSelection::integrateWithWorkingMemory(float dt) {
    
    // Integrate with working memory content
    if (brain_) {
        auto* wm = brain_->getWorkingMemory();
        if (wm) {
            integratedWithWorkingMemory_ = true;
            
            // Get working memory neurons
            auto memoryNeurons = wm->getMemoryNeurons();
            workingMemoryNeurons_ = memoryNeurons;
            
            // Apply attentional bias to working memory content
            for (const auto& neuron : memoryNeurons) {
                float activation = wm->getNeuronActivation(neuron);
                if (activation > 0.5f) {
                    applyTopDownBias(neuron, activation * 0.5f);
                }
            }
        }
    }
}

void AttentionalSelection::applyToNeuralPopulations(const std::vector<NeuronId>& winners, float dt) {
    
    if (!brain_) return;
    
    // Apply attentional effects to broader neural populations
    float attentionalGain = excitationStrength_ * dt * 3.0f;
    
    for (const auto& winner : winners) {
        // Find neighboring neurons (simplified: neurons in same region)
        RegionId region(winner.value / 1000);
        
        // Get all neurons in this region
        auto* regionPtr = brain_->getRegion(region);
        if (regionPtr) {
            auto neurons = regionPtr->getAllNeurons();
            if (neurons) {
                for (auto* neuron : *neurons) {
                    // Apply attentional gain to neurons in attended region
                    float distance = std::abs((int)neuron->getId().value - (int)winner.value);
                    float spatialModulation = std::exp(-distance / 100.0f); // Gaussian spatial profile
                    
                    brain_->injectCurrent(neuron->getId(), attentionalGain * spatialModulation * 0.5f);
                }
            }
        }
    }
}

void AttentionalSelection::update(float dt) {
    updateMaps(dt);
    checkAttentionSwitching(dt);
    selectBasedOnTask(dt);
    integrateWithWorkingMemory(dt);
    
    // Update attention level
    currentAttentionLevel_ = getAttentionLevel();
}

std::vector<NeuronId> AttentionalSelection::getAttentionalFocus() const {
    return winners_;
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return 0.0f; // Winners receive minimal inhibition
    }
    return inhibitionStrength_;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return excitationStrength_;
    }
    return 0.0f;
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    // Store bias in map keyed by NeuronId
    auto it = topDownBias_.find(neuron.value);
    if (it != topDownBias_.end()) {
        it->second += biasStrength;
    } else {
        topDownBias_[neuron.value] = biasStrength;
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    // Store salience in map keyed by NeuronId
    auto it = bottomUpSalience_.find(neuron.value);
    if (it != bottomUpSalience_.end()) {
        it->second += salienceStrength;
    } else {
        bottomUpSalience_[neuron.value] = salienceStrength;
    }
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
        attendedRegions_.push_back(region);
    }
    isSustainedAttention_ = true;
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
    winners_.clear();
    attentionalWeights_.clear();
    pImpl->competitionResults.clear();
    pImpl->lateralInhibitions.clear();
    pImpl->recurrentConnections.clear();
    isSustainedAttention_ = false;
}

void AttentionalSelection::reset() {
    releaseAttention();
    topDownBias_.clear();
    bottomUpSalience_.clear();
    attentionalSalience_.assign(attentionalSalience_.size(), 0.0f);
    currentAttentionLevel_ = 0.0f;
    attentionStability_ = 0.0f;
    attentionSwitchTimer_ = 0.0f;
    integratedWithWorkingMemory_ = false;
    workingMemoryNeurons_.clear();
}

float AttentionalSelection::getAttentionLevel() const {
    if (winners_.empty()) return 0.0f;
    
    // Compute attention level based on winner activity and stability
    float activityLevel = 0.0f;
    for (const auto& neuron : winners_) {
        activityLevel += attentionalSalience_[neuron.value];
    }
    activityLevel /= winners_.size();
    
    return activityLevel * attentionStability_;
}

float AttentionalSelection::getAttentionalBias(NeuronId neuron) const {
    auto it = topDownBias_.find(neuron.value);
    if (it != topDownBias_.end()) {
        return it->second;
    }
    return 0.0f;
}

void AttentionalSelection::updateFromCognitiveSystems(const NeuralPlanner* planner,
                                                     const ConceptFormation* concepts,
                                                     float dt) {
    
    if (!planner || !concepts) return;
    
    // Update attentional focus based on planning goals
    auto currentGoal = planner->getCurrentGoal();
    if (!currentGoal.empty()) {
        // Apply goal-directed attention
        float goalDrivenAttention = 0.3f * dt;
        
        // This would normally convert goal vector to neural pattern
        // For now, just maintain current attentional state
    }
    
    // Update based on concept formation (novel patterns)
    size_t conceptCount = concepts->getConceptCount();
    if (conceptCount > 0) {
        float conceptAttention = std::min(1.0f, conceptCount * 0.1f);
        // Maintain attentional focus on concept-rich areas
    }
}

float AttentionalSelection::getAttentionalStability() const {
    return attentionStability_;
}

} // namespace nlm
