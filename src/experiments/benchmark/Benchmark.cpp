// Benchmark executable for NLM Phase 2
// Performance testing and optimization verification

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "SimpleWorld.hpp"
#include "AgentBrain.hpp"
#include <iostream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cmath>

using namespace nlm;

class BenchmarkSuite {
public:
    struct BenchmarkResults {
        std::string benchmarkName;
        double executionTime;  // seconds
        double stepsPerSecond;
        double spikesPerSecond;
        size_t totalSpikes;
        size_t totalSteps;
        size_t neuronCount;
        float efficiencyScore;
        std::string metricsReport;
    };

    BenchmarkSuite() {
        // Initialize logger with appropriate settings
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }

    BenchmarkResults runBrainBenchmark(size_t numSteps = 1000, bool enablePlasticity = true) {
        std::cout << "=== Brain Performance Benchmark ===" << std::endl;
        std::cout << "Running " << numSteps << " simulation steps..." << std::endl;

        auto startTime = std::chrono::high_resolution_clock::now();

        // Create and configure brain
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);

        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return {"BrainBenchmark", 0.0, 0.0, 0.0, 0, 0, 0, 0.0, "Failed to initialize"};
        }

        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        size_t initialSteps = 0;

        // Run benchmark
        for (size_t step = 0; step < numSteps; ++step) {
            // Inject some current to stimulate activity
            if (step % 10 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Excitatory, 10.0f);
            }

            brain->step(step, step * 0.001);
            
            initialSteps = step;
        }

        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t totalSpikes = finalSpikes - initialSpikes;

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        double executionTime = duration.count() / 1000.0;  // convert to seconds
        double stepsPerSecond = (executionTime > 0) ? (numSteps / executionTime) : 0.0;
        double spikesPerSecond = (executionTime > 0) ? (totalSpikes / executionTime) : 0.0;

        // Calculate efficiency score
        double efficiencyScore = (stepsPerSecond > 0) ? 
            (spikesPerSecond / (brain->getTotalNeuronCount() * 10.0)) : 0.0;  // normalized

        BenchmarkResults results;
        results.benchmarkName = "BrainBenchmark";
        results.executionTime = executionTime;
        results.stepsPerSecond = stepsPerSecond;
        results.spikesPerSecond = spikesPerSecond;
        results.totalSpikes = totalSpikes;
        results.totalSteps = numSteps;
        results.neuronCount = brain->getTotalNeuronCount();
        results.efficiencyScore = efficiencyScore;
        results.metricsReport = generateMetricsReport(brain, executionTime);

        std::cout << "Benchmark completed successfully!" << std::endl;
        std::cout << results.metricsReport << std::endl;

        return results;
    }

    BenchmarkResults runWorldBenchmark(size_t numSteps = 500, bool enableLearning = true) {
        std::cout << "=== World Integration Benchmark ===" << std::endl;
        std::cout << "Running " << numSteps << " world simulation steps..." << std::endl;

        auto startTime = std::chrono::high_resolution_clock::now();

        // Create brain
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        config->set("simulation_timestep", 0.1, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);

        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return {"WorldBenchmark", 0.0, 0.0, 0.0, 0, 0, 0, 0.0, "Failed to initialize"};
        }

        // Create and configure world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width=10, height=10, visionWidth=8, visionHeight=8);
        world->reset();
        world->setAgentStart(5.0, 5.0);

        // Create agent brain interface
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);

        // Enable learning features
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }

        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();

        // Run benchmark
        for (size_t step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1);

            // Get sensory input
            auto percept = world->getSensoryPercept();

            // Process sensory input
            agent->processSensoryInput(percept);

            // Run brain step
            brain->step(step);

            // Get motor command
            auto action = agent->decodeMotorCommand();

            // Apply motor command
            world->applyMotorCommand(action, world->getSimulationTime());

            // Apply reward modulation
            double reward = 0.0;
            if (world->getAgentBody().energy > 50.0) {
                reward = 1.0;
            }
            agent->applyRewardModulation(reward, 0.0);

            // Update development
            agent->updateDevelopment(0.1);
        }

        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t totalSpikes = finalSpikes - initialSpikes;

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        double executionTime = duration.count() / 1000.0;  // convert to seconds
        double stepsPerSecond = (executionTime > 0) ? (numSteps / executionTime) : 0.0;
        double spikesPerSecond = (executionTime > 0) ? (totalSpikes / executionTime) : 0.0;

        // Calculate efficiency score
        double efficiencyScore = (stepsPerSecond > 0) ? 
            (spikesPerSecond / (brain->getTotalNeuronCount() * 5.0)) : 0.0;  // normalized

        BenchmarkResults results;
        results.benchmarkName = "WorldBenchmark";
        results.executionTime = executionTime;
        results.stepsPerSecond = stepsPerSecond;
        results.spikesPerSecond = spikesPerSecond;
        results.totalSpikes = totalSpikes;
        results.totalSteps = numSteps;
        results.neuronCount = brain->getTotalNeuronCount();
        results.efficiencyScore = efficiencyScore;
        results.metricsReport = generateMetricsReport(brain, executionTime);

        std::cout << "World benchmark completed successfully!" << std::endl;
        std::cout << results.metricsReport << std::endl;

        return results;
    }

    void runAllBenchmarks() {
        std::cout << "=== NLM Phase 2 Benchmark Suite ===" << std::endl;
        std::cout << "Running comprehensive performance tests..." << std::endl << std::endl;

        // Brain benchmark
        auto brainResults = runBrainBenchmark(1000);
        std::cout << std::endl;

        // World benchmark
        auto worldResults = runWorldBenchmark(500);
        std::cout << std::endl;

        // Summary
        std::cout << "=== Benchmark Summary ===" << std::endl;
        printResults(brainResults);
        std::cout << std::endl;
        printResults(worldResults);
        std::cout << std::endl;

        // Save results
        saveResultsToFile(brainResults, "benchmark_brain_results.json");
        saveResultsToFile(worldResults, "benchmark_world_results.json");
        std::cout << "Results saved to benchmark_*.json" << std::endl;
    }

private:
    std::string generateMetricsReport(std::shared_ptr<Brain> brain, double executionTime) {
        std::ostringstream report;
        report << std::fixed << std::setprecision(2);
        report << "--- Performance Metrics ---" << std::endl;
        report << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        report << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        report << "Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
        report << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        report << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        report << "Steps per Second: " << (executionTime > 0 ? brain->getTotalSpikeCount() / executionTime : 0.0) << std::endl;
        report << "Memory Efficiency: " << (brain->getTotalNeuronCount() * 10) / (executionTime * 1000) << " neurons/ms" << std::endl;
        report << "E/I Balance Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        return report.str();
    }

    void printResults(const BenchmarkResults& results) {
        std::cout << "Benchmark: " << results.benchmarkName << std::endl;
        std::cout << "Execution Time: " << results.executionTime << " seconds" << std::endl;
        std::cout << "Steps per Second: " << results.stepsPerSecond << std::endl;
        std::cout << "Spikes per Second: " << results.spikesPerSecond << std::endl;
        std::cout << "Total Spikes: " << results.totalSpikes << std::endl;
        std::cout << "Neurons: " << results.neuronCount << std::endl;
        std::cout << "Efficiency Score: " << results.efficiencyScore << std::endl;
    }

    void saveResultsToFile(const BenchmarkResults& results, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "{\n";
            file << "  \"benchmarkName\": \"" << results.benchmarkName << "\",\n";
            file << "  \"executionTime\": " << results.executionTime << ",\n";
            file << "  \"stepsPerSecond\": " << results.stepsPerSecond << ",\n";
            file << "  \"spikesPerSecond\": " << results.spikesPerSecond << ",\n";
            file << "  \"totalSpikes\": " << results.totalSpikes << ",\n";
            file << "  \"totalSteps\": " << results.totalSteps << ",\n";
            file << "  \"neuronCount\": " << results.neuronCount << ",\n";
            file << "  \"efficiencyScore\": " << results.efficiencyScore << ",\n";
            file << "  \"metricsReport\": \"" << results.metricsReport << "\"\n";
            file << "}\n";
            file.close();
        }
    }
};

int main() {
    std::cout << "=== NLM Phase 2 Benchmark Suite ===" << std::endl;
    std::cout << "Running comprehensive performance tests for neural learning machine." << std::endl << std::endl;

    BenchmarkSuite benchmarkSuite;
    benchmarkSuite.runAllBenchmarks();

    std::cout << std::endl;
    std::cout << "=== Benchmark Suite Complete ===" << std::endl;
    std::cout << "All benchmarks completed successfully. Check benchmark_*.json for detailed results." << std::endl;

    return 0;
}