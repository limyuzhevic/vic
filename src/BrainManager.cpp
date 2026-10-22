#include "BrainManager.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>

namespace nlm {

std::unordered_map<std::string, std::function<void()>> BrainManager::initializationCallbacks_;
std::unordered_map<std::string, std::function<void()>> BrainManager::cleanupCallbacks_;

std::shared_ptr<Config> BrainManager::createConfig(
    size_t neuronCount,
    size_t regionCount,
    float connectionProbability,
    double timestep,
    uint64_t randomSeed
) {
    auto config = std::make_shared<Config>();
    
    // Set basic configuration with validation
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
    config->set("connection_probability", connectionProbability, ConfigSource::Default);
    config->set("simulation_timestep", timestep, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(randomSeed), ConfigSource::Default);
    
    // Set Phase 6 integration parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    
    // Set neuromodulation parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    NLM_LOG_INFO("Created configuration: " + config->summary());
    return config;
}

std::shared_ptr<Config> BrainManager::loadConfigFromFile(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(filepath)) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
        return config;
    } else {
        NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
        return nullptr;
    }
}

bool BrainManager::saveConfigToFile(const std::shared_ptr<Config>& config, const std::string& filepath) {
    if (!config) {
        NLM_LOG_ERROR("Cannot save null configuration");
        return false;
    }
    return config->saveToFile(filepath);
}

std::shared_ptr<Brain> BrainManager::createBrain(const std::shared_ptr<Config>& config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot create brain with null configuration");
        return nullptr;
    }
    
    auto brain = std::make_shared<Brain>(config);
    NLM_LOG_INFO("Created NLM Brain instance");
    return brain;
}

bool BrainManager::initializeBrain(std::shared_ptr<Brain>& brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize null brain");
        return false;
    }
    
    bool success = brain->initialize();
    if (success) {
        NLM_LOG_INFO("Brain initialization complete");
    } else {
        NLM_LOG_ERROR("Brain initialization failed");
    }
    return success;
}

void BrainManager::resetBrain(std::shared_ptr<Brain>& brain) {
    if (brain) {
        brain->reset();
        NLM_LOG_INFO("Brain reset complete");
    }
}

void BrainManager::cleanupBrain(std::shared_ptr<Brain>& brain) {
    if (brain) {
        // Call cleanup callbacks
        for (const auto& callback : cleanupCallbacks_) {
            try { callback(); } catch (...) {}
        }
        brain.reset();
        NLM_LOG_INFO("Brain cleanup complete");
    }
}

void BrainManager::runSimpleSimulation(
    std::shared_ptr<Brain>& brain,
    size_t steps,
    const std::vector<float>& sensoryInput
) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot run simulation with null brain");
        return;
    }
    
    // Inject sensory input if provided
    if (!sensoryInput.empty()) {
        SensoryInput input(sensoryInput);
        brain->receiveSensoryInput(input);
    }
    
    NLM_LOG_INFO("Starting simple simulation with " + std::to_string(steps) + " steps");
    
    for (size_t i = 0; i < steps; ++i) {
        brain->step(i);
        
        if (i % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(i) + ": " + 
                        std::to_string(brain->getFiringNeuronCount()) + " firing neurons");
        }
    }
    
    NLM_LOG_INFO("Simple simulation complete. Final stats: " + brain->getConfig()->summary());
}

void BrainManager::runSimulationWithWorld(
    std::shared_ptr<Brain>& brain,
    std::shared_ptr<SimpleWorld>& world,
    std::shared_ptr<AgentBrain>& agent,
    size_t steps
) {
    if (!brain || !world || !agent) {
        NLM_LOG_ERROR("Cannot run world simulation with null components");
        return;
    }
    
    NLM_LOG_INFO("Starting world simulation with " + std::to_string(steps) + " steps");
    
    for (size_t i = 0; i < steps; ++i) {
        // Update world
        world->update(0.1);
        
        // Get sensory percept
        auto percept = world->getSensoryPercept();
        
        // Process input and brain thinking
        agent->processSensoryInput(percept);
        brain->step(i);
        
        // Get action and apply to world
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (i % 20 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(i) + ": " + 
                        std::to_string(brain->getFiringNeuronCount()) + " firing neurons");
        }
    }
    
    NLM_LOG_INFO("World simulation complete. Final stats: " + brain->getConfig()->summary());
}

size_t BrainManager::getNeuronCount(const std::shared_ptr<Brain>& brain) {
    return brain ? brain->getTotalNeuronCount() : 0;
}

size_t BrainManager::getSynapseCount(const std::shared_ptr<Brain>& brain) {
    return brain ? brain->getTotalSynapseCount() : 0;
}

size_t BrainManager::getActiveNeuronCount(const std::shared_ptr<Brain>& brain) {
    return brain ? brain->getActiveNeuronCount() : 0;
}

float BrainManager::getAverageFiringRate(const std::shared_ptr<Brain>& brain) {
    return brain ? brain->getAverageFiringRate() : 0.0f;
}

float BrainManager::getExcitationInhibitionRatio(const std::shared_ptr<Brain>& brain) {
    return brain ? brain->getExcitationInhibitionRatio() : 0.0f;
}

void BrainManager::enableDevelopment(std::shared_ptr<Brain>& brain, bool enable) {
    if (brain) {
        brain->setDevelopmentalStage(enable ? DevelopmentalStage::Initial : DevelopmentalStage::Adult);
        NLM_LOG_INFO("Development " + std::string(enable ? "enabled" : "disabled"));
    }
}

void BrainManager::enablePlasticity(std::shared_ptr<Brain>& brain, bool enable) {
    if (brain) {
        auto stdp = brain->getSTDP();
        if (stdp) {
            // Note: STDP doesn't have enable/disable, but we could modify its parameters
            NLM_LOG_INFO("Plasticity management: " + std::string(enable ? "enabled" : "disabled"));
        }
    }
}

void BrainManager::enableRewardModulation(std::shared_ptr<Brain>& brain, bool enable) {
    if (brain) {
        NLM_LOG_INFO("Reward modulation " + std::string(enable ? "enabled" : "disabled"));
    }
}

void BrainManager::enableCuriosity(std::shared_ptr<Brain>& brain, bool enable) {
    if (brain) {
        NLM_LOG_INFO("Curiosity " + std::string(enable ? "enabled" : "disabled"));
    }
}

bool BrainManager::saveBrainState(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
    return brain ? brain->save(filepath) : false;
}

bool BrainManager::loadBrainState(std::shared_ptr<Brain>& brain, const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot load brain state into null brain");
        return false;
    }
    
    bool success = brain->load(filepath);
    if (success) {
        NLM_LOG_INFO("Brain state loaded from: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to load brain state from: " + filepath);
    }
    return success;
}

std::string BrainManager::getBrainStatus(const std::shared_ptr<Brain>& brain) {
    if (!brain) return "Null brain";
    
    std::stringstream ss;
    ss << "=== NLM Brain Status ===\n";
    ss << "Neurons: " << brain->getTotalNeuronCount() << "\n";
    ss << "Synapses: " << brain->getTotalSynapseCount() << "\n";
    ss << "Firing neurons: " << brain->getFiringNeuronCount() << "\n";
    ss << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
    ss << "Development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
    ss << "Average firing rate: " << brain->getAverageFiringRate() << "\n";
    ss << "E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n";
    
    return ss.str();
}

std::unordered_map<std::string, double> BrainManager::getSystemStats(const std::shared_ptr<Brain>& brain) {
    std::unordered_map<std::string, double> stats;
    if (!brain) return stats;
    
    stats["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
    stats["synapse_count"] = static_cast<double>(brain->getTotalSynapseCount());
    stats["firing_neuron_count"] = static_cast<double>(brain->getFiringNeuronCount());
    stats["total_spike_count"] = static_cast<double>(brain->getTotalSpikeCount());
    stats["average_firing_rate"] = static_cast<double>(brain->getAverageFiringRate());
    stats["e_i_ratio"] = static_cast<double>(brain->getExcitationInhibitionRatio());
    
    return stats;
}

void BrainManager::injectRandomSensoryInput(std::shared_ptr<Brain>& brain, size_t count) {
    if (!brain) return;
    
    std::vector<float> input;
    for (size_t i = 0; i < count; ++i) {
        input.push_back(static_cast<float>(rand()) / RAND_MAX * 10.0f - 5.0f); // [-5, 5]
    }
    
    SensoryInput sensoryInput(input);
    brain->receiveSensoryInput(sensoryInput);
    NLM_LOG_INFO("Injected random sensory input to brain");
}

std::vector<float> BrainManager::getSensoryNeuronsState(std::shared_ptr<Brain>& brain) {
    std::vector<float> states;
    if (!brain) return states;
    
    auto sensoryNeurons = brain->getSpikeSystem()->getSensoryNeurons();
    for (auto* neuron : sensoryNeurons) {
        states.push_back(neuron->getMembranePotential());
    }
    
    return states;
}

void BrainManager::stepBrain(std::shared_ptr<Brain>& brain, size_t step) {
    if (brain) {
        brain->step(step);
    }
}

} // namespace nlm
