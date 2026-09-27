/**
 * Phase 5 Demo - Lifelong Learning Demonstration
 * 
 * This demo runs the Phase 5 lifetime experiment to demonstrate
 * the brain's ability to learn continuously over time.
 */

#include "experiments/Phase5Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 5 - Lifelong Learning Demo ===" << std::endl;
    std::cout << "Demonstrating continuous learning over multiple phases..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase5IntegratedExperiment experiment;
    
    // Parse command line arguments
    size_t numSeeds = 3;
    if (argc > 1) {
        numSeeds = std::stoi(argv[1]);
    }
    
    // Run multi-seed experiment for statistical significance
    std::cout << "--- Multi-Seed Experiment ---" << std::endl;
    std::cout << "Running with " << numSeeds << " different random seeds..." << std::endl;
    
    // Create base configuration
    LifetimeExperimentConfig baseConfig;
    
    // Create brain factory function
    auto createBrain = [](const LifetimeExperimentConfig& config, uint64_t seed) {
        auto brain = std::make_shared<Brain>();
        
        // Configure brain with seed
        auto brainConfig = std::make_shared<Config>();
        brainConfig->set("random_seed", static_cast<int64_t>(seed));
        brainConfig->set("neuron_count", config.initialNeurons);
        brainConfig->set("region_count", size_t(2));
        brainConfig->set("connection_probability", config.connectionProbability);
        
        brain->initialize(brainConfig);
        return brain;
    };
    
    // Create simulation function (simplified)
    auto runSimulation = [](Brain* brain, uint64_t steps) {
        // Run a simplified simulation
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        return true;
    };
    
    auto results = experiment.runWithMultipleSeeds(
        numSeeds, baseConfig, createBrain, runSimulation
    );
    
    std::cout << std::endl;
    
    if (results.empty()) {
        std::cerr << "ERROR: No experiments completed successfully!" << std::endl;
        return 1;
    }
    
    std::cout << "Experiments completed: " << results.size() << "/" << numSeeds << std::endl << std::endl;
    
    // Compute statistics
    auto stats = experiment.computeStatistics(results);
    
    // Display results
    std::cout << "=== LIFELONG LEARNING RESULTS ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Seeds run: " << results.size() << std::endl;
    std::cout << "Success rate: " << stats.successCount << "/" << (stats.successCount + stats.failureCount) << std::endl;
    std::cout << std::endl;
    
    std::cout << "Reward Statistics:" << std::endl;
    std::cout << "  Mean reward: " << std::fixed << std::setprecision(4) << stats.meanReward << std::endl;
    std::cout << "  Standard deviation: " << stats.stdDevReward << std::endl;
    std::cout << "  Min performance: " << stats.minPerformance << std::endl;
    std::cout << "  Max performance: " << stats.maxPerformance << std::endl;
    std::cout << "  Coefficient of variation: " << stats.coefficientOfVariation << std::endl;
    std::cout << std::endl;
    
    // Generate report
    std::cout << "=== DETAILED REPORT ===" << std::endl;
    std::string report = experiment.generateReport(results, stats);
    std::cout << report;
    
    // Performance assessment
    std::cout << "=== PERFORMANCE ASSESSMENT ===" << std::endl;
    std::cout << std::endl;
    
    if (stats.meanReward > 0.1f) {
        std::cout << "✓ GOOD: Brain is learning effectively with positive rewards" << std::endl;
    } else if (stats.meanReward > 0.0f) {
        std::cout << "⚠ MODERATE: Brain is learning but rewards are low" << std::endl;
    } else {
        std::cout << "✗ POOR: Brain is not learning effectively" << std::endl;
    }
    
    if (stats.coefficientOfVariation < 0.5f) {
        std::cout << "✓ GOOD: Learning is stable across different seeds" << std::endl;
    } else {
        std::cout << "⚠ VARIABLE: Learning performance varies significantly" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== LIFELONG LEARNING CAPABILITIES ===" << std::endl;
    std::cout << "Phase 5 demonstrates:" << std::endl;
    std::cout << "  - Large-scale neural computation" << std::endl;
    std::cout << "  - Event-driven spike propagation" << std::endl;
    std::cout << "  - Multithreading and SIMD support" << std::endl;
    std::cout << "  - Lifelong continuous adaptation" << std::endl;
    std::cout << "  - Memory consolidation mechanisms" << std::endl;
    std::cout << "  - Continual curiosity-driven exploration" << std::endl;
    std::cout << "  - Emergent specialization" << std::endl;
    
    return 0;
}

