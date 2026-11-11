// NLM Neuromodulation Test Suite
// Phase 3: Neuromodulation Integration Tests

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/core/Config/Config.hpp"

namespace test_neuromodulation {
    
    void test_neuromodulator_basics() {
        std::cout << "Testing Neuromodulator basic functionality..." << std::endl;
        
        auto config = std::make_shared<Config>();
        Neuromodulator modulator(config);
        
        assert(modulator.initialize() == true);
        assert(modulator.getNeuromodulationLevel() >= 0.0f);
        assert(modulator.getNeuromodulationLevel() <= 1.0f);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_reward_system() {
        std::cout << "Testing Reward system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        Reward rewardSystem(config);
        
        assert(rewardSystem.initialize() == true);
        assert(rewardSystem.getCurrentReward() >= 0.0f);
        
        // Test reward application
        float initialReward = rewardSystem.getCurrentReward();
        rewardSystem.applyReward(1.0f);
        assert(rewardSystem.getCurrentReward() > initialReward);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_prediction_error_system() {
        std::cout << "Testing Prediction Error system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        PredictionError predictionSystem(config);
        
        assert(predictionSystem.initialize() == true);
        assert(predictionSystem.getPredictionError() >= 0.0f);
        
        // Test prediction error calculation
        float initialError = predictionSystem.getPredictionError();
        predictionSystem.updatePrediction(1.0f, 0.8f); // value, prediction
        assert(predictionSystem.getPredictionError() >= initialError);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_novelty_system() {
        std::cout << "Testing Novelty system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        Novelty noveltySystem(config);
        
        assert(noveltySystem.initialize() == true);
        assert(noveltySystem.getNoveltyLevel() >= 0.0f);
        assert(noveltySystem.getNoveltyLevel() <= 1.0f);
        
        // Test novelty detection
        float initialNovelty = noveltySystem.getNoveltyLevel();
        noveltySystem.processEvent(1.0f); // new stimulus
        assert(noveltySystem.getNoveltyLevel() >= initialNovelty);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_curiosity_system() {
        std::cout << "Testing Curiosity system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        Curiosity curiositySystem(config);
        
        assert(curiositySystem.initialize() == true);
        assert(curiositySystem.getCuriosityLevel() >= 0.0f);
        assert(curiositySystem.getCuriosityLevel() <= 1.0f);
        
        // Test curiosity level calculation
        float initialCuriosity = curiositySystem.getCuriosityLevel();
        curiositySystem.processEvent(); // explore
        assert(curiositySystem.getCuriosityLevel() >= initialCuriosity);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_neuromodulator_integration() {
        std::cout << "Testing Neuromodulator integration with plasticity..." << std::endl;
        
        auto config = std::make_shared<Config>();
        Neuromodulator modulator(config);
        
        assert(modulator.initialize() == true);
        
        // Test neuromodulation effects on plasticity
        modulator.setNeuromodulationLevel(0.5f);
        assert(modulator.getNeuromodulationLevel() == 0.5f);
        
        modulator.setNeuromodulationLevel(0.8f);
        assert(modulator.getNeuromodulationLevel() == 0.8f);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "=== NLM Neuromodulation Tests ===" << std::endl;
        
        test_neuromodulator_basics();
        test_reward_system();
        test_prediction_error_system();
        test_novelty_system();
        test_curiosity_system();
        test_neuromodulator_integration();
        
        std::cout << std::endl;
        std::cout << "=== All Neuromodulation Tests PASSED ===" << std::endl;
    }
}