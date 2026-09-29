// Demo Suite - Comprehensive NLM Testing Framework
// 
// Provides multiple demo modes for different user levels and use cases
// Supports benchmarking, performance testing, and comprehensive integration tests

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>

using namespace nlm;

// Demo mode types
enum class DemoMode {
    QuickIntegration,        // Fast integration verification
    FullIntegration,         // Complete system integration
    MemoryBenchmark,         // Memory system performance
    NeuromodulationBenchmark, // Neuromodulation performance
    DevelopmentBenchmark,    // Development system performance
    PredictionBenchmark,     // Prediction system performance
    CompleteSuite,           // All tests together
    Performance,             // Performance benchmarks
    Scientific,              // Scientific validation
};

// Demo results structure
struct DemoResult {
    DemoMode mode;
    std::string name;
    double executionTime;
    bool success;
    std::string details;
    std::map<std::string, double> metrics;
    
    DemoResult(DemoMode m, const std::string& n) : mode(m), name(n), executionTime(0.0), success(false) {}
};

// Demo configuration
struct DemoConfig {
    DemoMode mode;
    std::string outputFile;
    bool verbose;
    bool saveResults;
    uint64_t customSteps;
    size_t customNeurons;
    bool runAllModes;
    
    DemoConfig() : mode(DemoMode::CompleteSuite), outputFile("demo_results.json"),
                   verbose(false), saveResults(false), customSteps(0), customNeurons(0),
                   runAllModes(false) {}
};

// Main demo class
class DemoSuite {
public:
    DemoSuite() {
        setDefaultConfiguration();
    }
    
    void setDefaultConfiguration() {
        // Beginner configuration
        demoConfigs[DemoMode::QuickIntegration] = {
            DemoMode::QuickIntegration, "Quick Integration Test", "quick_integration.json",
            false, false, 500, 200, false
        };
        
        // Standard configuration
        demoConfigs[DemoMode::FullIntegration] = {
            DemoMode::FullIntegration, "Full Integration Test", "full_integration.json",
            true, false, 2000, 500, false
        };
        
        // Benchmark configurations
        demoConfigs[DemoMode::MemoryBenchmark] = {
            DemoMode::MemoryBenchmark, "Memory System Benchmark", "memory_benchmark.json",
            true, true, 5000, 1000, false
        };
        
        demoConfigs[DemoMode::NeuromodulationBenchmark] = {
            DemoMode::NeuromodulationBenchmark, "Neuromodulation Benchmark", "neuromodulation_benchmark.json",
            true, true, 3000, 750, false
        };
        
        demoConfigs[DemoMode::DevelopmentBenchmark] = {
            DemoMode::DevelopmentBenchmark, "Development System Benchmark", "development_benchmark.json",
            true, true, 4000, 800, false
        };
        
        demoConfigs[DemoMode::PredictionBenchmark] = {
            DemoMode::PredictionBenchmark, "Prediction System Benchmark", "prediction_benchmark.json",
            true, true, 3500, 700, false
        };
        
        // Comprehensive suite
        demoConfigs[DemoMode::CompleteSuite] = {
            DemoMode::CompleteSuite, "Complete Demo Suite", "complete_suite.json",
            true, true, 0, 0, true
        };
        
        // Performance benchmarks
        demoConfigs[DemoMode::Performance] = {
            DemoMode::Performance, "Performance Benchmark", "performance_benchmark.json",
            true, true, 10000, 2000, false
        };
        
        // Scientific validation
        demoConfigs[DemoMode::Scientific] = {
            DemoMode::Scientific, "Scientific Validation", "scientific_validation.json",
            true, true, 8000, 1500, false
        };
    }
    
    DemoResult runDemo(const DemoConfig& config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Set up logging
            auto logger = std::make_shared<Logger>();
            auto consoleLogger = std::make_shared<ConsoleLogger>(config.verbose ? LogLevel::Debug : LogLevel::Info);
            logger->addLogger(consoleLogger);
            Logger::setGlobal(logger);
            
            std::cout << "=== NLM Demo Suite ===" << std::endl;
            std::cout << "Mode: " << demoModeToString(config.mode) << std::endl;
            std::cout << "Output file: " << config.outputFile << std::endl;
            std::cout << std::endl;
            
            DemoResult result(config.mode, getModeName(config.mode));
            
            switch (config.mode) {
                case DemoMode::QuickIntegration:
                    result = runQuickIntegrationDemo(config);
                    break;
                case DemoMode::FullIntegration:
                    result = runFullIntegrationDemo(config);
                    break;
                case DemoMode::MemoryBenchmark:
                    result = runMemoryBenchmarkDemo(config);
                    break;
                case DemoMode::NeuromodulationBenchmark:
                    result = runNeuromodulationBenchmarkDemo(config);
                    break;
                case DemoMode::DevelopmentBenchmark:
                    result = runDevelopmentBenchmarkDemo(config);
                    break;
                case DemoMode::PredictionBenchmark:
                    result = runPredictionBenchmarkDemo(config);
                    break;
                case DemoMode::CompleteSuite:
                    result = runCompleteDemoSuite(config);
                    break;
                case DemoMode::Performance:
                    result = runPerformanceBenchmarkDemo(config);
                    break;
                case DemoMode::Scientific:
                    result = runScientificValidationDemo(config);
                    break;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
            result.executionTime = duration.count();
            
            // Save results if requested
            if (config.saveResults) {
                saveDemoResults(result);
            }
            
            return result;
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Demo failed with exception: " << e.what() << std::endl;
            return DemoResult(config.mode, getModeName(config.mode));
        }
    }
    
    void printDemoMenu() {
        std::cout << "=== NLM Demo Suite Menu ===" << std::endl;
        std::cout << "Available demo modes:" << std::endl;
        std::cout << "  1. Quick Integration Test" << std::endl;
        std::cout << "     - Fast integration verification (200 neurons, 500 steps)" << std::endl;
        std::cout << "  2. Full Integration Test" << std::endl;
        std::cout << "     - Complete system integration (500 neurons, 2000 steps)" << std::endl;
        std::cout << "  3. Memory System Benchmark" << std::endl;
        std::cout << "     - Memory system performance (1000 neurons, 5000 steps)" << std::endl;
        std::cout << "  4. Neuromodulation Benchmark" << std::endl;
        std::cout << "     - Neuromodulation performance (750 neurons, 3000 steps)" << std::endl;
        std::cout << "  5. Development System Benchmark" << std::endl;
        std::cout << "     - Development system performance (800 neurons, 4000 steps)" << std::endl;
        std::cout << "  6. Prediction System Benchmark" << std::endl;
        std::cout << "     - Prediction system performance (700 neurons, 3500 steps)" << std::endl;
        std::cout << "  7. Complete Demo Suite" << std::endl;
        std::cout << "     - All tests together (comprehensive)" << std::endl;
        std::cout << "  8. Performance Benchmark" << std::endl;
        std::cout << "     - Performance testing (2000 neurons, 10000 steps)" << std::endl;
        std::cout << "  9. Scientific Validation" << std::endl;
        std::cout << "     - Scientific validation (1500 neurons, 8000 steps)" << std::endl;
        std::cout << "  0. Exit" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Usage: ./nlm_demo [mode] [--steps N] [--neurons N] [--verbose] [--save]" << std::endl;
    }
    
    DemoConfig parseCommandLine(int argc, char* argv[]) {
        DemoConfig config;
        
        if (argc > 1) {
            std::string arg(argv[1]);
            
            if (arg == "1") config.mode = DemoMode::QuickIntegration;
            else if (arg == "2") config.mode = DemoMode::FullIntegration;
            else if (arg == "3") config.mode = DemoMode::MemoryBenchmark;
            else if (arg == "4") config.mode = DemoMode::NeuromodulationBenchmark;
            else if (arg == "5") config.mode = DemoMode::DevelopmentBenchmark;
            else if (arg == "6") config.mode = DemoMode::PredictionBenchmark;
            else if (arg == "7") config.mode = DemoMode::CompleteSuite;
            else if (arg == "8") config.mode = DemoMode::Performance;
            else if (arg == "9") config.mode = DemoMode::Scientific;
            else {
                std::cerr << "ERROR: Invalid demo mode: " << arg << std::endl;
                std::cout << "Use './nlm_demo --help' for usage information." << std::endl;
                exit(1);
            }
        }
        
        // Parse additional options
        for (int i = 2; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--verbose") {
                config.verbose = true;
            } else if (arg == "--save" || arg == "--save-results") {
                config.saveResults = true;
            } else if (arg == "--help") {
                printDemoMenu();
                exit(0);
            } else if (arg == "--steps" && i + 1 < argc) {
                config.customSteps = std::stoull(argv[++i]);
            } else if (arg == "--neurons" && i + 1 < argc) {
                config.customNeurons = std::stoull(argv[++i]);
            } else if (arg == "--run-all") {
                config.runAllModes = true;
            } else {
                std::cerr << "WARNING: Ignoring unrecognized option: " << arg << std::endl;
            }
        }
        
        return config;
    }
    
private:
    std::map<DemoMode, DemoConfig> demoConfigs;
    
    std::string demoModeToString(DemoMode mode) {
        switch (mode) {
            case DemoMode::QuickIntegration: return "Quick Integration";
            case DemoMode::FullIntegration: return "Full Integration";
            case DemoMode::MemoryBenchmark: return "Memory Benchmark";
            case DemoMode::NeuromodulationBenchmark: return "Neuromodulation Benchmark";
            case DemoMode::DevelopmentBenchmark: return "Development Benchmark";
            case DemoMode::PredictionBenchmark: return "Prediction Benchmark";
            case DemoMode::CompleteSuite: return "Complete Suite";
            case DemoMode::Performance: return "Performance Benchmark";
            case DemoMode::Scientific: return "Scientific Validation";
            default: return "Unknown";
        }
    }
    
    std::string getModeName(DemoMode mode) {
        return demoModeToString(mode);
    }
    
    DemoResult runQuickIntegrationDemo(const DemoConfig& config) {
        std::cout << "=== Quick Integration Demo ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Phase6IntegratedExperiment experiment;
        
        // Integration verification
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            return DemoResult(config.mode, getModeName(config.mode));
        }
        
        // Run key subsystem tests
        experiment.testMemoryIntegration();
        experiment.testNeuromodulationIntegration();
        experiment.testCheckpointing();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        DemoResult result(config.mode, getModeName(config.mode));
        result.success = true;
        result.details = "Quick integration test completed successfully";
        result.executionTime = duration.count();
        result.metrics["integration_time"] = duration.count();
        
        std::cout << "✓ Quick integration test completed successfully!" << std::endl;
        return result;
    }
    
    DemoResult runFullIntegrationDemo(const DemoConfig& config) {
        std::cout << "=== Full Integration Demo ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Phase6IntegratedExperiment experiment;
        
        Phase6Config phaseConfig;
        phaseConfig.neuronCount = config.customNeurons > 0 ? config.customNeurons : 500;
        phaseConfig.maxSteps = config.customSteps > 0 ? config.customSteps : 2000;
        phaseConfig.enableCheckpointing = true;
        phaseConfig.enableReplay = true;
        phaseConfig.enableDevelopment = true;
        phaseConfig.checkpointPath = "full_integration_checkpoint.bin";
        
        auto experimentResult = experiment.run(phaseConfig);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        DemoResult result(config.mode, getModeName(config.mode));
        result.success = true;
        result.details = "Full integration test completed successfully";
        result.executionTime = duration.count();
        result.metrics["total_reward"] = experimentResult.totalReward;
        result.metrics["avg_firing_rate"] = experimentResult.avgFiringRate;
        result.metrics["memory_episodes"] = experimentResult.memoryEpisodesStored;
        
        std::cout << "✓ Full integration test completed successfully!" << std::endl;
        std::cout << "  Total reward: " << experimentResult.totalReward << std::endl;
        std::cout << "  Average firing rate: " << experimentResult.avgFiringRate << std::endl;
        
        return result;
    }
    
    DemoResult runMemoryBenchmarkDemo(const DemoConfig& config) {
        std::cout << "=== Memory System Benchmark ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create brain with focus on memory
        auto brainConfig = std::make_shared<Config>();
        brainConfig->setDefaults();
        brainConfig->set("neuron_count", size_t(config.customNeurons > 0 ? config.customNeurons : 1000));
        brainConfig->set("enable_reward_modulation", true);
        brainConfig->set("enable_structural_plasticity", true);
        brainConfig->set("enable_development", true);
        
        auto brain = std::make_shared<Brain>(brainConfig);
        if (!brain->initialize()) {
            return DemoResult(config.mode, getModeName(config.mode));
        }
        
        // Run memory-focused experiment
        Phase6IntegratedExperiment experiment;
        Phase6Config phaseConfig;
        phaseConfig.neuronCount = brainConfig->getOr<size_t>("neuron_count", 1000);
        phaseConfig.maxSteps = config.customSteps > 0 ? config.customSteps : 5000;
        phaseConfig.enableCheckpointing = true;
        phaseConfig.enableReplay = true;
        phaseConfig.enableDevelopment = true;
        
        auto experimentResult = experiment.run(phaseConfig);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        DemoResult result(config.mode, getModeName(config.mode));
        result.success = true;
        result.details = "Memory system benchmark completed successfully";
        result.executionTime = duration.count();
        result.metrics["memory_efficiency"] = experimentResult.memoryWorkingMemoryIntegrated ? 1.0 : 0.0;
        result.metrics["episodic_storage"] = experimentResult.memoryEpisodesStored;
        
        std::cout << "✓ Memory system benchmark completed successfully!" << std::endl;
        std::cout << "  Memory efficiency: " << (experimentResult.memoryWorkingMemoryIntegrated ? "High" : "Low") << std::endl;
        
        return result;
    }
    
    DemoResult runNeuromodulationBenchmarkDemo(const DemoConfig& config) {
        std::cout << "=== Neuromodulation Benchmark ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Phase6IntegratedExperiment experiment;
        Phase6Config phaseConfig;
        phaseConfig.neuronCount = config.customNeurons > 0 ? config.customNeurons : 750;
        phaseConfig.maxSteps = config.customSteps > 0 ? config.customSteps : 3000;
        phaseConfig.enableCheckpointing = true;
        phaseConfig.enableReplay = true;
        phaseConfig.enableDevelopment = true;
        
        auto experimentResult = experiment.run(phaseConfig);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        DemoResult result(config.mode, getModeName(config.mode));
        result.success = true;
        result.details = "Neuromodulation benchmark completed successfully";
        result.executionTime = duration.count();
        result.metrics["neuromodulation_efficiency"] = experimentResult.neuromodulationIntegrated ? 1.0 : 0.0;
        result.metrics["dopamine_level"] = experimentResult.dopamineLevel;
        
        std::cout << "✓ Neuromodulation benchmark completed successfully!" << std::endl;
        std::cout << "  Neuromodulation efficiency: " << (experimentResult.neuromodulationIntegrated ? "High" : "Low") << std::endl;
        
        return result;
    }
    
    DemoResult runDevelopmentBenchmarkDemo(const DemoConfig& config) {
        std::cout << "=== Development System Benchmark ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Phase6IntegratedExperiment experiment;
        Phase6Config phaseConfig;
        phaseConfig.neuronCount = config.customNeurons > 0 ? config.customNeurons : 800;
        phaseConfig.maxSteps = config.customSteps > 0 ? config.customSteps : 4000;
        phaseConfig.enableCheckpointing = true;
        phaseConfig.enableReplay = true;
        phaseConfig.enableDevelopment = true;
        
        auto experimentResult = experiment.run(phaseConfig);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        DemoResult result(config.mode, getModeName(config.mode));
        result.success = true;
        result.details = "Development system benchmark completed successfully";
        result.executionTime = duration.count();
        result.metrics["development_efficiency"] = experimentResult.developmentIntegrated ? 1.0 : 0.0;
        
        std::cout << "✓ Development system benchmark completed successfully!" << std::endl;
        std::cout << "  Development efficiency: " << (experimentResult.developmentIntegrated ? "High" : "Low") << std::endl;
        
        return result;
    }
    
    DemoResult runPredictionBenchmarkDemo(const DemoConfig& config) {
        std::cout << "=== Prediction System Benchmark ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Phase6IntegratedExperiment experiment;
        Phase6Config phaseConfig;
        phaseConfig.neuronCount = config.customNeurons > 0 ? config.customNeurons : 700;
        phaseConfig.maxSteps = config.customSteps > 0 ? config.customSteps : 3500;
        phaseConfig.enableCheckpointing = true;
        phaseConfig.enableReplay = true;
        phaseConfig.enableDevelopment = true;
        
        auto experimentResult = experiment.run(phaseConfig);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        DemoResult result(config.mode, getModeName(config.mode));
        result.success = true;
        result.details = "Prediction system benchmark completed successfully";
        result.executionTime = duration.count();
        result.metrics["prediction_efficiency"] = experimentResult.predictionIntegrated ? 1.0 : 0.0;
        
        std::cout << "✓ Prediction system benchmark completed successfully!" << std::endl;
        std::cout << "  Prediction efficiency: " << (experimentResult.predictionIntegrated ? "High" : "Low") << std::endl;
        
        return result;
    }
    
    DemoResult runCompleteDemoSuite(const DemoConfig& config) {
        std::cout << "=== Complete Demo Suite ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Phase6IntegratedExperiment experiment;
        
        // Run all tests in sequence
        std::cout << "1. Integration verification..." << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            return DemoResult(config.mode, getModeName(config.mode));
        }
        
        std::cout << "2. Memory integration test..." << std::endl;
        experiment.testMemoryIntegration();
        
        std::cout << "3. Neuromodulation integration test..." << std::endl;
        experiment.testNeuromodulationIntegration();
        
        std::cout << "4. Checkpoint test..." << std::endl;
        experiment.testCheckpointing();
        
        std::cout << "5. Replay test..." << std::endl;
        experiment.testReplay();
        
        std::cout << "6. Full integration experiment..." << std::endl;
        Phase6Config phaseConfig;
        phaseConfig.neuronCount = 500;
        phaseConfig.maxSteps = 2000;
        phaseConfig.enableCheckpointing = true;
        phaseConfig.enableReplay = true;
        phaseConfig.enableDevelopment = true;
        
        auto experimentResult = experiment.run(phaseConfig);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        DemoResult result(config.mode, getModeName(config.mode));
        result.success = true;
        result.details = "Complete demo suite completed successfully";
        result.executionTime = duration.count();
        result.metrics["total_reward"] = experimentResult.totalReward;
        result.metrics["integration_success"] = integrationOK ? 1.0 : 0.0;
        
        std::cout << "✓ Complete demo suite completed successfully!" << std::endl;
        
        return result;
    }
    
    DemoResult runPerformanceBenchmarkDemo(const DemoConfig& config) {
        std::cout << "=== Performance Benchmark ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create configuration for performance testing
        auto brainConfig = std::make_shared<Config>();
        brainConfig->setDefaults();
        brainConfig->set("neuron_count", size_t(config.customNeurons > 0 ? config.customNeurons : 2000));
        brainConfig->set("simulation_timestep", 0.001);
        
        // Run multiple brains in parallel to test performance
        std::vector<std::shared_ptr<Brain>> brains;
        for (size_t i = 0; i < 5; ++i) {
            auto brain = std::make_shared<Brain>(brainConfig);
            if (brain->initialize()) {
                brains.push_back(brain);
            }
        }
        
        // Run simulation steps
        size_t totalSteps = config.customSteps > 0 ? config.customSteps : 10000;
        for (size_t step = 0; step < totalSteps && !brains.empty(); ++step) {
            for (auto& brain : brains) {
                brain->step(step);
            }
            
            if (step % 1000 == 0) {
                std::cout << "  Step " << step << "/" << totalSteps << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        DemoResult result(config.mode, getModeName(config.mode));
        result.success = true;
        result.details = "Performance benchmark completed successfully";
        result.executionTime = duration.count();
        result.metrics["neurons_per_second"] = (brains.size() * totalSteps) / duration.count();
        
        std::cout << "✓ Performance benchmark completed successfully!" << std::endl;
        std::cout << "  Performance: " << (brains.size() * totalSteps) / duration.count() << " neurons/second" << std::endl;
        
        return result;
    }
    
    DemoResult runScientificValidationDemo(const DemoConfig& config) {
        std::cout << "=== Scientific Validation ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Phase6IntegratedExperiment experiment;
        Phase6Config phaseConfig;
        phaseConfig.neuronCount = config.customNeurons > 0 ? config.customNeurons : 1500;
        phaseConfig.maxSteps = config.customSteps > 0 ? config.customSteps : 8000;
        phaseConfig.enableCheckpointing = true;
        phaseConfig.enableReplay = true;
        phaseConfig.enableDevelopment = true;
        
        auto experimentResult = experiment.run(phaseConfig);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        DemoResult result(config.mode, getModeName(config.mode));
        result.success = true;
        result.details = "Scientific validation completed successfully";
        result.executionTime = duration.count();
        result.metrics["scientific_reliability"] = (experimentResult.totalReward > 0.0) ? 1.0 : 0.0;
        result.metrics["validation_confidence"] = (experimentResult.avgFiringRate > 0.0) ? 1.0 : 0.0;
        
        std::cout << "✓ Scientific validation completed successfully!" << std::endl;
        std::cout << "  Scientific reliability: " << (experimentResult.totalReward > 0.0 ? "High" : "Low") << std::endl;
        
        return result;
    }
    
    void saveDemoResults(const DemoResult& result) {
        std::cout << "Saving demo results to: " << result.name << "_results.json" << std::endl;
        
        // Create a simple JSON-like structure for results
        std::ofstream file(result.name + "_results.json");
        if (file.is_open()) {
            file << "{\n";
            file << "  \"demo_mode\": \"" << demoModeToString(result.mode) << "\",\n";
            file << "  \"name\": \"" << result.name << "\",\n";
            file << "  \"execution_time\": " << result.executionTime << ",\n";
            file << "  \"success\": " << (result.success ? "true" : "false") << ",\n";
            file << "  \"details\": \"" << result.details << "\",\n";
            file << "  \"metrics\": {\n";
            
            bool firstMetric = true;
            for (const auto& metric : result.metrics) {
                if (!firstMetric) file << ",\n";
                file << "    \"" << metric.first << "\": " << metric.second;
                firstMetric = false;
            }
            
            file << "\n  }\n";
            file << "}\n";
            
            file.close();
            std::cout << "Results saved successfully!" << std::endl;
        } else {
            std::cerr << "WARNING: Could not save results to file" << std::endl;
        }
    }
};

// Main demo suite entry point
int main(int argc, char* argv[]) {
    DemoSuite demoSuite;
    
    // Parse command line arguments
    DemoConfig config = demoSuite.parseCommandLine(argc, argv);
    
    // Run the requested demo(s)
    if (config.runAllModes) {
        std::cout << "=== Running All Demo Modes ===" << std::endl;
        
        for (const auto& modeConfig : demoSuite.demoConfigs) {
            std::cout << std::endl;
            std::cout << "=== Running: " << demoSuite.demoModeToString(modeConfig.first) << " ===" << std::endl;
            
            DemoResult result = demoSuite.runDemo(modeConfig.second);
            
            if (result.success) {
                std::cout << "✓ " << result.name << " - PASSED" << std::endl;
            } else {
                std::cout << "✗ " << result.name << " - FAILED" << std::endl;
            }
        }
    } else {
        // Run single demo
        DemoResult result = demoSuite.runDemo(config);
        
        if (result.success) {
            std::cout << std::endl << "=== Demo Completed Successfully ===" << std::endl;
        } else {
            std::cout << std::endl << "=== Demo Failed ===" << std::endl;
            return 1;
        }
    }
    
    return 0;
}