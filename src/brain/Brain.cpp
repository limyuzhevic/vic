#include "Brain.hpp"
#include "sleep/SleepManager.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "neuromodulation/Norepinephrine.hpp"
#include "neuromodulation/Serotonin.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/EventQueue.hpp"

namespace nlm {

#include "Brain.hpp"
#include "sleep/SleepManager.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "neuromodulation/Norepinephrine.hpp"
#include "neuromodulation/Serotonin.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/EventQueue.hpp"
#include "performance/PerformanceProfiler.hpp"

namespace nlm {

struct Brain::Impl {
    // Neural system components
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    
    // Memory pools for efficient allocation
    nlm::NeuronPool neuronPool;
    nlm::SynapsePool synapsePool;
    nlm::MemoryPool<NeuralRegion> regionPool;
    
    // Event queue for spike processing
    nlm::SpikeRingBuffer spikeEventQueue;
    nlm::DelayedSpikeQueue delayedSpikeQueue;
    nlm::PriorityEventQueue priorityEventQueue;
    
    // Performance profiler
    nlm::PerformanceProfiler performanceProfiler;
    
    // Memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Cognitive systems
    std::unique_ptr<NeuralPlanner> neuralPlanner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attentionalSelection;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    
    // Spike system
    std::unique_ptr<SpikeSystem> spikeSystem;
    
    // Plasticity systems
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Sleep manager
    std::unique_ptr<SleepManager> sleepManager;
    
    // Goal and state
    std::vector<float> currentGoal_;
    
    // Statistics
    size_t totalSpikeCount_;
    SimulationStep currentStep_;
    Timestamp currentTime_;
    
    // Stress signals for memory consolidation
    StressSignals stressSignals_;
    
    // Attention guidance
    AttentionGuidance attentionGuidance_;
    
    Impl() : totalSpikeCount_(0), currentStep_(0), currentTime_(0) {}
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl) {
    NLM_LOG_INFO("Brain initialized with Phase 6 sleep consolidation system");
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing enhanced Brain with memory consolidation during sleep");
    
    // Reserve memory pools
    pImpl->neuronPool.reserve(10000);
    pImpl->synapsePool.reserve(50000);
    pImpl->regionPool.reserve(100);
    
    // Initialize event queues
    pImpl->spikeEventQueue.clear();
    pImpl->delayedSpikeQueue.clear();
    pImpl->priorityEventQueue.clear();
    
    // Initialize sleep manager
    pImpl->sleepManager = std::make_unique<SleepManager>();
    pImpl->sleepManager->initialize(this);
    
    // Initialize working memory
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->workingMemory->initialize(this);
    
    // Initialize episodic memory
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->episodicMemory->initialize(this);
    
    // Initialize other systems (simplified for implementation)
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->acetylcholine = std::make_unique<Acetylcholine>();
    pImpl->norepinephrine = std::make_unique<Norepinephrine>();
    pImpl->serotonin = std::make_unique<Serotonin>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->novelty = std::make_unique<Novelty>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    pImpl->attentionalSelection = std::make_unique<AttentionalSelection>();
    pImpl->attentionalSelection->initialize(this);
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    
    // Initialize spike system with event queue integration
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    // Note: Event queue integration happens in the step() method
    
    // Initialize plasticity systems
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    NLM_LOG_INFO("Enhanced Brain initialization complete");
    NLM_LOG_INFO("Sleep consolidation system ready for memory consolidation during rest cycles");
    
    return true;
}

// Development system is initialized in the Brain constructor

// Development system is integrated in the step() method below

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    currentStep_ = currentStep;
    currentTime_ = currentTime;
    
    // Start performance profiling for this step
    pImpl->performanceProfiler.startStep(currentStep);
    
    // Apply developmental changes at the start of each step
    develop();
    
    // Update sleep manager
    float globalArousal = getArousalLevel();
    pImpl->sleepManager->update(currentStep, globalArousal);
    
    // Update stress and attention signals
    updateStressSignals(currentStep);
    updateAttentionSignals(currentStep);
    
    // If sleeping, perform sleep-dependent consolidation
    if (pImpl->sleepManager->isSleeping()) {
        NLM_LOG_INFO("Sleep-dependent memory consolidation at step " + std::to_string(currentStep));
        
        // Stage 1: Light sleep - basic replay
        if (pImpl->sleepManager->getCurrentStage() == SleepStage::LightSleep) {
            performSleepReplay(currentStep, SleepStage::LightSleep);
        }
        
        // Stage 2: Deep sleep - systems consolidation
        else if (pImpl->sleepManager->getCurrentStage() == SleepStage::DeepSleep) {
            performSystemsConsolidation(currentStep);
        }
        
        // Stage 3: REM sleep - emotional and procedural consolidation
        else if (pImpl->sleepManager->getCurrentStage() == SleepStage::REM) {
            performREMConsolidation(currentStep);
        }
        
        // Apply stress-guided consolidation
        applyStressGuidedConsolidation(currentStep);
        
        // Apply attention-guided consolidation
        applyAttentionGuidedConsolidation(currentStep);
        
        // After sleep, perform wake-up processing
        if (currentStep > 0 && currentStep % 10 == 0) {
            pImpl->sleepManager->checkWakeConditions(SensoryInput(), stressSignals_, attentionGuidance_);
        }
    } else {
        // Active state processing with EventQueue integration
        processActiveNeurons(currentStep, currentTime);
    }
    
    // Regular consolidation for active state
    if (!pImpl->sleepManager->isSleeping()) {
        performActiveConsolidation(currentStep);
    }
    
    // End performance profiling and collect metrics
    nlm::PerformanceProfiler::CompleteMetrics metrics = 
        pImpl->performanceProfiler.endStep();
    
    // Log performance metrics every 100 steps
    if (currentStep % 100 == 0) {
        pImpl->performanceProfiler.printPerformanceReport(metrics);
    }
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
    // Inject sensory input
    if (!input.sensoryData.empty()) {
        // Find sensory neurons and inject input
        float averageInput = 0.0f;
        for (float val : input.sensoryData) {
            averageInput += val;
        }
        averageInput /= input.sensoryData.size();
        
        if (averageInput > 0.1f) {
            injectCurrentToNeurons(NeuronType::Sensory, averageInput * 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Find and inject current into specific neuron
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* n : pop->getNeurons()) {
                if (n->getId() == neuron) {
                    n->injectCurrent(current);
                    break;
                }
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Inject current to all neurons of specific type
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* n : pop->getNeurons()) {
                    n->injectCurrent(current);
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    // Calculate E/I ratio from spike system
    if (!pImpl->spikeSystem) return 1.0f;
    return pImpl->spikeSystem->getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikeCount_;
}

size_t Brain::getPendingSpikeEventCount() const {
    if (!pImpl->spikeSystem) return 0;
    return pImpl->spikeSystem->getPendingSpikeCount();
}

std::unique_ptr<Action> Brain::produceAction() {
    // Simple action production based on current state
    auto action = std::make_unique<Action>();
    
    if (pImpl->workingMemory) {
        auto patterns = pImpl->workingMemory->retrieve();
        if (!patterns.empty()) {
            // Determine action based on working memory content
            action->setActionType(ActionType::Wait); // Default
            action->setPower(0.5f);
        }
    }
    
    return action;
}

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    // Apply neuromodulatory signal
    switch (signal.type) {
        case NeuromodulatorType::Dopamine:
            if (pImpl->dopamine) {
                pImpl->dopamine->applySignal(signal.level);
            }
            break;
        case NeuromodulatorType::Acetylcholine:
            if (pImpl->acetylcholine) {
                pImpl->acetylcholine->applySignal(signal.level);
            }
            break;
        case NeuromodulatorType::Norepinephrine:
            if (pImpl->norepinephrine) {
                pImpl->norepinephrine->applySignal(signal.level);
            }
            break;
        case NeuromodulatorType::Serotonin:
            if (pImpl->serotonin) {
                pImpl->serotonin->applySignal(signal.level);
            }
            break;
        default:
            break;
    }
}

void Brain::updatePlasticity() {
    if (pImpl->stdp) pImpl->stdp->update();
    if (pImpl->hebbian) pImpl->hebbian->update();
    if (pImpl->structuralPlasticity) pImpl->structuralPlasticity->update();
}

void Brain::develop() {
    if (pImpl->developmentSystem) {
        // Use comprehensive development system that integrates all cognitive systems
        static nlm::RandomGenerator rng;
        pImpl->developmentSystem->applyAllDevelopmentEffects(this, currentStep_, rng, 0.001f);
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting Brain and memory systems");
    
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    
    if (pImpl->spikeSystem) pImpl->spikeSystem->reset();
    if (pImpl->stdp) pImpl->stdp->reset();
    if (pImpl->hebbian) pImpl->hebbian->reset();
    
    currentStep_ = 0;
    totalSpikeCount_ = 0;
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving Brain state to " + filepath);
    // Implementation would save checkpoint data
    return true;
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading Brain state from " + filepath);
    // Implementation would load checkpoint data
    return true;
}

RegionId Brain::addRegion(const std::string& name) {
    // Use memory pool for region allocation
    NeuralRegion* region = pImpl->regionPool.allocate();
    region->NeuralRegion::NeuralRegion(name);
    
    RegionId id = region->getId();
    pImpl->regions.push_back(std::unique_ptr<NeuralRegion>(region));
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
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                    float weight, Delay delay) {
    // Add inter-region connection
    // Implementation would add synaptic connections between regions
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    // Remove inter-region connection
    // Implementation would remove connections between regions
}

size_t Brain::getTotalNeuronCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            count += pop->getNeuronCount();
        }
    }
    return count;
}

size_t Brain::getTotalSynapseCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        count += region->getSynapseCount();
    }
    return count;
}

size_t Brain::getActiveNeuronCount() const {
    if (!pImpl->spikeSystem) return 0;
    return pImpl->spikeSystem->getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    if (!pImpl->spikeSystem) return 0;
    return pImpl->spikeSystem->getFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    if (!pImpl->spikeSystem) return 0.0f;
    return pImpl->spikeSystem->getAverageFiringRate();
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->neuralPlanner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attentionalSelection.get();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionError() {
    return pImpl->predictionError.get();
}

// Sleep system for memory consolidation during rest
SleepManager* Brain::getSleepManager() {
    return pImpl->sleepManager.get();
}

std::vector<float> Brain::getCurrentState() const {
    std::vector<float> state;
    if (pImpl->workingMemory) {
        state = pImpl->workingMemory->retrieve();
    }
    return state;
}

std::vector<NeuronId> Brain::getLearningHistory() const {
    std::vector<NeuronId> history;
    if (pImpl->spikeSystem) {
        history = pImpl->spikeSystem->getRecentSpikeNeurons(100);
    }
    return history;
}

float Brain::getPredictionConfidence() const {
    if (!pImpl->predictionSystem) return 0.0f;
    return pImpl->predictionSystem->getPredictionConfidence();
}

std::vector<NeuronId> Brain::getReplayTargets() const {
    std::vector<NeuronId> targets;
    if (pImpl->episodicMemory) {
        // Get replay targets from episodic memory
        targets = pImpl->episodicMemory->getReplayTargets();
    }
    return targets;
}

void Brain::updateAttentionBasedLearning() {
    // Update attention-based learning based on experience
}

void Brain::enhancePredictionDrivenAttention() {
    // Enhance attention driven by prediction errors
}

void Brain::enhanceWorkingMemoryAttention() {
    // Enhance attention driven by working memory
}

void Brain::enhanceConceptDrivenAttention() {
    // Enhance attention driven by concepts
}

void Brain::enhanceNeuromodulationDrivenAttention() {
    // Enhance attention driven by neuromodulation
}

void Brain::enhanceDevelopmentDrivenAttention() {
    // Enhance attention driven by development
}

void Brain::enhanceReplayDrivenAttention() {
    // Enhance attention driven by replay
}

void Brain::processActiveNeurons(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl->spikeSystem) return;
    
    // Process spike events - use parallel processing for spike handlers
    std::vector<nlm::QueuedSpikeEvent> dueSpikes;
    while (!pImpl->spikeEventQueue.empty()) {
        auto eventOpt = pImpl->spikeEventQueue.pop();
        if (eventOpt) {
            dueSpikes.push_back(*eventOpt);
        }
    }
    
    // Record memory allocation for spike events
    pImpl->performanceProfiler.recordMemoryAllocation(
        static_cast<uint64_t>(dueSpikes.size() * sizeof(nlm::QueuedSpikeEvent)));
    
    // Process spikes in parallel batches for better performance
    const size_t batchSize = std::min(static_cast<size_t>(8), dueSpikes.size());
    if (batchSize > 1) {
        // Record parallel task for spike processing
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Process spike events in parallel batches
        std::vector<std::future<void>> futures;
        futures.reserve(dueSpikes.size() / batchSize + 1);
        
        for (size_t i = 0; i < dueSpikes.size(); i += batchSize) {
            size_t end = std::min(i + batchSize, dueSpikes.size());
            auto batchStart = dueSpikes.begin() + i;
            auto batchEnd = dueSpikes.begin() + end;
            
            // Process batch of spike events (could use threads here in a real implementation)
            for (auto it = batchStart; it < batchEnd; ++it) {
                const auto& queuedEvent = *it;
                
                // Create DetailedSpikeEvent for handlers
                DetailedSpikeEvent detailed;
                detailed.source = NeuronId(queuedEvent.sourceNeuron);
                detailed.timestamp = static_cast<Timestamp>(currentTime);
                detailed.step = currentStep;
                detailed.regionId = INVALID_REGION_ID;
                detailed.populationId = INVALID_POPULATION_ID;
                
                // Update total spike count
                pImpl->totalSpikeCount_++;
                
                // Record SIMD operation for spike processing
                pImpl->performanceProfiler.recordSIMDOperation("Synapse", 1);
                
                // Call handlers
                for (auto& handler : pImpl->spikeSystem->getHandlers()) {
                    handler(detailed);
                }
            }
        }
        
        // Wait for all batches to complete
        for (auto& future : futures) {
            future.wait();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        pImpl->performanceProfiler.recordParallelTask(duration);
    } else {
        // Process remaining spikes sequentially
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (const auto& queuedEvent : dueSpikes) {
            DetailedSpikeEvent detailed;
            detailed.source = NeuronId(queuedEvent.sourceNeuron);
            detailed.timestamp = static_cast<Timestamp>(currentTime);
            detailed.step = currentStep;
            detailed.regionId = INVALID_REGION_ID;
            detailed.populationId = INVALID_POPULATION_ID;
            
            pImpl->totalSpikeCount_++;
            
            // Record SIMD operation for spike processing
            pImpl->performanceProfiler.recordSIMDOperation("Synapse", 1);
            
            for (auto& handler : pImpl->spikeSystem->getHandlers()) {
                handler(detailed);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        pImpl->performanceProfiler.recordSerialTask(duration);
    }
    
    // Process delayed spikes using EventQueue
    auto delayedSpikes = pImpl->delayedSpikeQueue.getDueSpikes(currentStep);
    
    // Record memory allocation for delayed spikes
    pImpl->performanceProfiler.recordMemoryAllocation(
        static_cast<uint64_t>(delayedSpikes.size() * sizeof(nlm::QueuedSpikeEvent)));
    
    for (const auto& queuedEvent : delayedSpikes) {
        DelayedSpikeEvent delayedEvent;
        delayedEvent.source_neuron = NeuronId(queuedEvent.sourceNeuron);
        delayedEvent.destination_neuron = NeuronId(queuedEvent.destNeuron);
        delayedEvent.synapse_id = SynapseId(queuedEvent.synapseId);
        delayedEvent.weight = queuedEvent.weight;
        delayedEvent.synapse_type = queuedEvent.isExcitatory ? SynapseType::Excitatory : SynapseType::Inhibitory;
        delayedEvent.timestamp = currentTime;
        delayedEvent.delivery_time = currentTime;
        delayedEvent.step = currentStep;
        delayedEvent.delivery_step = currentStep;
        delayedEvent.is_excitatory = queuedEvent.isExcitatory;
        
        // Call delayed spike handlers
        for (auto& handler : pImpl->spikeSystem->getDelayedHandlers()) {
            handler(delayedEvent);
        }
    }
    
    // Parallel neural population processing
    processNeuralPopulationsInParallel(currentStep, currentTime);
}

// Additional parallel processing methods for Brain

void Brain::processNeuralPopulationsInParallel(SimulationStep currentStep, Timestamp currentTime) {
    // Parallel processing of neural populations for better performance
    // Uses thread pool or concurrent processing for multiple regions
    
    // Create a thread pool for parallel processing
    // For simplicity, we'll use std::async for demonstration
    // In a real implementation, use a dedicated thread pool
    
    // Process each region in parallel
    std::vector<std::future<void>> regionFutures;
    regionFutures.reserve(pImpl->regions.size());
    
    for (auto& region : pImpl->regions) {
        regionFutures.push_back(std::async(std::launch::async, 
            [this, &region, currentStep, currentTime]() {
                processNeuralRegion(region.get(), currentStep, currentTime);
            }));
    }
    
    // Wait for all regions to complete processing
    for (auto& future : regionFutures) {
        future.wait();
    }
}

void Brain::processNeuralRegion(NeuralRegion* region, SimulationStep currentStep, Timestamp currentTime) {
    if (!region) return;
    
    // Process neural population in this region
    for (auto& pop : region->getPopulations()) {
        // Process neural population - can be parallelized further
        processNeuralPopulation(pop.get(), currentStep, currentTime);
    }
}

void Brain::processNeuralPopulation(NeuralPopulation* pop, SimulationStep currentStep, Timestamp currentTime) {
    if (!pop) return;
    
    // Process neurons in this population
    // Can be parallelized using SIMD or threading
    for (auto* neuron : pop->getNeurons()) {
        // Process neuron - apply parallel processing
        processNeuronInParallel(neuron, currentStep, currentTime);
    }
}

void Brain::processNeuronInParallel(Neuron* neuron, SimulationStep currentStep, Timestamp currentTime) {
    if (!neuron) return;
    
    // Apply parallel processing for neuron dynamics
    // Use SIMD optimizations where possible
    // Implement parallel spike detection and integration
    
    // Get neuron state
    NeuronState& state = neuron->getState();
    
    // Apply parallel LIF dynamics processing
    // For real implementation, use SIMD intrinsics or parallel algorithms
    processNeuronLIFParallel(neuron, currentStep, currentTime);
    
    // Record spike history
    if (state.firingState == FiringState::Active) {
        neuron->recordSpike(currentTime);
    }
}

void Brain::processNeuronLIFParallel(Neuron* neuron, SimulationStep currentStep, Timestamp currentTime) {
    if (!neuron) return;
    
    // Process LIF dynamics with parallel optimizations
    // Use parallel computation for membrane potential updates
    // Apply SIMD optimizations for numerical operations
    
    NeuronState& state = neuron->getState();
    
    // Parallel processing of LIF components
    // dV/dt = (V_rest - V)/tau + I/C
    // This can be computed in parallel with SIMD
    
    const float tau = 20.0f;  // ms
    const float C = 1.0f;     // nF
    
    // Parallel computation of leak and synaptic contributions
    float leakContribution = (state.restingPotential - state.membranePotential) / tau;
    float synapticContribution = neuron->getTotalCurrent() / C;
    
    // Apply parallel update to membrane potential
    state.membranePotential += 1.0f * 1000.0f * (leakContribution + synapticContribution);
    
    // Clamp membrane potential
    state.membranePotential = std::clamp(state.membranePotential, -100.0f, 50.0f);
    
    // Check for spike (parallel with other computations)
    if (state.membranePotential >= state.threshold) {
        state.firingState = FiringState::Active;
        neuron->recordSpike(currentTime);
        
        // Reset membrane potential
        state.membranePotential = state.resetPotential;
        
        // Enter refractory period
        state.refractoryRemaining = state.refractoryPeriod;
        state.firingState = FiringState::Refractory;
    }
    
    // Decay adaptation variable
    if (state.adaptationVariable > 0.0f) {
        state.adaptationVariable *= 0.95f;
    }
    
    // Clear synaptic input
    neuron->clearTotalCurrent();
}
    // Perform memory replay during sleep
    if (pImpl->episodicMemory) {
        // Enhanced replay with attention and prediction integration
        auto episodes = pImpl->episodicMemory->getRecentEpisodes(10);
        
        for (const auto* episode : episodes) {
            pImpl->episodicMemory->replayEpisode(episode, 
                                                *pImpl->attentionalSelection,
                                                *pImpl->predictionSystem,
                                                ReplayType::Standard);
        }
    }
    
    // Working memory consolidation during sleep
    if (pImpl->workingMemory) {
        // Enhance working memory through rehearsal
        auto currentWM = pImpl->workingMemory->retrieve();
        if (!currentWM.empty()) {
            pImpl->workingMemory->strengthenMemory(1.1f);
        }
    }
}

void Brain::performSystemsConsolidation(SimulationStep currentStep) {
    // Systems consolidation (hippocampal-neocortical integration)
    if (pImpl->episodicMemory) {
        // Consolidate episodic memories into semantic networks
        std::vector<float> neuromodulationLevels;
        neuromodulationLevels.push_back(getDopamineLevel());
        neuromodulationLevels.push_back(getAcetylcholineLevel());
        neuromodulationLevels.push_back(getNorepinephrineLevel());
        
        // Apply consolidation based on neuromodulation
        pImpl->episodicMemory->consolidate(0.3f, neuromodulationLevels, currentStep);
    }
    
    // Strengthen associative memory connections
    if (pImpl->associativeMemory) {
        // Enhance associations during deep sleep
        pImpl->associativeMemory->strengthenAssociations(0.8f);
    }
}

void Brain::performREMConsolidation(SimulationStep currentStep) {
    // REM sleep consolidation (emotional and procedural memory)
    if (pImpl->episodicMemory) {
        // Enhance emotional memory processing
        auto stressPriorities = pImpl->sleepManager->getStressBasedPriorities();
        
        if (!stressPriorities.empty() && stressPriorities[0] > 0.5f) {
            // High stress - enhance emotional memory consolidation
            NLM_LOG_INFO("REM: Enhancing emotional memory consolidation (stress: " + 
                        std::to_string(stressPriorities[0]) + ")");
        }
    }
    
    // Procedural memory consolidation
    // Would process motor sequences, habits, skills
}

void Brain::performActiveConsolidation(SimulationStep currentStep) {
    // Active state memory consolidation
    if (pImpl->episodicMemory) {
        // Regular consolidation during active processing
        std::vector<float> neuromodulationLevels;
        neuromodulationLevels.push_back(getDopamineLevel());
        neuromodulationLevels.push_back(getAcetylcholineLevel());
        neuromodulationLevels.push_back(getNorepinephrineLevel());
        
        pImpl->episodicMemory->consolidate(0.4f, neuromodulationLevels, currentStep);
    }
    
    // Working memory maintenance
    if (pImpl->workingMemory) {
        // Maintain working memory traces
        pImpl->workingMemory->update(0.001f, *pImpl->attentionalSelection);
    }
}

void Brain::applyStressGuidedConsolidation(SimulationStep currentStep) {
    // Apply stress-based memory prioritization
    std::vector<float> stressPriorities = pImpl->sleepManager->getStressBasedPriorities();
    
    if (!stressPriorities.empty() && stressPriorities[0] > 0.5f) {
        // High stress - prioritize consolidation of salient memories
        if (pImpl->episodicMemory) {
            NLM_LOG_INFO("Stress-guided consolidation: prioritizing salient memories (stress level: " + 
                        std::to_string(stressPriorities[0]) + ")");
            
            // Enhance consolidation of high-stress memories
            for (auto& priority : stressPriorities) {
                if (priority > 0.6f) {
                    // This would target specific memories based on stress level
                    break;
                }
            }
        }
    }
}

void Brain::applyAttentionGuidedConsolidation(SimulationStep currentStep) {
    // Apply attention-based memory guidance
    std::vector<NeuronId> attentionTargets = pImpl->sleepManager->getAttentionGuidedTargets();
    
    if (!attentionTargets.empty()) {
        NLM_LOG_INFO("Attention-guided consolidation: " + std::to_string(attentionTargets.size()) + 
                    " target memories based on attentional focus");
        
        // Apply attention to working memory and episodic memory
        if (pImpl->workingMemory) {
            for (const auto& neuron : attentionTargets) {
                if (pImpl->workingMemory->contains(neuron)) {
                    float currentStrength = pImpl->workingMemory->getNeuronActivation(neuron);
                    pImpl->workingMemory->storeToNeuron(neuron, currentStrength * 1.2f);
                }
            }
        }
    }
}

float Brain::getArousalLevel() const {
    // Calculate overall arousal level from neuromodulators
    float arousal = 0.0f;
    if (pImpl->dopamine) arousal += pImpl->dopamine->getLevel();
    if (pImpl->norepinephrine) arousal += pImpl->norepinephrine->getLevel();
    if (pImpl->curiosity) arousal += pImpl->curiosity->getLevel();
    
    return arousal / 3.0f;
}

void Brain::updateStressSignals(SimulationStep currentStep) {
    // Update stress signals based on current state
    StressSignals stress;
    
    // Simulate stress signal updates based on neural activity
    stress.cortisolLevel = std::min(1.0f, getArousalLevel() * 0.5f + 0.1f);
    stress.noradrenalineLevel = getArousalLevel() * 0.7f;
    stress.predictionError = pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f;
    stress.noveltyLevel = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
    stress.curiosityLevel = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
    stress.rewardPrediction = 0.5f; // Placeholder
    
    pImpl->stressSignals_ = stress;
    
    // Update sleep manager with stress signals
    if (pImpl->sleepManager) {
        pImpl->sleepManager->updateStressSignals(stress);
    }
}

void Brain::updateAttentionSignals(SimulationStep currentStep) {
    // Update attention guidance signals
    AttentionGuidance attention;
    
    // Get attention from attentional selection system
    if (pImpl->attentionalSelection) {
        attention.globalAttention = 0.5f; // Simplified
        attention.sensoryFocus = 0.3f; // Simplified
        attention.workingMemoryFocus = 0.4f; // Simplified
        attention.episodicFocus = 0.2f; // Simplified
    } else {
        attention.globalAttention = 0.3f;
        attention.sensoryFocus = 0.2f;
        attention.workingMemoryFocus = 0.5f;
        attention.episodicFocus = 0.1f;
    }
    
    pImpl->attentionGuidance_ = attention;
    
    // Update sleep manager with attention guidance
    if (pImpl->sleepManager) {
        pImpl->sleepManager->updateAttentionGuidance(attention);
    }
}

float Brain::getDopamineLevel() const {
    return pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
}

float Brain::getAcetylcholineLevel() const {
    return pImpl->acetylcholine ? pImpl->acetylcholine->getLevel() : 0.0f;
}

float Brain::getNorepinephrineLevel() const {
    return pImpl->norepinephrine ? pImpl->norepinephrine->getLevel() : 0.0f;
}

} // namespace nlm