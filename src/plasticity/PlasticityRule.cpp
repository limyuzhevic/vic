#include "PlasticityRule.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {
    // Constants for plasticity rules - improve code maintainability
    constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f;
    constexpr float COINCIDENCES_TIME_WINDOW = 10.0f;  // 10ms window for coincident spikes
    constexpr float MAX_HEBBIAN_COORDINATIONS = 100.0f;  // Maximum weight change

    // HebbianRule implementation
    struct HebbianRule::Impl {
        float learningRate = DEFAULT_HEBBIAN_LEARNING_RATE;
        
        Impl() = default;
    };

    HebbianRule::HebbianRule() : pImpl(new Impl) {}

    HebbianRule::~HebbianRule() = default;

    void HebbianRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
        // TODO PHASE 2: Implement real Hebbian learning
        // PLACEHOLDER: Simple correlated firing increases weight
        
        // Validate inputs - prevent crashes from null pointers
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Count coincident spikes (simplified)
        size_t coincidences = 0;
        for (Timestamp pre : preSpikes) {
            for (Timestamp post : postSpikes) {
                if (std::abs(pre - post) < COINCIDENCES_TIME_WINDOW) {  // 10ms window
                    ++coincidences;
                }
            }
        }
        
        // Apply weight change proportional to coincidences
        if (coincidences > 0) {
            float maxWeightChange = MAX_HEBBIAN_COORDINATIONS * pImpl->learningRate * static_cast<float>(coincidences);
            applyWeightChange(synapse, maxWeightChange);
        }
    }

    void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
        // Safety check for null synapse
        if (synapse) {
            synapse->addToWeight(delta);
        }
    }

    const char* HebbianRule::getName() const {
        return "Hebbian";
    }

    void HebbianRule::setLearningRate(float rate) {
        // Validate learning rate - prevent negative values
        if (pImpl && rate >= 0.0f) {
            pImpl->learningRate = rate;
        }
    }

    float HebbianRule::getLearningRate() const {
        return pImpl ? pImpl->learningRate : DEFAULT_HEBBIAN_LEARNING_RATE;
    }

} // namespace nlm