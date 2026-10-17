#include "core/LoggerHelper.hpp"
#include "core/MathUtils.hpp"
#include "core/TimerUtils.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NLM Core Utilities..." << std::endl;
    
    // Test LoggerHelper
    LoggerHelper::info("LoggerHelper test message");
    LoggerHelper::warning("Warning test message");
    LoggerHelper::error("Error test message");
    
    // Test MathUtils
    std::vector<float> values = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    
    float mean = MathUtils::mean(values);
    float median = MathUtils::median(values);
    float stddev = MathUtils::standardDeviation(values, mean);
    
    std::cout << "MathUtils test - mean: " << mean << std::endl;
    std::cout << "MathUtils test - median: " << median << std::endl;
    std::cout << "MathUtils test - stddev: " << stddev << std::endl;
    
    // Test activation functions
    float sigmoid = MathUtils::sigmoid(2.0f);
    float relu = MathUtils::relu(2.0f);
    float tanh_val = MathUtils::tanh(2.0f);
    
    std::cout << "MathUtils test - sigmoid(2.0): " << sigmoid << std::endl;
    std::cout << "MathUtils test - relu(2.0): " << relu << std::endl;
    std::cout << "MathUtils test - tanh(2.0): " << tanh_val << std::endl;
    
    // Test TimerUtils
    TimerUtils::Timer timer;
    timer.start();
    
    // Simulate work
    for (int i = 0; i < 1000; ++i) {
        float processed = MathUtils::sigmoid(static_cast<float>(i) * 0.01f);
    }
    
    timer.stop();
    
    std::cout << "TimerUtils test - elapsed time: " << TimerUtils::formatTime(timer.elapsedSeconds()) << std::endl;
    
    // Test ScopedTimer
    {
        TimerUtils::ScopedTimer scopedTimer("ScopedTimerTest");
        std::vector<float> data1(100, 1.0f);
        std::vector<float> data2(100, 2.0f);
        
        float result = MathUtils::dotProduct(data1, data2);
        std::cout << "ScopedTimer test - dot product result: " << result << std::endl;
    }
    
    // Test distance calculations
    std::vector<float> point1 = {1.0f, 2.0f, 3.0f};
    std::vector<float> point2 = {4.0f, 5.0f, 6.0f};
    
    float euclidean = MathUtils::euclideanDistance(point1, point2);
    float manhattan = MathUtils::manhattanDistance(point1, point2);
    float cosine = MathUtils::cosineSimilarity(point1, point2);
    
    std::cout << "MathUtils test - euclidean distance: " << euclidean << std::endl;
    std::cout << "MathUtils test - manhattan distance: " << manhattan << std::endl;
    std::cout << "MathUtils test - cosine similarity: " << cosine << std::endl;
    
    std::cout << "All core utility tests completed successfully!" << std::endl;
    
    return 0;
}
