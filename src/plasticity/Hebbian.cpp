#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <numeric>

namespace nlm {

// SynapticTag implementation
// Note: Current implementation uses minimal actual feature to keep focus on main Hebbian enhancements

// EligibilityTrace implementation
EligibilityTrace::EligibilityTrace(float decayRate, float maxTrace) 
    : traceValue(0.0f), decayRate(decayRate), maxTrace(maxTrace) {}

void EligibilityTrace::update(float dt) {
    traceValue *= std::exp(-decayRate * dt);
}

void EligibilityTrace::add(SynapticWeight change) {
    traceValue = std::min(traceValue + change, maxTrace);
}

float EligibilityTrace::getValue() const {
    return traceValue;
}

void EligibilityTrace::reset() {
    traceValue = 0.0f;
}

// MultiStageHebbian implementation
MultiStageHebbian::MultiStageHebbian() : totalDelta(0.0f) {}

void MultiStageHebbian::update(Synapse* synapse, 
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt) {
    reset();
    
    // Update each learning stage
    for (auto& stage : stages) {
        // Calculate filtered activity for this stage
        float preActivity = applyTemporalFilter(preSpikes, stage.timeConstant, stage.weight);
        float postActivity = applyTemporalFilter(postSpikes, stage.timeConstant, stage.weight);
        
        // Compute stage-specific delta based on activity threshold
        float stageDelta = 0.0f;
        if (preActivity > stage.threshold && postActivity > stage.threshold) {
            // Co-activity detected
            float activityProduct = preActivity * postActivity;
            stageDelta = stage.weight * activityProduct;
            
            // Apply with biological constraints
            stageDelta = std::max(-1.0f, std::min(1.0f, stageDelta));
            stage.accumulatedChange += stageDelta;
            
            // Update stage activity for next time step
            stage.currentActivity = (stage.currentActivity * 0.9f) + (activityProduct * 0.1f);
        }
        
        totalDelta += stageDelta;
    }
}

SynapticWeight MultiStageHebbian::getTotalDelta() const {
    return totalDelta;
}

void MultiStageHebbian::reset() {
    for (auto& stage : stages) {
        stage.accumulatedChange = 0.0f;
        stage.currentActivity = 0.0f;
    }
    totalDelta = 0.0f;
}

void MultiStageHebbian::addLearningStage(float weight, float timeConstant, float threshold, const std::string& stageName) {
    std::string name = stageName.empty() ? "Stage_" + std::to_string(stages.size()) : stageName;
    stages.emplace_back(weight, timeConstant, threshold, name);
}

void MultiStageHebbian::removeStage(const std::string& stageName) {
    stages.erase(std::remove_if(stages.begin(), stages.end(),
                                [&stageName](const Stage& s) { return s.name == stageName; }),
                 stages.end());
}

void MultiStageHebbian::clearStages() {
    stages.clear();
    totalDelta = 0.0f;
}

// PopulationHebbian implementation
PopulationHebbian::PopulationHebbian() : timeWindow(50.0f), learningRate(0.01f), baselineActivity(0.1f), minCorrelation(0.1f) {}

PopulationHebbian::~PopulationHebbian() {}

void PopulationHebbian::addNeuron(NeuronId neuronId) {
    if (correlations.idToIndex.find(neuronId) == correlations.idToIndex.end()) {
        correlations.neuronIds.push_back(neuronId);
        correlations.idToIndex[neuronId] = correlations.neuronIds.size() - 1;
        
        // Resize correlation matrix
        int size = correlations.neuronIds.size();
        correlations.correlations.resize(size, std::vector<float>(size, baselineActivity));
    }
}

void PopulationHebbian::removeNeuron(NeuronId neuronId) {
    auto it = correlations.idToIndex.find(neuronId);
    if (it != correlations.idToIndex.end()) {
        int index = it->second;
        correlations.neuronIds.erase(correlations.neuronIds.begin() + index);
        correlations.idToIndex.erase(it);
        
        // Update idToIndex for remaining neurons
        for (auto& pair : correlations.idToIndex) {
            if (pair.second > index) {
                pair.second--;
            }
        }
        
        // Resize correlation matrix
        correlations.correlations.resize(correlations.neuronIds.size());
        for (auto& row : correlations.correlations) {
            row.resize(correlations.neuronIds.size());
        }
    }
}

void PopulationHebbian::updateCrossCorrelation(const std::vector<Timestamp>& spikeTimes,
                                               Timestamp currentTime) {
    if (correlations.neuronIds.empty()) return;
    
    recentSpikeHistory.push({currentTime, std::vector<NeuronId>(correlations.neuronIds.begin(), correlations.neuronIds.end())});
    
    // Decay old correlations
    while (!recentSpikeHistory.empty() && 
           (currentTime - recentSpikeHistory.front().first > timeWindow)) {
        recentSpikeHistory.pop();
    }
    
    // Update correlation matrix based on recent co-activity
    int matrixSize = correlations.neuronIds.size();
    
    // Reset correlations to baseline
    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            correlations.correlations[i][j] = baselineActivity;
        }
    }
    
    // Update based on recent spike patterns
    float decayFactor = 0.1f;
    int historySize = recentSpikeHistory.size();
    for (const auto& event : recentSpikeHistory) {
        const auto& neurons = event.second;
        for (size_t i = 0; i < neurons.size(); ++i) {
            for (size_t j = i + 1; j < neurons.size(); ++j) {
                int idxI = correlations.idToIndex[neurons[i]];
                int idxJ = correlations.idToIndex[neurons[j]];
                
                correlations.correlations[idxI][idxJ] += learningRate * (1.0f - correlations.correlations[idxI][idxJ]);
                correlations.correlations[idxJ][idxI] = correlations.correlations[idxI][idxJ];
                
                // Apply decay
                correlations.correlations[idxI][idxJ] *= decayFactor;
                correlations.correlations[idxJ][idxI] *= decayFactor;
            }
        }
    }
}

SynapticWeight PopulationHebbian::getCrossCorrelation(NeuronId neuron1, NeuronId neuron2) const {
    auto it1 = correlations.idToIndex.find(neuron1);
    auto it2 = correlations.idToIndex.find(neuron2);
    
    if (it1 == correlations.idToIndex.end() || it2 == correlations.idToIndex.end()) {
        return baselineActivity;
    }
    
    int idx1 = it1->second;
    int idx2 = it2->second;
    
    return std::max(baselineActivity, std::min(1.0f, correlations.correlations[idx1][idx2]));
}

void PopulationHebbian::decayCorrelations(float decayRate) {
    for (auto& row : correlations.correlations) {
        for (auto& corr : row) {
            corr *= std::exp(-decayRate * timeWindow);
        }
    }
}

void PopulationHebbian::reset() {
    for (auto& row : correlations.correlations) {
        for (auto& corr : row) {
            corr = baselineActivity;
        }
    }
    recentSpikeHistory = std::queue<std::pair<Timestamp, std::vector<NeuronId>>>();
}

void PopulationHebbian::setTimeWindow(float window) {
    timeWindow = std::max(1.0f, window);
}

void PopulationHebbian::setLearningRate(float rate) {
    learningRate = std::max(0.001f, std::min(0.1f, rate));
}

void PopulationHebbian::setBaselineActivity(float baseline) {
    baselineActivity = std::max(0.01f, std::min(1.0f, baseline));
}

// STDHebbian implementation
STDHebbian::STDHebbian() 
    : baseline(0.0f), gain(1.0f), trace(0.0f), totalDelta(0.0f) {
    window = {0.01f, 0.012f, 20.0f, 5.0f};  // Standard STDP parameters
}

STDHebbian::~STDHebbian() {}

void STDHebbian::update(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    totalDelta = 0.0f;
    Timestamp currentTime = *postSpikes.rbegin();
    
    // Update traces for each spike
    updateTraces(preSpikes, postSpikes, currentTime);
    
    // Compute weight changes based on spike timing differences
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = window.ltpWeight * std::exp(-dt / window.timeConstant);
                totalDelta += delta * trace;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -window.ltdWeight * std::exp(dt / window.timeConstant);
                totalDelta += delta * trace;
            }
        }
    }
    
    // Apply modulation
    float modulationGate = computeModulationGate(currentTime);
    totalDelta *= (baseline + gain * modulationGate);
}

SynapticWeight STDHebbian::getWeightChange() const {
    return totalDelta;
}

void STDHebbian::reset() {
    totalDelta = 0.0f;
    trace = 0.0f;
}

void STDHebbian::setLTPWeight(float weight) {
    window.ltpWeight = std::max(0.0f, std::min(0.1f, weight));
}

void STDHebbian::setLTDWeight(float weight) {
    window.ltdWeight = std::max(0.0f, std::min(0.1f, weight));
}

void STDHebbian::setTimeConstant(float tau) {
    window.timeConstant = std::max(1.0f, std::min(100.0f, tau));
}

void STDHebbian::setASAPTime(float asapTime) {
    window.asapTime = std::max(1.0f, std::min(50.0f, asapTime));
}

void STDHebbian::setTraceDecay(float decayRate) {
    window.timeConstant = std::max(1.0f, std::min(100.0f, 1.0f / decayRate));
}

void STDHebbian::setModulation(float baseline, float gain) {
    this->baseline = std::max(0.0f, baseline);
    this->gain = std::max(0.0f, gain);
}

void STDHebbian::updateTraces(const std::vector<Timestamp>& preSpikes, 
                              const std::vector<Timestamp>& postSpikes,
                              Timestamp currentTime) {
    trace = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        float dt = static_cast<float>(currentTime - preTime);
        if (dt > 0 && dt < window.asapTime) {
            trace += std::exp(-dt / window.timeConstant);
        }
    }
    
    for (Timestamp postTime : postSpikes) {
        float dt = static_cast<float>(postTime - currentTime);
        if (dt > 0 && dt < window.asapTime) {
            trace -= std::exp(-dt / window.timeConstant);
        }
    }
}

float STDHebbian::computeModulationGate(const Timestamp& spikeTime) const {
    // Simple neuromodulation gate: higher during high-frequency bursts
    return 1.0f;  // Placeholder for actual neuromodulation integration
}

// Hebbian implementation
struct Hebbian::Impl {
    float learningRate;
    float covarianceThreshold;
    float nmdaConductance;
    float tagDecayRate;
    float eligibilityDecayRate;
    float workingMemoryCapacity;
    float attentionModulation;
    float neuromodulationGate;
    float neuromodulationGain;
    float neuromodulationBaseline;
    float consolidationThreshold;
    float developmentalPruningAge;
    float pruningRate;
    
    bool enablePopulationLearning;
    float populationTimeWindow;
    float populationLearningRate;
    
    bool enableSTDHebbian;
    float stdHebbLTPWeight;
    float stdHebbLTDWeight;
    float stdHebbTimeConstant;
    
    // Multi-stage learning
    MultiStageHebbian multiStageLearning;
    
    // Population learning
    PopulationHebbian populationLearning;
    
    // STDHebbian learning
    STDHebbian stdHebbianLearning;
    
    // Synaptic tagging
    std::unordered_map<SynapseId, SynapticTag> synapticTags;
    std::unordered_map<NeuronId, std::vector<SynapseId>> neuronSynapticTags;
    
    // Eligibility traces
    std::unordered_map<SynapseId, EligibilityTrace> eligibilityTraces;
    
    // State variables
    Timestamp lastConsolidationTime;
    float accumulatedConsolidation;
    
    Impl() : learningRate(0.01f), covarianceThreshold(0.1f), nmdaConductance(0.5f),
             tagDecayRate(0.001f), eligibilityDecayRate(0.01f), 
             workingMemoryCapacity(10.0f), attentionModulation(1.0f),
             neuromodulationGate(1.0f), neuromodulationGain(1.0f),
             neuromodulationBaseline(0.1f), consolidationThreshold(0.5f),
             developmentalPruningAge(100.0f), pruningRate(0.001f),
             enablePopulationLearning(false), populationTimeWindow(50.0f),
             populationLearningRate(0.01f), enableSTDHebbian(false),
             stdHebbLTPWeight(0.01f), stdHebbLTDWeight(0.012f),
             stdHebbTimeConstant(20.0f), lastConsolidationTime(0),
             accumulatedConsolidation(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() {
    delete pImpl;
}

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    TimestepDuration currentTime = *postSpikes.rbegin();
    
    // Apply multi-stage Hebbian learning
    pImpl->multiStageLearning.update(synapse, preSpikes, postSpikes, dt);
    
    // Compute covariance-based delta (full covariance rule)
    SynapticWeight covarianceDelta = computeCovarianceDelta(preSpikes, postSpikes, dt);
    
    // Apply STDHebbian learning if enabled
    if (pImpl->enableSTDHebbian) {
        pImpl->stdHebbianLearning.update(synapse, preSpikes, postSpikes, dt);
        covarianceDelta += pImpl->stdHebbianLearning.getWeightChange();
    }
    
    // Apply population cross-correlation if enabled
    if (pImpl->enablePopulationLearning) {
        updatePopulationCrossCorrelation(synapse, preSpikes, postSpikes, dt);
    }
    
    // Apply neuromodulation gating
    float modulationGate = computeModulationGate(currentTime);
    float modulatedDelta = covarianceDelta * modulationGate;
    
    // Update eligibility traces
    for (auto& pair : pImpl->eligibilityTraces) {
        pair.second.update(dt);
    }
    
    // Apply synaptic tagging and capture
    if (!preSpikes.empty() && !postSpikes.empty()) {
        Timestamp preTime = *preSpikes.rbegin();
        Timestamp postTime = *postSpikes.rbegin();
        
        // Create synaptic tag for coactive synapse
        createSynapticTag(synapse->getId(), 1.0f);
        
        // Capture tags if both pre and post are active
        if (preTime > 0 && postTime > 0) {
            captureTags(synapse->getSourceNeuron(), synapse->getDestinationNeuron());
        }
        
        // Propagate tagging to connected synapses
        std::vector<SynapseId> activeSynapses;
        activeSynapses.push_back(synapse->getId());
        propagateTagging(activeSynapses);
    }
    
    // Decay synaptic tags
    decaySynapticTags(dt);
    
    // Apply NMDA receptor modulation
    if (!preSpikes.empty() && !postSpikes.empty()) {
        float preActivity = applyTemporalFilter(preSpikes, 10.0f, pImpl->learningRate);
        float postActivity = applyTemporalFilter(postSpikes, 10.0f, pImpl->learningRate);
        updateNMDAConductance(synapse, preActivity, postActivity);
    }
    
    // Update working memory and attention
    updateWorkingMemoryCapacity(synapse, modulatedDelta);
    modulateAttention(synapse, modulatedDelta);
    
    // Apply neuromodulation gating to final delta
    modulatedDelta *= (pImpl->neuromodulationBaseline + pImpl->neuromodulationGain * modulationGate);
    
    // Apply eligibility trace modulation
    float eligibilityValue = 0.0f;
    if (pImpl->eligibilityTraces.find(synapse->getId()) != pImpl->eligibilityTraces.end()) {
        eligibilityValue = pImpl->eligibilityTraces[synapse->getId()].getValue();
        modulatedDelta *= eligibilityValue;
    }
    
    // Add eligibility trace update
    pImpl->eligibilityTraces[synapse->getId()].add(modulatedDelta);
    
    // Apply total delta
    applyWeightChange(synapse, pImpl->multiStageLearning.getTotalDelta() + modulatedDelta);
    
    // Consolidate memory if needed
    if (std::abs(modulatedDelta) > pImpl->consolidationThreshold) {
        consolidateMemory(pImpl->consolidationThreshold);
    }
}

SynapticWeight Hebbian::computeCovarianceDelta(const std::vector<Timestamp>& preSpikes,
                                              const std::vector<Timestamp>& postSpikes,
                                              TimestepDuration dt) const {
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Full covariance rule: Δw = η × (⟨pre × post⟩ - ⟨pre⟩⟨post⟩)
    float preMean = 0.0f;
    float postMean = 0.0f;
    float prePostCovariance = 0.0f;
    
    // Convert spike times to firing rates for covariance calculation
    float preRate = static_cast<float>(preSpikes.size()) / (dt > 0 ? dt : 1.0f);
    float postRate = static_cast<float>(postSpikes.size()) / (dt > 0 ? dt : 1.0f);
    
    preMean = preRate;
    postMean = postRate;
    
    // Simplified covariance calculation based on spike timing correlations
    float correlationSum = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = static_cast<float>(std::abs(postTime - preTime));
            if (timeDiff < pImpl->covarianceThreshold) {
                correlationSum += 1.0f;
            }
        }
    }
    
    float averageCorrelation = correlationSum / (preSpikes.size() * postSpikes.size() + 1e-6f);
    prePostCovariance = averageCorrelation - (preMean * postMean);
    
    // Apply learning rate
    return pImpl->learningRate * prePostCovariance;
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply biological constraints
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    
    synapse->setWeight(newWeight);
    
    // Update NMDA receptors
    updateNMDAConductance(synapse, 0.0f, 0.0f);
}

const char* Hebbian::getName() const {
    return "EnhancedHebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void Hebbian::setCovarianceThreshold(float threshold) {
    pImpl->covarianceThreshold = std::max(0.0f, threshold);
}

void Hebbian::setNMDAConductance(float conductance) {
    pImpl->nmdaConductance = std::clamp(conductance, 0.0f, 1.0f);
}

void Hebbian::setTagDecayRate(float rate) {
    pImpl->tagDecayRate = std::max(0.0f, rate);
}

void Hebbian::setEligibilityDecay(float rate) {
    pImpl->eligibilityDecayRate = std::max(0.0f, rate);
}

void Hebbian::setWorkingMemoryCapacity(float capacity) {
    pImpl->workingMemoryCapacity = std::max(1.0f, capacity);
}

void Hebbian::setAttentionModulation(float weight) {
    pImpl->attentionModulation = std::clamp(weight, 0.0f, 2.0f);
}

void Hebbian::setNeuromodulationGate(float gate) {
    pImpl->neuromodulationGate = std::clamp(gate, 0.0f, 1.0f);
}

void Hebbian::setModulationParameters(float gain, float baseline) {
    pImpl->neuromodulationGain = std::max(0.0f, gain);
    pImpl->neuromodulationBaseline = std::max(0.0f, baseline);
}

void Hebbian::setConsolidationThreshold(float threshold) {
    pImpl->consolidationThreshold = std::max(0.0f, threshold);
}

void Hebbian::setDevelopmentalPruningAge(float age) {
    pImpl->developmentalPruningAge = std::max(1.0f, age);
}

void Hebbian::setPruningRate(float rate) {
    pImpl->pruningRate = std::max(0.0f, rate);
}

void Hebbian::addLearningStage(float weight, float timeConstant, float threshold, const std::string& stageName) {
    pImpl->multiStageLearning.addLearningStage(weight, timeConstant, threshold, stageName);
}

void Hebbian::removeLearningStage(const std::string& stageName) {
    pImpl->multiStageLearning.removeStage(stageName);
}

void Hebbian::clearLearningStages() {
    pImpl->multiStageLearning.clearStages();
}

void Hebbian::addToPopulation(NeuronId neuronId) {
    pImpl->populationLearning.addNeuron(neuronId);
}

void Hebbian::enablePopulationLearning(bool enable) {
    pImpl->enablePopulationLearning = enable;
}

void Hebbian::setPopulationTimeWindow(float window) {
    pImpl->populationLearning.setTimeWindow(window);
}

void Hebbian::setPopulationLearningRate(float rate) {
    pImpl->populationLearning.setLearningRate(rate);
}

void Hebbian::setNeuromodulator(Neuromodulator* modulator) {
    // Note: Actual neuromodulator integration would require additional code
}

void Hebbian::resetSynapticTags() {
    pImpl->synapticTags.clear();
    pImpl->neuronSynapticTags.clear();
}

void Hebbian::decaySynapticTags(float dt) {
    for (auto& pair : pImpl->synapticTags) {
        SynapticTag& tag = pair.second;
        float ageFactor = std::exp(-pImpl->tagDecayRate * dt);
        tag.tagStrength *= ageFactor;
        if (tag.tagStrength < 0.01f) {
            tag.captured = false;
        }
    }
}

void Hebbian::consolidateMemory(float consolidationThreshold) {
    TimestepDuration currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    if (currentTime - pImpl->lastConsolidationTime > 1000) {  // Consolidate every 1000ms
        pImpl->lastConsolidationTime = currentTime;
        pImpl->accumulatedConsolidation = 0.0f;
    }
    
    pImpl->accumulatedConsolidation += consolidationThreshold;
    
    if (pImpl->accumulatedConsolidation > 1.0f) {
        systemsConsolidation(0.1f);  // Consolidate 10% of memory
        pImpl->accumulatedConsolidation = 0.0f;
    }
}

void Hebbian::systemsConsolidation(float consolidationRate) {
    // Update synaptic weights based on consolidation
    for (auto& pair : pImpl->synapticTags) {
        SynapticTag& tag = pair.second;
        if (tag.captured) {
            float consolidationEffect = consolidationRate * tag.tagStrength;
            // Apply consolidation to connected synapses
            for (const auto& partner : tag.partnerTags) {
                // Modulate weight based on partner tag strength
                float modulation = partner.second * consolidationEffect;
                // This would require access to actual synapse for weight modification
            }
        }
    }
}

void Hebbian::applyDevelopmentalPruning(Synapse* synapse, float age) {
    if (synapse && age > pImpl->developmentalPruningAge) {
        // Probability of pruning based on weight and age
        float weight = synapse->getWeight();
        float pruneProbability = pImpl->pruningRate * (1.0f - std::abs(weight));
        
        // Random pruning decision
        srand(time(nullptr));
        if ((rand() % 1000) / 1000.0f < pruneProbability) {
            // Remove synapse (placeholder for actual synapse removal)
            // synapse->remove(); // Would need actual implementation
        }
    }
}

void Hebbian::createSynapticTag(SynapseId synapseId, float strength) {
    pImpl->synapticTags[synapseId] = SynapticTag{
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count(),
        strength,
        false,
        std::unordered_map<SynapseId, float>()
    };
}

void Hebbian::captureTags(SynapseId source, SynapseId target) {
    auto itSource = pImpl->synapticTags.find(source);
    auto itTarget = pImpl->synapticTags.find(target);
    
    if (itSource != pImpl->synapticTags.end() && itTarget != pImpl->synapticTags.end()) {
        SynapticTag& sourceTag = itSource->second;
        SynapticTag& targetTag = itTarget->second;
        
        if (!sourceTag.captured && !targetTag.captured) {
            sourceTag.captured = true;
            targetTag.captured = true;
            
            // Create partnership
            sourceTag.partnerTags[target] = targetTag.tagStrength;
            targetTag.partnerTags[source] = sourceTag.tagStrength;
        }
    }
}

void Hebbian::propagateTagging(const std::vector<SynapseId>& activeSynapses) {
    for (SynapseId synapseId : activeSynapses) {
        auto it = pImpl->synapticTags.find(synapseId);
        if (it != pImpl->synapticTags.end()) {
            SynapticTag& tag = it->second;
            
            // Find connected neurons
            auto neuronIt = pImpl->neuronSynapticTags.find(synapseId);
            if (neuronIt != pImpl->neuronSynapticTags.end()) {
                for (SynapseId connectedSynapse : neuronIt->second) {
                    if (pImpl->synapticTags.find(connectedSynapse) == pImpl->synapticTags.end()) {
                        createSynapticTag(connectedSynapse, tag.tagStrength * 0.5f);
                    }
                }
            }
        }
    }
}

float Hebbian::applyTemporalFilter(const std::vector<Timestamp>& spikes,
                                   float timeConstant,
                                   float decayRate) const {
    if (spikes.empty()) return 0.0f;
    
    float filteredActivity = 0.0f;
    Timestamp currentTime = *spikes.rbegin();
    
    for (Timestamp spikeTime : spikes) {
        float dt = static_cast<float>(currentTime - spikeTime);
        float weight = std::exp(-dt / timeConstant);
        filteredActivity += weight;
    }
    
    return filteredActivity * decayRate;
}

void Hebbian::updateNMDAConductance(Synapse* synapse, float preActivity, float postActivity) {
    if (!synapse) return;
    
    // Update NMDA receptor conductance based on activity
    float activitySum = preActivity + postActivity;
    float conductanceUpdate = pImpl->nmdaConductance * activitySum * 0.01f;
    
    // This would require NMDA receptor-specific methods in Synapse class
    // For now, we'll just store the modulation factor
    synapse->setWeight(synapse->getWeight() * (1.0f + conductanceUpdate));
}

void Hebbian::updateWorkingMemoryCapacity(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    // Adjust working memory capacity based on learning
    float capacityChange = delta * 0.1f;  // 10% coupling
    pImpl->workingMemoryCapacity += capacityChange;
    
    // Keep within reasonable bounds
    pImpl->workingMemoryCapacity = std::max(1.0f, std::min(100.0f, pImpl->workingMemoryCapacity));
}

void Hebbian::modulateAttention(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    // Modulate attentional weight
    float attentionChange = delta * pImpl->attentionModulation;
    // Apply attentional modulation to synaptic efficacy
    float currentEfficacy = synapse->getEfficacy();
    synapse->setEfficacy(currentEfficacy + attentionChange);
}

void Hebbian::integrateWithWorkingMemory(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    // Integrate with working memory for pattern completion
    float memoryIntegration = delta * 0.5f;  // 50% integration strength
    // This would require working memory integration methods
    // synapse->integrateWithWorkingMemory(memoryIntegration);
}

void Hebbian::consolidateWithEpisodicMemory(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Consolidate with episodic memory for long-term storage
    float episodicConsolidation = std::abs(delta) * 0.3f;  // 30% consolidation strength
    // This would require episodic memory integration methods
    // synapse->consolidateWithEpisodicMemory(episodicConsolidation);
}

void Hebbian::stabilizeMemoryTrace(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    // Stabilize memory trace to prevent drift
    float stabilization = delta * 0.05f;  // 5% stabilization strength
    // Apply stabilization to synaptic weight
    float currentWeight = synapse->getWeight();
    synapse->setWeight(currentWeight * (1.0f - stabilization));
}

float Hebbian::computeModulationGate(const Timestamp& spikeTime) const {
    // Neuromodulation gate based on spike timing and history
    float gate = pImpl->neuromodulationGate;
    
    // Apply temporal modulation
    float temporalMod = 1.0f + std::sin(spikeTime * 0.001f) * 0.1f;  // Small oscillation
    
    return gate * temporalMod;
}

void Hebbian::applyNeuromodulationGating(Synapse* synapse, float delta, float gateValue) {
    if (!synapse) return;
    
    // Apply neuromodulation gating to delta
    float gatedDelta = delta * gateValue;
    
    // This would be applied to weight changes in the update method
}

void Hebbian::modulateCognitiveStrategy(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    // Modulate cognitive strategies based on learning
    float strategyModulation = delta * 0.2f;  // 20% strategy modulation strength
    // This would require cognitive strategy integration methods
    // synapse->modulateCognitiveStrategy(strategyModulation);
}

void Hebbian::updateBehavioralPattern(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    // Update behavioral patterns based on learning
    float behavioralChange = delta * 0.15f;  // 15% behavioral change strength
    // This would require behavioral pattern integration methods
    // synapse->updateBehavioralPattern(behavioralChange);
}

void Hebbian::updatePopulationCrossCorrelation(Synapse* synapse,
                                              const std::vector<Timestamp>& preSpikes,
                                              const std::vector<Timestamp>& postSpikes,
                                              TimestepDuration dt) {
    if (!synapse) return;
    
    // Convert spike times to neuron activity vectors
    std::vector<float> preActivity(correlations.neuronIds.size(), 0.0f);
    std::vector<float> postActivity(correlations.neuronIds.size(), 0.0f);
    
    Timestamp currentTime = *postSpikes.rbegin();
    
    // Calculate population activities
    for (size_t i = 0; i < correlations.neuronIds.size(); ++i) {
        for (Timestamp spikeTime : preSpikes) {
            float dt = static_cast<float>(currentTime - spikeTime);
            if (dt < populationTimeWindow) {
                preActivity[i] += std::exp(-dt / populationTimeWindow);
            }
        }
        
        for (Timestamp spikeTime : postSpikes) {
            float dt = static_cast<float>(currentTime - spikeTime);
            if (dt < populationTimeWindow) {
                postActivity[i] += std::exp(-dt / populationTimeWindow);
            }
        }
    }
    
    // Update cross-correlation matrix
    float populationDelta = 0.0f;
    for (size_t i = 0; i < correlations.neuronIds.size(); ++i) {
        for (size_t j = 0; j < correlations.neuronIds.size(); ++j) {
            if (i != j) {
                float correlation = preActivity[i] * postActivity[j];
                populationDelta += correlation * populationLearningRate;
                
                // Update correlation matrix
                correlations.correlations[i][j] += correlation * populationLearningRate;
                correlations.correlations[j][i] = correlations.correlations[i][j];
            }
        }
    }
    
    // Apply population effect
    applyWeightChange(synapse, populationDelta);
}

void Hebbian::enableSTDHebbian(bool enable) {
    pImpl->enableSTDHebbian = enable;
}

void Hebbian::setSTDHebbianParameters(float ltp, float ltd, float tau) {
    if (pImpl->enableSTDHebbian) {
        pImpl->stdHebbianLearning.setLTPWeight(ltp);
        pImpl->stdHebbianLearning.setLTDWeight(ltd);
        pImpl->stdHebbianLearning.setTimeConstant(tau);
    }
}

void Hebbian::setWorkingMemoryIntegration(NeuralWorkingMemory* workingMemory) {
    // Note: Actual working memory integration would require additional code
    // workingMemory->addHebbianLearning(this);
}

void Hebbian::setCognitiveStrategyIntegration(ConceptFormation* strategy) {
    // Note: Actual cognitive strategy integration would require additional code
    // strategy->addHebbianLearning(this);
}

void Hebbian::decayEligibilityTraces(float dt) {
    for (auto& pair : pImpl->eligibilityTraces) {
        pair.second.update(dt);
    }
}

} // namespace nlm

// Developmental synaptic pruning implementation
void Hebbian::applyDevelopmentalPruning(Synapse* synapse, float age) {
    if (!synapse) return;
    
    // Probability of pruning based on synaptic efficacy and age
    float weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    // Calculate pruning probability using biological principles
    // Younger synapses have higher pruning probability if inactive
    // Older synapses are stabilized if active
    
    float ageFactor = std::min(1.0f, age / pImpl->developmentalPruningAge);
    float weightFactor = 1.0f - std::abs(weight);  // Weak synapses more likely to be pruned
    float efficacyFactor = 1.0f - std::min(1.0f, efficacy);  // Low efficacy synapses more likely to be pruned
    
    // Base pruning rate increases with age but is tempered by activity
    float basePruningRate = pImpl->pruningRate * ageFactor;
    
    // Activity-dependent modulation: active synapses are protected
    float activityProtection = 1.0f - (weightFactor * efficacyFactor * 0.5f);
    
    float finalPruningRate = basePruningRate * activityProtection;
    
    // Add noise for biological variability
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    if (dis(gen) < finalPruningRate) {
        // Prune the synapse
        // This would require actual synapse removal functionality
        // For now, we'll mark it for removal
        synapse->setWeight(0.0f);  // Mark as pruned
        
        // Record pruning event for analysis
        // synapse->recordPruningEvent(); // Would need to be implemented
        
        // Release synaptic resources
        // synapse->releaseResources(); // Would need to be implemented
    }
}

// Integrate with working memory and attention systems
void Hebbian::integrateWithWorkingMemory(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    // Modulate working memory based on Hebbian learning
    float workingMemoryEffect = std::min(1.0f, std::abs(delta) * 0.5f);
    
    // Update working memory capacity
    pImpl->workingMemoryCapacity += workingMemoryEffect * 0.1f;  // 10% coupling
    
    // Keep within reasonable bounds
    pImpl->workingMemoryCapacity = std::max(1.0f, std::min(100.0f, pImpl->workingMemoryCapacity));
    
    // Integrate with working memory for pattern completion
    float memoryIntegration = delta * pImpl->workingMemoryCapacity / 100.0f;
    
    // This would require working memory integration methods
    // synapse->integrateWithWorkingMemory(memoryIntegration);
}

// Integrate with cognitive strategy formation
void Hebbian::integrateWithCognitiveStrategy(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    // Modulate cognitive strategy based on learning
    float strategyEffect = std::min(1.0f, std::abs(delta) * 0.3f);
    
    // Update cognitive strategy parameters
    // This would require cognitive strategy integration methods
    // synapse->integrateWithCognitiveStrategy(strategyEffect);
    
    // Update behavioral patterns
    // This would require behavioral pattern integration methods
    // synapse->updateBehavioralPattern(strategyEffect);
}

// Update the main Hebbian update method to include developmental pruning
void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    TimestepDuration currentTime = *postSpikes.rbegin();
    
    // Get synapse age (placeholder - would need actual age tracking)
    float synapseAge = 10.0f;  // Placeholder age in simulation steps
    
    // Apply developmental pruning based on age
    applyDevelopmentalPruning(synapse, synapseAge);
    
    // Check if synapse has been pruned
    if (synapse->getWeight() == 0.0f) {
        return;  // Skip further processing for pruned synapses
    }
    
    // Apply multi-stage Hebbian learning
    pImpl->multiStageLearning.update(synapse, preSpikes, postSpikes, dt);
    
    // Compute covariance-based delta (full covariance rule)
    SynapticWeight covarianceDelta = computeCovarianceDelta(preSpikes, postSpikes, dt);
    
    // Apply STDHebbian learning if enabled
    if (pImpl->enableSTDHebbian) {
        pImpl->stdHebbianLearning.update(synapse, preSpikes, postSpikes, dt);
        covarianceDelta += pImpl->stdHebbianLearning.getWeightChange();
    }
    
    // Apply population cross-correlation if enabled
    if (pImpl->enablePopulationLearning) {
        updatePopulationCrossCorrelation(synapse, preSpikes, postSpikes, dt);
    }
    
    // Apply neuromodulation gating
    float modulationGate = computeModulationGate(currentTime);
    float modulatedDelta = covarianceDelta * modulationGate;
    
    // Update eligibility traces
    for (auto& pair : pImpl->eligibilityTraces) {
        pair.second.update(dt);
    }
    
    // Apply synaptic tagging and capture
    if (!preSpikes.empty() && !postSpikes.empty()) {
        Timestamp preTime = *preSpikes.rbegin();
        Timestamp postTime = *postSpikes.rbegin();
        
        // Create synaptic tag for coactive synapse
        createSynapticTag(synapse->getId(), 1.0f);
        
        // Capture tags if both pre and post are active
        if (preTime > 0 && postTime > 0) {
            captureTags(synapse->getSourceNeuron(), synapse->getDestinationNeuron());
        }
        
        // Propagate tagging to connected synapses
        std::vector<SynapseId> activeSynapses;
        activeSynapses.push_back(synapse->getId());
        propagateTagging(activeSynapses);
    }
    
    // Decay synaptic tags
    decaySynapticTags(dt);
    
    // Apply NMDA receptor modulation
    if (!preSpikes.empty() && !postSpikes.empty()) {
        float preActivity = applyTemporalFilter(preSpikes, 10.0f, pImpl->learningRate);
        float postActivity = applyTemporalFilter(postSpikes, 10.0f, pImpl->learningRate);
        updateNMDAConductance(synapse, preActivity, postActivity);
    }
    
    // Update working memory and attention
    integrateWithWorkingMemory(synapse, modulatedDelta);
    modulateAttention(synapse, modulatedDelta);
    
    // Apply neuromodulation gating to final delta
    modulatedDelta *= (pImpl->neuromodulationBaseline + pImpl->neuromodulationGain * modulationGate);
    
    // Apply eligibility trace modulation
    float eligibilityValue = 0.0f;
    if (pImpl->eligibilityTraces.find(synapse->getId()) != pImpl->eligibilityTraces.end()) {
        eligibilityValue = pImpl->eligibilityTraces[synapse->getId()].getValue();
        modulatedDelta *= eligibilityValue;
    }
    
    // Add eligibility trace update
    pImpl->eligibilityTraces[synapse->getId()].add(modulatedDelta);
    
    // Apply total delta
    applyWeightChange(synapse, pImpl->multiStageLearning.getTotalDelta() + modulatedDelta);
    
    // Consolidate memory if needed
    if (std::abs(modulatedDelta) > pImpl->consolidationThreshold) {
        consolidateMemory(pImpl->consolidationThreshold);
    }
}

} // namespace nlm
