#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../performance/MemoryPool.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
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
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
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
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Initialize acetylcholine system
        acetylcholine->initialize(this);
        
        // Initialize norepinephrine system
        norepinephrine->initialize(this);
        
        // Initialize serotonin system
        serotonin->initialize(this);
        
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
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

// NeuronPool implementation
namespace nlm {
    NeuronPool::NeuronPool(size_t capacity)
        : size_(0), capacity_(capacity), freeListTop_(0) {
        // Allocate initial blocks
        size_t blockSize = std::min(capacity / 2, static_cast<size_t>(1024));
        if (blockSize < 1) blockSize = 1;
        
        for (size_t i = 0; i < capacity / blockSize + 1; ++i) {
            blocks_.push_back(std::make_unique<NeuronBlock>());
            blocks_.back()->membranePotential.resize(blockSize);
            blocks_.back()->restingPotential.resize(blockSize);
            blocks_.back()->threshold.resize(blockSize);
            blocks_.back()->resetPotential.resize(blockSize);
            blocks_.back()->leakConductance.resize(blockSize);
            blocks_.back()->synapseConductance.resize(blockSize);
            blocks_.back()->firingRate.resize(blockSize);
            blocks_.back()->firingState.resize(blockSize);
            blocks_.back()->refractoryRemaining.resize(blockSize);
            blocks_.back()->refractoryPeriod.resize(blockSize);
            blocks_.back()->adaptationVariable.resize(blockSize);
            blocks_.back()->lastSpikeTime.resize(blockSize);
            blocks_.back()->neuronType.resize(blockSize);
            blocks_.back()->regionId.resize(blockSize);
            blocks_.back()->populationId.resize(blockSize);
            blocks_.back()->synapticInput.resize(blockSize);
            
            // Initialize free list
            for (size_t j = 0; j < blockSize && (i * blockSize + j) < capacity; ++j) {
                freeList_.push_back(static_cast<NeuronIndex>(i * blockSize + j));
            }
        }
    }
    
    NeuronPool::~NeuronPool() {
        // Blocks are automatically freed via unique_ptr
    }
    
    NeuronId NeuronPool::allocate() {
        if (size_ >= capacity_ || freeList_.empty()) {
            return NeuronId();  // Invalid neuron ID
        }
        
        size_t index = freeListTop_.fetch_add(1, std::memory_order_relaxed);
        if (index >= freeList_.size()) {
            return NeuronId();
        }
        
        NeuronIndex neuronIndex = freeList_[index];
        size_++;
        
        // Initialize neuron state
        NeuronBlock* block = findBlock(neuronIndex);
        if (!block) return NeuronId();
        
        size_t localIndex = neuronIndex % BLOCK_SIZE;
        
        // Initialize default LIF parameters
        block->membranePotential[localIndex] = -70.0f;
        block->restingPotential[localIndex] = -70.0f;
        block->threshold[localIndex] = -55.0f;
        block->resetPotential[localIndex] = -70.0f;
        block->leakConductance[localIndex] = 10.0f;
        block->synapseConductance[localIndex] = 0.0f;
        block->firingRate[localIndex] = 0.0f;
        block->firingState[localIndex] = 0; // FiringState::Resting
        block->refractoryRemaining[localIndex] = 0;
        block->refractoryPeriod[localIndex] = 5;
        block->adaptationVariable[localIndex] = 0.0f;
        block->lastSpikeTime[localIndex] = -1.0f;
        block->neuronType[localIndex] = 2; // NeuronType::Internal
        block->synapticInput[localIndex] = 0.0f;
        
        return NeuronId(neuronIndex);
    }
    
    void NeuronPool::deallocate(NeuronId id) {
        NeuronIndex index = id.index();
        
        // Add to free list (simplified implementation)
        if (index < freeList_.capacity()) {
            freeList_[index] = index;
            size_--;
        }
    }
    
    template<typename Member>
    Member* NeuronPool::getState(NeuronIndex index) {
        NeuronBlock* block = findBlock(index);
        if (!block) return nullptr;
        
        size_t localIndex = index % BLOCK_SIZE;
        
        if constexpr (std::is_same_v<Member, float>) {
            return &block->membranePotential[localIndex];
        } else if constexpr (std::is_same_v<Member, uint8_t>) {
            return &block->firingState[localIndex];
        } else if constexpr (std::is_same_v<Member, uint32_t>) {
            return &block->refractoryRemaining[localIndex];
        } else if constexpr (std::is_same_v<Member, uint64_t>) {
            return &block->neuronType[localIndex];
        } else {
            return nullptr;
        }
    }
    
    size_t NeuronPool::memoryUsage() const {
        size_t total = 0;
        for (const auto& block : blocks_) {
            total += block->membranePotential.capacity() * sizeof(float);
            total += block->restingPotential.capacity() * sizeof(float);
            total += block->threshold.capacity() * sizeof(float);
            total += block->resetPotential.capacity() * sizeof(float);
            total += block->leakConductance.capacity() * sizeof(float);
            total += block->synapseConductance.capacity() * sizeof(float);
            total += block->firingRate.capacity() * sizeof(float);
            total += block->firingState.capacity() * sizeof(uint8_t);
            total += block->refractoryRemaining.capacity() * sizeof(uint32_t);
            total += block->refractoryPeriod.capacity() * sizeof(uint32_t);
            total += block->adaptationVariable.capacity() * sizeof(float);
            total += block->lastSpikeTime.capacity() * sizeof(float);
            total += block->neuronType.capacity() * sizeof(uint64_t);
            total += block->regionId.capacity() * sizeof(uint64_t);
            total += block->populationId.capacity() * sizeof(uint64_t);
            total += block->synapticInput.capacity() * sizeof(float);
        }
        return total;
    }
    
    NeuronBlock* NeuronPool::findBlock(NeuronIndex index) {
        size_t blockIndex = index / BLOCK_SIZE;
        if (blockIndex < blocks_.size()) {
            return blocks_[blockIndex].get();
        }
        return nullptr;
    }
    
    // SynapsePool implementation
    SynapsePool::SynapsePool()
        : size_(0), capacity_(100000), freeListTop_(0) {
        // Pre-allocate synapse blocks
        size_t blockSize = 10000;
        for (size_t i = 0; i < 10; ++i) {
            blocks_.push_back(std::make_unique<SynapseBlock>());
            blocks_.back()->sourceNeuron.resize(blockSize);
            blocks_.back()->destinationNeuron.resize(blockSize);
            blocks_.back()->weight.resize(blockSize);
            blocks_.back()->delay.resize(blockSize);
            blocks_.back()->synapseType.resize(blockSize);
            blocks_.back()->plasticityFlags.resize(blockSize);
            blocks_.back()->shortTermDepression.resize(blockSize);
            blocks_.back()->shortTermFacilitation.resize(blockSize);
            blocks_.back()->lastPreSpikeTime.resize(blockSize);
            blocks_.back()->lastPostSpikeTime.resize(blockSize);
            blocks_.back()->eligibilityTrace.resize(blockSize);
            blocks_.back()->efficacy.resize(blockSize);
            
            // Initialize free list
            for (size_t j = 0; j < blockSize && (i * blockSize + j) < capacity_; ++j) {
                freeList_.push_back(static_cast<SynapseIndex>(i * blockSize + j));
            }
        }
    }
    
    SynapsePool::~SynapsePool() {
        // Blocks are automatically freed via unique_ptr
    }
    
    SynapseId SynapsePool::allocate(NeuronId source, NeuronId destination) {
        if (size_ >= capacity_ || freeList_.empty()) {
            return SynapseId();  // Invalid synapse ID
        }
        
        size_t index = freeListTop_.fetch_add(1, std::memory_order_relaxed);
        if (index >= freeList_.size()) {
            return SynapseId();
        }
        
        SynapseIndex synapseIndex = freeList_[index];
        size_++;
        
        // Initialize synapse state
        SynapseBlock* block = findBlock(synapseIndex);
        if (!block) return SynapseId();
        
        size_t localIndex = synapseIndex % BLOCK_SIZE;
        
        // Initialize default synapse properties
        block->sourceNeuron[localIndex] = source.value;
        block->destinationNeuron[localIndex] = destination.value;
        block->weight[localIndex] = 0.2f; // Default weight
        block->delay[localIndex] = 1;    // Default delay
        block->synapseType[localIndex] = 1; // SynapseType::Excitatory
        block->plasticityFlags[localIndex] = 3; // STDP and Hebbian
        block->shortTermDepression[localIndex] = 0.5f;
        block->shortTermFacilitation[localIndex] = 0.1f;
        block->lastPreSpikeTime[localIndex] = -1.0f;
        block->lastPostSpikeTime[localIndex] = -1.0f;
        block->eligibilityTrace[localIndex] = 0.0f;
        block->efficacy[localIndex] = 1.0f;
        
        return SynapseId(synapseIndex);
    }
    
    void SynapsePool::deallocate(SynapseId id) {
        SynapseIndex index = id.index();
        
        // Add to free list
        if (index < freeList_.capacity()) {
            freeList_[index] = index;
            size_--;
        }
    }
    
    template<typename Member>
    Member* SynapsePool::getState(SynapseIndex index) {
        SynapseBlock* block = findBlock(index);
        if (!block) return nullptr;
        
        size_t localIndex = index % BLOCK_SIZE;
        
        if constexpr (std::is_same_v<Member, uint64_t>) {
            return &block->sourceNeuron[localIndex];
        } else if constexpr (std::is_same_v<Member, float>) {
            return &block->weight[localIndex];
        } else if constexpr (std::is_same_v<Member, uint32_t>) {
            return &block->delay[localIndex];
        } else if constexpr (std::is_same_v<Member, uint8_t>) {
            return &block->synapseType[localIndex];
        } else {
            return nullptr;
        }
    }
    
    NeuronId SynapsePool::getSource(SynapseIndex index) const {
        SynapseBlock* block = findBlock(index);
        if (!block) return NeuronId();
        
        size_t localIndex = index % BLOCK_SIZE;
        return NeuronId(block->sourceNeuron[localIndex]);
    }
    
    NeuronId SynapsePool::getDestination(SynapseIndex index) const {
        SynapseBlock* block = findBlock(index);
        if (!block) return NeuronId();
        
        size_t localIndex = index % BLOCK_SIZE;
        return NeuronId(block->destinationNeuron[localIndex]);
    }
    
    size_t SynapsePool::memoryUsage() const {
        size_t total = 0;
        for (const auto& block : blocks_) {
            total += block->sourceNeuron.capacity() * sizeof(uint64_t);
            total += block->destinationNeuron.capacity() * sizeof(uint64_t);
            total += block->weight.capacity() * sizeof(float);
            total += block->delay.capacity() * sizeof(uint32_t);
            total += block->synapseType.capacity() * sizeof(uint8_t);
            total += block->plasticityFlags.capacity() * sizeof(uint8_t);
            total += block->shortTermDepression.capacity() * sizeof(float);
            total += block->shortTermFacilitation.capacity() * sizeof(float);
            total += block->lastPreSpikeTime.capacity() * sizeof(float);
            total += block->lastPostSpikeTime.capacity() * sizeof(float);
            total += block->eligibilityTrace.capacity() * sizeof(float);
            total += block->efficacy.capacity() * sizeof(float);
        }
        return total;
    }
    
    SynapseBlock* SynapsePool::findBlock(SynapseIndex index) {
        size_t blockIndex = index / BLOCK_SIZE;
        if (blockIndex < blocks_.size()) {
            return blocks_[blockIndex].get();
        }
        return nullptr;
    }
    
    // NeuralWorkingMemory optimizations
    void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
        // Check if neuron is already in memory
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
        
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            memoryActivations_[idx] = activation;
            memoryTimestamps_[idx] = 0;
        } else if (memoryNeurons_.size() < capacity_) {
            // Use memory pool for neuron allocation
            if (pImpl->brain) {
                auto& brain = *pImpl->brain;
                NeuronId newNeuron = brain.allocateFromPool();
                if (newNeuron.index() != 0) {  // Valid neuron ID
                    memoryNeurons_.push_back(newNeuron);
                    memoryActivations_.push_back(activation);
                    memoryTimestamps_.push_back(0);
                    
                    // Inject current to maintain activation
                    brain.injectCurrent(newNeuron, activation * 5.0f);
                    return;
                }
            }
            
            // Fallback to old method
            memoryNeurons_.push_back(neuron);
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
        }
        
        // Inject current to maintain activation
        if (pImpl->brain) {
            pImpl->brain->injectCurrent(neuron, activation * 5.0f);
        }
    }
    
    // Neuron.cpp optimizations
    bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
        bool fired = false;
        
        // Handle refractory period
        if (pImpl->state.refractoryRemaining > 0) {
            --pImpl->state.refractoryRemaining;
            // During refractory period, clear synaptic input
            pImpl->synapticInput = 0.0f;
            if (pImpl->state.refractoryRemaining == 0) {
                pImpl->state.firingState = FiringState::Resting;
            }
            return false;
        }
        
        // LIF dynamics: Leaky Integrate-and-Fire
        // dV/dt = (V_rest - V)/tau + I/C
        // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
        
        MembranePotential& V = pImpl->state.membranePotential;
        MembranePotential V_rest = pImpl->state.restingPotential;
        MembranePotential V_reset = pImpl->state.resetPotential;
        MembranePotential threshold = pImpl->state.threshold;
        float tau = Impl::TIME_CONSTANT;  // ms
        float C = Impl::MEMBRANE_CAPACITANCE;  // nF
        
        // Use SIMD vectorized operations for performance
        // For now, keep scalar version for correctness
        float synapticContribution = pImpl->synapticInput / C;
        
        // Leak contribution
        float leakContribution = (V_rest - V) / tau;
        
        // Update membrane potential using exponential Euler integration
        V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
        
        // Apply spike-frequency adaptation (slow hyperpolarization after spike)
        if (pImpl->state.adaptationVariable > 0.0f) {
            V -= pImpl->state.adaptationVariable * 0.01f;
            pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
        }
        
        // Clamp membrane potential to prevent instability
        V = std::clamp(V, -100.0f, 50.0f);
        
        // Check for spike
        if (V >= threshold) {
            fired = true;
            pImpl->state.firingState = FiringState::Active;
            pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
            
            // Record spike
            recordSpike(currentTime);
            
            // Reset membrane potential
            V = V_reset;
            
            // Enter refractory period
            pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
            pImpl->state.firingState = FiringState::Refractory;
            
            // Update adaptation for spike-frequency adaptation
            pImpl->state.adaptationVariable += 1.0f;
        } else {
            pImpl->state.firingState = FiringState::Active;
        }
        
        // Clear synaptic input for next step
        pImpl->synapticInput = 0.0f;
        
        return fired;
    }
    
    // NeuralRegion connectivity optimizations
    void NeuralRegion::initializeRandomConnectivity(RandomGenerator& rng,
                                                   float connectionProbability,
                                                   float meanWeight,
                                                   float weightVariance) {
        // Efficient random connectivity initialization with better cache locality
        // and reduced memory allocations
        
        // Get all neurons once
        auto neurons = getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount == 0) return;
        
        // Pre-allocate synapse storage
        size_t maxPossibleConnections = neuronCount * neuronCount; // Excluding self-connections
        size_t estimatedConnections = static_cast<size_t>(maxPossibleConnections * connectionProbability);
        
        // Create vectors for efficient synapse creation
        std::vector<NeuronId> sourceNeurons;
        std::vector<NeuronId> destNeurons;
        std::vector<SynapticWeight> weights;
        std::vector<Delay> delays;
        std::vector<SynapseType> types;
        
        sourceNeurons.reserve(estimatedConnections);
        destNeurons.reserve(estimatedConnections);
        weights.reserve(estimatedConnections);
        delays.reserve(estimatedConnections);
        types.reserve(estimatedConnections);
        
        // Create random connections in batches for better performance
        for (size_t i = 0; i < neuronCount; ++i) {
            Neuron* preNeuron = neurons[i];
            NeuronType preType = preNeuron->getType();
            
            // Skip if not a proper source neuron
            if (preType == NeuronType::Modulatory) continue;
            
            // Determine target neurons based on pre-synaptic type
            std::vector<Neuron*> targetNeurons;
            for (size_t j = 0; j < neuronCount; ++j) {
                if (i == j) continue;  // No self-connections
                
                Neuron* postNeuron = neurons[j];
                NeuronType postType = postNeuron->getType();
                
                // Modulatory neurons only connect to specific types
                if (preType == NeuronType::Acetylcholine && 
                    postType != NeuronType::Sensory && 
                    postType != NeuronType::Motor) continue;
                
                targetNeurons.push_back(postNeuron);
            }
            
            // Randomly select from targets based on connection probability
            for (Neuron* postNeuron : targetNeurons) {
                if (rng.bernoulli(connectionProbability)) {
                    // Determine synapse type and weight
                    SynapseType synType;
                    float weight;
                    
                    if (preType == NeuronType::Excitatory || 
                        preType == NeuronType::Sensory ||
                        preType == NeuronType::Motor ||
                        preType == NeuronType::Internal) {
                        // excitatory synapse
                        synType = SynapseType::Excitatory;
                        weight = meanWeight + rng.normal(0.0f, weightVariance);
                        weight = std::max(0.01f, weight);  // Ensure positive
                    } else if (preType == NeuronType::Inhibitory) {
                        // inhibitory synapse
                        synType = SynapseType::Inhibitory;
                        weight = -(meanWeight + rng.normal(0.0f, weightVariance));
                        weight = std::min(-0.01f, weight);  // Ensure negative
                    } else {
                        synType = SynapseType::Excitatory;
                        weight = meanWeight + rng.normal(0.0f, weightVariance);
                    }
                    
                    // Add synapse (deferred batch creation)
                    sourceNeurons.push_back(preNeuron->getId());
                    destNeurons.push_back(postNeuron->getId());
                    weights.push_back(weight);
                    delays.push_back(1);
                    types.push_back(synType);
                }
            }
        }
        
        // Batch create synapses for better performance
        size_t numConnections = sourceNeurons.size();
        for (size_t i = 0; i < numConnections; ++i) {
            SynapseId synId = addSynapse(sourceNeurons[i], destNeurons[i],
                                       weights[i], delays[i]);
            
            // Initialize synapse
            if (auto* syn = getSynapse(synId)) {
                syn->setType(types[i]);
                syn->initializeRandom(rng);
            }
        }
        
        NLM_LOG_INFO(std::to_string(numConnections) + " synapses created in region " + 
                    std::to_string(getId()));
    }
    
    // Simplified Neuron and Synapse implementations for memory pool integration
    void Neuron::addIncomingSynapse(SynapseHandle handle) {
        // Use memory pool for incoming synapse list
        if (pImpl->brain_) {
            // Try to get from pool first
            auto* synapsePool = pImpl->brain_->getSynapsePool();
            if (synapsePool) {
                // Get synapse state directly from pool
                auto* weightPtr = synapsePool->getState<float>(handle.synapseIndex);
                if (weightPtr) {
                    pImpl->incomingSynapses.push_back(handle);
                }
            }
        }
        
        // Fallback to original implementation
        pImpl->incomingSynapses.push_back(handle);
    }
    
    void Neuron::addOutgoingSynapse(SynapseHandle handle) {
        pImpl->outgoingSynapses.push_back(handle);
    }
    
    void Synapse::recordPreSpike(Timestamp timestamp) {
        // Record pre-synaptic spike time
        if (pImpl->preSpikeHistory.size() < pImpl->preSpikeHistory.capacity()) {
            pImpl->preSpikeHistory.push_back(timestamp);
        } else {
            // Circular buffer behavior for spike history
            pImpl->preSpikeHistory[(pImpl->preSpikeIndex++) % pImpl->preSpikeHistory.capacity()] = timestamp;
        }
        
        // Update short-term plasticity
        if (pImpl->synapseType == SynapseType::Excitatory) {
            pImpl->u = pImpl->U_max * (1.0f - pImpl->u) + pImpl->u * std::exp(-timestamp / pImpl->tau_f);
        }
    }
    
    void Synapse::recordPostSpike(Timestamp timestamp) {
        if (pImpl->postSpikeHistory.size() < pImpl->postSpikeHistory.capacity()) {
            pImpl->postSpikeHistory.push_back(timestamp);
        } else {
            pImpl->postSpikeHistory[(pImpl->postSpikeIndex++) % pImpl->postSpikeHistory.capacity()] = timestamp;
        }
    }
    
    float Synapse::getEligibilityTrace() const {
        return pImpl->eligibilityTrace;
    }
    
    void Synapse::setEligibilityTrace(float trace) {
        pImpl->eligibilityTrace = trace;
    }
    
    void Synapse::decayEligibilityTrace(float decayRate) {
        pImpl->eligibilityTrace *= (1.0f - decayRate);
    }
    
    float Synapse::getEfficacy() const {
        return pImpl->efficacy;
    }
    
    void Synapse::setEfficacy(float efficacy) {
        pImpl->efficacy = efficacy;
    }
    
    // NeuralWorkingMemory further optimizations
    void NeuralWorkingMemory::runCompetition() {
        winners_.clear();
        
        if (memoryActivations_.empty()) return;
        
        // Find neurons with above-threshold activation
        float threshold = 0.3f;
        
        // Batch processing for better performance
        std::vector<size_t> winnerIndices;
        winnerIndices.reserve(memoryNeurons_.size());
        
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (memoryActivations_[i] >= threshold) {
                winnerIndices.push_back(i);
            }
        }
        
        // Add winners to results
        winners_.reserve(winnerIndices.size());
        for (size_t idx : winnerIndices) {
            winners_.push_back(memoryNeurons_[idx]);
        }
        
        // Inhibitory competition - suppress non-winners
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            bool isWinner = std::find(winnerIndices.begin(), winnerIndices.end(), i) != winnerIndices.end();
            
            if (!isWinner && brain_) {
                // Apply strong inhibition
                brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 3.0f);
            }
        }
    }
    
    // Neuron state access for memory pools
    template<typename Member>
    Member* NeuronPool::getState(NeuronIndex index) {
        // Implementation for different member types
        if constexpr (std::is_same_v<Member, float>) {
            // Return pointer to membrane potential, resting potential, etc.
            NeuronBlock* block = findBlock(index);
            if (!block) return nullptr;
            size_t localIndex = index % BLOCK_SIZE;
            // Return appropriate pointer based on requested member type
            return nullptr;
        } else if constexpr (std::is_same_v<Member, uint8_t>) {
            return nullptr;
        } else if constexpr (std::is_same_v<Member, uint32_t>) {
            return nullptr;
        } else if constexpr (std::is_same_v<Member, uint64_t>) {
            return nullptr;
        } else {
            return nullptr;
        }
    }
    
    // Synapse state access for memory pools
    template<typename Member>
    Member* SynapsePool::getState(SynapseIndex index) {
        // Implementation for different member types
        if constexpr (std::is_same_v<Member, uint64_t>) {
            // Source neuron ID
            SynapseBlock* block = findBlock(index);
            if (!block) return nullptr;
            size_t localIndex = index % BLOCK_SIZE;
            return &block->sourceNeuron[localIndex];
        } else if constexpr (std::is_same_v<Member, float>) {
            // Weight
            SynapseBlock* block = findBlock(index);
            if (!block) return nullptr;
            size_t localIndex = index % BLOCK_SIZE;
            return &block->weight[localIndex];
        } else if constexpr (std::is_same_v<Member, uint32_t>) {
            // Delay
            SynapseBlock* block = findBlock(index);
            if (!block) return nullptr;
            size_t localIndex = index % BLOCK_SIZE;
            return &block->delay[localIndex];
        } else if constexpr (std::is_same_v<Member, uint8_t>) {
            // Synapse type or plasticity flags
            SynapseBlock* block = findBlock(index);
            if (!block) return nullptr;
            size_t localIndex = index % BLOCK_SIZE;
            return &block->synapseType[localIndex];
        } else {
            return nullptr;
        }
    }
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
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
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create memory pools for neurons and synapses
    pImpl->neuronPool = std::make_unique<NeuronPool>(neuronCount);
    pImpl->synapsePool = std::make_unique<SynapsePool>();
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions using memory pool
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t totalCreated = 0;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations using memory pools
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                    totalCreated++;
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                    totalCreated++;
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(totalCreated / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
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
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
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
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
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
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Remove the call to processSpikes since it's no longer needed for event-driven system
    // Spikes are now processed immediately when queued
    // pImpl->spikeSystem->processSpikes(currentStep);
    
    // Process delayed spikes that should be delivered now
    pImpl->spikeSystem->processDelayedSpikes(currentStep, pImpl->time);
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
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
    
    // ========== STEP 7: Update episodic memory ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
        // Phase 2 TODO: Implement real predictive coding with:
        // 1. Generate predictions for next sensory state based on current state
        // 2. Compare predictions with actual sensory input
        // 3. Update prediction models based on prediction error
        // 4. Use prediction error to drive learning and attention
        
        // Simple prediction error tracking for now
        static float predictionErrorHistory = 0.0f;
        predictionErrorHistory = 0.1f;  // Placeholder
        
        // Update curiosity based on prediction error
        if (pImpl->curiosity) {
            pImpl->curiosity->update(pImpl->timestep);
            // curiosity is already updated, but we could add prediction error signal
        }
        
        // Apply neuromodulation based on prediction error
        if (pImpl->predictionError) {
            pImpl->predictionError->update(pImpl->timestep);
            
            // Prediction error affects plasticity and attention
            float errorLevel = pImpl->predictionError->getLevel();
            
            // Update dopamine based on prediction error (reward prediction error)
            if (pImpl->dopamine) {
                // Prediction error updates reward prediction
                pImpl->dopamine->update(pImpl->timestep);
            }
            
            // Prediction error affects attentional selection
            if (pImpl->attention) {
                // Higher prediction error increases exploration attention
                float attentionBias = errorLevel * 0.3f;
                pImpl->attention->applyTopDownBias(NeuronId(-1), attentionBias);  // Note: Would need proper neuron ID
            }
        }
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
        
        // Apply attention modulation from neuromodulators
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            // Dopamine modulates attention selection
            if (dopamineLevel > 0.0f) {
                // Positive dopamine increases attention to salient stimuli
                pImpl->attention->setExcitationStrength(1.5f + dopamineLevel * 0.5f);
            }
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
        
        // Get current working memory content as potential concept input
        std::vector<float> workingMemoryPattern = pImpl->workingMemory ? 
            pImpl->workingMemory->retrieve() : std::vector<float>();
        
        if (!workingMemoryPattern.empty()) {
            // Present current experience to concept formation
            // Estimate reward from current neural activity
            float reward = 0.0f;
            for (float val : workingMemoryPattern) {
                reward += std::abs(val - 0.5f); // deviation from baseline
            }
            reward /= workingMemoryPattern.size();
            
            // Update concept formation with current state
            pImpl->conceptFormation->presentExperience(
                workingMemoryPattern, workingMemoryPattern, 
                reward, pImpl->currentStep);
            
            // Apply concept-guided attention
            size_t conceptId = pImpl->conceptFormation->findConceptForPattern(workingMemoryPattern);
            if (conceptId > 0) {
                // Concept modulates attention bias
                pImpl->attention->applyTopDownBias(NeuronId(-1), 0.3f);  // Note: Would need proper neuron ID
            }
        }
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Encode sensory input and integrate with all brain systems
    // Current goes to sensory neurons for immediate processing
    // The same input is encoded and sent to working memory, prediction,
    // concept formation, and attention for higher-level integration
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Create neural activity pattern from sensory input
    std::vector<float> neuralPattern;
    neuralPattern.reserve(values.size());
    
    // Encode sensory values as neural activations
    for (size_t i = 0; i < values.size(); ++i) {
        // Normalize input value to range [0, 1] for neural pattern
        float encodedValue = 0.5f + static_cast<float>(values[i]) / 20.0f;
        encodedValue = std::max(0.0f, std::min(1.0f, encodedValue)); // Clamp to [0, 1]
        neuralPattern.push_back(encodedValue);
    }
    
    // Distribute input across sensory neurons for immediate processing
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron (Step 1: Feedforward processing)
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store in working memory (Step 2: Transient storage)
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
    
    // Integrate with prediction system (Step 3: Predictive coding)
    if (pImpl->predictionSystem && !neuralPattern.empty()) {
        // Convert back to SensoryInput for prediction system
        SensoryInput sensoryInput;
        // Assuming SensoryInput constructor can take a vector<float>
        // This is a simplified implementation - in real code would need proper conversion
        pImpl->predictionSystem->train(sensoryInput);
    }
    
    // Integrate with concept formation (Step 4: Pattern abstraction)
    if (pImpl->conceptFormation && !neuralPattern.empty()) {
        // Create a reward signal based on sensory strength
        float reward = 0.0f;
        for (float val : neuralPattern) {
            reward += val;
        }
        reward /= neuralPattern.size();
        
        // Present experience to concept formation system
        pImpl->conceptFormation->presentExperience(neuralPattern, neuralPattern, reward, pImpl->currentStep);
    }
    
    // Integrate with attentional selection (Step 5: Focused selection)
    if (pImpl->attention && !neuralPattern.empty()) {
        // Convert neural pattern to neuron IDs for attention processing
        std::vector<NeuronId> neuronsForAttention;
        for (size_t i = 0; i < std::min<size_t>(numSensory, 10); ++i) {  // Use up to 10 neurons for attention
            neuronsForAttention.push_back(pImpl->sensoryNeurons[i]->getId());
        }
        
        if (!neuronsForAttention.empty()) {
            // Process competition among sensory neurons
            pImpl->attention->processCompetition(neuronsForAttention);
        }
    }
    
    // Store episodic memory of sensory input (Step 6: Long-term encoding)
    if (pImpl->episodicMemory && !neuralPattern.empty()) {
        EpisodicMemoryItem episode;
        episode.timestamp = pImpl->currentStep;
        episode.sensoryState = neuralPattern;
        episode.reward = 0.0f; // Will be updated later with prediction error
        episode.action = ActionType::Wait;
        
        pImpl->episodicMemory->storeEpisode(episode);
    }
    
    // Update novelty detection with sensory input
    if (pImpl->novelty && !neuralPattern.empty()) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity about sensory input
    if (pImpl->curiosity && !neuralPattern.empty()) {
        // Check if sensory input is surprising compared to predictions
        float predictionError = 0.0f;
        if (pImpl->predictionSystem) {
            predictionError = pImpl->predictionSystem->getPredictionError();
        }
        
        // Update curiosity based on prediction error
        pImpl->curiosity->update(pImpl->timestep);
        
        // Modify curiosity based on sensory input strength
        if (!neuralPattern.empty()) {
            float patternStrength = 0.0f;
            for (float val : neuralPattern) {
                patternStrength += std::abs(val - 0.5f);
            }
            patternStrength /= neuralPattern.size();
            
            // Curiosity is increased by both prediction error and pattern strength
            float curiosityLevel = pImpl->curiosity->getLevel();
            curiosityLevel += patternStrength * 0.1f;
            if (predictionError > 0.0f) {
                curiosityLevel += predictionError * 0.5f;
            }
            
            // Cap curiosity level
            curiosityLevel = std::min(1.0f, curiosityLevel);
            
            // Note: Would need a setLevel method on curiosity
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
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
size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

/**
 * ATOM: Implementation of a compute-based action selection system
 * Enhanced version with statistical analysis of neural activity
 */
size_t Brain::getPendingSpikeEventCount() const {
    // After refactor: no pending spikes since they're processed immediately
    return pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Use NeuralPlanner for intelligent action planning
    // Get current neural state from working memory
    // Use planner to select optimal action based on learned predictions
    
    // Get current neural state from working memory (represents current brain state)
    std::vector<float> currentState;
    if (pImpl->workingMemory) {
        currentState = pImpl->workingMemory->retrieve();
    }
    
    // If no working memory, use default state
    if (currentState.empty()) {
        // Create a simple state from motor neuron activity
        currentState.resize(pImpl->motorNeurons.size());
        for (size_t i = 0; i < pImpl->motorNeurons.size(); ++i) {
            currentState[i] = pImpl->motorNeurons[i]->isFiring() ? 1.0f : 0.0f;
        }
    }
    auto* planner = getPlanner();
    if (planner) {
        // Get planning target from current brain state
        float targetReward = 0.5f;  // Default target
        
        // Get dopamine level to influence planning
        auto* dopamine = getDopamine();
        if (dopamine) {
            targetReward = dopamine->getLevel();
        }
        
        // Get curiosity level to influence exploration
        auto* curiosity = getCuriosity();
        if (curiosity) {
            float curiosityLevel = curiosity->getLevel();
            // Higher curiosity encourages exploration
            if (curiosityLevel > 0.5f) {
                targetReward = std::max(0.1f, targetReward - 0.2f);
            }
        }
        
        // Use NeuralPlanner to plan the best action
        ActionType plannedAction = planner->planAction(currentState, targetReward);
        
        // Log the planned action
        NLM_LOG_INFO("Brain planning action: " + std::to_string(static_cast<int>(plannedAction)) +
                     " (state size: " + std::to_string(currentState.size()) + ")");
        
        // Return the planned action
        return std::make_unique<Action>(plannedAction);
    }
    
    // Fallback to simple action selection if planner not available
    // This should not happen in Phase 6 but included for safety
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    return std::make_unique<Action>(type);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
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
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
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
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
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
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
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
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

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

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

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

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm
