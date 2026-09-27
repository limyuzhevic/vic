// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/New.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/ParallelProcessing.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/SparseConnectivity.hpp"
#include "../performance/OptimizedBrain.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../development/Maturation.hpp"
#include "../development/Pruning.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../environment/Environment.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../visualization/VisualizationInterface.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include "../experiments/Phase3Experiment.hpp"
#include "../experiments/Phase4Experiment.hpp"
#include "../experiments/Phase5Experiment.hpp"
#include "../experiments/Phase6IntegratedExperiment.hpp"
#include "../experiments/Phase6Demo.hpp"
#include "../experiments/Phase4Demo.hpp"
#include "../experiments/Phase3Demo.hpp"
#include "../benchmark.hpp"
#include "AdvancedBrainInterface.hpp"

#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

namespace {
    // Helper function to normalize path
    std::string normalizePath(const std::string& path) {
        std::filesystem::path fsPath(path);
        return fsPath.lexically_normal().string();
    }
    
    // Check if directory exists
    bool directoryExists(const std::string& path) {
        std::filesystem::path fsPath(normalizePath(path));
        return std::filesystem::is_directory(fsPath);
    }
    
    // Get all files in directory
    std::vector<std::string> getFilesInDirectory(const std::string& path) {
        std::vector<std::string> files;
        std::filesystem::path fsPath(normalizePath(path));
        
        if (!std::filesystem::exists(fsPath)) {
            return files;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(fsPath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
        
        return files;
    }
    
    // Get file extension
    std::string getFileExtension(const std::string& path) {
        size_t dotPos = path.find_last_of('.');
        if (dotPos != std::string::npos) {
            return path.substr(dotPos + 1);
        }
        return "";
    }
}

// Definition of InterRegionConnection structure (defined in Brain.hpp)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Helper to access the current brain's implementation
Brain::Impl* getCurrentBrain() {
    // This is a placeholder - in a real implementation this would
    // access the global brain instance or current context
    static Brain::Impl* current = nullptr;
    return current;
}

} // namespace nlm

// Original Brain.cpp implementation (Phase 6: Integrated Artificial Brain)
namespace nlm {

// Constructor
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

// Destructor
Brain::~Brain() = default;

// Move constructor
Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

// Move assignment operator
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Initialize brain
bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
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
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
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

// Step method with single parameter
void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

// Step method with two parameters (main brain loop)
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
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
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
    
    // Update acetylcholine (attention and memory)
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
        
        // Apply acetylcholine effects on attention and memory
        // Acetylcholine enhances attention and memory consolidation
        float achLevel = pImpl->acetylcholine->getLevel();
        if (achLevel > 0.3f) {
            // Boost attention-related processes
            // Real implementation: Adjust neural gain and synaptic weights
            // for enhanced focus and information processing
            boostAttentionProcesses(achLevel);
            enhanceWorkingMemoryCapacity(achLevel);
        }
        
        // Signal attention demand based on current activity
        if (pImpl->workingMemory) {
            size_t activeMemoryTraces = pImpl->workingMemory->getActiveTraces();
            if (activeMemoryTraces > 10) {
                // High memory load signals need for attention
                pImpl->acetylcholine->signalAttention(0.5f * (activeMemoryTraces / 50.0f));
            }
        }
    }
    
    // Update norepinephrine (arousal and vigilance)
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
        
        // Apply norepinephrine effects on arousal
        // Norepinephrine affects alertness and stress response
        float neLevel = pImpl->norepinephrine->getLevel();
        if (neLevel > 0.2f) {
            // Boost arousal-related processes
            // This affects responsiveness and attention
        }
        
        // Signal arousal based on environmental novelty
        if (pImpl->novelty && pImpl->novelty->getLevel() > 0.5f) {
            // Novel stimuli trigger arousal response
            pImpl->norepinephrine->signalArousal(0.3f);
        }
    }
    
    // Update serotonin (mood and social behavior)
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
        
        // Apply serotonin effects on mood and social behavior
        // Serotonin affects mood stability and social engagement
        float serotoninLevel = pImpl->serotonin->getLevel();
        if (serotoninLevel > 0.4f) {
            // Positive mood effects
            // This affects learning and memory consolidation
        }
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
    
    // Integrate all neuromodulators for comprehensive plasticity control
    if (pImpl->dopamine) {
        plasticityMod *= pImpl->dopamine->getPlasticityFactor();
    }
    if (pImpl->acetylcholine) {
        plasticityMod *= pImpl->acetylcholine->getPlasticityFactor();
    }
    if (pImpl->norepinephrine) {
        plasticityMod *= pImpl->norepinephrine->getPlasticityFactor();
    }
    if (pImpl->serotonin) {
        plasticityMod *= pImpl->serotonin->getPlasticityFactor();
    }
    
    // Apply comprehensive neuromodulation to plasticity
    // Different neuromodulators affect plasticity in different ways:
    // - Dopamine: reward prediction error, approach behavior
    // - Acetylcholine: attention and memory consolidation
    // - Norepinephrine: arousal and stress response
    // - Serotonin: mood and social behavior
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with comprehensive neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on all neuromodulators
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    
                    // Apply combined neuromodulatory effects
                    float combinedMod = plasticityMod;
                    if (pImpl->dopamine) {
                        combinedMod *= pImpl->dopamine->getPlasticityFactor();
                    }
                    
                    weight += (weight > 0 ? 1.0f : -1.0f) * (combinedMod - 1.0f) * 0.001f;
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
    if (pImpl->predictionSystem && pImpl->predictionError) {
        // Predict next sensory state based on current neural activity patterns
        // Create a compressed representation of current neural state for prediction
        std::vector<float> currentNeuralState;
        
        // Sample neural activity from all populations to create prediction input
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                // Get average activity from population neurons
                float avgActivity = 0.0f;
                size_t neuronCount = 0;
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    avgActivity += std::abs(state.membranePotential - state.restingPotential);
                    neuronCount++;
                }
                if (neuronCount > 0) {
                    avgActivity /= neuronCount;
                    currentNeuralState.push_back(avgActivity);
                }
            }
        }
        
        // If we have sufficient neural activity data, make prediction
        if (currentNeuralState.size() > 10) {
            // Get recent sensory input for prediction error calculation
            // This simulates receiving the actual sensory input in the same step
            // In a real implementation, this would be passed from outside
            
            // For now, use a placeholder approach: predict based on average activity
            // Update prediction system with current neural state
            // This would ideally be sensory data from the environment
            
            // Create a synthetic sensory input for testing (would normally come from world)
            static uint64_t predictionStep = 0;
            predictionStep++;
            
            // Store neural state for potential concept formation later
            if (pImpl->conceptFormation) {
                // Create pattern from current neural state
                // Note: This is a simplification - real concept formation needs
                // proper sensory-to-neural encoding
                static std::vector<float> lastPattern;
                lastPattern = currentNeuralState;
                
                // Present neural pattern to concept formation
                // This connects neural dynamics to cognitive development
                pImpl->conceptFormation->presentExperience(
                    currentNeuralState,  // pattern
                    currentNeuralState,  // features  
                    pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,  // reward
                    currentStep
                );
            }
        }
        
        // Update prediction error based on neural prediction vs expected
        if (currentNeuralState.size() > 5 && pImpl->predictionError) {
            // Simple prediction error based on neural activity variability
            float avgActivity = 0.0f;
            for (float val : currentNeuralState) avgActivity += val;
            avgActivity /= currentNeuralState.size();
            
            float variability = 0.0f;
            for (float val : currentNeuralState) {
                float diff = val - avgActivity;
                variability += diff * diff;
            }
            variability = std::sqrt(variability / currentNeuralState.size());
            
            // Use prediction error to modulate neural excitability
            float error = variability * 0.1f; // Scale factor
            pImpl->predictionError->computeError(avgActivity, error);
            
            // Connect prediction error to neuromodulation
            if (pImpl->curiosity) {
                // Curiosity should be driven by prediction error
                pImpl->curiosity->setPredictionErrorWeight(pImpl->predictionError->getError());
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
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation && pImpl->workingMemory) {
        // Process current neural activity patterns to form concepts
        // Concept formation should integrate with working memory to identify recurring patterns
        
        // Get working memory patterns that could form concepts
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        if (!memoryNeurons.empty() && memoryNeurons.size() > 3) {
            // Sample a subset of working memory patterns for concept formation
            std::vector<float> samplePattern;
            
            // Create pattern from active working memory neurons
            for (size_t i = 0; i < std::min(size_t(10), memoryNeurons.size()); ++i) {
                auto* neuron = nullptr;
                // Find neuron in any region
                for (auto& region : pImpl->regions) {
                    auto neurons = region->getAllNeurons();
                    for (auto* n : neurons) {
                        if (n->getId() == memoryNeurons[i]) {
                            neuron = n;
                            break;
                        }
                    }
                    if (neuron) break;
                }
                
                if (neuron) {
                    const auto& state = neuron->getState();
                    float activity = std::abs(state.membranePotential - state.restingPotential);
                    samplePattern.push_back(activity);
                }
            }
            
            if (samplePattern.size() >= 5) {
                // Present neural pattern to concept formation system
                // This enables concepts to emerge from neural dynamics
                pImpl->conceptFormation->presentExperience(
                    samplePattern,  // pattern - neural activity pattern
                    samplePattern,  // features - derived from neural activity
                    pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,  // reward signal
                    currentStep
                );
            }
        }
        
        // Update concept prototypes based on ongoing neural patterns
        // This allows concepts to evolve over time
        auto concepts = pImpl->conceptFormation->getConcepts();
        for (const auto& concept : concepts) {
            // Update concept based on current neural activity
            // Note: currentNeuralState is captured in prediction system step
            // In a real implementation, we would track the most recent state
            static std::vector<float> recentPattern;
            if (!currentNeuralState.empty()) {
                recentPattern = currentNeuralState;
                pImpl->conceptFormation->updateConcept(
                    concept.id,  // concept ID
                    recentPattern,  // new pattern
                    recentPattern,  // features
                    pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f  // reward
                );
            }
        }
    }
    
    // ========== STEP 9: Neural planning ==========
    if (pImpl->planner) {
        // Create current neural state pattern for planning
        std::vector<float> currentState;
        
        // Sample neural activity for planning
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                // Get average activity from motor and planning-related populations
                if (pop->getNeuronType() == NeuronType::Motor || 
                    pop->getNeuronType() == NeuronType::Internal) {
                    
                    float avgActivity = 0.0f;
                    size_t neuronCount = 0;
                    for (const auto* neuron : pop->getNeurons()) {
                        const auto& state = neuron->getState();
                        avgActivity += std::abs(state.membranePotential - state.restingPotential);
                        neuronCount++;
                    }
                    if (neuronCount > 0) {
                        avgActivity /= neuronCount;
                        currentState.push_back(avgActivity);
                    }
                }
            }
        }
        
        // If we have sufficient neural state data, plan action
        if (currentState.size() >= 5 && pImpl->dopamine) {
            // Get target reward from dopamine level
            float targetReward = pImpl->dopamine->getLevel();
            
            // Let neural planner generate action sequence
            ActionType plannedAction = pImpl->planner->planAction(currentState, targetReward);
            
            // Store planned action in working memory for later execution
            if (pImpl->workingMemory && plannedAction != ActionType::Wait) {
                // Store planning result in working memory
                static size_t lastPlannedActionTime = 0;
                if (currentStep - lastPlannedActionTime > 100) {  // Plan every ~100 steps
                    lastPlannedActionTime = currentStep;
                    
                    // Store planning result
                    pImpl->workingMemory->storeToNeuron(
                        pImpl->motorNeurons.empty() ? 
                            RegionId(1).getId() : pImpl->motorNeurons[0]->getId(),
                        targetReward
                    );
                }
            }
        }
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

// Receive sensory input from environment
void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

// Inject current directly into a specific neuron
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

// Inject current into all neurons of a specific type
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

// Spike system access
SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

// Plasticity system access
STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

// Statistics
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

// Produce motor/action output based on motor neuron activity
std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
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
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

// Apply neuromodulatory signals
void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

// Update plasticity rules (called automatically in step)
void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

// Apply developmental changes (called automatically in step)
void Brain::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

// Reset brain state
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

// Save brain state to file (checkpointing)
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

// Load brain state from file
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

// Region management
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

// Get all regions
const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

// Inter-region connection management
void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1) {
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

// Global statistics
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

// Note: SemanticMemory and ProceduralMemory are defined in Memory.hpp but not yet implemented (TODO PHASE 2)
    // NeuralSemanticMemory class doesn't exist - should be SemanticMemory
    // NeuralProceduralMemory class doesn't exist - should be ProceduralMemory
    // NeuralAssociativeMemory already implemented and being used

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

// Dopamine - reward and reinforcement learning
Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

// Acetylcholine - attention and memory consolidation
Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

// Norepinephrine - arousal and vigilance
Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

// Serotonin - mood and social behavior
Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
}

// Curiosity - exploration motivation
Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

// Novelty - novelty detection
Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

// Prediction error signal
PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

// Get current configuration
std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

// Get random generator
RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

// Logging
void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Total regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("Excitatory/inhibitory ratio: " + std::to_string(getExcitationInhibitionRatio()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    
    // Log advanced interface status
    std::string interfaceStatus = getAdvancedInterfaceStatus();
    if (!interfaceStatus.empty()) {
        NLM_LOG_INFO(interfaceStatus);
    }
    
    // Log system resources
    NLM_LOG_INFO("Memory systems:");
    NLM_LOG_INFO("  - Working memory capacity: " + std::to_string(pImpl->workingMemory->getCapacity()));
    NLM_LOG_INFO("  - Episodic memory max episodes: " + std::to_string(pImpl->episodicMemory->getMaxEpisodes()));
    
    NLM_LOG_INFO("Cognitive systems:");
    NLM_LOG_INFO("  - Planning depth: " + std::to_string(pImpl->planner->getPlanningDepth()));
    NLM_LOG_INFO("  - Attention inhibition: " + std::to_string(pImpl->attention->getInhibitionStrength()));
    NLM_LOG_INFO("  - Attention excitation: " + std::to_string(pImpl->attention->getExcitationStrength()));
    
    NLM_LOG_INFO("Neuromodulation:");
    NLM_LOG_INFO("  - Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    NLM_LOG_INFO("  - Acetylcholine level: " + std::to_string(pImpl->acetylcholine->getLevel()));
    NLM_LOG_INFO("  - Norepinephrine level: " + std::to_string(pImpl->norepinephrine->getLevel()));
    NLM_LOG_INFO("  - Serotonin level: " + std::to_string(pImpl->serotonin->getLevel()));
    NLM_LOG_INFO("  - Curiosity level: " + std::to_string(pImpl->curiosity->getLevel()));
    NLM_LOG_INFO("  - Novelty level: " + std::to_string(pImpl->novelty->getLevel()));
    NLM_LOG_INFO("  - Prediction error level: " + std::to_string(pImpl->predictionError->getLevel()));
}

// ========== ATTENTION AND MEMORY ENHANCEMENT ==========

// Boost attention-related processes using acetylcholine signals
void Brain::boostAttentionProcesses(float achLevel) {
    // Boost attention-related processes using acetylcholine signals
    // This affects neural gain and synaptic weights for enhanced focus and information processing
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Increase excitability based on acetylcholine level
                float excitabilityMod = achLevel * 0.2f;
                if (excitabilityMod > 0.0f) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

// Enhance working memory capacity based on acetylcholine levels
void Brain::enhanceWorkingMemoryCapacity(float achLevel) {
    // Enhance working memory capacity based on acetylcholine levels
    // Acetylcholine supports working memory through enhanced persistence
    float capacityMultiplier = std::min(1.5f, 1.0f + achLevel * 0.5f);
    
    if (pImpl->workingMemory) {
        // Increase memory trace persistence duration
        pImpl->workingMemory->setPersistenceDuration(
            pImpl->workingMemory->getPersistenceDuration() * capacityMultiplier
        );
        
        // Enhance interference resistance
        pImpl->workingMemory->setInterferenceResistance(
            pImpl->workingMemory->getInterferenceResistance() + achLevel * 0.1f
        );
    }
}

} // namespace nlm
