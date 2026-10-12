#pragma once

#include "AgentBrain.hpp"
#include "MotorCommandSelector.hpp"
#include "NoveltyCalculator.hpp"
#include "MotorDecoder.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace nlm {

class MotorCommandSelector {
public:
    struct Config {
        float curiosityThreshold = 0.3f;      // Threshold for curiosity-based exploration
        float randomActionThreshold = 0.5f;   // Threshold for high curiosity exploration
        float maxExploreChance = 0.3f;       // Maximum probability of random action
        bool enableCuriosity = true;          // Enable curiosity system
        std::vector<MotorCommand> actionSpace; // Available actions for random selection
    };

    MotorCommandSelector(std::shared_ptr<Brain> brain, const Config& config = Config())
        : brain_(brain), config_(config) {
        if (brain_) {
            initializeActionSpace();
        }
    }

    MotorCommand selectWithCuriosity(MotorCommand defaultCmd) {
        if (!config_.enableCuriosity || curiosityLevel_ <= config_.randomActionThreshold) {
            return defaultCmd;
        }

        float exploreChance = std::min(curiosityLevel_ * config_.maxExploreChance, config_.maxExploreChance);
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        
        if (r < exploreChance) {
            return randomAction();
        }

        return defaultCmd;
    }

    void setCuriosityLevel(float level) { curiosityLevel_ = std::clamp(level, 0.0f, 1.0f); }
    void enableCuriosity(bool enable) { config_.enableCuriosity = enable; }

private:
    void initializeActionSpace() {
        config_.actionSpace.clear();
        config_.actionSpace.push_back(MotorCommand::MoveForward);
        config_.actionSpace.push_back(MotorCommand::MoveBackward);
        config_.actionSpace.push_back(MotorCommand::TurnLeft);
        config_.actionSpace.push_back(MotorCommand::TurnRight);
        config_.actionSpace.push_back(MotorCommand::LookLeft);
        config_.actionSpace.push_back(MotorCommand::LookRight);
        config_.actionSpace.push_back(MotorCommand::Interact);
        config_.actionSpace.push_back(MotorCommand::Wait);
    }

    MotorCommand randomAction() {
        if (config_.actionSpace.empty()) return MotorCommand::Wait;

        size_t index = brain_->getRandomGenerator()->uniformInt(0, static_cast<int>(config_.actionSpace.size()) - 1);
        return config_.actionSpace[index];
    }

    std::shared_ptr<Brain> brain_;
    Config config_;
    float curiosityLevel_ = 0.0f;
};

class NoveltyCalculator {
public:
    struct Config {
        float sensoryNoveltyDecay = 0.99f;   // Decay rate for novelty levels
        float visionScaleFactor = 2.0f;     // Scale factor for vision novelty calculation
        float touchScaleFactor = 1.0f;      // Scale factor for touch signals
        float internalScaleFactor = 1.0f;   // Scale factor for internal signals
        float proprioceptionScaleFactor = 1.0f; // Scale factor for proprioception
        bool enableCuriosity = true;         // Enable curiosity system
    };

    NoveltyCalculator(const Config& config = Config()) : config_(config) {}

    struct NoveltyResult {
        float noveltyLevel = 0.0f;
        float curiosityLevel = 0.0f;
        float predictionError = 0.0f;
    };

    NoveltyResult calculateNovelty(const std::vector<float>& vision, const std::vector<float>& previousVision,
                                  float predictionError = 0.0f) {
        NoveltyResult result;

        if (!vision.empty()) {
            float totalDiff = 0.0f;
            for (size_t i = 0; i < vision.size() && i < previousVision.size(); ++i) {
                float diff = std::abs(vision[i] - previousVision[i]);
                totalDiff += diff;
            }

            // Normalize and apply decay
            result.noveltyLevel = totalDiff / std::max<size_t>(vision.size(), 1);
            result.noveltyLevel *= config_.sensoryNoveltyDecay;
        }

        // Calculate curiosity based on novelty and prediction error
        if (config_.enableCuriosity) {
            result.curiosityLevel = result.noveltyLevel * 2.0f + std::abs(predictionError) * 0.5f;
            result.curiosityLevel = std::clamp(result.curiosityLevel, 0.0f, 1.0f);
        }

        result.predictionError = predictionError;

        return result;
    }

    void enableCuriosity(bool enable) { config_.enableCuriosity = enable; }

private:
    Config config_;
};

class MotorDecoder {
public:
    struct Config {
        float activationThreshold = 0.5f;       // Minimum activity to consider action
        bool normalizeActivities = true;        // Normalize neuron activities
        float visionScaleFactor = 5.0f;         // Scale factor for vision input
        float touchScaleFactor = 8.0f;          // Scale factor for touch input
        float internalScaleFactor = 5.0f;       // Scale factor for internal signals
        float proprioceptionScaleFactor = 3.0f; // Scale factor for proprioception
    };

    MotorDecoder(const Config& config = Config()) : config_(config) {}

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
        struct { MotorCommand cmd; float activity; } commands[] = {
            {MotorCommand::MoveForward, forwardAct},
            {MotorCommand::MoveBackward, backwardAct},
            {MotorCommand::TurnLeft, leftAct},
            {MotorCommand::TurnRight, rightAct},
            {MotorCommand::Interact, interactAct},
            {MotorCommand::Wait, waitAct}
        };

        MotorCommand best = MotorCommand::Wait;
        float bestActivity = waitAct; // Default to wait if nothing stronger

        for (const auto& c : commands) {
            if (c.activity > bestActivity) {
                bestActivity = c.activity;
                best = c.cmd;
            }
        }

        // Only act if there's meaningful activity
        if (bestActivity < config_.activationThreshold) {
            return MotorCommand::Wait;
        }

        return best;
    }

    std::vector<float> scaleSensoryInput(const std::vector<float>& input, float scaleFactor) {
        std::vector<float> scaled;
        scaled.reserve(input.size());
        for (float value : input) {
            scaled.push_back(value * scaleFactor);
        }
        return scaled;
    }

    void setConfig(const Config& config) { config_ = config; }

private:
    Config config_;
};

class AgentBrain {
public:
    struct Config {
        MotorCommandSelector::Config selectorConfig;
        NoveltyCalculator::Config noveltyConfig;
        MotorDecoder::Config decoderConfig;
        
        // Development configuration
        bool enableDevelopment = true;
        bool enableRewardModulation = true;
        bool enableStructuralPlasticity = true;
        bool enableCuriosity = true;
        
        // Sensory processing configuration
        float visionScaleFactor = 5.0f;
        float touchScaleFactor = 8.0f;
        float internalScaleFactor = 5.0f;
        float proprioceptionScaleFactor = 3.0f;
        
        // Novelty configuration
        float sensoryNoveltyDecay = 0.99f;
    };

    AgentBrain(std::shared_ptr<Brain> brain, const Config& config = Config())
        : brain_(brain), config_(config) {
        // Initialize subsystems
        noveltyCalculator_ = std::make_shared<NoveltyCalculator>(config.noveltyConfig);
        motorDecoder_ = std::make_shared<MotorDecoder>(config.decoderConfig);
        motorSelector_ = std::make_shared<MotorCommandSelector>(brain, config.selectorConfig);
        
        // Initialize neuron groups
        if (brain_) {
            initializeNeuronGroups();
        }
    }

    ~AgentBrain() = default;

    void initialize(const SimpleWorld& world) {
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    }

    void processSensoryInput(const SensoryPercept& percept) {
        if (!brain_) return;

        // Process each sensory modality using the decoder
        const auto& vision = percept.getVision();
        const auto& touch = percept.getTouch();
        const auto& intern = percept.getInternal();
        const auto& proprio = percept.getProprioception();

        // Vision input
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                std::vector<float> scaledVision = motorDecoder_->scaleSensoryInput(vision, config_.visionScaleFactor);
                if (i < scaledVision.size()) {
                    sensoryVision_[i]->injectCurrent(scaledVision[i]);
                }
            }
        }

        // Touch input
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                std::vector<float> scaledTouch = motorDecoder_->scaleSensoryInput(touch, config_.touchScaleFactor);
                if (i < scaledTouch.size()) {
                    sensoryTouch_[i]->injectCurrent(scaledTouch[i]);
                }
            }
        }

        // Internal signals
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                std::vector<float> scaledInternal = motorDecoder_->scaleSensoryInput(intern, config_.internalScaleFactor);
                if (i < scaledInternal.size()) {
                    sensoryInternal_[i]->injectCurrent(scaledInternal[i]);
                }
            }
        }

        // Proprioception
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                std::vector<float> scaledProprio = motorDecoder_->scaleSensoryInput(proprio, config_.proprioceptionScaleFactor);
                if (i < scaledProprio.size()) {
                    sensoryProprioception_[i]->injectCurrent(scaledProprio[i]);
                }
            }
        }

        // Calculate novelty and curiosity
        auto noveltyResult = noveltyCalculator_->calculateNovelty(vision, previousVision_, predictionError_);
        noveltyLevel_ = noveltyResult.noveltyLevel;
        curiosityLevel_ = noveltyResult.curiosityLevel;
        motorSelector_->setCuriosityLevel(curiosityLevel_);
        
        // Update novelty for next time step
        previousVision_ = vision;
    }

    MotorCommand decodeMotorCommand() {
        if (!brain_) return MotorCommand::Wait;

        MotorCommand decoded = motorDecoder_->decodeFromMotorNeurons(
            motorForward_, motorBackward_, motorTurnLeft_, motorTurnRight_, 
            motorInteract_, motorWait_);

        // Apply curiosity-based exploration
        if (config_.enableCuriosity && curiosityLevel_ > config_.selectorConfig.curiosityThreshold) {
            decoded = motorSelector_->selectWithCuriosity(decoded);
        }

        return decoded;
    }

    void applyRewardModulation(float reward, float predictedReward) {
        if (!brain_ || !config_.enableRewardModulation) return;

        predictionError_ = reward - predictedReward;
        noveltyCalculator_->setCuriosityEnable(config_.enableCuriosity);

        expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
        dopamineLevel_ = predictionError_;
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);

        // Apply neuromodulation to brain
        applyNeuromodulationToBrain();

        // Modulate plasticity based on dopamine
        float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
        plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);

        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }

    void updateDevelopment(double timestep) {
        if (!brain_ || !config_.enableDevelopment) return;

        developmentalAge_ += timestep;

        // Simple developmental stages based on age
        if (developmentalAge_ < 60.0) {  // ~1 minute
            plasticityModifier_ = 1.0f;
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        } else if (developmentalAge_ < 300.0) {  // ~5 minutes
            plasticityModifier_ = 0.8f;
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (developmentalAge_ < 900.0) {  // ~15 minutes
            plasticityModifier_ = 0.5f;
            brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else {
            plasticityModifier_ = 0.2f;
            brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }

        // Structural plasticity
        if (config_.enableStructuralPlasticity) {
            auto* sp = brain_->getStructuralPlasticity();
            if (sp) {
                float synRate = 0.0001f * plasticityModifier_;
                float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
    }

    void reset() {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    }

    // Configuration methods
    void enableRewardModulation(bool enable) { config_.enableRewardModulation = enable; }
    void enableStructuralPlasticity(bool enable) { config_.enableStructuralPlasticity = enable; }
    void enableDevelopment(bool enable) { config_.enableDevelopment = enable; }
    void enableCuriosity(bool enable) { config_.enableCuriosity = enable; motorSelector_->enableCuriosity(enable); noveltyCalculator_->enableCuriosity(enable); }

    bool isRewardModulationEnabled() const { return config_.enableRewardModulation; }
    bool isStructuralPlasticityEnabled() const { return config_.enableStructuralPlasticity; }
    bool isDevelopmentEnabled() const { return config_.enableDevelopment; }
    bool isCuriosityEnabled() const { return config_.enableCuriosity; }

    // Getters
    size_t getSensoryInputSize() const {
        return 256 + 8 + 4 + 6;  // Vision + touch + internal + proprioception
    }

    size_t getMotorOutputSize() const {
        return 6;  // One motor neuron per action
    }

    float getNeuromodulationLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    double getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }

    DevelopmentalStage getDevelopmentalStage() const {
        if (!brain_) return DevelopmentalStage::Initial;
        return brain_->getDevelopmentalStage();
    }

    Brain* getBrain() { return brain_.get(); }

    // Configuration access
    Config& getConfig() { return config_; }
    const Config& getConfig() const { return config_; }

    // Advanced features
    void setActionSpace(const std::vector<MotorCommand>& actions) {
        motorSelector_->setActionSpace(actions);
    }

    void setSensoryScaleFactors(float vision, float touch, float internal, float proprioception) {
        config_.visionScaleFactor = vision;
        config_.touchScaleFactor = touch;
        config_.internalScaleFactor = internal;
        config_.proprioceptionScaleFactor = proprioception;
        motorDecoder_->setConfig(config_.decoderConfig);
    }

    void setCuriosityParameters(float threshold, float maxChance) {
        config_.selectorConfig.curiosityThreshold = threshold;
        config_.selectorConfig.maxExploreChance = maxChance;
    }

    std::string getConfigSummary() const {
        std::string summary = "AgentBrain Configuration:\n";
        summary += "  Reward Modulation: " + std::string(config_.enableRewardModulation ? "Enabled" : "Disabled") + "\n";
        summary += "  Structural Plasticity: " + std::string(config_.enableStructuralPlasticity ? "Enabled" : "Disabled") + "\n";
        summary += "  Development: " + std::string(config_.enableDevelopment ? "Enabled" : "Disabled") + "\n";
        summary += "  Curiosity: " + std::string(config_.enableCuriosity ? "Enabled" : "Disabled") + "\n";
        summary += "  Vision Scale Factor: " + std::to_string(config_.visionScaleFactor) + "\n";
        summary += "  Curiosity Threshold: " + std::to_string(config_.selectorConfig.curiosityThreshold) + "\n";
        return summary;
    }

private:
    void initializeNeuronGroups() {
        if (!brain_) return;

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

    void applyNeuromodulationToBrain() {
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

    // Member variables
    std::shared_ptr<Brain> brain_;
    Config config_;
    
    // Neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;

    // State variables
    float dopamineLevel_ = 0.0f;
    float noveltyLevel_ = 0.0f;
    float curiosityLevel_ = 0.0f;
    float predictionError_ = 0.0f;
    float expectedReward_ = 0.0f;
    double developmentalAge_ = 0.0;
    float plasticityModifier_ = 1.0f;
    std::vector<float> previousVision_;

    // Subsystem components
    std::shared_ptr<MotorCommandSelector> motorSelector_;
    std::shared_ptr<NoveltyCalculator> noveltyCalculator_;
    std::shared_ptr<MotorDecoder> motorDecoder_;
};

} // namespace nlm
