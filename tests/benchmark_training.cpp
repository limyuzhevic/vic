// Training performance benchmarks
#include "../src/training/TrainingModule.hpp"
#include "../src/training/TrainingCore.hpp"
#include "../src/training/TrainingComponents.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <iomanip>

namespace benchmark_training {

struct TrainingBenchmarkResults {
    double timeMs;
    size_t episodes;
    double avgRewardPerEpisode;
    double avgLossPerEpisode;
    std::string description;
    
    TrainingBenchmarkResults(double time, size_t episodes, double avgReward, double avgLoss, const std::string& desc)
        : timeMs(time), episodes(episodes), avgRewardPerEpisode(avgReward), avgLossPerEpisode(avgLoss), description(desc) {}
    
    void print() const {
        std::cout << std::setw(80) << std::left << description
                  << std::setw(15) << std::right << timeMs << " ms"
                  << std::setw(12) << std::right << episodes << " eps"
                  << std::setw(15) << std::right << std::fixed << std::setprecision(4) << avgRewardPerEpisode << " reward"
                  << std::setw(15) << std::right << std::fixed << std::setprecision(4) << avgLossPerEpisode << " loss" << std::endl;
    }
};

TrainingBenchmarkResults benchmarkTrainingLoop(size_t neuronCount, size_t episodes, size_t stepsPerEpisode) {
    auto start = std::chrono::high_resolution_clock::now();
    
    double totalReward = 0.0;
    double totalLoss = 0.0;
    
    for (size_t episode = 0; episode < episodes; ++episode) {
        // Create brain for this episode
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int>(neuronCount), nlm::ConfigSource::Runtime);
        config->set("training.enabled", true, nlm::ConfigSource::Runtime);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        double episodeReward = 0.0;
        double episodeLoss = 0.0;
        
        // Run episode
        for (size_t step = 0; step < stepsPerEpisode; ++step) {
            auto simStep = nlm::SimulationStep{step, static_cast<nlm::Timestamp>(step * 0.1f)};
            brain->step(simStep);
            
            // Simulate reward and loss calculation
            double reward = 1.0 - (step * 0.01); // Decreasing reward over time
            double loss = std::exp(-static_cast<double>(brain->getAverageFiringRate()) / 100.0);
            
            episodeReward += reward;
            episodeLoss += loss;
            
            // Simulate training update (simplified)
            brain->setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
        }
        
        totalReward += episodeReward;
        totalLoss += episodeLoss;
        
        // Reset brain for next episode
        brain->reset();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    double avgReward = totalReward / episodes;
    double avgLoss = totalLoss / episodes;
    
    return TrainingBenchmarkResults(timeMs, episodes, avgReward, avgLoss,
                                   "Training Loop: " + std::to_string(neuronCount) + " neurons, " + std::to_string(episodes) + " episodes, " + std::to_string(stepsPerEpisode) + " steps/episode");
}

TrainingBenchmarkResults benchmarkCallbackSystem(size_t neuronCount, size_t episodes) {
    auto start = std::chrono::high_resolution_clock::now();
    
    double totalReward = 0.0;
    double totalLoss = 0.0;
    
    for (size_t episode = 0; episode < episodes; ++episode) {
        // Create training config
        auto trainingConfig = std::make_shared<nlm::TrainingConfig>();
        trainingConfig->enableValidation = true;
        trainingConfig->validationFrequency = 10.0f;
        trainingConfig->earlyStopping = false;
        
        // Create brain
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int>(neuronCount), nlm::ConfigSource::Runtime);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        double episodeReward = 0.0;
        double episodeLoss = 0.0;
        
        // Create training callback (simulated)
        auto callback = std::make_shared<nlm::SimpleTrainingCallback>();
        callback->onTrainingStart();
        
        // Run episode with callbacks
        for (size_t step = 0; step < 100; ++step) {
            auto simStep = nlm::SimulationStep{step, static_cast<nlm::Timestamp>(step * 0.1f)};
            brain->step(simStep);
            
            // Simulate callback invocations
            callback->afterStep(step, step, 1.0f, 0.5f);
            
            double reward = 1.0 - (step * 0.01);
            double loss = std::exp(-static_cast<double>(brain->getAverageFiringRate()) / 100.0);
            
            episodeReward += reward;
            episodeLoss += loss;
            
            if (step % 10 == 0) {
                callback->afterEpisode(step, episodeReward / (step + 1), true);
            }
        }
        
        callback->onTrainingEnd();
        
        totalReward += episodeReward;
        totalLoss += episodeLoss;
        
        brain->reset();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    double avgReward = totalReward / episodes;
    double avgLoss = totalLoss / episodes;
    
    return TrainingBenchmarkResults(timeMs, episodes, avgReward, avgLoss,
                                   "Callback System: " + std::to_string(neuronCount) + " neurons, " + std::to_string(episodes) + " episodes");
}

TrainingBenchmarkResults benchmarkMonitoringSystem(size_t neuronCount, size_t episodes) {
    auto start = std::chrono::high_resolution_clock::now();
    
    double totalReward = 0.0;
    double totalLoss = 0.0;
    
    for (size_t episode = 0; episode < episodes; ++episode) {
        // Create training config with monitoring
        auto trainingConfig = std::make_shared<nlm::TrainingConfig>();
        trainingConfig->enableValidation = true;
        trainingConfig->validationFrequency = 5.0f;
        trainingConfig->earlyStopping = false;
        
        // Create training monitor (simulated)
        nlm::TrainingMonitor monitor;
        
        // Create brain
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int>(neuronCount), nlm::ConfigSource::Runtime);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        double episodeReward = 0.0;
        double episodeLoss = 0.0;
        
        // Run episode with monitoring
        for (size_t step = 0; step < 200; ++step) {
            auto simStep = nlm::SimulationStep{step, static_cast<nlm::Timestamp>(step * 0.1f)};
            brain->step(simStep);
            
            // Record metrics
            double reward = 1.0 - (step * 0.01);
            double loss = std::exp(-static_cast<double>(brain->getAverageFiringRate()) / 100.0);
            
            monitor.recordEpisodePerformance(step, reward, loss, 1.0f);
            
            episodeReward += reward;
            episodeLoss += loss;
            
            if (step % 20 == 0) {
                monitor.recordEpisodePerformance(step, episodeReward / (step + 1), 
                                                episodeLoss / (step + 1), 1.0f);
            }
        }
        
        totalReward += episodeReward;
        totalLoss += episodeLoss;
        
        brain->reset();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    double avgReward = totalReward / episodes;
    double avgLoss = totalLoss / episodes;
    
    return TrainingBenchmarkResults(timeMs, episodes, avgReward, avgLoss,
                                   "Monitoring System: " + std::to_string(neuronCount) + " neurons, " + std::to_string(episodes) + " episodes");
}

TrainingBenchmarkResults benchmarkHyperparameterOptimization(size_t neuronCount, size_t paramSpaceSize, size_t iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    double totalScore = 0.0;
    double totalLoss = 0.0;
    
    for (size_t iteration = 0; iteration < iterations; ++iteration) {
        // Simulate hyperparameter optimization
        for (size_t i = 0; i < paramSpaceSize; ++i) {
            // Sample random hyperparameters
            float learningRate = static_cast<float>(std::rand()) / RAND_MAX * 0.1f + 0.001f;
            float discountFactor = static_cast<float>(std::rand()) / RAND_MAX * 0.2f + 0.8f;
            float complexity = static_cast<float>(std::rand()) / RAND_MAX;
            
            // Create brain with these hyperparameters
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int>(neuronCount), nlm::ConfigSource::Runtime);
            config->set("learning_rate", learningRate, nlm::ConfigSource::Runtime);
            config->set("discount_factor", discountFactor, nlm::ConfigSource::Runtime);
            config->set("brain.complexity", complexity, nlm::ConfigSource::Runtime);
            
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            
            // Run simulation
            double episodeScore = 0.0;
            double episodeLoss = 0.0;
            
            for (size_t step = 0; step < 50; ++step) {
                auto simStep = nlm::SimulationStep{step, static_cast<nlm::Timestamp>(step * 0.1f)};
                brain->step(simStep);
                
                double reward = static_cast<float>(std::rand()) / RAND_MAX;
                double loss = std::exp(-static_cast<double>(brain->getAverageFiringRate()) / 100.0);
                
                episodeScore += reward;
                episodeLoss += loss;
            }
            
            totalScore += episodeScore / 50.0;
            totalLoss += episodeLoss / 50.0;
            
            brain->reset();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double timeMs = duration.count() / 1000.0;
    double avgScore = totalScore / iterations;
    double avgLoss = totalLoss / iterations;
    
    return TrainingBenchmarkResults(timeMs, iterations, avgScore, avgLoss,
                                   "Hyperparameter Optimization: " + std::to_string(neuronCount) + " neurons, " + std::to_string(paramSpaceSize) + " params, " + std::to_string(iterations) + " iterations");
}

void runTrainingBenchmarks() {
    std::cout << "========================================" << std::endl;
    std::cout << "Training Performance Benchmarks" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    // Seed random number generator
    std::srand(42);
    
    // Benchmark training loop
    std::cout << "Training Loop Benchmarks:" << std::endl;
    std::cout << std::setw(80) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(12) << std::right << "Episodes"
              << std::setw(15) << std::right << "Avg Reward"
              << std::setw(15) << std::right << "Avg Loss" << std::endl;
    std::cout << std::string(127, '-') << std::endl;
    
    auto trainResults1 = benchmarkTrainingLoop(1000, 20, 100);
    trainResults1.print();
    
    auto trainResults2 = benchmarkTrainingLoop(5000, 10, 50);
    trainResults2.print();
    
    auto trainResults3 = benchmarkTrainingLoop(10000, 5, 20);
    trainResults3.print();
    
    std::cout << std::endl;
    
    // Benchmark callback system
    std::cout << "Callback System Benchmarks:" << std::endl;
    std::cout << std::setw(80) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(12) << std::right << "Episodes"
              << std::setw(15) << std::right << "Avg Reward"
              << std::setw(15) << std::right << "Avg Loss" << std::endl;
    std::cout << std::string(127, '-') << std::endl;
    
    auto callbackResults1 = benchmarkCallbackSystem(1000, 20);
    callbackResults1.print();
    
    auto callbackResults2 = benchmarkCallbackSystem(5000, 10);
    callbackResults2.print();
    
    std::cout << std::endl;
    
    // Benchmark monitoring system
    std::cout << "Monitoring System Benchmarks:" << std::endl;
    std::cout << std::setw(80) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(12) << std::right << "Episodes"
              << std::setw(15) << std::right << "Avg Reward"
              << std::setw(15) << std::right << "Avg Loss" << std::endl;
    std::cout << std::string(127, '-') << std::endl;
    
    auto monitorResults1 = benchmarkMonitoringSystem(1000, 20);
    monitorResults1.print();
    
    auto monitorResults2 = benchmarkMonitoringSystem(5000, 10);
    monitorResults2.print();
    
    std::cout << std::endl;
    
    // Benchmark hyperparameter optimization
    std::cout << "Hyperparameter Optimization Benchmarks:" << std::endl;
    std::cout << std::setw(80) << std::left << "Description"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(12) << std::right << "Iterations"
              << std::setw(15) << std::right << "Avg Score"
              << std::setw(15) << std::right << "Avg Loss" << std::endl;
    std::cout << std::string(127, '-') << std::endl;
    
    auto hyperResults1 = benchmarkHyperparameterOptimization(1000, 5, 10);
    hyperResults1.print();
    
    auto hyperResults2 = benchmarkHyperparameterOptimization(5000, 10, 5);
    hyperResults2.print();
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Training benchmark completed successfully!" << std::endl;
    std::cout << "========================================" << std::endl;
}

} // namespace benchmark_training

int main() {
    std::cout << "Running Training Performance Benchmarks..." << std::endl;
    benchmark_training::runTrainingBenchmarks();
    return 0;
}
