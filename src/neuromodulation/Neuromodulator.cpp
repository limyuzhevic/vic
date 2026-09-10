#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

void Dopamine::signal(Brain* brain) override {
    boost::shared_ptr<Brain> brainPtr(brain);
    // Apply dopamine effects on neural excitability
    float dopamineLevel = getLevel();
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Dopamine modulates excitability by injecting additional current
                // Higher dopamine increases excitability (lower effective threshold)
                float excitabilityMod = dopamineLevel * 0.5f;
                if (excitabilityMod > 0.0f) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void Acetylcholine::signal(Brain* brain) override {
    boost::shared_ptr<Brain> brainPtr(brain);
    signalAttention(brainPtr);
}

void Norepinephrine::signal(Brain* brain) override {
    boost::shared_ptr<Brain> brainPtr(brain);
    signalArousal(1.0f, brainPtr);
}

void Serotonin::signal(Brain* brain) override {
    boost::shared_ptr<Brain> brainPtr(brain);
    signalMoodChange(1.0f, brainPtr);
}

void Curiosity::signal(Brain* brain) override {
    // Curiosity drives exploration by biasing neural activity towards novel actions
    float curiosityLevel = getLevel();
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            // Bias motor populations towards exploration
            for (auto* neuron : pop->getNeurons()) {
                // Curiosity increases exploration bias
                neuron->injectCurrent(curiosityLevel * 0.1f);
            }
        }
    }
}

void Novelty::signal(Brain* brain) override {
    // Novelty detection influences attention and novelty-seeking behavior
    float noveltyLevel = getLevel();
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            // Novelty increases salience of sensory processing
            for (auto* neuron : pop->getNeurons()) {
                // Novelty enhances sensory processing
                neuron->injectCurrent(noveltyLevel * 0.2f);
            }
        }
    }
}

void PredictionError::signal(Brain* brain) override {
    // Prediction error drives learning and behavioral adaptation
    float predictionErrorLevel = getLevel();
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            // Prediction error modulates plasticity in all regions
            for (auto* neuron : pop->getNeurons()) {
                // Prediction error modulates learning rate
                neuron->injectCurrent(predictionErrorLevel * 0.3f);
            }
        }
    }
}

} // namespace nlm
