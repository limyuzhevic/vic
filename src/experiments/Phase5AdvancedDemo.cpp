/**
 * Phase 5 Advanced Demo - Advanced Optimized Brain Features
 * 
 * This demo demonstrates the advanced features of Phase 5:
 * - High-performance neural simulation with event-driven processing
 * - SIMD vectorization and multithreading
 * - Sparse connectivity representation
 * - Advanced configuration and profiling
 * - Adaptive algorithms (homeostatic plasticity, neural pruning)
 * - Performance monitoring and analytics
 * - Configuration export/import
 */

#include "performance/OptimizedBrain.hpp"
#include "performance/AdvancedOptimizedBrain.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <chrono>
#include <fstream>

using namespace nlm;

void demoPerformanceOptimizations() {
    std::cout << "=== Performance Optimizations Demo ===" << std::endl << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.05f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Configure for maximum performance
    config->set("enable_multithreading", true, ConfigSource::Default);
    config->set("enable_simd", true, ConfigSource::Default);
    config->set("enable_event_driven", true, ConfigSource::Default);
    config->set("enable_sparse_connectivity", true, ConfigSource::Default);
    
    // Create advanced optimized brain
    auto brain = createAdvancedOptimizedBrain(config);
    if (!brain) {
        std::cerr << "ERROR: Failed to create AdvancedOptimizedBrain" << std::endl;
        return;
    }
    
    std::cout << "Created AdvancedOptimizedBrain with:" << std::endl;
    std::cout << "  - Neuron count: 5000" << std::endl;
    std::cout << "  - Multithreading: enabled" << std::endl;
    std::cout << "  - SIMD vectorization: enabled" << std::endl;
    std::cout << "  - Event-driven processing: enabled" << std::endl;
    std::cout << "  - Sparse connectivity: enabled" << std::endl;
    std::cout << std::endl;
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    std::cout << "Running 1000 simulation steps..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (uint64_t step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            std::cout << "  Step " << step << ", "
                     << brain->getTotalSpikeCount() << " spikes, "
                     << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << std::endl;
    std::cout << "Performance Summary:" << std::endl;
    std::cout << "  - Total steps: 1000" << std::endl;
    std::cout << "  - Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  - Elapsed time: " << elapsed << "s" << std::endl;
    std::cout << "  - Steps per second: " << (1000.0 / elapsed) << std::endl;
    std::cout << "  - Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << std::endl;
}

void demoPerformanceMonitoring() {
    std::cout << "=== Performance Monitoring Demo ===" << std::endl << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Enable profiling
    config->set("advanced_enable_profiling", true, ConfigSource::Default);
    config->set("advanced_profiling_sample_rate", 100, ConfigSource::Default);
    config->set("advanced_output_directory", "./performance_demo", ConfigSource::Default);
    
    // Create advanced optimized brain
    auto brain = createAdvancedOptimizedBrain(config);
    if (!brain) {
        std::cerr << "ERROR: Failed to create AdvancedOptimizedBrain" << std::endl;
        return;
    }
    
    // Start profiling
    std::cout << "Starting performance monitoring..." << std::endl;
    std::cout << "(Profiling every 100 steps)" << std::endl << std::endl;
    
    brain->startProfiling();
    
    // Run simulation with profiling
    for (uint64_t step = 0; step < 500; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ", "
                     << brain->getTotalSpikeCount() << " spikes, "
                     << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    // Stop profiling and generate report
    brain->stopProfiling();
    
    std::cout << std::endl;
    std::cout << "=== Performance Report ===" << std::endl;
    std::cout << brain->generatePerformanceReport() << std::endl;
    
    // Export configuration
    std::cout << "Exporting configuration..." << std::endl;
    brain->exportConfiguration("config_backup.cfg");
    std::cout << "Configuration exported to: config_backup.cfg" << std::endl;
    std::cout << std::endl;
}

void demoAdvancedFeatures() {
    std::cout << "=== Advanced Features Demo ===" << std::endl << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(3000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    // Configure for advanced features
    config->set("advanced_enable_debug_mode", true, ConfigSource::Default);
    config->set("advanced_enable_adaptive_timestep", true, ConfigSource::Default);
    config->set("advanced_enable_homeostatic_plasticity", true, ConfigSource::Default);
    config->set("advanced_homeostatic_target_rate", 0.02f, ConfigSource::Default);
    config->set("advanced_enable_neural_pruning", true, ConfigSource::Default);
    config->set("advanced_pruning_threshold", 0.01f, ConfigSource::Default);
    
    // Create advanced optimized brain
    auto brain = createAdvancedOptimizedBrain(config);
    if (!brain) {
        std::cerr << "ERROR: Failed to create AdvancedOptimizedBrain" << std::endl;
        return;
    }
    
    std::cout << "Created AdvancedOptimizedBrain with advanced features:" << std::endl;
    std::cout << "  - Debug mode: enabled" << std::endl;
    std::cout << "  - Adaptive timestep: enabled" << std::endl;
    std::cout << "  - Homeostatic plasticity: enabled" << std::endl;
    std::cout << "  - Neural pruning: enabled" << std::endl;
    std::cout << std::endl;
    
    // Initialize and run simulation
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized with advanced features!" << std::endl;
    std::cout << "Running 800 simulation steps..." << std::endl;
    
    for (uint64_t step = 0; step < 800; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            std::cout << "  Step " << step << ", "
                     << brain->getTotalSpikeCount() << " spikes, "
                     << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    std::cout << std::endl;
    
    // Generate analysis reports
    std::cout << "=== Neural Dynamics Analysis ===" << std::endl;
    std::cout << brain->analyzeNeuralDynamics() << std::endl;
    
    std::cout << "=== Plasticity Effects Analysis ===" << std::endl;
    std::cout << brain->analyzePlasticityEffects() << std::endl;
    
    // Import configuration
    std::cout << "Importing configuration from file..." << std::endl;
    brain->importConfiguration("config_backup.cfg");
    std::cout << "Configuration imported successfully!" << std::endl;
    std::cout << std::endl;
}

void demoLifetimeExperiment() {
    std::cout << "=== Lifetime Experiment Demo ===" << std::endl << std::endl;
    
    // Create lifetime experiment configuration
    LifetimeExperimentConfig config;
    config.initialNeurons = 1000;
    config.maxNeurons = 5000;
    config.connectionProbability = 0.05f;
    
    config.synapseFormationSteps = 5000;
    config.sensoryDevelopmentSteps = 20000;
    config.motorDevelopmentSteps = 10000;
    config.associativeDevelopmentSteps = 20000;
    config.memoryConsolidationSteps = 20000;
    config.socialDevelopmentSteps = 15000;
    
    // Configure for performance
    config.enableMultithreading = true;
    config.enableSIMD = true;
    config.enableSparseConnectivity = true;
    config.enableEventDriven = true;
    config.threadCount = 0;  // auto
    
    config.saveCheckpoints = true;
    config.generateVisualizations = true;
    
    // Create brain factory function for experiment
    auto createBrain = [config](const LifetimeExperimentConfig& expConfig) -> std::shared_ptr<Brain> {
        // Use experiment configuration for brain creation
        auto brainConfig = std::make_shared<Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(expConfig.initialNeurons), ConfigSource::Default);
        brainConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        brainConfig->set("connection_probability", expConfig.connectionProbability, ConfigSource::Default);
        
        // Use advanced brain for lifetime experiment
        return createAdvancedOptimizedBrain(brainConfig);
    };
    
    // Create and run simulation function
    auto runSimulation = [](Brain* brain, uint64_t steps) -> bool {
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            if (step % 5000 == 0 && step > 0) {
                std::cout << "  Step " << step << ", "
                         << brain->getTotalSpikeCount() << " spikes, "
                         << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        return true;
    };
    
    // Create experiment
    Phase5IntegratedExperiment experiment;
    
    std::cout << "Running lifetime experiment with advanced brain..." << std::endl;
    std::cout << "Initial neurons: " << config.initialNeurons << std::endl;
    std::cout << "Max neurons: " << config.maxNeurons << std::endl;
    std::cout << "Duration: " << (config.synapseFormationSteps + config.sensoryDevelopmentSteps + 
                             config.motorDevelopmentSteps + config.associativeDevelopmentSteps +
                             config.memoryConsolidationSteps + config.socialDevelopmentSteps) << " steps" << std::endl;
    std::cout << std::endl;
    
    auto result = experiment.run(config, createBrain, runSimulation);
    
    std::cout << "=== Experiment Results ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg reward per step: " << result.avgRewardPerStep << std::endl;
    std::cout << "Learning efficiency: " << result.overallLearningEfficiency << std::endl;
    std::cout << "Final performance: " << result.finalPerformance << std::endl;
    std::cout << "Memory capacity: " << result.memoryCapacity << std::endl;
    std::cout << "Prediction accuracy: " << result.predictionAccuracy << std::endl;
    std::cout << "Generalization ability: " << result.generalizationAbility << std::endl;
    std::cout << std::endl;
    
    std::cout << "Phases completed:" << std::endl;
    for (const auto& phase : result.phaseResults) {
        std::cout << "  " << phase.phaseName << ": "
                 << phase.startStep << " - " << phase.endStep << " ("
                 << phase.duration << "s, " << phase.totalReward << " reward)" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 5 Advanced Features Demo ===" << std::endl;
    std::cout << "Demonstrating advanced OptimizedBrain capabilities..." << std::endl;
    std::cout << std::endl;
    
    std::cout << "Choose demo mode:" << std::endl;
    std::cout << "  1 - Performance Optimizations" << std::endl;
    std::cout << "  2 - Performance Monitoring" << std::endl;
    std::cout << "  3 - Advanced Features" << std::endl;
    std::cout << "  4 - Lifetime Experiment" << std::endl;
    std::cout << std::endl;
    
    int choice;
    std::cout << "Enter choice (1-4): ";
    std::cin >> choice;
    
    switch (choice) {
        case 1:
            demoPerformanceOptimizations();
            break;
        case 2:
            demoPerformanceMonitoring();
            break;
        case 3:
            demoAdvancedFeatures();
            break;
        case 4:
            demoLifetimeExperiment();
            break;
        default:
            std::cout << "Invalid choice! Running default performance optimizations demo." << std::endl;
            demoPerformanceOptimizations();
            break;
    }
    
    std::cout << std::endl;
    std::cout << "=== Demo Complete ===" << std::endl;
    std::cout << "The AdvancedOptimizedBrain has demonstrated:" << std::endl;
    std::cout << "  - High-performance neural simulation" << std::endl;
    std::cout << "  - SIMD vectorization and multithreading" << std::endl;
    std::cout << "  - Sparse connectivity optimization" << std::endl;
    std::cout << "  - Comprehensive performance monitoring" << std::endl;
    std::cout << "  - Advanced configuration management" << std::endl;
    std::cout << "  - Adaptive plasticity algorithms" << std::endl;
    std::cout << "  - Neural dynamics analysis" << std::endl;
    std::cout << std::endl;
    
    return 0;
}

} // namespace nlm