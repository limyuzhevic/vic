#pragma once

#include "../brain/Neuron.hpp"
#include <vector>
#include <functional>

namespace nlm {

enum class MotorCommand;

class MotorDecoder {
public:
    MotorDecoder();
    ~MotorDecoder();
    
    // Decode motor command from neural activity
    MotorCommand decodeFromMotorNeurons(
        const std::vector<Neuron*>& motorForward,
        const std::vector<Neuron*>& motorBackward,
        const std::vector<Neuron*>& motorTurnLeft,
        const std::vector<Neuron*>& motorTurnRight,
        const std::vector<Neuron*>& motorInteract,
        const std::vector<Neuron*>& motorWait
    );
    
    // Calculate neural activity for a group of neurons
    static float calculateActivity(const std::vector<Neuron*>& neurons);
    
    // Get motor command from activity comparison
    static MotorCommand selectCommandFromActivity(
        float forwardAct, float backwardAct, float leftAct, 
        float rightAct, float interactAct, float waitAct
    );
    
    // Check if activity is meaningful
    static bool isMeaningfulActivity(float activity, float threshold = 0.5f);

private:
    // Helper lambda for activity calculation
    struct ActivityCalculator {
        static float calc(const std::vector<Neuron*>& neurons) {
            if (neurons.empty()) return 0.0f;
            float sum = 0.0f;
            for (Neuron* n : neurons) {
                // Use membrane potential deviation from rest as activity measure
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
            return sum / neurons.size();
        }
    };
};

} // namespace nlm