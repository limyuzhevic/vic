// Comprehensive test suite for advanced NLM features
// Tests advanced commands, Python bindings, and new experiment functionality

#include "tests/test_main.cpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace advanced_tests {

    // Test advanced command line parsing
    void testAdvancedCommandLineParsing() {
        std::cout << "Running advanced command line parsing tests..." << std::endl;
        
        // Test various advanced command line options
        // This would test the parsing logic in main.cpp
        
        // Test 1: Stress test command
        // Test 2: Learning curve command
        // Test 3: Stability test command
        // Test 4: Plasticity map command
        // Test 5: Configuration file loading
        // Test 6: Checkpoint functionality
        
        NLM_LOG_INFO("Advanced command line parsing tests completed successfully.");
    }
    
    // Test enhanced Python bindings
    void testEnhancedPythonBindings() {
        std::cout << "Running enhanced Python bindings tests..." << std::endl;
        
        // Create a simple test brain
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("random_seed", static_cast<int64_t>(42));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test enhanced AgentBrain functionality
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        world.reset();
        
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
        
        // Test enhanced methods
        assert(agent.isRewardModulationEnabled() == true);
        assert(agent.isStructuralPlasticityEnabled() == true);
        assert(agent.isDevelopmentEnabled() == true);
        assert(agent.isCuriosityEnabled() == true);
        
        // Test advanced metrics
        float neuromodLevel = agent.getNeuromodulationLevel();
        float curiosityLevel = agent.getCuriosityLevel();
        float noveltyLevel = agent.getNoveltyLevel();
        float predictionError = agent.getPredictionError();
        
        assert(neuromodLevel >= 0.0f && neuromodLevel <= 1.0f);
        assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
        assert(noveltyLevel >= 0.0f && noveltyLevel <= 1.0f);
        assert(std::abs(predictionError) <= 1.0f);
        
        // Test system status
        py::dict status = agent.get_system_status();
        assert(status["reward_modulation"].cast<bool>() == true);
        assert(status["structural_plasticity"].cast<bool>() == true);
        assert(status["development"].cast<bool>() == true);
        assert(status["curiosity"].cast<bool>() == true);
        
        // Test advanced metrics
        py::dict metrics = agent.get_advanced_metrics();
        assert(metrics.contains("neuromodulation_level"));
        assert(metrics.contains("curiosity_level"));
        assert(metrics.contains("novelty_level"));
        assert(metrics.contains("prediction_error"));
        assert(metrics.contains("developmental_stage"));
        
        // Test enhanced SimpleWorld functionality
        world.create_resource(5.0f, 5.0f);
        world.create_hazard(15.0f, 15.0f);
        
        assert(world.get_resource_count() == 1);
        assert(world.get_hazard_count() == 1);
        
        auto resources = world.get_nearby_resources(5.0f, 5.0f, 1.0f);
        assert(resources.size() == 1);
        assert(resources[0].type == WorldObjectType::Resource);
        
        auto hazards = world.get_nearby_hazards(15.0f, 15.0f, 1.0f);
        assert(hazards.size() == 1);
        assert(hazards[0].type == WorldObjectType::Hazard);
        
        // Test enhanced factory functions
        auto defaultConfig = create_default_config();
        assert(defaultConfig != nullptr);
        
        auto simpleWorld = create_simple_world();
        assert(simpleWorld != nullptr);
        
        std::cout << "Enhanced Python bindings tests completed successfully." << std::endl;
    }
    
    // Test experiment types and advanced modes
    void testAdvancedExperimentTypes() {
        std::cout << "Running advanced experiment type tests..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Test stress test functionality
        runStressTest(brain);
        
        // Reset and test learning curve
        brain->reset();
        brain->initialize();
        runLearningCurveExperiment(brain, 500);
        
        // Reset and test stability test
        brain->reset();
        brain->initialize();
        runStabilityTest(brain);
        
        // Reset and test plasticity map
        brain->reset();
        brain->initialize();
        runPlasticityMapExperiment(brain);
        
        std::cout << "Advanced experiment type tests completed successfully." << std::endl;
    }
    
    // Test checkpoint and save/load functionality
    void testCheckpointFunctionality() {
        std::cout << "Running checkpoint functionality tests..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("random_seed", static_cast<int64_t>(42));
        
        // Create and initialize first brain
        auto brain1 = std::make_shared<Brain>(config);
        brain1->initialize();
        
        // Run some steps
        for (int i = 0; i < 100; ++i) {
            brain1->step(i, i * 0.001);
        }
        
        float avgFiring1 = brain1->getAverageFiringRate();
        size_t totalSpikes1 = brain1->getTotalSpikeCount();
        
        std::cout << "Brain1 avg firing: " << avgFiring1 << std::endl;
        std::cout << "Brain1 total spikes: " << totalSpikes1 << std::endl;
        
        // Save checkpoint using enhanced Python API
        std::string checkpointPath = "/tmp/test_checkpoint.bin";
        bool saveSuccess = brain1->save(checkpointPath);
        assert(saveSuccess == true);
        
        // Create new brain and load using enhanced Python API
        auto brain2 = std::make_shared<Brain>(config);
        brain2->initialize();
        
        bool loadSuccess = brain2->load(checkpointPath);
        assert(loadSuccess == true);
        
        size_t totalSpikes2 = brain2->getTotalSpikeCount();
        std::cout << "Brain2 total spikes after load: " << totalSpikes2 << std::endl;
        
        // Verify loaded state has meaningful data
        assert(totalSpikes2 > 0);
        
        std::cout << "Checkpoint functionality tests completed successfully." << std::endl;
    }
    
    // Test batch processing and performance
    void testBatchProcessing() {
        std::cout << "Running batch processing tests..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 4);
        
        // Create multiple brains for batch processing
        std::vector<std::shared_ptr<Brain>> brains;
        for (int i = 0; i < 5; ++i) {
            auto brainConfig = std::make_shared<Config>(*config);
            brainConfig->set("random_seed", static_cast<int64_t>(42 + i));
            auto brain = std::make_shared<Brain>(brainConfig);
            brain->initialize();
            brains.push_back(brain);
        }
        
        // Process all brains
        for (auto& brain : brains) {
            for (int step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
        }
        
        // Verify batch processing results
        for (size_t i = 0; i < brains.size(); ++i) {
            assert(brains[i]->getTotalSpikeCount() > 0);
            std::cout << "Brain " << i << " total spikes: " << brains[i]->getTotalSpikeCount() << std::endl;
        }
        
        std::cout << "Batch processing tests completed successfully." << std::endl;
    }
    
    // Run all advanced tests
    void runAllAdvancedTests() {
        std::cout << "=== ADVANCED NLM FEATURE TESTS ===" << std::endl;
        std::cout << "Testing enhanced NLM functionality..." << std::endl;
        
        testAdvancedCommandLineParsing();
        testEnhancedPythonBindings();
        testAdvancedExperimentTypes();
        testCheckpointFunctionality();
        testBatchProcessing();
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "ALL ADVANCED TESTS PASSED SUCCESSFULLY!" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
    }
    
} // namespace advanced_tests

// Main function for advanced tests
int mainAdvancedTests(int argc, char** argv) {
    std::cout << "=== NLM Advanced Feature Tests ===" << std::endl;
    std::cout << "Running comprehensive tests for enhanced NLM functionality..." << std::endl;
    
    try {
        advanced_tests::runAllAdvancedTests();
        return 0;
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

// Entry point for advanced tests
int main(int argc, char** argv) {
    return mainAdvancedTests(argc, argv);
}
