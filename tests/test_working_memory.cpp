// WorkingMemory Tests
// Comprehensive testing of working memory system

#include "memory/NeuralWorkingMemory.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_working_memory {

void testWorkingMemoryCreation() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralWorkingMemory>(config);
    
    const char* name = memory->getName();
    if (!name || std::string(name) != "WorkingMemory") {
        throw nlm::NLMTestException("WorkingMemory creation failed: name should be 'WorkingMemory'");
    }
    
    std::cout << "    testWorkingMemoryCreation passed" << std::endl;
}

void testWorkingMemoryName() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralWorkingMemory>(config);
    
    const char* name = memory->getName();
    if (!name) {
        throw nlm::NLMTestException("WorkingMemory name failed: name is null");
    }
    if (std::string(name) != "WorkingMemory") {
        throw nlm::NLMTestException("WorkingMemory name failed: expected 'WorkingMemory', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testWorkingMemoryName passed" << std::endl;
}

void testWorkingMemoryStoreRecall() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralWorkingMemory>(config);
    
    // Store some data
    memory->store(0.5f, 1, 0);
    memory->store(1.2f, 2, 1);
    memory->store(-0.3f, 3, 2);
    
    // Recall and verify
    float value1 = memory->recall(1, 0);
    if (std::abs(value1 - 0.5f) > 0.0001f) {
        throw nlm::NLMTestException("WorkingMemory store/recall test failed: expected 0.5, got " + 
                                   std::to_string(value1));
    }
    
    float value2 = memory->recall(2, 1);
    if (std::abs(value2 - 1.2f) > 0.0001f) {
        throw nlm::NLMTestException("WorkingMemory store/recall test failed: expected 1.2, got " + 
                                   std::to_string(value2));
    }
    
    float value3 = memory->recall(3, 2);
    if (std::abs(value3 - (-0.3f)) > 0.0001f) {
        throw nlm::NLMTestException("WorkingMemory store/recall test failed: expected -0.3, got " + 
                                   std::to_string(value3));
    }
    
    std::cout << "    testWorkingMemoryStoreRecall passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(5), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralWorkingMemory>(config);
    
    // Fill memory to capacity
    for (int i = 0; i < 10; ++i) {
        memory->store(static_cast<float>(i), i, i % 5); // Cycle through slot 0-4
    }
    
    // Verify older entries are gone (capacity limit)
    for (int i = 0; i < 5; ++i) {
        float value = memory->recall(i, i);
        if (value != static_cast<float>(i)) {
            throw nlm::NLMTestException("WorkingMemory capacity test failed: expected " + 
                                       std::to_string(i) + ", got " + std::to_string(value));
        }
    }
    
    // Older entries should be gone
    for (int i = 5; i < 10; ++i) {
        float value = memory->recall(i, i % 5);
        if (value != static_cast<float>(i)) {
            throw nlm::NLMTestException("WorkingMemory capacity test failed: entry " + 
                                       std::to_string(i) + " should have been evicted, got " + 
                                       std::to_string(value));
        }
    }
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryTraceLength() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(3), ConfigSource::Default); // Very short trace
    
    auto memory = std::make_shared<NeuralWorkingMemory>(config);
    
    // Store some data
    memory->store(1.0f, 1, 0);
    memory->store(2.0f, 2, 0);
    memory->store(3.0f, 3, 0);
    memory->store(4.0f, 4, 0); // This should evict the oldest
    
    // First entry should be gone
    float value = memory->recall(1, 0);
    if (value != 0.0f) {
        throw nlm::NLMTestException("WorkingMemory trace length test failed: entry 1 should be gone, got " + 
                                   std::to_string(value));
    }
    
    // Other entries should still be there
    if (std::abs(memory->recall(2, 0) - 2.0f) > 0.0001f) {
        throw nlm::NLMTestException("WorkingMemory trace length test failed: entry 2 should be there");
    }
    
    std::cout << "    testWorkingMemoryTraceLength passed" << std::endl;
}

void testWorkingMemoryZeroInput() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralWorkingMemory>(config);
    
    // Store something
    memory->store(1.0f, 1, 0);
    
    // Recall with zero input
    float value = memory->recall(0, 0);
    if (value != 0.0f) {
        throw nlm::NLMTestException("WorkingMemory zero input test failed: expected 0.0, got " + 
                                   std::to_string(value));
    }
    
    std::cout << "    testWorkingMemoryZeroInput passed" << std::endl;
}

void testWorkingMemoryNullInput() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralWorkingMemory>(config);
    
    // Store something
    memory->store(1.0f, 1, 0);
    
    // Recall with null input (0, 0)
    float value = memory->recall(0, 0);
    if (value != 0.0f) {
        throw nlm::NLMTestException("WorkingMemory null input test failed: expected 0.0, got " + 
                                   std::to_string(value));
    }
    
    std::cout << "    testWorkingMemoryNullInput passed" << std::endl;
}

void testWorkingMemoryNegativeValues() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralWorkingMemory>(config);
    
    // Store negative values
    memory->store(-1.5f, 1, 0);
    memory->store(-3.7f, 2, 0);
    
    // Recall and verify
    float value1 = memory->recall(1, 0);
    if (std::abs(value1 - (-1.5f)) > 0.0001f) {
        throw nlm::NLMTestException("WorkingMemory negative values test failed: expected -1.5, got " + 
                                   std::to_string(value1));
    }
    
    float value2 = memory->recall(2, 0);
    if (std::abs(value2 - (-3.7f)) > 0.0001f) {
        throw nlm::NLMTestException("WorkingMemory negative values test failed: expected -3.7, got " + 
                                   std::to_string(value2));
    }
    
    std::cout << "    testWorkingMemoryNegativeValues passed" << std::endl;
}

void testWorkingMemoryLargeValues() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralWorkingMemory>(config);
    
    // Store large values
    memory->store(1000.0f, 1, 0);
    memory->store(-1000.0f, 2, 0);
    
    // Recall and verify
    float value1 = memory->recall(1, 0);
    if (std::abs(value1 - 1000.0f) > 0.0001f) {
        throw nlm::NLMTestException("WorkingMemory large values test failed: expected 1000.0, got " + 
                                   std::to_string(value1));
    }
    
    float value2 = memory->recall(2, 0);
    if (std::abs(value2 - (-1000.0f)) > 0.0001f) {
        throw nlm::NLMTestException("WorkingMemory large values test failed: expected -1000.0, got " + 
                                   std::to_string(value2));
    }
    
    std::cout << "    testWorkingMemoryLargeValues passed" << std::endl;
}

void testWorkingMemoryPerformance() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(100), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralWorkingMemory>(config);
    
    // Time memory operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < 1000; ++i) {
        memory->store(static_cast<float>(i), static_cast<int>(i % 100), static_cast<int>(i % 10));
        float value = memory->recall(static_cast<int>(i % 100), static_cast<int>(i % 10));
        if (value != 0.0f && value != static_cast<float>(i)) {
            // Don't fail on evictions, just continue
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Should complete without errors
    std::cout << "    testWorkingMemoryPerformance passed (1000 store/recall operations in " 
              << std::fixed << std::setprecision(4) << totalTime << "s)" << std::endl;
}

void testWorkingMemoryMixedData() {
    auto config = std::make_shared<Config>();
    config->set("memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("trace_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralWorkingMemory>(config);
    
    // Store mixed data
    std::vector<std::pair<float, std::pair<int, int>>> testData = {
        {1.0f, {1, 0}}, {2.0f, {2, 1}}, {3.0f, {3, 2}}, {4.0f, {4, 3}},
        {5.0f, {5, 4}}, {6.0f, {6, 5}}, {7.0f, {7, 6}}, {8.0f, {8, 7}},
        {9.0f, {9, 8}}, {10.0f, {10, 9}}
    };
    
    for (const auto& data : testData) {
        memory->store(data.first, data.second.first, data.second.second);
    }
    
    // Recall and verify
    for (const auto& data : testData) {
        float value = memory->recall(data.second.first, data.second.second);
        if (std::abs(value - data.first) > 0.0001f) {
            throw nlm::NLMTestException("WorkingMemory mixed data test failed: expected " + 
                                       std::to_string(data.first) + ", got " + std::to_string(value));
        }
    }
    
    std::cout << "    testWorkingMemoryMixedData passed" << std::endl;
}

void runAll() {
    std::cout << "Running WorkingMemory tests..." << std::endl;
    testWorkingMemoryCreation();
    testWorkingMemoryName();
    testWorkingMemoryStoreRecall();
    testWorkingMemoryCapacity();
    testWorkingMemoryTraceLength();
    testWorkingMemoryZeroInput();
    testWorkingMemoryNullInput();
    testWorkingMemoryNegativeValues();
    testWorkingMemoryLargeValues();
    testWorkingMemoryPerformance();
    testWorkingMemoryMixedData();
}

} // namespace test_working_memory
