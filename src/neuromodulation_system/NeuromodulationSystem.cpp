#include "NeuromodulationSystem.h"
#include "core/Logger/Logger.hpp"

namespace nlm {

class NeuromodulationSystemImpl : public NeuromodulationSystem {
private:
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
    
public:
    NeuromodulationSystemImpl() {
        NLM_LOG_INFO("NeuromodulationSystem initialized");
    }
    
    void initialize(Brain* brain) override {
        brain_ = brain;
        
        dopamine_ = std::make_unique<Dopamine>();
        curiosity_ = std::make_unique<Curiosity>();
        novelty_ = std::make_unique<Novelty>();
        predictionError_ = std::make_unique<PredictionError>();
        
        if (dopamine_) dopamine_->initialize(brain_);
        if (curiosity_) curiosity_->initialize(brain_);
        if (novelty_) novelty_->initialize(brain_);
        if (predictionError_) predictionError_->initialize(brain_);
        
        NLM_LOG_INFO("Neuromodulation systems initialized successfully");
    }
    
    void update(TimestepDuration timestep) override {
        // Update individual neuromodulators
        if (dopamine_) {
            dopamine_->update(timestep);
        }
        
        if (curiosity_) {
            curiosity_->update(timestep);
        }
        
        if (novelty_) {
            novelty_->update(timestep);
        }
        
        if (predictionError_) {
            predictionError_->update(timestep);
        }
        
        // Apply neuromodulatory effects
        float dopamineLevel = getDopamineLevel();
        applyDopamineToNeurons(brain_->getRegions(), dopamineLevel);
        
        float noveltyLevel = getNoveltyLevel();
        applyNoveltyToLearning(noveltyLevel);
        
        float curiosityLevel = getCuriosityLevel();
        if (curiosityLevel > 0.0f) {
            applyCuriosityToExploration();
        }
        
        float predictionErrorLevel = getPredictionErrorLevel();
        if (predictionErrorLevel != 0.0f) {
            applyPredictionErrorToPlasticity(predictionErrorLevel);
        }
        
        NLM_LOG_DEBUG("Neuromodulation update complete: DA=" + 
                     std::to_string(dopamineLevel) + 
                     " Nov=" + std::to_string(noveltyLevel) + 
                     " Cur=" + std::to_string(curiosityLevel) + 
                     " PE=" + std::to_string(predictionErrorLevel));
    }
    
    // Neuromodulator accessors
    Dopamine* getDopamine() override {
        return dopamine_.get();
    }
    
    Curiosity* getCuriosity() override {
        return curiosity_.get();
    }
    
    Novelty* getNovelty() override {
        return novelty_.get();
    }
    
    PredictionError* getPredictionError() override {
        return predictionError_.get();
    }
    
    // Neuromodulatory effects
    void applyDopamineToNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                               float dopamineLevel) override {
        float excitabilityMod = dopamineLevel * 0.5f;
        if (excitabilityMod > 0.0f) {
            NLM_LOG_DEBUG("Applying dopamine modulation (level=" + 
                         std::to_string(dopamineLevel) + ") to " + 
                         std::to_string(countNeurons(regions)) + " neurons");
            
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    void applyNoveltyToLearning(float noveltyLevel) override {
        if (noveltyLevel > 0.0f) {
            NLM_LOG_DEBUG("Applying novelty modulation (level=" + 
                         std::to_string(noveltyLevel) + ") to learning processes");
            // Novelty affects plasticity - could enhance learning rate
            if (brain_ && brain_->getSTDP()) {
                brain_->getSTDP()->setLTPWeight(brain_->getSTDP()->getLTPWeight() * (1.0f + noveltyLevel * 0.1f));
            }
        }
    }
    
    void applyCuriosityToExploration() override {
        NLM_LOG_DEBUG("Applying curiosity modulation to exploration behavior");
        // Curiosity drives exploration - could increase action selection randomness
    }
    
    void applyPredictionErrorToPlasticity(float predictionError) override {
        if (predictionError != 0.0f) {
            NLM_LOG_DEBUG("Applying prediction error modulation (level=" + 
                         std::to_string(predictionError) + ") to plasticity");
            // Prediction error affects plasticity - used for reward prediction learning
            if (brain_ && brain_->getDopamine()) {
                brain_->getDopamine()->setLevel(std::abs(predictionError));
            }
        }
    }
    
    // Get current neuromodulatory states
    float getDopamineLevel() const override {
        return dopamine_ ? dopamine_->getLevel() : 0.0f;
    }
    
    float getNoveltyLevel() const override {
        return novelty_ ? novelty_->getLevel() : 0.0f;
    }
    
    float getCuriosityLevel() const override {
        return curiosity_ ? curiosity_->getLevel() : 0.0f;
    }
    
    float getPredictionErrorLevel() const override {
        return predictionError_ ? predictionError_->getLevel() : 0.0f;
    }
    
private:
    size_t countNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) const {
        size_t count = 0;
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                count += pop->getNeuronCount();
            }
        }
        return count;
    }
};

} // namespace nlm