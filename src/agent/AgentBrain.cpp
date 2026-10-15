#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <algorithm>

namespace nlm {

// Config implementation
bool AgentBrain::Config::loadFromFile(const std::string& filepath) {
    // Simple JSON-like parsing for basic config
    // In a real implementation, we'd use a proper JSON parser
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Logger::getInstance().error("Failed to open config file: " + filepath);
        return false;
    }
    
    // Parse simple key-value pairs
    std::string line;
    while (std::getline(file, line)) {
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) continue;
        
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        // Trim key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Set based on key
        if (key == "rewardModulationEnabled") {
            rewardModulationEnabled = (value == "true" || value == "1");
        } else if (key == "curiosityEnabled") {
            curiosityEnabled = (value == "true" || value == "1");
        } else if (key == "noveltyDetectionEnabled") {
            noveltyDetectionEnabled = (value == "true" || value == "1");
        } else if (key == "predictionErrorEnabled") {
            predictionErrorEnabled = (value == "true" || value == "1");
        } else if (key == "structuralPlasticityEnabled") {
            structuralPlasticityEnabled = (value == "true" || value == "1");
        } else if (key == "developmentEnabled") {
            developmentEnabled = (value == "true" || value == "1");
        } else if (key == "curiosityThreshold") {
            try { curiosityThreshold = std::stof(value); } catch (...) {}
        } else if (key == "explorationMultiplier") {
            try { explorationMultiplier = std::stof(value); } catch (...) {}
        } else if (key == "noveltySensitivity") {
            try { noveltySensitivity = std::stof(value); } catch (...) {}
        } else if (key == "predictionErrorWeight") {
            try { predictionErrorWeight = std::stof(value); } catch (...) {}
        } else if (key == "developmentalStartAge") {
            try { developmentalStartAge = std::stod(value); } catch (...) {}
        } else if (key == "criticalPeriodStart") {
            try { criticalPeriodStart = std::stod(value); } catch (...) {}
        } else if (key == "criticalPeriodEnd") {
            try { criticalPeriodEnd = std::stod(value); } catch (...) {}
        } else if (key == "maturationEnd") {
            try { maturationEnd = std::stod(value); } catch (...) {}
        } else if (key == "adaptivePlasticity") {
            adaptivePlasticity = (value == "true" || value == "1");
        } else if (key == "plasticityMaxFactor") {
            try { plasticityMaxFactor = std::stof(value); } catch (...) {}
        } else if (key == "plasticityMinFactor") {
            try { plasticityMinFactor = std::stof(value); } catch (...) {}
        } else if (key == "enableStateCheckpointing") {
            enableStateCheckpointing = (value == "true" || value == "1");
        } else if (key == "enableEpisodeManagement") {
            enableEpisodeManagement = (value == "true" || value == "1");
        } else if (key == "maxEpisodeLength") {
            try { maxEpisodeLength = std::stoul(value); } catch (...) {}
        } else if (key == "enableSensoryCompression") {
            enableSensoryCompression = (value == "true" || value == "1");
        } else if (key == "enableMotorSelectionOptimization") {
            enableMotorSelectionOptimization = (value == "true" || value == "1");
        }
    }
    
    file.close();
    Logger::getInstance().info("Config loaded from file: " + filepath);
    return validate();
}

bool AgentBrain::Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        Logger::getInstance().error("Failed to save config to file: " + filepath);
        return false;
    }
    
    // Write config as key=value pairs
    file << "# NLM AgentBrain Configuration\n";
    file << "rewardModulationEnabled=" << (rewardModulationEnabled ? "true" : "false") << "\n";
    file << "curiosityEnabled=" << (curiosityEnabled ? "true" : "false") << "\n";
    file << "noveltyDetectionEnabled=" << (noveltyDetectionEnabled ? "true" : "false") << "\n";
    file << "predictionErrorEnabled=" << (predictionErrorEnabled ? "true" : "false") << "\n";
    file << "structuralPlasticityEnabled=" << (structuralPlasticityEnabled ? "true" : "false") << "\n";
    file << "developmentEnabled=" << (developmentEnabled ? "true" : "false") << "\n";
    file << "curiosityThreshold=" << curiosityThreshold << "\n";
    file << "explorationMultiplier=" << explorationMultiplier << "\n";
    file << "noveltySensitivity=" << noveltySensitivity << "\n";
    file << "predictionErrorWeight=" << predictionErrorWeight << "\n";
    file << "developmentalStartAge=" << developmentalStartAge << "\n";
    file << "criticalPeriodStart=" << criticalPeriodStart << "\n";
    file << "criticalPeriodEnd=" << criticalPeriodEnd << "\n";
    file << "maturationEnd=" << maturationEnd << "\n";
    file << "adaptivePlasticity=" << (adaptivePlasticity ? "true" : "false") << "\n";
    file << "plasticityMaxFactor=" << plasticityMaxFactor << "\n";
    file << "plasticityMinFactor=" << plasticityMinFactor << "\n";
    file << "enableStateCheckpointing=" << (enableStateCheckpointing ? "true" : "false") << "\n";
    file << "enableEpisodeManagement=" << (enableEpisodeManagement ? "true" : "false") << "\n";
    file << "maxEpisodeLength=" << maxEpisodeLength << "\n";
    file << "enableSensoryCompression=" << (enableSensoryCompression ? "true" : "false") << "\n";
    file << "enableMotorSelectionOptimization=" << (enableMotorSelectionOptimization ? "true" : "false") << "\n";
    
    file.close();
    Logger::getInstance().info("Config saved to file: " + filepath);
    return true;
}

bool AgentBrain::Config::validate() const {
    std::string errors = getValidationErrors();
    if (!errors.empty()) {
        Logger::getInstance().error("Config validation failed: " + errors);
        return false;
    }
    Logger::getInstance().info("Config validation passed");
    return true;
}

std::string AgentBrain::Config::getValidationErrors() const {
    std::stringstream errors;
    
    if (curiosityThreshold < 0.0f || curiosityThreshold > 1.0f) {
        errors << "curiosityThreshold must be in [0.0, 1.0]; ";
    }
    
    if (explorationMultiplier < 1.0f) {
        errors << "explorationMultiplier must be >= 1.0; ";
    }
    
    if (noveltySensitivity < 0.0f) {
        errors << "noveltySensitivity must be >= 0.0; ";
    }
    
    if (predictionErrorWeight < 0.0f || predictionErrorWeight > 1.0f) {
        errors << "predictionErrorWeight must be in [0.0, 1.0]; ";
    }
    
    if (criticalPeriodStart >= criticalPeriodEnd) {
        errors << "criticalPeriodStart must be < criticalPeriodEnd; ";
    }
    
    if (maturationEnd < criticalPeriodEnd) {
        errors << "maturationEnd must be >= criticalPeriodEnd; ";
    }
    
    if (plasticityMaxFactor < plasticityMinFactor) {
        errors << "plasticityMaxFactor must be >= plasticityMinFactor; ";
    }
    
    if (maxEpisodeLength == 0) {
        errors << "maxEpisodeLength must be > 0; ";
    }
    
    return errors.str();
}

void AgentBrain::Config::resetToDefaults() {
    *this = getDefaultConfig();
}

AgentBrain::Config AgentBrain::Config::getDefaultConfig() {
    Config config;
    config.rewardModulationEnabled = true;
    config.curiosityEnabled = true;
    config.noveltyDetectionEnabled = true;
    config.predictionErrorEnabled = true;
    config.structuralPlasticityEnabled = true;
    config.developmentEnabled = true;
    
    config.curiosityThreshold = 0.3f;
    config.explorationMultiplier = 1.5f;
    config.noveltySensitivity = 2.0f;
    config.predictionErrorWeight = 0.5f;
    
    config.developmentalStartAge = 0.0;
    config.criticalPeriodStart = 60.0;
    config.criticalPeriodEnd = 300.0;
    config.maturationEnd = 900.0;
    
    config.adaptivePlasticity = true;
    config.plasticityMaxFactor = 2.0f;
    config.plasticityMinFactor = 0.1f;
    
    config.enableStateCheckpointing = true;
    config.enableEpisodeManagement = true;
    config.maxEpisodeLength = 1000;
    
    config.enableSensoryCompression = true;
    config.enableMotorSelectionOptimization = true;
    
    return config;
}

// Statistics implementation
void AgentBrain::Statistics::reset() {
    totalSteps = 0;
    totalEpisodes = 0;
    totalSpikes = 0;
    averageFiringRate = 0.0;
    averageCuriosityLevel = 0.0;
    averageNoveltyLevel = 0.0;
    averagePredictionError = 0.0;
    developmentalProgress = 0.0;
    memoryUsage = 0.0f;
}

void AgentBrain::Statistics::update(const AgentBrain& agent) {
    totalSteps++;
    
    // Update moving averages
    const double weight = 0.1; // Simple exponential moving average
    averageCuriosityLevel = (1.0 - weight) * averageCuriosityLevel + weight * agent.getCuriosityLevel();
    averageNoveltyLevel = (1.0 - weight) * averageNoveltyLevel + weight * agent.getNoveltyLevel();
    averagePredictionError = (1.0 - weight) * averagePredictionError + weight * agent.getPredictionError();
    
    // Estimate developmental progress (0-1 based on age)
    if (agent.getSimulationTime() < 60.0) {
        developmentalProgress = agent.getSimulationTime() / 60.0;
    } else {
        developmentalProgress = 1.0;
    }
    
    // Estimate memory usage
    memoryUsage = static_cast<float>(agent.getActiveNeuronCount() * sizeof(Neuron*) / 1024.0f); // KB
}

std::string AgentBrain::Statistics::toString() const {
    std::stringstream ss;
    ss << "Statistics Summary:\n";
    ss << "  Total Steps: " << totalSteps << "\n";
    ss << "  Total Episodes: " << totalEpisodes << "\n";
    ss << "  Total Spikes: " << totalSpikes << "\n";
    ss << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << averageFiringRate << "\n";
    ss << "  Average Curiosity Level: " << std::fixed << std::setprecision(3) << averageCuriosityLevel << "\n";
    ss << "  Average Novelty Level: " << std::fixed << std::setprecision(3) << averageNoveltyLevel << "\n";
    ss << "  Average Prediction Error: " << std::fixed << std::setprecision(3) << averagePredictionError << "\n";
    ss << "  Developmental Progress: " << std::fixed << std::setprecision(3) << developmentalProgress << "\n";
    ss << "  Memory Usage: " << std::fixed << std::setprecision(1) << memoryUsage << " KB\n";
    return ss.str();
}

// Enhanced AgentBrain constructor with configuration
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const Config& config)
    : brain_(brain)
    , config_(config)
    , dopamineLevel_(0.0f)
    , serotoninLevel_(0.2f)
    , norepinephrineLevel_(0.3f)
    , acetylcholineLevel_(0.1f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , uncertainty_(1.0f)
    , attentionLevel_(0.0f)
    , developmentalAge_(0.0)
    , simulationTime_(0.0)
    , plasticityModifier_(1.0f)
    , memoryStrength_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , noveltyDetectionEnabled_(true)
    , predictionErrorEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , currentEpisodeLength_(0)
    , episodeStartStep_(0)
{
    // Initialize neuromodulators
    dopamineLevel_ = 0.0f;
    serotoninLevel_ = 0.2f;
    norepinephrineLevel_ = 0.3f;
    acetylcholineLevel_ = 0.1f;
    
    // Load config flags
    rewardModulationEnabled_ = config_.rewardModulationEnabled;
    structuralPlasticityEnabled_ = config_.structuralPlasticityEnabled;
    developmentEnabled_ = config_.developmentEnabled;
    curiosityEnabled_ = config_.curiosityEnabled;
    noveltyDetectionEnabled_ = config_.noveltyDetectionEnabled;
    predictionErrorEnabled_ = config_.predictionErrorEnabled;
    
    // Initialize neuron groups
    initializeNeuronGroups();
    
    Logger::getInstance().info("AgentBrain created with advanced neuromodulation and configuration support");
}

AgentBrain::~AgentBrain() {
    Logger::getInstance().info("AgentBrain destroyed");
}

void AgentBrain::initialize(const SimpleWorld& world, const Config& config) {
    updateConfig(config);
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = config.developmentalStartAge;
    plasticityModifier_ = 1.0f;
    simulationTime_ = 0.0;
    currentEpisodeLength_ = 0;
    
    // Initialize statistics
    stats_.reset();
    
    // Update state cache
    updateStateCache();
    
    // Call episode start callback
    callEpisodeStartCallback();
    
    Logger::getInstance().info("AgentBrain initialized with enhanced configuration");
}

void AgentBrain::initializeNeuronGroups() {
    if (!brain_) return;
    
    // Clear existing groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
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
                        // Distribute sensory neurons based on population size
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

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Process enhanced sensory input with compression
    processEnhancedSensoryInput(percept);
    
    // Compute novelty with attention weighting
    computeEnhancedNovelty(percept);
    
    // Compute curiosity with custom callbacks
    computeCuriosity(percept);
    
    // Compute prediction error
    if (predictionErrorEnabled_) {
        float predictedReward = expectedReward_;
        computePredictionError(predictedReward, predictedReward);
    }
    
    // Update neuromodulation state
    updateNeuromodulationState();
    
    // Update statistics
    updateStatistics();
    
    // Update simulation time
    simulationTime_ += 1.0;
}

void AgentBrain::processEnhancedSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Compress sensory input if enabled
    if (config_.enableSensoryCompression) {
        // Apply adaptive sensory compression
        std::vector<float> compressedInput = compressSensoryInput(percept);
        
        // Vision input (256 values -> sensoryVision_ neurons)
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                // Apply compressed input
                float current = vision[i] * 5.0f;
                sensoryVision_[i]->injectCurrent(current);
            }
        }
        
        // Touch input (8 values -> sensoryTouch_ neurons)
        const auto& touch = percept.getTouch();
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
        
        // Internal signals (4 values -> sensoryInternal_ neurons)
        const auto& intern = percept.getInternal();
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
        
        // Proprioception (6 values -> sensoryProprioception_ neurons)
        const auto& proprio = percept.getProprioception();
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    } else {
        // Original processing without compression
        // Vision input (256 values -> sensoryVision_ neurons)
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                float current = vision[i] * 5.0f;
                sensoryVision_[i]->injectCurrent(current);
            }
        }
        
        // Touch input (8 values -> sensoryTouch_ neurons)
        const auto& touch = percept.getTouch();
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
        
        // Internal signals (4 values -> sensoryInternal_ neurons)
        const auto& intern = percept.getInternal();
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
        
        // Proprioception (6 values -> sensoryProprioception_ neurons)
        const auto& proprio = percept.getProprioception();
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
}

void AgentBrain::computeEnhancedNovelty(const SensoryPercept& percept) {
    if (!config_.noveltyDetectionEnabled) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    const auto& vision = percept.getVision();
    if (vision.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    // Enhanced novelty computation with attention weighting
    float totalDiff = 0.0f;
    float attentionWeight = 1.0f;
    
    // Apply attention weighting based on neuromodulators
    attentionWeight = acetylcholineLevel_ * 0.5f + attentionLevel_ * 0.5f;
    attentionWeight = std::clamp(attentionWeight, 0.0f, 2.0f);
    
    // Compute difference with attention weighting
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        // Apply attention weighting
        diff *= attentionWeight;
        totalDiff += diff;
    }
    
    // Apply novelty sensitivity from config
    totalDiff *= config_.noveltySensitivity;
    
    // Normalize and apply decay
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next time
    previousVision_ = vision;
}

void AgentBrain::computeCuriosity(const SensoryPercept& percept) {
    if (!config_.curiosityEnabled) {
        curiosityLevel_ = 0.0f;
        return;
    }
    
    float noveltyComponent = noveltyLevel_ * config_.noveltySensitivity;
    float predictionErrorComponent = std::abs(predictionError_) * config_.predictionErrorWeight;
    
    // Apply custom curiosity callback if provided
    if (config_.customCuriosityCallback) {
        curiosityLevel_ = config_.customCuriosityCallback(percept);
    } else {
        curiosityLevel_ = noveltyComponent + predictionErrorComponent;
    }
    
    // Apply developmental and attention factors
    float developmentalFactor = 1.0f;
    if (developmentEnabled_) {
        developmentalFactor = 1.0f + (developmentalAge_ / maturationEnd);
    }
    
    float attentionFactor = attentionLevel_ * 0.5f + 1.0f;
    
    curiosityLevel_ *= developmentalFactor * attentionFactor;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

void AgentBrain::computePredictionError(float reward, float predictedReward) {
    if (!predictionErrorEnabled_) {
        predictionError_ = 0.0f;
        return;
    }
    
    // Enhanced prediction error with uncertainty weighting
    predictionError_ = reward - predictedReward;
    
    // Apply uncertainty weighting
    if (uncertainty_ > 0.0f) {
        predictionError_ /= (uncertainty_ + 1.0f);
    }
    
    // Apply prediction error weight from config
    predictionError_ *= config_.predictionErrorWeight;
    
    // Clamp to reasonable range
    predictionError_ = std::clamp(predictionError_, -1.0f, 1.0f);
}

void AgentBrain::updateNeuromodulationState() {
    // Update neuromodulators based on various signals
    
    // Serotonin: related to novelty and prediction error
    serotoninLevel_ = noveltyLevel_ * 0.3f + std::abs(predictionError_) * 0.4f;
    serotoninLevel_ = std::clamp(serotoninLevel_, 0.0f, 1.0f);
    
    // Norepinephrine: related to attention and arousal
    norepinephrineLevel_ = attentionLevel_ * 0.5f + uncertainty_ * 0.3f;
    norepinephrineLevel_ = std::clamp(norepinephrineLevel_, 0.0f, 1.0f);
    
    // Acetylcholine: related to learning and attention
    acetylcholineLevel_ = attentionLevel_ * 0.7f + (1.0f - developmentalProgress) * 0.3f;
    acetylcholineLevel_ = std::clamp(acetylcholineLevel_, 0.0f, 1.0f);
    
    // Dopamine is updated during reward modulation
}

void AgentBrain::updateDevelopmentState(double timestep) {
    if (!developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    simulationTime_ += timestep;
    
    // Enhanced developmental stages based on config
    if (developmentalAge_ < config_.criticalPeriodStart) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < config_.criticalPeriodEnd) {
        plasticityModifier_ = config_.plasticityMaxFactor * (1.0f - 
            (developmentalAge_ - config_.criticalPeriodStart) / 
            (config_.criticalPeriodEnd - config_.criticalPeriodStart)) * 0.5f + 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < config_.maturationEnd) {
        plasticityModifier_ = config_.plasticityMinFactor + 
            (1.0f - config_.plasticityMinFactor) * 
            (1.0f - (developmentalAge_ - config_.criticalPeriodEnd) / 
             (config_.maturationEnd - config_.criticalPeriodEnd));
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = config_.plasticityMinFactor;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Memory strength changes with development
    memoryStrength_ = std::min(1.0f, plasticityModifier_ * 2.0f);
    
    // Update structural plasticity if enabled
    if (structuralPlasticityEnabled_ && config_.structuralPlasticityEnabled) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Enhanced synaptogenesis and pruning based on developmental stage
            float synRate = 0.0001f * plasticityModifier_ * (1.0f + developmentalAge_ / 1000.0f);
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_) * (1.0f - developmentalProgress);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

std::vector<float> AgentBrain::compressSensoryInput(const SensoryPercept& percept) const {
    std::vector<float> compressed;
    
    // Apply principal component analysis (simplified)
    const auto& vision = percept.getVision();
    
    // Simple temporal difference compression
    if (!previousVision_.empty() && vision.size() == previousVision_.size()) {
        for (size_t i = 0; i < vision.size(); ++i) {
            float diff = vision[i] - previousVision_[i];
            compressed.push_back(diff * 0.1f); // Temporal difference
        }
    } else {
        // Just downsample vision
        size_t step = std::max<size_t>(1, vision.size() / 256);
        for (size_t i = 0; i < vision.size(); i += step) {
            compressed.push_back(vision[i] * 0.5f);
        }
    }
    
    // Limit size to 256
    if (compressed.size() > 256) {
        compressed.resize(256);
    }
    
    return compressed;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
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
    
    // Find maximum activity with enhanced motor selection
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Apply confidence weighting if enabled
    if (config_.enableMotorSelectionOptimization && bestActivity > 0.5f) {
        float confidence = bestActivity;
        
        // Add exploration bias based on curiosity
        if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
            confidence *= (1.0f - curiosityLevel_ * 0.2f);
        }
        
        // Add uncertainty factor
        confidence *= (1.0f - uncertainty_ * 0.3f);
        
        // Only act if confidence is high enough
        if (confidence < 0.6f) {
            return MotorCommand::Wait;
        }
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced curiosity-driven exploration
    if (config_.curiosityEnabled && curiosityLevel_ > config_.curiosityThreshold) {
        // Calculate exploration probability
        float exploreChance = curiosityLevel_ * config_.explorationMultiplier * 0.2f;
        exploreChance = std::clamp(exploreChance, 0.0f, 0.5f);
        
        // Random decision based on explore chance
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command with developmental bias
            float developmentalBias = 1.0f;
            if (developmentalAge_ < config_.maturationEnd) {
                developmentalBias = 1.0f - (developmentalAge_ / config_.maturationEnd);
            }
            
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

MotorCommand AgentBrain::decodeAdvancedMotorCommand() {
    // Get all candidate motor commands
    std::vector<MotorCommand> candidates;
    candidates.push_back(MotorCommand::MoveForward);
    candidates.push_back(MotorCommand::MoveBackward);
    candidates.push_back(MotorCommand::TurnLeft);
    candidates.push_back(MotorCommand::TurnRight);
    candidates.push_back(MotorCommand::LookLeft);
    candidates.push_back(MotorCommand::LookRight);
    candidates.push_back(MotorCommand::Interact);
    candidates.push_back(MotorCommand::Wait);
    
    // Evaluate each candidate based on neural activity
    std::vector<std::pair<MotorCommand, float>> scores;
    for (MotorCommand cmd : candidates) {
        float score = evaluateMotorCommand(cmd);
        scores.push_back(std::make_pair(cmd, score));
    }
    
    // Select best command using advanced optimization
    return optimizeMotorSelection(scores);
}

float AgentBrain::evaluateMotorCommand(MotorCommand cmd) const {
    // Map motor command to expected neuron group activity
    float expectedActivity = 0.0f;
    
    switch (cmd) {
        case MotorCommand::MoveForward: expectedActivity = 0.8f; break;
        case MotorCommand::MoveBackward: expectedActivity = 0.6f; break;
        case MotorCommand::TurnLeft: expectedActivity = 0.7f; break;
        case MotorCommand::TurnRight: expectedActivity = 0.7f; break;
        case MotorCommand::LookLeft: expectedActivity = 0.4f; break;
        case MotorCommand::LookRight: expectedActivity = 0.4f; break;
        case MotorCommand::Interact: expectedActivity = 0.9f; break;
        case MotorCommand::Wait: expectedActivity = 0.1f; break;
    }
    
    return expectedActivity;
}

MotorCommand AgentBrain::optimizeMotorSelection(const std::vector<std::pair<MotorCommand, float>>& candidates) const {
    if (candidates.empty()) return MotorCommand::Wait;
    
    // Find best candidate
    auto bestIt = std::max_element(candidates.begin(), candidates.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    
    MotorCommand bestCmd = bestIt->first;
    float bestScore = bestIt->second;
    
    // Apply uncertainty-based exploration
    if (bestScore < 0.5f && uncertainty_ > 0.5f) {
        // Higher uncertainty = more exploration
        float exploreProb = (uncertainty_ - 0.5f) * 0.4f;
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreProb) {
            // Pick a random command with moderate scores
            std::vector<MotorCommand> moderateCommands;
            for (const auto& pair : candidates) {
                if (pair.second > 0.3f && pair.second < 0.7f) {
                    moderateCommands.push_back(pair.first);
                }
            }
            if (!moderateCommands.empty()) {
                return moderateCommands[brain_->getRandomGenerator()->uniformInt(0, moderateCommands.size() - 1)];
            }
        }
    }
    
    return bestCmd;
}

MotorCommand AgentBrain::selectWithAdvancedCuriosity(MotorCommand defaultCmd) {
    // Enhanced curiosity-driven exploration with developmental and attention factors
    if (!config_.curiosityEnabled) return defaultCmd;
    
    if (curiosityLevel_ > config_.curiosityThreshold) {
        // Calculate exploration probability based on multiple factors
        float curiosityFactor = curiosityLevel_ * 0.4f;
        float developmentalFactor = 1.0f - (developmentalAge_ / config_.maturationEnd);
        float attentionFactor = attentionLevel_ * 0.2f;
        
        float exploreChance = curiosityFactor * developmentalFactor * attentionFactor;
        exploreChance = std::clamp(exploreChance, 0.0f, 0.8f);
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command with bias toward interaction when developmental
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            
            // Add developmental bias toward exploration in early stages
            if (developmentalAge_ < config_.criticalPeriodStart) {
                // Favor exploration in initial stage
                if (choice >= 6) choice = 0; // Reduce wait/chance of interaction
            }
            
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

void AgentBrain::computeEnhancedNeuromodulation(const SensoryPercept& percept, float reward, float predictedReward) {
    // Enhanced neuromodulation computation
    
    // Compute prediction error
    computePredictionError(reward, predictedReward);
    
    // Update reward prediction
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Update uncertainty based on prediction error variance
    uncertainty_ = std::abs(predictionError_) * 0.5f + uncertainty_ * 0.9f;
    uncertainty_ = std::clamp(uncertainty_, 0.0f, 1.0f);
    
    // Update attention level based on novelty
    attentionLevel_ = noveltyLevel_ * 0.5f + attentionLevel_ * 0.8f;
    attentionLevel_ = std::clamp(attentionLevel_, 0.0f, 1.0f);
    
    // Apply neuromodulators to brain
    if (rewardModulationEnabled_) {
        auto* dopamine = brain_->getDopamine();
        if (dopamine) {
            dopamine->signalRewardPredictionError(predictionError_);
            dopamine->setLevel(dopamineLevel_);
        }
        
        brain_->applyNeuromodulation(*dopamine);
    }
    
    // Update development
    updateDevelopmentState(1.0);
}

void AgentBrain::updateAdvancedDevelopment(double timestep) {
    updateDevelopmentState(timestep);
}

void AgentBrain::updateStatistics() {
    stats_.update(*this);
}

void AgentBrain::updateNeuromodulationState() {
    // Update neuromodulators based on current signals
    serotoninLevel_ = noveltyLevel_ * 0.3f + std::abs(predictionError_) * 0.4f;
    serotoninLevel_ = std::clamp(serotoninLevel_, 0.0f, 1.0f);
    
    norepinephrineLevel_ = attentionLevel_ * 0.5f + uncertainty_ * 0.3f;
    norepinephrineLevel_ = std::clamp(norepinephrineLevel_, 0.0f, 1.0f);
    
    acetylcholineLevel_ = attentionLevel_ * 0.7f + (1.0f - developmentalProgress) * 0.3f;
    acetylcholineLevel_ = std::clamp(acetylcholineLevel_, 0.0f, 1.0f);
    
    // Apply adaptive plasticity if enabled
    if (config_.adaptivePlasticity) {
        float adaptivePlasticityFactor = 0.5f + 0.5f * dopamineLevel_;
        adaptivePlasticityFactor = std::clamp(adaptivePlasticityFactor, 
            config_.plasticityMinFactor, config_.plasticityMaxFactor);
        
        // Apply to plasticity systems
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * adaptivePlasticityFactor);
            stdp->setLTDWeight(0.012f * adaptivePlasticityFactor);
        }
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward, float uncertainty) {
    if (!brain_ || !config_.rewardModulationEnabled) return;
    
    // Update uncertainty
    uncertainty_ = uncertainty;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Apply uncertainty weighting
    if (uncertainty_ > 0.0f) {
        predictionError_ /= (uncertainty_ + 1.0f);
    }
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on weighted prediction error)
    dopamineLevel_ = predictionError_ * config_.predictionErrorWeight;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply reward modulation hook if provided
    if (config_.rewardModulationHook) {
        config_.rewardModulationHook(reward, predictedReward);
    }
    
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
    updateAdvancedDevelopment(timestep);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getDopamineLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getSerotoninLevel() const {
    return serotoninLevel_;
}

float AgentBrain::getNorepinephrineLevel() const {
    return norepinephrineLevel_;
}

float AgentBrain::getAcetylcholineLevel() const {
    return acetylcholineLevel_;
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

float AgentBrain::getExpectedReward() const {
    return expectedReward_;
}

float AgentBrain::getUncertainty() const {
    return uncertainty_;
}

float AgentBrain::getAttentionLevel() const {
    return attentionLevel_;
}

double AgentBrain::getSimulationTime() const {
    return simulationTime_;
}

double AgentBrain::getDevelopmentalProgress() const {
    if (config_.maturationEnd > 0.0) {
        return std::min(1.0, developmentalAge_ / config_.maturationEnd);
    }
    return 0.0;
}

bool AgentBrain::isInCriticalPeriod() const {
    return developmentEnabled_ && 
           developmentalAge_ >= config_.criticalPeriodStart && 
           developmentalAge_ < config_.criticalPeriodEnd;
}

size_t AgentBrain::getActiveNeuronCount() const {
    if (!brain_) return 0;
    return brain_->getActiveNeuronCount();
}

float AgentBrain::getLearningRate() const {
    if (!brain_) return 0.01f;
    
    // Base learning rate with neuromodulation factors
    float baseRate = 0.01f;
    float modulation = 1.0f;
    
    // Apply neuromodulation
    modulation *= (1.0f + dopamineLevel_ * 0.5f); // Dopamine increases learning
    modulation *= (1.0f - serotoninLevel_ * 0.2f); // Serotonin decreases learning
    modulation *= (1.0f - norepinephrineLevel_ * 0.1f); // NE has small effect
    modulation *= (1.0f - acetylcholineLevel_ * 0.3f); // ACh decreases learning
    
    modulation = std::clamp(modulation, 0.1f, 3.0f);
    
    return baseRate * modulation;
}

void AgentBrain::reset() {
    // Reset neuromodulation levels
    dopamineLevel_ = 0.0f;
    serotoninLevel_ = 0.2f;
    norepinephrineLevel_ = 0.3f;
    acetylcholineLevel_ = 0.1f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    uncertainty_ = 1.0f;
    attentionLevel_ = 0.0f;
    
    // Reset development
    developmentalAge_ = config_.developmentalStartAge;
    simulationTime_ = 0.0;
    plasticityModifier_ = 1.0f;
    memoryStrength_ = 1.0f;
    
    // Clear sensory history
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset statistics
    stats_.reset();
    
    // Call episode end callback
    callEpisodeEndCallback();
    
    // Start new episode
    currentEpisodeLength_ = 0;
    episodeStartStep_ = stats_.totalSteps;
    
    // Call episode start callback
    callEpisodeStartCallback();
    
    Logger::getInstance().info("AgentBrain reset with advanced neuromodulation");
}

void AgentBrain::resetEpisode() {
    if (!config_.enableEpisodeManagement) return;
    
    // Call episode end callback
    callEpisodeEndCallback();
    
    // Increment episode counter
    stats_.totalEpisodes++;
    
    // Reset episode-specific state
    currentEpisodeLength_ = 0;
    episodeStartStep_ = stats_.totalSteps;
    
    // Reset some neuromodulators for new episode
    float resetFactor = 0.3f;
    serotoninLevel_ *= resetFactor;
    norepinephrineLevel_ *= resetFactor;
    acetylcholineLevel_ *= resetFactor;
    
    // Call episode start callback
    callEpisodeStartCallback();
}

const AgentBrain::Statistics& AgentBrain::getStatistics() const {
    return stats_;
}

std::string AgentBrain::getStatisticsString() const {
    std::stringstream ss;
    ss << "=== AgentBrain Statistics ===\n";
    ss << stats_.toString();
    ss << "\nNeuromodulation:\n";
    ss << "  Dopamine: " << std::fixed << std::setprecision(3) << dopamineLevel_ << "\n";
    ss << "  Serotonin: " << std::fixed << std::setprecision(3) << serotoninLevel_ << "\n";
    ss << "  Norepinephrine: " << std::fixed << std::setprecision(3) << norepinephrineLevel_ << "\n";
    ss << "  Acetylcholine: " << std::fixed << std::setprecision(3) << acetylcholineLevel_ << "\n";
    ss << "  Novelty: " << std::fixed << std::setprecision(3) << noveltyLevel_ << "\n";
    ss << "  Curiosity: " << std::fixed << std::setprecision(3) << curiosityLevel_ << "\n";
    ss << "  Prediction Error: " << std::fixed << std::setprecision(3) << predictionError_ << "\n";
    ss << "  Expected Reward: " << std::fixed << std::setprecision(3) << expectedReward_ << "\n";
    ss << "  Uncertainty: " << std::fixed << std::setprecision(3) << uncertainty_ << "\n";
    ss << "  Attention: " << std::fixed << std::setprecision(3) << attentionLevel_ << "\n";
    ss << "\nDevelopment:\n";
    ss << "  Age: " << std::fixed << std::setprecision(1) << developmentalAge_ << "\n";
    ss << "  Simulation Time: " << std::fixed << std::setprecision(1) << simulationTime_ << "\n";
    ss << "  Plasticity Modifier: " << std::fixed << std::setprecision(3) << plasticityModifier_ << "\n";
    ss << "  Memory Strength: " << std::fixed << std::setprecision(3) << memoryStrength_ << "\n";
    ss << "  Stage: " << static_cast<int>(getDevelopmentalStage()) << "\n";
    ss << "  In Critical Period: " << (isInCriticalPeriod() ? "Yes" : "No") << "\n";
    ss << "  Progress: " << std::fixed << std::setprecision(3) << getDevelopmentalProgress() << "\n";
    return ss.str();
}

bool AgentBrain::saveState(const std::string& filepath) const {
    if (!config_.enableStateCheckpointing) return false;
    
    // Create state snapshot
    std::ofstream file(filepath);
    if (!file.is_open()) {
        Logger::getInstance().error("Failed to save state to file: " + filepath);
        return false;
    }
    
    // Write state as key=value pairs
    file << "# AgentBrain State Checkpoint\n";
    file << "dopamineLevel=" << dopamineLevel_ << "\n";
    file << "serotoninLevel=" << serotoninLevel_ << "\n";
    file << "norepinephrineLevel=" << norepinephrineLevel_ << "\n";
    file << "acetylcholineLevel=" << acetylcholineLevel_ << "\n";
    file << "noveltyLevel=" << noveltyLevel_ << "\n";
    file << "curiosityLevel=" << curiosityLevel_ << "\n";
    file << "predictionError=" << predictionError_ << "\n";
    file << "expectedReward=" << expectedReward_ << "\n";
    file << "uncertainty=" << uncertainty_ << "\n";
    file << "attentionLevel=" << attentionLevel_ << "\n";
    file << "developmentalAge=" << developmentalAge_ << "\n";
    file << "simulationTime=" << simulationTime_ << "\n";
    file << "plasticityModifier=" << plasticityModifier_ << "\n";
    file << "memoryStrength=" << memoryStrength_ << "\n";
    file << "currentEpisodeLength=" << currentEpisodeLength_ << "\n";
    file << "episodeStartStep=" << episodeStartStep_ << "\n";
    
    file.close();
    Logger::getInstance().info("AgentBrain state saved to: " + filepath);
    return true;
}

bool AgentBrain::loadState(const std::string& filepath) {
    if (!config_.enableStateCheckpointing) return false;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Logger::getInstance().error("Failed to load state from file: " + filepath);
        return false;
    }
    
    // Parse state file
    std::string line;
    bool success = true;
    while (std::getline(file, line)) {
        // Remove whitespace and comments
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        
        if (line.empty() || line[0] == '#') continue;
        
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) continue;
        
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        // Parse based on key
        try {
            if (key == "dopamineLevel") dopamineLevel_ = std::stof(value);
            else if (key == "serotoninLevel") serotoninLevel_ = std::stof(value);
            else if (key == "norepinephrineLevel") norepinephrineLevel_ = std::stof(value);
            else if (key == "acetylcholineLevel") acetylcholineLevel_ = std::stof(value);
            else if (key == "noveltyLevel") noveltyLevel_ = std::stof(value);
            else if (key == "curiosityLevel") curiosityLevel_ = std::stof(value);
            else if (key == "predictionError") predictionError_ = std::stof(value);
            else if (key == "expectedReward") expectedReward_ = std::stof(value);
            else if (key == "uncertainty") uncertainty_ = std::stof(value);
            else if (key == "attentionLevel") attentionLevel_ = std::stof(value);
            else if (key == "developmentalAge") developmentalAge_ = std::stod(value);
            else if (key == "simulationTime") simulationTime_ = std::stod(value);
            else if (key == "plasticityModifier") plasticityModifier_ = std::stof(value);
            else if (key == "memoryStrength") memoryStrength_ = std::stof(value);
            else if (key == "currentEpisodeLength") currentEpisodeLength_ = std::stoul(value);
            else if (key == "episodeStartStep") episodeStartStep_ = std::stoull(value);
        } catch (...) {
            Logger::getInstance().error("Error parsing state file: " + line);
            success = false;
        }
    }
    
    file.close();
    
    if (success) {
        updateStateCache();
        Logger::getInstance().info("AgentBrain state loaded from: " + filepath);
    }
    
    return success;
}

std::string AgentBrain::getStateFingerprint() const {
    // Create a simple fingerprint based on state
    std::stringstream ss;
    ss << "state_";
    ss << std::fixed << std::setprecision(3);
    ss << dopamineLevel_ << serotoninLevel_ << norepinephrineLevel_ 
        << acetylcholineLevel_ << noveltyLevel_ << curiosityLevel_
        << predictionError_ << expectedReward_ << uncertainty_
        << attentionLevel_ << developmentalAge_ << simulationTime_
        << plasticityModifier_ << memoryStrength_ << currentEpisodeLength_;
    
    // Simple hash (not cryptographically secure)
    std::string fingerprint = ss.str();
    std::hash<std::string> hasher;
    size_t hash = hasher(fingerprint);
    
    std::stringstream hexStream;
    hexStream << std::hex << hash;
    
    return hexStream.str();
}

void AgentBrain::callEpisodeStartCallback() {
    if (config_.episodeStartCallback) {
        config_.episodeStartCallback(*this);
    }
}

void AgentBrain::callEpisodeEndCallback() {
    if (config_.episodeEndCallback) {
        config_.episodeEndCallback(*this);
    }
}

void AgentBrain::updateConfig(const Config& config) {
    // Validate new config
    if (!config.validate()) {
        Logger::getInstance().error("Invalid config provided: " + config.getValidationErrors());
        return;
    }
    
    config_ = config;
    
    // Update internal flags
    rewardModulationEnabled_ = config_.rewardModulationEnabled;
    structuralPlasticityEnabled_ = config_.structuralPlasticityEnabled;
    developmentEnabled_ = config_.developmentEnabled;
    curiosityEnabled_ = config_.curiosityEnabled;
    noveltyDetectionEnabled_ = config_.noveltyDetectionEnabled;
    predictionErrorEnabled_ = config_.predictionErrorEnabled;
    
    // Reinitialize neuron groups if needed
    if (config_.enableSensoryCompression != (sensoryVision_.size() < 100)) {
        initializeNeuronGroups();
    }
    
    Logger::getInstance().info("AgentBrain configuration updated");
}

void AgentBrain::updateStateCache() {
    // Update state cache for checkpointing
    stateCache_["dopamineLevel"] = std::to_string(dopamineLevel_);
    stateCache_["serotoninLevel"] = std::to_string(serotoninLevel_);
    stateCache_["curiosityLevel"] = std::to_string(curiosityLevel_);
    stateCache_["predictionError"] = std::to_string(predictionError_);
    stateCache_["developmentalAge"] = std::to_string(developmentalAge_);
    stateCache_["simulationTime"] = std::to_string(simulationTime_);
    stateCache_["plasticityModifier"] = std::to_string(plasticityModifier_);
    
    // Update fingerprint
    lastStateFingerprint_ = getStateFingerprint();
}

bool AgentBrain::validateConfig() const {
    return config_.validate();
}

void AgentBrain::resetEpisode() {
    resetEpisode();
}

MotorCommand AgentBrain::decodeMotorCommand() {
    // Enhanced motor decoding with curiosity
    MotorCommand decoded = decodeAdvancedMotorCommand();
    
    // Apply curiosity-based exploration
    if (config_.curiosityEnabled && curiosityLevel_ > config_.curiosityThreshold) {
        decoded = selectWithAdvancedCuriosity(decoded);
    }
    
    return decoded;
}

void AgentBrain::processEnhancedSensoryInput(const SensoryPercept& percept) {
    // Already implemented in processSensoryInput
    processSensoryInput(percept);
}

MotorCommand AgentBrain::decodeAdvancedMotorCommand() {
    // Already implemented as decodeFromMotorNeurons
    return decodeFromMotorNeurons();
}

MotorCommand AgentBrain::selectWithAdvancedCuriosity(MotorCommand defaultCmd) {
    // Already implemented
    return selectWithCuriosity(defaultCmd);
}

void AgentBrain::computeEnhancedNeuromodulation(const SensoryPercept& percept, float reward, float predictedReward) {
    // Already implemented in processSensoryInput
    computeEnhancedNeuromodulation(percept, reward, predictedReward);
}

void AgentBrain::updateAdvancedDevelopment(double timestep) {
    // Already implemented
    updateDevelopmentState(timestep);
}

void AgentBrain::updateStatistics() {
    // Already implemented
    stats_.update(*this);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    // Forward compatibility method
    return decodeMotorCommand();
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // Forward compatibility method
    applyRewardModulation(reward, predictedReward, 1.0f);
}

void AgentBrain::updateDevelopment(double timestep) {
    // Forward compatibility method
    updateDevelopment(timestep);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    // Forward compatibility method
    return getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    // Forward compatibility method - returns overall modulation level
    float totalModulation = (dopamineLevel_ + serotoninLevel_ + 
                           norepinephrineLevel_ + acetylcholineLevel_) / 4.0f;
    return std::clamp(totalModulation, 0.0f, 1.0f);
}

float AgentBrain::getCuriosityLevel() const {
    // Forward compatibility method
    return getCuriosityLevel();
}

float AgentBrain::getNoveltyLevel() const {
    // Forward compatibility method
    return getNoveltyLevel();
}

float AgentBrain::getPredictionError() const {
    // Forward compatibility method
    return getPredictionError();
}

void AgentBrain::reset() {
    // Forward compatibility method
    reset();
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6; // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6; // One motor neuron per action
}

// Additional utility methods
float AgentBrain::getOverallModulationLevel() const {
    float total = (dopamineLevel_ + serotoninLevel_ + 
                   norepinephrineLevel_ + acetylcholineLevel_);
    return std::clamp(total / 4.0f, 0.0f, 1.0f);
}

float AgentBrain::getMemoryStrength() const {
    return memoryStrength_;
}

void AgentBrain::setMemoryStrength(float strength) {
    memoryStrength_ = std::clamp(strength, 0.0f, 2.0f);
}

bool AgentBrain::isDevelopmentEnabled() const {
    return developmentEnabled_;
}

bool AgentBrain::isCuriosityEnabled() const {
    return curiosityEnabled_;
}

bool AgentBrain::isRewardModulationEnabled() const {
    return rewardModulationEnabled_;
}

bool AgentBrain::isStructuralPlasticityEnabled() const {
    return structuralPlasticityEnabled_;
}

bool AgentBrain::isNoveltyDetectionEnabled() const {
    return noveltyDetectionEnabled_;
}

bool AgentBrain::isPredictionErrorEnabled() const {
    return predictionErrorEnabled_;
}

float AgentBrain::getSensoryNoveltyDecay() const {
    return sensoryNoveltyDecay_;
}

void AgentBrain::setSensoryNoveltyDecay(float decay) {
    sensoryNoveltyDecay_ = std::clamp(decay, 0.0f, 1.0f);
}

std::string AgentBrain::getStateCache(const std::string& key) const {
    auto it = stateCache_.find(key);
    if (it != stateCache_.end()) {
        return it->second;
    }
    return "";
}

void AgentBrain::setStateCache(const std::string& key, const std::string& value) {
    stateCache_[key] = value;
}

std::string AgentBrain::getLastStateFingerprint() const {
    return lastStateFingerprint_;
}

size_t AgentBrain::getCurrentEpisodeLength() const {
    return currentEpisodeLength_;
}

uint64_t AgentBrain::getEpisodeStartStep() const {
    return episodeStartStep_;
}

void AgentBrain::setCurrentEpisodeLength(size_t length) {
    currentEpisodeLength_ = length;
}

void AgentBrain::setEpisodeStartStep(uint64_t step) {
    episodeStartStep_ = step;
}

void AgentBrain::incrementEpisodeLength() {
    if (config_.enableEpisodeManagement) {
        currentEpisodeLength_++;
    }
}

bool AgentBrain::isEpisodeComplete() const {
    if (!config_.enableEpisodeManagement) return false;
    return currentEpisodeLength_ >= config_.maxEpisodeLength;
}

void AgentBrain::forceEndEpisode() {
    if (config_.enableEpisodeManagement) {
        resetEpisode();
    }
}

float AgentBrain::getPlasticityModifier() const {
    return plasticityModifier_;
}

void AgentBrain::setPlasticityModifier(float modifier) {
    plasticityModifier_ = std::clamp(modifier, config_.plasticityMinFactor, config_.plasticityMaxFactor);
}

float AgentBrain::getMemoryCurrentStrength() const {
    return memoryStrength_;
}

void AgentBrain::setMemoryCurrentStrength(float strength) {
    memoryStrength_ = std::clamp(strength, 0.0f, 2.0f);
}

float AgentBrain::getNoveltySensitivity() const {
    return config_.noveltySensitivity;
}

void AgentBrain::setNoveltySensitivity(float sensitivity) {
    config_.noveltySensitivity = std::max(0.0f, sensitivity);
}

float AgentBrain::getPredictionErrorWeight() const {
    return config_.predictionErrorWeight;
}

void AgentBrain::setPredictionErrorWeight(float weight) {
    config_.predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

float AgentBrain::getExplorationMultiplier() const {
    return config_.explorationMultiplier;
}

void AgentBrain::setExplorationMultiplier(float multiplier) {
    config_.explorationMultiplier = std::max(1.0f, multiplier);
}

float AgentBrain::getCuriosityThreshold() const {
    return config_.curiosityThreshold;
}

void AgentBrain::setCuriosityThreshold(float threshold) {
    config_.curiosityThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float AgentBrain::getAttentionLevel() const {
    return attentionLevel_;
}

void AgentBrain::setAttentionLevel(float level) {
    attentionLevel_ = std::clamp(level, 0.0f, 1.0f);
}

float AgentBrain::getUncertainty() const {
    return uncertainty_;
}

void AgentBrain::setUncertainty(float uncertainty) {
    uncertainty_ = std::clamp(uncertainty, 0.0f, 1.0f);
}

std::vector<float> AgentBrain::getPreviousVision() const {
    return previousVision_;
}

void AgentBrain::setPreviousVision(const std::vector<float>& vision) {
    previousVision_ = vision;
}

void AgentBrain::setRewardModulationEnabled(bool enabled) {
    rewardModulationEnabled_ = enabled;
    config_.rewardModulationEnabled = enabled;
}

void AgentBrain::setStructuralPlasticityEnabled(bool enabled) {
    structuralPlasticityEnabled_ = enabled;
    config_.structuralPlasticityEnabled = enabled;
}

void AgentBrain::setDevelopmentEnabled(bool enabled) {
    developmentEnabled_ = enabled;
    config_.developmentEnabled = enabled;
}

void AgentBrain::setCuriosityEnabled(bool enabled) {
    curiosityEnabled_ = enabled;
    config_.curiosityEnabled = enabled;
}

void AgentBrain::setNoveltyDetectionEnabled(bool enabled) {
    noveltyDetectionEnabled_ = enabled;
    config_.noveltyDetectionEnabled = enabled;
}

void AgentBrain::setPredictionErrorEnabled(bool enabled) {
    predictionErrorEnabled_ = enabled;
    config_.predictionErrorEnabled = enabled;
}

bool AgentBrain::isAdaptivePlasticityEnabled() const {
    return config_.adaptivePlasticity;
}

void AgentBrain::setAdaptivePlasticityEnabled(bool enabled) {
    config_.adaptivePlasticity = enabled;
}

float AgentBrain::getPlasticityMaxFactor() const {
    return config_.plasticityMaxFactor;
}

void AgentBrain::setPlasticityMaxFactor(float factor) {
    config_.plasticityMaxFactor = factor;
}

float AgentBrain::getPlasticityMinFactor() const {
    return config_.plasticityMinFactor;
}

void AgentBrain::setPlasticityMinFactor(float factor) {
    config_.plasticityMinFactor = factor;
}

bool AgentBrain::isStateCheckpointingEnabled() const {
    return config_.enableStateCheckpointing;
}

void AgentBrain::setStateCheckpointingEnabled(bool enabled) {
    config_.enableStateCheckpointing = enabled;
}

bool AgentBrain::isEpisodeManagementEnabled() const {
    return config_.enableEpisodeManagement;
}

void AgentBrain::setEpisodeManagementEnabled(bool enabled) {
    config_.enableEpisodeManagement = enabled;
}

size_t AgentBrain::getMaxEpisodeLength() const {
    return config_.maxEpisodeLength;
}

void AgentBrain::setMaxEpisodeLength(size_t length) {
    config_.maxEpisodeLength = length;
}

bool AgentBrain::isSensoryCompressionEnabled() const {
    return config_.enableSensoryCompression;
}

void AgentBrain::setSensoryCompressionEnabled(bool enabled) {
    config_.enableSensoryCompression = enabled;
}

bool AgentBrain::isMotorSelectionOptimizationEnabled() const {
    return config_.enableMotorSelectionOptimization;
}

void AgentBrain::setMotorSelectionOptimizationEnabled(bool enabled) {
    config_.enableMotorSelectionOptimization = enabled;
}

std::string AgentBrain::getConfigValidationErrors() const {
    return config_.getValidationErrors();
}

bool AgentBrain::validateConfig() const {
    return config_.validate();
}

void AgentBrain::resetConfigToDefaults() {
    config_.resetToDefaults();
    updateConfig(config_);
}

AgentBrain::Config AgentBrain::getCurrentConfig() const {
    return config_;
}

std::string AgentBrain::getDefaultConfigString() const {
    return Config::getDefaultConfig().summary();
}

} // namespace nlm