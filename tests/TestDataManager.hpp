// Test Data Manager
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <chrono>
#include <cmath>

// Test data management for common test fixtures
class TestDataManager {
public:
    TestDataManager() = default;
    ~TestDataManager() = default;
    
    // Get a simple log message for testing
    static std::string getSimpleLogMessage() {
        return "This is a test log message for unit testing purposes.";
    }
    
    // Get a simple config value for testing
    static std::string getSimpleConfigValue() {
        return "test_value_" + std::to_string(std::rand());
    }
    
    // Get test data for batch processing
    static std::vector<float> getTestBatchData(size_t size = 100) {
        std::vector<float> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<float>(i) * 0.1f;
        }
        return data;
    }
    
    // Get test matrix for performance testing
    static std::vector<std::vector<float>> getTestMatrix(size_t rows = 10, size_t cols = 10) {
        std::vector<std::vector<float>> matrix(rows, std::vector<float>(cols));
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                matrix[i][j] = static_cast<float>(i * cols + j) * 0.01f;
            }
        }
        return matrix;
    }
    
    // Get test string for configuration
    static std::string getTestConfigString() {
        return "{\"test\": \"value\", \"number\": 42, \"enabled\": true}";
    }
    
    // Get test performance data
    static std::vector<double> getTestPerformanceData(size_t size = 100) {
        std::vector<double> data(size);
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<double>(std::rand()) / RAND_MAX * 100.0;
        }
        return data;
    }
    
    // Get test components for integration testing
    static std::unordered_map<std::string, std::string> getTestComponents() {
        return {
            {"core", "Core neural components"},
            {"sensory", "Sensory processing"},
            {"motor", "Motor control"},
            {"memory", "Memory system"},
            {"plasticity", "Synaptic plasticity"}
        };
    }
};
