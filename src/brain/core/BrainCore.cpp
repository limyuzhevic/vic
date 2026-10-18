#include "brain/core/BrainCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

bool BrainCore::validateConfiguration(const std::shared_ptr<const Config>& config) {
    if (!config) {
        NLM_LOG_ERROR("Configuration is null");
        return false;
    }
    
    // Validate essential parameters
    bool valid = true;
    
    auto neuronCount = config->getOr<size_t>("neuron_count", 1000);
    if (neuronCount == 0 || neuronCount > 1000000) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount));
        valid = false;
    }
    
    auto regionCount = config->getOr<size_t>("region_count", 1);
    if (regionCount == 0 || regionCount > 1000) {
        NLM_LOG_ERROR("Invalid region_count: " + std::to_string(regionCount));
        valid = false;
    }
    
    auto timestep = config->getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0 || timestep > 1.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep));
        valid = false;
    }
    
    return valid;
}

std::string BrainCore::generateRegionName(size_t index) {
    return "Region_" + std::to_string(index + 1);
}

void BrainCore::updateStatistics(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                size_t& totalSpikes,
                                size_t& totalSynapses) {
    totalSpikes = 0;
    totalSynapses = 0;
    
    for (const auto& region : regions) {
        totalSpikes += region->getActiveNeuronCount();
        totalSynapses += region->getSynapseCount();
    }
}

float BrainCore::calculateExcitatoryInhibitionRatio(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t BrainCore::countActiveNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

void BrainCore::initializeMemorySystems(std::unique_ptr<NeuralWorkingMemory>& workingMemory,
                                         std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                                         std::unique_ptr<NeuralAssociativeMemory>& associativeMemory,
                                         const std::shared_ptr<const Config>& config,
                                         const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    size_t neuronCount = 0;
    for (const auto& region : regions) {
        neuronCount += region->getTotalNeuronCount();
    }
    
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Memory system configuration would go here
    workingMemory->setCapacity(neuronCount / 10);
    episodicMemory->setMaxEpisodes(1000);
    // Add more configuration as needed
    
    NLM_LOG_INFO("Memory systems initialized: working memory capacity = " + std::to_string(neuronCount / 10));
}

void BrainCore::initializePredictionSystem(std::unique_ptr<PredictionSystem>& predictionSystem,
                                           const std::shared_ptr<const Config>& config) {
    predictionSystem = std::make_unique<PredictionSystem>();
    NLM_LOG_INFO("Prediction system initialized");
}

void BrainCore::initializeCognitionSystems(std::unique_ptr<NeuralPlanner>& planner,
                                           std::unique_ptr<ConceptFormation>& conceptFormation,
                                           std::unique_ptr<AttentionalSelection>& attention,
                                           const std::shared_ptr<const Config>& config,
                                           const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    planner = std::make_unique<NeuralPlanner>();
    conceptFormation = std::make_unique<ConceptFormation>();
    attention = std::make_unique<AttentionalSelection>();
    
    planner->setPlanningDepth(5);
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
    
    NLM_LOG_INFO("Cognition systems initialized: planner depth = 5, attention weights configured");
}

void BrainCore::initializeNeuromodulationSystems(std::unique_ptr<Dopamine>& dopamine,
                                                 std::unique_ptr<Curiosity>& curiosity,
                                                 std::unique_ptr<PredictionError>& predictionError,
                                                 std::unique_ptr<Novelty>& novelty,
                                                 const std::shared_ptr<const Config>& config) {
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    predictionError = std::make_unique<PredictionError>();
    novelty = std::make_unique<Novelty>();
    
    NLM_LOG_INFO("Neuromodulation systems initialized");
}

void BrainCore::initializeDevelopmentSystem(std::unique_ptr<DevelopmentSystem>& developmentSystem,
                                           const std::shared_ptr<const Config>& config) {
    developmentSystem = std::make_unique<DevelopmentSystem>();
    NLM_LOG_INFO("Development system initialized");
}

void BrainCore::initializePlasticitySystems(std::unique_ptr<SpikeSystem>& spikeSystem,
                                           std::unique_ptr<STDP>& stdp,
                                           std::unique_ptr<Hebbian>& hebbian,
                                           std::unique_ptr<StructuralPlasticity>& structuralPlasticity,
                                           const std::shared_ptr<const Config>& config) {
    spikeSystem = std::make_unique<SpikeSystem>();
    stdp = std::make_unique<STDP>();
    hebbian = std::make_unique<Hebbian>();
    structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Configure STDP parameters
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity->setPruningRate(pruningRate);
    
    // Register spike handlers
    spikeSystem->registerHandler([](const DetailedSpikeEvent& event) {
        // Basic spike counting - can be expanded
    });
    
    NLM_LOG_INFO("Plasticity systems initialized: STDP and Hebbian configured");
}

void BrainCore::configureCheckpointSystem(std::unique_ptr<CheckpointManager>& checkpointManager,
                                         const std::shared_ptr<const Config>& config) {
    checkpointManager = std::make_unique<CheckpointManager>();
    
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("Checkpoint system configured: " + checkpointDir);
}

void BrainCore::distributeSensoryNeurons(std::vector<Neuron*>& sensoryNeurons,
                                        std::vector<Neuron*>& motorNeurons,
                                        const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    sensoryNeurons.clear();
    motorNeurons.clear();
    
    for (const auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            } else if (type == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

std::string BrainCore::formatStatistics(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                       const std::shared_ptr<const Config>& config,
                                       size_t totalSpikes,
                                       size_t totalSynapses,
                                       const SpikeSystem* spikeSystem,
                                       const Dopamine* dopamine,
                                       DevelopmentalStage developmentalStage) {
    std::ostringstream oss;
    oss << "=== Brain Core Statistics ===\n";
    oss << "Region count: " << regions.size() << "\n";
    oss << "Total neurons: ";
    size_t totalNeurons = 0;
    for (const auto& region : regions) {
        totalNeurons += region->getTotalNeuronCount();
    }
    oss << totalNeurons << "\n";
    oss << "Total synapses: " << totalSynapses << "\n";
    oss << "Total spikes: " << totalSpikes << "\n";
    oss << "Active neurons: " << countActiveNeurons(regions) << "\n";
    oss << "Development stage: " << static_cast<int>(developmentalStage) << "\n";
    
    if (spikeSystem) {
        oss << "Pending spike events: " << spikeSystem->getPendingSpikeCount() << "\n";
    }
    
    if (dopamine) {
        oss << "Dopamine level: " << dopamine->getLevel() << "\n";
    }
    
    return oss.str();
}

} // namespace nlm
