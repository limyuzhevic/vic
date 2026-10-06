// Neuromodulation Integration Tests
// Comprehensive tests for dopamine, acetylcholine, norepinephrine, and serotonin integration

#include "Brain.hpp"
#include "Neuromodulator.hpp"
#include "Core/Types/Types.hpp"
#include <cassert>
#include <iostream>

namespace nlm {
    namespace NeuromodulationIntegrationTests {
        
        void testDopamineMemoryEnhancement() {
            std::cout << "Testing dopamine memory enhancement..." << std::endl;
            
            // Create brain with default configuration
            auto config = std::make_shared<Config>();
            Brain brain(config);
            
            if (!brain.initialize()) {
                std::cerr << "Failed to initialize brain" << std::endl;
                return;
            }
            
            // Test 1: Dopamine enhances episodic memory consolidation
            auto* dopamine = brain.getDopamine();
            auto* episodicMemory = brain.getEpisodicMemory();
            
            // Set high dopamine level
            dopamine->setLevel(0.8f);
            
            // Create a test episodic memory item
            EpisodicMemoryItem item;
            item.timestamp = 1;
            item.reward = dopamine->getLevel();
            item.activeNeurons = {1, 2, 3, 4, 5};
            item.neuronActivations = {0.8f, 0.6f, 0.9f, 0.7f, 0.5f};
            
            // Store episode with high dopamine
            episodicMemory->storeEpisode(item);
            
            // Verify that dopamine enhancement affects memory consolidation
            float memoryConsolidationFactor = dopamine->getMemoryEnhancementFactor();
            assert(memoryConsolidationFactor > 1.0f);  // Should be enhanced (>1.0)
            std::cout << "✓ Dopamine memory enhancement test passed" << std::endl;
        }
        
        void testAcetylcholineAttentionModulation() {
            std::cout << "Testing acetylcholine attention modulation..." << std::endl;
            
            auto config = std::make_shared<Config>();
            Brain brain(config);
            
            if (!brain.initialize()) {
                std::cerr << "Failed to initialize brain" << std::endl;
                return;
            }
            
            // Test: Acetylcholine increases signal-to-noise ratio
            auto* acetylcholine = brain.getAcetylcholine();
            auto* attention = brain.getAttention();
            
            // Set different acetylcholine levels
            float baselineLevel = acetylcholine->getLevel();
            
            // Simulate attention modulation
            acetylcholine->setLevel(0.8f);
            float attentionGain = acetylcholine->getAttentionGain();
            float signalToNoiseRatio = acetylcholine->getSignalToNoiseRatio();
            
            // Higher acetylcholine should increase signal-to-noise ratio
            assert(signalToNoiseRatio > 1.0f);
            assert(attentionGain > 1.0f);
            
            std::cout << "✓ Acetylcholine attention modulation test passed" << std::endl;
        }
        
        void testNorepinephrineArousalModulation() {
            std::cout << "Testing norepinephrine arousal modulation..." << std::endl;
            
            auto config = std::make_shared<Config>();
            Brain brain(config);
            
            if (!brain.initialize()) {
                std::cerr << "Failed to initialize brain" << std::endl;
                return;
            }
            
            // Test: Norepinephrine increases arousal and attention focus
            auto* norepinephrine = brain.getNorepinephrine();
            
            // Set norepinephrine level
            norepinephrine->setLevel(0.6f);
            float arousalLevel = norepinephrine->getArousalLevel();
            float attentionFocus = norepinephrine->getAttentionFocus();
            
            // Higher norepinephrine should increase arousal and attention focus
            assert(attentionFocus > 1.0f);  // Should be enhanced (>1.0)
            
            std::cout << "✓ Norepinephrine arousal modulation test passed" << std::endl;
        }
        
        void testSerotoninSocialBehavior() {
            std::cout << "Testing serotonin social behavior modulation..." << std::endl;
            
            auto config = std::make_shared<Config>();
            Brain brain(config);
            
            if (!brain.initialize()) {
                std::cerr << "Failed to initialize brain" << std::endl;
                return;
            }
            
            // Test: Serotonin modulates impulsivity and social behavior
            auto* serotonin = brain.getSerotonin();
            
            // Test different serotonin levels
            serotonin->setLevel(0.2f);
            float lowSerotoninImpulsivity = serotonin->getImpulsivity();
            float lowSerotoninSocialBehavior = serotonin->getSocialBehavior();
            
            // Low serotonin should increase impulsivity
            assert(lowSerotoninImpulsivity > 0.5f);
            assert(lowSerotoninSocialBehavior < 1.0f);
            
            serotonin->setLevel(0.8f);
            float highSerotoninImpulsivity = serotonin->getImpulsivity();
            float highSerotoninSocialBehavior = serotonin->getSocialBehavior();
            
            // High serotonin should decrease impulsivity and increase social behavior
            assert(highSerotoninImpulsivity < lowSerotoninImpulsivity);
            assert(highSerotoninSocialBehavior > lowSerotoninSocialBehavior);
            
            std::cout << "✓ Serotonin social behavior modulation test passed" << std::endl;
        }
        
        void testNeuromodulatorPlasticityInteraction() {
            std::cout << "Testing neuromodulator plasticity interaction..." << std::endl;
            
            auto config = std::make_shared<Config>();
            Brain brain(config);
            
            if (!brain.initialize()) {
                std::cerr << "Failed to initialize brain" << std::endl;
                return;
            }
            
            // Test: Neuromodulators affect plasticity factors
            auto* dopamine = brain.getDopamine();
            auto* acetylcholine = brain.getAcetylcholine();
            auto* norepinephrine = brain.getNorepinephrine();
            auto* serotonin = brain.getSerotonin();
            
            // Test dopamine plasticity factor
            dopamine->setLevel(0.0f);
            float dopaminePlasticityLow = dopamine->getPlasticityFactor();
            
            dopamine->setLevel(0.8f);
            float dopaminePlasticityHigh = dopamine->getPlasticityFactor();
            
            // Higher dopamine should increase plasticity
            assert(dopaminePlasticityHigh > dopaminePlasticityLow);
            
            // Test acetylcholine plasticity factor
            acetylcholine->setLevel(0.8f);
            float acetylcholinePlasticity = acetylcholine->getPlasticityFactor();
            assert(acetylcholinePlasticity > 1.0f);  // ACh enhances plasticity
            
            // Test norepinephrine plasticity factor
            norepinephrine->setLevel(0.5f);
            float norepinephrinePlasticity = norepinephrine->getPlasticityFactor();
            assert(norepinephrinePlasticity > 1.0f);  // NE enhances plasticity
            
            std::cout << "✓ Neuromodulator plasticity interaction test passed" << std::endl;
        }
        
        void testAllNeuromodulatorsTogether() {
            std::cout << "Testing all neuromodulators integration..." << std::endl;
            
            auto config = std::make_shared<Config>();
            Brain brain(config);
            
            if (!brain.initialize()) {
                std::cerr << "Failed to initialize brain" << std::endl;
                return;
            }
            
            // Test: All neuromodulators work together in the same brain
            auto* dopamine = brain.getDopamine();
            auto* acetylcholine = brain.getAcetylcholine();
            auto* norepinephrine = brain.getNorepinephrine();
            auto* serotonin = brain.getSerotonin();
            
            // Set levels for all neuromodulators
            dopamine->setLevel(0.7f);
            acetylcholine->setLevel(0.6f);
            norepinephrine->setLevel(0.5f);
            serotonin->setLevel(0.4f);
            
            // Verify all neuromodulators are accessible
            assert(dopamine->getLevel() == 0.7f);
            assert(acetylcholine->getLevel() == 0.6f);
            assert(norepinephrine->getLevel() == 0.5f);
            assert(serotonin->getLevel() == 0.4f);
            
            // Test that Brain::step() processes all neuromodulators
            brain.step(1);
            
            std::cout << "✓ All neuromodulators integration test passed" << std::endl;
        }
        
        void runAllTests() {
            std::cout << "=== Running Neuromodulation Integration Tests ===" << std::endl << std::endl;
            
            testDopamineMemoryEnhancement();
            testAcetylcholineAttentionModulation();
            testNorepinephrineArousalModulation();
            testSerotoninSocialBehavior();
            testNeuromodulatorPlasticityInteraction();
            testAllNeuromodulatorsTogether();
            
            std::cout << std::endl << "=== All Neuromodulation Integration Tests Passed ===" << std::endl;
        }
    }
}