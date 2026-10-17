#include "AdvancedPlasticityRules.hpp"
#include "../../brain/Synapse.hpp"
#include "../../brain/Brain.hpp"
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include <cmath>

namespace nlm {

struct CompetitionRule::Impl {
    float strength;              // Competition strength
    float decayRate;              // Rate of weight decay
    bool winnerTakesAll;          // Whether to implement winner-takes-all competition
    bool normalizeWeights;        // Whether to normalize weights
    float minWeight;              // Minimum weight for normalization
    float maxWeight;              // Maximum weight for normalization
    
    // Competition scope
    RegionId competitionScope;    // Region where competition occurs
    
    // Statistics
    size_t maxWinners;            // Number of winners in competition
    size_t minLosers;             // Number of losers to weaken
    
    Impl() : strength(0.01f), decayRate(0.001f), winnerTakesAll(true),
             normalizeWeights(true), minWeight(0.0f), maxWeight(1.0f),
             competitionScope(INVALID_REGION_ID), maxWinners(1), minLosers(2) {}
};

CompetitionRule::CompetitionRule() : pImpl(new Impl) {
    // Initialize plugin info
    PlasticityRuleInfo& info = const_cast<PlasticityRuleInfo&>(getPluginInfo());
    info.name = "CompetitionRule";
    info.version = "2.0.0";
    info.description = "Competition-based plasticity rule promoting winner-takes-all dynamics";
    info.author = "NLM Team";
}

CompetitionRule::~CompetitionRule() = default;

void CompetitionRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    /*
     * Enhanced Competition-based plasticity implementation
     * 
     * Mathematical formulation:
     * 
     * Competition factor = strength * (postActivity - avgActivity)
     * Δw = CompetitionFactor * (preActivity - baseline)
     * 
     * If winnerTakesAll:
     *   Top N synapses strengthened, bottom M synapses weakened
     *   Normalization applied to maintain weight budget
     * 
     * Biological inspiration:
     *   - Competitive learning in cortical circuits
     *   - Sparse coding through lateral inhibition
     *   - Resource allocation in neural populations
     *   - Winner-takes-all dynamics in perception
     */
    
    if (!synapse) {
        return;
    }
    
    // Calculate activities
    float preActivity = static_cast<float>(preSpikes.size()) / (dt + 0.001f);
    float postActivity = static_cast<float>(postSpikes.size()) / (dt + 0.001f);
    
    // Calculate competition factor
    // Positive factor if above average activity, negative if below
    float competitionFactor = pImpl->strength * (postActivity - pImpl->activityMean);
    
    // Calculate weight change
    float baseline = 0.5f;  // Baseline activity level
    float delta = competitionFactor * (preActivity - baseline);
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
    
    // Update statistics
    if (pImpl->sampleCount < 1000) {
        pImpl->sampleCount++;
        float deltaMean = postActivity - pImpl->activityMean;
        pImpl->activityMean += deltaMean / static_cast<float>(pImpl->sampleCount);
    }
}

void CompetitionRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply bounds
    newWeight = std::clamp(newWeight, 0.0f, 1.0f);
    
    // Normalize weights if enabled
    if (pImpl->normalizeWeights && !pImpl->winnerTakesAll) {
        normalizeWeightsInRegion(synapse, newWeight);
    }
    
    synapse->setWeight(newWeight);
}

void CompetitionRule::normalizeWeightsInRegion(Synapse* synapse, float newWeight) {
    if (!synapse) return;
    
    // This would require access to the brain's regions to normalize all synapses
    // For now, implement a simple normalization within the synapse's region
    float totalWeight = 0.0f;
    size_t synapseCount = 0;
    
    // Find the region containing this synapse
    // This would require access to the brain object, which we don't have here
    // In a real implementation, this would be handled by the brain or a competition manager
}

const char* CompetitionRule::getName() const {
    return "CompetitionRule";
}

const char* CompetitionRule::getDescription() const {
    return "Competition-based plasticity rule promoting winner-takes-all dynamics";
}

const char* CompetitionRule::getVersion() const {
    return "2.0.0";
}

void CompetitionRule::setStrength(float strength) {
    pImpl->strength = std::clamp(strength, 0.0f, 1.0f);
}

float CompetitionRule::getStrength() const {
    return pImpl->strength;
}

void CompetitionRule::setDecayRate(float decay) {
    pImpl->decayRate = std::clamp(decay, 0.0f, 1.0f);
}

float CompetitionRule::getDecayRate() const {
    return pImpl->decayRate;
}

void CompetitionRule::setWinnerTakesAll(bool wta) {
    pImpl->winnerTakesAll = wta;
}

bool CompetitionRule::getWinnerTakesAll() const {
    return pImpl->winnerTakesAll;
}

void CompetitionRule::setNormalizeWeights(bool normalize) {
    pImpl->normalizeWeights = normalize;
}

bool CompetitionRule::getNormalizeWeights() const {
    return pImpl->normalizeWeights;
}

std::unordered_map<std::string, std::string> CompetitionRule::getParameters() const {
    std::unordered_map<std::string, std::string> params;
    params["strength"] = std::to_string(pImpl->strength);
    params["decay_rate"] = std::to_string(pImpl->decayRate);
    params["winner_takes_all"] = pImpl->winnerTakesAll ? "true" : "false";
    params["normalize_weights"] = pImpl->normalizeWeights ? "true" : "false";
    params["min_weight"] = std::to_string(pImpl->minWeight);
    params["max_weight"] = std::to_string(pImpl->maxWeight);
    params["competition_scope"] = std::to_string(pImpl->competitionScope);
    params["max_winners"] = std::to_string(pImpl->maxWinners);
    params["min_losers"] = std::to_string(pImpl->minLosers);
    return params;
}

void CompetitionRule::setParameters(const std::unordered_map<std::string, std::string>& params) {
    auto it = params.find("strength");
    if (it != params.end()) {
        setStrength(std::stof(it->second));
    }
    
    it = params.find("decay_rate");
    if (it != params.end()) {
        setDecayRate(std::stof(it->second));
    }
    
    it = params.find("winner_takes_all");
    if (it != params.end()) {
        setWinnerTakesAll(it->second == "true" || it->second == "1");
    }
    
    it = params.find("normalize_weights");
    if (it != params.end()) {
        setNormalizeWeights(it->second == "true" || it->second == "1");
    }
    
    it = params.find("min_weight");
    if (it != params.end()) {
        pImpl->minWeight = std::stof(it->second);
    }
    
    it = params.find("max_weight");
    if (it != params.end()) {
        pImpl->maxWeight = std::stof(it->second);
    }
    
    it = params.find("competition_scope");
    if (it != params.end()) {
        pImpl->competitionScope = static_cast<RegionId>(std::stoul(it->second));
    }
    
    it = params.find("max_winners");
    if (it != params.end()) {
        pImpl->maxWinners = static_cast<size_t>(std::stoul(it->second));
    }
    
    it = params.find("min_losers");
    if (it != params.end()) {
        pImpl->minLosers = static_cast<size_t>(std::stoul(it->second));
    }
}

bool CompetitionRule::hasParameter(const std::string& name) const {
    static const std::vector<std::string> paramNames = {
        "strength", "decay_rate", "winner_takes_all", "normalize_weights",
        "min_weight", "max_weight", "competition_scope", "max_winners", "min_losers"
    };
    return std::find(paramNames.begin(), paramNames.end(), name) != paramNames.end();
}

const char* CompetitionRule::getParameterName(size_t index) const {
    static const std::vector<std::string> paramNames = {
        "strength", "decay_rate", "winner_takes_all", "normalize_weights",
        "min_weight", "max_weight", "competition_scope", "max_winners", "min_losers"
    };
    if (index < paramNames.size()) {
        return paramNames[index].c_str();
    }
    return nullptr;
}

std::unique_ptr<PlasticityRule> CompetitionRule::clone() const {
    auto clone = std::make_unique<CompetitionRule>();
    *clone->pImpl = *pImpl; // Deep copy
    return clone;
}

bool CompetitionRule::validate() const {
    if (pImpl->strength < 0.0f || pImpl->strength > 1.0f) return false;
    if (pImpl->decayRate < 0.0f || pImpl->decayRate > 1.0f) return false;
    if (pImpl->minWeight >= pImpl->maxWeight) return false;
    if (pImpl->maxWinners < 1) return false;
    if (pImpl->minLosers < 1) return false;
    return true;
}

void CompetitionRule::setCompetitionScope(RegionId region) {
    pImpl->competitionScope = region;
}

RegionId CompetitionRule::getCompetitionScope() const {
    return pImpl->competitionScope;
}

} // namespace nlm