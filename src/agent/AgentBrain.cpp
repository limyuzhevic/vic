#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <memory>

namespace nlm {

// =============================================================================
// AGENT BRAIN IMPLEMENTATION - IMPROVED MODULARITY
// =============================================================================

// ========= FORWARD DECLARATIONS =========
class MotorDecoder;
class SensoryProcessor;
class NeuromodulationProcessor;
class DevelopmentManager;
class ExplorationManager;

// ========= COMPONENT CLASSES ==========

/**
 * @brief Manages motor neuron group classification and activity calculation
 */
class MotorGroupManager {
public:
    MotorGroupManager() {
        groupNames_ = {"Forward", "Backward", "TurnLeft", "TurnRight", "Interact", "Wait"};
    }
    
    // Add neuron to appropriate motor group based on its position/index
    void registerNeuron(Neuron* neuron) {
        if (!neuron) return;
        
        size_t idx = getNextIndex();
        std::string group = groupNames_[idx % groupNames_.size()];
        
        auto& groupList = getGroup(group);
        groupList.push_back(neuron);
    }
    
    float getGroupActivity(const std::string& group) const {
        auto it = groupActivities_.find(group);
        if (it != groupActivities_.end()) {
            return it->second;
        }
        return 0.0f;
    }
    
    void updateGroupActivities(const std::unordered_map<std::string, std::vector<Neuron*>>& groups) {
        groupActivities_.clear();
        for (const auto& [group, neurons] : groups) {
            groupActivities_[group] = calculateActivity(neurons);
        }
    }
    
    const std::vector<Neuron*>& getGroup(const std::string& group) {
        static std::vector<Neuron*> empty;
        auto it = groups_.find(group);
        if (it != groups_.end()) {
            return it->second;
        }
        return empty;
    }
    
    std::vector<std::string> getGroupNames() const {
        return groupNames_;
    }
    
private:
    float calculateActivity(const std::vector<Neuron*>& neurons) const {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* neuron : neurons) {
            sum += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        }
        return sum / neurons.size();
    }
    
    size_t getNextIndex() {
        return nextIndex_++;
    }
    
    std::vector<std::string> groupNames_;
    std::unordered_map<std::string, std::vector<Neuron*>> groups_;
    std::unordered_map<std::string, float> groupActivities_;
    size_t nextIndex_ = 0;
};

/**
 * @brief Manages sensory input processing and transduction
 */
class SensoryProcessingManager {
public:
    SensoryProcessingManager() {
        // Define sensory modalities
        visionNeurons_.reserve(256);
        touchNeurons_.reserve(8);
        internalNeurons_.reserve(4);
        proprioceptionNeurons_.reserve(6);
    }
    
    // Register sensory neurons by type
    void registerVisionNeurons(const std::vector<Neuron*>& neurons) {
        visionNeurons_ = neurons;
        NLM_LOG_DEBUG(std::to_string(visionNeurons_.size()) + " vision neurons registered");
    }
    
    void registerTouchNeurons(const std::vector<Neuron*>& neurons) {
        touchNeurons_ = neurons;
        NLM_LOG_DEBUG(std::to_string(touchNeurons_.size()) + " touch neurons registered");
    }
    
    void registerInternalNeurons(const std::vector<Neuron*>& neurons) {
        internalNeurons_ = neurons;
        NLM_LOG_DEBUG(std::to_string(internalNeurons_.size()) + " internal neurons registered");
    }
    
    void registerProprioceptionNeurons(const std::vector<Neuron*>& neurons) {
        proprioceptionNeurons_ = neurons;
        NLM_LOG_DEBUG(std::to_string(proprioceptionNeurons_.size()) + " proprioception neurons registered");
    }
    
    // Process sensory percept and inject current into appropriate neurons
    void processVision(const std::vector<float>& visionData) {
        injectScaledCurrent(visionNeurons_, visionData, 5.0f, "vision");
    }
    
    void processTouch(const std::vector<float>& touchData) {
        injectScaledCurrent(touchNeurons_, touchData, 8.0f, "touch");
    }
    
    void processInternal(const std::vector<float>& internalData) {
        // Apply centering and scaling for internal signals
        std::vector<float> centeredData;
        centeredData.reserve(internalData.size());
        for (float val : internalData) {
            centeredData.push_back((val * 2.0f - 1.0f) * 5.0f);
        }
        injectScaledCurrent(internalNeurons_, centeredData, 1.0f, "internal");
    }
    
    void processProprioception(const std::vector<float>& proprioData) {
        // Apply centering and scaling for proprioception
        std::vector<float> centeredData;
        centeredData.reserve(proprioData.size());
        for (float val : proprioData) {
            centeredData.push_back((val * 2.0f - 1.0f) * 3.0f);
        }
        injectScaledCurrent(proprioceptionNeurons_, centeredData, 1.0f, "proprioception");
    }
    
    // Compute novelty from vision data
    float computeNovelty(const std::vector<float>& currentVision, float decayFactor) {
        if (currentVision.empty() || previousVision_.empty()) {
            previousVision_ = currentVision;
            return 0.0f;
        }
        
        float totalDiff = 0.0f;
        size_t minSize = std::min(currentVision.size(), previousVision_.size());
        for (size_t i = 0; i < minSize; ++i) {
            float diff = std::abs(currentVision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize and apply decay
        float novelty = totalDiff / std::max<size_t>(minSize, 1);
        novelty *= decayFactor;
        
        previousVision_ = currentVision;
        return novelty;
    }
    
    // Get total number of registered sensory neurons
    size_t getTotalSensoryNeuronCount() const {
        return visionNeurons_.size() + touchNeurons_.size() + 
               internalNeurons_.size() + proprioceptionNeurons_.size();
    }
    
    // Get expected sensory input sizes
    size_t getVisionNeuronCount() const { return visionNeurons_.size(); }
    size_t getTouchNeuronCount() const { return touchNeurons_.size(); }
    size_t getInternalNeuronCount() const { return internalNeurons_.size(); }
    size_t getProprioceptionNeuronCount() const { return proprioceptionNeurons_.size(); }
    
private:
    void injectScaledCurrent(const std::vector<Neuron*>& neurons, 
                           const std::vector<float>& data, 
                           float scaleFactor, 
                           const std::string& modality) {
        for (size_t i = 0; i < neurons.size() && i < data.size(); ++i) {
            if (neurons[i]) {
                float current = data[i] * scaleFactor;
                neurons[i]->injectCurrent(current);
                
                // Store in working memory for significant signals
                if (std::abs(current) > 0.5f && workingMemory_) {
                    workingMemory_->storeToNeuron(neurons[i]->getId(), std::abs(current) / 10.0f);
                }
            }
        }
        
        if (!neurons.empty()) {
            NLM_LOG_DEBUG(modality + " processed: " + std::to_string(neurons.size()) + 
                         " neurons with " + std::to_string(data.size()) + " data points");
        }
    }
    
    std::vector<Neuron*> visionNeurons_;
    std::vector<Neuron*> touchNeurons_;
    std::vector<Neuron*> internalNeurons_;
    std::vector<Neuron*> proprioceptionNeurons_;
    std::vector<float> previousVision_;
    NeuralWorkingMemory* workingMemory_ = nullptr;
};

/**
 * @brief Handles neuromodulation processing and effects
 */
class NeuromodulationManager {
public:
    NeuromodulationManager() {
        reset();
    }
    
    void update(float timestep) {
        // Update novelty and curiosity levels
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Apply exponential moving average to prediction error
        predictionError_ = 0.95f * predictionError_ + 0.05f * rewardPredictionError_;
        
        // Compute curiosity level based on novelty and prediction error
        if (enableCuriosity_) {
            curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        } else {
            curiosityLevel_ = 0.0f;
        }
        
        // Dopamine signal based on prediction error
        dopamineLevel_ = predictionError_;
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    }
    
    void applyRewardModulation(float reward, float predictedReward) {
        if (!enableRewardModulation_) return;
        
        rewardPredictionError_ = reward - predictedReward;
        expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
        
        // Apply neuromodulation to synapses
        applySynapticModulation();
        
        // Update plasticity factors
        updatePlasticityFactors();
    }
    
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getDopamineLevel() const { return dopamineLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }
    
    void enableCuriosity(bool enable) { enableCuriosity_ = enable; }
    void enableRewardModulation(bool enable) { enableRewardModulation_ = enable; }
    
    void setSensoryNoveltyDecay(float decay) { sensoryNoveltyDecay_ = decay; }
    
private:
    void applySynapticModulation() {
        if (!brain_) return;
        
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
    }
    
    void updatePlasticityFactors() {
        // Positive dopamine increases plasticity, negative decreases
        float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
        plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
        
        // Apply to STDP
        auto* stdp = brain_ ? brain_->getSTDP() : nullptr;
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }
    
    void reset() {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        rewardPredictionError_ = 0.0f;
        enableCuriosity_ = true;
        enableRewardModulation_ = true;
        sensoryNoveltyDecay_ = 0.99f;
    }
    
    // State
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float rewardPredictionError_;
    
    // Configuration
    bool enableCuriosity_;
    bool enableRewardModulation_;
    float sensoryNoveltyDecay_;
    
    // Dependencies
    Brain* brain_ = nullptr;
    
    friend class AgentBrain; // AgentBrain manages neuromodulation
};

/**
 * @brief Handles developmental progression and age-based effects
 */
class DevelopmentManager {
public:
    DevelopmentManager() {
        reset();
    }
    
    void update(double timestep) {
        developmentalAge_ += timestep;
        
        // Simple developmental stages based on age
        if (developmentalAge_ < 60.0) {  // ~1 minute
            setStage(DevelopmentalStage::Initial, 1.0f);
        } else if (developmentalAge_ < 300.0) {  // ~5 minutes
            setStage(DevelopmentalStage::CriticalPeriod, 0.8f);
        } else if (developmentalAge_ < 900.0) {  // ~15 minutes
            setStage(DevelopmentalStage::Maturation, 0.5f);
        } else {
            setStage(DevelopmentalStage::Adult, 0.2f);
        }
        
        // Update structural plasticity based on development
        if (enableStructuralPlasticity_) {
            updateStructuralPlasticityRates();
        }
    }
    
    DevelopmentalStage getCurrentStage() const { return currentStage_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    double getDevelopmentalAge() const { return developmentalAge_; }
    
    void enableDevelopment(bool enable) { enableDevelopment_ = enable; }
    void enableStructuralPlasticity(bool enable) { enableStructuralPlasticity_ = enable; }
    
private:
    void setStage(DevelopmentalStage stage, float modifier) {
        currentStage_ = stage;
        plasticityModifier_ = modifier;
        
        if (brain_) {
            brain_->setDevelopmentalStage(stage);
        }
        
        NLM_LOG_DEBUG(std::string("Development stage: ") + std::to_string(static_cast<int>(stage)) +
                     std::string(" (age=") + std::to_string(developmentalAge_) + ")");
    }
    
    void updateStructuralPlasticityRates() {
        auto* sp = brain_ ? brain_->getStructuralPlasticity() : nullptr;
        if (sp) {
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
    
    void reset() {
        developmentalAge_ = 0.0;
        currentStage_ = DevelopmentalStage::Initial;
        plasticityModifier_ = 1.0f;
        enableDevelopment_ = true;
        enableStructuralPlasticity_ = true;
    }
    
    // State
    double developmentalAge_;
    DevelopmentalStage currentStage_;
    float plasticityModifier_;
    
    // Configuration
    bool enableDevelopment_;
    bool enableStructuralPlasticity_;
    
    // Dependencies
    Brain* brain_ = nullptr;
    
    friend class AgentBrain; // AgentBrain manages development
};

/**
 * @brief Handles curiosity-driven exploration behavior
 */
class ExplorationManager {
public:
    ExplorationManager() {
        reset();
    }
    
    void update(float curiosityLevel, RandomGenerator* rng) {
        curiosityLevel_ = curiosityLevel;
        rng_ = rng;
    }
    
    MotorCommand selectExploratoryAction(MotorCommand defaultCmd) {
        // Exploration: occasionally choose random action when curiosity is high
        if (curiosityLevel_ > 0.5f) {
            float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
            
            if (rng_ && rng_->uniformReal(0.0f, 1.0f) < exploreChance) {
                // Random motor command from expanded set
                int choice = rng_->uniformInt(0, 11);  // Include LookLeft, LookRight
                return getRandomMotorCommand(choice);
            }
        }
        
        return defaultCmd;
    }
    
    MotorCommand getRandomMotorCommand(int choice) {
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            case 6: return MotorCommand::Interact;
            case 7: return MotorCommand::Eat;
            case 8: return MotorCommand::Drink;
            case 9: return MotorCommand::Rest;
            case 10: return MotorCommand::Wait;
            default: return MotorCommand::Custom;
        }
    }
    
    void setExplorationEnabled(bool enable) { enableExploration_ = enable; }
    
private:
    void reset() {
        curiosityLevel_ = 0.0f;
        enableExploration_ = true;
        rng_ = nullptr;
    }
    
    float curiosityLevel_;
    bool enableExploration_;
    RandomGenerator* rng_;
    
    friend class AgentBrain; // AgentBrain manages exploration
};

// =============================================================================
// AGENT BRAIN IMPLEMENTATION
// =============================================================================

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , motorDecoder_(std::make_unique<MotorDecoder>())
    , sensoryProcessor_(std::make_unique<SensoryProcessingManager>())
    , neuromodulation_(std::make_unique<NeuromodulationManager>())
    , development_(std::make_unique<DevelopmentManager>())
    , exploration_(std::make_unique<ExplorationManager>())
    , motorGroupManager_(std::make_unique<MotorGroupManager>()) {
    
    // Link dependencies
    neuromodulation_->brain_ = brain_.get();
    development_->brain_ = brain_.get();
    
    // Initialize motor and sensory neuron groups using the component manager
    if (brain_) {
        initializeNeuronGroups();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeNeuronGroups() {
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    motorGroupManager_->registerNeuron(n);
                }
            } else if (type == NeuronType::Sensory) {
                registerSensoryNeurons(pop->getNeurons());
            }
        }
    }
}

void AgentBrain::registerSensoryNeurons(const std::vector<Neuron*>& neurons) {
    // Distribute sensory neurons by type (simple heuristic based on neuron ID)
    size_t visionCount = 0, touchCount = 0, internalCount = 0, proprioceptionCount = 0;
    
    for (Neuron* neuron : neurons) {
        if (!neuron) continue;
        
        // Simple distribution based on neuron ID modulo 4
        int mod = neuron->getId().index() % 4;
        switch (mod) {
            case 0:
                sensoryProcessor_->registerVisionNeurons({neuron});
                visionCount++;
                break;
            case 1:
                sensoryProcessor_->registerTouchNeurons({neuron});
                touchCount++;
                break;
            case 2:
                sensoryProcessor_->registerInternalNeurons({neuron});
                internalCount++;
                break;
            case 3:
                sensoryProcessor_->registerProprioceptionNeurons({neuron});
                proprioceptionCount++;
                break;
        }
    }
    
    NLM_LOG_INFO(std::string("Sensory distribution: Vision=") + std::to_string(visionCount) +
                std::string(", Touch=") + std::to_string(touchCount) +
                std::string(", Internal=") + std::to_string(internalCount) +
                std::string(", Proprioception=") + std::to_string(proprioceptionCount));
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    development_->updateDevelopment(0.0);  // Initialize development
    neuromodulation_->setSensoryNoveltyDecay(sensoryNoveltyDecay_);
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                std::to_string(sensoryProcessor_->getVisionNeuronCount()) + " vision sensory neurons, " +
                std::to_string(sensoryProcessor_->getTouchNeuronCount()) + " touch sensory neurons, " +
                std::to_string(sensoryProcessor_->getInternalNeuronCount()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    return sensoryProcessor_->getTotalSensoryNeuronCount();
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action (including LookLeft, LookRight)
    return 8;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Process each sensory modality
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        sensoryProcessor_->processVision(vision);
    }
    
    const auto& touch = percept.getTouch();
    if (!touch.empty()) {
        sensoryProcessor_->processTouch(touch);
    }
    
    const auto& intern = percept.getInternal();
    if (!intern.empty()) {
        sensoryProcessor_->processInternal(intern);
    }
    
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty()) {
        sensoryProcessor_->processProprioception(proprio);
    }
    
    // Compute novelty for curiosity-driven exploration
    noveltyLevel_ = sensoryProcessor_->computeNovelty(vision, sensoryNoveltyDecay_);
    
    // Update neuromodulation with current levels
    neuromodulation_->update(0.001);  // Use timestep
    
    // Update curiosity level
    curiosityLevel_ = neuromodulation_->getCuriosityLevel();
    
    // Update development
    development_->update(0.001);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (exploration_->getExplorationEnabled() && curiosityLevel_ > 0.3f) {
        decoded = exploration_->selectExploratoryAction(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Update group activities for motor decoding
    auto groups = getMotorGroups();
    motorGroupManager_->updateGroupActivities(groups);
    
    // Calculate activity of motor neuron groups
    std::vector<std::pair<MotorCommand, float>> commandActivities;
    
    for (const auto& [command, activity] : getCommandActivities(groups)) {
        commandActivities.push_back({command, activity});
    }
    
    // Find maximum activity command
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.5f;  // Default threshold
    
    for (const auto& cmdActivity : commandActivities) {
        if (cmdActivity.second > bestActivity) {
            bestActivity = cmdActivity.second;
            best = cmdActivity.first;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

std::unordered_map<std::string, std::vector<Neuron*>> AgentBrain::getMotorGroups() {
    std::unordered_map<std::string, std::vector<Neuron*>> groups;
    
    // Extract motor groups by type using the component manager
    auto groupNames = motorGroupManager_->getGroupNames();
    for (const auto& groupName : groupNames) {
        groups[groupName] = motorGroupManager_->getGroup(groupName);
    }
    
    return groups;
}

std::vector<std::pair<MotorCommand, float>> AgentBrain::getCommandActivities(
    const std::unordered_map<std::string, std::vector<Neuron*>>& groups) {
    
    std::vector<std::pair<MotorCommand, float>> activities;
    
    // Map group names to motor commands
    std::unordered_map<std::string, MotorCommand> groupToCommand = {
        {"Forward", MotorCommand::MoveForward},
        {"Backward", MotorCommand::MoveBackward},
        {"TurnLeft", MotorCommand::TurnLeft},
        {"TurnRight", MotorCommand::TurnRight},
        {"Interact", MotorCommand::Interact},
        {"Wait", MotorCommand::Wait}
    };
    
    for (const auto& [groupName, neurons] : groups) {
        auto it = groupToCommand.find(groupName);
        if (it != groupToCommand.end()) {
            float activity = motorGroupManager_->getGroupActivity(groupName);
            activities.push_back({it->second, activity});
        }
    }
    
    return activities;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    neuromodulation_->applyRewardModulation(reward, predictedReward);
}

void AgentBrain::updateDevelopment(double timestep) {
    development_->update(timestep);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    return development_->getCurrentStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return neuromodulation_->getDopamineLevel();
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return neuromodulation_->getPredictionError();
}

void AgentBrain::reset() {
    neuromodulation_->reset();
    development_->reset();
    exploration_->reset();
    
    curiosityLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
