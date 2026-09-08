#include "LearningRateAdapter.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "Testing Learning Rate Adapter...\n\n";
    
    // Create learning rate adapter
    nlm::LearningRateAdapter adapter;
    
    // Initialize with configuration
    adapter.initialize(
        true,                    // enabled
        nlm::AdaptationAlgorithm::Homeostatic,  // algorithm
        0.01f,                   // adaptation rate
        0.1f,                    // stability threshold
        0.1f,                    // max learning rate
        1e-4f,                   // min learning rate
        0.01f                    // base learning rate
    );
    
    // Display status
    std::cout << adapter.getStatus() << std::endl;
    
    // Test with a synapse ID
    nlm::SynapseId testSynapse(1);
    
    // Simulate some weight changes, stability, and performance data
    std::vector<float> weightHistory = {0.1f, 0.12f, 0.15f, 0.18f, 0.22f};
    std::vector<float> stabilityHistory = {1.0f, 0.9f, 0.8f, 0.7f, 0.6f};
    std::vector<float> performanceHistory = {0.5f, 0.6f, 0.7f, 0.8f, 0.9f};
    
    // Update learning rate
    adapter.update(testSynapse, weightHistory, stabilityHistory, performanceHistory, 0.001f);
    
    // Get and display current learning rate
    float learningRate = adapter.getLearningRate(testSynapse);
    std::cout << "Learning rate for synapse " << testSynapse.index() << ": " << learningRate << std::endl;
    
    // Get statistics
    std::cout << "Average weight change: " << adapter.getAverageWeightChange(testSynapse) << std::endl;
    std::cout << "Stability measure: " << adapter.getStabilityMeasure(testSynapse) << std::endl;
    std::cout << "Performance metric: " << adapter.getPerformanceMetric(testSynapse) << std::endl;
    
    // Test algorithm selection
    std::cout << "\nTesting different algorithms:\n";
    std::cout << "Current algorithm: " << adapter.getAlgorithmName() << std::endl;
    
    // Change algorithm to RMSprop
    adapter.setAlgorithm(nlm::AdaptationAlgorithm::RMSprop);
    std::cout << "Changed algorithm to: " << adapter.getAlgorithmName() << std::endl;
    
    // Reset and test again
    adapter.reset(testSynapse);
    std::cout << "Reset synapse data. Current learning rate: " << adapter.getLearningRate(testSynapse) << std::endl;
    
    return 0;
}
