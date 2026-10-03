#include "IntegrationSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

struct IntegrationSystem::Impl {
    // System references
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<WorkingMemory> workingMem_;
    std::shared_ptr<EpisodicMemory> episodicMem_;
    std::shared_ptr<SemanticMemory> semanticMem_;
    std::shared_ptr<ProceduralMemory> proceduralMem_;
    std::shared_ptr<AssociativeMemory> associativeMem_;
    std::shared_ptr<PredictionSystem> predictionSys_;
    std::shared_ptr<Neuromodulator> neuromodulator_;
    std::shared_ptr<DevelopmentSystem> developmentSys_;
    
    // Integration state
    bool memoryPredictionEnabled_;
    bool neuromodulationMemoryEnabled_;
    bool replayEnabled_;
    bool persistenceEnabled_;
    bool phase6Enabled_;
    
    // Connection flags
    bool memoryPredictionConnected_;
    bool neuromodulationMemoryConnected_;
    bool developmentConnected_;
    bool agentWorldConnected_;
    bool persistenceConnected_;
    
    // Replay and consolidation
    std::vector<EpisodicMemoryItem> replayBuffer_;
    std::chrono::steady_clock::time_point lastReplayTime_;
    float consolidationThreshold_;
    
    // Timing and statistics
    IntegrationStats stats_;
    double lastStepTime_;
    double totalIntegrationTime_;
    size_t integrationSteps_;
};

IntegrationSystem::IntegrationSystem(std::shared_ptr<Brain> brain,
                                     std::shared_ptr<AgentBrain> agent,
                                     std::shared_ptr<WorkingMemory> workingMem,
                                     std::shared_ptr<EpisodicMemory> episodicMem,
                                     std::shared_ptr<SemanticMemory> semanticMem,
                                     std::shared_ptr<ProceduralMemory> proceduralMem,
                                     std::shared_ptr<AssociativeMemory> associativeMem,
                                     std::shared_ptr<PredictionSystem> predictionSys,
                                     std::shared_ptr<Neuromodulator> neuromodulator,
                                     std::shared_ptr<DevelopmentSystem> developmentSys)
    : pImpl(std::make_unique<Impl>())
    , phase6Enabled_(false)
    , memoryPredictionEnabled_(true)
    , neuromodulationMemoryEnabled_(true)
    , replayEnabled_(true)
    , persistenceEnabled_(true)
{
    // Store system references
    pImpl->brain_ = brain;
    pImpl->agent_ = agent;
    pImpl->workingMem_ = workingMem;
    pImpl->episodicMem_ = episodicMem;
    pImpl->semanticMem_ = semanticMem;
    pImpl->proceduralMem_ = proceduralMem;
    pImpl->associativeMem_ = associativeMem;
    pImpl->predictionSys_ = predictionSys;
    pImpl->neuromodulator_ = neuromodulator;
    pImpl->developmentSys_ = developmentSys;
    
    // Initialize statistics
    pImpl->stats_ = IntegrationStats{};
    pImpl->lastStepTime_ = 0.0;
    pImpl->totalIntegrationTime_ = 0.0;
    pImpl->integrationSteps_ = 0;
    pImpl->consolidationThreshold_ = 0.7f;
    
    NLM_LOG_INFO("IntegrationSystem initialized with " + 
                 std::to_string(pImpl->integrationSteps_) + " systems");
}

IntegrationSystem::~IntegrationSystem() = default;

void IntegrationSystem::integrateStep(SimulationStep step, TimestepDuration dt) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Phase 6 features check
    if (!phase6Enabled_) {
        enablePhase6Features();
    }
    
    // Main integration loop
    if (memoryPredictionEnabled_ && pImpl->memoryPredictionConnected_) {
        integrateMemoryPrediction();
    }
    
    if (neuromodulationMemoryEnabled_ && pImpl->neuromodulationMemoryConnected_) {
        integrateNeuromodulationMemory();
    }
    
    if (pImpl->developmentConnected_) {
        integrateDevelopmentSystem();
    }
    
    if (pImpl->agentWorldConnected_) {
        integrateAgentWorld();
    }
    
    if (persistenceEnabled_ && pImpl->persistenceConnected_) {
        integratePersistence();
    }
    
    // Trigger replay if needed
    if (replayEnabled_ && shouldTriggerReplay()) {
        triggerMemoryReplay();
    }
    
    // Update statistics
    pImpl->integrationSteps_++;
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->totalIntegrationTime_ += duration;
    pImpl->lastStepTime_ = duration;
    
    NLM_LOG_DEBUG(std::to_string(duration * 1000) + "ms integration time for step " +
                 std::to_string(step));
}

void IntegrationSystem::establishSystemConnections() {
    NLM_LOG_INFO("Establishing system connections for Phase 6 integration");
    
    // Memory-Prediction connections
    if (memoryPredictionEnabled_) {
        integrateMemoryPrediction();
        pImpl->memoryPredictionConnected_ = true;
        NLM_LOG_INFO("Memory-Prediction integration connected");
    }
    
    // Neuromodulation-Memory connections
    if (neuromodulationMemoryEnabled_) {
        integrateNeuromodulationMemory();
        pImpl->neuromodulationMemoryConnected_ = true;
        NLM_LOG_INFO("Neuromodulation-Memory integration connected");
    }
    
    // Development system connections
    if (pImpl->developmentSys_) {
        integrateDevelopmentSystem();
        pImpl->developmentConnected_ = true;
        NLM_LOG_INFO("Development-System integration connected");
    }
    
    // Agent-World connections
    if (pImpl->agent_ && pImpl->brain_) {
        integrateAgentWorld();
        pImpl->agentWorldConnected_ = true;
        NLM_LOG_INFO("Agent-World integration connected");
    }
    
    // Persistence connections
    if (persistenceEnabled_) {
        integratePersistence();
        pImpl->persistenceConnected_ = true;
        NLM_LOG_INFO("Persistence integration connected");
    }
}

void IntegrationSystem::integrateMemoryPrediction() {
    NLM_LOG_DEBUG("Integrating memory and prediction systems");
    
    // Use prediction error to update working memory
    if (pImpl->predictionSys_ && pImpl->workingMem_) {
        float predictionError = pImpl->predictionSys_->getPredictionError();
        pImpl->stats_.predictionMemoryIntegrations++;
        
        updateWorkingMemoryWithPredictionError(
            pImpl->workingMem_, predictionError, 
            pImpl->predictionSys_->getConfidence());
    }
    
    // Use prediction to enhance episodic memory
    if (pImpl->predictionSys_ && pImpl->episodicMem_) {
        // Generate prediction-based memory updates
        MemoryPredictionIntegration::predictAndUpdateMemory(
            /* Need to get current sensory input */, 
            pImpl->predictionSys_, 
            pImpl->episodicMem_);
    }
    
    // Update semantic associations based on prediction confidence
    if (pImpl->predictionSys_ && pImpl->semanticMem_ && pImpl->associativeMem_) {
        MemoryPredictionIntegration::enhanceMemoryBasedOnPrediction(
            pImpl->semanticMem_, pImpl->associativeMem_,
            pImpl->predictionSys_->getPredictionError(),
            pImpl->predictionSys_->getConfidence());
    }
}

void IntegrationSystem::onPredictionError(float error, float confidence) {
    NLM_LOG_DEBUG("Prediction error received: " + std::to_string(error) + ", confidence: " + std::to_string(confidence));
    
    IntegrationEvent event;
    event.type = IntegrationEvent::Type::PredictionError;
    event.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    event.parameters["error"] = error;
    event.parameters["confidence"] = confidence;
    
    notifyObservers(event);
}

void IntegrationSystem::onMemoryUpdate(const std::string& memoryType, const std::string& operation) {
    IntegrationEvent event;
    event.type = IntegrationEvent::Type::MemoryUpdate;
    event.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    event.parameters["memory_type"] = memoryType;
    event.parameters["operation"] = operation;
    
    notifyObservers(event);
}

void IntegrationSystem::integrateNeuromodulationMemory() {
    NLM_LOG_DEBUG("Integrating neuromodulation and memory systems");
    
    if (pImpl->neuromodulator_ && pImpl->episodicMem_) {
        // Get neuromodulator state
        auto neuromodState = pImpl->neuromodulator_->getState();
        
        // Enhance memory consolidation based on neuromodulator states
        NeuromodulationMemoryIntegration::consolidateMemoryWithNeuromodulation(
            pImpl->episodicMem_, pImpl->workingMem_, neuromodState);
        
        pImpl->stats_.neuromodulationMemoryIntegrations++;
    }
}

void IntegrationSystem::onNeuromodulationChange(const Neuromodulator::State& state) {
    NLM_LOG_DEBUG("Neuromodulation state changed: dopamine=" + std::to_string(state.dopamine) + 
                 ", novelty=" + std::to_string(state.novelty) + ", curiosity=" + std::to_string(state.curiosity));
    
    IntegrationEvent event;
    event.type = IntegrationEvent::Type::NeuromodulationChange;
    event.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    event.parameters["dopamine"] = state.dopamine;
    event.parameters["novelty"] = state.novelty;
    event.parameters["curiosity"] = state.curiosity;
    
    notifyObservers(event);
}

void IntegrationSystem::enhanceMemoryConsolidation(float dopamineLevel, float noveltyLevel) {
    NLM_LOG_DEBUG("Enhancing memory consolidation with dopamine=" + std::to_string(dopamineLevel) + 
                 ", novelty=" + std::to_string(noveltyLevel));
    
    IntegrationEvent event;
    event.type = IntegrationEvent::Type::Consolidation;
    event.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    event.parameters["dopamine"] = dopamineLevel;
    event.parameters["novelty"] = noveltyLevel;
    event.parameters["threshold"] = pImpl->consolidationThreshold_;
    
    notifyObservers(event);
}

void IntegrationSystem::integrateDevelopmentSystem() {
    NLM_LOG_DEBUG("Integrating development system");
    
    if (pImpl->developmentSys_ && pImpl->workingMem_ && pImpl->episodicMem_) {
        // Get current developmental stage
        auto stage = pImpl->developmentSys_->getCurrentStage();
        
        // Adapt memory systems for developmental stage
        DevelopmentSystemIntegration::adaptMemorySystemsForDevelopment(
            pImpl->workingMem_, pImpl->episodicMem_, stage);
        
        onDevelopmentStageChange(stage);
    }
}

void IntegrationSystem::onDevelopmentStageChange(DevelopmentalStage stage) {
    NLM_LOG_DEBUG("Development stage changed to: " + std::to_string(static_cast<int>(stage)));
    
    IntegrationEvent event;
    event.type = IntegrationEvent::Type::DevelopmentStageChange;
    event.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    event.parameters["stage"] = static_cast<float>(static_cast<int>(stage));
    
    notifyObservers(event);
}

void IntegrationSystem::adaptMemoryForDevelopment() {
    // Adapt memory systems based on developmental stage
    if (pImpl->developmentSys_ && pImpl->associativeMem_) {
        auto stage = pImpl->developmentSys_->getCurrentStage();
        float plasticity = pImpl->developmentSys_->getPlasticityModifier();
        
        DevelopmentSystemIntegration::integrateDevelopmentPlasticity(
            pImpl->associativeMem_, plasticity, stage);
    }
}

void IntegrationSystem::integrateAgentWorld() {
    NLM_LOG_DEBUG("Integrating agent and world systems");
    
    if (pImpl->agent_ && pImpl->episodicMem_) {
        // Check if agent has performed any actions recently
        auto lastActionTime = pImpl->agent_->getLastActionTime();
        auto currentTime = std::chrono::steady_clock::now();
        
        auto duration = std::chrono::duration<double>(currentTime - lastActionTime).count();
        
        if (duration < 1.0) { // Record recent actions as episodic memories
            recordAgentActionAsEpisodicMemory();
        }
    }
}

void IntegrationSystem::onActionExecuted(const MotorCommand& action) {
    IntegrationEvent event;
    event.type = IntegrationEvent::Type::ActionExecuted;
    event.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    event.parameters["action"] = static_cast<float>(static_cast<int>(action));
    
    notifyObservers(event);
}

void IntegrationSystem::onWorldInteraction(const WorldInteraction& interaction) {
    IntegrationEvent event;
    event.type = IntegrationEvent::Type::WorldInteraction;
    event.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    event.parameters["interaction_type"] = static_cast<float>(static_cast<int>(interaction.type));
    
    notifyObservers(event);
}

void IntegrationSystem::integratePersistence() {
    NLM_LOG_DEBUG("Integrating persistence systems");
    
    if (persistenceEnabled_) {
        // Auto-save periodically
        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(currentTime - pImpl->lastReplayTime_).count();
        
        if (duration > 30.0) { // Save every 30 seconds
            saveIntegrationState("auto_save_" + std::to_string(std::chrono::duration<double>(currentTime.time_since_epoch()).count()) + ".nlm");
            pImpl->lastReplayTime_ = currentTime;
        }
    }
}

void IntegrationSystem::saveIntegrationState(const std::string& filename) {
    NLM_LOG_INFO("Saving integration state to: " + filename);
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for saving: " + filename);
        return;
    }
    
    // Save integration statistics
    file.write(reinterpret_cast<const char*>(&pImpl->stats_), sizeof(IntegrationStats));
    
    // Save system states
    if (pImpl->workingMem_) {
        // Export working memory state
        auto workingMemState = exportWorkingMemoryState(*pImpl->workingMem_);
        file.write(workingMemState.data(), workingMemState.size());
    }
    
    if (pImpl->episodicMem_) {
        // Export episodic memory state
        auto episodicMemState = exportEpisodicMemoryState(*pImpl->episodicMem_);
        file.write(episodicMemState.data(), episodicMemState.size());
    }
    
    file.close();
}

void IntegrationSystem::loadIntegrationState(const std::string& filename) {
    NLM_LOG_INFO("Loading integration state from: " + filename);
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for loading: " + filename);
        return;
    }
    
    // Load integration statistics
    file.read(reinterpret_cast<char*>(&pImpl->stats_), sizeof(IntegrationStats));
    
    // Load system states (simplified implementation)
    // In a real implementation, we would properly deserialize all system states
    
    file.close();
}

void IntegrationSystem::triggerMemoryReplay() {
    NLM_LOG_DEBUG("Triggering memory replay");
    
    IntegrationEvent event;
    event.type = IntegrationEvent::Type::MemoryReplay;
    event.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    
    notifyObservers(event);
    
    pImpl->stats_.memoryReplayEvents++;
}

void IntegrationSystem::registerObserver(IntegrationObserver* observer) {
    observers_.push_back(observer);
}

void IntegrationSystem::unregisterObserver(IntegrationObserver* observer) {
    auto it = std::find(observers_.begin(), observers_.end(), observer);
    if (it != observers_.end()) {
        observers_.erase(it);
    }
}

void IntegrationSystem::notifyObservers(const IntegrationEvent& event) {
    for (auto* observer : observers_) {
        observer->onIntegrationEvent(event);
    }
}

void IntegrationSystem::configureMemoryPredictionIntegration(bool enable) {
    memoryPredictionEnabled_ = enable;
    if (enable) {
        integrateMemoryPrediction();
    }
}

void IntegrationSystem::configureNeuromodulationMemoryIntegration(bool enable) {
    neuromodulationMemoryEnabled_ = enable;
    if (enable) {
        integrateNeuromodulationMemory();
    }
}

void IntegrationSystem::configureReplayMechanism(bool enable) {
    replayEnabled_ = enable;
}

void IntegrationSystem::configurePersistence(bool enable) {
    persistenceEnabled_ = enable;
}

bool IntegrationSystem::validateIntegration() const {
    bool valid = true;
    
    if (!pImpl->brain_) valid = false;
    if (!pImpl->agent_) valid = false;
    if (!pImpl->workingMem_) valid = false;
    if (!pImpl->episodicMem_) valid = false;
    if (!pImpl->semanticMem_) valid = false;
    if (!pImpl->associativeMem_) valid = false;
    if (!pImpl->predictionSys_) valid = false;
    if (!pImpl->neuromodulator_) valid = false;
    if (!pImpl->developmentSys_) valid = false;
    
    return valid;
}

std::string IntegrationSystem::getIntegrationReport() const {
    std::stringstream report;
    report << "=== NLM Phase 6 Integration Report ===\n";
    report << "Integration Statistics:\n";
    report << "  Prediction-Memory Integrations: " << pImpl->stats_.predictionMemoryIntegrations << "\n";
    report << "  Neuromodulation-Memory Integrations: " << pImpl->stats_.neuromodulationMemoryIntegrations << "\n";
    report << "  Memory Replay Events: " << pImpl->stats_.memoryReplayEvents << "\n";
    report << "  Consolidation Events: " << pImpl->stats_.consolidationEvents << "\n";
    report << "  Average Prediction Error: " << pImpl->stats_.averagePredictionError << "\n";
    report << "  Memory Retention Rate: " << pImpl->stats_.memoryRetentionRate << "\n";
    report << "  Active Connections: " << pImpl->stats_.activeConnections << "\n";
    report << "  Total Integration Time: " << pImpl->stats_.totalIntegrationTime << "s\n";
    report << "\n=== System Connections ===\n";
    report << "  Memory-Prediction: " << (pImpl->memoryPredictionConnected_ ? "Connected" : "Disconnected") << "\n";
    report << "  Neuromodulation-Memory: " << (pImpl->neuromodulationMemoryConnected_ ? "Connected" : "Disconnected") << "\n";
    report << "  Development System: " << (pImpl->developmentConnected_ ? "Connected" : "Disconnected") << "\n";
    report << "  Agent-World: " << (pImpl->agentWorldConnected_ ? "Connected" : "Disconnected") << "\n";
    report << "  Persistence: " << (pImpl->persistenceConnected_ ? "Connected" : "Disconnected") << "\n";
    report << "\n=== Phase 6 Features ===\n";
    report << "  Phase 6 Enabled: " << (phase6Enabled_ ? "Yes" : "No") << "\n";
    report << "  Replay Enabled: " << (replayEnabled_ ? "Yes" : "No") << "\n";
    report << "  Persistence Enabled: " << (persistenceEnabled_ ? "Yes" : "No") << "\n";
    
    return report.str();
}

void IntegrationSystem::enablePhase6Features() {
    NLM_LOG_INFO("Enabling Phase 6 integration features");
    
    // Enable all Phase 6 features
    phase6Enabled_ = true;
    replayEnabled_ = true;
    persistenceEnabled_ = true;
    
    // Establish all system connections
    establishSystemConnections();
    
    NLM_LOG_INFO("Phase 6 integration features enabled");
}

std::unique_ptr<IntegrationSystem> createIntegrationSystem(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<AgentBrain> agent,
    std::shared_ptr<WorkingMemory> workingMem,
    std::shared_ptr<EpisodicMemory> episodicMem,
    std::shared_ptr<SemanticMemory> semanticMem,
    std::shared_ptr<ProceduralMemory> proceduralMem,
    std::shared_ptr<AssociativeMemory> associativeMem,
    std::shared_ptr<PredictionSystem> predictionSys,
    std::shared_ptr<Neuromodulator> neuromodulator,
    std::shared_ptr<DevelopmentSystem> developmentSys)
{
    return std::make_unique<IntegrationSystem>(
        brain, agent, workingMem, episodicMem, semanticMem,
        proceduralMem, associativeMem, predictionSys,
        neuromodulator, developmentSys);
}

// Implementation of IntegrationEvent helper functions
bool IntegrationEvent::operator==(const IntegrationEvent& other) const {
    return type == other.type && 
           std::abs(timestamp - other.timestamp) < 0.001 &&
           parameters == other.parameters;
}

// Memory-Prediction integration implementations
void MemoryPredictionIntegration::predictAndUpdateMemory(
    const SensoryInput& currentState,
    std::shared_ptr<PredictionSystem> prediction,
    std::shared_ptr<EpisodicMemory> episodic) {
    if (!prediction || !episodic) return;
    
    // Generate prediction from current state
    auto predictedState = prediction->predictNextState(currentState);
    
    // Create episodic memory of prediction
    EpisodicMemoryItem predictionMemory;
    predictionMemory.timestamp = prediction->getCurrentStep();
    predictionMemory.metadata = "prediction";
    
    episodic->storeEpisode(predictionMemory);
}

void MemoryPredictionIntegration::updateWorkingMemoryWithPredictionError(
    std::shared_ptr<WorkingMemory> workingMem,
    float predictionError,
    float confidence) {
    if (!workingMem) return;
    
    // Update working memory based on prediction error
    float weight = confidence * (1.0f - predictionError);
    
    // Decay working memory
    workingMem->decay(weight);
}

void MemoryPredictionIntegration::enhanceMemoryBasedOnPrediction(
    std::shared_ptr<SemanticMemory> semantic,
    std::shared_ptr<AssociativeMemory> associative,
    float predictionError,
    float confidence) {
    if (!semantic || !associative) return;
    
    // Update semantic facts based on prediction accuracy
    if (confidence > 0.8f) {
        semantic->storeFact("prediction_confidence", std::to_string(confidence));
    }
    
    // Create associations based on prediction patterns
    if (predictionError < 0.1f) {
        associative->associate(1, 2, confidence); // Example association
    }
}

// Neuromodulation-Memory integration implementations
void NeuromodulationMemoryIntegration::consolidateMemoryWithNeuromodulation(
    std::shared_ptr<EpisodicMemory> episodic,
    std::shared_ptr<WorkingMemory> workingMem,
    const Neuromodulator::State& neuromodState) {
    if (!episodic || !workingMem) return;
    
    // Enhance consolidation based on neuromodulator levels
    if (neuromodState.dopamine > 0.5f) {
        // Strong dopamine enhances consolidation
        episodic->consolidate(0.8f);
    }
    
    if (neuromodState.novelty > 0.7f) {
        // High novelty focuses memory encoding
        workingMem->decay(0.1f);
    }
}

void NeuromodulationMemoryIntegration::dopamineEnhancedMemoryStrengthening(
    std::shared_ptr<EpisodicMemory> episodic,
    float dopamineLevel,
    float predictionError) {
    if (!episodic) return;
    
    // Dopamine enhances memory based on prediction error
    float consolidationStrength = dopamineLevel * (1.0f - predictionError);
    episodic->consolidate(consolidationStrength);
}

void NeuromodulationMemoryIntegration::noveltyDrivenMemoryEncoding(
    std::shared_ptr<WorkingMemory> workingMem,
    float noveltyLevel) {
    if (!workingMem) return;
    
    // High novelty increases encoding efficiency
    if (noveltyLevel > 0.6f) {
        workingMem->decay(0.05f);
    }
}

// Development-System integration implementations
void DevelopmentSystemIntegration::adaptMemorySystemsForDevelopment(
    std::shared_ptr<WorkingMemory> workingMem,
    std::shared_ptr<EpisodicMemory> episodic,
    DevelopmentalStage stage) {
    if (!workingMem || !episodic) return;
    
    // Adapt memory parameters based on developmental stage
    switch (stage) {
        case DevelopmentalStage::Initial:
            workingMem->setCapacity(50); // Smaller capacity for young brains
            workingMem->setDecayRate(0.1f); // Faster decay
            break;
        case DevelopmentalStage::CriticalPeriod:
            workingMem->setCapacity(100); // Growing capacity
            workingMem->setDecayRate(0.05f); // Slower decay
            break;
        case DevelopmentalStage::Maturation:
            workingMem->setCapacity(150); // Larger capacity
            workingMem->setDecayRate(0.02f); // Slow decay
            break;
        case DevelopmentalStage::Adult:
            workingMem->setCapacity(200); // Full capacity
            workingMem->setDecayRate(0.01f); // Minimal decay
            break;
        case DevelopmentalStage::Aging:
            workingMem->setCapacity(100); // Reduced capacity
            workingMem->setDecayRate(0.05f); // Faster decay
            break;
    }
}

void DevelopmentSystemIntegration::integrateDevelopmentPlasticity(
    std::shared_ptr<AssociativeMemory> associative,
    float plasticityModifier,
    DevelopmentalStage stage) {
    if (!associative) return;
    
    // Modulate association strength based on plasticity
    if (plasticityModifier > 1.0f) {
        // High plasticity strengthens associations
        associative->updateAssociation(1, 2, 0.1f);
    } else if (plasticityModifier < 0.5f) {
        // Low plasticity weakens associations
        associative->updateAssociation(1, 2, -0.05f);
    }
}

// Persistence integration implementations
void PersistenceIntegration::saveIntegrationState(
    const IntegrationSystem& integration,
    const std::string& filename) {
    // Save integration state to file
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        // Write integration statistics
        auto stats = integration.getStats();
        file.write(reinterpret_cast<const char*>(&stats), sizeof(IntegrationStats));
        file.close();
    }
}

void PersistenceIntegration::loadIntegrationState(
    IntegrationSystem& integration,
    const std::string& filename) {
    // Load integration state from file
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        // Read integration statistics
        IntegrationStats stats;
        file.read(reinterpret_cast<char*>(&stats), sizeof(IntegrationStats));
        file.close();
    }
}

// Helper functions for integration system
std::string exportWorkingMemoryState(const WorkingMemory& workingMem) {
    std::stringstream stream;
    stream.write(reinterpret_cast<const char*>(&workingMem), sizeof(WorkingMemory));
    return stream.str();
}

std::string exportEpisodicMemoryState(const EpisodicMemory& episodicMem) {
    std::stringstream stream;
    stream.write(reinterpret_cast<const char*>(&episodicMem), sizeof(EpisodicMemory));
    return stream.str();
}

bool IntegrationSystem::shouldTriggerReplay() {
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(currentTime - pImpl->lastReplayTime_).count();
    
    // Trigger replay every 10 seconds
    return duration > 10.0;
}

void IntegrationSystem::recordAgentActionAsEpisodicMemory() {
    if (!pImpl->agent_ || !pImpl->episodicMem_) return;
    
    // Create episodic memory of agent action
    EpisodicMemoryItem actionMemory;
    actionMemory.timestamp = pImpl->agent_->getCurrentStep();
    actionMemory.metadata = "agent_action";
    
    // Add action information to memory
    pImpl->episodicMem_->storeEpisode(actionMemory);
}

} // namespace nlm
