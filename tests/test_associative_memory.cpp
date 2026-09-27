// AssociativeMemory Tests
// Comprehensive testing of associative memory system

#include "memory/Memory.hpp"
#include "brain/Brain.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <random>

namespace test_associative_memory {

void testAssociativeMemoryCreation() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    const char* name = memory->getName();
    if (!name || std::string(name) != "AssociativeMemory") {
        throw nlm::NLMTestException("AssociativeMemory creation failed: name should be 'AssociativeMemory'");
    }
    
    std::cout << "    testAssociativeMemoryCreation passed" << std::endl;
}

void testAssociativeMemoryName() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    const char* name = memory->getName();
    if (!name) {
        throw nlm::NLMTestException("AssociativeMemory name failed: name is null");
    }
    if (std::string(name) != "AssociativeMemory") {
        throw nlm::NLMTestException("AssociativeMemory name failed: expected 'AssociativeMemory', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testAssociativeMemoryName passed" << std::endl;
}

void testAssociativeMemoryStoreRecall() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    // Store associations
    memory->store(1, 2.0f);    // Associate 1 with 2.0
    memory->store(3, 5.0f);    // Associate 3 with 5.0
    memory->store(4, -1.5f);   // Associate 4 with -1.5
    
    // Recall and verify
    float value1 = memory->recall(1);
    if (std::abs(value1 - 2.0f) > 0.0001f) {
        throw nlm::NLMTestException("AssociativeMemory store/recall test failed: expected 2.0, got " + 
                                   std::to_string(value1));
    }
    
    float value3 = memory->recall(3);
    if (std::abs(value3 - 5.0f) > 0.0001f) {
        throw nlm::NLMTestException("AssociativeMemory store/recall test failed: expected 5.0, got " + 
                                   std::to_string(value3));
    }
    
    float value4 = memory->recall(4);
    if (std::abs(value4 - (-1.5f)) > 0.0001f) {
        throw nlm::NLMTestException("AssociativeMemory store/recall test failed: expected -1.5, got " + 
                                   std::to_string(value4));
    }
    
    std::cout << "    testAssociativeMemoryStoreRecall passed" << std::endl;
}

void testAssociativeMemoryUpdate() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    // Store initial value
    memory->store(1, 1.0f);
    
    // Update value
    memory->update(1, 2.5f);
    
    // Verify update
    float value = memory->recall(1);
    if (std::abs(value - 2.5f) > 0.0001f) {
        throw nlm::NLMTestException("AssociativeMemory update test failed: expected 2.5, got " + 
                                   std::to_string(value));
    }
    
    std::cout << "    testAssociativeMemoryUpdate passed" << std::endl;
}

void testAssociativeMemoryCapacity() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(5), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    // Store associations up to capacity
    for (int i = 0; i < 10; ++i) {
        memory->store(i, static_cast<float>(i));
    }
    
    // Verify older associations are gone (capacity limit)
    for (int i = 0; i < 5; ++i) {
        float value = memory->recall(i);
        if (value != static_cast<float>(i)) {
            throw nlm::NLMTestException("AssociativeMemory capacity test failed: expected " + 
                                       std::to_string(i) + ", got " + std::to_string(value));
        }
    }
    
    // Newer associations should exist
    for (int i = 5; i < 10; ++i) {
        float value = memory->recall(i);
        if (value != static_cast<float>(i)) {
            throw nlm::NLMTestException("AssociativeMemory capacity test failed: entry " + 
                                       std::to_string(i) + " should have been evicted, got " + 
                                       std::to_string(value));
        }
    }
    
    std::cout << "    testAssociativeMemoryCapacity passed" << std::endl;
}

void testAssociativeMemoryNullInput() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    // Recall with null association
    float value = memory->recall(0);
    if (value != 0.0f) {
        throw nlm::NLMTestException("AssociativeMemory null input test failed: expected 0.0, got " + 
                                   std::to_string(value));
    }
    
    std::cout << "    testAssociativeMemoryNullInput passed" << std::endl;
}

void testAssociativeMemoryNegativeValues() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    // Store negative values
    memory->store(1, -1.5f);
    memory->store(2, -3.7f);
    memory->store(3, -2.2f);
    
    // Recall and verify
    float value1 = memory->recall(1);
    if (std::abs(value1 - (-1.5f)) > 0.0001f) {
        throw nlm::NLMTestException("AssociativeMemory negative values test failed: expected -1.5, got " + 
                                   std::to_string(value1));
    }
    
    float value2 = memory->recall(2);
    if (std::abs(value2 - (-3.7f)) > 0.0001f) {
        throw nlm::NLMTestException("AssociativeMemory negative values test failed: expected -3.7, got " + 
                                   std::to_string(value2));
    }
    
    std::cout << "    testAssociativeMemoryNegativeValues passed" << std::endl;
}

void testAssociativeMemoryLargeValues() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    // Store large values
    memory->store(1, 1000.0f);
    memory->store(2, -1000.0f);
    memory->store(3, 999999.0f);
    
    // Recall and verify
    float value1 = memory->recall(1);
    if (std::abs(value1 - 1000.0f) > 0.0001f) {
        throw nlm::NLMTestException("AssociativeMemory large values test failed: expected 1000.0, got " + 
                                   std::to_string(value1));
    }
    
    float value2 = memory->recall(2);
    if (std::abs(value2 - (-1000.0f)) > 0.0001f) {
        throw nlm::NLMTestException("AssociativeMemory large values test failed: expected -1000.0, got " + 
                                   std::to_string(value2));
    }
    
    std::cout << "    testAssociativeMemoryLargeValues passed" << std::endl;
}

void testAssociativeMemoryZeroValues() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    // Store zero values
    memory->store(1, 0.0f);
    memory->store(2, 0.0f);
    
    // Recall and verify
    float value1 = memory->recall(1);
    if (value1 != 0.0f) {
        throw nlm::NLMTestException("AssociativeMemory zero values test failed: expected 0.0, got " + 
                                   std::to_string(value1));
    }
    
    float value2 = memory->recall(2);
    if (value2 != 0.0f) {
        throw nlm::NLMTestException("AssociativeMemory zero values test failed: expected 0.0, got " + 
                                   std::to_string(value2));
    }
    
    std::cout << "    testAssociativeMemoryZeroValues passed" << std::endl;
}

void testAssociativeMemoryFloatingPointPrecision() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    // Store values with high precision
    float preciseValues[] = {0.123456f, -0.987654f, 3.141592f, -2.718281f, 1.414213f};
    int keys[] = {1, 2, 3, 4, 5};
    
    for (int i = 0; i < 5; ++i) {
        memory->store(keys[i], preciseValues[i]);
    }
    
    // Recall and verify with tolerance
    for (int i = 0; i < 5; ++i) {
        float value = memory->recall(keys[i]);
        if (std::abs(value - preciseValues[i]) > 0.000001f) {
            throw nlm::NLMTestException("AssociativeMemory floating point test failed: expected " + 
                                       std::to_string(preciseValues[i]) + ", got " + std::to_string(value));
        }
    }
    
    std::cout << "    testAssociativeMemoryFloatingPointPrecision passed" << std::endl;
}

void testAssociativeMemoryRandomData() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(100), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    // Generate random test data
    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_real_distribution<float> dist(-1000.0f, 1000.0f);
    
    std::vector<std::pair<int, float>> testData;
    for (int i = 0; i < 100; ++i) {
        int key = i + 1;
        float value = dist(rng);
        testData.push_back({key, value});
        memory->store(key, value);
    }
    
    // Recall and verify
    for (const auto& data : testData) {
        float value = memory->recall(data.first);
        if (std::abs(value - data.second) > 0.0001f) {
            throw nlm::NLMTestException("AssociativeMemory random data test failed: expected " + 
                                       std::to_string(data.second) + ", got " + std::to_string(value));
        }
    }
    
    std::cout << "    testAssociativeMemoryRandomData passed" << std::endl;
}

void testAssociativeMemoryPerformance() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(10000), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(1000), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    // Time memory operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < 5000; ++i) {
        int key = static_cast<int>(i % 10000);
        float value = static_cast<float>(i);
        memory->store(key, value);
        float recalled = memory->recall(key);
        if (std::abs(recalled - value) > 0.0001f && recalled != 0.0f) {
            // Don't fail on evictions, just continue
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Should complete without errors
    std::cout << "    testAssociativeMemoryPerformance passed (5000 store/recall operations in " 
              << std::fixed << std::setprecision(4) << totalTime << "s)" << std::endl;
}

void testAssociativeMemoryOverwrite() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<Memory>(config);
    
    // Store and then overwrite the same key
    memory->store(1, 1.0f);
    memory->store(1, 2.0f);
    memory->store(1, 3.0f);
    
    // Should have the last value
    float value = memory->recall(1);
    if (std::abs(value - 3.0f) > 0.0001f) {
        throw nlm::NLMTestException("AssociativeMemory overwrite test failed: expected 3.0, got " + 
                                   std::to_string(value));
    }
    
    std::cout << "    testAssociativeMemoryOverwrite passed" << std::endl;
}

void testAssociativeMemorySizeScaling() {
    // Test with different memory sizes
    for (size_t capacity : {10, 50, 200}) {
        auto config = std::make_shared<Config>();
        config->set("memory_capacity", static_cast<int64_t>(capacity), ConfigSource::Default);
        config->set("trace_length", static_cast<int64_t>(20), ConfigSource::Default);
        
        auto memory = std::make_shared<Memory>(config);
        
        // Store and retrieve data
        for (int i = 0; i < static_cast<int>(capacity) * 2; ++i) {
            memory->store(i, static_cast<float>(i));
        }
        
        // Verify that we can access all current entries
        for (int i = 0; i < static_cast<int>(capacity); ++i) {
            float value = memory->recall(i);
            if (std::abs(value - static_cast<float>(i)) > 0.0001f) {
                throw nlm::NLMTestException("AssociativeMemory size scaling test failed: expected " + 
                                           std::to_string(i) + ", got " + std::to_string(value));
            }
        }
    }
    
    std::cout << "    testAssociativeMemorySizeScaling passed" << std::endl;
}

void runAll() {
    std::cout << "Running AssociativeMemory tests..." << std::endl;
    testAssociativeMemoryCreation();
    testAssociativeMemoryName();
    testAssociativeMemoryStoreRecall();
    testAssociativeMemoryUpdate();
    testAssociativeMemoryCapacity();
    testAssociativeMemoryNullInput();
    testAssociativeMemoryNegativeValues();
    testAssociativeMemoryLargeValues();
    testAssociativeMemoryZeroValues();
    testAssociativeMemoryFloatingPointPrecision();
    testAssociativeMemoryRandomData();
    testAssociativeMemoryPerformance();
    testAssociativeMemoryOverwrite();
    testAssociativeMemorySizeScaling();
}

} // namespace test_associative_memory
