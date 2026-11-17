// AgentBrain Component Implementations
#include "AgentBrainNew.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// === SensoryProcessor Implementation ===
class SensoryProcessor {
public:
    SensoryProcessor(std::vector<Neuron*>& visionNeurons,
                    std::vector<Neuron*>& touchNeurons,
                    std::vector<Neuron*>& internalNeurons,
                    std::vector<Neuron*>& proprioceptionNeurons)
        : visionNeurons_(visionNeurons), touchNeurons_(touchNeurons),
          internalNeurons_(internalNeurons), proprioceptionNeurons_(proprioceptionNeurons) {}
    
    void processVision(const std::vector<float>& vision, std::vector<float>& predictedVision,
                       float& noveltyLevel, float& sensoryNoveltyDecay) {
        // Process vision input
        for (size_t i = 0; i < visionNeurons_.size() && i < vision.size(); ++i) {
            if (visionNeurons_[i]) {
                float current = vision[i] * 5.0f;
                visionNeurons_[i]->injectCurrent(current);
            }
        }
        
        // Update novelty detection
        if (!vision.empty()) {
            if (predictedVision.size() != vision.size()) {
                predictedVision.assign(vision.size(), 0.0f);
            }
            float totalDiff = 0.0f;
            for (size_t i = 0; i < vision.size(); ++i) {
                totalDiff += std::abs(vision[i] - predictedVision[i]);
            }
            noveltyLevel = totalDiff / std::max<size_t>(vision.size(), 1);
            noveltyLevel *= sensoryNoveltyDecay;
            
            constexpr float kPredictionAlpha = 0.05f;
            for (size_t i = 0; i < vision.size(); ++i) {
                predictedVision[i] += kPredictionAlpha * (vision[i] - predictedVision[i]);
            }
        }
    }
    
    void processTouch(const std::vector<float>& touch, float& maxTouch,
                     float& forwardTouch, float& forwardObjectVision,
                     const std::vector<float>& vision) {
        maxTouch = 0.0f;
        for (float t : touch) maxTouch = std::max(maxTouch, t);
        
        forwardTouch = 0.0f;
        if (touch.size() >= 8) {
            forwardTouch = std::max({touch[0], touch[1], touch[7]});
        } else {
            forwardTouch = maxTouch;
        }
        
        forwardObjectVision = 1.0f;
        if (touch.size() >= 8 && !vision.empty() && forwardTouch > 0.05f) {
            size_t touchDir = 0;
            float touchBest = touch[0];
            if (touch[1] > touchBest) { touchBest = touch[1]; touchDir = 1; }
            if (touch[7] > touchBest) { touchBest = touch[7]; touchDir = 7; }
            
            std::vector<size_t> rays;
            if (touchDir == 0) rays = {7, 8};
            else if (touchDir == 1) rays = {15};
            else rays = {0};
            
            for (size_t r : rays) {
                if (r < vision.size() && vision[r] < 0.99f) {
                    forwardObjectVision = std::min(forwardObjectVision, vision[r]);
                }
            }
        } else if (!vision.empty()) {
            for (size_t r = 5; r <= 10 && r < vision.size(); ++r) {
                if (vision[r] < 0.99f) {
                    forwardObjectVision = std::min(forwardObjectVision, vision[r]);
                }
            }
        }
        
        for (size_t i = 0; i < touchNeurons_.size() && i < touch.size(); ++i) {
            if (touchNeurons_[i]) {
                float current = touch[i] * 8.0f;
                touchNeurons_[i]->injectCurrent(current);
            }
        }
    }
    
    void processInternal(const std::vector<float>& internal) {
        for (size_t i = 0; i < internalNeurons_.size() && i < internal.size(); ++i) {
            if (internalNeurons_[i]) {
                float current = (internal[i] * 2.0f - 1.0f) * 5.0f;
                internalNeurons_[i]->injectCurrent(current);
            }
        }
    }
    
    void processProprioception(const std::vector<float>& proprioception) {
        for (size_t i = 0; i < proprioceptionNeurons_.size() && i < proprioception.size(); ++i) {
            if (proprioceptionNeurons_[i]) {
                float current = (proprioception[i] * 2.0f - 1.0f) * 3.0f;
                proprioceptionNeurons_[i]->injectCurrent(current);
            }
        }
    }
    
    void updateNovelty(const std::vector<float>& vision, std::vector<float>& predictedVision,
                      float& noveltyLevel, float sensoryNoveltyDecay) {
        if (!vision.empty()) {
            if (predictedVision.size() != vision.size()) {
                predictedVision.assign(vision.size(), 0.0f);
            }
            float totalDiff = 0.0f;
            for (size_t i = 0; i < vision.size(); ++i) {
                totalDiff += std::abs(vision[i] - predictedVision[i]);
            }
            noveltyLevel = totalDiff / std::max<size_t>(vision.size(), 1);
            noveltyLevel *= sensoryNoveltyDecay;
            
            constexpr float kPredictionAlpha = 0.05f;
            for (size_t i = 0; i < vision.size(); ++i) {
                predictedVision[i] += kPredictionAlpha * (vision[i] - predictedVision[i]);
            }
        }
    }
    
private:
    std::vector<Neuron*>& visionNeurons_;
    std::vector<Neuron*>& touchNeurons_;
    std::vector<Neuron*>& internalNeurons_;
    std::vector<Neuron*>& proprioceptionNeurons_;
};

// === MotorDecoder Implementation ===
class MotorDecoder {
public:
    MotorDecoder(std::vector<Neuron*>& forwardNeurons,
                std::vector<Neuron*>& backwardNeurons,
                std::vector<Neuron*>& leftNeurons,
                std::vector<Neuron*>& rightNeurons,
                std::vector<Neuron*>& interactNeurons,
                std::vector<Neuron*>& waitNeurons)
        : forwardNeurons_(forwardNeurons), backwardNeurons_(backwardNeurons),
          leftNeurons_(leftNeurons), rightNeurons_(rightNeurons),
          interactNeurons_(interactNeurons), waitNeurons_(waitNeurons) {}
    
    std::vector<AgentBrain::MotorGroupActivity> calculateMotorGroupActivities() const {
        auto calc = [](const std::vector<Neuron*>& neurons, const char* name) {
            AgentBrain::MotorGroupActivity out{name, 0.0f, 0, neurons.size()};
            if (neurons.empty()) return out;
            float sum = 0.0f;
            float rateSum = 0.0f;
            for (const Neuron* n : neurons) {
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
                if (n->getState().firingState == FiringState::Refractory) ++out.refractoryCount;
                rateSum += n->getFiringRate();
            }
            out.activity = sum / static_cast<float>(neurons.size());
            out.firingRate = rateSum / static_cast<float>(neurons.size());
            return out;
        };
        return {
            calc(forwardNeurons_, "forward"),
            calc(backwardNeurons_, "backward"),
            calc(leftNeurons_, "left"),
            calc(rightNeurons_, "right"),
            calc(interactNeurons_, "interact"),
            calc(waitNeurons_, "wait"),
        };
    }
    
    MotorCommand decodeFromMotorNeurons(const std::vector<AgentBrain::MotorGroupActivity>& activities,
                                        float forwardTouch, float forwardObjectVision,
                                        float maxTouch, float noveltyLevel,
                                        const Brain* brain, const std::vector<float>& lastVision,
                                        SelfModel& selfModel, SocialLearning& socialLearning,
                                        bool selfModelReady, bool socialReady,
                                        int& stepsSinceTurn, int& repeatCount,
                                        int& reflexBonusCount, int& reflexSuppressCount,
                                        int& wtaPulseCount, MotorCommand& lastDecoded,
                                        bool& hasLastDecoded) {
        // Calculate firing rates
        float forwardRate = activities[0].firingRate;
        float backwardRate = activities[1].firingRate;
        float turnLeftRate = activities[2].firingRate;
        float turnRightRate = activities[3].firingRate;
        float interactRate = activities[4].firingRate;
        
        // Apply curiosity-based exploration later in main class
        
        // Relative, spike-based decoding
        struct { MotorCommand cmd; float activity; } commands[] = {
            {MotorCommand::MoveForward, forwardRate},
            {MotorCommand::MoveBackward, backwardRate},
            {MotorCommand::TurnLeft, turnLeftRate},
            {MotorCommand::TurnRight, turnRightRate},
            {MotorCommand::Interact, interactRate},
        };
        
        MotorCommand best = MotorCommand::Wait;
        float bestActivity = 0.0f;
        for (const auto& c : commands) {
            if (c.activity > bestActivity) {
                bestActivity = c.activity;
                best = c.cmd;
            }
        }
        
        if (bestActivity <= 0.1f) return MotorCommand::Wait;
        
        // Update state
        if (hasLastDecoded && lastDecoded == MotorCommand::TurnLeft || lastDecoded == MotorCommand::TurnRight) {
            stepsSinceTurn_ = 0;
        } else {
            ++stepsSinceTurn_;
        }
        
        if (hasLastDecoded && best == lastDecoded &&
            (best == MotorCommand::MoveForward || best == MotorCommand::MoveBackward)) {
            ++repeatCount_;
        } else {
            repeatCount_ = 1;
        }
        
        lastDecoded = best;
        hasLastDecoded = true;
        
        return best;
    }
    
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd, float curiosityLevel,
                                    const RandomGenerator* rng) const {
        if (!curiosityLevel) return defaultCmd;
        
        float exploreChance = curiosityLevel * 0.3f;
        
        float r = rng->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            int choice = rng->uniformInt(uint32_t(0), uint32_t(7));
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
        
        return defaultCmd;
    }
    
private:
    std::vector<Neuron*>& forwardNeurons_;
    std::vector<Neuron*>& backwardNeurons_;
    std::vector<Neuron*>& leftNeurons_;
    std::vector<Neuron*>& rightNeurons_;
    std::vector<Neuron*>& interactNeurons_;
    std::vector<Neuron*>& waitNeurons_;
};

// === NeuromodulationController Implementation ===
class NeuromodulationController {
public:
    NeuromodulationController(Dopamine* dopamine, Novelty* novelty,
                             Curiosity* curiosity, PredictionError* predictionError)
        : dopamine_(dopamine), novelty_(novelty), curiosity_(curiosity), predictionError_(predictionError),
          dopamineLevel_(0.0f), curiosityLevel_(0.0f), noveltyLevel_(0.0f), predictionError_(0.0f) {}
    
    void applyRewardModulation(float reward, float predictedReward,
                               float& dopamineLevel, float& noveltyLevel,
                               float& curiosityLevel, float& predictionError,
                               float& expectedReward, float plasticityModifier,
                               STDP* stdp, const std::vector<Neuron*>& allNeurons) {
        // Compute prediction error
        predictionError = reward - predictedReward;
        
        // Update expected reward (exponential moving average)
        expectedReward = 0.95f * expectedReward + 0.05f * reward;
        
        // Dopamine-like signal (based on prediction error)
        float dopamineSignal = predictionError;
        dopamineSignal = std::clamp(dopamineSignal, -1.0f, 1.0f);
        
        dopamineLevel_ = dopamineSignal;
        dopamineLevel = dopamineSignal;
        
        // Apply to all synapses with eligibility traces
        for (const Neuron* neuron : allNeurons) {
            // Simplified neuromodulation effect
        }
        
        // Modulate plasticity based on dopamine
        float plasticityFactor = 0.5f + 0.5f * dopamineSignal;
        plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
        
        // Apply to STDP
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
        
        // Update novelty
        noveltyLevel_ = 0.5f * std::abs(predictionError);
        noveltyLevel = noveltyLevel_;
        
        // Update curiosity
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        curiosityLevel = curiosityLevel_;
    }
    
    void updateDevelopment(double timestep, double& developmentalAge,
                          float& plasticityModifier, StructuralPlasticity* structuralPlasticity,
                          DevelopmentalStage& developmentalStage) {
        developmentalAge_ += timestep;
        
        // Simple developmental stages based on age
        if (developmentalAge_ < 60.0) {  // ~1 minute
            plasticityModifier_ = 1.0f;
            developmentalStage = DevelopmentalStage::Initial;
        } else if (developmentalAge_ < 300.0) {  // ~5 minutes
            plasticityModifier_ = 0.8f;
            developmentalStage = DevelopmentalStage::CriticalPeriod;
        } else if (developmentalAge_ < 900.0) {  // ~15 minutes
            plasticityModifier_ = 0.5f;
            developmentalStage = DevelopmentalStage::Maturation;
        } else {
            plasticityModifier_ = 0.2f;  // Adult - more stable
            developmentalStage = DevelopmentalStage::Adult;
        }
        
        // Structural plasticity changes with development
        if (structuralPlasticity) {
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            structuralPlasticity->setSynaptogenesisRate(synRate);
            structuralPlasticity->setPruningRate(pruneRate);
        }
        
        dopamineLevel_ = developmentalAge_ / 900.0f;  // Simplified age-based dopamine
    }
    
    float getNeuromodulationLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    
private:
    Dopamine* dopamine_;
    Novelty* novelty_;
    Curiosity* curiosity_;
    PredictionError* predictionError_;
    
    float dopamineLevel_;
    float curiosityLevel_;
    float noveltyLevel_;
    float predictionError_;
};

// === CognitiveIntegration Implementation ===
class CognitiveIntegration {
public:
    CognitiveIntegration(Brain* brain, SelfModel& selfModel, SocialLearning& socialLearning,
                       ConceptFormation* conceptFormation, NeuralPlanner* planner)
        : brain_(brain), selfModel_(selfModel), socialLearning_(socialLearning),
          conceptFormation_(conceptFormation), planner_(planner) {}
    
    void applyCognitiveBiases(float& forwardRate, float& backwardRate, float& turnLeftRate,
                             float& turnRightRate, float& interactRate,
                             const std::vector<float>& lastVision, SelfModel& selfModel,
                             SocialLearning& socialLearning, bool selfModelReady,
                             bool socialReady, const std::vector<float>& conceptPattern,
                             ConceptFormation* conceptFormation, size_t conceptCount,
                             float selfModelConfidence, float maxTouch,
                             float avgReward, int& conceptBiasCount, int& selfBiasCount,
                             int& socialBiasCount) {
        // Phase 6 planner bias
        if (planner_ && !lastVision.empty()) {
            ActionType planned = planner_->planAction(lastVision);
            // Apply bias to motor rates based on planner
        }
        
        // Phase 6 concept bias
        if (conceptFormation_ && conceptCount > 0) {
            auto* foundConcept = conceptFormation_->getMatchingConcept(conceptPattern);
            if (foundConcept && !foundConcept->instances.empty()) {
                float avg = 0.0f;
                for (const auto& inst : foundConcept->instances) avg += inst.avgReward;
                avg /= static_cast<float>(foundConcept->instances.size());
                
                if (avg > 0.1f && maxTouch > 0.05f) { interactRate += 8.0f; ++conceptBiasCount; }
                else if (avg < -0.1f) {
                    interactRate = std::max(0.0f, interactRate - 8.0f);
                    turnLeftRate += 6.0f; turnRightRate += 6.0f;
                    ++conceptBiasCount_;
                }
            }
        }
        
        // Phase 6 self-model bias
        if (selfModelReady_ && selfModel.getSelfModelConfidence(ActionType::MoveForward) > 0.8f) {
            forwardRate += 3.0f; ++selfBiasCount_;
        }
        
        // Phase 6 social bias
        if (socialReady_ && socialLearning_.getObservationCount() > 0) {
            auto [imitAct, sim] = socialLearning_.getImitationActionAndSimilarity(lastVision);
            if (sim > 0.8f) {
                // Apply bias based on observed action
                ++socialBiasCount_;
            }
        }
    }
    
    std::string getThoughtTrace() const {
        return "Cognitive Integration Active";
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        return DevelopmentalStage::Adult;  // Simplified
    }
    
private:
    Brain* brain_;
    SelfModel& selfModel_;
    SocialLearning& socialLearning_;
    ConceptFormation* conceptFormation_;
    NeuralPlanner* planner_;
};

// === AgentBrain Method Implementations ===
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , lastWinnerGroup_(-1)
    , consecutiveWinnerCount_(0)
    , stepsSinceTurn_(0)
    , repeatCount_(1)
    , reflexBonusCount_(0)
    , reflexSuppressCount_(0)
    , wtaPulseCount_(0)
    , hasLastDecoded_(false)
    , hasThoughtPlanned_(false)
    , hasThoughtOutcome_(false)
    , plannerOverrideCount_(0)
    , plannerAgreementCount_(0)
    , plannerDecisionCount_(0)
    , conceptBiasCount_(0)
    , selfBiasCount_(0)
    , socialBiasCount_(0)
    , selfModelReady_(false)
    , socialReady_(false)
{
    // Initialize neuron groups from brain
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
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
                } else if (type == NeuronType::Internal) {
                    for (Neuron* n : pop->getNeurons()) {
                        internalNeurons_.push_back(n);
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
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
    
    // Initialize components
    sensoryProcessor_ = std::make_unique<SensoryProcessor>(
        sensoryVision_, sensoryTouch_, sensoryInternal_, sensoryProprioception_);
    
    motorDecoder_ = std::make_unique<MotorDecoder>(
        motorForward_, motorBackward_, motorTurnLeft_, motorTurnRight_,
        motorInteract_, motorWait_);
    
    neuromodulationController_ = std::make_unique<NeuromodulationController>(
        brain_->getDopamine(), brain_->getNovelty(),
        brain_->getCuriosity(), brain_->getPredictionError());
    
    cognitiveIntegration_ = std::make_unique<CognitiveIntegration>(
        brain_.get(), selfModel_, socialLearning_,
        brain_->getConceptFormation(), brain_->getPlanner());
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    predictedVision_.clear();
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // Phase 6 self-model integration
    if (hasLastDecoded_ && !lastVision_.empty() && !percept.getVision().empty()) {
        if (!selfModelReady_) { selfModel_.initialize(brain_.get()); selfModelReady_ = true; }
        ActionType act = ActionType::Wait;
        switch (lastDecoded_) {
            case MotorCommand::MoveForward: act = ActionType::MoveForward; break;
            case MotorCommand::MoveBackward: act = ActionType::MoveBackward; break;
            case MotorCommand::TurnLeft: act = ActionType::TurnLeft; break;
            case MotorCommand::TurnRight: act = ActionType::TurnRight; break;
            case MotorCommand::Interact: act = ActionType::Interact; break;
            default: break;
        }
        selfModel_.recordSelfAction(act, lastVision_, percept.getVision());
    }
    
    lastText_ = percept.getText();
    
    // Phase 5 winner-take-all lateral inhibition (simplified)
    if (lastWinnerGroup_ >= 0 && consecutiveWinnerCount_ < 3 &&
        lastWinnerMargin_ > 5.0f && noveltyLevel_ < 0.3f) {
        // Simplified WTA logic
        ++wtaPulseCount_;
    }
    
    // Process vision input
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * 5.0f;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Process text input
    if (!percept.getText().empty()) {
        for (size_t idx : encodeText(percept.getText())) {
            size_t neuronIdx = 16 + idx;
            if (neuronIdx < sensoryVision_.size() && sensoryVision_[neuronIdx]) {
                sensoryVision_[neuronIdx]->injectCurrent(8.0f);
            }
        }
    }
    
    // Process touch input
    const auto& touch = percept.getTouch();
    maxTouch_ = 0.0f;
    for (float t : touch) maxTouch_ = std::max(maxTouch_, t);
    
    // Calculate forward touch
    forwardTouch_ = 0.0f;
    if (touch.size() >= 8) {
        forwardTouch_ = std::max({touch[0], touch[1], touch[7]});
    } else {
        forwardTouch_ = maxTouch_;
    }
    
    // Calculate forward object vision
    forwardObjectVision_ = 1.0f;
    if (touch.size() >= 8 && !vision.empty() && forwardTouch_ > 0.05f) {
        float touchBest = touch[0];
        size_t touchDir = 0;
        if (touch[1] > touchBest) { touchBest = touch[1]; touchDir = 1; }
        if (touch[7] > touchBest) { touchBest = touch[7]; touchDir = 7; }
        
        std::vector<size_t> rays;
        if (touchDir == 0) rays = {7, 8};
        else if (touchDir == 1) rays = {15};
        else rays = {0};
        
        for (size_t r : rays) {
            if (r < vision.size() && vision[r] < 0.99f) {
                forwardObjectVision_ = std::min(forwardObjectVision_, vision[r]);
            }
        }
    }
    
    lastVision_ = vision;
    
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    for (size_t i = 0; i < sensoryInternal_.size() && i < percept.getInternal().size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (percept.getInternal()[i] * 2.0f - 1.0f) * 5.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    for (size_t i = 0; i < sensoryProprioception_.size() && i < percept.getProprioception().size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (percept.getProprioception()[i] * 2.0f - 1.0f) * 3.0f;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Update novelty detection
    noveltyLevel_ = 0.0f;  // Simplified
    curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f + 0.6f * curiosityLevel_;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    // Calculate motor group activities
    auto activities = motorDecoder_->calculateMotorGroupActivities();
    
    // Check if any motor neurons are firing
    float maxRaw = 0.0f;
    for (size_t i = 0; i < 5 && i < activities.size(); ++i) {
        maxRaw = std::max(maxRaw, activities[i].firingRate);
    }
    
    if (maxRaw <= 0.1f) return MotorCommand::Wait;
    
    // Apply cognitive biases
    float forwardRate = activities[0].firingRate;
    float backwardRate = activities[1].firingRate;
    float turnLeftRate = activities[2].firingRate;
    float turnRightRate = activities[3].firingRate;
    float interactRate = activities[4].firingRate;
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_) {
        // Simplified curiosity selection
        float exploreChance = curiosityLevel_ * 0.3f;
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            int choice = brain_->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(7));
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
    
    // Select best action
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;
    for (int i = 0; i < 5 && i < static_cast<int>(activities.size()); ++i) {
        if (activities[i].firingRate > bestActivity) {
            bestActivity = activities[i].firingRate;
            switch (i) {
                case 0: best = MotorCommand::MoveForward; break;
                case 1: best = MotorCommand::MoveBackward; break;
                case 2: best = MotorCommand::TurnLeft; break;
                case 3: best = MotorCommand::TurnRight; break;
                case 4: best = MotorCommand::Interact; break;
            }
        }
    }
    
    // Update state
    if (hasLastDecoded_ && best == lastDecoded_ &&
        (best == MotorCommand::MoveForward || best == MotorCommand::MoveBackward)) {
        ++repeatCount_;
    } else {
        repeatCount_ = 1;
    }
    
    lastDecoded_ = best;
    hasLastDecoded_ = true;
    
    return best;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // Simplified reward modulation
    predictionError_ = reward - predictedReward;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal
    float dopamineLevel = predictionError_;
    dopamineLevel = std::clamp(dopamineLevel, -1.0f, 1.0f);
    
    // Apply to brain
    neuromodulationController_->applyRewardModulation(
        reward, predictedReward, dopamineLevel_, curiosityLevel_, noveltyLevel_,
        predictionError_, expectedReward_, plasticityModifier_,
        brain_->getSTDP(), brain_->getTotalNeurons());
}

void AgentBrain::updateDevelopment(double timestep) {
    neuromodulationController_->updateDevelopment(
        timestep, developmentalAge_, plasticityModifier_,
        brain_->getStructuralPlasticity(), developmentalStage_);
}

float AgentBrain::getNeuromodulationLevel() const {
    return neuromodulationController_->getNeuromodulationLevel();
}

float AgentBrain::getCuriosityLevel() const {
    return neuromodulationController_->getCuriosityLevel();
}

float AgentBrain::getNoveltyLevel() const {
    return neuromodulationController_->getNoveltyLevel();
}

float AgentBrain::getPredictionError() const {
    return neuromodulationController_->getPredictionError();
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    return neuromodulationController_->getDevelopmentalStage();
}

std::string AgentBrain::getThoughtTrace() const {
    return "Sensory:" + thoughtPercept_ + ", Planned:" + std::to_string(static_cast<int>(thoughtPlanned_)) +
           ", Reward:" + std::to_string(thoughtReward_);
}

float AgentBrain::getLearnedPreference() const {
    return brain_->getLearnedPreference();
}

size_t AgentBrain::getConceptCount() const {
    return brain_->getConceptCount();
}

float AgentBrain::getSelfModelConfidence() const {
    return selfModel_.getSelfModelConfidence(ActionType::MoveForward);
}

size_t AgentBrain::getSocialObservationCount() const {
    return socialLearning_.getObservationCount();
}

std::vector<size_t> AgentBrain::encodeText(const std::string& text) {
    std::string norm;
    for (char c : text) {
        if (c == ' ' || c == '\t' || c == '\n') continue;
        norm += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (norm.empty()) return {};
    uint64_t h = 1469598103934665603ULL;
    for (char c : norm) {
        h ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        h *= 1099511628211ULL;
    }
    std::vector<size_t> out;
    uint64_t state = h;
    while (out.size() < 4) {
        state += 0x9e3779b97f4a7c15ULL;
        uint64_t z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        z = z ^ (z >> 31);
        size_t idx = static_cast<size_t>(z % 15);
        if (std::find(out.begin(), out.end(), idx) == out.end()) out.push_back(idx);
    }
    return out;
}

std::vector<float> AgentBrain::getInternalFiringRates() const {
    std::vector<float> rates;
    for (const Neuron* n : internalNeurons_) {
        rates.push_back(n ? n->getFiringRate() : 0.0f);
    }
    return rates;
}

void AgentBrain::reset() {
    predictedVision_.clear();
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    lastWinnerGroup_ = -1;
    consecutiveWinnerCount_ = 0;
    stepsSinceTurn_ = 0;
    repeatCount_ = 1;
    
    reflexBonusCount_ = 0;
    reflexSuppressCount_ = 0;
    wtaPulseCount_ = 0;
    
    lastDecoded_ = MotorCommand::Wait;
    hasLastDecoded_ = false;
    
    lastVision_.clear();
    thoughtPercept_ = "none";
    thoughtPlanned_ = MotorCommand::Wait;
    hasThoughtPlanned_ = false;
    thoughtReward_ = 0.0f;
    hasThoughtOutcome_ = false;
}

} // namespace nlm
