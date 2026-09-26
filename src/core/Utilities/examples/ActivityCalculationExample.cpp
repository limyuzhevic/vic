// ============================================================================
// ACTIVITY CALCULATION UTILITIES EXAMPLE
// ============================================================================

#include <iostream>
#include <vector>
#include <memory>
#include "Utilities.hpp"

// Mock Neuron class for demonstration
class MockNeuron {
public:
    struct State {
        float membranePotential = 0.0f;
        float restingPotential = -65.0f;
    };
    
    MockNeuron(float membranePot) : state_{membranePot, -65.0f} {}
    State getState() const { return state_; }
private:
    State state_;
};

int main() {
    std::cout << "=== Activity Calculation Utilities Example ===\n";
    
    // Create mock neurons with different activities
    std::vector<MockNeuron*> neurons;
    neurons.push_back(new MockNeuron(0.0f));   // Low activity
    neurons.push_back(new MockNeuron(-35.0f)); // High activity
    neurons.push_back(new MockNeuron(-25.0f)); // Medium activity
    neurons.push_back(new MockNeuron(-60.0f)); // Very low activity
    
    // Example 1: Calculate average activity
    float avgActivity = nlm::calculateAverageActivity(neurons);
    std::cout << "Average neural activity: " << avgActivity << "\n";
    
    // Example 2: Calculate maximum activity
    float maxActivity = nlm::calculateMaxActivity(neurons);
    std::cout << "Maximum neural activity: " << maxActivity << "\n";
    
    // Example 3: Calculate weighted activity
    std::vector<float> weights = {0.1f, 0.5f, 0.3f, 0.1f};
    float weightedActivity = nlm::calculateWeightedActivity(neurons, weights);
    std::cout << "Weighted neural activity (weights 0.1, 0.5, 0.3, 0.1): " << weightedActivity << "\n";
    
    // Cleanup
    for (auto* neuron : neurons) {
        delete neuron;
    }
    
    return 0;
}
