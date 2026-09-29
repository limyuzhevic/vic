// NLM Multi-Agent Benchmarks
// Comprehensive benchmarking for Multi-Agent system

#include "benchmarks/MultiAgentBenchmarks.hpp"
#include "learning/MultiAgent.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "core/Config/Config.hpp"
#include "core/Profiler.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <set>
#include <string>

using namespace nlm;

namespace benchmarks {

// Inter-agent communication benchmarks
void MultiAgentBenchmarks::runInterAgentCommunicationBenchmarks() {
    std::cout << "=== Inter-Agent Communication Benchmarks ===" << std::endl;
    
    const std::vector<size_t> agentCounts = {10, 50, 100, 200};
    const size_t communicationSteps = 100;
    
    CommunicationBenchmarkResults globalResults;
    globalResults.totalTests = agentCounts.size();
    
    for (size_t agentCount : agentCounts) {
        std::cout << "Inter-agent communication benchmark: " << agentCount << " agents, " << communicationSteps << " steps..." << std::flush;
        
        // Create multi-agent configuration
        auto config = std::make_shared<Config>();
        config->set("agent_count", static_cast<int64_t>(agentCount), ConfigSource::Default);
        config->set("neuron_count_per_agent", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        
        // Create multi-agent system
        auto multiAgent = std::make_shared<MultiAgent>(config);
        multiAgent->initialize();
        
        // Start profiling communication operations
        startPhase(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        InterAgentCommunicationResults result;
        result.agentCount = agentCount;
        result.totalSteps = communicationSteps;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run communication simulation
        for (size_t step = 0; step < communicationSteps; ++step) {
            // Each agent performs communication operations
            for (size_t agent = 0; agent < agentCount; ++agent) {
                // Generate agent-specific input
                float agentInput = 20.0f + (30.0f * std::sin(static_cast<double>(step) * 0.02 + agent * 0.1));
                
                // Agent performs communication (in real implementation, this would involve inter-agent messaging)
                multiAgent->performCommunication(agent, agentInput);
            }
            
            // Integrate all agents
            multiAgent->integrateAllAgents(step, step * 0.001);
            
            // Check communication metrics
            size_t totalMessages = multiAgent->getTotalCommunicationMessages();
            result.totalMessages += totalMessages;
            
            // Get inter-agent communication statistics
            auto commStats = multiAgent->getCommunicationStats(agent);
            result.totalLatency += commStats.averageLatency;
            result.maxThroughput = std::max(result.maxThroughput, commStats.throughput);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stopPhase(); // PHASE_NEURON_ACTIVATION
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto neuronActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        CommunicationStats commStats;
        commStats.totalMessages = result.totalMessages;
        commStats.averageLatency = result.totalLatency / result.totalMessages;
        commStats.throughput = result.maxThroughput;
        
        result.totalTime = totalTime;
        result.stepsPerSecond = communicationSteps / totalTime;
        result.messagesPerSecond = result.totalMessages / totalTime;
        result.averageLatency = commStats.averageLatency;
        result.throughput = commStats.throughput;
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Messages/sec: " << std::fixed << std::setprecision(1) << result.messagesPerSecond << std::endl;
        std::cout << "    Average latency: " << std::fixed << std::setprecision(3) << result.averageLatency << " ms" << std::endl;
        std::cout << "    Throughput: " << std::fixed << std::setprecision(1) << result.throughput << " messages/s" << std::endl;
        
        resetProfiling(); // Reset for next test
    }
    
    printInterAgentCommunicationResults(globalResults);
}

// Coordination overhead benchmarks
void MultiAgentBenchmarks::runCoordinationOverheadBenchmarks() {
    std::cout << "=== Coordination Overhead Benchmarks ===" << std::endl;
    
    const std::vector<std::pair<size_t, size_t>> configurations = {
        {10, 50},    // 10 agents, 50 integration steps
        {50, 100},   // 50 agents, 100 integration steps
        {100, 200},  // 100 agents, 200 integration steps
        {200, 300},  // 200 agents, 300 integration steps
    };
    
    CoordinationOverheadResults globalResults;
    globalResults.totalTests = configurations.size();
    
    for (const auto& [agentCount, integrationSteps] : configurations) {
        std::cout << "Coordination overhead benchmark: " << agentCount << " agents, " << integrationSteps << " integrations..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("agent_count", static_cast<int64_t>(agentCount), ConfigSource::Default);
        config->set("neuron_count_per_agent", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        
        auto multiAgent = std::make_shared<MultiAgent>(config);
        multiAgent->initialize();
        
        // Start profiling coordination operations
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        startPhase(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        CoordinationOverheadResults::TestResult result;
        result.agentCount = agentCount;
        result.integrationSteps = integrationSteps;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run coordination-intensive simulation
        for (size_t step = 0; step < integrationSteps; ++step) {
            // Each integration step involves extensive coordination
            for (size_t agent = 0; agent < agentCount; ++agent) {
                // Agent-specific coordination
                multiAgent->coordinateWithAgents(agent);
            }
            
            // Global integration
            multiAgent->globalIntegration(step, step * 0.001);
            
            // Get coordination metrics
            auto coordStats = multiAgent->getCoordinationStats();
            result.totalCoordinationTime += coordStats.averageCoordinationTime;
            result.totalSynchronizationTime += coordStats.synchronizationTime;
            result.maxCoordinationOverhead = std::max(result.maxCoordinationOverhead, coordStats.coordinationOverhead);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stopPhase(); // PHASE_NEURON_ACTIVATION
        stopPhase(); // PHASE_SYNAPSE_ACTIVATION
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto synapseActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        auto neuronActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        CoordinationStats coordStats;
        coordStats.averageCoordinationTime = result.totalCoordinationTime / integrationSteps;
        coordStats.synchronizationTime = result.totalSynchronizationTime / integrationSteps;
        coordStats.coordinationOverhead = result.maxCoordinationOverhead;
        
        result.totalTime = totalTime;
        result.stepsPerSecond = integrationSteps / totalTime;
        result.averageCoordinationTime = coordStats.averageCoordinationTime;
        result.averageSynchronizationTime = coordStats.synchronizationTime;
        result.coordinationOverhead = coordStats.coordinationOverhead;
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Avg coordination time: " << std::fixed << std::setprecision(3) << result.averageCoordinationTime << " ms" << std::endl;
        std::cout << "    Coordination overhead: " << std::fixed << std::setprecision(2) << result.coordinationOverhead << std::endl;
        
        resetProfiling(); // Reset for next test
    }
    
    printCoordinationOverheadResults(globalResults);
}

// Social learning efficiency benchmarks
void MultiAgentBenchmarks::runSocialLearningEfficiencyBenchmarks() {
    std::cout << "=== Social Learning Efficiency Benchmarks ===" << std::endl;
    
    const std::vector<size_t> agentCounts = {20, 50, 100, 200};
    const size_t learningSteps = 150;
    
    SocialLearningResults globalResults;
    globalResults.totalTests = agentCounts.size();
    
    for (size_t agentCount : agentCounts) {
        std::cout << "Social learning efficiency benchmark: " << agentCount << " agents, " << learningSteps << " learning steps..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("agent_count", static_cast<int64_t>(agentCount), ConfigSource::Default);
        config->set("neuron_count_per_agent", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("social_learning_rate", 0.1f, ConfigSource::Default);
        
        auto multiAgent = std::make_shared<MultiAgent>(config);
        multiAgent->initialize();
        
        // Start profiling social learning operations
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        startPhase(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        SocialLearningResults::TestResult result;
        result.agentCount = agentCount;
        result.learningSteps = learningSteps;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run social learning simulation
        for (size_t step = 0; step < learningSteps; ++step) {
            // Each agent engages in social learning
            for (size_t agent = 0; agent < agentCount; ++agent) {
                // Generate agent-specific social learning input
                float socialInput = 20.0f + (30.0f * std::sin(static_cast<double>(step) * 0.02 + agent * 0.05));
                
                // Social learning operation
                multiAgent->performSocialLearning(agent, socialInput);
            }
            
            // Group social learning
            multiAgent->socialLearningGroup(step, step * 0.001);
            
            // Get social learning metrics
            auto socialStats = multiAgent->getSocialLearningStats();
            result.totalLearningEfficiency += socialStats.learningEfficiency;
            result.maxLearningGain = std::max(result.maxLearningGain, socialStats.learningGain);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stopPhase(); // PHASE_NEURON_ACTIVATION
        stopPhase(); // PHASE_SYNAPSE_ACTIVATION
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto synapseActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        auto neuronActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        SocialLearningStats socialStats;
        socialStats.learningEfficiency = result.totalLearningEfficiency / learningSteps;
        socialStats.learningGain = result.maxLearningGain;
        
        result.totalTime = totalTime;
        result.stepsPerSecond = learningSteps / totalTime;
        result.averageLearningEfficiency = socialStats.learningEfficiency;
        result.maxLearningGain = socialStats.learningGain;
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Avg learning efficiency: " << std::fixed << std::setprecision(2) << result.averageLearningEfficiency << std::endl;
        std::cout << "    Max learning gain: " << std::fixed << std::setprecision(2) << result.maxLearningGain << std::endl;
        
        resetProfiling(); // Reset for next test
    }
    
    printSocialLearningEfficiencyResults(globalResults);
}

// Competitive vs. cooperative learning benchmarks
void MultiAgentBenchmarks::runCompetitiveCooperativeLearningBenchmarks() {
    std::cout << "=== Competitive vs. Cooperative Learning Benchmarks ===" << std::endl;
    
    const std::vector<size_t> agentCounts = {20, 50, 100};
    const size_t learningSteps = 120;
    
    CompetitiveCooperativeResults globalResults;
    globalResults.totalTests = agentCounts.size();
    
    for (size_t agentCount : agentCounts) {
        std::cout << "Competitive/cooperative learning benchmark: " << agentCount << " agents, " << learningSteps << " steps..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("agent_count", static_cast<int64_t>(agentCount), ConfigSource::Default);
        config->set("neuron_count_per_agent", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        
        auto multiAgent = std::make_shared<MultiAgent>(config);
        multiAgent->initialize();
        
        // Start profiling competitive/cooperative operations
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        startPhase(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        CompetitiveCooperativeResults::TestResult result;
        result.agentCount = agentCount;
        result.learningSteps = learningSteps;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation with both competitive and cooperative phases
        for (size_t step = 0; step < learningSteps; ++step) {
            // Determine phase based on step
            bool isCooperativePhase = (step % 3 == 0); // Every 3rd step is cooperative
            
            if (isCooperativePhase) {
                // Cooperative learning phase
                multiAgent->cooperativeLearningPhase(step, step * 0.001);
                auto coopStats = multiAgent->getCooperativeLearningStats();
                result.totalCooperativeEfficiency += coopStats.cooperativeEfficiency;
                result.maxCooperativeGain = std::max(result.maxCooperativeGain, coopStats.learningGain);
            } else {
                // Competitive learning phase
                multiAgent->competitiveLearningPhase(step, step * 0.001);
                auto compStats = multiAgent->getCompetitiveLearningStats();
                result.totalCompetitiveEfficiency += compStats.competitiveEfficiency;
                result.maxCompetitiveGain = std::max(result.maxCompetitiveGain, compStats.learningGain);
            }
            
            // Mixed phase
            multiAgent->mixedLearningPhase(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stopPhase(); // PHASE_NEURON_ACTIVATION
        stopPhase(); // PHASE_SYNAPSE_ACTIVATION
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto synapseActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        auto neuronActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        CompetitiveCooperativeStats ccStats;
        ccStats.cooperativeEfficiency = result.totalCooperativeEfficiency / (learningSteps / 3);
        ccStats.competitiveEfficiency = result.totalCompetitiveEfficiency / (learningSteps * 2 / 3);
        ccStats.cooperativeGain = result.maxCooperativeGain;
        ccStats.competitiveGain = result.maxCompetitiveGain;
        
        result.totalTime = totalTime;
        result.stepsPerSecond = learningSteps / totalTime;
        result.averageCooperativeEfficiency = ccStats.cooperativeEfficiency;
        result.averageCompetitiveEfficiency = ccStats.competitiveEfficiency;
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Avg cooperative efficiency: " << std::fixed << std::setprecision(2) << result.averageCooperativeEfficiency << std::endl;
        std::cout << "    Avg competitive efficiency: " << std::fixed << std::setprecision(2) << result.averageCompetitiveEfficiency << std::endl;
        
        resetProfiling(); // Reset for next test
    }
    
    printCompetitiveCooperativeLearningResults(globalResults);
}

// Helper functions for multi-agent benchmark analysis

double MultiAgentBenchmarks::calculateCommunicationEfficiency(size_t agentCount, size_t steps) {
    double agentFactor = 1000.0 / agentCount;
    double stepFactor = steps / 100.0;
    
    return agentFactor / stepFactor;
}

double MultiAgentBenchmarks::calculateCoordinationOverhead(size_t agentCount, size_t integrationSteps) {
    double agentFactor = std::log(agentCount + 1); // Logarithmic scaling
    double integrationFactor = integrationSteps / 50.0;
    
    return agentFactor * integrationFactor;
}

double MultiAgentBenchmarks::calculateSocialLearningGain(size_t agentCount, size_t learningSteps) {
    double agentFactor = agentCount / 50.0;
    double stepFactor = learningSteps / 100.0;
    
    return agentFactor * stepFactor;
}

double MultiAgentBenchmarks::calculateLearningBalance(size_t agentCount, size_t steps) {
    double agentFactor = 1.0 / agentCount;
    double stepFactor = steps / 100.0;
    
    return agentFactor * stepFactor;
}

// Result printing functions

void MultiAgentBenchmarks::printInterAgentCommunicationResults(const CommunicationBenchmarkResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║            Inter-Agent Communication Benchmark Results                ║" << std::endl;
    std::cout << "╠════════════╦════════╦════════════╦════════════╦═════════╦════════╗" << std::endl;
    std::cout << "║  Agents   ║ Steps/s ║ Messages/s ║ Latency(ms) ║ Throughput ║" << std::endl;
    std::cout << "╠════════════╬════════╬════════════╬════════════╬═════════╬════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.agentCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(1) << r.messagesPerSecond
                  << " ║ " << std::setw(13) << std::fixed << std::setprecision(3) << r.averageLatency
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(1) << r.throughput
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩════════════╩════════════╩═════════╩════════╝" << std::endl;
    std::cout << std::endl;
}

void MultiAgentBenchmarks::printCoordinationOverheadResults(const CoordinationOverheadResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Coordination Overhead Benchmark Results                ║" << std::endl;
    std::cout << "╠════════════╦════════╦════════╦════════════════╦═════════╣" << std::endl;
    std::cout << "║  Agents   ║ Steps/s ║ Coord(ms) ║ Sync(ms)  ║ Overhead ║" << std::endl;
    std::cout << "╠════════════╬════════╬════════╬════════════════╬═════════╣" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.agentCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(3) << r.averageCoordinationTime
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(3) << r.averageSynchronizationTime
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(2) << r.coordinationOverhead
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩════════╩════════════════╩═════════╝" << std::endl;
    std::cout << std::endl;
}

void MultiAgentBenchmarks::printSocialLearningEfficiencyResults(const SocialLearningResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║             Social Learning Efficiency Benchmark Results             ║" << std::endl;
    std::cout << "╠════════════╦════════╦══════════════╦════════╗" << std::endl;
    std::cout << "║  Agents   ║ Steps/s ║ Avg Eff     ║ Max Gain ║" << std::endl;
    std::cout << "╠════════════╬════════╬══════════════╬════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.agentCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(14) << std::fixed << std::setprecision(2) << r.averageLearningEfficiency
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(2) << r.maxLearningGain
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩══════════════╩════════╝" << std::endl;
    std::cout << std::endl;
}

void MultiAgentBenchmarks::printCompetitiveCooperativeLearningResults(const CompetitiveCooperativeResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           Competitive vs. Cooperative Learning Benchmark Results     ║" << std::endl;
    std::cout << "╠════════════╦════════╦════════════╦════════════════╦════════════════╗" << std::endl;
    std::cout << "║  Agents   ║ Steps/s ║ Coop Eff  ║ Comp Eff  ║ Learning Bal ║" << std::endl;
    std::cout << "╠════════════╬════════╬════════════╬════════════════╬════════════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.agentCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(2) << r.averageCooperativeEfficiency
                  << " ║ " << std::setw(16) << std::fixed << std::setprecision(2) << r.averageCompetitiveEfficiency
                  << " ║ " << std::setw(15) << std::fixed << std::setprecision(2) << calculateLearningBalance(r.agentCount, r.learningSteps)
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩════════════╩════════════════╩════════════════╝" << std::endl;
    std::cout << std::endl;
}

} // namespace benchmarks

// Main multi-agent benchmark runner
int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Multi-Agent Benchmarks                             ║
    ║                                                               ║
    ║     Comprehensive Multi-Agent System Benchmarking              ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize random number generator
    auto rng = std::make_shared<RandomGenerator>();
    RandomGenerator::setGlobal(rng);
    
    std::cout << "Running Multi-Agent Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    // Run all multi-agent benchmark suites
    benchmarks::MultiAgentBenchmarks::runInterAgentCommunicationBenchmarks();
    std::cout << std::endl;
    
    benchmarks::MultiAgentBenchmarks::runCoordinationOverheadBenchmarks();
    std::cout << std::endl;
    
    benchmarks::MultiAgentBenchmarks::runSocialLearningEfficiencyBenchmarks();
    std::cout << std::endl;
    
    benchmarks::MultiAgentBenchmarks::runCompetitiveCooperativeLearningBenchmarks();
    std::cout << std::endl;
    
    // Generate final profiling report
    std::cout << "=== FINAL MULTI-AGENT BENCHMARK PROFILING REPORT ===" << std::endl;
    std::cout << generateProfilingReport() << std::endl;
    
    std::cout << "Multi-Agent Benchmarks complete." << std::endl;
    
    return 0;
}
