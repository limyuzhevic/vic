// brain_test_common.hpp - Common test utilities and fixtures for Phase 6 integration tests
// Author: Kilo
// Purpose: Shared test infrastructure for all Phase 6 integration tests

#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <memory>
#include <cassert>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>

namespace nlm {
namespace test {

/**
 * Test configuration for Phase 6 integration tests
 */
struct Phase6TestConfig {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    Phase6TestConfig()
        : maxSteps(1000)
        , neuronCount(500)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(false)
        , enableReplay(false)
        , enableDevelopment(false)
        , checkpointPath("/tmp/nlm_test_checkpoint.bin") {}
};

/**
 * Common test fixtures and utilities
 */
class Phase6TestFixture {
public:
    Phase6TestFixture(const Phase6TestConfig& config = Phase6TestConfig())
        : config(config)
        , success(true)
    {
        // Initialize logging for tests
        Logger::getInstance().setLevel(Logger::Level::Error);
    }
    
    virtual ~Phase6TestFixture() {}
    
    /**
     * Create a configured brain for testing
     */
    std::shared_ptr<Brain> createBrain() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
        cfg->set("connection_probability", config.connectionProbability, ConfigSource::Default);
        cfg->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        cfg->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        cfg->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        cfg->set("pruning_rate", 0.00001f, ConfigSource::Default);
        cfg->set("random_seed", 12345ULL, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "ERROR: Brain initialization failed" << std::endl;
            success = false;
        }
        return brain;
    }
    
    /**
     * Create an agent with brain integration
     */
    std::pair<std::shared_ptr<Brain>, std::unique_ptr<AgentBrain>> createAgent() {
        auto brain = createBrain();
        auto agent = std::make_unique<AgentBrain>(brain);
        
        // Create a simple world for testing
        SimpleWorld world;
        world.initialize(16, 16);
        
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(config.enableDevelopment);
        agent->enableDevelopment(config.enableDevelopment);
        agent->enableCuriosity(true);
        
        return {brain, std::move(agent)};
    }
    
    /**
     * Run a basic simulation loop
     */
    void runBasicSimulation(std::shared_ptr<Brain>& brain, std::unique_ptr<AgentBrain>& agent,
                           std::shared_ptr<SimpleWorld>& world) {
        if (!brain || !agent || !world) {
            std::cout << "ERROR: Invalid simulation components" << std::endl;
            success = false;
            return;
        }
        
        for (uint64_t step = 0; step < config.maxSteps; ++step) {
            // Get observation
            SensoryPercept percept = world->observe(brain->getRegions()[0].get());
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyAction(brain->getRegions()[0].get(), cmd);
            
            // Compute reward
            float reward = world->computeReward(brain->getRegions()[0].get());
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            if (config.enableDevelopment) {
                agent->updateDevelopment(0.001);
            }
            
            // Progress monitoring
            if (step % 1000 == 0) {
                std::cout << "  Step " << step 
                         << " | Firing: " << brain->getAverageFiringRate()
                         << " | Reward: " << reward << std::endl;
            }
        }
    }
    
    /**
     * Assert and fail gracefully
     */
    void assertTest(bool condition, const std::string& message) {
        if (!condition) {
            std::cout << "TEST FAILED: " << message << std::endl;
            success = false;
        }
    }
    
    /**
     * Get test success status
     */
    bool isSuccess() const { return success; }
    
    /**
     * Get test config
     */
    const Phase6TestConfig& getConfig() const { return config; }
    
protected:
    Phase6TestConfig config;
    bool success;
};

/**
 * Performance measurement utilities
 */
class PerformanceTimer {
public:
    PerformanceTimer() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime).count();
    }
    
    void reset() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};

/**
 * Test statistics collector
 */
class TestStats {
public:
    TestStats() : passed(0), failed(0), totalTime(0.0) {}
    
    void addTest(const std::string& name, bool passed, double time) {
        if (passed) {
            this->passed++;
        } else {
            this->failed++;
        }
        totalTime += time;
        testNames.push_back(name);
        testPassed.push_back(passed);
        testTimes.push_back(time);
    }
    
    void report() const {
        std::cout << "=== TEST SUMMARY ===" << std::endl;
        std::cout << "Tests passed: " << passed << std::endl;
        std::cout << "Tests failed: " << failed << std::endl;
        std::cout << "Total tests: " << (passed + failed) << std::endl;
        std::cout << "Total time: " << totalTime << "s" << std::endl;
        
        if (failed > 0) {
            std::cout << "\nFAILED TESTS:" << std::endl;
            for (size_t i = 0; i < testNames.size(); ++i) {
                if (!testPassed[i]) {
                    std::cout << "  " << testNames[i] << " (" << testTimes[i] << "s)" << std::endl;
                }
            }
        }
    }
    
    size_t getPassed() const { return passed; }
    size_t getFailed() const { return failed; }
    size_t getTotal() const { return passed + failed; }
    double getTotalTime() const { return totalTime; }
    
private:
    size_t passed;
    size_t failed;
    double totalTime;
    std::vector<std::string> testNames;
    std::vector<bool> testPassed;
    std::vector<double> testTimes;
};

} // namespace test
} // namespace nlm