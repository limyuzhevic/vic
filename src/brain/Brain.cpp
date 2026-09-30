#include "Brain.hpp"

namespace nlm {

// Implementation details for Brain class
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
    
    // Development
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
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
        , replayInterval(100)
        , consolidationInterval(1000)
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
    }
};

// Brain constructor
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

// Brain destructor
Brain::~Brain() = default;

// Initialize brain with configuration
bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Use BrainInitializer to set up all systems
    if (!BrainInitializer::initializeSystems(*pImpl, pImpl->config)) {
        return false;
    }
    
    // Use BrainSystemManager to set up regions and integrate systems
    BrainSystemManager::setupRegions(*pImpl, pImpl->config, this);
    
    NLM_LOG_INFO("NLM Brain initialization completed successfully");
    return true;
}

// Main simulation step
void Brain::step(SimulationStep currentStep) {
    step(currentStep, pImpl->timestep);
}

// Simulation time step
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    BrainStepProcessor::processStep(*pImpl, currentStep, currentTime);
}

// Receive sensory input from environment
void Brain::receiveSensoryInput(const SensoryInput& input) {
    // Convert input to current injection into sensory neurons
    for (Neuron* neuron : pImpl->sensoryNeurons) {
        if (neuron) {
            float current = input.getCurrentAt(neuron->getId());
            neuron->injectCurrent(current);
        }
    }
}

// Inject current directly into a specific neuron
void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        if (region->injectCurrentIntoNeuron(neuron, current)) {
            break;
        }
    }
}

// Inject current into all neurons of a specific type
void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        region->injectCurrentToNeurons(type, current);
    }
}

// Spike system access
SpikeSystem* Brain::getSpikeSystem() { return pImpl->spikeSystem.get(); }
const SpikeSystem* Brain::getSpikeSystem() const { return pImpl->spikeSystem.get(); }

// Plasticity system access
STDP* Brain::getSTDP() { return pImpl->stdp.get(); }
Hebbian* Brain::getHebbian() { return pImpl->hebbian.get(); }
StructuralPlasticity* Brain::getStructuralPlasticity() { return pImpl->structuralPlasticity.get(); }

// Statistics
float Brain::getExcitationInhibitionRatio() const {
    return pImpl->spikeSystem ? pImpl->spikeSystem->getExcitationInhibitionRatio() : 1.0f;
}

// Overload for compatibility
float Brain::getTotalSpikeCount() const {
    return static_cast<float>(pImpl->totalSpikesTotal);
}

// Other statistics methods would be implemented similarly...

// Produce motor/action output based on neural activity
std::unique_ptr<Action> Brain::produceAction() {
    // Simple action production based on motor neuron activity
    auto action = std::make_unique<Action>();
    action->setType(ActionType::Wait);  // Default action
    
    // Check motor neuron activity
    float maxMotorActivity = 0.0f;
    for (Neuron* neuron : pImpl->motorNeurons) {
        if (neuron && neuron->getFiringState() == FiringState::Active) {
            float activity = neuron->getMembranePotential();
            if (activity > maxMotorActivity) {
                maxMotorActivity = activity;
            }
        }
    }
    
    if (maxMotorActivity > 0.5f) {
        action->setType(ActionType::MoveForward);
    }
    
    return action;
}

// Reset brain state
void Brain::reset() {
    // Reset all components
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    // Reset memories
    if (pImpl->workingMemory) {
        pImpl->workingMemory->reset();
    }
    
    NLM_LOG_INFO("Brain state reset");
}

// Save brain state to file (checkpointing)
bool Brain::save(const std::string& filepath) const {
    return BrainCheckpointHandler::saveCheckpoint(*pImpl, filepath);
}

// Load brain state from file
bool Brain::load(const std::string& filepath) {
    return BrainCheckpointHandler::loadCheckpoint(*pImpl, filepath);
}

// Region management
RegionId Brain::addRegion(const std::string& name) {
    pImpl->nextRegionId = pImpl->regions.size() + 1;
    pImpl->regions.push_back(std::make_unique<NeuralRegion>(pImpl->nextRegionId, name));
    return pImpl->nextRegionId;
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
    return const_cast<Brain*>(this)->getRegion(id);
}

// Configuration getters
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

// Development system access
DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// Get all required methods for the interface...
// (Continuing with remaining methods for interface completeness)

// Memory systems access
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// Prediction system access
PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// Cognition systems access
NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// Neuromodulation systems access
Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
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

// Get configuration
std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

// Get random generator
RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

// Get other system access methods...

// Statistics methods
// (These would continue with the full implementation)

// Enable/disable system
void Brain::enableSystem(const std::string& systemName, bool enable) {
    // Implementation would enable specific systems
    NLM_LOG_INFO(std::string("System ") + systemName + " " + 
                 std::string(enable ? "enabled" : "disabled"));
}

bool Brain::isSystemEnabled(const std::string& systemName) const {
    // Return whether system is enabled
    return true;
}

// Validation and health checks
bool Brain::validateSystemState() const {
    return !pImpl->regions.empty() && pImpl->rng != nullptr;
}

std::string Brain::getSystemStatus() const {
    std::stringstream ss;
    ss << "Brain Status:\n";
    ss << "  Regions: " << pImpl->regions.size() << "\n";
    ss << "  Total Neurons: " << getTotalNeuronCount() << "\n";
    ss << "  Current Step: " << pImpl->currentStep << "\n";
    ss << "  Developmental Stage: " << static_cast<int>(pImpl->developmentalStage) << "\n";
    ss << "  Systems Active: " << (validateSystemState() ? "Yes" : "No");
    return ss.str();
}

// Performance monitoring
double Brain::getPerformanceMetrics() const {
    // Calculate and return performance metrics
    return 0.0;
}

void Brain::logPerformanceStats() const {
    NLM_LOG_INFO(getSystemStatus());
}

// Debug functionality
void Brain::debugSystem() const {
    NLM_LOG_DEBUG("=== Brain Debug Information ===");
    NLM_LOG_DEBUG(std::string("Regions: ") + std::to_string(pImpl->regions.size()));
    NLM_LOG_DEBUG(std::string("Sensory Neurons: ") + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_DEBUG(std::string("Motor Neurons: ") + std::to_string(pImpl->motorNeurons.size()));
}

void Brain::setDebugLevel(int level) {
    // Set debug logging level
    // Implementation would configure logger
}

int Brain::getDebugLevel() const {
    return 0;  // Default debug level
}

} // namespace nlm
