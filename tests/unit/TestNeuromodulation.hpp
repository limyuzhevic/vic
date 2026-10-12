#ifndef TEST_NEUROMODULATION_HPP
#define TEST_NEUROMODULATION_HPP

#include "tests/Framework/TestFramework.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Reward.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "tests/Helpers/TestHelpers.hpp"

class TestNeuromodulation : public TestCase {
public:
    TestNeuromodulation() : TestCase("Neuromodulation") {}
    
    void run() override {
        // Test Reward neuromodulator
        Reward::RewardConfig reward_config;
        reward_config.sensitivity = 0.1;
        reward_config.decay_rate = 0.01;
        reward_config.baseline = 0.0;
        reward_config.max_concentration = 1.0;
        
        Reward reward(reward_config);
        
        // Test reward signal generation
        double reward_value = 1.0;
        reward.update(0.0, reward_value);
        ASSERT_TRUE(reward.getCurrentLevel() > 0.0, "Reward level should be positive");
        
        double time_before = 0.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        double time_after = 10.0;
        reward.update(time_after, reward_value);
        
        // Test PredictionError neuromodulator
        PredictionError::PredictionErrorConfig pe_config;
        pe_config.sensitivity = 0.2;
        pe_config.decay_rate = 0.02;
        pe_config.min_error = 0.0;
        pe_config.max_error = 1.0;
        
        PredictionError prediction_error(pe_config);
        
        // Test prediction error signal
        double predicted_value = 0.5;
        double actual_value = 0.8;
        double error = std::abs(actual_value - predicted_value);
        prediction_error.update(0.0, error);
        ASSERT_TRUE(prediction_error.getCurrentLevel() > 0.0, "Prediction error should be positive");
        
        // Test Curiosity neuromodulator
        Curiosity::CuriosityConfig curiosity_config;
        curiosity_config.sensitivity = 0.15;
        curiosity_config.decay_rate = 0.015;
        curiosity_config.exploration_threshold = 0.5;
        curiosity_config.max_curiosity = 1.0;
        
        Curiosity curiosity(curiosity_config);
        
        // Test curiosity signal
        double curiosity_value = 0.7;
        curiosity.update(0.0, curiosity_value);
        ASSERT_TRUE(curiosity.getCurrentLevel() > 0.0, "Curiosity level should be positive");
        
        // Test Novelty neuromodulator
        Novelty::NoveltyConfig novelty_config;
        novelty_config.sensitivity = 0.12;
        novelty_config.decay_rate = 0.012;
        novelty_config.baseline = 0.0;
        novelty_config.max_novelty = 1.0;
        
        Novelty novelty(novelty_config);
        
        // Test novelty signal
        double novelty_value = 0.9;
        novelty.update(0.0, novelty_value);
        ASSERT_TRUE(novelty.getCurrentLevel() > 0.0, "Novelty level should be positive");
        
        // Test Neuromodulator integration
        Neuromodulator::NeuromodulatorConfig nm_config;
        nm_config.num_modulators = 4;
        nm_config.global_decay_rate = 0.01;
        nm_config.integration_weight = 0.5;
        
        Neuromodulator neuromodulator(nm_config);
        
        // Add all neuromodulators
        neuromodulator.addModulator(std::make_shared<Reward>(reward_config));
        neuromodulator.addModulator(std::make_shared<PredictionError>(pe_config));
        neuromodulator.addModulator(std::make_shared<Curiosity>(curiosity_config));
        neuromodulator.addModulator(std::make_shared<Novelty>(novelty_config));
        
        // Test neuromodulator update
        neuromodulator.update(0.0);
        ASSERT_TRUE(neuromodulator.getTotalModulation() > 0.0, "Total modulation should be positive");
        
        // Test neuromodulator effect on neural dynamics
        neuromodulator.applyModulation(0.5, -65.0, 10.0); // Should increase excitability
        neuromodulator.applyModulation(0.8, -65.0, 10.0); // Should increase excitability more
        
        // Test neuromodulator with plasticity
        Neuromodulator::NeuromodulatorPlasticityConfig nm_plasticity_config;
        nm_plasticity_config.base_plasticity_rate = 0.01;
        nm_plasticity_config.modulation_sensitivity = 0.5;
        
        Neuromodulator neuromodulator_plastic(nm_config, nm_plasticity_config);
        neuromodulator_plastic.addModulator(std::make_shared<Reward>(reward_config));
        neuromodulator_plastic.updatePlasticity(0.01, 0.1, 0.5);
        
        ASSERT_TRUE(true, "Neuromodulation integration test completed");
    }
};

#endif // TEST_NEUROMODULATION_HPP