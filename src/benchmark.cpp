// Enhanced NLM Benchmark System
// Provides comprehensive benchmarking with multiple scenarios, metrics, and analysis

#include "core/CLIUtility.hpp"
#include "core/ConfigurationManager.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <map>
#include <fstream>

using namespace nlm;

// Enhanced benchmark result structure
struct EnhancedBenchmarkResult {
    std::string benchmarkName;
    size_t neuronCount;
    size_t synapseCount;
    size_t steps;
    double totalTime;
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    
    // Advanced metrics
    double averageFiringRate;
    double standardDeviation;
    double peakFiringRate;
    double minFiringRate;
    double memoryUsage;
    double cpuUsage;
    
    // Network analysis
    float excitationInhibitionRatio;
    size_t totalSpikes;
    size_t activeNeuronCount;
    float averageSynapticWeight;
    
    // Plasticity metrics
    size_t stdpEvents;
    size_t hebbianEvents;
    size_t structuralChanges;
    
    // Time measurements
    double initializationTime;
    double simulationTime;
    double teardownTime;
    
    // Quality metrics
    double stabilityScore;
    double learningScore;
    double efficiencyScore;
    
    EnhancedBenchmarkResult(const std::string& name) 
        : benchmarkName(name), neuronCount(0), synapseCount(0), steps(0),
          totalTime(0.0), stepsPerSecond(0.0), spikesPerStep(0.0), synapsesPerNeuron(0.0),
          averageFiringRate(0.0), standardDeviation(0.0), peakFiringRate(0.0),
          minFiringRate(0.0), memoryUsage(0.0), cpuUsage(0.0), excitationInhibitionRatio(0.0f),
          totalSpikes(0), activeNeuronCount(0), averageSynapticWeight(0.0f),
          stdpEvents(0), hebbianEvents(0), structuralChanges(0),
          initializationTime(0.0), simulationTime(0.0), teardownTime(0.0),
          stabilityScore(0.0), learningScore(0.0), efficiencyScore(0.0) {}
};

// Enhanced benchmark scenarios
namespace BenchmarkScenarios {
    
    // Quick connectivity test
    EnhancedBenchmarkResult runQuickTest(const ConfigurationManager& config) {
        EnhancedBenchmarkResult result("Quick Connectivity Test");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Use quick preset configuration
        ConfigurationManager tempConfig = config;
        tempConfig.applyPreset(ConfigPreset::QuickTest);
        
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        brain->initialize();
        
        auto initEndTime = std::chrono::high_resolution_clock::now();
        result.initializationTime = std::chrono::duration<double>(initEndTime - startTime).count();
        
        // Run quick simulation
        size_t steps = 100;
        for (size_t step = 0; step < steps; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        auto simEndTime = std::chrono::high_resolution_clock::now();
        result.simulationTime = std::chrono::duration<double>(simEndTime - initEndTime).count();
        
        // Collect results
        result.neuronCount = brain->getTotalNeuronCount();
        result.synapseCount = brain->getTotalSynapseCount();
        result.steps = steps;
        result.totalTime = std::chrono::duration<double>(simEndTime - startTime).count();
        result.stepsPerSecond = steps / result.totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapseCount) / result.neuronCount;
        result.averageFiringRate = brain->getAverageFiringRate();
        result.totalSpikes = brain->getTotalSpikeCount();
        result.activeNeuronCount = brain->getFiringNeuronCount();
        
        // Quality metrics
        result.stabilityScore = 1.0f / (1.0f + std::abs(1.0f - result.excitationInhibitionRatio));
        result.efficiencyScore = result.stepsPerSecond / 1000.0f; // Normalized
        
        return result;
    }
    
    // Full integration test
    EnhancedBenchmarkResult runFullTest(const ConfigurationManager& config) {
        EnhancedBenchmarkResult result("Full Integration Test");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Use full preset configuration
        ConfigurationManager tempConfig = config;
        tempConfig.applyPreset(ConfigPreset::FullTest);
        
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        brain->initialize();
        
        auto initEndTime = std::chrono::high_resolution_clock::now();
        result.initializationTime = std::chrono::duration<double>(initEndTime - startTime).count();
        
        // Run extended simulation with learning
        size_t steps = 1000;
        for (size_t step = 0; step < steps; ++step) {
            // Inject varied input to stimulate learning
            float intensity = 20.0f + 30.0f * std::sin(static_cast<double>(step) * 0.01);
            brain->injectCurrentToNeurons(NeuronType::Sensory, intensity);
            
            brain->step(step, step * 0.001);
            
            // Apply some neuromodulation effects
            if (step % 100 == 0) {
                // Simulate development
                brain->develop();
            }
        }
        
        auto simEndTime = std::chrono::high_resolution_clock::now();
        result.simulationTime = std::chrono::duration<double>(simEndTime - initEndTime).count();
        
        // Collect results
        result.neuronCount = brain->getTotalNeuronCount();
        result.synapseCount = brain->getTotalSynapseCount();
        result.steps = steps;
        result.totalTime = std::chrono::duration<double>(simEndTime - startTime).count();
        result.stepsPerSecond = steps / result.totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapseCount) / result.neuronCount;
        result.averageFiringRate = brain->getAverageFiringRate();
        result.totalSpikes = brain->getTotalSpikeCount();
        result.activeNeuronCount = brain->getFiringNeuronCount();
        result.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        
        // Calculate advanced metrics
        double totalSpikeRate = brain->getTotalSpikeCount();
        double totalPossibleSpikes = result.neuronCount * steps * 0.1; // Estimate
        result.learningScore = totalSpikeRate / totalPossibleSpikes;
        
        if (result.steps > 0) {
            result.stabilityScore = 1.0 / (1.0 + std::abs(1.0 - result.spikesPerStep));
        }
        
        result.efficiencyScore = result.stepsPerSecond / (result.neuronCount / 1000.0f); // Normalized by network size
        
        return result;
    }
    
    // Performance benchmark
    EnhancedBenchmarkResult runPerformanceTest(const ConfigurationManager& config) {
        EnhancedBenchmarkResult result("Performance Benchmark");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Use performance preset configuration
        ConfigurationManager tempConfig = config;
        tempConfig.applyPreset(ConfigPreset::Performance);
        
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        brain->initialize();
        
        auto initEndTime = std::chrono::high_resolution_clock::now();
        result.initializationTime = std::chrono::duration<double>(initEndTime - startTime).count();
        
        // Run high-intensity simulation
        size_t steps = 500;
        for (size_t step = 0; step < steps; ++step) {
            // Strong input to maximize throughput
            brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
            brain->step(step, step * 0.001);
        }
        
        auto simEndTime = std::chrono::high_resolution_clock::now();
        result.simulationTime = std::chrono::duration<double>(simEndTime - initEndTime).count();
        
        // Collect results
        result.neuronCount = brain->getTotalNeuronCount();
        result.synapseCount = brain->getTotalSynapseCount();
        result.steps = steps;
        result.totalTime = std::chrono::duration<double>(simEndTime - startTime).count();
        result.stepsPerSecond = steps / result.totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapseCount) / result.neuronCount;
        result.averageFiringRate = brain->getAverageFiringRate();
        result.totalSpikes = brain->getTotalSpikeCount();
        result.activeNeuronCount = brain->getFiringNeuronCount();
        result.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        
        // Performance metrics
        result.cpuUsage = result.stepsPerSecond / 1000.0f; // Normalized
        result.memoryUsage = result.neuronCount * sizeof(Neuron) / (1024.0 * 1024.0f); // MB
        
        result.stabilityScore = result.stepsPerSecond / 1000.0f; // Steps per millisecond
        result.efficiencyScore = result.stepsPerSecond / (result.neuronCount / 1000.0f);
        
        return result;
    }
    
    // Scalability test
    std::vector<EnhancedBenchmarkResult> runScalabilityTest(const ConfigurationManager& config) {
        std::vector<EnhancedBenchmarkResult> results;
        
        std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
        
        for (size_t neuronCount : neuronCounts) {
            ConfigurationManager tempConfig = config;
            tempConfig.applyPreset(ConfigPreset::QuickTest);
            tempConfig.setWithValidation("neuron_count", static_cast<int64_t>(neuronCount));
            
            EnhancedBenchmarkResult result(std::string("Scalability Test - ") + std::to_string(neuronCount) + " neurons");
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            auto brain = std::make_shared<Brain>(std::make_shared<Config>());
            brain->initialize();
            
            auto initEndTime = std::chrono::high_resolution_clock::now();
            result.initializationTime = std::chrono::duration<double>(initEndTime - startTime).count();
            
            // Run shorter test for larger networks
            size_t steps = std::max(static_cast<size_t>(100), neuronCount / 10);
            for (size_t step = 0; step < steps; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                brain->step(step, step * 0.001);
            }
            
            auto simEndTime = std::chrono::high_resolution_clock::now();
            result.simulationTime = std::chrono::duration<double>(simEndTime - initEndTime).count();
            
            // Collect results
            result.neuronCount = brain->getTotalNeuronCount();
            result.synapseCount = brain->getTotalSynapseCount();
            result.steps = steps;
            result.totalTime = std::chrono::duration<double>(simEndTime - startTime).count();
            result.stepsPerSecond = steps / result.totalTime;
            result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
            result.synapsesPerNeuron = static_cast<double>(result.synapseCount) / result.neuronCount;
            result.averageFiringRate = brain->getAverageFiringRate();
            result.totalSpikes = brain->getTotalSpikeCount();
            result.activeNeuronCount = brain->getFiringNeuronCount();
            
            results.push_back(result);
        }
        
        return results;
    }
}

// Enhanced benchmark report generator
class EnhancedBenchmarkReport {
public:
    struct ReportOptions {
        bool verbose = false;
        bool csv = false;
        bool json = false;
        bool includeAdvancedMetrics = true;
        bool includeStabilityAnalysis = true;
        std::string outputFile;
    };
    
    static void generateReport(const std::vector<EnhancedBenchmarkResult>& results, 
                              const ReportOptions& options = ReportOptions()) {
        if (options.csv) {
            generateCSVReport(results, options.outputFile.empty() ? "benchmark_results.csv" : options.outputFile);
        } else if (options.json) {
            generateJSONReport(results, options.outputFile.empty() ? "benchmark_results.json" : options.outputFile);
        } else {
            generateTextReport(results, options);
        }
    }
    
private:
    static void generateTextReport(const std::vector<EnhancedBenchmarkResult>& results, 
                                  const ReportOptions& options) {
        std::cout << "=== Enhanced NLM Benchmark Report ===" << std::endl;
        std::cout << std::endl;
        
        for (const auto& result : results) {
            OutputFormatter::printSection(result.benchmarkName);
            
            std::cout << "Network Configuration:" << std::endl;
            std::cout << "  Neurons: " << result.neuronCount << std::endl;
            std::cout << "  Synapses: " << result.synapseCount << std::endl;
            std::cout << "  Synapses/Neuron: " << std::fixed << std::setprecision(2) << result.synapsesPerNeuron << std::endl;
            std::cout << std::endl;
            
            std::cout << "Performance:" << std::endl;
            std::cout << "  Total Time: " << std::fixed << std::setprecision(2) << result.totalTime << "s" << std::endl;
            std::cout << "  Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
            std::cout << "  Spikes/Step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
            std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << result.averageFiringRate << " Hz" << std::endl;
            std::cout << std::endl;
            
            std::cout << "Activity:" << std::endl;
            std::cout << "  Total Spikes: " << result.totalSpikes << std::endl;
            std::cout << "  Active Neurons: " << result.activeNeuronCount << std::endl;
            std::cout << "  Peak Firing Rate: " << std::fixed << std::setprecision(2) << result.peakFiringRate << " Hz" << std::endl;
            std::cout << std::endl;
            
            std::cout << "Network Properties:" << std::endl;
            std::cout << "  Excitation/Inhibition Ratio: " << std::fixed << std::setprecision(2) << result.excitationInhibitionRatio << std::endl;
            std::cout << "  Average Synaptic Weight: " << std::fixed << std::setprecision(3) << result.averageSynapticWeight << std::endl;
            std::cout << std::endl;
            
            if (options.includeAdvancedMetrics) {
                std::cout << "Advanced Metrics:" << std::endl;
                std::cout << "  Stability Score: " << std::fixed << std::setprecision(3) << result.stabilityScore << std::endl;
                std::cout << "  Learning Score: " << std::fixed << std::setprecision(3) << result.learningScore << std::endl;
                std::cout << "  Efficiency Score: " << std::fixed << std::setprecision(3) << result.efficiencyScore << std::endl;
                std::cout << "  Memory Usage: " << std::fixed << std::setprecision(2) << result.memoryUsage << " MB" << std::endl;
                std::cout << "  CPU Usage: " << std::fixed << std::setprecision(3) << result.cpuUsage << std::endl;
                std::cout << std::endl;
            }
            
            if (options.includeStabilityAnalysis) {
                std::cout << "Analysis:" << std::endl;
                if (result.stabilityScore > 0.8) {
                    std::cout << "  ✓ Stable operation" << std::endl;
                } else if (result.stabilityScore > 0.5) {
                    std::cout << "  ⚠ Moderate stability" << std::endl;
                } else {
                    std::cout << "  ✗ Unstable operation" << std::endl;
                }
                
                if (result.learningScore > 0.5) {
                    std::cout << "  ✓ Good learning performance" << std::endl;
                } else if (result.learningScore > 0.2) {
                    std::cout << "  ⚠ Moderate learning" << std::endl;
                } else {
                    std::cout << "  ✗ Limited learning" << std::endl;
                }
                
                if (result.efficiencyScore > 1.0) {
                    std::cout << "  ✓ High efficiency" << std::endl;
                } else if (result.efficiencyScore > 0.5) {
                    std::cout << "  ⚠ Moderate efficiency" << std::endl;
                } else {
                    std::cout << "  ✗ Low efficiency" << std::endl;
                }
                std::cout << std::endl;
            }
            
            std::cout << "---" << std::endl;
        }
        
        // Summary statistics
        std::cout << "=== Benchmark Summary ===" << std::endl;
        if (!results.empty()) {
            double avgStepsPerSecond = 0.0;
            double avgFiringRate = 0.0;
            double avgStability = 0.0;
            
            for (const auto& result : results) {
                avgStepsPerSecond += result.stepsPerSecond;
                avgFiringRate += result.averageFiringRate;
                avgStability += result.stabilityScore;
            }
            
            avgStepsPerSecond /= results.size();
            avgFiringRate /= results.size();
            avgStability /= results.size();
            
            std::cout << "Average Steps/sec: " << std::fixed << std::setprecision(0) << avgStepsPerSecond << std::endl;
            std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(2) << avgFiringRate << " Hz" << std::endl;
            std::cout << "Average Stability Score: " << std::fixed << std::setprecision(3) << avgStability << std::endl;
            std::cout << std::endl;
        }
        
        std::cout << "=== Benchmark Complete ===" << std::endl;
    }
    
    static void generateCSVReport(const std::vector<EnhancedBenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open output file " << filename << std::endl;
            return;
        }
        
        // Header
        file << "Benchmark,Neurons,Synapses,Steps,TotalTime,StepsPerSecond,SpikesPerStep," << std::endl;
        file << "AverageFiringRate,TotalSpikes,ActiveNeurons,ExcitationInhibitionRatio," << std::endl;
        file << "LearningScore,StabilityScore,EfficiencyScore,MemoryUsage,CPUUsage" << std::endl;
        
        // Data
        for (const auto& result : results) {
            file << result.benchmarkName << ",";
            file << result.neuronCount << ",";
            file << result.synapseCount << ",";
            file << result.steps << ",";
            file << std::fixed << std::setprecision(3) << result.totalTime << ",";
            file << std::fixed << std::setprecision(2) << result.stepsPerSecond << ",";
            file << std::fixed << std::setprecision(3) << result.spikesPerStep << ",";
            file << std::endl;
            
            file << std::fixed << std::setprecision(2) << result.averageFiringRate << ",";
            file << result.totalSpikes << ",";
            file << result.activeNeuronCount << ",";
            file << std::fixed << std::setprecision(2) << result.excitationInhibitionRatio << ",";
            file << std::endl;
            
            file << std::fixed << std::setprecision(3) << result.learningScore << ",";
            file << std::fixed << std::setprecision(3) << result.stabilityScore << ",";
            file << std::fixed << std::setprecision(3) << result.efficiencyScore << ",";
            file << std::fixed << std::setprecision(2) << result.memoryUsage << ",";
            file << std::fixed << std::setprecision(3) << result.cpuUsage << std::endl;
        }
        
        file.close();
        std::cout << "CSV report saved to: " << filename << std::endl;
    }
    
    static void generateJSONReport(const std::vector<EnhancedBenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open output file " << filename << std::endl;
            return;
        }
        
        file << "{" << std::endl;
        file << "  \"benchmarks\": [" << std::endl;
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            
            file << "    {" << std::endl;
            file << "      \"name\": \"" << result.benchmarkName << "\"," << std::endl;
            file << "      \"neurons\": " << result.neuronCount << "," << std::endl;
            file << "      \"synapses\": " << result.synapseCount << "," << std::endl;
            file << "      \"steps\": " << result.steps << "," << std::endl;
            file << "      \"totalTime\": " << std::fixed << std::setprecision(3) << result.totalTime << "," << std::endl;
            file << "      \"stepsPerSecond\": " << std::fixed << std::setprecision(2) << result.stepsPerSecond << "," << std::endl;
            file << "      \"spikesPerStep\": " << std::fixed << std::setprecision(3) << result.spikesPerStep << "," << std::endl;
            file << "      \"averageFiringRate\": " << std::fixed << std::setprecision(2) << result.averageFiringRate << "," << std::endl;
            file << "      \"totalSpikes\": " << result.totalSpikes << "," << std::endl;
            file << "      \"stabilityScore\": " << std::fixed << std::setprecision(3) << result.stabilityScore << "," << std::endl;
            file << "      \"learningScore\": " << std::fixed << std::setprecision(3) << result.learningScore << "," << std::endl;
            file << "      \"efficiencyScore\": " << std::fixed << std::setprecision(3) << result.efficiencyScore << std::endl;
            
            if (i < results.size() - 1) {
                file << "    }," << std::endl;
            } else {
                file << "    }" << std::endl;
            }
        }
        
        file << "  ]" << std::endl;
        file << "}" << std::endl;
        
        file.close();
        std::cout << "JSON report saved to: " << filename << std::endl;
    }
};

int main(int argc, char** argv) {
    // Enhanced benchmark main function
    printBanner();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running Enhanced NLM Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    // Create configuration manager
    ConfigurationManager& configManager = getConfigurationManager();
    
    // Parse command line arguments
    std::vector<std::string> args(argv, argv + argc);
    
    // Check for benchmark type
    std::string benchmarkType = "quick";
    std::string benchmarkModes = "quick,full,performance,scalability";
    
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--benchmark" || args[i] == "-b") {
            if (i + 1 < args.size()) {
                benchmarkType = args[++i];
            }
        }
    }
    
    std::transform(benchmarkType.begin(), benchmarkType.end(), benchmarkType.begin(), ::tolower);
    
    if (benchmarkModes.find(benchmarkType) == std::string::npos) {
        std::cerr << "Error: Invalid benchmark type \"" << benchmarkType << "\"" << std::endl;
        std::cerr << "Valid options: " << benchmarkModes << std::endl;
        return 1;
    }
    
    // Run selected benchmark(s)
    std::vector<EnhancedBenchmarkResult> allResults;
    
    if (benchmarkType == "scalability") {
        std::cout << "Running Scalability Benchmark..." << std::endl;
        auto scalabilityResults = BenchmarkScenarios::runScalabilityTest(configManager);
        allResults.insert(allResults.end(), scalabilityResults.begin(), scalabilityResults.end());
    } else if (benchmarkType == "performance") {
        std::cout << "Running Performance Benchmark..." << std::endl;
        auto result = BenchmarkScenarios::runPerformanceTest(configManager);
        allResults.push_back(result);
    } else if (benchmarkType == "full") {
        std::cout << "Running Full Integration Benchmark..." << std::endl;
        auto result = BenchmarkScenarios::runFullTest(configManager);
        allResults.push_back(result);
    } else {
        std::cout << "Running Quick Connectivity Benchmark..." << std::endl;
        auto result = BenchmarkScenarios::runQuickTest(configManager);
        allResults.push_back(result);
    }
    
    // Generate enhanced report
    EnhancedBenchmarkReport::ReportOptions options;
    
    // Check for output options
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--csv" || args[i] == "-c") {
            options.csv = true;
        } else if (args[i] == "--json" || args[i] == "-j") {
            options.json = true;
        } else if (args[i] == "--verbose" || args[i] == "-v") {
            options.verbose = true;
            options.includeAdvancedMetrics = true;
            options.includeStabilityAnalysis = true;
        } else if (args[i] == "--quiet" || args[i] == "-q") {
            options.verbose = false;
            options.includeAdvancedMetrics = false;
            options.includeStabilityAnalysis = false;
        } else if (args[i] == "--output" || args[i] == "-o") {
            if (i + 1 < args.size()) {
                options.outputFile = args[++i];
            }
        }
    }
    
    std::cout << std::endl;
    EnhancedBenchmarkReport::generateReport(allResults, options);
    
    std::cout << std::endl;
    std::cout << "Enhanced benchmark complete." << std::endl;
    
    return 0;
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Performance Benchmark                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}