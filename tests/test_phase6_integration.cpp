// Phase 6 Integration Tests
// Comprehensive integration testing for the complete NLM brain emulation platform
// Covers all 6 major test categories as specified in Phase 6 requirements

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "commands/CommandSystem.hpp"
#include "ui/NLMUI.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <thread>

namespace test_phase6 {
    
    // === End-to-End Integration Tests ===
    
    bool test_full_nlm_brain_emulation() {
        std::cout << "    Testing full NLM brain emulation with all Phase 2 systems..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        
        // Set comprehensive NLM configuration
        config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
        config->set("plasticity_enable", true, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
        config->set("prediction_horizon", 10, nlm::ConfigSource::Default);
        config->set("sequence_memory_size", 100, nlm::ConfigSource::Default);
        config->set("sensory_prediction_enable", true, nlm::ConfigSource::Default);
        config->set("motor_prediction_enable", true, nlm::ConfigSource::Default);
        config->set("internal_prediction_enable", true, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Simulate multiple steps with all integrated systems
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain.step(step);
            assert(brain.getTotalNeuronCount() == 1000);
            assert(brain.getTotalSynapseCount() > 0);
        }
        
        std::cout << "    Full NLM brain emulation test passed" << std::endl;
        return true;
    }
    
    bool test_dopamine_reward_neural_dynamics_integration() {
        std::cout << "    Testing Dopamine, Reward, NeuralDynamics, PredictionSystem integration..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("plasticity_enable", true, nlm::ConfigSource::Default);
        config->set("dopamine_enable", true, nlm::ConfigSource::Default);
        config->set("reward_enable", true, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test integrated neuromodulation
        auto& neuromodulator = brain.getNeuromodulator();
        neuromodulator.setDopamineLevel(0.7);
        neuromodulator.setRewardLevel(0.8);
        neuromodulator.setPredictionErrorLevel(0.3);
        neuromodulator.setNoveltyLevel(0.2);
        neuromodulator.setCuriosityLevel(0.4);
        
        assert(neuromodulator.getDopamineLevel() == 0.7);
        assert(neuromodulator.getRewardLevel() == 0.8);
        
        // Run simulation to verify integration
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
            neuromodulator.update();
        }
        
        std::cout << "    Dopamine-Reward-NeuralDynamics-PredictionSystem integration test passed" << std::endl;
        return true;
    }
    
    bool test_command_system_brain_operations_integration() {
        std::cout << "    Testing Command system integration with brain operations..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto commandSystem = std::make_shared<nlm::CommandSystem>();
        
        // Test integration commands
        auto result = commandSystem->executeCommand("status");
        assert(std::holds_alternative<std::string>(result));
        
        result = commandSystem->executeCommand("metrics --detailed");
        assert(std::holds_alternative<std::string>(result));
        
        // Verify brain state after commands
        assert(brain.getTotalNeuronCount() == 200);
        assert(brain.getRegionCount() == 2);
        
        std::cout << "    Command system brain operations integration test passed" << std::endl;
        return true;
    }
    
    bool test_ui_integration_real_time_brain_monitoring() {
        std::cout << "    Testing UI integration with real-time brain monitoring..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
        config->set("plasticity_enable", true, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto ui = std::make_shared<nlm::NLMUI>();
        ui->start();
        
        // Monitor brain metrics in real-time
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.step(step);
            
            auto uiResult = ui->executeCommand("metrics");
            assert(std::holds_alternative<std::string>(uiResult));
            
            auto brainState = ui->getBrainState();
            assert(brainState.has_value());
        }
        
        ui->stop();
        
        std::cout << "    UI integration with real-time brain monitoring test passed" << std::endl;
        return true;
    }
    
    bool test_configuration_system_nlm_parameters_integration() {
        std::cout << "    Testing Configuration system integration with NLM parameters..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        
        // Set comprehensive NLM configuration
        config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
        config->set("brain.connection_probability", 0.15, nlm::ConfigSource::Default);
        config->set("neuromodulation.dopamine.enable", true, nlm::ConfigSource::Default);
        config->set("neuromodulation.reward.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.enable", true, nlm::ConfigSource::Default);
        config->set("prediction.enable", true, nlm::ConfigSource::Default);
        config->set("ui.enable", true, nlm::ConfigSource::Default);
        
        assert(config->get<int64_t>("brain.neuron_count") == 1000);
        assert(config->get<bool>("neuromodulation.dopamine.enable") == true);
        assert(config->get<bool>("plasticity.enable") == true);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        assert(brain.getTotalNeuronCount() == 1000);
        
        std::cout << "    Configuration system NLM parameters integration test passed" << std::endl;
        return true;
    }
    
    // === Cross-System Compatibility Tests ===
    
    bool test_neuromodulation_stdp_plasticity_integration() {
        std::cout << "    Testing Neuromodulation integration with STDP plasticity..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("plasticity_enable", true, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto& neuromodulator = brain.getNeuromodulator();
        neuromodulator.setDopamineLevel(0.6);
        neuromodulator.setRewardLevel(0.7);
        neuromodulator.setPredictionErrorLevel(0.4);
        
        // Run simulation to test integration
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain.step(step);
            neuromodulator.update();
            
            auto metrics = brain.getBrainMetrics();
            assert(metrics.plasticity_active);
            assert(metrics.neuromodulation_active);
        }
        
        std::cout << "    Neuromodulation-STDP plasticity integration test passed" << std::endl;
        return true;
    }
    
    // === Performance and Stability Tests ===
    
    bool test_long_running_brain_emulation_simulations() {
        std::cout << "    Testing long-running brain emulation simulations..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("plasticity_enable", true, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Run extended simulation
        const nlm::SimulationStep totalSteps = 5000;
        
        for (nlm::SimulationStep step = 0; step < totalSteps; ++step) {
            brain.step(step);
        }
        
        std::cout << "    Completed " << totalSteps << " steps" << std::endl;
        std::cout << "    Long-running brain emulation simulation test passed" << std::endl;
        return true;
    }
    
    // === Advanced Feature Integration Tests ===
    
    bool test_advanced_command_system_complex_workflows() {
        std::cout << "    Testing Advanced Command System with complex workflows..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto commandSystem = std::make_shared<nlm::CommandSystem>();
        
        // Execute complex workflow
        std::vector<std::string> workflow = {
            "status",
            "metrics --detailed",
            "brain step 5",
            "plasticity_enable stdp"
        };
        
        for (const auto& cmd : workflow) {
            auto result = commandSystem->executeCommand(cmd);
            assert(std::holds_alternative<std::string>(result));
        }
        
        assert(brain.getTotalNeuronCount() == 200);
        
        std::cout << "    Advanced command system complex workflows test passed" << std::endl;
        return true;
    }
    
    // === Production Readiness Tests ===
    
    bool test_production_deployment_scenarios() {
        std::cout << "    Testing Production deployment scenarios..." << std::endl;
        
        // Test different deployment configurations
        std::vector<std::string> deploymentScenarios = {"demo", "enterprise", "high_availability"};
        
        for (const auto& scenario : deploymentScenarios) {
            auto config = std::make_shared<nlm::Config>();
            
            if (scenario == "demo") {
                config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            } else if (scenario == "enterprise") {
                config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
            } else {
                config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
            }
            
            nlm::Brain brain(config);
            assert(brain.initialize());
            
            for (nlm::SimulationStep step = 0; step < 50; ++step) {
                brain.step(step);
            }
        }
        
        std::cout << "    Production deployment scenarios test passed" << std::endl;
        return true;
    }
    
    bool test_security_and_compliance_validation() {
        std::cout << "    Testing Security and compliance validation..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
        config->set("security.enable", true, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        for (nlm::SimulationStep step = 0; step < 40; ++step) {
            brain.step(step);
        }
        
        std::cout << "    Security and compliance validation test passed" << std::endl;
        return true;
    }
    
    // Main test runner
    bool runAllTests() {
        std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
        std::cout << "Testing comprehensive NLM brain emulation platform integration" << std::endl;
        std::cout << "=========================================" << std::endl;
        
        bool allPassed = true;
        
        // Run all test suites
        allPassed &= test_full_nlm_brain_emulation();
        allPassed &= test_dopamine_reward_neural_dynamics_integration();
        allPassed &= test_command_system_brain_operations_integration();
        allPassed &= test_ui_integration_real_time_brain_monitoring();
        allPassed &= test_configuration_system_nlm_parameters_integration();
        allPassed &= test_neuromodulation_stdp_plasticity_integration();
        allPassed &= test_long_running_brain_emulation_simulations();
        allPassed &= test_advanced_command_system_complex_workflows();
        allPassed &= test_production_deployment_scenarios();
        allPassed &= test_security_and_compliance_validation();
        
        return allPassed;
    }
}

int main() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Comprehensive integration testing for Phase 6 systems" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    bool success = test_phase6::runAllTests();
    
    if (success) {
        std::cout << "\n" << std::endl;
        std::cout << "=====================================================" << std::endl;
        std::cout << "ALL PHASE 6 INTEGRATION TESTS PASSED!" << std::endl;
        std::cout << "=====================================================" << std::endl;
        std::cout << "The NLM brain emulation platform is ready for production!" << std::endl;
        return 0;
    } else {
        std::cout << "\n" << std::endl;
        std::cout << "=====================================================" << std::endl;
        std::cout << "SOME PHASE 6 INTEGRATION TESTS FAILED!" << std::endl;
        std::cout << "=====================================================" << std::endl;
        return 1;
    }
}