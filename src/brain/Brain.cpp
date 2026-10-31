// Enhanced Brain implementation with component architecture
#include "BrainComponents.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    std::shared_ptr<Logger> logger;
    
    // System components
    std::unique_ptr<StepManager> stepManager;
    std::unique_ptr<PlasticityManager> plasticityManager;
    std::unique_ptr<MemoryManager> memoryManager;
    std::unique_ptr<NeuromodulationManager> neuromodulationManager;
    std::unique_ptr<DevelopmentManager> developmentManager;
    std::unique_ptr<SensoryMotorManager> sensoryMotorManager;
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Original brain components (for backward compatibility)
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg)
        , rng(std::make_shared<RandomGenerator>(42))
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
    {
        // Initialize logger
        logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Initialize component instances
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Configure plasticity systems
        float ltpWeight = 0.01f;
        float ltdWeight = 0.012f;
        float tau = 20.0f;
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        structuralPlasticity->setSynaptogenesisRate(0.0001f);
        structuralPlasticity->setPruningRate(0.00001f);
        
        timestep = 0.001;
        
        // Create component instances
        stepManager = std::make_unique<StepManager>(std::shared_ptr<Brain>(this, [](Brain*) {}), BrainSystemConfig());
        plasticityManager = std::make_unique<PlasticityManager>(std::shared_ptr<Brain>(this, [](Brain*) {}), BrainSystemConfig());
        memoryManager = std::make_unique<MemoryManager>(std::shared_ptr<Brain>(this, [](Brain*) {}), BrainSystemConfig());
        neuromodulationManager = std::make_unique<NeuromodulationManager>(std::shared_ptr<Brain>(this, [](Brain*) {}), BrainSystemConfig());
        developmentManager = std::make_unique<DevelopmentManager>(std::shared_ptr<Brain>(this, [](Brain*) {}), BrainSystemConfig());
        sensoryMotorManager = std::make_unique<SensoryMotorManager>(std::shared_ptr<Brain>(this, [](Brain*) {}), BrainSystemConfig());
        checkpointManager = std::make_unique<CheckpointManager>(std::shared_ptr<Brain>(this, [](Brain*) {}), BrainSystemConfig());
    }
    
    ~Impl() {
        // Clean up components in reverse order
        checkpointManager.reset();
        sensoryMotorManager.reset();
        developmentManager.reset();
        neuromodulationManager.reset();
        memoryManager.reset();
        plasticityManager.reset();
        stepManager.reset();
        structuralPlasticity.reset();
        hebbian.reset();
        stdp.reset();
        spikeSystem.reset();
        regions.clear();
        interRegionConnections.clear();
        rng.reset();
        config.reset();
        logger.reset();
    }
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing enhanced Brain (Phase 6: Integrated Component Architecture)...");
    
    // Initialize components
    if (!pImpl->stepManager->initialize()) {
        NLM_LOG_ERROR("Failed to initialize step manager!");
        return false;
    }
    
    if (!pImpl->plasticityManager->initialize()) {
        NLM_LOG_ERROR("Failed to initialize plasticity manager!");
        return false;
    }
    
    if (!pImpl->memoryManager->initialize()) {
        NLM_LOG_ERROR("Failed to initialize memory manager!");
        return false;
    }
    
    if (!pImpl->neuromodulationManager->initialize()) {
        NLM_LOG_ERROR("Failed to initialize neuromodulation manager!");
        return false;
    }
    
    if (!pImpl->developmentManager->initialize()) {
        NLM_LOG_ERROR("Failed to initialize development manager!");
        return false;
    }
    
    if (!pImpl->sensoryMotorManager->initialize()) {
        NLM_LOG_ERROR("Failed to initialize sensory-motor manager!");
        return false;
    }
    
    if (!pImpl->checkpointManager->initialize()) {
        NLM_LOG_ERROR("Failed to initialize checkpoint manager!");
        return false;
    }
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
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
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    NLM_LOG_INFO("Enhanced Brain initialization complete (Phase 6 - Component Architecture)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== PHASE 1: COMPONENT COORDINATION ==========
    
    // Update all components
    pImpl->stepManager->update();
    pImpl->memoryManager->update();
    pImpl->neuromodulationManager->update();
    pImpl->developmentManager->update();
    pImpl->checkpointManager->update();
    
    // ========== PHASE 2: NEURAL COMPUTATION ==========
    
    // Step 1: Process pending delayed spikes (deliver synaptic input)
    pImpl->stepManager->processDelayedSpikes(currentStep, currentTime);
    
    // Step 2: Update all neurons (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // Step 3: Detect spikes and schedule spike events
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);
                
                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);
                    
                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }
                    
                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;
                        
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
                        
                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                        
                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->stepManager->processSpikes(currentStep);
    
    // ========== PHASE 3: PLASTICITY PROCESSING ==========
    
    // Apply STDP with neuromodulation
    float plasticityMod = 1.0f;
    if (pImpl->neuromodulationManager->getDopamineLevel() > 0.0f) {
        plasticityMod = pImpl->neuromodulationManager->getDopamineLevel();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== PHASE 4: MEMORY PROCESSING ==========
    
    // Update working memory
    pImpl->memoryManager->updateWorkingMemory(currentStep, currentTime);
    
    // Update episodic memory
    pImpl->memoryManager->updateEpisodicMemory(currentStep, currentTime);
    
    // Update associative memory
    pImpl->memoryManager->updateAssociativeMemory(currentStep, currentTime);
    
    // ========== PHASE 5: NEUROMODULATION PROCESSING ==========
    
    // Update neuromodulation systems
    pImpl->neuromodulationManager->updateDopamine(currentStep, currentTime);
    pImpl->neuromodulationManager->updateCuriosity(currentStep, currentTime);
    pImpl->neuromodulationManager->updateNovelty(currentStep, currentTime);
    pImpl->neuromodulationManager->updatePredictionError(currentStep, currentTime);
    
    // Apply neuromodulation effects
    pImpl->neuromodulationManager->applyDopamineEffects(currentStep, currentTime);
    pImpl->neuromodulationManager->applyCuriosityEffects(currentStep, currentTime);
    
    // ========== PHASE 6: DEVELOPMENT PROCESSING ==========
    
    // Apply development effects
    pImpl->developmentManager->applyDevelopment(currentStep, currentTime);
    
    // ========== PHASE 7: STRUCTURAL PLASTICITY ==========
    
    // Apply structural plasticity periodically
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== PHASE 8: MEMORY REPLAY ==========
    
    // Replay important memories
    if (currentStep % pImpl->replayInterval == 0) {
        pImpl->memoryManager->replayMemory(currentStep, currentTime);
    }
    
    // ========== PHASE 9: MEMORY CONSOLIDATION ==========
    
    // Periodic memory consolidation
    if (currentStep % pImpl->consolidationInterval == 0) {
        pImpl->memoryManager->consolidateMemory(currentStep, currentTime);
    }
    
    // ========== PHASE 10: CHECKPOINT MANAGEMENT ==========
    
    pImpl->checkpointManager->update(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    pImpl->sensoryMotorManager->processSensoryInput(input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    pImpl->sensoryMotorManager->injectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    pImpl->sensoryMotorManager->injectCurrentToNeurons(type, current);
}

// Component access methods
StepManager* Brain::getStepManager() { return pImpl->stepManager.get(); }
const StepManager* Brain::getStepManager() const { return pImpl->stepManager.get(); }

PlasticityManager* Brain::getPlasticityManager() { return pImpl->plasticityManager.get(); }
const PlasticityManager* Brain::getPlasticityManager() const { return pImpl->plasticityManager.get(); }

MemoryManager* Brain::getMemoryManager() { return pImpl->memoryManager.get(); }
const MemoryManager* Brain::getMemoryManager() const { return pImpl->memoryManager.get(); }

NeuromodulationManager* Brain::getNeuromodulationManager() { return pImpl->neuromodulationManager.get(); }
const NeuromodulationManager* Brain::getNeuromodulationManager() const { return pImpl->neuromodulationManager.get(); }

DevelopmentManager* Brain::getDevelopmentManager() { return pImpl->developmentManager.get(); }
const DevelopmentManager* Brain::getDevelopmentManager() const { return pImpl->developmentManager.get(); }

SensoryMotorManager* Brain::getSensoryMotorManager() { return pImpl->sensoryMotorManager.get(); }
const SensoryMotorManager* Brain::getSensoryMotorManager() const { return pImpl->sensoryMotorManager.get(); }

CheckpointManager* Brain::getCheckpointManager() { return pImpl->checkpointManager.get(); }
const CheckpointManager* Brain::getCheckpointManager() const { return pImpl->checkpointManager.get(); }

// Original Brain interface methods (delegated to components)
SpikeSystem* Brain::getSpikeSystem() { return pImpl->spikeSystem.get(); }
const SpikeSystem* Brain::getSpikeSystem() const { return pImpl->spikeSystem.get(); }

STDP* Brain::getSTDP() { return pImpl->stdp.get(); }
Hebbian* Brain::getHebbian() { return pImpl->hebbian.get(); }
StructuralPlasticity* Brain::getStructuralPlasticity() { return pImpl->structuralPlasticity.get(); }

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
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

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    return pImpl->sensoryMotorManager->generateAction();
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
}

void Brain::updatePlasticity() {
    // Plasticity is applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting enhanced Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->memoryManager) pImpl->memoryManager->reset();
    if (pImpl->neuromodulationManager) pImpl->neuromodulationManager->reset();
    if (pImpl->developmentManager) pImpl->developmentManager->reset();
    if (pImpl->sensoryMotorManager) pImpl->sensoryMotorManager->reset();
    
    NLM_LOG_INFO("Enhanced Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    return pImpl->checkpointManager->save(filepath);
}

bool Brain::load(const std::string& filepath) {
    return pImpl->checkpointManager->load(filepath);
}

// Continue with remaining Brain methods (region management, statistics, etc.)

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// Memory system accessors
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->memoryManager->getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->memoryManager->getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->memoryManager->getAssociativeMemory();
}

// Prediction system accessor
PredictionSystem* Brain::getPredictionSystem() {
    // Would be implemented based on actual PredictionSystem
    return nullptr;
}

// Cognition system accessors
NeuralPlanner* Brain::getPlanner() {
    return pImpl->developmentManager->getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->developmentManager->getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->developmentManager->getAttention();
}

// Development system
DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentManager->getDevelopmentSystem();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentManager->getDevelopmentalStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentManager->setDevelopmentalStage(stage);
}

// Neuromodulation systems
Dopamine* Brain::getDopamine() {
    return pImpl->neuromodulationManager->getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->neuromodulationManager->getCuriosity();
}

Novelty* Brain::getNovelty() {
    return pImpl->neuromodulationManager->getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->neuromodulationManager->getPredictionError();
}

// Configuration and random generator
std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== Enhanced Brain Status (Phase 6 - Component Architecture) ===");
    
    // Log component status
    if (pImpl->stepManager) pImpl->stepManager->logStatus();
    if (pImpl->plasticityManager) pImpl->plasticityManager->logStatus();
    if (pImpl->memoryManager) pImpl->memoryManager->logStatus();
    if (pImpl->neuromodulationManager) pImpl->neuromodulationManager->logStatus();
    if (pImpl->developmentManager) pImpl->developmentManager->logStatus();
    if (pImpl->sensoryMotorManager) pImpl->sensoryMotorManager->logStatus();
    if (pImpl->checkpointManager) pImpl->checkpointManager->logStatus();
    
    // Log original brain status
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
}

} // namespace nlm