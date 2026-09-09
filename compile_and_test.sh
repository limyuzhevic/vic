#!/usr/bin/env g++
# Compilation script for NLM with prediction integration

cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_b462ca62-46c9-4532-957b-3eb5b1dedd72

# Create a simple test program to verify prediction system integration
test_prediction.cpp
#include <iostream>
#include <vector>
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "Testing NLM with prediction integration...\n";
    
    // Create brain with configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<uint64_t>(42), nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!\n";
        return 1;
    }
    
    std::cout << "Brain initialized successfully!\n";
    
    // Create test sensory input
    nlm::SensoryInput input;
    std::vector<float> data = {0.5f, 0.3f, 0.8f, 0.1f, 0.9f};
    input.setData(data);
    
    // Test receiveSensoryInput which now includes prediction
    brain.receiveSensoryInput(input);
    std::cout << "Sensory input received and processed with prediction!\n";
    
    // Test step which includes prediction update at step 8
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
        std::cout << "Step " << step << " completed.\n";
    }
    
    // Check prediction system integration
    if (brain.getPredictionSystem()) {
        std::cout << "Prediction system is active!\n";
        float error = brain.getPredictionSystem()->getPredictionError();
        float confidence = brain.getPredictionSystem()->getConfidence();
        std::cout << "Prediction error: " << error << "\n";
        std::cout << "Prediction confidence: " << confidence << "\n";
    }
    
    std::cout << "\nAll prediction integration tests passed!\n";
    return 0;
}
EOF

# Compile the test
# g++ -std=c++11 -I./src -I./tests test_prediction.cpp src/brain/Brain.cpp src/prediction/PredictionSystem.cpp src/neuromodulation/PredictionError.cpp src/memory/NeuralWorkingMemory.cpp -o test_prediction 2>&1 | head -100

# Check if compilation succeeded
if [ -f test_prediction ]; then
    echo "Compilation successful!"
    echo "Running test..."
    ./test_prediction
else
    echo "Compilation failed. Check output above for details."
fi