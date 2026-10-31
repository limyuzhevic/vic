// NLM - Neural Learning Machine
// Implementation of utility functions for simplified API access
// Provides helper functions to make the NLM API easier to use

#include "UtilityFunctions.hpp"
#include "brain/Brain.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include <fstream>
#include <iomanip>

namespace nlm {

// BrainUtils implementation
std::shared_ptr<Brain> BrainUtils::createStandardBrain(
    size_t neuronCount,
    size_t regionCount,
    float connectionProbability,
    std::shared_ptr<Config> config
) {
    auto brainConfig = config ? config : std::make_shared<Config>();
    
    // Set standard configuration values
    brainConfig->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    brainConfig->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
    brainConfig->set("connection_probability", connectionProbability, ConfigSource::Default);
    
    // Set world parameters
    brainConfig->set("world_width", 20.0f, ConfigSource::Default);
    brainConfig->set("world_height", 20.0f, ConfigSource::Default);
    brainConfig->set("max_energy", 100.0f, ConfigSource::Default);
    
    // Set physics parameters
    brainConfig->set("move_speed", 3.0f, ConfigSource::Default);
    brainConfig->set("turn_speed", 2.0f, ConfigSource::Default);
    brainConfig->set("interact_range", 1.0f, ConfigSource::Default);
    
    // Set reward parameters
    brainConfig->set("reward_movement_cost", 0.01f, ConfigSource::Default);
    brainConfig->set("reward_wall_cost", 0.05f, ConfigSource::Default);
    brainConfig->set("reward_hit_hazard_cost", 0.02f, ConfigSource::Default);
    brainConfig->set("reward_hit_resource_gain", -0.005f, ConfigSource::Default);
    
    // Set neuromodulation parameters
    brainConfig->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    brainConfig->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    brainConfig->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    return std::make_shared<Brain>(brainConfig);
}

std::shared_ptr<Config> ConfigUtils::createDefaultConfig(
    const std::string& worldWidth,
    const std::string& worldHeight,
    const std::string& maxEnergy,
    const std::string& moveSpeed,
    const std::string& turnSpeed
) {
    auto config = std::make_shared<Config>();
    
    // Set world parameters
    config->set("world_width", std::stof(worldWidth), ConfigSource::Default);
    config->set("world_height", std::stof(worldHeight), ConfigSource::Default);
    config->set("max_energy", std::stof(maxEnergy), ConfigSource::Default);
    
    // Set physics parameters
    config->set("move_speed", std::stof(moveSpeed), ConfigSource::Default);
    config->set("turn_speed", std::stof(turnSpeed), ConfigSource::Default);
    config->set("interact_range", 1.0f, ConfigSource::Default);
    
    return config;
}

void BrainUtils::runSimpleDemo(
    const std::shared_ptr<Config>& config,
    int numEpisodes,
    int stepsPerEpisode
) {
    auto brain = createStandardBrain(500, 1, 0.15f, config);
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20.0f, 20.0f, 16, 16);
    world->setRandomSeed(42);
    
    AgentBrain agentBrain(brain);
    agentBrain.initialize(*world);
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    
    float totalReward = 0.0f;
    for (int episode = 0; episode < numEpisodes; ++episode) {
        world->reset();
        agentBrain.reset();
        brain->reset();
        brain->initialize();
        
        float episodeReward = 0.0f;
        for (int step = 0; step < stepsPerEpisode; ++step) {
            const SensoryPercept& percept = world->getSensoryPercept();
            agentBrain.processSensoryInput(percept);
            
            for (int i = 0; i < 10; ++i) {
                brain->step(static_cast<SimulationStep>(step * 10 + i));
            }
            
            MotorCommand cmd = agentBrain.decodeMotorCommand();
            ActionResult result = world->applyMotorCommand(cmd, world->getSimulationTime());
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            world->update(0.01f);
            
            episodeReward += result.reward;
        }
        
        totalReward += episodeReward;
        std::cout << "Episode " << episode << ": " << episodeReward << " reward, " 
                  << brain->getAverageFiringRate() << " Hz" << std::endl;
    }
    
    std::cout << "Total reward: " << totalReward << std::endl;
    std::cout << "Average reward per episode: " << totalReward / numEpisodes << std::endl;
}

// SensoryUtils implementation
SensoryPercept SensoryUtils::createVisionPercept(
    const std::vector<float>& vision,
    const std::vector<float>& touch,
    const std::vector<float>& internal,
    const std::vector<float>& proprioception
) {
    SensoryPercept percept;
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    return percept;
}

SensoryPercept SensoryUtils::createEmptyPercept() {
    return createVisionPercept(std::vector<float>(256, 1.0f));
}

SensoryPercept SensoryUtils::createResourcePercept(
    size_t visionIndex,
    float visionValue
) {
    std::vector<float> vision(256, 1.0f);
    if (visionIndex < vision.size()) {
        vision[visionIndex] = visionValue;
    }
    
    return createVisionPercept(vision);
}

// MotorUtils implementation
MotorCommand MotorUtils::getBestAction(std::shared_ptr<Brain> brain) {
    // This is a simple implementation - in practice, you would use
    // the agent's decodeMotorCommand method
    return MotorCommand::Wait;
}

MotorCommand MotorUtils::getActionWithRewardBias(
    std::shared_ptr<Brain> brain,
    float rewardBias
) {
    // This is a simple implementation - in practice, you would use
    // the agent's decodeMotorCommand method with reward bias
    return MotorCommand::Wait;
}

MotorCommand MotorUtils::getActionWithCuriosity(
    std::shared_ptr<Brain> brain,
    float curiosityLevel
) {
    // This is a simple implementation - in practice, you would use
    // the agent's decodeMotorCommand method with curiosity
    return MotorCommand::Wait;
}

// SimulationUtils implementation
float SimulationUtils::runSimulation(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    int numEpisodes,
    int stepsPerEpisode
) {
    AgentBrain agentBrain(brain);
    agentBrain.initialize(*world);
    agentBrain.enableRewardModulation(true);
    
    float totalReward = 0.0f;
    for (int episode = 0; episode < numEpisodes; ++episode) {
        world->reset();
        agentBrain.reset();
        brain->reset();
        brain->initialize();
        
        float episodeReward = 0.0f;
        for (int step = 0; step < stepsPerEpisode; ++step) {
            const SensoryPercept& percept = world->getSensoryPercept();
            agentBrain.processSensoryInput(percept);
            
            for (int i = 0; i < 10; ++i) {
                brain->step(static_cast<SimulationStep>(step * 10 + i));
            }
            
            MotorCommand cmd = agentBrain.decodeMotorCommand();
            ActionResult result = world->applyMotorCommand(cmd, world->getSimulationTime());
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            world->update(0.01f);
            
            episodeReward += result.reward;
        }
        
        totalReward += episodeReward;
    }
    
    return totalReward;
}

EpisodeMetrics SimulationUtils::runSimulationWithMetrics(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    int numEpisodes,
    int stepsPerEpisode
) {
    EpisodeMetrics metrics{};
    metrics.totalReward = 0.0f;
    metrics.finalEnergy = 0.0f;
    metrics.finalNovelty = 0.0f;
    metrics.finalCuriosity = 0.0f;
    metrics.predictionError = 0.0f;
    metrics.totalSpikes = 0;
    metrics.resourcesConsumed = 0;
    metrics.hazardsHit = 0;
    metrics.emptyInteracts = 0;
    metrics.steps = numEpisodes * stepsPerEpisode;
    
    AgentBrain agentBrain(brain);
    agentBrain.initialize(*world);
    agentBrain.enableRewardModulation(true);
    
    for (int episode = 0; episode < numEpisodes; ++episode) {
        world->reset();
        agentBrain.reset();
        brain->reset();
        brain->initialize();
        
        float episodeReward = 0.0f;
        for (int step = 0; step < stepsPerEpisode; ++step) {
            const SensoryPercept& percept = world->getSensoryPercept();
            agentBrain.processSensoryInput(percept);
            
            for (int i = 0; i < 10; ++i) {
                brain->step(static_cast<SimulationStep>(step * 10 + i));
            }
            
            MotorCommand cmd = agentBrain.decodeMotorCommand();
            ActionResult result = world->applyMotorCommand(cmd, world->getSimulationTime());
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            world->update(0.01f);
            
            episodeReward += result.reward;
            
            if (result.message == "consumed_resource") ++metrics.resourcesConsumed;
            else if (result.message == "hit_hazard") ++metrics.hazardsHit;
            else if (result.message == "no_object") ++metrics.emptyInteracts;
        }
        
        metrics.totalReward += episodeReward;
        metrics.finalEnergy = world->getAgentBody().energy;
        metrics.finalNovelty = agentBrain.getNoveltyLevel();
        metrics.finalCuriosity = agentBrain.getCuriosityLevel();
        metrics.predictionError = agentBrain.getPredictionError();
        metrics.totalSpikes += brain->getTotalSpikeCount();
    }
    
    return metrics;
}

// ConfigUtils implementation
template<typename T>
T ConfigUtils::getConfig(const std::shared_ptr<Config>& config, const std::string& key, const T& defaultValue) {
    return config->getOr<T>(key, defaultValue);
}

template<typename T>
void ConfigUtils::setConfig(std::shared_ptr<Config> config, const std::string& key, const T& value) {
    config->set(key, value, ConfigSource::Runtime);
}

void ConfigUtils::printConfigSummary(const std::shared_ptr<Config>& config) {
    std::cout << "=== Configuration Summary ===" << std::endl;
    for (const auto& key : config->getKeys()) {
        std::cout << "  " << key << ": ";
        try {
            if (auto intVal = config->get<int64_t>(key)) {
                std::cout << *intVal << std::endl;
            } else if (auto floatVal = config->get<float>(key)) {
                std::cout << *floatVal << std::endl;
            } else if (auto stringVal = config->get<std::string>(key)) {
                std::cout << *stringVal << std::endl;
            } else {
                std::cout << "[unknown type]" << std::endl;
            }
        } catch (...) {
            std::cout << "[error reading value]" << std::endl;
        }
    }
}

bool ConfigUtils::saveConfigTemplate(const std::string& filename, const std::string& useCase) {
    auto config = std::make_shared<Config>();
    
    // Add standard configuration values
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Save to file
    return config->saveToFile(filename);
}

// DebugUtils implementation
void DebugUtils::printBrainState(std::shared_ptr<Brain> brain) {
    if (!brain) {
        std::cout << "Brain is null!" << std::endl;
        return;
    }
    
    std::cout << "=== Brain State ===" << std::endl;
    std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
}

void DebugUtils::printConfiguration(std::shared_ptr<Config> config) {
    printConfigSummary(config);
}

bool DebugUtils::saveSimulationData(
    const std::string& filename,
    const std::vector<std::shared_ptr<Brain>>& brains,
    const std::vector<float>& rewards
) {
    std::ofstream file(filename);
    if (!file) {
        return false;
    }
    
    file << "Simulation Data" << std::endl;
    file << "================" << std::endl;
    file << "Number of brains: " << brains.size() << std::endl;
    file << "Number of rewards: " << rewards.size() << std::endl;
    
    for (size_t i = 0; i < brains.size(); ++i) {
        file << "\nBrain " << i << ":" << std::endl;
        if (brains[i]) {
            file << "  Neurons: " << brains[i]->getTotalNeuronCount() << std::endl;
            file << "  Synapses: " << brains[i]->getTotalSynapseCount() << std::endl;
            file << "  Total spikes: " << brains[i]->getTotalSpikeCount() << std::endl;
        } else {
            file << "  (null brain)" << std::endl;
        }
    }
    
    file << "\nRewards:" << std::endl;
    for (size_t i = 0; i < rewards.size(); ++i) {
        file << "  " << i << ": " << rewards[i] << std::endl;
    }
    
    file.close();
    return true;
}

bool DebugUtils::loadSimulationData(
    const std::string& filename,
    std::vector<std::shared_ptr<Brain>>& brains,
    std::vector<float>& rewards
) {
    std::ifstream file(filename);
    if (!file) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Parse the data (this is a simplified implementation)
        // In practice, you would use a proper parser
    }
    
    file.close();
    return true;
}

} // namespace nlm
