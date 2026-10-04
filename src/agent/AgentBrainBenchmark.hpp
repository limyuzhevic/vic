// Agent-specific performance benchmarks
#include "AgentBrain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../experiments/ScalingBenchmark.hpp"
#include <chrono>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>

namespace nlm {

AgentBrainBenchmark::AgentBrainBenchmark()
    : totalTests_(0)
    , passedTests_(0)
    , failedTests_(0)
{
}

AgentBrainBenchmark::~AgentBrainBenchmark() = default;

AgentBrainBenchmarkResult AgentBrainBenchmark::runBrainBenchmark(const std::string& brainType, 
                                                                 const ScaleLevel& scale,
                                                                 uint64_t steps) {
    std::cout << "Running " << brainType << " benchmark: " << scale.name 
              << " (" << scale.neurons << " neurons, " << steps << " steps)..." << std::endl;
    
    AgentBrainBenchmarkResult result;
    result.brainType = brainType;
    result.scale = scale;
    result.steps = steps;
    result.seed = 42; // Fixed seed for reproducible benchmarks
    
    // Create brain configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(scale.neurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", scale.connectionProbability, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Performance measurement
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    for (uint64_t step = 0; step < steps; ++step) {
        // Inject sensory input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        
        // Step brain
        brain->step(step, step * 0.001f);
        
        // Create simple action if needed
        if (brainType == "agent") {
            // Agent-specific processing
            brain->produceAction();
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect metrics
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.activeNeurons = brain->getActiveNeuronCount();
    result.firingNeurons = brain->getFiringNeuronCount();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.averageFiringRate = brain->getAverageFiringRate();
    result.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    
    result.totalTime = totalTime;
    result.stepsPerSecond = static_cast<double>(steps) / totalTime;
    result.spikesPerSecond = static_cast<double>(result.totalSpikes) / totalTime;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    result.memoryUsageMB = estimateMemoryUsage(result.neurons, result.synapses);
    
    result.success = true;
    
    std::cout << "  Completed in " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "  Performance: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << " steps/sec" << std::endl;
    
    return result;
}

AgentBrainBenchmarkResult AgentBrainBenchmark::runAgentBenchmark(const std::string& agentType,
                                                                  const ScaleLevel& scale,
                                                                  uint64_t steps) {
    std::cout << "Running agent benchmark: " << agentType << " - " 
              << scale.name << " (" << scale.neurons << " neurons)..." << std::endl;
    
    AgentBrainBenchmarkResult result;
    result.brainType = "agent:" + agentType;
    result.scale = scale;
    result.steps = steps;
    result.seed = 42;
    
    // Create brain configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(scale.neurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", scale.connectionProbability, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create AgentBrain (if agent_type == "agent")
    std::shared_ptr<AgentBrain> agentBrain;
    if (agentType == "agent") {
        agentBrain = std::make_shared<AgentBrain>(brain);
    }
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(100, 100, 16, 16);
    world->reset();
    
    // Performance measurement
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation loop
    for (uint64_t step = 0; step < steps; ++step) {
        // Create sensory percept
        auto percept = world->getSensoryPercept();
        
        // Process input through AgentBrain
        if (agentBrain) {
            agentBrain->processSensoryInput(*percept);
            
            // Get action
            auto action = agentBrain->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, static_cast<double>(step));
            
            // Get new percept
            percept = world->getSensoryPercept();
        }
        
        // Process through brain
        brain->receiveSensoryInput(*percept);
        brain->step(step, static_cast<double>(step) * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect metrics
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.activeNeurons = brain->getActiveNeuronCount();
    result.firingNeurons = brain->getFiringNeuronCount();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.averageFiringRate = brain->getAverageFiringRate();
    result.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    
    // Agent-specific metrics
    if (agentBrain) {
        result.agentMetrics.dopamineLevel = agentBrain->getNeuromodulationLevel();
        result.agentMetrics.curiosityLevel = agentBrain->getCuriosityLevel();
        result.agentMetrics.noveltyLevel = agentBrain->getNoveltyLevel();
        result.agentMetrics.predictionError = agentBrain->getPredictionError();
        result.agentMetrics.developmentalAge = agentBrain->getDevelopmentalStage();
    }
    
    result.totalTime = totalTime;
    result.stepsPerSecond = static_cast<double>(steps) / totalTime;
    result.spikesPerSecond = static_cast<double>(result.totalSpikes) / totalTime;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    result.memoryUsageMB = estimateMemoryUsage(result.neurons, result.synapses);
    
    result.success = true;
    
    std::cout << "  Completed in " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "  Performance: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << " steps/sec" << std::endl;
    
    return result;
}

void AgentBrainBenchmark::addBenchmark(const BenchmarkInfo& benchmark) {
    benchmarks_.push_back(benchmark);
}

BenchmarkComparison AgentBrainBenchmark::compareResults(const std::vector<AgentBrainBenchmarkResult>& results) {
    BenchmarkComparison comparison;
    
    if (results.empty()) {
        comparison.success = false;
        comparison.message = "No results to compare";
        return comparison;
    }
    
    comparison.success = true;
    comparison.message = "Comparison successful";
    
    // Calculate statistics for each metric
    for (const auto& metric : benchmarkMetrics_) {
        std::vector<double> values;
        
        for (const auto& result : results) {
            if (metric == "steps_per_second") {
                values.push_back(result.stepsPerSecond);
            } else if (metric == "spikes_per_second") {
                values.push_back(result.spikesPerSecond);
            } else if (metric == "memory_usage_mb") {
                values.push_back(static_cast<double>(result.memoryUsageMB));
            } else if (metric == "firing_rate_hz") {
                values.push_back(result.averageFiringRate);
            } else if (metric == "synapses_per_neuron") {
                values.push_back(result.synapsesPerNeuron);
            }
        }
        
        if (!values.empty()) {
            double min = *std::min_element(values.begin(), values.end());
            double max = *std::max_element(values.begin(), values.end());
            double avg = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
            
            BenchmarkMetricStats stats;
            stats.metric = metric;
            stats.minimum = min;
            stats.maximum = max;
            stats.average = avg;
            stats.standardDeviation = calculateStdDev(values, avg);
            
            comparison.metricStats.push_back(stats);
        }
    }
    
    return comparison;
}

std::string AgentBrainBenchmark::generateReport(const std::vector<AgentBrainBenchmarkResult>& results) {
    std::stringstream report;
    
    report << "=== NLM Agent Brain Benchmark Report ===\n\n";
    report << "Generated: " << std::put_time(std::gmtime(&benchmarkTime_), "%Y-%m-%d %H:%M:%S") << "\n";
    report << "Total tests: " << totalTests_ << "\n";
    report << "Passed: " << passedTests_ << "\n";
    report << "Failed: " << failedTests_ << "\n\n";
    
    if (results.empty()) {
        report << "No benchmark results available.\n";
        return report.str();
    }
    
    // Summary table
    report << "Results Summary:\n";
    report << "================\n";
    report << "Test Name\tScale\tNeurons\tSteps/sec\tSpikes/sec\tMemory(MB)\tTime(s)\n";
    report << "------------------------------------------------------------------------------------------------\n";
    
    for (const auto& result : results) {
        std::string testName = extractTestName(result.brainType);
        std::string scaleName = result.scale.name;
        
        report << testName << "\t" << scaleName << "\t"
               << result.neurons << "\t"
               << std::fixed << std::setprecision(0) << result.stepsPerSecond << "\t"
               << std::fixed << std::setprecision(1) << result.spikesPerSecond << "\t"
               << std::fixed << std::setprecision(1) << result.memoryUsageMB << "\t"
               << std::fixed << std::setprecision(2) << result.totalTime << "\n";
    }
    
    // Performance analysis
    report << "\nPerformance Analysis:\n";
    report << "-------------------\n";
    
    double totalStepsPerSecond = 0.0;
    double totalSpikesPerSecond = 0.0;
    double totalMemoryUsage = 0.0;
    size_t validResults = 0;
    
    for (const auto& result : results) {
        if (result.success) {
            totalStepsPerSecond += result.stepsPerSecond;
            totalSpikesPerSecond += result.spikesPerSecond;
            totalMemoryUsage += result.memoryUsageMB;
            validResults++;
        }
    }
    
    if (validResults > 0) {
        double avgStepsPerSec = totalStepsPerSecond / validResults;
        double avgSpikesPerSec = totalSpikesPerSecond / validResults;
        double avgMemoryUsage = totalMemoryUsage / validResults;
        
        report << "Average Steps/sec: " << std::fixed << std::setprecision(0) << avgStepsPerSec << "\n";
        report << "Average Spikes/sec: " << std::fixed << std::setprecision(1) << avgSpikesPerSec << "\n";
        report << "Average Memory Usage: " << std::fixed << std::setprecision(1) << avgMemoryUsage << " MB\n";
        
        // Performance rating
        if (avgStepsPerSec > 1000.0) {
            report << "Performance Rating: EXCELLENT (>1000 steps/sec)\n";
        } else if (avgStepsPerSec > 500.0) {
            report << "Performance Rating: GOOD (500-1000 steps/sec)\n";
        } else if (avgStepsPerSec > 100.0) {
            report << "Performance Rating: FAIR (100-500 steps/sec)\n";
        } else {
            report << "Performance Rating: POOR (<100 steps/sec)\n";
        }
    }
    
    return report.str();
}

void AgentBrainBenchmark::generateVisualization(const std::vector<AgentBrainBenchmarkResult>& results,
                                                 const std::string& filename) {
    // Placeholder for visualization generation
    // In production, this would generate charts and graphs
    NLM_LOG_INFO("Visualization generation not yet implemented for " + filename);
}

size_t AgentBrainBenchmark::estimateMemoryUsage(size_t neurons, size_t synapses) {
    // Estimate memory usage based on neuron and synapse counts
    // This is a rough estimate - real implementation would use more precise methods
    
    // Neuron memory: ~1 KB per neuron (state, synaptic inputs, etc.)
    size_t neuronMemory = neurons * 1024; // bytes
    
    // Synapse memory: ~100 bytes per synapse (weight, eligibility trace, etc.)
    size_t synapseMemory = synapses * 100; // bytes
    
    // Additional overhead for brain structures
    size_t overhead = (neurons + synapses) * 16; // bytes per object
    
    size_t totalBytes = neuronMemory + synapseMemory + overhead;
    
    return totalBytes / (1024 * 1024); // Convert to MB
}

double AgentBrainBenchmark::calculateStdDev(const std::vector<double>& values, double mean) {
    if (values.empty()) return 0.0;
    
    double sum = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sum += diff * diff;
    }
    
    return std::sqrt(sum / values.size());
}

std::string AgentBrainBenchmark::extractTestName(const std::string& brainType) {
    // Extract test name from brain type string
    size_t pos = brainType.find(':');
    if (pos != std::string::npos) {
        return brainType.substr(pos + 1);
    }
    return brainType;
}

}} // namespace nlm
