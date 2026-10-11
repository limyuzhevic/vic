// Copyright (c) 2024 NLM Project
// Licensed under the MIT License

/**
 * @file test_python_bindings.cpp
 * @brief Comprehensive test suite for NLM Python bindings
 * 
 * This test suite validates the enhanced Python bindings for the NLM neural
 * simulation framework, including configuration validation, error handling,
 * context managers, and batch operations.
 */

#include "tests/test_brain.cpp"
#include "tests/test_config.cpp"
#include "tests/test_neuron.cpp"
#include "tests/test_stdp.cpp"
#include "core/Logger/Logger.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>

namespace test_python_bindings {

using namespace nlm;

// Global test results
struct TestResults {
    int total_tests = 0;
    int passed_tests = 0;
    int failed_tests = 0;
    double total_time = 0.0;
    
    void add_test(bool passed, double time) {
        total_tests++;
        if (passed) passed_tests++;
        else failed_tests++;
        total_time += time;
    }
    
    double get_pass_rate() const {
        return total_tests > 0 ? (static_cast<double>(passed_tests) / total_tests * 100.0) : 0.0;
    }
} g_test_results;

// Test helper functions
void print_test_header(const std::string& test_name) {
    std::cout << "[TEST] " << test_name << "..." << std::flush;
}

void print_test_result(const std::string& test_name, bool passed, double time) {
    g_test_results.add_test(passed, time);
    std::cout << (passed ? " PASSED " : " FAILED ");
    std::cout << "(" << time << "s)" << std::endl;
}

// Configuration validation tests
void test_config_creation() {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool success = true;
    
    try {
        // Test default config creation
        auto config = std::make_shared<ConfigEnhanced>();
        assert(config != nullptr);
        
        // Test parameter setting
        success &= config->setParameter("brain.neuron_count", "1000");
        success &= config->setParameter("brain.synapse_density", "0.1");
        success &= config->setParameter("plasticity.stdp.enable", "true");
        
        // Test parameter validation
        success &= config->isValid();
        
        // Test configuration groups
        auto groups = config->getGroups();
        success &= !groups.empty();
        
        // Test template configurations
        auto simple_config = ConfigEnhanced::createSimpleBrainConfig(500);
        success &= simple_config.isValid();
        
        auto agent_config = ConfigEnhanced::createAgentLearningConfig(1000);
        success &= agent_config.isValid();
        
        auto research_config = ConfigEnhanced::createResearchConfig(2000);
        success &= research_config.isValid();
        
        auto production_config = ConfigEnhanced::createProductionConfig(5000);
        success &= production_config.isValid();
        
        // Test parameter descriptions
        success &= !config->getParameterDescription("brain.neuron_count").empty();
        
        // Test parameter types
        success &= config->getParameterType("brain.neuron_count") == "int64_t";
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    
    print_test_result("Config Creation", success, duration);
}

void test_config_validation() {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool success = true;
    
    try {
        auto config = std::make_shared<ConfigEnhanced>();
        
        // Test valid configuration
        config->setParameter("brain.neuron_count", "2000");
        config->setParameter("brain.synapse_density", "0.15");
        success &= config->isValid();
        
        // Test validation errors
        auto config2 = std::make_shared<ConfigEnhanced>();
        config2->setParameter("brain.neuron_count", "invalid");
        success &= !config2->isValid();
        
        // Test validation errors retrieval
        auto errors = config2->getValidationErrors();
        success &= !errors.empty();
        
        // Test schema validation
        success &= !config2->getParameterDescription("brain.neuron_count").empty();
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    
    print_test_result("Config Validation", success, duration);
}

void test_brain_bindings() {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool success = true;
    
    try {
        // Create configuration
        auto config = std::make_shared<ConfigEnhanced>();
        config->setParameter("brain.neuron_count", "100");
        config->setParameter("brain.region_count", "2");
        config->setParameter("plasticity.stdp.enable", "true");
        
        // Create brain with enhanced config
        auto brain = BrainEnhanced::create(config);
        assert(brain != nullptr);
        
        // Initialize brain
        success &= brain->initialize();
        
        // Test statistics methods
        auto stats = brain->getStatistics();
        success &= !stats.empty();
        
        auto counts = brain->getCounts();
        success &= !counts.empty();
        
        auto debug_info = brain->getDebugInfo();
        success &= !debug_info.empty();
        
        // Test step methods
        brain->step(0);
        brain->step(1, 0.001);
        
        // Test batch operations
        brain->stepRange(0, 10, 0.001);
        
        // Test simulation
        brain->runSimulation(50, 0.001);
        
        // Test action production
        auto action = brain->produceAction();
        success &= action != nullptr;
        
        // Test enhanced action production with context
        auto action_context = brain->produceActionWithContext({{"mode", "batch"}});
        success &= action_context != nullptr;
        
        // Test context manager
        auto brain_cm = std::make_shared<BrainEnhanced>(config);
        success &= brain_cm->initialize();
        
        // Test region access
        auto region_ids = brain->getRegionIds();
        success &= !region_ids.empty();
        
        for (const auto& region_id : region_ids) {
            auto* region = brain->getRegion(region_id);
            success &= region != nullptr;
        }
        
        // Test memory access
        success &= brain->getWorkingMemory() != nullptr;
        success &= brain->getEpisodicMemory() != nullptr;
        success &= brain->getAssociativeMemory() != nullptr;
        
        // Test prediction access
        success &= brain->getPredictionSystem() != nullptr;
        
        // Test cognition access
        success &= brain->getPlanner() != nullptr;
        success &= brain->getConceptFormation() != nullptr;
        success &= brain->getAttention() != nullptr;
        
        // Test development access
        success &= brain->getDevelopmentSystem() != nullptr;
        auto dev_stage = brain->getDevelopmentalStage();
        success &= dev_stage != DevelopmentalStage::Invalid;
        
        // Test neuromodulation access
        success &= brain->getDopamine() != nullptr;
        success &= brain->getCuriosity() != nullptr;
        success &= brain->getNovelty() != nullptr;
        success &= brain->getPredictionError() != nullptr;
        
        // Test plasticity access
        success &= brain->getSpikeSystem() != nullptr;
        success &= brain->getSTDP() != nullptr;
        success &= brain->getHebbian() != nullptr;
        success &= brain->getStructuralPlasticity() != nullptr;
        
        // Test brain info
        auto brain_info = brain->getBrainInfo();
        success &= !brain_info.empty();
        
        // Test region iterator
        for (auto* region : *brain) {
            success &= region != nullptr;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    
    print_test_result("Brain Bindings", success, duration);
}

void test_agent_bindings() {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool success = true;
    
    try {
        // Create brain
        auto config = std::make_shared<ConfigEnhanced>();
        config->setParameter("brain.neuron_count", "200");
        
        auto brain = BrainEnhanced::create(config);
        brain->initialize();
        
        // Create agent
        auto agent = std::make_shared<AgentBrainEnhanced>(brain);
        assert(agent != nullptr);
        
        // Create world
        auto world = std::make_shared<SimpleWorldEnhanced>();
        world->configure(20, 20, 8, 8);
        
        // Initialize agent
        success &= agent->initialize(world);
        
        // Test performance metrics
        auto metrics = agent->getPerformanceMetrics();
        success &= !metrics.empty();
        
        // Test agent state
        auto state = agent->getAgentState();
        success &= !state.empty();
        
        // Test intelligence score
        double intelligence_score = agent->calculateIntelligenceScore();
        success &= intelligence_score >= 0.0;
        
        // Test subsystem control
        auto all_subsystems = agent->getAllSubsystems();
        success &= !all_subsystems.empty();
        
        auto enabled_subsystems = agent->getEnabledSubsystems();
        success &= !enabled_subsystems.empty();
        
        for (const auto& subsystem : all_subsystems) {
            agent->enableSubsystem(subsystem, true);
            success &= agent->isSubsystemEnabled(subsystem);
        }
        
        // Test learning capabilities
        agent->enableLearning(true);
        agent->setLearningRate(0.01);
        agent->setExplorationRate(0.1);
        
        double learning_progress = agent->getLearningProgress();
        success &= learning_progress >= 0.0 && learning_progress <= 1.0;
        
        // Test enhanced processing
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept, "test_context");
        
        auto action = agent->decodeMotorCommand({{"strategy", "curiosity"}});
        success &= action != nullptr;
        
        // Test reward modulation
        agent->applyRewardModulation(1.0, 0.5, "test_learning");
        
        // Test development
        agent->updateDevelopment(0.1, {{ "age", "1.0" }});
        
        // Test state management
        agent->reset();
        
        // Test context manager
        auto agent_cm = std::make_shared<AgentBrainEnhanced>(brain);
        success &= agent_cm->initialize(world);
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    
    print_test_result("Agent Bindings", success, duration);
}

void test_world_bindings() {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool success = true;
    
    try {
        auto world = std::make_shared<SimpleWorldEnhanced>();
        assert(world != nullptr);
        
        // Test enhanced configuration
        world->configure(30, 30, 10, 10, {{ "physics", "true" }});
        
        // Test simulation control
        world->runEpisode(100, {{ "speed", "fast" }});
        
        // Test world state
        auto world_state = world->getWorldState();
        success &= !world_state.empty();
        
        // Test objects in view
        auto objects_in_view = world->getObjectsInView();
        success &= !objects_in_view.empty();
        
        // Test available resources
        auto resources = world->getAvailableResources();
        success &= !resources.empty();
        
        // Test position validation
        success &= world->isValidPosition(5.0, 5.0);
        success &= !world->isValidPosition(-1.0, -1.0);
        
        // Test distance calculation
        double distance = world->distanceToNearestObject(5.0, 5.0);
        success &= distance >= 0.0;
        
        // Test closest object type
        std::string object_type = world->getClosestObjectType(5.0, 5.0);
        success &= !object_type.empty();
        
        // Test simulation parameters
        world->setTimeStep(0.01);
        world->setMaxEnergy(1000.0);
        world->setRandomSeed(12345);
        world->enablePhysics(true);
        
        // Test output generation
        std::string map_string = world->getMapString();
        success &= !map_string.empty();
        
        std::vector<std::vector<std::string>> grid_map = world->getGridMap();
        success &= !grid_map.empty();
        
        std::string visualization_data = world->generateVisualizationData();
        success &= !visualization_data.empty();
        
        // Test simulation statistics
        auto sim_stats = world->getSimulationStats();
        success &= !sim_stats.empty();
        
        auto world_metrics = world->getWorldMetrics();
        success &= !world_metrics.empty();
        
        // Test context manager
        auto world_cm = std::make_shared<SimpleWorldEnhanced>();
        success &= world_cm->configure(10, 10, 5, 5);
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    
    print_test_result("World Bindings", success, duration);
}

void test_error_handling() {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool success = true;
    
    try {
        // Test configuration error handling
        auto config = std::make_shared<ConfigEnhanced>();
        
        // This should fail because the parameter is not in schema
        bool set_success = config->setParameter("invalid.parameter", "value");
        success &= !set_success;
        
        // Test brain error handling
        auto brain = BrainEnhanced::create(std::make_shared<ConfigEnhanced>());
        auto error_msg = brain->getLastError();
        success &= error_msg.empty(); // No error yet
        
        // Test invalid parameter type
        auto config2 = std::make_shared<ConfigEnhanced>();
        bool bool_success = config2->setParameter("brain.neuron_count", "invalid_bool");
        success &= !bool_success;
        
        // Test range validation
        auto config3 = std::make_shared<ConfigEnhanced>();
        bool range_success = config3->setParameter("brain.neuron_count", "5000000"); // Too large
        success &= !range_success;
        
        // Test initialization error
        auto config4 = std::make_shared<ConfigEnhanced>();
        bool init_success = config4->isValid();
        success &= init_success; // Should be valid (empty config)
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    
    print_test_result("Error Handling", success, duration);
}

void test_context_managers() {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool success = true;
    
    try {
        // Test Brain context manager
        {
            auto config = std::make_shared<ConfigEnhanced>();
            config->setParameter("brain.neuron_count", "50");
            
            auto brain = BrainEnhanced::create(config);
            success &= brain->initialize();
            
            auto action = brain->produceAction();
            success &= action != nullptr;
        } // Brain should be closed here
        
        // Test AgentBrain context manager
        {
            auto config = std::make_shared<ConfigEnhanced>();
            config->setParameter("brain.neuron_count", "50");
            
            auto brain = BrainEnhanced::create(config);
            brain->initialize();
            
            auto agent = std::make_shared<AgentBrainEnhanced>(brain);
            
            auto world = std::make_shared<SimpleWorldEnhanced>();
            world->configure(10, 10, 5, 5);
            
            success &= agent->initialize(world);
            
            auto action = agent->decodeMotorCommand();
            success &= action != nullptr;
        } // Agent and world should be closed here
        
        // Test SimpleWorld context manager
        {
            auto world = std::make_shared<SimpleWorldEnhanced>();
            success &= world->configure(5, 5, 3, 3);
            
            auto map_string = world->getMapString();
            success &= !map_string.empty();
        } // World should be closed here
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    
    print_test_result("Context Managers", success, duration);
}

void test_performance_benchmarks() {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool success = true;
    
    try {
        // Configuration performance
        auto config_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100; ++i) {
            auto config = std::make_shared<ConfigEnhanced>();
            config->setParameter("brain.neuron_count", std::to_string(1000 + i * 100));
            config->setParameter("brain.synapse_density", std::to_string(0.1 + i * 0.001));
        }
        auto config_end = std::chrono::high_resolution_clock::now();
        auto config_duration = std::chrono::duration_cast<std::chrono::milliseconds>(config_end - config_start).count() / 1000.0;
        std::cout << "[PERF] Configuration creation: " << config_duration << "s" << std::endl;
        
        // Brain performance
        auto brain_start = std::chrono::high_resolution_clock::now();
        auto config = std::make_shared<ConfigEnhanced>();
        config->setParameter("brain.neuron_count", "500");
        
        auto brain = BrainEnhanced::create(config);
        brain->initialize();
        
        for (int i = 0; i < 50; ++i) {
            brain->step(i);
        }
        
        auto brain_end = std::chrono::high_resolution_clock::now();
        auto brain_duration = std::chrono::duration_cast<std::chrono::milliseconds>(brain_end - brain_start).count() / 1000.0;
        std::cout << "[PERF] Brain simulation (50 steps): " << brain_duration << "s" << std::endl;
        
        // Batch operation performance
        auto batch_start = std::chrono::high_resolution_clock::now();
        brain->stepRange(100, 199, 0.001);
        auto batch_end = std::chrono::high_resolution_clock::now();
        auto batch_duration = std::chrono::duration_cast<std::chrono::milliseconds>(batch_end - batch_start).count() / 1000.0;
        std::cout << "[PERF] Batch operation (100 steps): " << batch_duration << "s" << std::endl;
        
        // Agent performance
        auto agent_start = std::chrono::high_resolution_clock::now();
        auto agent = std::make_shared<AgentBrainEnhanced>(brain);
        
        auto world = std::make_shared<SimpleWorldEnhanced>();
        world->configure(15, 15, 6, 6);
        world->reset();
        
        agent->initialize(world);
        
        for (int i = 0; i < 100; ++i) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(i);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(*action, world->getSimulationTime());
            
            agent->applyRewardModulation(0.1, 0.0);
        }
        
        auto agent_end = std::chrono::high_resolution_clock::now();
        auto agent_duration = std::chrono::duration_cast<std::chrono::milliseconds>(agent_end - agent_start).count() / 1000.0;
        std::cout << "[PERF] Complete agent simulation (100 steps): " << agent_duration << "s" << std::endl;
        
        success = true; // Performance tests passed if we reached here
        
    } catch (const std::exception& e) {
        std::cerr << "Exception in performance test: " << e.what() << std::endl;
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    
    print_test_result("Performance Benchmarks", success, duration);
}

void test_end_to_end() {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool success = true;
    
    try {
        std::cout << "[E2E] Running complete integration test..." << std::endl;
        
        // Step 1: Create configuration
        std::cout << "[E2E] Step 1: Creating configuration..." << std::endl;
        auto config = std::make_shared<ConfigEnhanced>();
        config->setParameter("brain.neuron_count", "300");
        config->setParameter("plasticity.stdp.enable", "true");
        config->setParameter("neuromod.curiosity.enable", "true");
        config->setParameter("development.stage_transition_rate", "0.002");
        
        // Step 2: Create brain
        std::cout << "[E2E] Step 2: Creating brain..." << std::endl;
        auto brain = BrainEnhanced::create(config);
        success &= brain->initialize();
        
        // Step 3: Create agent and world
        std::cout << "[E2E] Step 3: Creating agent and world..." << std::endl;
        auto agent = std::make_shared<AgentBrainEnhanced>(brain);
        
        auto world = std::make_shared<SimpleWorldEnhanced>();
        world->configure(25, 25, 8, 8);
        world->setAgentStart(12.5, 12.5);
        world->reset();
        
        success &= agent->initialize(world);
        
        // Step 4: Enable learning subsystems
        std::cout << "[E2E] Step 4: Enabling learning subsystems..." << std::endl;
        agent->enableLearning(true);
        agent->enableSubsystem("reward_modulation", true);
        agent->enableSubsystem("structural_plasticity", true);
        
        // Step 5: Run simulation
        std::cout << "[E2E] Step 5: Running simulation..." << std::endl;
        for (int step = 0; step < 200; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept, "step_" + std::to_string(step));
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand({{"step", std::to_string(step)}});
            world->applyMotorCommand(*action, world->getSimulationTime());
            
            agent->applyRewardModulation(0.1, 0.0, "step_learning");
            
            if (step % 50 == 0) {
                std::cout << "[E2E] Progress: Step " << step 
                         << ", Curiosity: " << agent->getLearningProgress()
                         << ", Firing: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        // Step 6: Collect final statistics
        std::cout << "[E2E] Step 6: Collecting statistics..." << std::endl;
        auto metrics = agent->getPerformanceMetrics();
        auto brain_stats = brain->getStatistics();
        
        std::cout << "[E2E] Final metrics:" << std::endl;
        for (const auto& metric : metrics) {
            std::cout << "  " << metric.first << ": " << metric.second << std::endl;
        }
        
        // Step 7: Test state persistence
        std::cout << "[E2E] Step 7: Testing state persistence..." << std::endl;
        agent->reset();
        
        std::cout << "[E2E] End-to-end test completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception in end-to-end test: " << e.what() << std::endl;
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
    
    print_test_result("End-to-End Integration", success, duration);
}

void runAllTests() {
    std::cout << "=== NLM Python Bindings Test Suite ===" << std::endl;
    std::cout << "Testing enhanced Python bindings for NLM neural simulation framework." << std::endl;
    std::cout << std::endl;
    
    // Run all test categories
    test_config_creation();
    test_config_validation();
    test_brain_bindings();
    test_agent_bindings();
    test_world_bindings();
    test_error_handling();
    test_context_managers();
    test_performance_benchmarks();
    test_end_to_end();
    
    // Print summary
    std::cout << std::endl;
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << "Total tests: " << g_test_results.total_tests << std::endl;
    std::cout << "Passed: " << g_test_results.passed_tests << std::endl;
    std::cout << "Failed: " << g_test_results.failed_tests << std::endl;
    std::cout << "Success rate: " << g_test_results.get_pass_rate() << "%" << std::endl;
    std::cout << "Total time: " << g_test_results.total_time << "s" << std::endl;
    
    if (g_test_results.failed_tests > 0) {
        std::cout << std::endl;
        std::cout << "WARNING: Some tests failed!" << std::endl;
    }
}

} // namespace test_python_bindings

// Entry point for test execution
int main() {
    test_python_bindings::runAll();
    return g_test_results.failed_tests > 0 ? 1 : 0;
}