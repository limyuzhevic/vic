// Phase 6 Integration Experiment
// Final Integration Phase - connects all systems into a coherent artificial brain

#include "Phase6Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>

namespace nlm {

// ============================================================================
// Phase6IntegrationResult
// ============================================================================

std::string Phase6IntegrationResult::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"totalReward\": " << totalReward << ",\n";
    oss << "  \"avgFiringRate\": " << avgFiringRate << ",\n";
    oss << "  \"memoryWorkingMemoryIntegrated\": " << (memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
    oss << "  \"memoryEpisodicMemoryIntegrated\": " << (memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
    oss << "  \"neuromodulationIntegrated\": " << (neuromodulationIntegrated ? "true" : "false") << ",\n";
    oss << "  \"predictionIntegrated\": " << (predictionIntegrated ? "true" : "false") << ",\n";
    oss << "  \"developmentIntegrated\": " << (developmentIntegrated ? "true" : "false") << ",\n";
    oss << "  \"checkpointingWorks\": " << (checkpointingWorks ? "true" : "false") << ",\n";
    oss << "  \"memoryEpisodesStored\": " << memoryEpisodesStored << ",\n";
    oss << "  \"noveltyLevel\": " << noveltyLevel << ",\n";
    oss << "  \"curiosityLevel\": " << curiosityLevel << ",\n";
    oss << "  \"dopamineLevel\": " << dopamineLevel << ",\n";
    oss << "  \"totalSteps\": " << totalSteps << ",\n";
    oss << "  \"wallClockTime\": " << totalWallClockTime << "\n";
    oss << "}\n";
    return oss.str();
}

std::string Phase6IntegrationResult::summary() const {
    std::ostringstream oss;
    oss << "=== NLM Phase 6 Final Integration Results ===\n\n";
    oss << "Integration Status:\n";
    oss << "  Working Memory: " << (memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    oss << "  Episodic Memory: " << (memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    oss << "  Neuromodulation: " << (neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    oss << "  Prediction: " << (predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    oss << "  Development: " << (developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    oss << "  Checkpointing: " << (checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n\n";
    
    oss << "Performance Metrics:\n";
    oss << "  Total Reward: " << std::fixed << std::setprecision(4) << totalReward << "\n";
    oss << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << avgFiringRate << "\n";
    oss << "  Memory Episodes: " << std::fixed << std::setprecision(2) << memoryEpisodesStored << "\n";
    oss << "  Novelty Level: " << std::fixed << std::setprecision(2) << noveltyLevel << "\n";
    oss << "  Curiosity Level: " << std::fixed << std::setprecision(2) << curiosityLevel << "\n";
    oss << "  Dopamine Level: " << std::fixed << std::setprecision(2) << dopamineLevel << "\n";
    oss << "  Total Simulation Steps: " << totalSteps << "\n";
    oss << "  Wall Clock Time: " << std::fixed << std::setprecision(2) << totalWallClockTime << "s\n";
    
    return oss.str();
}

// ============================================================================
// Phase6IntegratedExperiment
// ============================================================================

Phase6IntegratedExperiment::Phase6IntegratedExperiment() {
    NLM_LOG_INFO("Initializing NLM Phase 6 Final Integration");
}

Phase6IntegratedExperiment::~Phase6IntegratedExperiment() = default;

Phase6IntegrationResult Phase6IntegratedExperiment::run(
    const Phase6Config& config,
    const std::function<std::shared_ptr<Brain>(const Config& config)>& createBrain,
    const std::function<std::shared_ptr<SimpleWorld>(const Config& config)>& createWorld,
    const std::function<std::shared_ptr<AgentBrain>(std::shared_ptr<Brain> brain)>& createAgent
) {
    auto result = initializeResult(config);
    
    NLM_LOG_INFO("=== Phase 6 Final Integration Experiment ===");
    NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                 std::to_string(config.maxSteps) + " steps");
    
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", config.neuronCount);
    cfg->set("region_count", config.regionCount);
    cfg->set("connection_probability", config.connectionProbability);
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    cfg->set("synaptogenesis_rate", 0.0001f);
    cfg->set("pruning_rate", 0.00001f);
    
    // Create brain using provided factory
    auto brain = createBrain(*cfg);
    if (!brain) {
        NLM_LOG_ERROR("Failed to create brain");
        return result;
    }
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return result;
    }
    
    // Create world using provided factory
    auto world = createWorld(*cfg);
    if (!world) {
        NLM_LOG_ERROR("Failed to create world");
        return result;
    }
    
    world->initialize(config.environmentWidth, config.environmentHeight);
    
    // Create agent using provided factory
    auto agent = createAgent(brain);
    if (!agent) {
        NLM_LOG_ERROR("Failed to create agent");
        return result;
    }
    
    agent->initialize(*world);
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(config.enableDevelopment);
    agent->enableDevelopment(config.enableDevelopment);
    agent->enableCuriosity(true);
    
    NLM_LOG_INFO("Integrated brain, world, and agent initialized successfully");
    
    // Run simulation loop
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    for (uint64_t step = 0; step < config.maxSteps; ++step) {
        // Get observation from world
        SensoryPercept percept = world->observe(brain->getRegions()[0].get());
        
        // Process sensory input through agent
        agent->processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Get motor command from agent
        MotorCommand cmd = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyAction(brain->getRegions()[0].get(), cmd);
        
        // Compute reward
        float reward = world->computeReward(brain->getRegions()[0].get());
        totalReward += reward;
        
        // Apply reward modulation
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development system
        if (config.enableDevelopment) {
            agent->updateDevelopment(0.001);
        }
        
        // Collect metrics
        totalFiringRate += brain->getAverageFiringRate();
        if (brain->getFiringNeuronCount() > 0) firingCount++;
        
        // Periodic status update
        if (step % 1000 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(totalReward / (step + 1)) +
                        " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                        " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                            brain->getWorkingMemory()->getActiveTraces() : 0));
        }
    }
    
    // Collect final metrics
    result.totalReward = totalReward / config.maxSteps;
    result.avgFiringRate = totalFiringRate / config.maxSteps;
    result.memoryEpisodesStored = brain->getEpisodicMemory() ? 
        static_cast<float>(brain->getEpisodicMemory()->getEpisodeCount()) : 0.0f;
    result.noveltyLevel = agent->getNoveltyLevel();
    result.curiosityLevel = agent->getCuriosityLevel();
    result.dopamineLevel = agent->getNeuromodulationLevel();
    result.totalSteps = config.maxSteps;
    
    // Verify integration status
    result.memoryWorkingMemoryIntegrated = (brain->getWorkingMemory() != nullptr);
    result.memoryEpisodicMemoryIntegrated = (brain->getEpisodicMemory() != nullptr);
    result.neuromodulationIntegrated = (brain->getDopamine() != nullptr);
    result.predictionIntegrated = (brain->getPredictionSystem() != nullptr);
    result.developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
    
    // Test checkpointing if enabled
    if (config.enableCheckpointing) {
        NLM_LOG_INFO("Testing checkpoint save/load...");
        if (brain->save(config.checkpointPath)) {
            NLM_LOG_INFO("Checkpoint saved successfully");
            
            // Create new brain and load
            auto brain2 = std::make_shared<Brain>(cfg);
            brain2->initialize();
            
            if (brain2->load(config.checkpointPath)) {
                NLM_LOG_INFO("Checkpoint loaded successfully");
                result.checkpointingWorks = true;
            } else {
                NLM_LOG_ERROR("Failed to load checkpoint");
            }
        } else {
            NLM_LOG_ERROR("Failed to save checkpoint");
        }
    }
    
    return result;
}

std::vector<Phase6IntegrationResult> Phase6IntegratedExperiment::runMultipleConfigurations(
    const std::vector<Phase6Config>& configs,
    const std::function<std::shared_ptr<Brain>(const Config& config)>& createBrain,
    const std::function<std::shared_ptr<SimpleWorld>(const Config& config)>& createWorld,
    const std::function<std::shared_ptr<AgentBrain>(std::shared_ptr<Brain> brain)>& createAgent
) {
    NLM_LOG_INFO("Running multiple configurations for comprehensive testing");
    
    std::vector<Phase6IntegrationResult> results;
    results.reserve(configs.size());
    
    for (size_t i = 0; i < configs.size(); ++i) {
        NLM_LOG_INFO("Running configuration " + std::to_string(i + 1) + "/" + std::to_string(configs.size()));
        
        Phase6Config config = configs[i];
        config.checkpointPath = "/tmp/nlm_phase6_checkpoint_" + std::to_string(i) + ".bin";
        
        auto result = run(config, createBrain, createWorld, createAgent);
        results.push_back(result);
    }
    
    return results;
}

Phase6IntegrationResult Phase6IntegratedExperiment::initializeResult(const Phase6Config& config) {
    Phase6IntegrationResult result;
    result.startTime = time(nullptr);
    
    NLM_LOG_INFO("Starting Phase 6 integration experiment");
    
    return result;
}

std::vector<std::string> Phase6IntegratedExperiment::generateSystemReport(
    std::shared_ptr<Brain> brain
) {
    std::vector<std::string> report;
    
    report.push_back("=== NLM Phase 6 System Report ===");
    report.push_back("Brain Status:");
    report.push_back("  Total Neurons: " + std::to_string(brain->getTotalNeuronCount()));
    report.push_back("  Total Synapses: " + std::to_string(brain->getTotalSynapseCount()));
    report.push_back("  Total Spikes: " + std::to_string(brain->getTotalSpikeCount()));
    
    if (brain->getWorkingMemory()) {
        report.push_back("  Working Memory: Active traces = " + std::to_string(brain->getWorkingMemory()->getActiveTraces()));
    }
    
    if (brain->getEpisodicMemory()) {
        report.push_back("  Episodic Memory: Episodes = " + std::to_string(brain->getEpisodicMemory()->getEpisodeCount()));
    }
    
    if (brain->getDopamine()) {
        report.push_back("  Neuromodulation: Active = true");
    }
    
    report.push_back("  Prediction System: " + std::string(brain->getPredictionSystem() ? "Active" : "Inactive"));
    report.push_back("  Development System: " + std::string(brain->getDevelopmentSystem() ? "Active" : "Inactive"));
    
    return report;
}

} // namespace nlm
