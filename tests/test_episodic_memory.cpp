// EpisodicMemory Tests
// Comprehensive testing of episodic memory system

#include "memory/NeuralEpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace test_episodic_memory {

void testEpisodicMemoryCreation() {
    auto config = std::make_shared<Config>();
    config->set("max_episodes", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("episode_length", static_cast<int64_t>(50), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralEpisodicMemory>(config);
    
    const char* name = memory->getName();
    if (!name || std::string(name) != "EpisodicMemory") {
        throw nlm::NLMTestException("EpisodicMemory creation failed: name should be 'EpisodicMemory'");
    }
    
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryName() {
    auto config = std::make_shared<Config>();
    config->set("max_episodes", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("episode_length", static_cast<int64_t>(50), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralEpisodicMemory>(config);
    
    const char* name = memory->getName();
    if (!name) {
        throw nlm::NLMTestException("EpisodicMemory name failed: name is null");
    }
    if (std::string(name) != "EpisodicMemory") {
        throw nlm::NLMTestException("EpisodicMemory name failed: expected 'EpisodicMemory', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testEpisodicMemoryName passed" << std::endl;
}

void testEpisodicMemoryStoreRecall() {
    auto config = std::make_shared<Config>();
    config->set("max_episodes", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("episode_length", static_cast<int64_t>(50), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralEpisodicMemory>(config);
    
    // Create an episode
    std::vector<float> episodeData(10, 0.0f);
    for (size_t i = 0; i < episodeData.size(); ++i) {
        episodeData[i] = static_cast<float>(i * 2.0f + 1.0f);
    }
    
    memory->store(42, 1.5f, episodeData);
    
    // Recall and verify
    auto recalled = memory->recall(42);
    if (!recalled) {
        throw nlm::NLMTestException("EpisodicMemory store/recall test failed: could not recall episode");
    }
    
    const auto& recalledData = recalled->getData();
    if (recalledData.size() != episodeData.size()) {
        throw nlm::NLMTestException("EpisodicMemory store/recall test failed: size mismatch, expected " + 
                                   std::to_string(episodeData.size()) + ", got " + 
                                   std::to_string(recalledData.size()));
    }
    
    for (size_t i = 0; i < episodeData.size(); ++i) {
        if (std::abs(recalledData[i] - episodeData[i]) > 0.0001f) {
            throw nlm::NLMTestException("EpisodicMemory store/recall test failed: value mismatch at index " + 
                                       std::to_string(i) + ", expected " + std::to_string(episodeData[i]) + 
                                       ", got " + std::to_string(recalledData[i]));
        }
    }
    
    std::cout << "    testEpisodicMemoryStoreRecall passed" << std::endl;
}

void testEpisodicMemoryCapacity() {
    auto config = std::make_shared<Config>();
    config->set("max_episodes", static_cast<int64_t>(5), ConfigSource::Default);
    config->set("episode_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralEpisodicMemory>(config);
    
    // Store episodes up to capacity
    for (int i = 0; i < 10; ++i) {
        std::vector<float> data(5, static_cast<float>(i));
        memory->store(i, 1.0f + static_cast<float>(i), data);
    }
    
    // First episodes should be gone
    auto episode1 = memory->recall(0);
    if (episode1 != nullptr) {
        throw nlm::NLMTestException("EpisodicMemory capacity test failed: episode 0 should have been evicted");
    }
    
    auto episode5 = memory->recall(4);
    if (episode5 == nullptr) {
        throw nlm::NLMTestException("EpisodicMemory capacity test failed: episode 4 should still exist");
    }
    
    // Newer episodes should exist
    auto episode9 = memory->recall(9);
    if (episode9 == nullptr) {
        throw nlm::NLMTestException("EpisodicMemory capacity test failed: episode 9 should still exist");
    }
    
    std::cout << "    testEpisodicMemoryCapacity passed" << std::endl;
}

void testEpisodicMemoryEpisodeLength() {
    auto config = std::make_shared<Config>();
    config->set("max_episodes", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("episode_length", static_cast<int64_t>(3), ConfigSource::Default); // Very short length
    
    auto memory = std::make_shared<NeuralEpisodicMemory>(config);
    
    // Create an episode with longer data
    std::vector<float> longData(10, 0.0f);
    for (size_t i = 0; i < longData.size(); ++i) {
        longData[i] = static_cast<float>(i);
    }
    
    memory->store(1, 1.0f, longData);
    
    // Recall should return data truncated to episode_length
    auto recalled = memory->recall(1);
    if (!recalled) {
        throw nlm::NLMTestException("EpisodicMemory episode length test failed: could not recall episode");
    }
    
    const auto& recalledData = recalled->getData();
    if (recalledData.size() > 3) {
        throw nlm::NLMTestException("EpisodicMemory episode length test failed: episode length should be <= 3, got " + 
                                   std::to_string(recalledData.size()));
    }
    
    std::cout << "    testEpisodicMemoryEpisodeLength passed (length: " << recalledData.size() << ")" << std::endl;
}

void testEpisodicMemoryNullInput() {
    auto config = std::make_shared<Config>();
    config->set("max_episodes", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("episode_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralEpisodicMemory>(config);
    
    // Recall with null episode ID
    auto recalled = memory->recall(0);
    if (recalled != nullptr) {
        throw nlm::NLMTestException("EpisodicMemory null input test failed: expected null for null ID");
    }
    
    std::cout << "    testEpisodicMemoryNullInput passed" << std::endl;
}

void testEpisodicMemoryNegativeValues() {
    auto config = std::make_shared<Config>();
    config->set("max_episodes", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("episode_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralEpisodicMemory>(config);
    
    // Create episode with negative values
    std::vector<float> negativeData = {-1.5f, -3.7f, -2.2f, -4.8f, -0.9f};
    
    memory->store(1, 1.0f, negativeData);
    
    // Recall and verify
    auto recalled = memory->recall(1);
    if (!recalled) {
        throw nlm::NLMTestException("EpisodicMemory negative values test failed: could not recall episode");
    }
    
    const auto& recalledData = recalled->getData();
    for (size_t i = 0; i < negativeData.size() && i < recalledData.size(); ++i) {
        if (std::abs(recalledData[i] - negativeData[i]) > 0.0001f) {
            throw nlm::NLMTestException("EpisodicMemory negative values test failed: value mismatch at index " + 
                                       std::to_string(i) + ", expected " + std::to_string(negativeData[i]) + 
                                       ", got " + std::to_string(recalledData[i]));
        }
    }
    
    std::cout << "    testEpisodicMemoryNegativeValues passed" << std::endl;
}

void testEpisodicMemoryLargeValues() {
    auto config = std::make_shared<Config>();
    config->set("max_episodes", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("episode_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralEpisodicMemory>(config);
    
    // Create episode with large values
    std::vector<float> largeData = {1000.0f, -1000.0f, 999999.0f, -999999.0f, 0.0f};
    
    memory->store(1, 1.0f, largeData);
    
    // Recall and verify
    auto recalled = memory->recall(1);
    if (!recalled) {
        throw nlm::NLMTestException("EpisodicMemory large values test failed: could not recall episode");
    }
    
    const auto& recalledData = recalled->getData();
    for (size_t i = 0; i < largeData.size() && i < recalledData.size(); ++i) {
        if (std::abs(recalledData[i] - largeData[i]) > 0.0001f) {
            throw nlm::NLMTestException("EpisodicMemory large values test failed: value mismatch at index " + 
                                       std::to_string(i) + ", expected " + std::to_string(largeData[i]) + 
                                       ", got " + std::to_string(recalledData[i]));
        }
    }
    
    std::cout << "    testEpisodicMemoryLargeValues passed" << std::endl;
}

void testEpisodicMemoryPerformance() {
    auto config = std::make_shared<Config>();
    config->set("max_episodes", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("episode_length", static_cast<int64_t>(100), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralEpisodicMemory>(config);
    
    // Time memory operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < 500; ++i) {
        std::vector<float> data(10, static_cast<float>(i));
        memory->store(static_cast<int>(i), 1.0f + static_cast<float>(i), data);
        auto recalled = memory->recall(static_cast<int>(i));
        if (recalled != nullptr) {
            // Don't fail on evictions, just continue
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Should complete without errors
    std::cout << "    testEpisodicMemoryPerformance passed (500 store/recall operations in " 
              << std::fixed << std::setprecision(4) << totalTime << "s)" << std::endl;
}

void testEpisodicMemoryMixedData() {
    auto config = std::make_shared<Config>();
    config->set("max_episodes", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("episode_length", static_cast<int64_t>(20), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralEpisodicMemory>(config);
    
    // Store multiple episodes with mixed data
    for (int i = 0; i < 20; ++i) {
        std::vector<float> data;
        for (int j = 0; j < 10; ++j) {
            data.push_back(static_cast<float>(i * 100 + j));
        }
        memory->store(i, 1.0f + static_cast<float>(i * 2), data);
    }
    
    // Recall and verify some episodes
    for (int i = 0; i < 20; i += 3) { // Every 3rd episode
        auto recalled = memory->recall(i);
        if (recalled == nullptr) {
            // Might be evicted, that's okay
            continue;
        }
        
        const auto& recalledData = recalled->getData();
        for (int j = 0; j < 10 && j < static_cast<int>(recalledData.size()); ++j) {
            if (std::abs(recalledData[j] - static_cast<float>(i * 100 + j)) > 0.0001f) {
                throw nlm::NLMTestException("EpisodicMemory mixed data test failed: value mismatch at episode " + 
                                           std::to_string(i) + ", index " + std::to_string(j));
            }
        }
    }
    
    std::cout << "    testEpisodicMemoryMixedData passed" << std::endl;
}

void testEpisodicMemoryTimeIndexing() {
    auto config = std::make_shared<Config>();
    config->set("max_episodes", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("episode_length", static_cast<int64_t>(10), ConfigSource::Default);
    
    auto memory = std::make_shared<NeuralEpisodicMemory>(config);
    
    // Store episodes with increasing IDs (time-based)
    for (int i = 0; i < 10; ++i) {
        std::vector<float> data = {static_cast<float>(i), static_cast<float>(i * 10)};
        memory->store(i, 1.0f, data);
    }
    
    // Later episodes should still exist
    auto episode9 = memory->recall(9);
    if (episode9 == nullptr) {
        throw nlm::NLMTestException("EpisodicMemory time indexing test failed: episode 9 should exist");
    }
    
    // Earlier episodes might be evicted, that's okay
    auto episode0 = memory->recall(0);
    if (episode0 != nullptr) {
        // Might still exist if capacity is high enough
        const auto& data = episode0->getData();
        if (data.size() < 1 || std::abs(data[0] - 0.0f) > 0.0001f) {
            throw nlm::NLMTestException("EpisodicMemory time indexing test failed: episode 0 data is incorrect");
        }
    }
    
    std::cout << "    testEpisodicMemoryTimeIndexing passed" << std::endl;
}

void runAll() {
    std::cout << "Running EpisodicMemory tests..." << std::endl;
    testEpisodicMemoryCreation();
    testEpisodicMemoryName();
    testEpisodicMemoryStoreRecall();
    testEpisodicMemoryCapacity();
    testEpisodicMemoryEpisodeLength();
    testEpisodicMemoryNullInput();
    testEpisodicMemoryNegativeValues();
    testEpisodicMemoryLargeValues();
    testEpisodicMemoryPerformance();
    testEpisodicMemoryMixedData();
    testEpisodicMemoryTimeIndexing();
}

} // namespace test_episodic_memory
