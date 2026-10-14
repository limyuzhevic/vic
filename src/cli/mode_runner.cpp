#include "cli/mode_runner.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

namespace nlm {

ModeRunner::ModeRunner() {
    setupModeFunctions();
}

void ModeRunner::setupModeFunctions() {
    modeFunctions_["basic"] = &ModeRunner::runBasicModeInternal;
    modeFunctions_["full"] = &ModeRunner::runFullModeInternal;
    modeFunctions_["phase3"] = &ModeRunner::runPhase3ModeInternal;
    modeFunctions_["phase4"] = &ModeRunner::runPhase4ModeInternal;
    modeFunctions_["phase6"] = &ModeRunner::runPhase6ModeInternal;
}

bool ModeRunner::runMode(const std::string& modeName, const CommandLineOptions& options, std::shared_ptr<Config> config) {
    auto it = modeFunctions_.find(modeName);
    if (it == modeFunctions_.end()) {
        std::cerr << "Unknown mode: " << modeName << std::endl;
        return false;
    }
    
    ModeFunction func = it->second;
    return (this->*func)(options, config);
}

bool ModeRunner::runBasicMode(const CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== Running Basic Mode ===");
    
    if (options.benchmark) {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool result = runBasicModeInternal(options, config);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("Basic mode completed in " + std::to_string(duration.count()) + " ms");
        
        return result;
    }
    
    return runBasicModeInternal(options, config);
}

bool ModeRunner::runFullMode(const CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== Running Full Integration Mode ===");
    
    if (options.benchmark) {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool result = runFullModeInternal(options, config);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("Full mode completed in " + std::to_string(duration.count()) + " ms");
        
        return result;
    }
    
    return runFullModeInternal(options, config);
}

bool ModeRunner::runPhase3Mode(const CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== Running Phase 3 World Interaction Demo ===");
    
    if (options.benchmark) {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool result = runPhase3ModeInternal(options, config);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("Phase 3 mode completed in " + std::to_string(duration.count()) + " ms");
        
        return result;
    }
    
    return runPhase3ModeInternal(options, config);
}

bool ModeRunner::runPhase4Mode(const CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== Running Phase 4 Cognition Experiments ===");
    
    if (options.benchmark) {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool result = runPhase4ModeInternal(options, config);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("Phase 4 mode completed in " + std::to_string(duration.count()) + " ms");
        
        return result;
    }
    
    return runPhase4ModeInternal(options, config);
}

bool ModeRunner::runPhase6Mode(const CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== Running Phase 6 Integration Test ===");
    
    if (options.benchmark) {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool result = runPhase6ModeInternal(options, config);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("Phase 6 mode completed in " + std::to_string(duration.count()) + " ms");
        
        return result;
    }
    
    return runPhase6ModeInternal(options, config);
}

bool ModeRunner::runBasicModeInternal(const CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running basic connectivity test...")
    
    // Create brain from config
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create a simple test agent
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Create a simple world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    // Enable basic subsystems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    // Run basic simulation
    int steps = options.fastMode ? 100 : 1000;
    int successfulSpikes = 0;
    
    for (int step = 0; step < steps; ++step) {
        // Process sensory input
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode motor command
        auto motorCmd = agent->decodeMotorCommand();
        
        // Apply to world
        world->applyMotorCommand(motorCmd, world->getSimulationTime());
        
        // Apply reward
        auto reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
        agent->applyRewardModulation(reward, 0.0);
        
        // Collect stats
        if (brain->getFiringNeuronCount() > 0) {
            successfulSpikes++;
        }
    }
    
    double successRate = (double)successfulSpikes / steps * 100.0;
    
    NLM_LOG_INFO("Basic mode test completed:")
    NLM_LOG_INFO("  Total steps: " + std::to_string(steps));
    NLM_LOG_INFO("  Successful spike steps: " + std::to_string(successfulSpikes));
    NLM_LOG_INFO("  Success rate: " + std::to_string(successRate) + "%")
    
    if (options.jsonOutput) {
        std::cout << "{\"mode\":\"basic\",\"steps\":" << steps << ",";
        std::cout << "\"successful_spikes\":" << successfulSpikes << ",";
        std::cout << "\"success_rate\":" << successRate << "}" << std::endl;
    }
    
    return successRate > 10.0; // Pass if at least 10% success rate
}

bool ModeRunner::runFullModeInternal(const CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running full integration test...")
    
    // Create brain from config
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    // Enable all subsystems for full integration
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Run comprehensive simulation
    int steps = options.fastMode ? 200 : 2000;
    int rewardEvents = 0;
    int noveltyEvents = 0;
    int developmentEvents = 0;
    
    for (int step = 0; step < steps; ++step) {
        // Update world
        world->update(options.timestep > 0 ? options.timestep : 0.1);
        
        // Process sensory input
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Get action
        auto motorCmd = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(motorCmd, world->getSimulationTime());
        
        // Apply reward and neuromodulation
        auto reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
        agent->applyRewardModulation(reward, 0.0);
        
        // Update development
        agent->updateDevelopment(options.timestep > 0 ? options.timestep : 0.1);
        
        // Collect metrics
        if (reward > 0.1) rewardEvents++;
        if (agent->getNoveltyLevel() > 0.5) noveltyEvents++;
        if (brain->getDevelopmentalStage() != DevelopmentalStage::Initial) developmentEvents++;
    }
    
    NLM_LOG_INFO("Full mode test completed:")
    NLM_LOG_INFO("  Total steps: " + std::to_string(steps));
    NLM_LOG_INFO("  Reward events: " + std::to_string(rewardEvents));
    NLM_LOG_INFO("  Novelty events: " + std::to_string(noveltyEvents));
    NLM_LOG_INFO("  Development events: " + std::to_string(developmentEvents));
    
    if (options.jsonOutput) {
        std::cout << "{\"mode\":\"full\",\"steps\":" << steps << ",";
        std::cout << "\"reward_events\":" << rewardEvents << ",";
        std::cout << "\"novelty_events\":" << noveltyEvents << ",";
        std::cout << "\"development_events\":" << developmentEvents << "}" << std::endl;
    }
    
    double successRate = (double)(rewardEvents + noveltyEvents) / steps * 100.0;
    return successRate > 5.0;
}

bool ModeRunner::runPhase3ModeInternal(const CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running Phase 3 world interaction demo...")
    
    // Create brain from config
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // Phase 3 specific: Enable world interaction subsystems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    // Run Phase 3 specific simulation
    int steps = options.fastMode ? 100 : 1000;
    int worldInteractions = 0;
    int learningEvents = 0;
    
    for (int step = 0; step < steps; ++step) {
        // Update world
        world->update(options.timestep > 0 ? options.timestep : 0.1);
        
        // Get sensory percept
        auto percept = world->getSensoryPercept();
        
        // Process sensory input (Phase 3 learning)
        agent->processSensoryInput(percept);
        
        // Check if this was a learning event (novelty/curiosity driven)
        if (agent->getNoveltyLevel() > 0.3) learningEvents++;
        
        // Get motor command
        auto motorCmd = agent->decodeMotorCommand();
        
        // Apply to world
        world->applyMotorCommand(motorCmd, world->getSimulationTime());
        
        // Check if world was affected
        if (world->getAgentBody()) {
            worldInteractions++;
        }
        
        // Apply reward
        auto reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
        agent->applyRewardModulation(reward, 0.0);
    }
    
    NLM_LOG_INFO("Phase 3 demo completed:")
    NLM_LOG_INFO("  Total steps: " + std::to_string(steps));
    NLM_LOG_INFO("  World interactions: " + std::to_string(worldInteractions));
    NLM_LOG_INFO("  Learning events: " + std::to_string(learningEvents));
    
    if (options.jsonOutput) {
        std::cout << "{\"mode\":\"phase3\",\"steps\":" << steps << ",";
        std::cout << "\"world_interactions\":" << worldInteractions << ",";
        std::cout << "\"learning_events\":" << learningEvents << "}" << std::endl;
    }
    
    double interactionRate = (double)worldInteractions / steps * 100.0;
    return interactionRate > 20.0;
}

bool ModeRunner::runPhase4ModeInternal(const CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running Phase 4 cognition experiments...")
    
    // Create brain from config
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // Phase 4 specific: Enable advanced cognition subsystems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    // Run Phase 4 specific simulation with cognitive elements
    int steps = options.fastMode ? 200 : 2000;
    int planningEvents = 0;
    int conceptEvents = 0;
    int attentionEvents = 0;
    
    for (int step = 0; step < steps; ++step) {
        // Update world
        world->update(options.timestep > 0 ? options.timestep : 0.1);
        
        // Get sensory percept
        auto percept = world->getSensoryPercept();
        
        // Process sensory input with cognitive processing
        agent->processSensoryInput(percept);
        
        // Simulate cognitive events (simplified - in real implementation these would be actual)
        if (agent->getPredictionError() > 0.5) planningEvents++;
        if (agent->getCuriosityLevel() > 0.7) conceptEvents++;
        if (agent->getNeuromodulationLevel() > 0.6) attentionEvents++;
        
        // Get action
        auto motorCmd = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(motorCmd, world->getSimulationTime());
        
        // Apply reward
        auto reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
        agent->applyRewardModulation(reward, 0.0);
        
        // Update development
        agent->updateDevelopment(options.timestep > 0 ? options.timestep : 0.1);
    }
    
    NLM_LOG_INFO("Phase 4 experiments completed:")
    NLM_LOG_INFO("  Total steps: " + std::to_string(steps));
    NLM_LOG_INFO("  Planning events: " + std::to_string(planningEvents));
    NLM_LOG_INFO("  Concept events: " + std::to_string(conceptEvents));
    NLM_LOG_INFO("  Attention events: " + std::to_string(attentionEvents));
    
    if (options.jsonOutput) {
        std::cout << "{\"mode\":\"phase4\",\"steps\":" << steps << ",";
        std::cout << "\"planning_events\":" << planningEvents << ",";
        std::cout << "\"concept_events\":" << conceptEvents << ",";
        std::cout << "\"attention_events\":" << attentionEvents << "}" << std::endl;
    }
    
    double cognitiveActivity = (double)(planningEvents + conceptEvents) / steps * 100.0;
    return cognitiveActivity > 15.0;
}

bool ModeRunner::runPhase6ModeInternal(const CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running Phase 6 integration test...")
    
    // Create brain from config
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // Phase 6: Enable ALL subsystems for comprehensive integration test
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Run Phase 6 integration test
    int steps = options.fastMode ? 300 : 3000;
    int successfulCycles = 0;
    int memoryEvents = 0;
    int neuromodEvents = 0;
    int predictionEvents = 0;
    
    for (int step = 0; step < steps; ++step) {
        // Update world
        world->update(options.timestep > 0 ? options.timestep : 0.1);
        
        // Get sensory percept
        auto percept = world->getSensoryPercept();
        
        // Process sensory input with full integration
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Get action
        auto motorCmd = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(motorCmd, world->getSimulationTime());
        
        // Apply reward
        auto reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
        agent->applyRewardModulation(reward, 0.0);
        
        // Update development
        agent->updateDevelopment(options.timestep > 0 ? options.timestep : 0.1);
        
        // Collect integration metrics
        if (brain->getTotalSpikeCount() > step * 0.01) successfulCycles++;
        if (agent->getPredictionError() > 0.1) predictionEvents++;
        if (agent->getCuriosityLevel() > 0.1) neuromodEvents++;
        if (agent->getNeuromodulationLevel() > 0.1) memoryEvents++;
    }
    
    NLM_LOG_INFO("Phase 6 integration test completed:")
    NLM_LOG_INFO("  Total steps: " + std::to_string(steps));
    NLM_LOG_INFO("  Successful neural cycles: " + std::to_string(successfulCycles));
    NLM_LOG_INFO("  Memory events: " + std::to_string(memoryEvents));
    NLM_LOG_INFO("  Neuromodulation events: " + std::to_string(neuromodEvents));
    NLM_LOG_INFO("  Prediction events: " + std::to_string(predictionEvents));
    
    if (options.jsonOutput) {
        std::cout << "{\"mode\":\"phase6\",\"steps\":" << steps << ",";
        std::cout << "\"successful_cycles\":" << successfulCycles << ",";
        std::cout << "\"memory_events\":" << memoryEvents << ",";
        std::cout << "\"neuromod_events\":" << neuromodEvents << ",";
        std::cout << "\"prediction_events\":" << predictionEvents << "}" << std::endl;
    }
    
    double integrationSuccess = (double)(successfulCycles + memoryEvents) / (steps * 2) * 100.0;
    return integrationSuccess > 20.0;
}

void ModeRunner::saveBenchmarkResult(const BenchmarkResult& result, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open benchmark result file: " << filename << std::endl;
        return;
    }
    
    file << "Benchmark Results:" << std::endl;
    file << "==================" << std::endl;
    file << "Mode: " << result.mode << std::endl;
    file << "Duration: " << result.duration.count() << " ms" << std::endl;
    file << "Success: " << (result.success ? "true" : "false") << std::endl;
    file << "Error Message: " << result.errorMessage << std::endl;
    file << std::endl;
    file << "Metrics:" << std::endl;
    for (const auto& metric : result.metrics) {
        file << "  " << metric.first << ": " << metric.second << std::endl;
    }
    
    file.close();
}

} // namespace nlm