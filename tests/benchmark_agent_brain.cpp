// Performance Benchmarks for NLM AgentBrain
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

namespace benchmark_agent_brain {

class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start_).count();
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_;
};

void benchmarkRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    agentBrain.enableAllFeatures();
    
    const int iterations = 1000;
    
    Timer timer;
    for (int i = 0; i < iterations; ++i) {
        agentBrain.applyRewardModulation(
            static_cast<float>(rand()) / RAND_MAX,  // random reward
            static_cast<float>(rand()) / RAND_MAX   // random predicted reward
        );
    }
    
    double elapsed = timer.elapsed();
    double avgTimePerCall = elapsed / iterations;
    
    std::cout << "    Reward Modulation Benchmark:" << std::endl;
    std::cout << "      Total time: " << elapsed << " ms" << std::endl;
    std::cout << "      Average time per call: " << avgTimePerCall << " ms" << std::endl;
    std::cout << "      Calls per second: " << (iterations / (elapsed / 1000.0)) << std::endl;
}

void benchmarkSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 32, 32); // Larger world for more realistic test
    agentBrain.initialize(world);
    
    // Create test percept with realistic data
    nlm::SensoryPercept percept;
    std::vector<float> vision(1024, 0.0f); // 32x32 vision grid
    std::vector<float> touch(8, 0.0f);
    std::vector<float> internal(4, 0.0f);
    std::vector<float> proprioception(6, 0.0f);
    
    // Fill with realistic data
    for (size_t i = 0; i < vision.size(); ++i) {
        vision[i] = static_cast<float>(rand()) / RAND_MAX;
    }
    for (size_t i = 0; i < touch.size(); ++i) {
        touch[i] = static_cast<float>(rand()) / RAND_MAX * 0.1f;
    }
    for (size_t i = 0; i < internal.size(); ++i) {
        internal[i] = (static_cast<float>(rand()) / RAND_MAX * 2.0f) - 1.0f;
    }
    for (size_t i = 0; i < proprioception.size(); ++i) {
        proprioception[i] = (static_cast<float>(rand()) / RAND_MAX * 2.0f) - 1.0f;
    }
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    
    const int iterations = 500;
    
    Timer timer;
    for (int i = 0; i < iterations; ++i) {
        agentBrain.processSensoryInput(percept);
    }
    
    double elapsed = timer.elapsed();
    double avgTimePerCall = elapsed / iterations;
    
    std::cout << "    Sensory Processing Benchmark:" << std::endl;
    std::cout << "      Total time: " << elapsed << " ms" << std::endl;
    std::cout << "      Average time per call: " << avgTimePerCall << " ms" << std::endl;
    std::cout << "      Calls per second: " << (iterations / (elapsed / 1000.0)) << std::endl;
}

void benchmarkMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    const int iterations = 1000;
    
    Timer timer;
    for (int i = 0; i < iterations; ++i) {
        nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
        (void)cmd; // Use the command to avoid unused variable warning
    }
    
    double elapsed = timer.elapsed();
    double avgTimePerCall = elapsed / iterations;
    
    std::cout << "    Motor Decoding Benchmark:" << std::endl;
    std::cout << "      Total time: " << elapsed << " ms" << std::endl;
    std::cout << "      Average time per call: " << avgTimePerCall << " ms" << std::endl;
    std::cout << "      Calls per second: " << (iterations / (elapsed / 1000.0)) << std::endl;
}

void benchmarkDevelopmentUpdates() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    agentBrain.enableDevelopment(true);
    
    const int iterations = 100; // Development updates are less frequent
    double timestep = 1.0;
    
    Timer timer;
    for (int i = 0; i < iterations; ++i) {
        agentBrain.updateDevelopment(timestep);
    }
    
    double elapsed = timer.elapsed();
    double avgTimePerCall = elapsed / iterations;
    
    std::cout << "    Development Updates Benchmark:" << std::endl;
    std::cout << "      Total time: " << elapsed << " ms" << std::endl;
    std::cout << "      Average time per call: " << avgTimePerCall << " ms" << std::endl;
    std::cout << "      Calls per second: " << (iterations / (elapsed / 1000.0)) << std::endl;
}

void benchmarkCompleteAgentCycle() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    nlm::SimpleWorld world;
    world.configure(10, 10, 16, 16);
    agentBrain.initialize(world);
    
    // Create test percept
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.5f);
    percept.setVision(vision);
    
    const int cycles = 200;
    
    Timer timer;
    for (int i = 0; i < cycles; ++i) {
        // Simulate one complete agent cycle
        agentBrain.processSensoryInput(percept);
        agentBrain.applyRewardModulation(0.5f, 0.3f); // Simulated reward
        nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
        agentBrain.updateDevelopment(1.0);
        (void)cmd; // Use the command
    }
    
    double elapsed = timer.elapsed();
    double avgTimePerCycle = elapsed / cycles;
    
    std::cout << "    Complete Agent Cycle Benchmark:" << std::endl;
    std::cout << "      Total time: " << elapsed << " ms" << std::endl;
    std::cout << "      Average time per cycle: " << avgTimePerCycle << " ms" << std::endl;
    std::cout << "      Cycles per second: " << (cycles / (elapsed / 1000.0)) << std::endl;
}

void benchmarkMemoryUsage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Measure memory usage of multiple agent instances
    const int instances = 10;
    std::vector<std::shared_ptr<nlm::AgentBrain>> agents;
    
    Timer timer;
    for (int i = 0; i < instances; ++i) {
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        agents.push_back(agent);
        
        // Do some work
        agent->processSensoryInput(nlm::SensoryPercept());
        agent->decodeMotorCommand();
    }
    
    double elapsed = timer.elapsed();
    double avgTimePerInstance = elapsed / instances;
    
    std::cout << "    Memory Usage Benchmark:" << std::endl;
    std::cout << "      Time to create " << instances << " agent instances: " << elapsed << " ms" << std::endl;
    std::cout << "      Average time per instance: " << avgTimePerInstance << " ms" << std::endl;
}

void runAllBenchmarks() {
    std::cout << "=== NLM AgentBrain Performance Benchmarks ===" << std::endl;
    benchmarkRewardModulation();
    std::cout << std::endl;
    benchmarkSensoryProcessing();
    std::cout << std::endl;
    benchmarkMotorDecoding();
    std::cout << std::endl;
    benchmarkDevelopmentUpdates();
    std::cout << std::endl;
    benchmarkCompleteAgentCycle();
    std::cout << std::endl;
    benchmarkMemoryUsage();
    std::cout << "=== Benchmarks Complete ===" << std::endl;
}

} // namespace benchmark_agent_brain