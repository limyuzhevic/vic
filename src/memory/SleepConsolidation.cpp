#include "SleepConsolidation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct SleepConsolidation::Impl {
    Brain* brain;
    
    // Consolidation parameters for each state
    std::map<SleepState, StateConsolidationParameters> consolidationParameters;
    
    // Statistics
    float systemsConsolidationProgress;
    float synapticConsolidationProgress;
    float schemaFormationProgress;
    float memoryIntegrationProgress;
    float consolidationEfficiency;
    size_t totalConsolidationEvents;
    
    // Current consolidation state
    float currentConsolidationProgress;
    SleepState currentConsolidationState;
    bool consolidationActive;
    
    // Memory consolidation buffers
    std::vector<float> memoryBuffer;
    std::vector<float> schemaBuffer;
    std::deque<float> recentConsolidationMetrics;
    
    // Error handling
    bool inErrorState;
    std::string lastErrorMessage;
    size_t consecutiveErrors;
    
    // Consolidation timing
    float consolidationInterval;
    float consolidationDuration;
    float nextConsolidationStep;
    
    Impl() : brain(nullptr)
        , systemsConsolidationProgress(0.0f)
        , synapticConsolidationProgress(0.0f)
        , schemaFormationProgress(0.0f)
        , memoryIntegrationProgress(0.0f)
        , consolidationEfficiency(0.0f)
        , totalConsolidationEvents(0)
        , currentConsolidationProgress(0.0f)
        , currentConsolidationState(SleepState::Wake)
        , consolidationActive(false)
        , inErrorState(false)
        , consecutiveErrors(0)
        , consolidationInterval(1.0f)
        , consolidationDuration(0.0f)
        , nextConsolidationStep(0.0f) {
        
        // Initialize state-specific parameters
        consolidationParameters[SleepState::QuietSleep] = {
            0.2f,    // systemsConsolidationStrength
            0.5f,    // synapticConsolidationStrength
            0.1f,    // schemaFormationStrength
            0.3f,    // memoryIntegrationStrength
            0.4f,    // abstractionLevel
            0.5f,    // generalizationCapacity
            true,    // enableReorganization
            0.7f     // neuromodulationSensitivity
        };
        
        consolidationParameters[SleepState::DeepSleep] = {
            0.8f,    // systemsConsolidationStrength
            0.6f,    // synapticConsolidationStrength
            0.4f,    // schemaFormationStrength
            0.7f,    // memoryIntegrationStrength
            0.7f,    // abstractionLevel
            0.8f,    // generalizationCapacity
            true,    // enableReorganization
            0.6f     // neuromodulationSensitivity
        };
        
        consolidationParameters[SleepState::REMSleep] = {
            0.4f,    // systemsConsolidationStrength
            0.3f,    // synapticConsolidationStrength
            0.9f,    // schemaFormationStrength
            0.8f,    // memoryIntegrationStrength
            0.9f,    // abstractionLevel
            0.7f,    // generalizationCapacity
            true,    // enableReorganization
            1.0f     // neuromodulationSensitivity
        };
        
        consolidationParameters[SleepState::Wake] = {
            0.0f,    // systemsConsolidationStrength
            0.0f,    // synapticConsolidationStrength
            0.0f,    // schemaFormationStrength
            0.0f,    // memoryIntegrationStrength
            0.0f,    // abstractionLevel
            0.0f,    // generalizationCapacity
            false,   // enableReorganization
            0.0f     // neuromodulationSensitivity
        };
    }
};

SleepConsolidation::SleepConsolidation()
    : pImpl(new Impl)
    , brain_(nullptr)
    , systemsConsolidationProgress_(0.0f)
    , synapticConsolidationProgress_(0.0f)
    , schemaFormationProgress_(0.0f)
    , memoryIntegrationProgress_(0.0f)
    , consolidationEfficiency_(0.0f)
    , totalConsolidationEvents_(0)
    , currentConsolidationProgress_(0.0f)
    , currentConsolidationState_(SleepState::Wake)
    , consolidationActive_(false)
    , inErrorState_(false)
    , consecutiveErrors_(0)
    , consolidationInterval_(1.0f)
    , consolidationDuration_(0.0f)
    , nextConsolidationStep_(0.0f) {
}

SleepConsolidation::~SleepConsolidation() = default;

void SleepConsolidation::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SleepConsolidation initialized");
}

void SleepConsolidation::systemsConsolidation(SleepState sleepState) {
    const auto& params = pImpl->consolidationParameters[sleepState];
    
    // Apply systems consolidation based on sleep state
    switch (sleepState) {
        case SleepState::QuietSleep:
            // Initial systems consolidation
            systemsConsolidationProgress_ += params.systemsConsolidationStrength * 0.3f;
            break;
        case SleepState::DeepSleep:
            // Strong systems consolidation during deep sleep
            systemsConsolidationProgress_ += params.systemsConsolidationStrength * 1.0f;
            break;
        case SleepState::REMSleep:
            // Integration during REM sleep
            systemsConsolidationProgress_ += params.systemsConsolidationStrength * 0.6f;
            break;
        case SleepState::Wake:
            // No systems consolidation during wake
            break;
    }
    
    updateConsolidationStatistics(systemsConsolidationProgress_, true);
}

void SleepConsolidation::consolidateMemoryNetworks(SleepState sleepState) {
    // Consolidate memory networks based on sleep state
    if (sleepState == SleepState::DeepSleep) {
        // Reorganize network connections during deep sleep
        reorganizeMemoryNetworks();
    }
}

void SleepConsolidation::reorganizeMemorySchemas(SleepState sleepState) {
    // Reorganize memory schemas based on sleep state
    if (sleepState == SleepState::REMSleep) {
        // Form new schemas during REM sleep
        formNewSchemas();
    }
}

void SleepConsolidation::synapticConsolidation(SleepState sleepState) {
    const auto& params = pImpl->consolidationParameters[sleepState];
    
    // Apply synaptic consolidation based on sleep state
    switch (sleepState) {
        case SleepState::QuietSleep:
            // Synaptic strengthening during quiet sleep
            synapticConsolidationProgress_ += params.synapticConsolidationStrength * 0.7f;
            break;
        case SleepState::DeepSleep:
            // Synaptic downscaling during deep sleep
            synapticConsolidationProgress_ += params.synapticConsolidationStrength * 0.5f;
            break;
        case SleepState::REMSleep:
            // Synaptic integration during REM sleep
            synapticConsolidationProgress_ += params.synapticConsolidationStrength * 0.8f;
            break;
        case SleepState::Wake:
            // No synaptic consolidation during wake
            break;
    }
    
    updateConsolidationStatistics(synapticConsolidationProgress_, true);
}

void SleepConsolidation::strengthenMemoryTraces(SleepState sleepState) {
    if (!brain_) return;
    
    const auto& params = pImpl->consolidationParameters[sleepState];
    
    // Strengthen memory traces based on sleep state
    // This would find and strengthen weak memory connections
    
    // Simulate strengthening
    float strengtheningEffect = params.synapticConsolidationStrength * 100.0f;
    synapticConsolidationProgress_ += strengtheningEffect;
}

void SleepConsolidation::weakenIrrelevantConnections(SleepState sleepState) {
    if (!brain_) return;
    
    const auto& params = pImpl->consolidationParameters[sleepState];
    
    // Weaken irrelevant connections based on sleep state
    if (sleepState == SleepState::DeepSleep) {
        // Strong downscaling during deep sleep
        float downscalingEffect = 0.8f * 1000.0f;
        synapticConsolidationProgress_ -= downscalingEffect; // Negative effect
    }
}

void SleepConsolidation::schemaFormation(SleepState sleepState) {
    const auto& params = pImpl->consolidationParameters[sleepState];
    
    // Apply schema formation based on sleep state
    if (sleepState == SleepState::REMSleep) {
        // Strong schema formation during REM sleep
        schemaFormationProgress_ += params.schemaFormationStrength * 0.9f;
    } else if (sleepState == SleepState::QuietSleep) {
        // Initial schema formation during quiet sleep
        schemaFormationProgress_ += params.schemaFormationStrength * 0.3f;
    }
    
    updateConsolidationStatistics(schemaFormationProgress_, true);
}

void SleepConsolidation::abstractMemoryRepresentations(SleepState sleepState) {
    // Abstract memory representations based on sleep state
    if (sleepState == SleepState::REMSleep) {
        // High-level abstraction during REM sleep
        float abstractionEffect = 0.9f * 100.0f;
        schemaFormationProgress_ += abstractionEffect;
    }
}

void SleepConsolidation::generalizeMemoryKnowledge(SleepState sleepState) {
    // Generalize memory knowledge based on sleep state
    if (sleepState == SleepState::REMSleep) {
        // Generalization during REM sleep
        float generalizationEffect = 0.7f * 100.0f;
        schemaFormationProgress_ += generalizationEffect;
    }
}

void SleepConsolidation::memoryIntegration(SleepState sleepState) {
    const auto& params = pImpl->consolidationParameters[sleepState];
    
    // Apply memory integration based on sleep state
    if (sleepState == SleepState::REMSleep) {
        // Strong integration during REM sleep
        memoryIntegrationProgress_ += params.memoryIntegrationStrength * 0.8f;
    } else if (sleepState == SleepState::DeepSleep) {
        // Moderate integration during deep sleep
        memoryIntegrationProgress_ += params.memoryIntegrationStrength * 0.5f;
    }
    
    updateConsolidationStatistics(memoryIntegrationProgress_, true);
}

void SleepConsolidation::integrateNewMemories(SleepState sleepState) {
    // Integrate new memories based on sleep state
    if (sleepState == SleepState::REMSleep) {
        // Strong integration during REM sleep
        float integrationEffect = 0.8f * 100.0f;
        memoryIntegrationProgress_ += integrationEffect;
    }
}

void SleepConsolidation::generalizeToNewContexts(SleepState sleepState) {
    // Generalize memories to new contexts based on sleep state
    if (sleepState == SleepState::REMSleep) {
        // Generalization during REM sleep
        float generalizationEffect = 0.6f * 100.0f;
        memoryIntegrationProgress_ += generalizationEffect;
    }
}

const SleepConsolidation::StateConsolidationParameters& SleepConsolidation::getConsolidationParameters(SleepState state) const {
    auto it = pImpl->consolidationParameters.find(state);
    if (it != pImpl->consolidationParameters.end()) {
        return it->second;
    }
    // Return default parameters for unknown state
    static StateConsolidationParameters defaultParams = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false, 0.0f};
    return defaultParams;
}

void SleepConsolidation::setConsolidationParameters(SleepState state, const StateConsolidationParameters& params) {
    if (validateConsolidationParameters(params)) {
        pImpl->consolidationParameters[state] = params;
    }
}

void SleepConsolidation::integrateWithHebbianPlasticity(Hebbian* hebbian) {
    // Integrate with Hebbian plasticity system
    if (hebbian) {
        // Configure Hebbian learning with consolidation parameters
        float learningRate = 0.01f;  // Base learning rate
        hebbian->configure(learningRate, 0.01f);
    }
}

void SleepConsolidation::integrateWithEpisodicMemory(NeuralEpisodicMemory* episodicMemory) {
    // Integrate with episodic memory system
    if (episodicMemory) {
        // Configure episodic memory consolidation parameters
        episodicMemory->setMaxEpisodes(2000);  // Increase capacity
    }
}

void SleepConsolidation::integrateWithWorkingMemory(NeuralWorkingMemory* workingMemory) {
    // Integrate with working memory system
    if (workingMemory) {
        // Configure working memory consolidation parameters
        workingMemory->setDecayRate(0.005f);  // Slower decay during consolidation
    }
}

void SleepConsolidation::integrateWithNeuromodulationSystems() {
    // Integrate with neuromodulation systems
    if (brain_ && brain_->getDopamine()) {
        // Configure neuromodulation integration
        // In a full implementation, this would set up neuromodulation integration
    }
}

void SleepConsolidation::scheduleConsolidation(SleepState sleepState, float duration) {
    currentConsolidationState_ = sleepState;
    consolidationDuration_ = duration;
    consolidationActive_ = true;
    currentConsolidationProgress_ = 0.0f;
    
    // Schedule consolidation based on sleep state
    switch (sleepState) {
        case SleepState::QuietSleep:
            consolidationInterval_ = 2.0f;
            break;
        case SleepState::DeepSleep:
            consolidationInterval_ = 5.0f;
            break;
        case SleepState::REMSleep:
            consolidationInterval_ = 1.0f;
            break;
        case SleepState::Wake:
            consolidationInterval_ = 0.0f;
            break;
    }
}

void SleepConsolidation::updateConsolidationTiming(SleepState sleepState, float dt) {
    // Update consolidation timing based on sleep state
    consolidationInterval_ += 0.1f * dt;
    
    // Update consolidation progress
    if (consolidationActive_) {
        currentConsolidationProgress_ += dt / consolidationDuration_;
    }
}

void SleepConsolidation::triggerConsolidationEvents(SleepState sleepState) {
    // Trigger consolidation events based on sleep state
    switch (sleepState) {
        case SleepState::QuietSleep:
            systemsConsolidation(sleepState);
            synapticConsolidation(sleepState);
            schemaFormation(sleepState);
            break;
        case SleepState::DeepSleep:
            systemsConsolidation(sleepState);
            synapticConsolidation(sleepState);
            reorganizeMemoryNetworks();
            break;
        case SleepState::REMSleep:
            systemsConsolidation(sleepState);
            synapticConsolidation(sleepState);
            schemaFormation(sleepState);
            memoryIntegration(sleepState);
            break;
        case SleepState::Wake:
            // No consolidation during wake
            break;
    }
}

void SleepConsolidation::updateConsolidationParameters() {
    // Update consolidation parameters based on current state
    // This would adjust parameters based on consolidation progress and metrics
}

void SleepConsolidation::executeStateSpecificConsolidation(SleepState state) {
    // Execute consolidation specific to the sleep state
    switch (state) {
        case SleepState::QuietSleep:
            systemsConsolidation(state);
            synapticConsolidation(state);
            schemaFormation(state);
            break;
        case SleepState::DeepSleep:
            systemsConsolidation(state);
            synapticConsolidation(state);
            reorganizeMemoryNetworks();
            break;
        case SleepState::REMSleep:
            systemsConsolidation(state);
            synapticConsolidation(state);
            schemaFormation(state);
            memoryIntegration(state);
            break;
        case SleepState::Wake:
            // No consolidation during wake
            break;
    }
}

void SleepConsolidation::updateConsolidationBuffers() {
    // Update consolidation buffers
    // This would update memoryBuffer_, schemaBuffer_, etc.
    
    // Simulate buffer updates
    float bufferUpdateEffect = 0.1f;
    memoryIntegrationProgress_ += bufferUpdateEffect;
}

void SleepConsolidation::integrateConsolidationWithPlasticity(SleepState state) {
    // Integrate consolidation with plasticity system
    if (brain_ && brain_->getHebbian()) {
        // Integrate with Hebbian plasticity
        integrateWithHebbianPlasticity(brain_->getHebbian());
    }
}

void SleepConsolidation::integrateConsolidationWithMemory(SleepState state) {
    // Integrate consolidation with memory systems
    if (brain_ && brain_->getEpisodicMemory()) {
        // Integrate with episodic memory
        integrateWithEpisodicMemory(brain_->getEpisodicMemory());
    }
    if (brain_ && brain_->getWorkingMemory()) {
        // Integrate with working memory
        integrateWithWorkingMemory(brain_->getWorkingMemory());
    }
}

void SleepConsolidation::updateConsolidationStatistics(float progress, bool success) {
    totalConsolidationEvents_++;
    
    // Calculate efficiency
    consolidationEfficiency_ = (progress * success) / totalConsolidationEvents_;
    
    if (success) {
        // Update progress
        currentConsolidationProgress_ = progress;
    }
}

void SleepConsolidation::resetConsolidationState() {
    currentConsolidationState_ = SleepState::Wake;
    currentConsolidationProgress_ = 0.0f;
    consolidationActive_ = false;
    inErrorState_ = false;
    recentConsolidationMetrics_.clear();
    consecutiveErrors_ = 0;
}

bool SleepConsolidation::validateConsolidationParameters(const StateConsolidationParameters& params) const {
    // Validate consolidation parameters
    if (params.systemsConsolidationStrength < 0.0f || params.systemsConsolidationStrength > 1.0f) return false;
    if (params.synapticConsolidationStrength < 0.0f || params.synapticConsolidationStrength > 1.0f) return false;
    if (params.schemaFormationStrength < 0.0f || params.schemaFormationStrength > 1.0f) return false;
    if (params.memoryIntegrationStrength < 0.0f || params.memoryIntegrationStrength > 1.0f) return false;
    if (params.abstractionLevel < 0.0f || params.abstractionLevel > 1.0f) return false;
    if (params.generalizationCapacity < 0.0f || params.generalizationCapacity > 1.0f) return false;
    if (params.neuromodulationSensitivity < 0.0f || params.neuromodulationSensitivity > 1.0f) return false;
    
    return true;
}

void SleepConsolidation::scheduleNextConsolidation(SleepState state, float dt) {
    // Schedule next consolidation based on sleep state
    if (state == SleepState::Wake) {
        // No consolidation during wake
        consolidationActive_ = false;
        return;
    }
    
    // Schedule next consolidation
    consolidationInterval_ += dt;
    if (consolidationInterval_ >= 10.0f) {
        // Trigger consolidation
        triggerConsolidationEvents(state);
        consolidationInterval_ = 0.0f;
    }
}

void SleepConsolidation::triggerSystemsConsolidation(SleepState state) {
    // Trigger systems consolidation
    systemsConsolidation(state);
}

void SleepConsolidation::triggerSynapticConsolidation(SleepState state) {
    // Trigger synaptic consolidation
    synapticConsolidation(state);
}

void SleepConsolidation::triggerSchemaFormation(SleepState state) {
    // Trigger schema formation
    schemaFormation(state);
}

void SleepConsolidation::triggerMemoryIntegration(SleepState state) {
    // Trigger memory integration
    memoryIntegration(state);
}

void SleepConsolidation::reorganizeMemoryNetworks() {
    if (!brain_) return;
    
    // Reorganize memory networks
    // This would modify network connections and representations
    
    // Simulate reorganization
    float reorganizationEffect = 0.5f * 100.0f;
    systemsConsolidationProgress_ += reorganizationEffect;
}

void SleepConsolidation::formNewSchemas() {
    if (!brain_) return;
    
    // Form new schemas
    // This would create new abstract representations
    
    // Simulate schema formation
    float schemaEffect = 0.8f * 100.0f;
    schemaFormationProgress_ += schemaEffect;
}

void SleepConsolidation::handleConsolidationError(const std::string& errorMessage) {
    inErrorState_ = true;
    lastErrorMessage_ = errorMessage;
    consecutiveErrors_++;
    
    NLM_LOG_ERROR("SleepConsolidation error: " + errorMessage);
    
    // Attempt recovery
    recoverFromConsolidationError();
}

void SleepConsolidation::recoverFromConsolidationError() {
    // Attempt to recover from consolidation error
    if (consecutiveErrors_ > 0) {
        // Reduce consolidation parameters on error
        for (auto& pair : pImpl->consolidationParameters) {
            pair.second.systemsConsolidationStrength *= 0.5f;
            pair.second.synapticConsolidationStrength *= 0.5f;
        }
        
        // Reset state
        inErrorState_ = false;
        
        NLM_LOG_INFO("SleepConsolidation recovered from error");
    }
}

std::string SleepConsolidation::serialize() const {
    std::stringstream ss;
    
    // Serialize consolidation parameters for each state
    ss << pImpl->consolidationParameters.size() << " ";
    for (const auto& pair : pImpl->consolidationParameters) {
        ss << static_cast<int>(pair.first) << " ";
        ss << pair.second.systemsConsolidationStrength << " " << pair.second.synapticConsolidationStrength << " ";
        ss << pair.second.schemaFormationStrength << " " << pair.second.memoryIntegrationStrength << " ";
        ss << pair.second.abstractionLevel << " " << pair.second.generalizationCapacity << " ";
        ss << pair.second.enableReorganization << " " << pair.second.neuromodulationSensitivity << " ";
    }
    
    // Serialize statistics
    ss << systemsConsolidationProgress_ << " ";
    ss << synapticConsolidationProgress_ << " " << schemaFormationProgress_ << " " << memoryIntegrationProgress_ << " ";
    ss << consolidationEfficiency_ << " " << totalConsolidationEvents_ << " ";
    
    // Serialize current state
    ss << static_cast<int>(currentConsolidationState_) << " ";
    ss << currentConsolidationProgress_ << " " << consolidationActive_ << " ";
    
    // Serialize metrics
    ss << recentConsolidationMetrics_.size() << " ";
    for (float metric : recentConsolidationMetrics_) {
        ss << metric << " ";
    }
    
    return ss.str();
}

bool SleepConsolidation::deserialize(const std::string& data) {
    std::stringstream ss(data);
    
    size_t paramCount;
    ss >> paramCount;
    
    // Clear existing parameters
    pImpl->consolidationParameters.clear();
    
    // Deserialize consolidation parameters
    for (size_t i = 0; i < paramCount; ++i) {
        int stateInt;
        ss >> stateInt;
        SleepState state = static_cast<SleepState>(stateInt);
        
        StateConsolidationParameters params;
        ss >> params.systemsConsolidationStrength >> params.synapticConsolidationStrength;
        ss >> params.schemaFormationStrength >> params.memoryIntegrationStrength;
        ss >> params.abstractionLevel >> params.generalizationCapacity;
        ss >> params.enableReorganization >> params.neuromodulationSensitivity;
        
        pImpl->consolidationParameters[state] = params;
    }
    
    // Deserialize statistics
    ss >> systemsConsolidationProgress_;
    ss >> synapticConsolidationProgress_ >> schemaFormationProgress_ >> memoryIntegrationProgress_;
    ss >> consolidationEfficiency_ >> totalConsolidationEvents_;
    
    // Deserialize current state
    int stateInt;
    ss >> stateInt;
    currentConsolidationState_ = static_cast<SleepState>(stateInt);
    
    ss >> currentConsolidationProgress_ >> consolidationActive_;
    
    // Deserialize metrics
    size_t metricsCount;
    ss >> metricsCount;
    
    recentConsolidationMetrics_.clear();
    for (size_t i = 0; i < metricsCount; ++i) {
        float metric;
        ss >> metric;
        recentConsolidationMetrics_.push_back(metric);
    }
    
    return !ss.fail();
}

} // namespace nlm
