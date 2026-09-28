#include "../integration/BrainIntegration.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <random>

using namespace nlm;

class PerformanceBenchmark {
public:
    PerformanceBenchmark() {
        // Create a simple configuration
        config_ = std::make_shared<Config>();
        config_->set("neuron_count", 500);
        config_->set("region_count", 1);
        config_->set("simulation_timestep", 0.001);
        
        // Create brain integration
        brainIntegration_ = std::make_unique<BrainIntegration>(std::make_shared<Brain>(config_));
        
        if (brainIntegration_->initialize()) {
            std::cout << "Benchmark initialized successfully" << std::endl;
        } else {
            std::cout << "Benchmark initialization failed" << std::endl;
            return;
        }
        
        // Enable optimizations
        brainIntegration_->enableOptimizations(true);
        brainIntegration_->enableSIMD(true);
        brainIntegration_->enableCaching(true);
        brainIntegration_->enableMultithreading(true);
    }
    
    void runBenchmark() {
        std::cout << "\n=== Performance Benchmark for Optimized NLM Brain ===" << std::endl;
        
        // Benchmark parameters
        const size_t numSteps = 1000;
        const size_t numInputs = 100;
        
        std::cout << "Running " << numSteps << " simulation steps with " << numInputs << " sensory inputs per step..." << std::endl;
        
        // Generate random sensory inputs
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        std::vector<float> vision(256);
        std::vector<float> touch(8);
        std::vector<float> internal(4);
        std::vector<float> proprioception(6);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation
        for (size_t step = 0; step < numSteps; ++step) {
            // Generate random sensory input
            for (size_t i = 0; i < 256; ++i) vision[i] = dist(gen);
            for (size_t i = 0; i < 8; ++i) touch[i] = dist(gen);
            for (size_t i = 0; i < 4; ++i) internal[i] = dist(gen);
            for (size_t i = 0; i < 6; ++i) proprioception[i] = dist(gen);
            
            // Create sensory input
            SensoryInput input;
            input.setVision(vision);
            input.setTouch(touch);
            input.setInternal(internal);
            input.setProprioception(proprioception);
            
            // Process sensory input
            brainIntegration_->receiveSensoryInput(input);
            
            // Generate action
            auto action = brainIntegration_->produceAction();
            
            // Advance simulation
            brainIntegration_->step(step, static_cast<Timestamp>(step * 0.001));
            
            // Update performance stats periodically
            if (step % 100 == 0) {
                brainIntegration_->getStats();
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        // Calculate statistics
        double stepsPerSecond = (numSteps / duration) * 1000.0;
        double avgStepTime = duration / numSteps;
        
        std::cout << "\n=== Benchmark Results ===" << std::endl;
        std::cout << "Total steps: " << numSteps << std::endl;
        std::cout << "Total time: " << duration << " ms" << std::endl;
        std::cout << "Steps per second: " << stepsPerSecond << std::endl;
        std::cout << "Average step time: " << avgStepTime << " ms" << std::endl;
        
        // Get final performance statistics
        auto stats = brainIntegration_->getStats();
        
        std::cout << "\n=== Performance Statistics ===" << std::endl;
        std::cout << "Memory Usage: " << stats.memoryUsageMB << " MB" << std::endl;
        std::cout << "Active Neurons: " << stats.activeNeurons << std::endl;
        std::cout << "Total Spikes: " << stats.totalSpikes << std::endl;
        std::cout << "Average Firing Rate: " << stats.avgFiringRate << " Hz" << std::endl;
        std::cout << "Prediction Accuracy: " << (stats.predictionAccuracy * 100) << "%" << std::endl;
        std::cout << "Cache Hit Rate: " << stats.cacheHits << "%" << std::endl;
        std::cout << "Neuromodulator Stability: " << stats.neuromodulatorStability << std::endl;
        
        // Validate performance
        std::cout << "\n=== Performance Validation ===" << std::endl;
        if (brainIntegration_->validatePerformance()) {
            std::cout << "✓ Performance validation PASSED" << std::endl;
            std::cout << "✓ All optimizations are working correctly" << std::endl;
        } else {
            std::cout << "✗ Performance validation FAILED" << std::endl;
        }
        
        // Log final performance stats
        brainIntegration_->logPerformanceStats();
        
        std::cout << "\n=== Benchmark Complete ===" << std::endl;
    }
    
private:
    std::shared_ptr<Config> config_;
    std::unique_ptr<BrainIntegration> brainIntegration_;
};

int main() {
    std::cout << "NLM Performance Optimization Benchmark" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    PerformanceBenchmark benchmark;
    benchmark.runBenchmark();
    
    return 0;
}