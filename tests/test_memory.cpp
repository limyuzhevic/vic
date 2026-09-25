// Memory Tests
#include "memory/Memory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_memory {

TEST_CASE("Memory base class functionality", "[memory][base][memory]") {
    nlm::Memory memory;
    
    REQUIRE(memory.getTotalCapacity() > 0);
    REQUIRE(memory.getCurrentUsage() >= 0);
}

TEST_CASE("NeuralWorkingMemory creation and operations", "[memory][working_memory][creation]") {
    nlm::NeuralWorkingMemory workingMemory;
    
    REQUIRE(workingMemory.getCapacity() > 0);
    REQUIRE(workingMemory.getActiveItems() == 0);
}

TEST_CASE("NeuralWorkingMemory item management", "[memory][working_memory][items]") {
    nlm::NeuralWorkingMemory workingMemory;
    
    workingMemory.addItem(nlm::MemoryItem(nlm::NeuronId(1), 0.5f));
    REQUIRE(workingMemory.getActiveItems() == 1);
    
    workingMemory.updateItem(nlm::NeuronId(1), 0.7f);
    
    REQUIRE(workingMemory.getItemStrength(nlm::NeuronId(1)) == 0.7f);
}

TEST_CASE("NeuralEpisodicMemory creation and storage", "[memory][episodic][creation]") {
    nlm::NeuralEpisodicMemory episodicMemory;
    
    REQUIRE(episodicMemory.getMemoryTraceCount() == 0);
}

TEST_CASE("NeuralEpisodicMemory episode recording", "[memory][episodic][episodes]") {
    nlm::NeuralEpisodicMemory episodicMemory;
    
    nlm::Episode episode;
    episode.id = nlm::EpisodeId(1);
    episode.content = "Test episode content";
    episode.timestamp = 1.0;
    
    episodicMemory.recordEpisode(episode);
    REQUIRE(episodicMemory.getMemoryTraceCount() == 1);
}

TEST_CASE("Memory consolidation process", "[memory][consolidation][process]") {
    nlm::NeuralEpisodicMemory episodicMemory;
    
    nlm::Episode episode;
    episode.id = nlm::EpisodeId(2);
    episode.content = "Test episode for consolidation";
    episode.timestamp = 2.0;
    
    episodicMemory.recordEpisode(episode);
    REQUIRE(episodicMemory.getMemoryTraceCount() == 1);
    
    // Consolidation should strengthen memory traces
    episodicMemory.consolidateTraces();
    
    // Memory should still be present
    REQUIRE(episodicMemory.getMemoryTraceCount() == 1);
}

TEST_CASE("Memory system capacity management", "[memory][system][capacity]") {
    nlm::NeuralWorkingMemory workingMemory;
    
    // Add items until near capacity
    for (int i = 0; i < 10; ++i) {
        workingMemory.addItem(nlm::MemoryItem(nlm::NeuronId(i), 0.1f * (i + 1)));
    }
    
    REQUIRE(workingMemory.getActiveItems() <= workingMemory.getCapacity());
}

TEST_CASE("Memory item retrieval and cleanup", "[memory][items][retrieval]") {
    nlm::NeuralWorkingMemory workingMemory;
    
    workingMemory.addItem(nlm::MemoryItem(nlm::NeuronId(3), 0.9f));
    
    auto item = workingMemory.getItem(nlm::NeuronId(3));
    REQUIRE(item.has_value());
    REQUIRE(item->strength == 0.9f);
    
    // Remove item
    workingMemory.removeItem(nlm::NeuronId(3));
    REQUIRE(workingMemory.getItem(nlm::NeuronId(3)) == std::nullopt);
}

TEST_CASE("Memory system integration", "[memory][integration][system]") {
    nlm::NeuralWorkingMemory workingMemory;
    nlm::NeuralEpisodicMemory episodicMemory;
    
    // Test memory system coordination
    workingMemory.addItem(nlm::MemoryItem(nlm::NeuronId(4), 0.6f));
    episodicMemory.recordEpisode(
        nlm::Episode{nlm::EpisodeId(3), "Test episode", 3.0}
    );
    
    REQUIRE(workingMemory.getActiveItems() == 1);
    REQUIRE(episodicMemory.getMemoryTraceCount() == 1);
}

TEST_CASE("Memory performance under load", "[memory][performance][load]") {
    nlm::NeuralWorkingMemory workingMemory;
    
    // Measure performance with increasing memory load
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        workingMemory.addItem(nlm::MemoryItem(nlm::NeuronId(i), 0.01f));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Should complete within reasonable time
    REQUIRE(duration.count() < 1000000);  // Less than 1 second
}

} // namespace test_memory
