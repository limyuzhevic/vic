#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib> // For rand()

namespace nlm {

// MotorCommandSelector - Handles motor decoding logic
class MotorCommandSelector {
public:
    MotorCommandSelector() : curiosityThreshold_(0.3f), minimumActivity_(0.5f) {}
    
    MotorCommand decodeFromMotorNeurons(const std::vector<Neuron*>& motorForward,
                                       const std::vector<Neuron*>& motorBackward,
                                       const std::vector<Neuron*>& motorTurnLeft,
                                       const std::vector<Neuron*>& motorTurnRight,
                                       const std::vector<Neuron*>& motorInteract,
                                       const std::vector<Neuron*>& motorWait) {
        auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
            if (neurons.empty()) return 0.0f;
            float sum = 0.0f;
            for (Neuron* n : neurons) {
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
            return sum / neurons.size();
        };
        
        float forwardAct = calcActivity(motorForward);
        float backwardAct = calcActivity(motorBackward);
        float leftAct = calcActivity(motorTurnLeft);
        float rightAct = calcActivity(motorTurnRight);
        float interactAct = calcActivity(motorInteract);
        float waitAct = calcActivity(motorWait);
        
        // Find maximum activity
        struct CommandWithActivity {
            MotorCommand cmd;
            float activity;
        };
        
        CommandWithActivity commands[] = {
            {MotorCommand::MoveForward, forwardAct},
            {MotorCommand::MoveBackward, backwardAct},
            {MotorCommand::TurnLeft, leftAct},
            {MotorCommand::TurnRight, rightAct},
            {MotorCommand::Interact, interactAct},
            {MotorCommand::Wait, waitAct}
        };
        
        MotorCommand best = MotorCommand::Wait;
        float bestActivity = waitAct;  // Default to wait if nothing stronger
        
        for (const auto& c : commands) {
            if (c.activity > bestActivity) {
                bestActivity = c.activity;
                best = c.cmd;
            }
        }
        
        // Only act if there's meaningful activity
        if (bestActivity < minimumActivity_) {
            return MotorCommand::Wait;
        }
        
        return best;
    }
    
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    
    float getMinimumActivity() const { return minimumActivity_; }
    void setMinimumActivity(float activity) { minimumActivity_ = activity; }
    
private:
    float curiosityThreshold_;
    float minimumActivity_;
};

// NoveltyDetector - Handles novelty detection logic
class NoveltyDetector {
public:
    NoveltyDetector(float decayRate = 0.99f, size_t historySize = 256) 
        : noveltyDecay_(decayRate), maxHistorySize_(historySize) {}
    
    float computeNovelty(const std::vector<float>& currentVision,
                        std::vector<float>& previousVision,
                        float& noveltyLevel) {
        if (currentVision.empty()) {
            return 0.0f;
        }
        
        // Resize previous vision if needed
        if (previousVision.size() < currentVision.size()) {
            previousVision.resize(currentVision.size(), 0.0f);
        }
        
        // Compute novelty (difference from previous vision)
        float totalDiff = 0.0f;
        for (size_t i = 0; i < currentVision.size(); ++i) {
            float diff = std::abs(currentVision[i] - previousVision[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel = totalDiff / std::max<size_t>(currentVision.size(), 1);
        
        // Decay and update previous vision
        noveltyLevel *= noveltyDecay_;
        previousVision = currentVision;
        
        return noveltyLevel;
    }
    
    float getDecayRate() const { return noveltyDecay_; }
    void setDecayRate(float rate) { noveltyDecay_ = rate; }
    
    size_t getMaxHistorySize() const { return maxHistorySize_; }
    void setMaxHistorySize(size_t size) { maxHistorySize_ = size; }
    
private:
    float noveltyDecay_;
    size_t maxHistorySize_;
};

// CuriosityEngine - Handles curiosity-driven exploration
class CuriosityEngine {
public:
    CuriosityEngine(float noveltyWeight = 2.0f, float errorWeight = 0.5f,
                   float maxCuriosity = 1.0f, float explorationChance = 0.3f)
        : noveltyWeight_(noveltyWeight), errorWeight_(errorWeight),
          maxCuriosity_(maxCuriosity), explorationChance_(explorationChance) {}
    
    float calculateCuriosity(float noveltyLevel, float predictionError) {
        float curiosity = noveltyLevel * noveltyWeight_ + 
                         std::abs(predictionError) * errorWeight_;
        return std::clamp(curiosity, 0.0f, maxCuriosity_);
    }
    
    bool shouldExplore(float curiosityLevel) {
        if (curiosityLevel <= 0.5f) return false;
        
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel * explorationChance_;
        
        // This would use brain random generator in real implementation
        // For now, use simple random
        return (static_cast<float>(rand()) / RAND_MAX) < exploreChance;
    }
    
    MotorCommand selectRandomAction() {
        int choice = rand() % 8;  // 8 different actions
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            case 6: return MotorCommand::Interact;
            default: return MotorCommand::Wait;
        }
    }
    
    float getNoveltyWeight() const { return noveltyWeight_; }
    void setNoveltyWeight(float weight) { noveltyWeight_ = weight; }
    
    float getErrorWeight() const { return errorWeight_; }
    void setErrorWeight(float weight) { errorWeight_ = weight; }
    
    float getMaxCuriosity() const { return maxCuriosity_; }
    void setMaxCuriosity(float max) { maxCuriosity_ = max; }
    
    float getExplorationChance() const { return explorationChance_; }
    void setExplorationChance(float chance) { explorationChance_ = chance; }
    
private:
    float noveltyWeight_;
    float errorWeight_;
    float maxCuriosity_;
    float explorationChance_;
};

// AgentBrain implementation using components
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , motorSelector_(new MotorCommandSelector())
    , noveltyDetector_(new NoveltyDetector())
    , curiosityEngine_(new CuriosityEngine())
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() {
    // Clean up component objects
    delete motorSelector_;
    delete noveltyDetector_;
    delete curiosityEngine_;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty using dedicated detector
    if (!vision.empty()) {
        noveltyDetector_->computeNovelty(vision, previousVision_, noveltyLevel_);
    }
    
    // Update curiosity using dedicated engine
    if (curiosityEnabled_) {
        curiosityLevel_ = curiosityEngine_->calculateCuriosity(noveltyLevel_, predictionError_);
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Store vision for next novelty computation
    previousVision_ = vision;
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration using dedicated engine
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Delegate to component
    return motorSelector_->decodeFromMotorNeurons(
        motorForward_, motorBackward_, motorTurnLeft_, 
        motorTurnRight_, motorInteract_, motorWait_);
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Delegate to component
    if (curiosityEngine_->shouldExplore(curiosityLevel_)) {
        return curiosityEngine_->selectRandomAction();
    }
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm