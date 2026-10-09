// SimpleBrain implementation

#include "SimpleBrain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

SimpleBrain::SimpleBrain() : SimpleBrain(std::make_shared<Config>()) {}

SimpleBrain::SimpleBrain(std::shared_ptr<Config> config) : config_(config), currentStep_(0) {
    brain_ = std::make_unique<Brain>(config_);
}

bool SimpleBrain::initialize(const std::string& configFile) {
    if (configFile.empty()) {
        // Initialize with default configuration
        config_->set("neuron_count", 1000);
        config_->set("region_count", 1);
        config_->set("connection_probability", 0.1f);
        config_->set("simulation_timestep", 0.001);
        config_->set("replay_interval", 100);
        config_->set("consolidation_interval", 1000);
        config_->set("rest_interval", 5000);
        config_->set("random_seed", 42ULL);
    } else {
        // TODO: Load from file
        // For now, use defaults
    }
    
    if (!brain_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return false;
    }
    
    NLM_LOG_INFO("SimpleBrain initialized with " + 
                 std::to_string(brain_->getTotalNeuronCount()) + " neurons");
    return true;
}

void SimpleBrain::step() {
    brain_->step(currentStep_);
    currentStep_++;
}

void SimpleBrain::processSensoryInput(const SensoryInput& input) {
    brain_->receiveSensoryInput(input);
}

ActionType SimpleBrain::getAction() {
    auto action = brain_->produceAction();
    return action->getType();
}

std::string SimpleBrain::getStatus() const {
    std::stringstream ss;
    ss << "=== SimpleBrain Status ===" << std::endl;
    ss << "Neurons: " << brain_->getTotalNeuronCount() << std::endl;
    ss << "Synapses: " << brain_->getTotalSynapseCount() << std::endl;
    ss << "Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
    ss << "Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
    ss << "Average firing rate: " << brain_->getAverageFiringRate() << std::endl;
    ss << "E/I ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
    ss << "Developmental stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
    ss << "Is resting: " << (brain_->isResting() ? "Yes" : "No") << std::endl;
    
    if (brain_->getWorkingMemory()) {
        ss << "Working memory activity: " << brain_->getWorkingMemory()->getMemoryActivity() << std::endl;
    }
    
    if (brain_->getEpisodicMemory()) {
        ss << "Episodic memory size: " << brain_->getEpisodicMemory()->getEpisodeCount() << std::endl;
    }
    
    if (brain_->getPredictionSystem()) {
        ss << "Prediction error: " << brain_->getPredictionSystem()->getPredictionError() << std::endl;
        ss << "Prediction confidence: " << brain_->getPredictionSystem()->getConfidence() << std::endl;
    }
    
    if (brain_->getDopamine()) {
        ss << "Dopamine level: " << brain_->getDopamine()->getLevel() << std::endl;
    }
    
    if (brain_->getCuriosity()) {
        ss << "Curiosity level: " << brain_->getCuriosity()->getLevel() << std::endl;
    }
    
    return ss.str();
}

bool SimpleBrain::save(const std::string& filepath) const {
    return brain_->save(filepath);
}

bool SimpleBrain::load(const std::string& filepath) {
    // Reset first
    reset();
    return brain_->load(filepath);
}

void SimpleBrain::reset() {
    brain_->reset();
    currentStep_ = 0;
}

DevelopmentalStage SimpleBrain::getDevelopmentalStage() const {
    return brain_->getDevelopmentalStage();
}

float SimpleBrain::getDopamineLevel() const {
    auto* dopamine = brain_->getDopamine();
    return dopamine ? dopamine->getLevel() : 0.0f;
}

float SimpleBrain::getCuriosityLevel() const {
    auto* curiosity = brain_->getCuriosity();
    return curiosity ? curiosity->getLevel() : 0.0f;
}

namespace Diagnostics {
    
    BrainMetrics collectMetrics(Brain* brain) {
        BrainMetrics metrics;
        
        // Basic neural statistics
        metrics.neuronCount = brain->getTotalNeuronCount();
        metrics.synapseCount = brain->getTotalSynapseCount();
        metrics.firingRate = brain->getFiringNeuronCount();
        metrics.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        metrics.activeNeurons = brain->getActiveNeuronCount();
        
        // Memory system statistics
        auto* workingMemory = brain->getWorkingMemory();
        metrics.workingMemoryActivity = workingMemory ? workingMemory->getMemoryActivity() : 0.0f;
        
        auto* episodicMemory = brain->getEpisodicMemory();
        metrics.episodicMemorySize = episodicMemory ? episodicMemory->getEpisodeCount() : 0;
        
        // Prediction system statistics
        auto* predictionSystem = brain->getPredictionSystem();
        metrics.predictionError = predictionSystem ? predictionSystem->getPredictionError() : 0.0f;
        metrics.predictionConfidence = predictionSystem ? predictionSystem->getConfidence() : 0.0f;
        
        // Neuromodulation statistics
        auto* dopamine = brain->getDopamine();
        metrics.dopamineLevel = dopamine ? dopamine->getLevel() : 0.0f;
        
        auto* curiosity = brain->getCuriosity();
        metrics.curiosityLevel = curiosity ? curiosity->getLevel() : 0.0f;
        
        // Developmental and state
        metrics.developmentalStage = brain->getDevelopmentalStage();
        
        // Scientific metrics
        // Learning progress based on spike count
        metrics.learningProgress = static_cast<float>(brain->getTotalSpikeCount()) / 1000.0f;
        
        // Memory retention based on episode age
        if (episodicMemory) {
            float totalAge = 0.0f;
            size_t count = 0;
            for (size_t i = 0; i < episodicMemory->getEpisodeCount(); ++i) {
                if (const auto* episode = episodicMemory->getEpisode(i)) {
                    totalAge += episode->age;
                    count++;
                }
            }
            metrics.memoryRetention = count > 0 ? 1.0f - (totalAge / count) / 100.0f : 0.0f;
        } else {
            metrics.memoryRetention = 0.0f;
        }
        
        // Novelty processing based on prediction error
        metrics.noveltyProcessing = metrics.predictionError * 2.0f;
        
        // Prediction accuracy based on confidence
        metrics.predictionAccuracy = metrics.predictionConfidence * (1.0f - metrics.predictionError);
        
        // Rest state
        metrics.isResting = brain->isResting();
        
        return metrics;
    }
    
    bool validateBrain(Brain* brain) {
        // Basic validation checks
        if (!brain) {
            return false;
        }
        
        // Check for required systems
        if (!brain->getWorkingMemory()) {
            return false;
        }
        
        if (!brain->getPredictionSystem()) {
            return false;
        }
        
        // Check for reasonable neuron/synapse ratio
        size_t neurons = brain->getTotalNeuronCount();
        size_t synapses = brain->getTotalSynapseCount();
        if (neurons == 0 || synapses / neurons > 100.0f) {  // Arbitrary high threshold
            return false;
        }
        
        // Check firing rate is reasonable
        float avgFiring = brain->getAverageFiringRate();
        if (avgFiring > 100.0f) {  // Very high firing rate might indicate issues
            return false;
        }
        
        return true;
    }
    
    std::vector<float> getLearningCurve(Brain* brain, size_t steps) {
        std::vector<float> curve;
        curve.reserve(steps);
        
        for (size_t i = 0; i < steps; ++i) {
            // Simulate brain state for learning curve
            // In a real implementation, this would track learning over time
            float spikeContribution = static_cast<float>(brain->getTotalSpikeCount()) / (i + 1);
            float memoryContribution = brain->getWorkingMemory() ? 
                                        brain->getWorkingMemory()->getMemoryActivity() : 0.0f;
            
            float learningValue = spikeContribution * 0.6f + memoryContribution * 0.4f;
            curve.push_back(std::min(1.0f, learningValue));
        }
        
        return curve;
    }
    
    std::vector<float> getMemoryRetentionCurve(Brain* brain, size_t timeSteps) {
        std::vector<float> curve;
        curve.reserve(timeSteps);
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (!episodicMemory) {
            // Return empty curve if no episodic memory
            return curve;
        }
        
        for (size_t i = 0; i < timeSteps; ++i) {
            // Calculate retention based on episode ages
            float totalRetention = 0.0f;
            size_t count = 0;
            
            size_t lookback = std::min(size_t(10), episodicMemory->getEpisodeCount());
            for (size_t j = 0; j < lookback; ++j) {
                size_t idx = episodicMemory->getEpisodeCount() - 1 - j;
                if (const auto* episode = episodicMemory->getEpisode(idx)) {
                    float retention = 1.0f / (1.0f + episode->age * 0.1f);
                    totalRetention += retention;
                    count++;
                }
            }
            
            float avgRetention = count > 0 ? totalRetention / count : 0.0f;
            curve.push_back(avgRetention);
        }
        
        return curve;
    }
    
    std::string generateScientificReport(Brain* brain) {
        std::stringstream ss;
        ss << "=== NLM Scientific Report ===" << std::endl;
        ss << "Brain Structure Analysis:" << std::endl;
        ss << "  - Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  - Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  - Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        ss << "  - E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        ss << "\nMemory System Performance:" << std::endl;
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            ss << "  - Working memory activity: " << workingMemory->getMemoryActivity() << std::endl;
        }
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            ss << "  - Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
            ss << "  - Average episode reward: " << episodicMemory->getAverageReward() << std::endl;
        }
        
        ss << "\nPrediction System Status:" << std::endl;
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            ss << "  - Prediction error: " << predictionSystem->getPredictionError() << std::endl;
            ss << "  - Prediction confidence: " << predictionSystem->getConfidence() << std::endl;
            ss << "  - Error history size: " << predictionSystem->getErrorHistory().size() << std::endl;
        }
        
        ss << "\nNeuromodulation Levels:" << std::endl;
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            ss << "  - Dopamine: " << dopamine->getLevel() << std::endl;
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            ss << "  - Curiosity: " << curiosity->getLevel() << std::endl;
        }
        
        auto* novelty = brain->getNovelty();
        if (novelty) {
            ss << "  - Novelty: " << novelty->getLevel() << std::endl;
        }
        
        ss << "\nDevelopment Status:" << std::endl;
        ss << "  - Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        ss << "  - Current step: " << brain->getCurrentStep() << std::endl;
        ss << "  - Current time: " << brain->getCurrentTime() << " ms" << std::endl;
        
        ss << "\nIntegration Features:" << std::endl;
        ss << "  - Working memory: " << (brain->getWorkingMemory() ? "✅ Active" : "❌ Inactive") << std::endl;
        ss << "  - Episodic memory: " << (brain->getEpisodicMemory() ? "✅ Active" : "❌ Inactive") << std::endl;
        ss << "  - Prediction system: " << (brain->getPredictionSystem() ? "✅ Active" : "❌ Inactive") << std::endl;
        ss << "  - Cognition systems: " << (brain->getPlanner() ? "✅ Active" : "❌ Inactive") << std::endl;
        ss << "  - Neuromodulation: " << (brain->getDopamine() ? "✅ Active" : "❌ Inactive") << std::endl;
        
        ss << "\nScientific Validation:" << std::endl;
        auto metrics = collectMetrics(brain);
        ss << "  - Learning progress: " << metrics.learningProgress << std::endl;
        ss << "  - Memory retention: " << metrics.memoryRetention << std::endl;
        ss << "  - Prediction accuracy: " << metrics.predictionAccuracy << std::endl;
        ss << "  - Validation: " << (validateBrain(brain) ? "✅ Healthy" : "❌ Issues detected") << std::endl;
        
        return ss.str();
    }

} // namespace Diagnostics

} // namespace nlm