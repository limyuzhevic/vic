#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Error/ErrorHandling.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , brainConfig_(brain ? brain->getConfig() : nullptr)
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
    , initialized_(false)
{
    NLM_CHECK_CONDITION(brain, "Brain pointer cannot be null", "brain");
    
    if (brainConfig_) {
        validateBrainState();
    }
    
    initializeNeuronGroups();
    initialized_ = true;
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    NLM_CHECK_CONDITION(world.getVisionWidth() > 0 && world.getVisionHeight() > 0, 
                       "Invalid world dimensions", "world");
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryGroups_.vision.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryGroups_.touch.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryGroups_.internal.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input: Brain is null");
        return;
    }
    
    NLM_CHECK_CONDITION(percpt.getVision().size() <= 256, 
                       "Vision input size exceeds expected maximum", "vision_input");
    
    processVisionInput(percpt.getVision());
    processTouchInput(percpt.getTouch());
    processInternalInput(percpt.getInternal());
    processProprioceptionInput(percpt.getProprioception());
    
    computeNovelty(percpt.getVision());
    updateCuriosityBasedOnNovelty();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input: Brain is null");
        return;
    }
    
    NLM_CHECK_CONDITION(percpt.getVision().size() <= 256, 
                       "Vision input size exceeds expected maximum", "vision_input");
    
    processVisionInput(percpt.getVision());
    processTouchInput(percpt.getTouch());
    processInternalInput(percpt.getInternal());
    processProprioceptionInput(percpt.getProprioception());
    
    computeNovelty(percpt.getVision());
    updateCuriosityBasedOnNovelty();
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryGroups_.vision.size() && i < vision.size(); ++i) {
        NLM_CHECK_CONDITION(sensoryGroups_.vision[i], "Vision sensory neuron cannot be null", "sensory_neurons");
        float current = vision[i] * 5.0f;
        sensoryGroups_.vision[i]->injectCurrent(current);
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryGroups_.touch.size() && i < touch.size(); ++i) {
        NLM_CHECK_CONDITION(sensoryGroups_.touch[i], "Touch sensory neuron cannot be null", "sensory_neurons");
        float current = touch[i] * 8.0f;
        sensoryGroups_.touch[i]->injectCurrent(current);
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < sensoryGroups_.internal.size() && i < internal.size(); ++i) {
        NLM_CHECK_CONDITION(sensoryGroups_.internal[i], "Internal sensory neuron cannot be null", "sensory_neurons");
        float current = (internal[i] * 2.0f - 1.0f) * 5.0f;
        sensoryGroups_.internal[i]->injectCurrent(current);
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryGroups_.proprioception.size() && i < proprio.size(); ++i) {
        NLM_CHECK_CONDITION(sensoryGroups_.proprioception[i], "Proprioception sensory neuron cannot be null", "sensory_neurons");
        float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
        sensoryGroups_.proprioception[i]->injectCurrent(current);
    }
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        previousVision_ = vision;
    }
}

void AgentBrain::updateCuriosityBasedOnNovelty() {
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    MotorCommand decoded = decodeFromMotorNeurons();
    
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorGroups_.forward);
    float backwardAct = calcActivity(motorGroups_.backward);
    float leftAct = calcActivity(motorGroups_.turnLeft);
    float rightAct = calcActivity(motorGroups_.turnRight);
    float interactAct = calcActivity(motorGroups_.interact);
    float waitAct = calcActivity(motorGroups_.wait);
    
    const MotorCommand commands[] = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward,
        MotorCommand::TurnLeft, MotorCommand::TurnRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    
    const float activities[] = {forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct};
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (size_t i = 0; i < 6; ++i) {
        if (activities[i] > bestActivity) {
            bestActivity = activities[i];
            best = commands[i];
        }
    }
    
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (curiosityLevel_ > 0.5f) {
        float exploreChance = curiosityLevel_ * 0.3f;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
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
    
    predictionError_ = reward - predictedReward;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    updateRewardModulatedWeights();
    modulatePlasticityBasedOnDopamine();
}

void AgentBrain::updateRewardModulatedWeights() {
    if (std::abs(predictionError_) > 0.001f) {
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > 0.001f) {
                    float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                    syn->addToWeight(delta);
                    syn->decayEligibilityTrace(0.1f);
                }
            }
        }
    }
}

void AgentBrain::modulatePlasticityBasedOnDopamine() {
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    updateDevelopmentalStage();
    updateDevelopmentPlasticityRates();
}

void AgentBrain::updateDevelopmentalStage() {
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

void AgentBrain::updateDevelopmentPlasticityRates() {
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
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
    
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

void AgentBrain::initializeNeuronGroups() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        NLM_CHECK_CONDITION(region, "Region cannot be null", "brain_regions");
        
        for (auto& pop : region->getPopulations()) {
            NLM_CHECK_CONDITION(pop, "Population cannot be null", "brain_populations");
            
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                distributeMotorNeurons(pop->getNeurons());
            } else if (type == NeuronType::Sensory) {
                distributeSensoryNeurons(pop->getNeurons());
            }
        }
    }
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        NLM_CHECK_CONDITION(n, "Motor neuron cannot be null", "brain_neurons");
        size_t idx = motorGroups_.totalCount();
        
        switch (idx % 6) {
            case 0: motorGroups_.forward.push_back(n); break;
            case 1: motorGroups_.backward.push_back(n); break;
            case 2: motorGroups_.turnLeft.push_back(n); break;
            case 3: motorGroups_.turnRight.push_back(n); break;
            case 4: motorGroups_.interact.push_back(n); break;
            case 5: motorGroups_.wait.push_back(n); break;
        }
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        NLM_CHECK_CONDITION(n, "Sensory neuron cannot be null", "brain_neurons");
        size_t idx = sensoryGroups_.totalCount();
        
        switch (idx % 4) {
            case 0: sensoryGroups_.vision.push_back(n); break;
            case 1: sensoryGroups_.touch.push_back(n); break;
            case 2: sensoryGroups_.internal.push_back(n); break;
            case 3: sensoryGroups_.proprioception.push_back(n); break;
        }
    }
}

void AgentBrain::validateBrainState() const {
    if (!brainConfig_) {
        lastErrorMessage_ = "AgentBrain: Brain config is null";
    }
}

} // namespace nlm
