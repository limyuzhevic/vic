#include "BrainImpl.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Config/Config.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../performance/CheckpointManager.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

BrainImpl::BrainImpl(std::shared_ptr<Config> cfg)
    : config(cfg)
    , rng(nullptr)
    , developmentalStage(DevelopmentalStage::Initial)
    , nextRegionId(1)
    , timestep(0.001)
    , currentStep(0)
    , currentTime(0.0)
    , totalSpikesThisStep(0)
    , totalSpikesTotal(0)
    , isResting(false)
    , stepsSinceLastEpisode(0)
    , replayInterval(100)
    , consolidationInterval(1000)
{
    NLM_LOG_INFO("BrainImpl: Initializing Phase 6 Integrated Brain");
    
    // Initialize random generator
    uint64_t seed = 42;
    if (auto seedOpt = config->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng = std::make_unique<RandomGenerator>(seed);
    
    // Initialize plasticity systems
    spikeSystem = std::make_unique<SpikeSystem>();
    stdp = std::make_unique<STDP>();
    hebbian = std::make_unique<Hebbian>();
    structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Initialize memory systems
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize prediction system
    predictionSystem = std::make_unique<PredictionSystem>();
    
    // Initialize cognition systems
    planner = std::make_unique<NeuralPlanner>();
    conceptFormation = std::make_unique<ConceptFormation>();
    attention = std::make_unique<AttentionalSelection>();
    
    // Initialize development system
    developmentSystem = std::make_unique<DevelopmentSystem>();
    
    // Initialize neuromodulation systems
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    predictionError = std::make_unique<PredictionError>();
    novelty = std::make_unique<Novelty>();
    
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
    
    // Get timestep
    timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Get integration intervals
    replayInterval = config->getOr<size_t>("replay_interval", 100);
    consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
    
    // Initialize checkpoint manager
    checkpointManager = std::make_unique<CheckpointManager>();
    
    NLM_LOG_INFO("BrainImpl: Phase 6 Integrated Brain initialized successfully");
}

BrainImpl::~BrainImpl() = default;

// Interface implementations for BrainImpl
bool BrainImpl::initialize() {
    NLM_LOG_INFO("BrainImpl: Initializing neural structure");
    
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("BrainImpl: Configuration - " + std::to_string(neuronCount) + 
                 " neurons, " + std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    motorNeurons.push_back(neuron);
                }
            }
        }
    }
    
    // Initialize connectivity
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // Initialize integrated systems
    workingMemory->initialize(this);
    workingMemory->setCapacity(neuronCount / 10);
    
    episodicMemory->initialize(this);
    episodicMemory->setMaxEpisodes(1000);
    
    associativeMemory->initialize(this);
    
    planner->initialize(this);
    planner->setPlanningDepth(5);
    
    conceptFormation->initialize(this);
    
    attention->initialize(this);
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
    
    novelty->initialize(this);
    curiosity->initialize(this);
    
    // Register spike handlers
    spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        ++totalSpikesThisStep;
        ++totalSpikesTotal;
    });
    
    spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        for (auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    MembranePotential synapticCurrent = event.weight * 10.0f;
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
    
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("BrainImpl: Neural structure initialized (Phase 6)");
    NLM_LOG_INFO("BrainImpl: Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("BrainImpl: Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("BrainImpl: Sensory neurons: " + std::to_string(sensoryNeurons.size()));
    NLM_LOG_INFO("BrainImpl: Motor neurons: " + std::to_string(motorNeurons.size()));
    
    return true;
}

void BrainImpl::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * timestep);
}

void BrainImpl::step(SimulationStep currentStep, Timestamp currentTime) {
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    totalSpikesThisStep = 0;
    
    // Process delayed spikes
    spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // Update neurons
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
    
    // Detect spikes
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    spikeSystem->queueSpike(event);

                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );

                        spikeSystem->queueDelayedSpike(delayedEvent);

                        syn->recordPreSpike(currentTime);
                    }

                    if (workingMemory) {
                        workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    spikeSystem->processSpikes(currentStep);
    
    // Update working memory
    if (workingMemory) {
        workingMemory->update(timestep);
    }
    
    // Apply neuromodulation
    novelty->update(timestep);
    curiosity->update(timestep);
    dopamine->update(timestep);
    
    float dopamineLevel = dopamine->getLevel();
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                float excitabilityMod = dopamineLevel * 0.5f;
                if (excitabilityMod > 0.0f) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
    
    // Apply plasticity
    float plasticityMod = 1.0f;
    if (dopamine) {
        plasticityMod = dopamine->getPlasticityFactor();
    }
    
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    stdp->update(syn, preSpikes, postSpikes, timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    hebbian->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            syn->step(currentTime);
        }
    }
    
    // Update episodic memory
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {
        stepsSinceLastEpisode = 0;
        
        if (episodicMemory) {
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
            
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.neurons.push_back(neuron->getId());
                            episode.values.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
            episodicMemory->storeEpisode(episode);
        }
    }
    
    // Update prediction system
    if (predictionSystem) {
    }
    
    // Update attention system
    if (attention) {
        attention->update(timestep);
        
        if (workingMemory && !workingMemory->getActiveTraces().empty()) {
            std::vector<NeuronId> competitors = workingMemory->getActiveTraces();
            attention->processCompetition(competitors);
        }
    }
    
    // Update concept formation
    if (conceptFormation) {
    }
    
    // Apply structural plasticity
    if (currentStep % 100 == 0) {
        structuralPlasticity->update(this, *rng);
    }
    
    // Replay memories
    if (currentStep % replayInterval == 0 && episodicMemory) {
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
    
    // Update development
    if (currentStep % 1000 == 0) {
        developmentSystem->update(this, *rng, timestep * 1000);
        
        auto* sp = structuralPlasticity.get();
        if (sp) {
            float plasticityMod = 1.0f;
            switch (developmentalStage) {
                case DevelopmentalStage::Initial: plasticityMod = 1.0f; break;
                case DevelopmentalStage::CriticalPeriod: plasticityMod = 0.8f; break;
                case DevelopmentalStage::Maturation: plasticityMod = 0.5f; break;
                case DevelopmentalStage::Adult: plasticityMod = 0.2f; break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
    
    // Memory consolidation
    if (currentStep % consolidationInterval == 0 && episodicMemory) {
        episodicMemory->consolidate(0.3f);
    }
    
    if (checkpointManager) {
        checkpointManager->update(currentStep, currentTime);
    }
}

void BrainImpl::receiveSensoryInput(const SensoryInput& input) {
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons.size();
    if (numSensory == 0) return;
    
    for (size_t i = 0; i < numSensory; ++i) {
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        if (workingMemory && normalizedValue > 0.5f) {
            workingMemory->storeToNeuron(sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void BrainImpl::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void BrainImpl::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

// Interface method implementations
SpikeSystem* BrainImpl::getSpikeSystem() { return spikeSystem.get(); }
const SpikeSystem* BrainImpl::getSpikeSystem() const { return spikeSystem.get(); }
STDP* BrainImpl::getSTDP() { return stdp.get(); }
Hebbian* BrainImpl::getHebbian() { return hebbian.get(); }
StructuralPlasticity* BrainImpl::getStructuralPlasticity() { return structuralPlasticity.get(); }

float BrainImpl::getExcitationInhibitionRatio() const {
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

size_t BrainImpl::getTotalSpikeCount() const { return totalSpikesTotal; }

// Memory system access
IWorkingMemorySystem* BrainImpl::getWorkingMemory() { 
    return static_cast<IWorkingMemorySystem*>(workingMemory.get()); 
}
IEpisodicMemorySystem* BrainImpl::getEpisodicMemory() { 
    return static_cast<IEpisodicMemorySystem*>(episodicMemory.get()); 
}
IAssociativeMemorySystem* BrainImpl::getAssociativeMemory() { 
    return static_cast<IAssociativeMemorySystem*>(associativeMemory.get()); 
}

// Prediction system
IPredictionSystem* BrainImpl::getPredictionSystem() { 
    return static_cast<IPredictionSystem*>(predictionSystem.get()); 
}

// Cognition systems
ICognitionSystem* BrainImpl::getPlanner() { 
    return static_cast<ICognitionSystem*>(planner.get()); 
}
ICognitionSystem* BrainImpl::getConceptFormation() { 
    return static_cast<ICognitionSystem*>(conceptFormation.get()); 
}
ICognitionSystem* BrainImpl::getAttention() { 
    return static_cast<ICognitionSystem*>(attention.get()); 
}

// Development system
IDevelopmentSystem* BrainImpl::getDevelopmentSystem() { 
    return static_cast<IDevelopmentSystem*>(developmentSystem.get()); 
}

// Neuromodulation systems
INeuromodulationSystem* BrainImpl::getDopamine() { 
    return static_cast<INeuromodulationSystem*>(dopamine.get()); 
}
INeuromodulationSystem* BrainImpl::getCuriosity() { 
    return static_cast<INeuromodulationSystem*>(curiosity.get()); 
}
INeuromodulationSystem* BrainImpl::getNovelty() { 
    return static_cast<INeuromodulationSystem*>(novelty.get()); 
}
INeuromodulationSystem* BrainImpl::getPredictionErrorSignal() { 
    return static_cast<INeuromodulationSystem*>(predictionError.get()); 
}

// Dynamics systems
IDynamicsSystem* BrainImpl::getSpikeSystem() { return spikeSystem.get(); }
IDynamicsSystem* BrainImpl::getSTDP() { return stdp.get(); }
IDynamicsSystem* BrainImpl::getHebbian() { return hebbian.get(); }
IPlasticitySystem* BrainImpl::getStructuralPlasticity() { return structuralPlasticity.get(); }

// Other methods
std::shared_ptr<const Config> BrainImpl::getConfig() const { return config; }
RandomGenerator* BrainImpl::getRandomGenerator() { return rng.get(); }

void BrainImpl::logStatus() const {
    NLM_LOG_INFO("=== BrainImpl Status (Phase 6) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(regions.size()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
}

RegionId BrainImpl::addRegion(const std::string& name) {
    RegionId id(nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    regions.push_back(std::move(region));
    return id;
}

NeuralRegion* BrainImpl::getRegion(RegionId id) {
    for (auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainImpl::getRegion(RegionId id) const {
    for (const auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t BrainImpl::getRegionCount() const { return regions.size(); }

std::vector<RegionId> BrainImpl::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(regions.size());
    for (const auto& region : regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainImpl::getRegions() const {
    return regions;
}

void BrainImpl::addInterRegionConnection(RegionId source, RegionId target, 
                                         float weight, Delay delay) {
    interRegionConnections.emplace_back(source, target, weight, delay);
}

void BrainImpl::removeInterRegionConnection(RegionId source, RegionId target) {
    interRegionConnections.erase(
        std::remove_if(interRegionConnections.begin(),
                      interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        interRegionConnections.end()
    );
}

size_t BrainImpl::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainImpl::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getSynapseCount();
    }
    total += interRegionConnections.size();
    return total;
}

size_t BrainImpl::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t BrainImpl::getFiringNeuronCount() const {
    return totalSpikesThisStep;
}

float BrainImpl::getAverageFiringRate() const {
    if (regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(regions.size());
}

std::unique_ptr<Action> BrainImpl::produceAction() {
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    size_t firingMotor = 0;
    for (auto* neuron : motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    return action;
}

void BrainImpl::applyNeuromodulation(const Neuromodulator& signal) {
    float modulation = signal.getLevel();
    stdp->setLTPWeight(0.01f * modulation);
    stdp->setLTDWeight(0.012f * modulation);
}

void BrainImpl::updatePlasticity() {
}

void BrainImpl::develop() {
    structuralPlasticity->update(this, *rng);
}

void BrainImpl::reset() {
    NLM_LOG_INFO("BrainImpl: Resetting neural system");
    
    currentStep = 0;
    currentTime = 0.0;
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    isResting = false;
    stepsSinceLastEpisode = 0;
    
    for (auto& region : regions) {
        region->reset();
    }
    
    spikeSystem->reset();
    developmentalStage = DevelopmentalStage::Initial;
    
    if (workingMemory) workingMemory->clear();
    if (episodicMemory) episodicMemory->clear();
    if (associativeMemory) associativeMemory->clear();
    if (attention) attention->reset();
    
    NLM_LOG_INFO("BrainImpl: Neural system reset complete");
}

bool BrainImpl::save(const std::string& filepath) const {
    NLM_LOG_INFO("BrainImpl: Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("BrainImpl: Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            currentStep,
            currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    neuronData.membranePotential.push_back(state.membranePotential);
                    neuronData.restingPotential.push_back(state.restingPotential);
                    neuronData.threshold.push_back(state.threshold);
                    neuronData.resetPotential.push_back(state.resetPotential);
                    neuronData.leakConductance.push_back(state.leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("BrainImpl: Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("BrainImpl: Failed to write synapses to checkpoint");
            return false;
        }
        
        if (!writer.finalize()) {
            NLM_LOG_ERROR("BrainImpl: Failed to finalize checkpoint");
        }
        
        NLM_LOG_INFO("BrainImpl: Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("BrainImpl: Exception saving brain: ") + e.what());
        throw nlm::NeuralError(std::string("BrainImpl save failed: ") + e.what(), "save", "Checkpoint save operation");
    }
}

bool BrainImpl::load(const std::string& filepath) {
    NLM_LOG_INFO("BrainImpl: Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("BrainImpl: Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("BrainImpl: Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("BrainImpl: Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (idx < neuronData.membranePotential.size()) {
                        neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        neuron->setThreshold(neuronData.threshold[idx]);
                        neuron->setResetPotential(neuronData.resetPotential[idx]);
                        neuron->setLeakConductance(neuronData.leakConductance[idx]);
                        if (idx < neuronData.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                        }
                        if (idx < neuronData.refractoryRemaining.size()) {
                            neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                        }
                    }
                    idx++;
                }
            }
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("BrainImpl: Failed to read synapses from checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("BrainImpl: Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("BrainImpl: Exception loading brain: ") + e.what());
        throw nlm::NeuralError(std::string("BrainImpl load failed: ") + e.what(), "load", "Checkpoint load operation");
    }
}

} // namespace nlm
