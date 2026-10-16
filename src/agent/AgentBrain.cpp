#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

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
    , maxSteps_(1000)
    , learningRate_(0.01f)
    , learningEnabled_(true)
    , temperature_(1.0f)
    , totalSteps_(0)
    , episodeSteps_(0)
    , episodeNumber_(0)
    , cumulativeReward_(0.0f)
    , successfulActions_(0)
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
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
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

} // namespace nlm

// Implementation of enhanced AgentBrain methods

py::dict AgentBrain::getAllStats() const {
    py::dict stats;
    if (brain_) {
        stats["firingRate"] = brain_->getAverageFiringRate();
        stats["spikeCount"] = brain_->getTotalSpikeCount();
        stats["neuronCount"] = brain_->getTotalNeuronCount();
        stats["synapseCount"] = brain_->getTotalSynapseCount();
        stats["curiosity"] = curiosityLevel_;
        stats["novelty"] = noveltyLevel_;
        stats["predictionError"] = predictionError_;
        stats["neuromodulation"] = dopamineLevel_;
        stats["developmentStage"] = static_cast<int>(getDevelopmentalStage());
    }
    stats["temperature"] = temperature_;
    stats["learningRate"] = learningRate_;
    stats["episodeNumber"] = episodeNumber_;
    stats["totalSteps"] = totalSteps_;
    stats["episodeSteps"] = episodeSteps_;
    stats["cumulativeReward"] = cumulativeReward_;
    stats["successfulActions"] = successfulActions_;
    stats["learningEnabled"] = learningEnabled_;
    stats["rewardModulationEnabled"] = rewardModulationEnabled_;
    stats["structuralPlasticityEnabled"] = structuralPlasticityEnabled_;
    stats["developmentEnabled"] = developmentEnabled_;
    stats["curiosityEnabled"] = curiosityEnabled_;
    
    return stats;
}

void AgentBrain::saveEpisode(const std::string& filename) {
    // Simple episode serialization
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return;
    }
    
    // Write episode number
    file.write(reinterpret_cast<const char*>(&episodeNumber_), sizeof(episodeNumber_));
    
    // Write statistics
    file.write(reinterpret_cast<const char*>(&totalSteps_), sizeof(totalSteps_));
    file.write(reinterpret_cast<const char*>(&episodeSteps_), sizeof(episodeSteps_));
    file.write(reinterpret_cast<const char*>(&cumulativeReward_), sizeof(cumulativeReward_));
    file.write(reinterpret_cast<const char*>(&successfulActions_), sizeof(successfulActions_));
    
    // Write agent state
    file.write(reinterpret_cast<const char*>(&dopamineLevel_), sizeof(dopamineLevel_));
    file.write(reinterpret_cast<const char*>(&noveltyLevel_), sizeof(noveltyLevel_));
    file.write(reinterpret_cast<const char*>(&curiosityLevel_), sizeof(curiosityLevel_));
    file.write(reinterpret_cast<const char*>(&predictionError_), sizeof(predictionError_));
    file.write(reinterpret_cast<const char*>(&expectedReward_), sizeof(expectedReward_));
    file.write(reinterpret_cast<const char*>(&developmentalAge_), sizeof(developmentalAge_));
    file.write(reinterpret_cast<const char*>(&plasticityModifier_), sizeof(plasticityModifier_));
    
    // Write configuration flags
    file.write(reinterpret_cast<const char*>(&rewardModulationEnabled_), sizeof(rewardModulationEnabled_));
    file.write(reinterpret_cast<const char*>(&structuralPlasticityEnabled_), sizeof(structuralPlasticityEnabled_));
    file.write(reinterpret_cast<const char*>(&developmentEnabled_), sizeof(developmentEnabled_));
    file.write(reinterpret_cast<const char*>(&curiosityEnabled_), sizeof(curiosityEnabled_));
    file.write(reinterpret_cast<const char*>(&learningEnabled_), sizeof(learningEnabled_));
    
    file.close();
    
    NLM_LOG_INFO("Episode saved to: " + filename);
}

void AgentBrain::loadEpisode(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for reading: " + filename);
        return;
    }
    
    // Read episode number
    file.read(reinterpret_cast<char*>(&episodeNumber_), sizeof(episodeNumber_));
    
    // Read statistics
    file.read(reinterpret_cast<char*>(&totalSteps_), sizeof(totalSteps_));
    file.read(reinterpret_cast<char*>(&episodeSteps_), sizeof(episodeSteps_));
    file.read(reinterpret_cast<char*>(&cumulativeReward_), sizeof(cumulativeReward_));
    file.read(reinterpret_cast<char*>(&successfulActions_), sizeof(successfulActions_));
    
    // Read agent state
    file.read(reinterpret_cast<char*>(&dopamineLevel_), sizeof(dopamineLevel_));
    file.read(reinterpret_cast<char*>(&noveltyLevel_), sizeof(noveltyLevel_));
    file.read(reinterpret_cast<char*>(&curiosityLevel_), sizeof(curiosityLevel_));
    file.read(reinterpret_cast<char*>(&predictionError_), sizeof(predictionError_));
    file.read(reinterpret_cast<char*>(&expectedReward_), sizeof(expectedReward_));
    file.read(reinterpret_cast<char*>(&developmentalAge_), sizeof(developmentalAge_));
    file.read(reinterpret_cast<char*>(&plasticityModifier_), sizeof(plasticityModifier_));
    
    // Read configuration flags
    file.read(reinterpret_cast<char*>(&rewardModulationEnabled_), sizeof(rewardModulationEnabled_));
    file.read(reinterpret_cast<char*>(&structuralPlasticityEnabled_), sizeof(structuralPlasticityEnabled_));
    file.read(reinterpret_cast<char*>(&developmentEnabled_), sizeof(developmentEnabled_));
    file.read(reinterpret_cast<char*>(&curiosityEnabled_), sizeof(curiosityEnabled_));
    file.read(reinterpret_cast<char*>(&learningEnabled_), sizeof(learningEnabled_));
    
    file.close();
    
    NLM_LOG_INFO("Episode loaded from: " + filename);
}

void AgentBrain::resetStats() {
    totalSteps_ = 0;
    episodeSteps_ = 0;
    episodeNumber_ = 0;
    cumulativeReward_ = 0.0f;
    successfulActions_ = 0;
    
    NLM_LOG_INFO("AgentBrain statistics reset");
}

} // namespace nlm
