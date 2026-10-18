#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

void Brain::applyNeuromodulationInternal(const class Neuromodulator& signal) {
    // Apply neuromodulatory signal to appropriate neural populations
    float modulationStrength = signal.getStrength();
    
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Apply modulation based on neuromodulator type
                switch (signal.getType()) {
                    case NeuromodulatorType::Dopamine:
                        // Dopamine affects neural excitability
                        neuron->injectCurrent(modulationStrength * 0.5f);
                        break;
                    case NeuromodulatorType::Curiosity:
                        // Curiosity increases exploration tendency
                        neuron->increaseExplorationTendency(modulationStrength);
                        break;
                    case NeuromodulatorType::Novelty:
                        // Novelty enhances novelty detection
                        neuron->enhanceNoveltyDetection(modulationStrength);
                        break;
                    case NeuromodulatorType::PredictionError:
                        // Prediction error updates learning rate
                        neuron->updateLearningRate(modulationStrength);
                        break;
                    default:
                        // Generic modulation
                        neuron->injectCurrent(modulationStrength);
                        break;
                }
            }
        }
    }
}

} // namespace nlm