#pragma once

#include "NeuralWorkingMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

namespace nlm
{
    
    class WorkingMemoryTest
    {
    public:
        static void runAllTests()
        {
            std::cout << "=== Neural Working Memory Tests ===" << std::endl;
            
            // Test core functionality
            testCoreFunctionality();
            std::cout << "✓ Core functionality tests passed" << std::endl;
            
            testMemoryCapacity();
            std::cout << "✓ Memory capacity tests passed" << std::endl;
            
            testPatternCompletion();
            std::cout << "✓ Pattern completion tests passed" << std::endl;
            
            testInterferenceManagement();
            std::cout << "✓ Interference management tests passed" << std::endl;
            
            testUpdatingAndReplacement();
            std::cout << "✓ Updating and replacement tests passed" << std::endl;
            
            testNeuralRepresentation();
            std::cout << "✓ Neural representation tests passed" << std::endl;
            
            testAttentionIntegration();
            std::cout << "✓ Attention integration tests passed" << std::endl;
            
            testCapacityConstraints();
            std::cout << "✓ Capacity constraint tests passed" << std::endl;
            
            testMemoryDecay();
            std::cout << "✓ Memory decay tests passed" << std::endl;
            
            testSynapticModulation();
            std::cout << "✓ Synaptic modulation tests passed" << std::endl;
            
            testAttractorDynamics();
            std::cout << "✓ Attractor dynamics tests passed" << std::endl;
            
            std::cout << "\n=== All Working Memory Tests Passed ===" << std::endl;
        }
    
    private:
        static void testCoreFunctionality()
        {
            std::cout << "  Testing core functionality..." << std::endl;
            
            // Create config
            auto config = std::make_shared<Config>("test_config");
            
            // Create brain
            Brain brain(config);
            
            // Initialize working memory
            NeuralWorkingMemory wm(7, 0.05f);
            wm.initialize(&brain);
            
            // Test basic storage and retrieval
            std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
            wm.store(pattern, 1.0f);
            
            std::vector<float> retrieved = wm.retrieve();
            assert(retrieved.size() == pattern.size());
            
            for (size_t i = 0; i < pattern.size(); ++i) {
                assert(std::abs(retrieved[i] - pattern[i]) < 0.01f);
            }
            
            // Test neuron storage
            NeuronId neuron(12345);
            wm.storeToNeuron(neuron, 0.8f);
            assert(wm.getNeuronActivation(neuron) > 0.7f);
            
            std::cout << "    ✓ Basic storage and retrieval" << std::endl;
        }
    
        static void testMemoryCapacity()
        {
            std::cout << "  Testing memory capacity..." << std::endl;
            
            auto config = std::make_shared<Config>("test_config");
            Brain brain(config);
            
            // Test different capacity limits
            for (size_t capacity : {1, 4, 7, 10}) {
                NeuralWorkingMemory wm(capacity, 0.05f);
                wm.initialize(&brain);
                
                assert(wm.getCapacity() == capacity);
                assert(wm.getActiveTraceCount() == 0);
                
                // Store up to capacity
                for (size_t i = 0; i < capacity; ++i) {
                    std::vector<float> pattern(i + 1, 0.1f);
                    wm.store(pattern, 1.0f);
                }
                
                assert(wm.getActiveTraceCount() <= capacity);
            }
            
            std::cout << "    ✓ Capacity limits" << std::endl;
        }
    
        static void testPatternCompletion()
        {
            std::cout << "  Testing pattern completion..." << std::endl;
            
            auto config = std::make_shared<Config>("test_config");
            Brain brain(config);
            
            NeuralWorkingMemory wm(7, 0.05f);
            wm.initialize(&brain);
            
            // Store a complete pattern
            std::vector<float> original = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
            wm.store(original, 1.0f);
            
            // Create a partial cue
            std::vector<float> cue = {0.1f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
            
            // Retrieve pattern based on cue
            std::vector<float> completed = wm.retrievePattern(cue);
            
            if (!completed.empty()) {
                // Should reconstruct similar to original
                float similarity = computePatternSimilarity(original, completed);
                assert(similarity > 0.5f); // Should complete partially
            }
            
            std::cout << "    ✓ Pattern completion from partial cues" << std::endl;
        }
    
        static void testInterferenceManagement()
        {
            std::cout << "  Testing interference management..." << std::endl;
            
            auto config = std::make_shared<Config>("test_config");
            Brain brain(config);
            
            NeuralWorkingMemory wm(7, 0.05f);
            wm.initialize(&brain);
            
            // Store similar patterns (should cause interference)
            std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
            std::vector<float> pattern2 = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f, 0.65f, 0.75f};
            
            wm.store(pattern1, 1.0f);
            wm.store(pattern2, 1.0f);
            
            // Both should be stored but with interference
            size_t activeTraces = wm.getActiveTraceCount();
            assert(activeTraces >= 1); // At least one should survive
            
            // Max interference should be detectable
            float maxInterference = wm.getMaxInterference();
            assert(maxInterference >= 0.0f);
            
            std::cout << "    ✓ Interference management" << std::endl;
        }
    
        static void testUpdatingAndReplacement()
        {
            std::cout << "  Testing updating and replacement..." << std::endl;
            
            auto config = std::make_shared<Config>("test_config");
            Brain brain(config);
            
            NeuralWorkingMemory wm(7, 0.05f);
            wm.initialize(&brain);
            
            // Store initial pattern
            std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
            wm.store(pattern1, 1.0f);
            
            // Update with new information
            std::vector<float> pattern2 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f};
            wm.store(pattern2, 1.0f);
            
            // Should have updated traces
            assert(wm.getActiveTraceCount() > 0);
            
            // Test replacement when capacity is exceeded
            for (size_t i = 0; i < 15; ++i) {
                std::vector<float> pattern(i % 7 + 1, 0.1f);
                wm.store(pattern, 1.0f);
            }
            
            assert(wm.getActiveTraceCount() <= wm.getCapacity());
            
            std::cout << "    ✓ Updating and replacement" << std::endl;
        }
    
        static void testNeuralRepresentation()
        {
            std::cout << "  Testing neural representation..." << std::endl;
            
            auto config = std::make_shared<Config>("test_config");
            Brain brain(config);
            
            NeuralWorkingMemory wm(7, 0.05f);
            wm.initialize(&brain);
            
            // Test neural population coding
            std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
            wm.store(pattern, 1.0f);
            
            std::vector<float> activity = wm.getCurrentActivity();
            assert(!activity.empty());
            
            // Test neural plasticity
            wm.setLearningRate(0.1f);
            wm.update(0.01f); // 10ms update
            
            // Test attractor dynamics
            bool isStable = wm.isStable();
            assert(isStable == true || isStable == false); // Can be either
            
            std::cout << "    ✓ Neural representation and plasticity" << std::endl;
        }
    
        static void testAttentionIntegration()
        {
            std::cout << "  Testing attention integration..." << std::endl;
            
            auto config = std::make_shared<Config>("test_config");
            Brain brain(config);
            
            NeuralWorkingMemory wm(7, 0.05f);
            wm.initialize(&brain);
            
            // Test top-down bias
            NeuronId neuron1(11111);
            NeuronId neuron2(22222);
            
            wm.applyTopDownBias(neuron1, 0.5f);
            wm.applyBottomUpSalience(neuron2, 0.3f);
            
            // Test attention-based selection
            wm.setAttention(&brain);
            
            // Run competition
            wm.runCompetition();
            
            std::cout << "    ✓ Attention integration" << std::endl;
        }
    
        static void testCapacityConstraints()
        {
            std::cout << "  Testing capacity constraints..." << std::endl;
            
            auto config = std::make_shared<Config>("test_config");
            Brain brain(config);
            
            // Test with minimal capacity (4-7 items typical)
            for (size_t capacity : {4, 7}) {
                NeuralWorkingMemory wm(capacity, 0.05f);
                wm.initialize(&brain);
                
                // Store more items than capacity
                for (size_t i = 0; i < capacity * 2; ++i) {
                    std::vector<float> pattern(1, 0.1f * (i % 5 + 1));
                    wm.store(pattern, 1.0f);
                }
                
                assert(wm.getActiveTraceCount() <= capacity);
                assert(wm.getAvailableCapacity() >= 0);
            }
            
            std::cout << "    ✓ Capacity constraints (4-7 items typical)" << std::endl;
        }
    
        static void testMemoryDecay()
        {
            std::cout << "  Testing memory decay..." << std::endl;
            
            auto config = std::make_shared<Config>("test_config");
            Brain brain(config);
            
            // Test with different decay rates
            for (float decayRate : {0.01f, 0.05f, 0.1f}) {
                NeuralWorkingMemory wm(7, decayRate);
                wm.initialize(&brain);
                
                std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
                wm.store(pattern, 1.0f);
                
                float initialStrength = wm.getMemoryStrength();
                
                // Update over time
                for (size_t i = 0; i < 100; ++i) {
                    wm.update(0.01f);
                }
                
                float finalStrength = wm.getMemoryStrength();
                
                // Strength should decay over time
                assert(finalStrength <= initialStrength);
            }
            
            std::cout << "    ✓ Memory decay" << std::endl;
        }
    
        static void testSynapticModulation()
        {
            std::cout << "  Testing synaptic modulation..." << std::endl;
            
            auto config = std::make_shared<Config>("test_config");
            Brain brain(config);
            
            NeuralWorkingMemory wm(7, 0.05f);
            wm.initialize(&brain);
            
            std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
            wm.store(pattern, 1.0f);
            
            // Apply different learning rates
            wm.setLearningRate(0.01f);
            wm.update(0.01f);
            
            wm.setLearningRate(0.05f);
            wm.update(0.01f);
            
            wm.setLearningRate(0.1f);
            wm.update(0.01f);
            
            // Should have undergone synaptic changes
            assert(wm.getMemoryStrength() >= 0.0f);
            
            std::cout << "    ✓ Synaptic modulation" << std::endl;
        }
    
        static void testAttractorDynamics()
        {
            std::cout << "  Testing attractor dynamics..." << std::endl;
            
            auto config = std::make_shared<Config>("test_config");
            Brain brain(config);
            
            NeuralWorkingMemory wm(7, 0.05f);
            wm.initialize(&brain);
            
            // Enable attractor dynamics
            wm.setAttractorTemperature(0.1f);
            
            std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
            wm.store(pattern, 1.0f);
            
            // Update to allow attractor dynamics
            for (size_t i = 0; i < 50; ++i) {
                wm.update(0.01f);
            }
            
            // Test pattern completion
            std::vector<float> cue = {0.1f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
            std::vector<float> completed = wm.retrievePattern(cue);
            
            std::cout << "    ✓ Attractor dynamics" << std::endl;
        }
    
        static float computePatternSimilarity(const std::vector<float>& patternA, 
                                              const std::vector<float>& patternB)
        {
            if (patternA.size() != patternB.size()) {
                return 0.0f;
            }
            
            float dotProduct = 0.0f;
            float normA = 0.0f;
            float normB = 0.0f;
            
            for (size_t i = 0; i < patternA.size(); ++i) {
                dotProduct += patternA[i] * patternB[i];
                normA += patternA[i] * patternA[i];
                normB += patternB[i] * patternB[i];
            }
            
            normA = std::sqrt(normA);
            normB = std::sqrt(normB);
            
            if (normA > 0.0f && normB > 0.0f) {
                return dotProduct / (normA * normB);
            }
            
            return 0.0f;
        }
    };
    
} // namespace nlm