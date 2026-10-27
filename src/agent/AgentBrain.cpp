#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <numeric>
#include <iomanip>

namespace nlm {

struct AgentBrain::Impl {
    // Pimpl implementation details
    std::shared_ptr<Brain> brain;
    AgentBrainConfig config;
    
    // State management
    std::vector<Neuron*> motorForward;
    std::vector<Neuron*> motorBackward;
    std::vector<Neuron*> motorTurnLeft;
    std::vector<Neuron*> motorTurnRight;
    std::vector<Neuron*> motorInteract;
    std::vector<Neuron*> motorWait;
    
    std::vector<Neuron*> sensoryVision;
    std::vector<Neuron*> sensoryTouch;
    std::vector<Neuron*> sensoryInternal;
    std::vector<Neuron*> sensoryProprioception;
    
    // Neuromodulation state
    float dopamineLevel;
    float noveltyLevel;
    float curiosityLevel;
    float predictionError;
    float expectedReward;
    
    // Development state
    double developmentalAge;
    float plasticityModifier;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision;
    float sensoryNoveltyDecay;
    
    // Performance tracking
    mutable size_t processingCycles;
    size_t cacheHits;
    size_t cacheMisses;
    
    // Batch processing
    std::vector<SensoryPercept> batchBuffer;
    std::vector<MotorCommand> batchResults;
    
    // Expert feature state
    std::function<float(SynapticWeight)> customPlasticityRule;
    
    Impl(std::shared_ptr<Brain> b, const AgentBrainConfig& cfg) 
        : brain(b), config(cfg)
        , dopamineLevel(0.0f), noveltyLevel(0.0f), curiosityLevel(0.0f)
        , predictionError(0.0f), expectedReward(0.0f)
        , developmentalAge(0.0), plasticityModifier(1.0f)
        , sensoryNoveltyDecay(cfg.noveltyDecayRate)
        , processingCycles(0), cacheHits(0), cacheMisses(0) {
        initializeNeuronGroups();
    }
    
    void initializeNeuronGroups() {
        if (!brain) return;
        
        // Reset all groups
        motorForward.clear(); motorBackward.clear(); motorTurnLeft.clear();
        motorTurnRight.clear(); motorInteract.clear(); motorWait.clear();
        sensoryVision.clear(); sensoryTouch.clear(); sensoryInternal.clear();
        sensoryProprioception.clear();
        
        for (const auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        distributeToMotorGroup(n);
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        distributeToSensoryGroup(n);
                    }
                }
            }
        }
    }
    
    void distributeToMotorGroup(Neuron* neuron) {
        size_t idx = motorForward.size() + motorBackward.size() + 
                     motorTurnLeft.size() + motorTurnRight.size() +
                     motorInteract.size() + motorWait.size();
        
        switch (idx % 6) {
            case 0: motorForward.push_back(neuron); break;
            case 1: motorBackward.push_back(neuron); break;
            case 2: motorTurnLeft.push_back(neuron); break;
            case 3: motorTurnRight.push_back(neuron); break;
            case 4: motorInteract.push_back(neuron); break;
            case 5: motorWait.push_back(neuron); break;
        }
    }
    
    void distributeToSensoryGroup(Neuron* neuron) {
        size_t idx = sensoryVision.size() + sensoryTouch.size() +
                     sensoryInternal.size() + sensoryProprioception.size();
        
        switch (idx % 4) {
            case 0: sensoryVision.push_back(neuron); break;
            case 1: sensoryTouch.push_back(neuron); break;
            case 2: sensoryInternal.push_back(neuron); break;
            case 3: sensoryProprioception.push_back(neuron); break;
        }
    }
    
    float calculateActivity(const std::vector<Neuron*>& neurons) const {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    }
    
    MotorCommand selectMotorCommand() const {
        float forwardAct = calculateActivity(motorForward);
        float backwardAct = calculateActivity(motorBackward);
        float leftAct = calculateActivity(motorTurnLeft);
        float rightAct = calculateActivity(motorTurnRight);
        float interactAct = calculateActivity(motorInteract);
        float waitAct = calculateActivity(motorWait);
        
        // Command priorities with weights
        struct CommandWithWeight {
            MotorCommand cmd;
            float activity;
            float weight;
        };
        
        CommandWithWeight commands[] = {
            {MotorCommand::MoveForward, forwardAct, 1.0f},
            {MotorCommand::MoveBackward, backwardAct, 0.8f},
            {MotorCommand::TurnLeft, leftAct, 0.8f},
            {MotorCommand::TurnRight, rightAct, 0.8f},
            {MotorCommand::Interact, interactAct, 0.9f},
            {MotorCommand::Wait, waitAct, 0.0f}
        };
        
        MotorCommand best = MotorCommand::Wait;
        float bestScore = 0.0f;
        
        for (const auto& c : commands) {
            float score = c.activity * c.weight;
            if (score > bestScore && score > 0.5f) {
                bestScore = score;
                best = c.cmd;
            }
        }
        
        return best;
    }
    
    void applyBatchProcessing(const SensoryPercept& percept, double timestep) {
        batchBuffer.push_back(percept);
        
        if (config.enableBatchProcessing && 
            (batchBuffer.size() >= 100 || config.enableBatchProcessing)) {
            
            processBatchInternal(timestep);
            batchResults.clear();
        }
    }
    
    void processBatchInternal(double timestep) {
        // Process all sensory inputs in batch
        for (const auto& percept : batchBuffer) {
            processSingleSensory(percept);
        }
        
        // Generate actions for all processed percepts
        for (size_t i = 0; i < batchBuffer.size(); ++i) {
            batchResults.push_back(selectMotorCommand());
        }
        
        batchBuffer.clear();
    }
    
    void processSingleSensory(const SensoryPercept& percept) {
        // Vision input
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < sensoryVision.size() && i < vision.size(); ++i) {
            if (sensoryVision[i]) {
                float current = vision[i] * 5.0f;
                sensoryVision[i]->injectCurrent(current);
            }
        }
        
        // Touch input
        const auto& touch = percept.getTouch();
        for (size_t i = 0; i < sensoryTouch.size() && i < touch.size(); ++i) {
            if (sensoryTouch[i]) {
                float current = touch[i] * 8.0f;
                sensoryTouch[i]->injectCurrent(current);
            }
        }
        
        // Internal signals
        const auto& intern = percept.getInternal();
        for (size_t i = 0; i < sensoryInternal.size() && i < intern.size(); ++i) {
            if (sensoryInternal[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
                sensoryInternal[i]->injectCurrent(current);
            }
        }
        
        // Proprioception
        const auto& proprio = percept.getProprioception();
        for (size_t i = 0; i < sensoryProprioception.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
                sensoryProprioception[i]->injectCurrent(current);
            }
        }
        
        // Update novelty detection
        if (!vision.empty()) {
            float totalDiff = 0.0f;
            for (size_t i = 0; i < vision.size() && i < previousVision.size(); ++i) {
                float diff = std::abs(vision[i] - previousVision[i]);
                totalDiff += diff;
            }
            
            noveltyLevel = totalDiff / std::max<size_t>(vision.size(), 1);
            noveltyLevel *= sensoryNoveltyDecay;
            previousVision = vision;
        }
    }
    
    void updateWithCuriosity(MotorCommand& action) const {
        if (!config.enableCuriosity || curiosityLevel <= config.curiosityThreshold) {
            return;
        }
        
        float exploreChance = curiosityLevel * config.explorationProbability;
        float r = brain->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        
        if (r < exploreChance) {
            int choice = brain->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: action = MotorCommand::MoveForward; break;
                case 1: action = MotorCommand::MoveBackward; break;
                case 2: action = MotorCommand::TurnLeft; break;
                case 3: action = MotorCommand::TurnRight; break;
                case 4: action = MotorCommand::LookLeft; break;
                case 5: action = MotorCommand::LookRight; break;
                case 6: action = MotorCommand::Interact; break;
                default: action = MotorCommand::Wait; break;
            }
        }
    }
    
    void applyAdvancedRewardModulation(float reward, float predictedReward) {
        predictionError = reward - predictedReward;
        expectedReward = 0.95f * expectedReward + 0.05f * reward;
        dopamineLevel = predictionError;
        dopamineLevel = std::clamp(dopamineLevel, -1.0f, 1.0f);
        
        // Apply custom plasticity rule if enabled
        float plasticityFactor = 1.0f;
        if (config.enableCustomPlasticityRules && customPlasticityRule) {
            plasticityFactor = customPlasticityRule(dopamineLevel);
        } else {
            // Standard plasticity based on dopamine
            float plasticityBase = 0.5f + 0.5f * dopamineLevel;
            plasticityFactor = std::clamp(plasticityBase, 0.1f, 2.0f);
        }
        
        // Apply to all synapses
        for (const auto& region : brain->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                if (std::abs(eligibility) > 0.001f) {
                    float delta = eligibility * dopamineLevel * plasticityFactor;
                    syn->addToWeight(delta);
                    syn->decayEligibilityTrace(0.1f);
                }
            }
        }
        
        // Update STDP parameters
        auto* stdp = brain->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }
    
    void updateDevelopmentAdvanced(double timestep) {
        if (!config.enableDevelopment) return;
        
        developmentalAge += timestep;
        
        // Advanced developmental stages with smooth transitions
        if (developmentalAge < 60.0) {
            plasticityModifier = 1.0f + (developmentalAge / 60.0f) * 0.5f; // 1.0 to 1.5
            brain->setDevelopmentalStage(DevelopmentalStage::Initial);
        } else if (developmentalAge < 300.0) {
            float ageRatio = (developmentalAge - 60.0f) / 240.0f; // 0.0 to 1.0
            plasticityModifier = 0.8f + ageRatio * 0.4f; // 0.8 to 1.2
            brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (developmentalAge < 900.0) {
            float ageRatio = (developmentalAge - 300.0f) / 600.0f; // 0.0 to 1.0
            plasticityModifier = 0.5f + ageRatio * 0.7f; // 0.5 to 1.2
            brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else {
            float ageRatio = (developmentalAge - 900.0f) / 900.0f; // 0.0 to 1.0
            plasticityModifier = 0.2f + ageRatio * 0.8f; // 0.2 to 1.0
            brain->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
        
        plasticityModifier = std::clamp(plasticityModifier, 0.1f, 2.0f);
        
        // Structural plasticity with age-dependent rates
        if (config.enableStructuralPlasticity) {
            auto* sp = brain->getStructuralPlasticity();
            if (sp) {
                float synRate = 0.0001f * (1.5f - developmentalAge / 2000.0f);
                float pruneRate = 0.00001f * (1.0f + developmentalAge / 1000.0f);
                synRate = std::clamp(synRate, 0.00001f, 0.001f);
                pruneRate = std::clamp(pruneRate, 0.000001f, 0.0001f);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
    }
    
    std::string getDebugInfo() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3);
        oss << "=== AgentBrain Debug Info ===\n";
        oss << "Motor Groups: Fwd=" << motorForward.size() 
            << " Bkwd=" << motorBackward.size()
            << " L=" << motorTurnLeft.size()
            << " R=" << motorTurnRight.size()
            << " Int=" << motorInteract.size()
            << " Wait=" << motorWait.size() << "\n";
        oss << "Sensory Groups: Vision=" << sensoryVision.size()
            << " Touch=" << sensoryTouch.size()
            << " Internal=" << sensoryInternal.size()
            << " Proprioception=" << sensoryProprioception.size() << "\n";
        oss << "Neuromodulation: Dopamine=" << dopamineLevel
            << " Novelty=" << noveltyLevel
            << " Curiosity=" << curiosityLevel
            << " PredictionError=" << predictionError << "\n";
        oss << "Development: Age=" << developmentalAge << " Stage=" 
            << static_cast<int>(brain->getDevelopmentalStage())
            << " Plasticity=" << plasticityModifier << "\n";
        oss << "Performance: Cycles=" << processingCycles
            << " Cache Hits=" << cacheHits << " Misses=" << cacheMisses;
        oss << "\nCache Size: " << previousVision.size() << " vision samples";
        return oss.str();
    }
};

// AgentBrain implementation
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config)
    : pImpl_(new Impl(brain, config)), brain_(brain) {
    if (config.enableDebugLogging) {
        NLM_LOG_INFO(pImpl_->getDebugInfo());
    }
}

AgentBrain::~AgentBrain() {
    delete pImpl_;
}

AgentBrain::AgentBrain(AgentBrain&& other) noexcept
    : pImpl_(other.pImpl_), brain_(std::move(other.brain_)) {
    other.pImpl_ = nullptr;
}

AgentBrain& AgentBrain::operator=(AgentBrain&& other) noexcept {
    if (this != &other) {
        delete pImpl_;
        pImpl_ = other.pImpl_;
        brain_ = std::move(other.brain_);
        other.pImpl_ = nullptr;
    }
    return *this;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    pImpl_->previousVision.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    pImpl_->developmentalAge = 0.0;
    pImpl_->plasticityModifier = 1.0f;
    
    NLM_LOG_INFO("Enhanced AgentBrain initialized with " +
                 std::to_string(pImpl_->sensoryVision.size()) + " vision sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6; // Vision + touch + internal + proprioception
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6; // One per action type
}

void AgentBrain::process(const SensoryPercept& percept, double timestep) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    validateState();
    
    if (pImpl_->config.enableBatchProcessing) {
        pImpl_->applyBatchProcessing(percept, timestep);
    } else {
        pImpl_->processSingleSensory(percept);
    }
    
    // Update metrics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    pImpl_->processingCycles++;
    
    updateMetrics(static_cast<size_t>(duration.count()));
}

MotorCommand AgentBrain::getAction() const {
    MotorCommand action = pImpl_->selectMotorCommand();
    
    if (pImpl_->config.enableCuriosity) {
        pImpl_->updateWithCuriosity(action);
    }
    
    return action;
}

void AgentBrain::applyReward(float reward, float predictedReward) {
    if (!pImpl_->config.enableRewardModulation) return;
    
    pImpl_->applyAdvancedRewardModulation(reward, predictedReward);
    
    // Execute expert callbacks
    executeCustomCallbacks(std::vector<float>(), getAction(), reward, predictedReward);
}

void AgentBrain::updateDevelopment(double timestep) {
    if (pImpl_->config.enableDevelopment) {
        pImpl_->updateDevelopmentAdvanced(timestep);
    }
}

float AgentBrain::getNeuromodulationLevel() const {
    return pImpl_->dopamineLevel;
}

float AgentBrain::getCuriosityLevel() const {
    return pImpl_->curiosityLevel;
}

float AgentBrain::getNoveltyLevel() const {
    return pImpl_->noveltyLevel;
}

float AgentBrain::getPredictionError() const {
    return pImpl_->predictionError;
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!pImpl_->brain) return DevelopmentalStage::Initial;
    return pImpl_->brain->getDevelopmentalStage();
}

void AgentBrain::startBatchProcessing() {
    if (pImpl_->config.enableBatchProcessing) {
        pImpl_->batchBuffer.clear();
        pImpl_->batchResults.clear();
    }
}

void AgentBrain::addToBatch(const SensoryPercept& percept) {
    if (pImpl_->config.enableBatchProcessing) {
        pImpl_->batchBuffer.push_back(percept);
    }
}

std::vector<MotorCommand> AgentBrain::executeBatch(double timestep) {
    std::vector<MotorCommand> results;
    if (pImpl_->config.enableBatchProcessing && !pImpl_->batchBuffer.empty()) {
        pImpl_->processBatchInternal(timestep);
        results = std::move(pImpl_->batchResults);
    }
    return results;
}

void AgentBrain::endBatchProcessing() {
    pImpl_->batchBuffer.clear();
    pImpl_->batchResults.clear();
}

void AgentBrain::clearCaches() {
    pImpl_->previousVision.clear();
    pImpl_->cachedSensory_.clear();
    pImpl_->cachedAction_ = MotorCommand::Wait;
}

void AgentBrain::reset() {
    pImpl_->dopamineLevel = 0.0f;
    pImpl_->noveltyLevel = 0.0f;
    pImpl_->curiosityLevel = 0.0f;
    pImpl_->predictionError = 0.0f;
    pImpl_->expectedReward = 0.0f;
    pImpl_->developmentalAge = 0.0;
    pImpl_->plasticityModifier = 1.0f;
    std::fill(pImpl_->previousVision.begin(), pImpl_->previousVision.end(), 0.0f);
}

void AgentBrain::cleanup() {
    clearCaches();
    pImpl_->customPlasticityRule = nullptr;
}

void AgentBrain::validateState() const {
    if (!pImpl_->brain) {
        throw NLMException("Brain not initialized");
    }
    if (pImpl_->sensoryVision.empty() && pImpl_->config.enableDebugLogging) {
        NLM_LOG_WARN("Warning: No sensory neurons initialized");
    }
}

void AgentBrain::updateMetrics(size_t operationTime) const {
    if (pImpl_->config.enablePerformanceMetrics) {
        pImpl_->metrics_.sensoryProcessingTime += operationTime;
        pImpl_->metrics_.processingCycles++;
    }
}

MotorCommand AgentBrain::processWithAdvancedFeatures() const {
    return getAction();
}

void AgentBrain::processSensoryWithCaching(const SensoryPercept& percept) {
    if (pImpl_->config.enableCaching) {
        // Cache sensory processing results
        pImpl_->cachedSensory_.clear();
        // Implementation would cache processed sensory data
    }
}

void AgentBrain::neuromodulateWithCustomRules(float reward, float predictedReward) {
    pImpl_->applyAdvancedRewardModulation(reward, predictedReward);
}

MotorCommand AgentBrain::selectActionWithExploration() const {
    return getAction();
}

float AgentBrain::applyCustomPlasticity(SynapticWeight currentWeight) const {
    if (pImpl_->config.enableCustomPlasticityRules && pImpl_->customPlasticityRule) {
        return pImpl_->customPlasticityRule(currentWeight);
    }
    return 1.0f;
}

void AgentBrain::executeCustomCallbacks(const std::vector<float>& sensoryData, 
                                       MotorCommand action, float reward, float predictedReward) {
    if (pImpl_->config.onSensoryProcessed) {
        pImpl_->config.onSensoryProcessed(sensoryData);
    }
    if (pImpl_->config.onActionSelected) {
        pImpl_->config.onActionSelected(action);
    }
    if (pImpl_->config.onRewardReceived) {
        pImpl_->config.onRewardReceived(reward, predictedReward);
    }
}

std::string AgentBrain::getDebugInfo() const {
    std::string info = pImpl_->getDebugInfo();
    info += "\nEnhanced AgentBrain v2.0\n";
    info += "Config: RewardMod=" + std::string(pImpl_->config.enableRewardModulation ? "ON" : "OFF") +
            " Curiosity=" + std::string(pImpl_->config.enableCuriosity ? "ON" : "OFF") +
            " Development=" + std::string(pImpl_->config.enableDevelopment ? "ON" : "OFF") +
            " Caching=" + std::string(pImpl_->config.enableCaching ? "ON" : "OFF");
    return info;
}

} // namespace nlm
