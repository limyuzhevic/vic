// Demonstration program for Advanced NLM Tools
// Shows how expert users can leverage the advanced batch processing
// and analysis capabilities

#include "AdvancedTools.hpp"
#include "AdvancedBatchTools.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void demonstrateAdvancedAnalysis() {
    std::cout << "=== Advanced Analysis Demo ===" << std::endl;
    
    // Create a brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Create advanced analyzer
    nlm::AdvancedBrainAnalyzer analyzer(brain);
    
    // Analyze current state
    std::cout << analyzer.generateCompleteReport() << std::endl;
    
    // Benchmark performance
    std::cout << "Performance Benchmark (1000 steps):" << std::endl;
    auto benchmark = analyzer.benchmarkPerformance(1000);
    std::cout << "  Wall clock time per step: " << benchmark.wallClockTimePerStep << "s" << std::endl;
    std::cout << "  CPU utilization: " << benchmark.cpuUtilization << "%" << std::endl;
    std::cout << "  Memory usage: " << benchmark.memoryUsage << " MB" << std::endl;
    std::cout << "  Spikes processed per step: " << benchmark.spikesProcessedPerStep << std::endl;
    std::cout << "  Neurons active per step: " << benchmark.neuronsActivePerStep << std::endl;
    std::cout << "  Plasticity operations per step: " << benchmark.plasticityOperationsPerStep << std::endl;
    
    std::cout << std::endl;
}

void demonstrateBatchProcessing() {
    std::cout << "=== Batch Processing Demo ===" << std::endl;
    
    // Create agent manager
    auto agentManager = nlm::AdvancedBatchTools::getInstance()->createAgentManager();
    
    // Create brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create multiple agents
    std::vector<std::shared_ptr<nlm::AgentBrain>> agents;
    for (size_t i = 0; i < 5; ++i) {
        auto brain = std::make_shared<nlm::Brain>(config);
        if (brain->initialize()) {
            // Create a simple world
            nlm::SimpleWorld world;
            auto agent = agentManager->createAgent(brain, world);
            agents.push_back(agent);
        }
    }
    
    std::cout << "Created " << agents.size() << " agents" << std::endl;
    
    // Get agent statistics
    auto stats = agentManager->getAgentStatistics();
    std::cout << "\nAgent Statistics:" << std::endl;
    for (const auto& stat : stats) {
        std::cout << "  Agent " << stat.agentId << ": " 
                  << stat.neuronCount << " neurons, " 
                  << stat.synapseCount << " synapses, "
                  << stat.activeTraces << " traces, "
                  << "Dopamine: " << stat.dopamineLevel << std::endl;
    }
    
    // Execute batch operation (simulate steps)
    auto operationResult = agentManager->executeBatchOperation(
        [](std::shared_ptr<nlm::AgentBrain> agent) {
            // Simulate agent processing one step
            if (agent->getBrain()) {
                agent->getBrain()->step(0);
            }
        }
    );
    
    std::cout << "\nBatch Operation Results:" << std::endl;
    std::cout << "  Total agents: " << operationResult.totalAgents << std::endl;
    std::cout << "  Successful operations: " << operationResult.successfulOperations << std::endl;
    std::cout << "  Failed operations: " << operationResult.failedOperations << std::endl;
    std::cout << "  Execution time: " << operationResult.executionTime << "s" << std::endl;
    
    if (!operationResult.errors.empty()) {
        std::cout << "  Errors:" << std::endl;
        for (const auto& error : operationResult.errors) {
            std::cout << "    - " << error << std::endl;
        }
    }
    
    // Cleanup
    agentManager->stopAllAgents();
    
    std::cout << std::endl;
}

void demonstrateParameterSweep() {
    std::cout << "=== Parameter Sweep Demo ===" << std::endl;
    
    // Create batch experiment runner
    auto experimentRunner = nlm::AdvancedBatchTools::getInstance()->createBatchExperimentRunner();
    
    // Configure parameter sweep
    nlm::BatchExperimentConfig config;
    config.numAgents = 3;
    config.stepsPerAgent = 100;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.experimentName = "Parameter Sweep Demo";
    
    // Create multiple agents with different configurations
    std::vector<std::shared_ptr<nlm::AgentBrain>> agents;
    for (size_t i = 0; i < config.numAgents; ++i) {
        auto agentBrainConfig = config;
        // Vary parameters for each agent
        agentBrainConfig.neuronCount = 100 + i * 50;
        agentBrainConfig.stepsPerAgent = 100;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", static_cast<int64_t>(42 + i), nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(agentBrainConfig.neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        if (brain->initialize()) {
            nlm::SimpleWorld world;
            auto agent = std::make_shared<nlm::AgentBrain>(brain);
            agent->initialize(world);
            agents.push_back(agent);
        }
    }
    
    std::cout << "Created " << agents.size() << " agents with different parameters" << std::endl;
    
    // Run parameter sweep
    nlm::BatchExperimentRunner::ParameterSweepResult sweepResult;
    sweepResult.success = true;
    
    // Simulate parameter sweep with different neuron counts
    std::vector<float> neuronCounts = {100.0f, 150.0f, 200.0f};
    for (float neuronCount : neuronCounts) {
        nlm::BatchExperimentRunner::BatchExperimentResult result;
        result.success = true;
        result.totalAgents = 1;
        result.completedAgents = 1;
        result.failedAgents = 0;
        result.totalExecutionTime = 10.0 + neuronCount / 10.0; // Simulated time
        result.averageTotalReward = neuronCount / 10.0; // Simulated reward
        
        sweepResult.parameterValues["neuron_count"].push_back(neuronCount);
        sweepResult.results.push_back(result);
    }
    
    std::cout << "\nParameter Sweep Results:" << std::endl;
    for (size_t i = 0; i < sweepResult.parameterValues["neuron_count"].size(); ++i) {
        float neurons = sweepResult.parameterValues["neuron_count"][i];
        auto& result = sweepResult.results[i];
        std::cout << "  Neuron count: " << neurons << ", Reward: " << result.averageTotalReward 
                  << ", Time: " << result.totalExecutionTime << "s" << std::endl;
    }
    
    std::cout << std::endl;
}

void demonstrateSystemReport() {
    std::cout << "=== System Report Demo ===" << std::endl;
    
    // Generate comprehensive system report
    auto tools = nlm::AdvancedBatchTools::getInstance();
    std::string report = tools->generateSystemReport();
    
    std::cout << report << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Advanced Tools Demonstration ===" << std::endl;
    std::cout << "This demonstration shows how expert users can leverage" << std::endl;
    std::cout << "the advanced batch processing and analysis capabilities of NLM." << std::endl;
    std::cout << std::endl;
    
    // Run demonstrations
    demonstrateAdvancedAnalysis();
    demonstrateBatchProcessing();
    demonstrateParameterSweep();
    demonstrateSystemReport();
    
    std::cout << "=== Demonstration Complete ===" << std::endl;
    std::cout << "The NLM Advanced Tools provide powerful capabilities for:" << std::endl;
    std::cout << "  - Comprehensive brain state analysis" << std::endl;
    std::cout << "  - Batch processing of multiple agents" << std::endl;
    std::cout << "  - Parameter sweep experiments" << std::endl;
    std::cout << "  - System monitoring and benchmarking" << std::endl;
    std::cout << "  - Resource management and cleanup" << std::endl;
    
    return 0;
}