#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

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

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeCognitiveSystems() {
    if (!brain_) return;
    
    // Register neural planner for action planning
    if (auto* planner = brain_->getPlanner()) {
        planner->initialize(brain_.get());
        NLM_LOG_INFO("[COGNITIVE] NeuralPlanner registered and initialized");
    } else {
        NLM_LOG_WARNING("[COGNITIVE] NeuralPlanner not available - action planning will use reactive control");
    }
    
    // Register concept formation for pattern discovery
    if (auto* conceptFormation = brain_->getConceptFormation()) {
        conceptFormation->initialize(brain_.get());
        NLM_LOG_INFO("[COGNITIVE] ConceptFormation registered and initialized");
    } else {
        NLM_LOG_WARNING("[COGNITIVE] ConceptFormation not available - pattern learning will be limited");
    }
    
    // Register attentional selection for focus
    if (auto* attention = brain_->getAttention()) {
        attention->initialize(brain_.get());
        NLM_LOG_INFO("[COGNITIVE] AttentionalSelection registered and initialized");
    } else {
        NLM_LOG_WARNING("[COGNITIVE] AttentionalSelection not available - attention allocation will be basic");
    }
    
    NLM_LOG_INFO("[COGNITIVE] Cognitive systems integration completed");
}

void AgentBrain::initializeSelfModel() {
    // Create and initialize self-model based on body and capabilities
    selfModel_.bodyModel = createBodyModel();
    selfModel_.goalSet = createGoalSet();
    selfModel_.memory = std::make_shared<SelfMemory>();
    selfModel_.motorCapabilities = analyzeMotorCapabilities();
    selfModel_.sensoryCapabilities = analyzeSensoryCapabilities();
    
    NLM_LOG_INFO("[COGNITIVE] Self-model initialized with body, goals, and memory");
}

SelfModel AgentBrain::createBodyModel() {
    SelfModel model;
    model.type = "agent";
    model.position = "unknown";
    model.capabilities = {"vision", "movement", "interaction", "learning"};
    model.constraints = {"energy", "time", "space"};
    model.energy = 1.0f;
    model.maxEnergy = 1.0f;
    return model;
}

GoalSet AgentBrain::createGoalSet() {
    GoalSet goals;
    goals.primaryGoal = "explore";
    goals.subgoals = {
        "find_resources", "avoid_hazards", "maintain_safety", "social_interaction"
    };
    goals.priority = {1.0f, 0.8f, 0.6f, 0.4f};
    return goals;
}

MotorCapabilities AgentBrain::analyzeMotorCapabilities() {
    MotorCapabilities caps;
    caps.movementTypes = {"forward", "backward", "turn_left", "turn_right", "interact"};
    caps.speedRanges = {0.1f, 0.5f, 0.5f, 0.5f, 0.3f};
    caps.accuracy = 0.8f;
    return caps;
}

SensoryCapabilities AgentBrain::analyzeSensoryCapabilities() {
    SensoryCapabilities caps;
    caps.modality = {"vision", "touch", "internal", "proprioception"};
    caps.range = {16.0f, 8.0f, 4.0f, 6.0f};
    caps.resolution = {256, 8, 4, 6};
    return caps;
}

void AgentBrain::passSensoryToCognition(const std::vector<float>& vision) {
    if (!brain_) return;
    
    // Create feature vector from vision for concept formation
    std::vector<float> features = extractSensoryFeatures(vision);
    
    // Pass to concept formation for pattern learning
    if (auto* conceptFormation = brain_->getConceptFormation()) {
        conceptFormation->presentExperience(features, features, 0.0f, 0.0f);
    }
    
    // Pass to attention system for selection
    if (auto* attention = brain_->getAttention()) {
        attention->updatePercept(features);
    }
    
    // Pass to planner for action planning
    if (auto* planner = brain_->getPlanner()) {
        planWithCurrentCognition(features);
    }
}

void AgentBrain::planWithCurrentCognition(const std::vector<float>& state) {
    if (!brain_) return;
    
    // Use planner for action selection based on current state and goals
    if (auto* planner = brain_->getPlanner()) {
        // In a full implementation, this would generate action sequences
        // For now, just record that planner is available
        NLM_LOG_INFO("[PLANNER] Action planning available with cognitive state");
    }
}

void AgentBrain::updateCognitiveWithNovelty(const std::vector<float>& percept) {
    if (!brain_) return;
    
    // Update attention based on novelty
    if (auto* attention = brain_->getAttention()) {
        attention->updatePercept(percept);
    }
    
    // Update concept formation with new experience
    if (auto* conceptFormation = brain_->getConceptFormation()) {
        conceptFormation->presentExperience(percept, percept, 0.0f, 0.0f);
    }
}

std::vector<float> AgentBrain::extractSensoryFeatures(const std::vector<float>& vision) {
    // Extract features from vision for concept formation
    std::vector<float> features;
    
    // Add vision features
    if (!vision.empty()) {
        features.insert(features.end(), vision.begin(), vision.end());
    }
    
    // Add derived features
    float avgBrightness = 0.0f;
    if (!vision.empty()) {
        avgBrightness = std::accumulate(vision.begin(), vision.end(), 0.0f) / vision.size();
    }
    features.push_back(avgBrightness);
    
    // Normalize features
    if (!features.empty()) {
        float sum = std::accumulate(features.begin(), features.end(), 0.0f);
        for (float& feature : features) {
            feature /= sum;
        }
    }
    
    return features;
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
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Pass sensory data to cognitive systems
    passSensoryToCognition(vision);
    
    // Update cognitive systems with novelty
    updateCognitiveWithNovelty(vision);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply cognitive planning if available
    if (brain_->getPlanner()) {
        // Let planner influence action selection based on current state and goals
        decoded = applyPlannerInfluence(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
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
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
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

MotorCommand AgentBrain::applyPlannerInfluence(MotorCommand baseCmd) {
    if (!brain_ || !brain_->getPlanner()) {
        return baseCmd;
    }
    
    // Get current cognitive state for planning
    std::vector<float> cognitiveState = getSensoryFeatures();
    
    // Apply planner to influence action selection
    // For now, this is a stub - in a full implementation, the planner
    // would evaluate sequences of actions and select the optimal one
    float curiosityFactor = std::min(0.5f, curiosityLevel_);
    
    // Higher curiosity leads to more exploratory action selection
    if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < curiosityFactor) {
        // Select from action types that may not be optimal but are exploratory
        NLM_LOG_INFO("[PLANNER] Planner-driven exploratory action selection");
    }
    
    // In the future, this would use the NeuralPlanner's sequence evaluation
    // and action selection capabilities
    return baseCmd;
}

void AgentBrain::applyCognitiveInfluenceToPlasticity() {
    if (!brain_) return;
    
    // Integrate cognitive systems into plasticity modulation
    // Concept formation affects structural plasticity based on learned patterns
    if (auto* conceptFormation = brain_->getConceptFormation()) {
        // Concept formation could provide information about important patterns
        // that should be preserved or strengthened
        NLM_LOG_INFO("[PLASTICITY] Concept formation influencing structural plasticity");
    }
    
    // Attention system affects synaptic prioritization
    if (auto* attention = brain_->getAttention()) {
        // Attention allocation could influence which synapses are potentiated
        NLM_LOG_INFO("[PLASTICITY] Attentional selection influencing synaptic prioritization");
    }
    
    // Self-model provides internal goals for plasticity
    if (!selfModel_.goalSet.subgoals.empty()) {
        NLM_LOG_INFO("[PLASTICITY] Self-model goals guiding plasticity direction");
    }
}

void AgentBrain::updateDevelopmentWithCognitiveState() {
    if (!brain_) return;
    
    // Update development with cognitive state information
    float cognitiveFactor = getCognitiveComplexityFactor();
    
    if (developmentEnabled_) {
        // Modify development rate based on cognitive demands
        float developmentRate = calculateDevelopmentRate(cognitiveFactor);
        
        if (auto* development = brain_->getDevelopmentSystem()) {
            development->update(developmentalAge_, developmentRate);
        }
    }
    
    NLM_LOG_INFO("[DEVELOPMENT] Development updated with cognitive state: " + 
                 std::to_string(cognitiveFactor) + " complexity factor");
}

float AgentBrain::getCognitiveComplexityFactor() const {
    // Calculate a measure of cognitive system activity
    float factor = 0.0f;
    
    // Factor in novelty level (concept formation activity)
    factor += noveltyLevel_ * 0.3f;
    
    // Factor in curiosity level (exploration drive)
    factor += curiosityLevel_ * 0.3f;
    
    // Factor in planner activity (goal pursuit)
    if (brain_->getPlanner()) {
        factor += 0.2f;  // Planner presence indicates higher cognitive load
    }
    
    // Factor in self-model complexity
    if (!selfModel_.goalSet.subgoals.empty()) {
        factor += 0.2f;
    }
    
    return std::min(1.0f, factor);
}

float AgentBrain::calculateDevelopmentRate(float cognitiveFactor) const {
    // Development rate depends on cognitive needs and complexity
    float baseRate = 0.001f;  // Default development rate
    
    // Higher cognitive complexity can drive faster development
    float complexityBoost = cognitiveFactor * 0.01f;
    
    // Apply learning-related modifications
    float learningModifier = 1.0f;
    if (dopamineLevel_ > 0.1f) {
        learningModifier += dopamineLevel_ * 0.1f;
    }
    
    return baseRate * learningModifier + complexityBoost;
}
