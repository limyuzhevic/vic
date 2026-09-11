#include "NeuromodulatorFactory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "Brain.hpp"

namespace nlm {

NeuromodulatorFactory::NeuromodulatorFactory() {}

NeuromodulatorFactory::~ NeuromodulatorFactory() {}

void NeuromodulatorFactory::initializeAll(Brain* brain) {
    NLM_LOG_INFO("Initializing all neuromodulators");
    
    // Create neuromodulators
    dopamine_ = std::make_unique<Dopamine>();
    acetylcholine_ = std::make_unique<Acetylcholine>();
    norepinephrine_ = std::make_unique<Norepinephrine>();
    serotonin_ = std::make_unique<Serotonin>();
    curiosity_ = std::make_unique<Curiosity>();
    novelty_ = std::make_unique<Novelty>();
    predictionError_ = std::make_unique<PredictionError>();
    reward_ = std::make_unique<Reward>();
    
    // Initialize with brain reference
    if (brain) {
        curiosity_->initialize(brain);
        novelty_->initialize(brain);
        predictionError_->initialize(brain);
        // Note: Other neuromodulators may not need initialization yet
    }
    
    // Fill neuromodulators vector
    neuromodulators_.clear();
    if (dopamine_) neuromodulators_.push_back(dopamine_.get());
    if (acetylcholine_) neuromodulators_.push_back(acetylcholine_.get());
    if (norepinephrine_) neuromodulators_.push_back(norepinephrine_.get());
    if (serotonin_) neuromodulators_.push_back(serotonin_.get());
    if (curiosity_) neuromodulators_.push_back(curiosity_.get());
    if (novelty_) neuromodulators_.push_back(novelty_.get());
    if (predictionError_) neuromodulators_.push_back(predictionError_.get());
    if (reward_) neuromodulators_.push_back(reward_.get());
    
    NLM_LOG_INFO("All neuromodulators initialized (" + std::to_string(neuromodulators_.size()) + " active)");
}

} // namespace nlm
