// NLM Memory System Stress Tests
// Comprehensive stress testing for Memory system

#include "stress_tests/MemoryStressTests.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "memory/Memory.hpp"
#include "core/Config/Config.hpp"
#include "core/Profiler.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <set>
#include <string>

using namespace nlm;

namespace stress_tests {

// Working memory stress tests focusing on memory pressure scenarios
void MemoryStressTests::runWorkingMemoryStressTests() {
    std::cout << "=== Working Memory Stress Tests ===" << std::endl;
    
    const std::vector<size_t> memorySizes = {50, 100, 200, 500, 1000};
    const size_t steps = 200;
    const std::vector<float> pressureLevels = {0.5f, 1.0f, 2.0f, 5.0f, 10.0f};
    
    WorkingMemoryResults globalResults;
    globalResults.totalTests = memorySizes.size() * pressureLevels.size();
    
    for (size_t memorySize : memorySizes) {
        for (float pressureLevel : pressureLevels) {
            std::cout << "Working memory stress test: " << memorySize << " items, pressure " << pressureLevel << "..." << std::flush;
            
            // Create working memory instance
            auto workingMemory = std::make_shared<NeuralWorkingMemory>();
            workingMemory->initialize(nullptr); // Pass null brain for testing
            
            WorkingMemoryStressTestResults result;
            result.memorySize = memorySize;
            result.pressureLevel = pressureLevel;
            result.totalSteps = steps;
            
            // Start memory tracking
            auto& memProfiler = MemoryAllocationProfiler::getInstance();
            memProfiler.getStats(); // Clear
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                // Vary memory pressure based on pressure level
                float pressure = 1.0f + (pressureLevel - 1.0f) * (0.5f + 0.5f * std::sin(static_cast<double>(step) * 0.05));
                
                // Perform memory operations under pressure
                for (size_t i = 0; i < memorySize / 10; ++i) {
                    size_t index = (step + i) % memorySize;
                    
                    // Store item with pressure factor
                    std::string key = "item_" + std::to_string(index);
                    std::string value = "value_" + std::to_string(step) + "_" + std::to_string(index);
                    float priority = 0.5f + (0.5f * std::sin(static_cast<double>(index) * 0.1)) * pressure;
                    
                    workingMemory->store(key, value, priority);
                    
                    // Retrieve item
                    auto retrieved = workingMemory->retrieve(key);
                    if (retrieved) {
                        // Access memory to trigger consolidation under pressure
                        workingMemory->consolidate(key);
                    }
                    
                    // Update item with pressure
                    workingMemory->updatePriority(key, priority * pressure);
                }
                
                // Simulate memory decay under high pressure
                if (pressureLevel > 5.0f) {
                    workingMemory->decayMemory(0.1f * pressure);
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Get memory statistics
            MemoryAllocationProfiler::MemoryStats memStats = memProfiler.getStats();
            
            result.totalTime = totalTime;
            result.stepsPerSecond = steps / totalTime;
            result.peakMemoryKB = memStats.peakAllocated / 1024.0;
            result.totalMemoryKB = memStats.totalAllocated / 1024.0;
            result.allocationCount = memStats.allocationCount;
            result.memoryPressureScore = calculateWorkingMemoryPressureScore(workingMemory, memorySize, pressureLevel);
            
            globalResults.testResults.push_back(result);
            
            std::cout << " Done!" << std::endl;
            std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
            std::cout << "    Memory pressure score: " << std::fixed << std::setprecision(2) << result.memoryPressureScore << std::endl;
        }
    }
    
    printWorkingMemoryStressResults(globalResults);
}

// Episodic memory stress tests focusing on memory consolidation limits
void MemoryStressTests::runEpisodicMemoryStressTests() {
    std::cout << "=== Episodic Memory Stress Tests ===" << std::endl;
    
    const std::vector<size_t> episodeCounts = {50, 100, 200, 500, 1000};
    const size_t steps = 150;
    const std::vector<float> consolidationRates = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f};
    
    EpisodicMemoryResults globalResults;
    globalResults.totalTests = episodeCounts.size() * consolidationRates.size();
    
    for (size_t episodeCount : episodeCounts) {
        for (float consolidationRate : consolidationRates) {
            std::cout << "Episodic memory stress test: " << episodeCount << " episodes, consolidation rate " << consolidationRate << "..." << std::flush;
            
            auto episodicMemory = std::make_shared<NeuralEpisodicMemory>();
            episodicMemory->initialize(nullptr); // Pass null brain for testing
            
            EpisodicMemoryStressTestResults result;
            result.episodeCount = episodeCount;
            result.consolidationRate = consolidationRate;
            result.totalSteps = steps;
            
            // Start memory tracking
            auto& memProfiler = MemoryAllocationProfiler::getInstance();
            memProfiler.getStats(); // Clear
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                // Create episodic items with consolidation rate influence
                for (size_t i = 0; i < episodeCount / 20; ++i) {
                    size_t episodeId = (step + i) % episodeCount;
                    
                    // Create episode with metadata
                    EpisodicMemoryItem episode;
                    episode.id = episodeId;
                    episode.step = step;
                    episode.relevance = 0.5f + (0.5f * std::sin(static_cast<double>(episodeId) * 0.1));
                    episode.duration = 1.0f + consolidationRate * (0.5f + 0.5f * std::cos(static_cast<double>(step) * 0.05));
                    
                    // Create action sequence
                    for (size_t j = 0; j < 3 + static_cast<size_t>(consolidationRate); ++j) {
                        episode.actions.push_back("action_" + std::to_string((step + j) % 10));
                    }
                    
                    // Create context information
                    for (size_t j = 0; j < 2 + static_cast<size_t>(consolidationRate / 2); ++j) {
                        episode.context.push_back("context_" + std::to_string((step + j * 5) % 5));
                    }
                    
                    // Store episode
                    episodicMemory->storeEpisode(episode);
                    
                    // Attempt retrieval
                    auto similarEpisodes = episodicMemory->retrieveSimilar(&episode);
                    auto temporalEpisodes = episodicMemory->retrieveTemporal(episode.step, 10);
                    auto locationEpisodes = episodicMemory->retrieveByLocation("location_1");
                    auto actionEpisodes = episodicMemory->retrieveAfterAction("action_start", 5);
                    
                    // Update relevance under consolidation pressure
                    episodicMemory->updateRelevance(episodeId, consolidationRate * 0.1f);
                }
                
                // Force consolidation when under high rate
                if (consolidationRate > 2.0f) {
                    // Simulate consolidation burst
                    for (size_t i = 0; i < std::min<size_t>(episodeCount / 10, 50); ++i) {
                        episodicMemory->consolidateEpisode(static_cast<size_t>(i));
                    }
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Get memory statistics
            MemoryAllocationProfiler::MemoryStats memStats = memProfiler.getStats();
            
            result.totalTime = totalTime;
            result.stepsPerSecond = steps / totalTime;
            result.peakMemoryKB = memStats.peakAllocated / 1024.0;
            result.totalMemoryKB = memStats.totalAllocated / 1024.0;
            result.allocationCount = memStats.allocationCount;
            result.consolidationStressScore = calculateEpisodicMemoryConsolidationStress(episodicMemory, episodeCount, consolidationRate);
            
            globalResults.testResults.push_back(result);
            
            std::cout << " Done!" << std::endl;
            std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
            std::cout << "    Consolidation stress score: " << std::fixed << std::setprecision(2) << result.consolidationStressScore << std::endl;
            
            resetProfiling(); // Reset for next test
        }
    }
    
    printEpisodicMemoryStressResults(globalResults);
}

// Associative memory stress tests focusing on pattern association loads
void MemoryStressTests::runAssociativeMemoryStressTests() {
    std::cout << "=== Associative Memory Stress Tests ===" << std::endl;
    
    const std::vector<size_t> patternSets = {50, 100, 200, 500, 1000};
    const size_t steps = 100;
    const std::vector<float> learningRates = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f};
    
    AssociativeMemoryResults globalResults;
    globalResults.totalTests = patternSets.size() * learningRates.size();
    
    for (size_t patternSet : patternSets) {
        for (float learningRate : learningRates) {
            std::cout << "Associative memory stress test: " << patternSet << " patterns, LR " << learningRate << "..." << std::flush;
            
            auto associativeMemory = std::make_shared<NeuralAssociativeMemory>();
            associativeMemory->initialize(nullptr); // Pass null brain for testing
            
            AssociativeMemoryStressTestResults result;
            result.patternSet = patternSet;
            result.learningRate = learningRate;
            result.totalSteps = steps;
            
            // Start memory tracking
            auto& memProfiler = MemoryAllocationProfiler::getInstance();
            memProfiler.getStats(); // Clear
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                // Create pattern associations
                for (size_t i = 0; i < patternSet / 10; ++i) {
                    size_t patternId = (step + i) % patternSet;
                    
                    // Create cue pattern
                    std::vector<float> cue(patternSet / 10);
                    for (size_t j = 0; j < cue.size(); ++j) {
                        cue[j] = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f;
                    }
                    
                    // Create response pattern
                    std::vector<float> response(patternSet / 10);
                    for (size_t j = 0; j < response.size(); ++j) {
                        response[j] = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f;
                    }
                    
                    // Learn association with stress level
                    float associationStrength = learningRate * (0.5f + (static_cast<float>(std::rand()) / RAND_MAX) * 0.5f);
                    associativeMemory->learnAssociation(cue, response, associationStrength);
                    
                    // Retrieve association
                    auto retrievedResponse = associativeMemory->retrieveAssociation(cue);
                    
                    // Update association strength
                    associativeMemory->updateAssociationStrength(cue, learningRate * 0.1f);
                }
                
                // Force pattern completion under high learning rates
                if (learningRate > 2.0f) {
                    for (size_t i = 0; i < std::min<size_t>(patternSet / 20, 20); ++i) {
                        // Create incomplete cue for completion
                        std::vector<float> incompleteCue(patternSet / 20);
                        for (size_t j = 0; j < incompleteCue.size(); ++j) {
                            incompleteCue[j] = (j < incompleteCue.size() / 2) ? 
                                (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f : 0.0f;
                        }
                        
                        auto completed = associativeMemory->completePattern(incompleteCue);
                    }
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Get memory statistics
            MemoryAllocationProfiler::MemoryStats memStats = memProfiler.getStats();
            
            result.totalTime = totalTime;
            result.stepsPerSecond = steps / totalTime;
            result.peakMemoryKB = memStats.peakAllocated / 1024.0;
            result.totalMemoryKB = memStats.totalAllocated / 1024.0;
            result.allocationCount = memStats.allocationCount;
            result.associationStressScore = calculateAssociativeMemoryStress(associativeMemory, patternSet, learningRate);
            
            globalResults.testResults.push_back(result);
            
            std::cout << " Done!" << std::endl;
            std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
            std::cout << "    Association stress score: " << std::fixed << std::setprecision(2) << result.associationStressScore << std::endl;
            
            resetProfiling(); // Reset for next test
        }
    }
    
    printAssociativeMemoryStressResults(globalResults);
}

// Memory leak detection and tracking
void MemoryStressTests::runMemoryLeakTests() {
    std::cout << "=== Memory Leak Detection Tests ===" << std::endl;
    
    const std::vector<size_t> testSizes = {100, 500, 1000, 5000};
    const size_t allocations = 10000;
    const size_t iterations = 5;
    
    LeakDetectionResults globalResults;
    globalResults.totalTests = testSizes.size() * iterations;
    
    for (size_t testSize : testSizes) {
        for (size_t iter = 0; iter < iterations; ++iter) {
            std::cout << "Memory leak test: " << testSize << " allocations, iteration " << iter << "..." << std::flush;
            
            auto& memProfiler = MemoryAllocationProfiler::getInstance();
            memProfiler.getStats(); // Clear before test
            
            LeakTestResult result;
            result.testSize = testSize;
            result.iteration = iter;
            
            // Create objects with tracked allocations
            std::vector<std::unique_ptr<int>> allocations;
            allocations.reserve(testSize);
            
            // Phase 1: Allocation
            auto allocStartTime = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < testSize; ++i) {
                allocations.push_back(std::make_unique<int>(static_cast<int>(i)));
                memProfiler.recordAllocation(sizeof(int));
            }
            auto allocEndTime = std::chrono::high_resolution_clock::now();
            double allocTime = std::chrono::duration<double>(allocEndTime - allocStartTime).count();
            
            // Phase 2: Intensive access and modification
            auto accessStartTime = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < testSize; ++i) {
                if (allocations[i]) {
                    // Modify to trigger potential reallocation
                    *allocations[i] = static_cast<int>(i * 2);
                    memProfiler.recordAllocation(sizeof(int));
                }
                
                // Random access patterns to stress memory management
                if (i % 5 == 0) {
                    size_t randomIndex = (static_cast<size_t>(std::rand()) % testSize);
                    if (allocations[randomIndex]) {
                        *allocations[randomIndex] = static_cast<int>(randomIndex * 3);
                        memProfiler.recordAllocation(sizeof(int));
                    }
                }
            }
            auto accessEndTime = std::chrono::high_resolution_clock::now();
            double accessTime = std::chrono::duration<double>(accessEndTime - accessStartTime).count();
            
            // Phase 3: Deallocation with tracking
            auto deallocStartTime = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < testSize; ++i) {
                if (allocations[i]) {
                    memProfiler.recordDeallocation(sizeof(int));
                    allocations[i].reset();
                }
            }
            auto deallocEndTime = std::chrono::high_resolution_clock::now();
            double deallocTime = std::chrono::duration<double>(deallocEndTime - deallocStartTime).count();
            
            // Force garbage collection by clearing vector
            allocations.clear();
            
            // Wait for potential delayed deallocation
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Get final memory statistics
            MemoryAllocationProfiler::MemoryStats memStats = memProfiler.getStats();
            
            result.allocTime = allocTime;
            result.accessTime = accessTime;
            result.deallocTime = deallocTime;
            result.peakMemoryKB = memStats.peakAllocated / 1024.0;
            result.totalMemoryKB = memStats.totalAllocated / 1024.0;
            result.allocationCount = memStats.allocationCount;
            result.deallocationCount = memStats.deallocationCount;
            result.leakRate = calculateLeakRate(memStats, testSize, iter);
            
            globalResults.testResults.push_back(result);
            
            std::cout << " Done!" << std::endl;
            std::cout << "    Alloc time: " << std::fixed << std::setprecision(3) << allocTime << "s" << std::endl;
            std::cout <<    "    Access time: " << std::fixed << std::setprecision(3) << accessTime << "s" << std::endl;
            std::cout <<    "    Dealloc time: " << std::fixed << std::setprecision(3) << deallocTime << "s" << std::endl;
            std::cout << "    Leak rate: " << std::fixed << std::setprecision(4) << result.leakRate << std::endl;
        }
    }
    
    printLeakDetectionResults(globalResults);
}

// Helper functions for memory stress test analysis

double MemoryStressTests::calculateWorkingMemoryPressureScore(
    const std::shared_ptr<NeuralWorkingMemory>& workingMemory,
    size_t memorySize, float pressureLevel) {
    
    // Get current working memory statistics
    auto stats = workingMemory->getMemoryStats();
    
    // Calculate pressure score based on memory utilization and pressure level
    double utilizationScore = static_cast<double>(stats.currentSize) / memorySize;
    double pressureImpact = std::min(1.0, pressureLevel / 10.0f); // Normalize pressure
    
    return (utilizationScore * 0.7 + pressureImpact * 0.3);
}

double MemoryStressTests::calculateEpisodicMemoryConsolidationStress(
    const std::shared_ptr<NeuralEpisodicMemory>& episodicMemory,
    size_t episodeCount, float consolidationRate) {
    
    // Simulate consolidation stress calculation
    double episodeUtilization = static_cast<double>(episodeCount) / 1000.0; // Normalize to max 1000
    double consolidationPressure = std::min(1.0, consolidationRate / 5.0f); // Normalize to max 5.0
    
    // Higher score indicates more stress
    return (episodeUtilization * 0.6 + consolidationPressure * 0.4) * 100.0;
}

double MemoryStressTests::calculateAssociativeMemoryStress(
    const std::shared_ptr<NeuralAssociativeMemory>& associativeMemory,
    size_t patternSet, float learningRate) {
    
    // Simulate associative memory stress
    double patternDensity = static_cast<double>(patternSet) / 2000.0; // Normalize to max 2000
    double learningPressure = std::min(1.0, learningRate / 5.0f); // Normalize to max 5.0
    
    return (patternDensity * 0.5 + learningPressure * 0.5) * 100.0;
}

double MemoryStressTests::calculateLeakRate(const MemoryAllocationProfiler::MemoryStats& memStats,
                                           size_t expectedSize, size_t iteration) {
    
    // Calculate leak rate based on allocation/deallocation imbalance
    double allocationRate = static_cast<double>(memStats.allocationCount) / expectedSize;
    double deallocationRate = static_cast<double>(memStats.deallocationCount) / expectedSize;
    
    // Leak is allocation without corresponding deallocation
    double leakRatio = allocationRate - deallocationRate;
    
    // Normalize to percentage
    return std::max(0.0, leakRatio) * 100.0;
}

// Result printing functions

void MemoryStressTests::printWorkingMemoryStressResults(const WorkingMemoryResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Working Memory Stress Test Results                        ║" << std::endl;
    std::cout << "╠════════════╦════════╦════════╦══════════╦══════════╦════════════╗" << std::endl;
    std::cout << "║  Items    ║ Stress ║ Steps/s ║ Peak(KB) ║ Allocs   ║ Pressure  ║" << std::endl;
    std::cout << "╠════════════╬════════╬════════╬══════════╬══════════╬════════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.memorySize
                  << " ║ " << std::setw(6) << std::fixed << std::setprecision(1) << r.pressureLevel
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(1) << r.peakMemoryKB
                  << " ║ " << std::setw(8) << r.allocationCount
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(2) << r.memoryPressureScore
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩════════╩══════════╩══════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

void MemoryStressTests::printEpisodicMemoryStressResults(const EpisodicMemoryResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Episodic Memory Stress Test Results                       ║" << std::endl;
    std::cout << "╠════════════╦════════════╦════════╦══════════╦══════════╦════════════╗" << std::endl;
    std::cout << "║  Episodes  ║ Consolidation ║ Steps/s ║ Peak(KB) ║ Allocs   ║ Stress  ║" << std::endl;
    std::cout << "╠════════════╬════════════╬════════╬══════════╬══════════╬════════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.episodeCount
                  << " ║ " << std::setw(13) << std::fixed << std::setprecision(1) << r.consolidationRate
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(1) << r.peakMemoryKB
                  << " ║ " << std::setw(8) << r.allocationCount
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(2) << r.consolidationStressScore
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════════╩════════╩══════════╩══════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

void MemoryStressTests::printAssociativeMemoryStressResults(const AssociativeMemoryResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Associative Memory Stress Test Results                   ║" << std::endl;
    std::cout << "╠════════════╦════════╦════════╦══════════╦══════════╦════════════╗" << std::endl;
    std::cout << "║  Patterns  ║   LR   ║ Steps/s ║ Peak(KB) ║ Allocs   ║ Stress  ║" << std::endl;
    std::cout << "╠════════════╬════════╬════════╬══════════╬══════════╬════════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.patternSet
                  << " ║ " << std::setw(6) << std::fixed << std::setprecision(1) << r.learningRate
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(1) << r.peakMemoryKB
                  << " ║ " << std::setw(8) << r.allocationCount
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(2) << r.associationStressScore
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩════════╩══════════╩══════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

void MemoryStressTests::printLeakDetectionResults(const LeakDetectionResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Memory Leak Detection Test Results                       ║" << std::endl;
    std::cout << "╠════════════╦════════╦════════╦═════════╦═════════╦═════════╦════════╗" << std::endl;
    std::cout << "║  Allocs    ║  Iter  ║   Leak  ║ Peak(KB) ║ Alloc/s ║ Dealloc/s ║ Ratio  ║" << std::endl;
    std::cout << "╠════════════╬════════╬════════╬═════════╬═════════╬═════════╬════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        double allocsPerSecond = static_cast<double>(r.allocationCount) / r.allocTime;
        double deallocsPerSecond = static_cast<double>(r.deallocationCount) / r.deallocTime;
        double leakRatio = r.leakRate / 100.0;
        
        std::cout << "║ " << std::setw(10) << r.testSize
                  << " ║ " << std::setw(6) << r.iteration
                  << " ║ " << std::setw(6) << std::fixed << std::setprecision(2) << r.leakRate << "%"
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(1) << r.peakMemoryKB
                  << " ║ " << std::setw(8) << std::fixed << std::setprecision(1) << allocsPerSecond
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(1) << deallocsPerSecond
                  << " ║ " << std::setw(6) << std::fixed << std::setprecision(2) << leakRatio
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩════════╩═════════╩═════════╩═════════╩════════╝" << std::endl;
    std::cout << std::endl;
}

} // namespace stress_tests

// Main memory stress test runner
int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Memory System Stress Tests                          ║
    ║                                                               ║
    ║     Comprehensive Memory System Stress Testing                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize random number generator
    auto rng = std::make_shared<RandomGenerator>();
    RandomGenerator::setGlobal(rng);
    
    std::cout << "Running Memory System Stress Tests..." << std::endl;
    std::cout << std::endl;
    
    // Run all memory stress test suites
    stress_tests::MemoryStressTests::runWorkingMemoryStressTests();
    std::cout << std::endl;
    
    stress_tests::MemoryStressTests::runEpisodicMemoryStressTests();
    std::cout << std::endl;
    
    stress_tests::MemoryStressTests::runAssociativeMemoryStressTests();
    std::cout << std::endl;
    
    stress_tests::MemoryStressTests::runMemoryLeakTests();
    std::cout << std::endl;
    
    // Generate final profiling report
    std::cout << "=== FINAL MEMORY STRESS TEST PROFILING REPORT ===" << std::endl;
    std::cout << generateProfilingReport() << std::endl;
    
    std::cout << "Memory System Stress Tests complete." << std::endl;
    
    return 0;
}
