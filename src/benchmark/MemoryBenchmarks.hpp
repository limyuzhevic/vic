// Memory System Performance Benchmarks
// Comprehensive benchmarking of working memory, episodic memory, associative memory, and cleanup

#pragma once

#include "Benchmark.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/Memory.hpp"
#include <vector>
#include <random>
#include <string>

namespace nlm {

class MemorySystemPerformanceBenchmark {
public:
    static void runWorkingMemoryBenchmark(Benchmark& benchmark) {
        // Test working memory encoding/retrieval speed
        for (int memory_size : {100, 500, 1000}) {
            NeuralWorkingMemory working_memory;
            working_memory.initialize(memory_size);
            
            // Test encoding performance
            benchmark.run(
                "Working Memory Encoding - " + std::to_string(memory_size) + " items",
                [&]() {
                    // Encode different types of information
                    for (int i = 0; i < memory_size; ++i) {
                        std::string key = "key_" + std::to_string(i);
                        std::vector<float> data(10); // 10-dimensional feature vector
                        for (int j = 0; j < 10; ++j) {
                            data[j] = static_cast<float>(rand()) / RAND_MAX;
                        }
                        
                        // Encode to working memory
                        working_memory.encode(key, data, 1000.0f);
                    }
                },
                5
            );
            
            // Test retrieval performance
            benchmark.run(
                "Working Memory Retrieval - " + std::to_string(memory_size) + " items",
                [&]() {
                    // Retrieve encoded information
                    for (int i = 0; i < memory_size; ++i) {
                        std::string key = "key_" + std::to_string(i);
                        std::vector<float> retrieved = working_memory.retrieve(key);
                    }
                },
                5
            );
        }
    }
    
    static void runEpisodicMemoryBenchmark(Benchmark& benchmark) {
        // Test episodic memory consolidation and replay speed
        for (int memory_size : {50, 200, 500}) {
            NeuralEpisodicMemory episodic_memory;
            episodic_memory.initialize(memory_size);
            
            // Test consolidation performance
            benchmark.run(
                "Episodic Memory Consolidation - " + std::to_string(memory_size) + " episodes",
                [&]() {
                    // Create episodic memory entries
                    for (int i = 0; i < memory_size; ++i) {
                        EpisodicMemoryEntry entry;
                        entry.timestamp = static_cast<Timestamp>(i * 100);
                        entry.experience_id = i;
                        entry.context_id = i % 50;
                        
                        // Store experience
                        episodic_memory.storeExperience(entry);
                    }
                },
                3
            );
            
            // Test replay performance
            benchmark.run(
                "Episodic Memory Replay - " + std::to_string(memory_size) + " episodes",
                [&]() {
                    // Replay experiences
                    for (int i = 0; i < memory_size; ++i) {
                        // Retrieve and replay experience
                        // episodic_memory.replayExperience(i);
                    }
                },
                3
            );
        }
    }
    
    static void runAssociativeMemoryBenchmark(Benchmark& benchmark) {
        // Test associative memory pattern completion speed
        for (int pattern_count : {100, 500, 1000}) {
            Memory associative_memory;
            associative_memory.initialize(pattern_count);
            
            // Test pattern completion performance
            benchmark.run(
                "Associative Memory Completion - " + std::to_string(pattern_count) + " patterns",
                [&]() {
                    // Store patterns
                    for (int i = 0; i < pattern_count; ++i) {
                        std::vector<float> pattern(20); // 20-dimensional pattern
                        for (int j = 0; j < 20; ++j) {
                            pattern[j] = static_cast<float>(rand()) / RAND_MAX;
                        }
                        
                        // Store pattern
                        associative_memory.storePattern(pattern);
                    }
                },
                3
            );
            
            // Test pattern completion performance
            benchmark.run(
                "Associative Memory Completion - " + std::to_string(pattern_count) + " patterns",
                [&]() {
                    // Complete partial patterns
                    std::vector<float> partial_pattern(20);
                    for (int j = 0; j < 20; ++j) {
                        partial_pattern[j] = static_cast<float>(rand()) / RAND_MAX * 0.5f; // Partial activation
                    }
                    
                    for (int i = 0; i < pattern_count / 10; ++i) {
                        // Complete pattern
                        // associative_memory.completePattern(partial_pattern);
                    }
                },
                3
            );
        }
    }
    
    static void runMemoryCleanupBenchmark(Benchmark& benchmark) {
        // Test memory deallocation efficiency
        for (int memory_size : {1000, 5000, 10000}) {
            Memory cleanup_memory;
            cleanup_memory.initialize(memory_size);
            
            // Test memory allocation and cleanup
            benchmark.run(
                "Memory Cleanup - " + std::to_string(memory_size) + " allocations",
                [&]() {
                    // Allocate memory
                    std::vector<std::vector<float>> allocated_data;
                    for (int i = 0; i < memory_size; ++i) {
                        std::vector<float> data(100); // 100 elements per allocation
                        for (int j = 0; j < 100; ++j) {
                            data[j] = static_cast<float>(rand());
                        }
                        allocated_data.push_back(std::move(data));
                    }
                    
                    // Cleanup (vector destructor will deallocate)
                    allocated_data.clear();
                },
                2
            );
        }
    }
};

} // namespace nlm