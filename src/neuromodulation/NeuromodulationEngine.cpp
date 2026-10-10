#include "NeuromodulationEngine.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuromodulationEngine::Impl {
    std::shared_ptr<Brain> brain;
    NeuromodulationConfig config;
    NeuromodulationMetrics metrics;
    
    // Component systems
    std::unique_ptr<DopamineSystem> dopamineSystem;
    std::unique_ptr<CuriositySystem> curiositySystem;
    std::unique_ptr<PredictionErrorSystem> predictionErrorSystem;
    std::unique_ptr<NoveltySystem> noveltySystem;
    std::unique_ptr<NeuromodulatorNetwork> neuromodulatorNetwork;
    std::unique_ptr<PlasticityController> plasticityController;
    std::unique_ptr<NeuromodulatorDynamics> neuromodulatorDynamics;
    std::unique_ptr<NeuromodulatorSynthesis> neuromodulatorSynthesis;
    std::unique_ptr<NeuromodulatorFeedback> neuromodulatorFeedback;
    
    // Neuromodulator collection
    std::map<std::string, std::unique_ptr<Neuromodulator>> neuromodulators;
    std::map<std::string, NeuromodulatorState> neuromodulatorStates;
    std::vector<NeuromodulatorConnection> connections;
    
    // Learning state
    std::vector<MultiStepPrediction> multiStepPredictions;
    ExplorationState explorationState;
    
    // Integration flags
    bool brainSystemsIntegrated;
    bool memorySystemsIntegrated;
    bool plasticitySystemsIntegrated;
    bool developmentSystemIntegrated;
    bool agentSystemIntegrated;
    
    Impl() : brain(nullptr),
             brainSystemsIntegrated(false),
             memorySystemsIntegrated(false),
             plasticitySystemsIntegrated(false),
             developmentSystemIntegrated(false),
             agentSystemIntegrated(false) {}
};

NeuromodulationEngine::NeuromodulationEngine(std::shared_ptr<Brain> brain, 
                                            const NeuromodulationConfig& config)
    : pImpl(new Impl)
{
    pImpl->brain = brain;
    pImpl->config = config;
    
    NLM_LOG_INFO("NeuromodulationEngine initialized");
}

NeuromodulationEngine::~NeuromodulationEngine() {
    shutdown();
}

NeuromodulationEngine::NeuromodulationEngine(NeuromodulationEngine&& other) noexcept
    : pImpl(std::exchange(other.pImpl, nullptr)) {}

NeuromodulationEngine& NeuromodulationEngine::operator=(NeuromodulationEngine&& other) noexcept {
    if (this != &other) {
        shutdown();
        pImpl = std::exchange(other.pImpl, nullptr);
    }
    return *this;
}

bool NeuromodulationEngine::initialize() {
    try {
        // Initialize component systems
        pImpl->dopamineSystem = std::make_unique<DopamineSystem>();
        pImpl->curiositySystem = std::make_unique<CuriositySystem>();
        pImpl->predictionErrorSystem = std::make_unique<PredictionErrorSystem>();
        pImpl->noveltySystem = std::make_unique<NoveltySystem>();
        
        // Initialize advanced systems
        pImpl->neuromodulatorNetwork = std::make_unique<NeuromodulatorNetwork>();
        pImpl->plasticityController = std::make_unique<PlasticityController>();
        pImpl->neuromodulatorDynamics = std::make_unique<NeuromodulatorDynamics>();
        pImpl->neuromodulatorSynthesis = std::make_unique<NeuromodulatorSynthesis>();
        pImpl->neuromodulatorFeedback = std::make_unique<NeuromodulatorFeedback>();
        
        // Create base neuromodulators
        createBaseNeuromodulators();
        
        // Establish connections between neuromodulators
        establishConnections();
        
        NLM_LOG_INFO("NeuromodulationEngine initialization completed successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuromodulationEngine initialization failed: " + std::string(e.what()));
        return false;
    }
}

void NeuromodulationEngine::step(TimestepDuration dt, SimulationStep step) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Compute cross-talk effects between neuromodulators
    computeCrossTalkEffects();
    
    // Update neuromodulator states
    for (auto& [name, state] : pImpl->neuromulatorStates) {
        updateNeuromodulatorState(name, dt);
    }
    
    // Update dopamine system
    if (pImpl->dopamineSystem) {
        pImpl->dopamineSystem->update(dt, step);
    }
    
    // Update curiosity system
    if (pImpl->curiositySystem) {
        pImpl->curiositySystem->update(dt);
    }
    
    // Update prediction error system
    if (pImpl->predictionErrorSystem) {
        pImpl->predictionErrorSystem->update(dt);
    }
    
    // Update novelty system
    if (pImpl->noveltySystem) {
        pImpl->noveltySystem->update(dt);
    }
    
    // Update neuromodulator network
    if (pImpl->neuromodulatorNetwork) {
        pImpl->neuromodulatorNetwork->update(dt);
    }
    
    // Update neuromodulator dynamics
    if (pImpl->neuromodulatorDynamics) {
        pImpl->neuromodulatorDynamics->update(dt);
    }
    
    // Update neuromodulator synthesis
    if (pImpl->neuromodulatorSynthesis) {
        pImpl->neuromodulatorSynthesis->update(dt);
    }
    
    // Update neuromodulator feedback loops
    if (pImpl->neuromodulatorFeedback) {
        pImpl->neuromodulatorFeedback->update(dt);
    }
    
    // Update neuromodulator network
    if (pImpl->neuromodulatorNetwork) {
        pImpl->neuromodulatorNetwork->update(dt);
    }
    
    // Update neuromodulator dynamics
    if (pImpl->neuromodulatorDynamics) {
        pImpl->neuromodulatorDynamics->update(dt);
    }
    
    // Update neuromodulator synthesis
    if (pImpl->neuromodulatorSynthesis) {
        pImpl->neuromodulatorSynthesis->update(dt);
    }
    
    // Update neuromodulator feedback
    if (pImpl->neuromodulatorFeedback) {
        pImpl->neuromodulatorFeedback->update(dt);
    }
    
    // Update plasticity controller
    if (pImpl->plasticityController) {
        pImpl->plasticityController->update(dt);
    }
    
    // Update exploration state
    updateExplorationState(dt);
    
    // Integrate with brain systems
    integrateWithBrainSystems();
    
    // Adapt to environment and maintain homeostasis
    if (pImpl->config.enableRealTimeComputation) {
        adaptToEnvironment();
        maintainHomeostasis();
    }
    
    // Compute metrics
    computeMetrics(dt, step);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (duration.count() > pImpl->config.maxComputationTimePerStep * 1000) {
        NLM_LOG_WARNING("NeuromodulationEngine computation exceeded time limit: " + 
                       std::to_string(duration.count()) + "ms");
    }
}

void NeuromodulationEngine::setConfig(const NeuromodulationConfig& config) {
    pImpl->config = config;
    
    // Reinitialize systems if needed
    if (pImpl->config.enableMultiLayerModulation) {
        establishConnections();
    }
}

void NeuromodulationEngine::updateNeuromodulatorState(const std::string& name, const NeuromodulatorState& state) {
    pImpl->neuromodulatorStates[name] = state;
}

NeuromodulatorState NeuromodulationEngine::getNeuromodulatorState(const std::string& name) const {
    auto it = pImpl->neuromulatorStates.find(name);
    if (it != pImpl->neuromulatorStates.end()) {
        return it->second;
    }
    return NeuromodulatorState();
}

void NeuromodulationEngine::synthesizeNewModulator(const std::string& name, float intensity, float duration) {
    if (pImpl->neuromodulatorSynthesis) {
        pImpl->neuromodulatorSynthesis->synthesizeModulator(name, intensity, duration);
    }
}

void NeuromodulationEngine::establishConnection(const NeuromodulatorConnection& connection) {
    pImpl->connections.push_back(connection);
}

void NeuromodulationEngine::computeCrossTalkEffects() {
    // Clear existing effects
    for (auto& [name, state] : pImpl->neuromulatorStates) {
        state.accumulation = 0.0f;
    }
    
    // Compute cross-talk effects from connections
    for (const auto& connection : pImpl->connections) {
        if (pImpl->neuromodulatorStates.find(connection.sourceModulator) != pImpl->neuromodulatorStates.end() &&
            pImpl->neuromulatorStates.find(connection.targetModulator) != pImpl->neuromulatorStates.end()) {
            
            const auto& sourceState = pImpl->neuromulatorStates[connection.sourceModulator];
            auto& targetState = pImpl->neuromulatorStates[connection.targetModulator];
            
            float effect = sourceState.currentLevel * connection.strength;
            
            switch (connection.type) {
                case NeuromodulatorConnection::ConnectionType::Excitatory:
                    targetState.accumulation += effect;
                    break;
                case NeuromodulatorConnection::ConnectionType::Inhibitory:
                    targetState.accumulation -= effect;
                    break;
                case NeuromodulatorConnection::ConnectionType::Modulatory:
                    targetState.accumulation += effect * targetState.plasticityInfluence;
                    break;
                case NeuromodulatorConnection::ConnectionType::Mutualistic:
                    targetState.accumulation += effect * 0.5f;
                    sourceState.accumulation += effect * 0.5f;
                    break;
                case NeuromodulatorConnection::ConnectionType::Antagonistic:
                    targetState.accumulation -= effect * 0.5f;
                    break;
            }
        }
    }
    
    // Update neuromodulator levels based on accumulation
    for (auto& [name, state] : pImpl->neuromulatorStates) {
        state.targetLevel = std::clamp(state.targetLevel + state.accumulation, 
                                     0.0f, pImpl->config.safetyLevelThreshold);
    }
}

void NeuromodulationEngine::processPredictionError(float prediction, float target, float reward) {
    // Update prediction error system
    if (pImpl->predictionErrorSystem) {
        pImpl->predictionErrorSystem->computeError(prediction, target, reward);
    }
    
    // Update multi-step predictions
    MultiStepPrediction pred;
    pred.currentPrediction = prediction;
    pred.targetValue = target;
    pred.futurePredictions = std::vector<float>(3, prediction * 0.9f);  // Simple decay model
    pred.futureRewards = std::vector<float>(3, reward * 0.8f);
    pred.gamma = pImpl->config.predictionErrorLearningRate;
    
    pImpl->multiStepPredictions.push_back(pred);
    
    // Keep predictions bounded
    if (pImpl->multiStepPredictions.size() > 10) {
        pImpl->multiStepPredictions.erase(pImpl->multiStepPredictions.begin());
    }
}

void NeuromodulationEngine::processCuriositySignal(float novelty, float predictionError) {
    // Update curiosity system
    if (pImpl->curiositySystem) {
        pImpl->curiositySystem->update(novelty, predictionError, 1.0f);
    }
    
    // Update exploration state
    updateExplorationState(1.0f);
}

void NeuromodulationEngine::processRewardSignal(float reward, float predictedReward) {
    float error = reward - predictedReward;
    
    // Update dopamine system
    if (pImpl->dopamineSystem) {
        pImpl->dopamineSystem->update(error, 1.0f);
    }
    
    // Update prediction error
    if (pImpl->predictionErrorSystem) {
        pImpl->predictionErrorSystem->computeError(predictedReward, reward);
    }
}

void NeuromodulationEngine::applyPlasticityModulation(float synapseWeight, const Neuromodulator& modulator) const {
    if (pImpl->plasticityController) {
        pImpl->plasticityController->applyModulation(synapseWeight, modulator);
    }
}

float NeuromodulationEngine::computePlasticityFactor(const std::unordered_set<std::string>& neuromodulators) const {
    float totalFactor = 1.0f;
    
    for (const auto& name : neuromodulators) {
        auto it = pImpl->neuromulatorStates.find(name);
        if (it != pImpl->neuromulatorStates.end()) {
            totalFactor *= (1.0f + it->second.plasticityInfluence * 0.5f);
        }
    }
    
    return std::clamp(totalFactor, 0.0f, 10.0f);  // Cap at 10x for safety
}

void NeuromodulationEngine::adaptToEnvironment() {
    if (pImpl->config.enableAdaptation) {
        // Simple adaptation mechanism
        for (auto& [name, state] : pImpl->neuromulatorStates) {
            // Smooth adaptation towards homeostatic target
            float adaptation = (pImpl->config.homeostaticTargetLevel - state.currentLevel) * 
                              pImpl->config.adaptationRate;
            state.targetLevel += adaptation;
        }
    }
}

void NeuromodulationEngine::maintainHomeostasis() {
    // Maintain homeostasis by regulating neuromodulator levels
    for (auto& [name, state] : pImpl->neuromulatorStates) {
        float deviation = std::abs(state.currentLevel - pImpl->config.homeostaticTargetLevel);
        float correction = -deviation * pImpl->config.adaptationRate;
        
        state.targetLevel += correction;
        
        // Apply bounded regulation
        state.targetLevel = std::clamp(state.targetLevel, 
                                      0.0f, pImpl->config.safetyLevelThreshold);
    }
}

void NeuromodulationEngine::integrateWithBrainSystems() {
    if (pImpl->brain && !pImpl->brainSystemsIntegrated) {
        NLM_LOG_INFO("Integrating neuromodulation with brain systems");
        
        // Integrate with spike system
        if (auto spikeSystem = pImpl->brain->getSpikeSystem()) {
            // Apply neuromodulatory effects to spike timing
        }
        
        // Integrate with memory systems
        integrateWithMemorySystems();
        
        // Integrate with plasticity systems
        integrateWithPlasticitySystems();
        
        // Integrate with development system
        integrateWithDevelopmentSystem();
        
        // Integrate with agent system
        integrateWithAgentSystem();
        
        pImpl->brainSystemsIntegrated = true;
    }
}

void NeuromodulationEngine::integrateWithMemorySystems() {
    if (pImpl->brain && !pImpl->memorySystemsIntegrated && pImpl->config.integrateWithMemorySystems) {
        // Integrate with working memory
        if (auto workingMemory = pImpl->brain->getWorkingMemory()) {
            // Modulate working memory consolidation
        }
        
        // Integrate with episodic memory
        if (auto episodicMemory = pImpl->brain->getEpisodicMemory()) {
            // Enhance episodic memory based on neuromodulators
        }
        
        // Integrate with associative memory
        if (auto associativeMemory = pImpl->brain->getAssociativeMemory()) {
            // Facilitate associative learning with neuromodulation
        }
        
        pImpl->memorySystemsIntegrated = true;
    }
}

void NeuromodulationEngine::integrateWithPlasticitySystems() {
    if (pImpl->brain && !pImpl->plasticitySystemsIntegrated && pImpl->config.integrateWithPlasticitySystems) {
        // Integrate with STDP
        if (auto stdp = pImpl->brain->getSTDP()) {
            // Apply neuromodulatory scaling to STDP
        }
        
        // Integrate with Hebbian plasticity
        if (auto hebbian = pImpl->brain->getHebbian()) {
            // Apply neuromodulatory gating to Hebbian learning
        }
        
        // Integrate with structural plasticity
        if (auto structural = pImpl->brain->getStructuralPlasticity()) {
            // Enable structural changes based on neuromodulatory signals
        }
        
        pImpl->plasticitySystemsIntegrated = true;
    }
}

void NeuromodulationEngine::integrateWithDevelopmentSystem() {
    if (pImpl->brain && !pImpl->developmentSystemIntegrated && pImpl->config.integrateWithDevelopmentSystem) {
        // Integrate with development system
        if (auto development = pImpl->brain->getDevelopmentSystem()) {
            // Apply neuromodulatory effects to developmental processes
        }
        
        pImpl->developmentSystemIntegrated = true;
    }
}

void NeuromodulationEngine::integrateWithAgentSystem() {
    if (pImpl->brain && !pImpl->agentSystemIntegrated && pImpl->config.integrateWithAgentSystem) {
        // Integrate with agent brain
        if (auto agentBrain = pImpl->brain->getAgentBrain()) {
            // Apply neuromodulatory effects to agent behavior
        }
        
        pImpl->agentSystemIntegrated = true;
    }
}

std::vector<std::string> NeuromodulationEngine::getActiveNeuromodulators() const {
    std::vector<std::string> active;
    
    for (const auto& [name, state] : pImpl->neuromulatorStates) {
        if (state.currentLevel > 0.01f) {
            active.push_back(name);
        }
    }
    
    return active;
}

float NeuromodulationEngine::getTotalNeuromodulationLevel() const {
    float total = 0.0f;
    
    for (const auto& [name, state] : pImpl->neuromulatorStates) {
        total += state.currentLevel;
    }
    
    return total / static_cast<float>(pImpl->neuromulatorStates.size());
}

bool NeuromodulationEngine::validateSafetyConstraints() const {
    for (const auto& [name, state] : pImpl->neuromulatorStates) {
        if (state.currentLevel > pImpl->config.safetyLevelThreshold) {
            NLM_LOG_ERROR("Neuromodulator " + name + " exceeded safety threshold: " + 
                         std::to_string(state.currentLevel));
            return false;
        }
        
        // Check for runaway accumulation
        if (std::isnan(state.currentLevel) || std::isinf(state.currentLevel)) {
            NLM_LOG_ERROR("Neuromodulator " + name + " has invalid value: " + 
                         std::to_string(state.currentLevel));
            return false;
        }
    }
    
    return true;
}

void NeuromodulationEngine::enforceSafetyConstraints() {
    for (auto& [name, state] : pImpl->neuromulatorStates) {
        state.currentLevel = std::clamp(state.currentLevel, 0.0f, pImpl->config.safetyLevelThreshold);
    }
}

bool NeuromodulationEngine::checkNumericalStability() const {
    for (const auto& [name, state] : pImpl->neuromulatorStates) {
        if (std::isnan(state.currentLevel) || std::isinf(state.currentLevel)) {
            return false;
        }
        
        // Check for excessively large values
        if (state.currentLevel > 1000.0f) {
            return false;
        }
    }
    
    return true;
}

void NeuromodulationEngine::reset() {
    NLM_LOG_INFO("Resetting NeuromodulationEngine");
    
    // Reset all neuromodulator states
    for (auto& [name, state] : pImpl->neuromulatorStates) {
        state.currentLevel = 0.0f;
        state.targetLevel = 0.0f;
        state.accumulation = 0.0f;
        state.homeostasisValue = pImpl->config.homeostaticTargetLevel;
    }
    
    // Clear predictions and exploration state
    pImpl->multiStepPredictions.clear();
    pImpl->explorationState.intrinsicMotivation = 0.5f;
    
    // Reset integration flags
    pImpl->brainSystemsIntegrated = false;
    pImpl->memorySystemsIntegrated = false;
    pImpl->plasticitySystemsIntegrated = false;
    pImpl->developmentSystemIntegrated = false;
    pImpl->agentSystemIntegrated = false;
}

void NeuromodulationEngine::shutdown() {
    NLM_LOG_INFO("Shutting down NeuromodulationEngine");
    
    reset();
    
    // Clear all systems
    pImpl->dopamineSystem.reset();
    pImpl->curiositySystem.reset();
    pImpl->predictionErrorSystem.reset();
    pImpl->noveltySystem.reset();
    pImpl->neuromodulatorNetwork.reset();
    pImpl->plasticityController.reset();
    pImpl->neuromodulatorDynamics.reset();
    pImpl->neuromodulatorSynthesis.reset();
    pImpl->neuromodulatorFeedback.reset();
    
    pImpl->neuromodulators.clear();
    pImpl->neuromulatorStates.clear();
    pImpl->connections.clear();
    pImpl->multiStepPredictions.clear();
}

void NeuromodulationEngine::updateNeuromodulatorState(const std::string& name, TimestepDuration dt) {
    auto it = pImpl->neuromulatorStates.find(name);
    if (it != pImpl->neuromulatorStates.end()) {
        auto& state = it->second;
        
        // Apply accumulation from cross-talk
        state.accumulation = std::min(1.0f, state.accumulation);
        
        // Smooth transition to target level
        float transitionSpeed = 1.0f / pImpl->config.neuromodulatorTimeConstant;
        float delta = state.targetLevel - state.currentLevel;
        state.currentLevel += delta * transitionSpeed * static_cast<float>(dt);
        
        // Apply homeostatic regulation
        float homeostasisCorrection = (pImpl->config.homeostaticTargetLevel - state.currentLevel) * 
                                     pImpl->config.adaptationRate;
        state.currentLevel += homeostasisCorrection * static_cast<float>(dt);
        
        // Decay excess
        state.currentLevel -= state.decayRate * static_cast<float>(dt);
        state.currentLevel = std::max(0.0f, state.currentLevel);
        
        // Update plasticity influence
        state.plasticityInfluence = 1.0f + state.currentLevel * 0.5f;
        
        // Update history
        state.history.push_back(state.currentLevel);
        if (state.history.size() > 1000) {
            state.history.erase(state.history.begin());
        }
        
        // Validate bounds
        state.currentLevel = std::clamp(state.currentLevel, 0.0f, pImpl->config.safetyLevelThreshold);
    }
}

void NeuromodulationEngine::createBaseNeuromodulators() {
    // Create dopamine neuromodulator
    auto dopamine = std::make_unique<Dopamine>();
    dopamine->setName("Dopamine");
    pImpl->neuromodulators["Dopamine"] = std::move(dopamine);
    
    // Create acetylcholine neuromodulator
    auto acetylcholine = std::make_unique<Acetylcholine>();
    acetylcholine->setName("Acetylcholine");
    pImpl->neuromodulators["Acetylcholine"] = std::move(acetylcholine);
    
    // Create norepinephrine neuromodulator
    auto norepinephrine = std::make_unique<Norepinephrine>();
    norepinephrine->setName("Norepinephrine");
    pImpl->neuromodulators["Norepinephrine"] = std::move(norepinephrine);
    
    // Create serotonin neuromodulator
    auto serotonin = std::make_unique<Serotonin>();
    serotonin->setName("Serotonin");
    pImpl->neuromodulators["Serotonin"] = std::move(serotonin);
    
    // Initialize their states
    for (const auto& [name, modulator] : pImpl->neuromodulators) {
        NeuromodulatorState state;
        state.homeostasisValue = 0.5f + (rand() % 100) / 200.0f;  // Random between 0.5 and 1.0
        pImpl->neuromodulatorStates[name] = state;
    }
}

void NeuromodulationEngine::establishConnections() {
    // Establish cross-talk connections between neuromodulators
    if (pImpl->config.enableCrossTalk) {
        // Dopamine excites acetylcholine (attention modulation)
        NeuromodulatorConnection da_ach;
        da_ach.sourceModulator = "Dopamine";
        da_ach.targetModulator = "Acetylcholine";
        da_ach.type = NeuromodulatorConnection::ConnectionType::Excitatory;
        da_ach.strength = 0.3f;
        pImpl->connections.push_back(da_ach);
        
        // Norepinephrine excites dopamine (arousal effect)
        NeuromodulatorConnection ne_da;
        ne_da.sourceModulator = "Norepinephrine";
        ne_da.targetModulator = "Dopamine";
        ne_da.type = NeuromodulatorConnection::ConnectionType::Excitatory;
        ne_da.strength = 0.4f;
        pImpl->connections.push_back(ne_da);
        
        // Serotonin inhibits dopamine (mood regulation)
        NeuromodulatorConnection 5ht_da;
        5ht_da.sourceModulator = "Serotonin";
        5ht_da.targetModulator = "Dopamine";
        5ht_da.type = NeuromodulatorConnection::ConnectionType::Inhibitory;
        5ht_da.strength = 0.2f;
        pImpl->connections.push_back(5ht_da);
        
        // Mutualistic connection between acetylcholine and norepinephrine
        NeuromodulatorConnection ach_ne;
        ach_ne.sourceModulator = "Acetylcholine";
        ach_ne.targetModulator = "Norepinephrine";
        ach_ne.type = NeuromodulatorConnection::ConnectionType::Mutualistic;
        ach_ne.strength = 0.3f;
        pImpl->connections.push_back(ach_ne);
    }
}

void NeuromodulationEngine::updateExplorationState(TimestepDuration dt) {
    // Update exploration state based on curiosity and novelty
    float curiosity = pImpl->curiositySystem ? pImpl->curiositySystem->getLevel() : 0.0f;
    float novelty = pImpl->noveltySystem ? pImpl->noveltySystem->getLevel() : 0.0f;
    float predictionError = pImpl->predictionErrorSystem ? pImpl->predictionErrorSystem->getMagnitude() : 0.0f;
    
    // Update exploration state with weighted inputs
    pImpl->explorationState.intrinsicMotivation = 
        curiosity * 0.4f + novelty * 0.3f + predictionError * 0.3f;
    
    // Decay exploration bonus
    pImpl->explorationState.explorationBonus -= pImpl->explorationState.noveltySensitivity * 0.01f * static_cast<float>(dt);
    pImpl->explorationState.explorationBonus = std::max(0.0f, pImpl->explorationState.explorationBonus);
}

void NeuromodulationEngine::computeMetrics(TimestepDuration dt, SimulationStep step) {
    pImpl->metrics.computationTime = dt;
    
    // Compute memory usage
    size_t totalMemory = 0;
    for (const auto& [name, state] : pImpl->neuromulatorStates) {
        totalMemory += state.history.size() * sizeof(float);
    }
    pImpl->metrics.memoryUsage = static_cast<float>(totalMemory) / 1024.0f;  // KB
    
    // Compute average neuromodulator level
    float totalLevel = 0.0f;
    int count = 0;
    for (const auto& [name, state] : pImpl->neuromulatorStates) {
        totalLevel += state.currentLevel;
        count++;
    }
    pImpl->metrics.averageNeuromodulatorLevel = count > 0 ? totalLevel / count : 0.0f;
    
    // Compute homeostasis ratio
    float homeostasisSum = 0.0f;
    for (const auto& [name, state] : pImpl->neuromulatorStates) {
        homeostasisSum += std::abs(state.currentLevel - pImpl->config.homeostaticTargetLevel);
    }
    pImpl->metrics.homeostasisRatio = 1.0f - (homeostasisSum / static_cast<float>(count) / pImpl->config.homeostaticTargetLevel);
    
    // Compute learning efficiency (placeholder)
    pImpl->metrics.learningEfficiency = 0.5f;
    
    // Compute exploration efficiency
    pImpl->metrics.explorationEfficiency = pImpl->explorationState.intrinsicMotivation;
    
    // Check numerical stability
    pImpl->metrics.numericalStability = checkNumericalStability() ? 1.0f : 0.0f;
    
    // Compute boundedness (all levels within safe range)
    float boundednessSum = 0.0f;
    for (const auto& [name, state] : pImpl->neuromulatorStates) {
        boundednessSum += state.currentLevel / pImpl->config.safetyLevelThreshold;
    }
    pImpl->metrics.boundedness = std::min(1.0f, boundednessSum / static_cast<float>(count));
    
    // Simple convergence rate (change in neuromodulator levels)
    static float previousTotalLevel = 0.0f;
    float currentTotalLevel = getTotalNeuromodulationLevel();
    pImpl->metrics.convergenceRate = std::abs(currentTotalLevel - previousTotalLevel);
    previousTotalLevel = currentTotalLevel;
}

} // namespace nlm