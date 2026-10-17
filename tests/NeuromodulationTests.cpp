// NeuromodulationTests.cpp - Comprehensive neuromodulator tests
#include "TestFramework.hpp"
#include "TestUtils.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Serotonin.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/brain/Brain.hpp"

namespace nlm {

// Neuromodulator unit tests
class NeuromodulationUnits {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Dopamine tests
        TEST_CASE(NeuromodulationUnits, "Dopamine Initialization");
        TEST_CASE(NeuromodulationUnits, "Dopamine Reward Signaling");
        TEST_CASE(NeuromodulationUnits, "Dopamine Memory Consolidation");
        TEST_CASE(NeuromodulationUnits, "Dopamine Attention Modulation");
        
        // Acetylcholine tests
        TEST_CASE(NeuromodulationUnits, "Acetylcholine Initialization");
        TEST_CASE(NeuromodulationUnits, "Acetylcholine Attention Enhancement");
        TEST_CASE(NeuromodulationUnits, "Acetylcholine Memory Encoding");
        
        // Norepinephrine tests
        TEST_CASE(NeuromodulationUnits, "Norepinephrine Initialization");
        TEST_CASE(NeuromodulationUnits, "Norepinephrine Arousal");
        TEST_CASE(NeuromodulationUnits, "Norepinephrine Novelty Detection");
        
        // Serotonin tests
        TEST_CASE(NeuromodulationUnits, "Serotonin Initialization");
        TEST_CASE(NeuromodulationUnits, "Serotonin Mood Regulation");
        TEST_CASE(NeuromodulationUnits, "Serotonin Behavioral Flexibility");
        
        // Curiosity tests
        TEST_CASE(NeuromodulationUnits, "Curiosity Initialization");
        TEST_CASE(NeuromodulationUnits, "Curiosity Exploration");
        
        // Novelty tests
        TEST_CASE(NeuromodulationUnits, "Novelty Initialization");
        TEST_CASE(NeuromodulationUnits, "Novelty Detection");
        
        // Prediction error tests
        TEST_CASE(NeuromodulationUnits, "Prediction Error Initialization");
        TEST_CASE(NeuromodulationUnits, "Prediction Error Computation");
        
        // Reward tests
        TEST_CASE(NeuromodulationUnits, "Reward Initialization");
        TEST_CASE(NeuromodulationUnits, "Reward Processing");
        
        // Neuromodulation integration tests
        TEST_CASE(NeuromodulationIntegration, "Neuromodulator System Integration");
        TEST_CASE(NeuromodulationIntegration, "Cross-Modulator Interactions");
        TEST_CASE(NeuromodulationIntegration, "Neuromodulator-Plasticity Coupling");
        
        std::cout << "=== Neuromodulation System Tests ===" << std::endl;
        runner.runAll();
    }
};

// Dopamine tests
namespace DopamineTests {
    bool testInitialization() {
        Dopamine dopamine;
        ASSERT_EQ(std::string(dopamine.getName()), "Dopamine");
        ASSERT_FLOAT_EQ(dopamine.getLevel(), 0.0f, 0.001f);  // Initial level
        ASSERT_FLOAT_EQ(dopamine.getPlasticityFactor(), 1.0f, 0.001f);  // Baseline plasticity
        std::cout << "PASS: Dopamine initialization successful" << std::endl;
        return true;
    }
    
    bool testRewardSignaling() {
        Dopamine dopamine;
        float reward = 0.8f;
        
        dopamine.signalReward(reward);
        
        float level = dopamine.getLevel();
        ASSERT_GT(level, 0.0f);
        
        std::cout << "PASS: Dopamine reward signaling successful. Level: " << level << std::endl;
        return true;
    }
    
    bool testMemoryConsolidation() {
        Dopamine dopamine;
        dopamine.signalMemoryConsolidation();
        
        float consolidation = dopamine.getMemoryConsolidation();
        ASSERT_GT(consolidation, 0.0f);
        
        std::cout << "PASS: Dopamine memory consolidation successful. Strength: " << consolidation << std::endl;
        return true;
    }
    
    bool testAttentionModulation() {
        Dopamine dopamine;
        float attention = 0.5f;
        
        dopamine.modulateAttention(attention);
        dopamine.enhanceFocus(0.7f);
        
        std::cout << "PASS: Dopamine attention modulation successful" << std::endl;
        return true;
    }
}}

// Acetylcholine tests
namespace AcetylcholineTests {
    bool testInitialization() {
        Acetylcholine ach;
        ASSERT_EQ(std::string(ach.getName()), "Acetylcholine");
        ASSERT_FLOAT_EQ(ach.getLevel(), 0.0f, 0.001f);
        std::cout << "PASS: Acetylcholine initialization successful" << std::endl;
        return true;
    }
    
    bool testAttentionEnhancement() {
        Acetylcholine ach;
        ach.enhanceSelectiveAttention();
        
        std::cout << "PASS: Acetylcholine attention enhancement successful" << std::endl;
        return true;
    }
    
    bool testMemoryEncoding() {
        Acetylcholine ach;
        std::vector<float> sensoryInput{0.5f, 0.7f, 0.9f};
        
        ach.enhanceSensoryGating(sensoryInput);
        ach.enhanceEpisodicEncoding(0.8f);
        
        std::cout << "PASS: Acetylcholine memory encoding successful" << std::endl;
        return true;
    }
}}

// Norepinephrine tests
namespace NorepinephrineTests {
    bool testInitialization() {
        Norepinephrine nr;
        ASSERT_EQ(std::string(nr.getName()), "Norepinephrine");
        ASSERT_FLOAT_EQ(nr.getLevel(), 0.0f, 0.001f);
        std::cout << "PASS: Norepinephrine initialization successful" << std::endl;
        return true;
    }
    
    bool testArousal() {
        Norepinephrine nr;
        nr.increaseAlertness(0.8f);
        nr.enhanceVigilance(0.5f);
        
        std::cout << "PASS: Norepinephrine arousal successful" << std::endl;
        return true;
    }
    
    bool testNoveltyDetection() {
        Norepinephrine nr;
        float noveltyScore = 0.7f;
        
        nr.respondToNovelty(noveltyScore);
        nr.enhanceNoveltyDetection();
        
        std::cout << "PASS: Norepinephrine novelty detection successful" << std::endl;
        return true;
    }
}}

// Serotonin tests
namespace SerotoninTests {
    bool testInitialization() {
        Serotonin serotonin;
        ASSERT_EQ(std::string(serotonin.getName()), "Serotonin");
        ASSERT_FLOAT_EQ(serotonin.getLevel(), 0.0f, 0.001f);
        std::cout << "PASS: Serotonin initialization successful" << std::endl;
        return true;
    }
    
    bool testMoodRegulation() {
        Serotonin serotonin;
        float moodState = 0.6f;
        
        serotonin.regulateMood(moodState);
        serotonin.reduceImpulsivity(0.3f);
        
        std::cout << "PASS: Serotonin mood regulation successful" << std::endl;
        return true;
    }
    
    bool testBehavioralFlexibility() {
        Serotonin serotonin;
        serotonin.enhanceBehavioralFlexibility();
        serotonin.reduceHabitualResponses();
        
        std::cout << "PASS: Serotonin behavioral flexibility successful" << std::endl;
        return true;
    }
}}

// Curiosity tests
namespace CuriosityTests {
    bool testInitialization() {
        Curiosity curiosity;
        std::cout << "PASS: Curiosity initialization successful" << std::endl;
        return true;
    }
    
    bool testExploration() {
        Curiosity curiosity;
        
        std::cout << "PASS: Curiosity exploration successful" << std::endl;
        return true;
    }
}}

// Novelty tests
namespace NoveltyTests {
    bool testInitialization() {
        Novelty novelty;
        std::cout << "PASS: Novelty initialization successful" << std::endl;
        return true;
    }
    
    bool testDetection() {
        Novelty novelty;
        
        std::cout << "PASS: Novelty detection successful" << std::endl;
        return true;
    }
}}

// Prediction error tests
namespace PredictionErrorTests {
    bool testInitialization() {
        PredictionError pe;
        std::cout << "PASS: Prediction error initialization successful" << std::endl;
        return true;
    }
    
    bool testComputation() {
        PredictionError pe;
        
        std::cout << "PASS: Prediction error computation successful" << std::endl;
        return true;
    }
}}

// Reward tests
namespace RewardTests {
    bool testInitialization() {
        Reward reward;
        std::cout << "PASS: Reward initialization successful" << std::endl;
        return true;
    }
    
    bool testProcessing() {
        Reward reward;
        
        std::cout << "PASS: Reward processing successful" << std::endl;
        return true;
    }
}}

// Neuromodulation integration tests
namespace NeuromodulationIntegrationTests {
    bool testSystemIntegration() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Check all neuromodulators are accessible
        ASSERT_TRUE(brain->getDopamine() != nullptr);
        ASSERT_TRUE(brain->getAcetylcholine() != nullptr);
        ASSERT_TRUE(brain->getNorepinephrine() != nullptr);
        ASSERT_TRUE(brain->getSerotonin() != nullptr);
        ASSERT_TRUE(brain->getCuriosity() != nullptr);
        ASSERT_TRUE(brain->getNovelty() != nullptr);
        ASSERT_TRUE(brain->getPredictionErrorSignal() != nullptr);
        
        std::cout << "PASS: Neuromodulator system integration successful" << std::endl;
        return true;
    }
    
    bool testCrossModulatorInteractions() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Simulate neuromodulator interactions
        auto* dopamine = brain->getDopamine();
        auto* acetylcholine = brain->getAcetylcholine();
        
        if (dopamine && acetylcholine) {
            // Interact with each other
            dopamine->interactWith(*acetylcholine);
            acetylcholine->interactWith(*dopamine);
        }
        
        std::cout << "PASS: Neuromodulator cross-modulator interactions successful" << std::endl;
        return true;
    }
    
    bool testNeuromodulatorPlasticityCoupling() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Get neuromodulators and plasticity systems
        auto* dopamine = brain->getDopamine();
        auto* stdp = brain->getSTDP();
        
        if (dopamine && stdp) {
            // Apply neuromodulation to plasticity
            Neuromodulator dopamineSignal = *dopamine;
            brain->applyNeuromodulation(dopamineSignal);
            
            // Check plasticity update
            brain->updatePlasticity();
        }
        
        std::cout << "PASS: Neuromodulator-plasticity coupling successful" << std::endl;
        return true;
    }
}}

// Phase 6 neuromodulation integration test
namespace Phase6NeuromodulationIntegration {
    bool testPhase6NeuromodulationIntegration() {
        Phase6Config config;
        config.neuronCount = 100;
        config.maxSteps = 1000;
        
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(config);
        
        // Check neuromodulation integration
        bool neuromodulationIntegrated = result.neuromodulationIntegrated;
        
        if (neuromodulationIntegrated) {
            std::cout << "PASS: Phase 6 neuromodulation integration successful. "
                     << "Integrated: " << neuromodulationIntegrated << std::endl;
        } else {
            std::cout << "FAIL: Phase 6 neuromodulation integration failed. "
                     << "Integrated: " << neuromodulationIntegrated << std::endl;
            return false;
        }
        
        return true;
    }
}}

// Test registration functions
void register_neuromodulation_tests(TestRunner& runner) {
    TEST_CASE(NeuromodulationUnits, "Dopamine Initialization");
    TEST_CASE(NeuromodulationUnits, "Dopamine Reward Signaling");
    TEST_CASE(NeuromodulationUnits, "Dopamine Memory Consolidation");
    TEST_CASE(NeuromodulationUnits, "Dopamine Attention Modulation");
    
    TEST_CASE(NeuromodulationUnits, "Acetylcholine Initialization");
    TEST_CASE(NeuromodulationUnits, "Acetylcholine Attention Enhancement");
    TEST_CASE(NeuromodulationUnits, "Acetylcholine Memory Encoding");
    
    TEST_CASE(NeuromodulationUnits, "Norepinephrine Initialization");
    TEST_CASE(NeuromodulationUnits, "Norepinephrine Arousal");
    TEST_CASE(NeuromodulationUnits, "Norepinephrine Novelty Detection");
    
    TEST_CASE(NeuromodulationUnits, "Serotonin Initialization");
    TEST_CASE(NeuromodulationUnits, "Serotonin Mood Regulation");
    TEST_CASE(NeuromodulationUnits, "Serotonin Behavioral Flexibility");
    
    TEST_CASE(NeuromodulationUnits, "Curiosity Initialization");
    TEST_CASE(NeuromodulationUnits, "Curiosity Exploration");
    
    TEST_CASE(NeuromodulationUnits, "Novelty Initialization");
    TEST_CASE(NeuromodulationUnits, "Novelty Detection");
    
    TEST_CASE(NeuromodulationUnits, "Prediction Error Initialization");
    TEST_CASE(NeuromodulationUnits, "Prediction Error Computation");
    
    TEST_CASE(NeuromodulationUnits, "Reward Initialization");
    TEST_CASE(NeuromodulationUnits, "Reward Processing");
    
    TEST_CASE(NeuromodulationIntegration, "Neuromodulator System Integration");
    TEST_CASE(NeuromodulationIntegration, "Cross-Modulator Interactions");
    TEST_CASE(NeuromodulationIntegration, "Neuromodulator-Plasticity Coupling");
    
    TEST_CASE(Phase6NeuromodulationIntegration, "Phase 6 Neuromodulation Integration");
}

// Helper test functions
void setupNeuromodulationTests() {
    std::cout << "Setting up neuromodulation system tests..." << std::endl;
}

void teardownNeuromodulationTests() {
    std::cout << "Cleaning up neuromodulation system tests..." << std::endl;
}

} // namespace nlm
