#include "examples/BrainWithAccessPatterns.hpp"
#include "core/SystemInitializer.hpp"
#include "core/LoggerHelper.hpp"
#include "core/MathUtils.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Refactoring Example ===\n" << std::endl;
    
    // Initialize logger
    LoggerHelper::info("Starting NLM example with new access patterns");
    
    // Create configuration (simplified)
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("working_memory_capacity", 100);
    config->set("episodic_memory_max_episodes", 1000);
    
    // Create brain with new access patterns
    auto brain = std::make_unique<BrainWithAccessPatterns>(config);
    
    // Initialize using SystemInitializer
    if (brain->initialize()) {
        LoggerHelper::info("Brain initialization successful");
    } else {
        LoggerHelper::error("Brain initialization failed");
        return 1;
    }
    
    // Demonstrate new access patterns
    brain->demonstrateAccessPatterns();
    
    // Example: Using MathUtils for calculations
    std::cout << "\n=== MathUtils Examples ===" << std::endl;
    std::vector<float> values = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    
    float mean = MathUtils::mean(values);
    float median = MathUtils::median(values);
    float stddev = MathUtils::standardDeviation(values, mean);
    
    std::cout << "Values: [1.0, 2.0, 3.0, 4.0, 5.0]" << std::endl;
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Median: " << median << std::endl;
    std::cout << "Standard Deviation: " << stddev << std::endl;
    
    // Example: Using activation functions
    std::cout << "\n=== Activation Function Examples ===" << std::endl;
    float input = 2.0f;
    float sigmoid = MathUtils::sigmoid(input);
    float relu = MathUtils::relu(input);
    float tanh_val = MathUtils::tanh(input);
    
    std::cout << "Input: " << input << std::endl;
    std::cout << "Sigmoid: " << sigmoid << std::endl;
    std::cout << "ReLU: " << relu << std::endl;
    std::cout << "Tanh: " << tanh_val << std::endl;
    
    // Example: Using distance calculations
    std::vector<float> point1 = {1.0f, 2.0f, 3.0f};
    std::vector<float> point2 = {4.0f, 5.0f, 6.0f};
    
    float euclidean = MathUtils::euclideanDistance(point1, point2);
    float manhattan = MathUtils::manhattanDistance(point1, point2);
    float cosine = MathUtils::cosineSimilarity(point1, point2);
    
    std::cout << "\n=== Distance Calculation Examples ===" << std::endl;
    std::cout << "Point 1: [1.0, 2.0, 3.0]" << std::endl;
    std::cout << "Point 2: [4.0, 5.0f, 6.0]" << std::endl;
    std::cout << "Euclidean Distance: " << euclidean << std::endl;
    std::cout << "Manhattan Distance: " << manhattan << std::endl;
    std::cout << "Cosine Similarity: " << cosine << std::endl;
    
    // Example: Using TimerUtils
    std::cout << "\n=== TimerUtils Examples ===" << std::endl;
    
    TimerUtils::Timer timer;
    timer.start();
    
    // Simulate work
    std::vector<float> largeVector(10000, 1.0f);
    for (float val : largeVector) {
        // Do some work
        float processed = MathUtils::sigmoid(val * 0.1f);
    }
    
    timer.stop();
    
    std::cout << "Processing 10,000 elements took " << TimerUtils::formatTime(timer.elapsedSeconds()) << std::endl;
    
    // Example: Using ScopedTimer
    std::cout << "\n=== ScopedTimer Examples ===" << std::endl;
    {
        TimerUtils::ScopedTimer scopedTimer("ComplexCalculation");
        
        // Simulate complex calculation
        std::vector<float> data1(5000, 1.0f);
        std::vector<float> data2(5000, 2.0f);
        
        float result = MathUtils::dotProduct(data1, data2);
        float average = MathUtils::mean(data1);
        
        // Log the results
        LoggerHelper::info("Calculation result: " + std::to_string(result) + ", average: " + std::to_string(average));
    }
    
    std::cout << "\n=== Example Complete ===" << std::endl;
    LoggerHelper::info("NLM refactoring example completed successfully");
    
    return 0;
}
