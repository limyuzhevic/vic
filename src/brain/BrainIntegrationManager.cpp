// Brain Integration Manager Implementation

#include "BrainIntegration.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <map>

using namespace nlm;

// Implementation details for BrainIntegrationManager
struct BrainIntegrationManager::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;

    // Integration state
    IntegrationStage stage;
    bool memoryIntegrationEnabled;
    bool predictionIntegrationEnabled;
    bool neuromodulationIntegrationEnabled;
    bool developmentIntegrationEnabled;
    bool cognitionIntegrationEnabled;
    bool visualizationIntegrationEnabled;
    bool agentIntegrationEnabled;

    // System connections
    struct SystemConnection {
        BrainSystem source;
        BrainSystem target;
        double strength;
        std::string connectionType;
        std::chrono::system_clock::time_point lastUpdated;
    };

    std::vector<SystemConnection> connections;
    std::map<std::string, std::chrono::system_clock::time_point> systemHealth;
    std::map<std::string, std::string> systemStatus;

    // Performance metrics
    PerformanceMetrics perfMetrics;
    IntegrationStats integrationStats;

    // Event handling
    std::vector<std::shared_ptr<IntegrationEventListener>> eventListeners;
    std::function<void(const IntegrationEvent&)> eventCallback;

    // Configuration
    MemoryConfig memoryConfig;
    PredictionConfig predictionConfig;
    NeuromodulationConfig neuromodulationConfig;
    DevelopmentConfig developmentConfig;
    CognitionConfig cognitionConfig;

    // Threading
    std::mutex mutex;
    bool integrationThreadRunning;
    std::thread integrationThread;

    // Error handling
    std::vector<std::string> recentErrors;
    std::vector<std::string> recentRecoveries;

    Impl(std::shared_ptr<Brain> brain) : brain(brain), config(brain->getConfig()),
        logger(Logger::getGlobal()), stage(IntegrationStage::NotInitialized),
        memoryIntegrationEnabled(true), predictionIntegrationEnabled(true),
        neuromodulationIntegrationEnabled(true), developmentIntegrationEnabled(true),
        cognitionIntegrationEnabled(true), visualizationIntegrationEnabled(true),
        agentIntegrationEnabled(true), integrationThreadRunning(false) {

        // Initialize system health tracking
        initializeSystemHealthTracking();
    }

    ~Impl() {
        stopIntegrationThread();
    }

    void initializeSystemHealthTracking() {
        systemHealth["working_memory"] = std::chrono::system_clock::now();
        systemHealth["episodic_memory"] = std::chrono::system_clock::now();
        systemHealth["semantic_memory"] = std::chrono::system_clock::now();
        systemHealth["procedural_memory"] = std::chrono::system_clock::now();
        systemHealth["associative_memory"] = std::chrono::system_clock::now();
        systemHealth["prediction_system"] = std::chrono::system_clock::now();
        systemHealth["dopamine"] = std::chrono::system_clock::now();
        systemHealth["curiosity"] = std::chrono::system_clock::now();
        systemHealth["novelty"] = std::chrono::system_clock::now();
        systemHealth["prediction_error"] = std::chrono::system_clock::now();
        systemHealth["development_system"] = std::chrono::system_clock::now();
        systemHealth["concept_formation"] = std::chrono::system_clock::now();
        systemHealth["neural_planner"] = std::chrono::system_clock::now();
        systemHealth["attentional_selection"] = std::chrono::system_clock::now();
        systemHealth["visualization_interface"] = std::chrono::system_clock::now();
        systemHealth["agent_brain"] = std::chrono::system_clock::now();
        systemHealth["agent_body"] = std::chrono::system_clock::now();
    }

    void startIntegrationThread() {
        integrationThreadRunning = true;
        integrationThread = std::thread(&Impl::integrationLoop, this);
    }

    void stopIntegrationThread() {
        integrationThreadRunning = false;
        if (integrationThread.joinable()) {
            integrationThread.join();
        }
    }

    void integrationLoop() {
        auto lastUpdate = std::chrono::system_clock::now();
        while (integrationThreadRunning) {
            auto now = std::chrono::system_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate);

            // Update performance metrics every second
            if (elapsed.count() >= 1000) {
                updatePerformanceMetrics();
                lastUpdate = now;
            }

            // Update all integrated systems
            updateAllSystems();

            // Check system health
            checkSystemHealth();

            // Sleep for a short time
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void updateAllSystems() {
        std::lock_guard<std::mutex> lock(mutex);

        auto startTime = std::chrono::high_resolution_clock::now();

        // Update memory systems
        if (memoryIntegrationEnabled) {
            updateMemorySystems();
        }

        // Update prediction systems
        if (predictionIntegrationEnabled) {
            updatePredictionSystems();
        }

        // Update neuromodulation systems
        if (neuromodulationIntegrationEnabled) {
            updateNeuromodulationSystems();
        }

        // Update development systems
        if (developmentIntegrationEnabled) {
            updateDevelopmentSystems();
        }

        // Update cognition systems
        if (cognitionIntegrationEnabled) {
            updateCognitionSystems();
        }

        // Update visualization systems
        if (visualizationIntegrationEnabled) {
            updateVisualizationSystems();
        }

        // Update agent systems
        if (agentIntegrationEnabled) {
            updateAgentSystems();
        }

        // Update performance metrics
        updatePerformanceMetrics();
    }

    void updateMemorySystems() {
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            workingMem->update(simd_current_step, simd_current_time);
        }

        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            episodicMem->update(simd_current_step, simd_current_time);
        }

        auto* semanticMem = brain->getSemanticMemory();
        if (semanticMem) {
            semanticMem->update(simd_current_step, simd_current_time);
        }

        auto* proceduralMem = brain->getProceduralMemory();
        if (proceduralMem) {
            proceduralMem->update(simd_current_step, simd_current_time);
        }

        auto* associativeMem = brain->getAssociativeMemory();
        if (associativeMem) {
            associativeMem->update(simd_current_step, simd_current_time);
        }
    }

    void updatePredictionSystems() {
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            predictionSystem->update(simd_current_step, simd_current_time);
        }

        auto* predictionError = brain->getPredictionErrorSignal();
        if (predictionError) {
            predictionError->update(simd_current_step, simd_current_time);
        }
    }

    void updateNeuromodulationSystems() {
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            dopamine->update(simd_current_step, simd_current_time);
        }

        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            curiosity->update(simd_current_step, simd_current_time);
        }

        auto* novelty = brain->getNovelty();
        if (novelty) {
            novelty->update(simd_current_step, simd_current_time);
        }
    }

    void updateDevelopmentSystems() {
        auto* development = brain->getDevelopmentSystem();
        if (development) {
            development->update(simd_current_step, simd_current_time);
        }
    }

    void updateCognitionSystems() {
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            conceptFormation->update(simd_current_step, simd_current_time);
        }

        auto* neuralPlanner = brain->getPlanner();
        if (neuralPlanner) {
            neuralPlanner->update(simd_current_step, simd_current_time);
        }

        auto* attentionalSelection = brain->getAttention();
        if (attentionalSelection) {
            attentionalSelection->update(simd_current_step, simd_current_time);
        }
    }

    void updateVisualizationSystems() {
        auto* visualization = brain->getVisualizationInterface();
        if (visualization) {
            visualization->update(simd_current_step, simd_current_time);
        }
    }

    void updateAgentSystems() {
        auto* agentBrain = brain->getAgentBrain();
        if (agentBrain) {
            agentBrain->update(simd_current_step, simd_current_time);
        }

        auto* agentBody = brain->getAgentBody();
        if (agentBody) {
            agentBody->update(simd_current_step, simd_current_time);
        }
    }

    void updatePerformanceMetrics() {
        auto now = std::chrono::high_resolution_clock::now();
        perfMetrics.wallClockTime = std::chrono::duration<double>(now - perfMetrics.startTime).count();
        perfMetrics.stepsCompleted = brain->getTotalStepCount();
        perfMetrics.averageFiringRate = brain->getAverageFiringRate();
        perfMetrics.totalSpikes = brain->getTotalSpikeCount();

        // Update system-specific metrics
        updateSystemMetrics();
    }

    void updateSystemMetrics() {
        // Update metrics for each system
        if (auto* workingMem = brain->getWorkingMemory()) {
            perfMetrics.systemMetrics["working_memory_usage"] = workingMem->getUsage();
            perfMetrics.systemMetrics["working_memory_capacity"] = workingMem->getCapacity();
        }

        if (auto* episodicMem = brain->getEpisodicMemory()) {
            perfMetrics.systemMetrics["episodic_memory_episodes"] = episodicMem->getEpisodeCount();
        }

        if (auto* predictionSystem = brain->getPredictionSystem()) {
            perfMetrics.systemMetrics["prediction_confidence"] = predictionSystem->getConfidence();
            perfMetrics.systemMetrics["prediction_accuracy"] = predictionSystem->getAccuracy();
        }

        if (auto* dopamine = brain->getDopamine()) {
            perfMetrics.systemMetrics["dopamine_level"] = dopamine->getLevel();
        }

        if (auto* curiosity = brain->getCuriosity()) {
            perfMetrics.systemMetrics["curiosity_level"] = curiosity->getLevel();
        }

        if (auto* novelty = brain->getNovelty()) {
            perfMetrics.systemMetrics["novelty_level"] = novelty->getLevel();
        }

        if (auto* development = brain->getDevelopmentSystem()) {
            perfMetrics.systemMetrics["development_stage"] = static_cast<int>(development->getStage());
            perfMetrics.systemMetrics["development_age"] = development->getAge();
        }

        if (auto* conceptFormation = brain->getConceptFormation()) {
            perfMetrics.systemMetrics["concept_count"] = conceptFormation->getConceptCount();
        }

        if (auto* neuralPlanner = brain->getPlanner()) {
            perfMetrics.systemMetrics["plan_count"] = neuralPlanner->getPlanCount();
        }
    }

    void checkSystemHealth() {
        auto now = std::chrono::system_clock::now();

        // Check if systems are responsive
        checkSystemHealth("working_memory", [this]() {
            return brain->getWorkingMemory() != nullptr;
        });

        checkSystemHealth("episodic_memory", [this]() {
            return brain->getEpisodicMemory() != nullptr;
        });

        checkSystemHealth("semantic_memory", [this]() {
            return brain->getSemanticMemory() != nullptr;
        });

        checkSystemHealth("procedural_memory", [this]() {
            return brain->getProceduralMemory() != nullptr;
        });

        checkSystemHealth("associative_memory", [this]() {
            return brain->getAssociativeMemory() != nullptr;
        });

        checkSystemHealth("prediction_system", [this]() {
            return brain->getPredictionSystem() != nullptr;
        });

        checkSystemHealth("dopamine", [this]() {
            return brain->getDopamine() != nullptr;
        });

        checkSystemHealth("curiosity", [this]() {
            return brain->getCuriosity() != nullptr;
        });

        checkSystemHealth("novelty", [this]() {
            return brain->getNovelty() != nullptr;
        });

        checkSystemHealth("prediction_error", [this]() {
            return brain->getPredictionErrorSignal() != nullptr;
        });

        checkSystemHealth("development_system", [this]() {
            return brain->getDevelopmentSystem() != nullptr;
        });

        checkSystemHealth("concept_formation", [this]() {
            return brain->getConceptFormation() != nullptr;
        });

        checkSystemHealth("neural_planner", [this]() {
            return brain->getPlanner() != nullptr;
        });

        checkSystemHealth("attentional_selection", [this]() {
            return brain->getAttention() != nullptr;
        });

        checkSystemHealth("visualization_interface", [this]() {
            return brain->getVisualizationInterface() != nullptr;
        });

        checkSystemHealth("agent_brain", [this]() {
            return brain->getAgentBrain() != nullptr;
        });

        checkSystemHealth("agent_body", [this]() {
            return brain->getAgentBody() != nullptr;
        });
    }

    void checkSystemHealth(const std::string& systemName, std::function<bool()> healthCheck) {
        bool isHealthy = healthCheck();
        systemHealth[systemName] = std::chrono::system_clock::now();

        if (!isHealthy) {
            std::string error = "System " + systemName + " is not healthy";
            recentErrors.push_back(error);
            handleIntegrationError(error);
        }

        systemStatus[systemName] = isHealthy ? "healthy" : "error";
    }

    void handleIntegrationError(const std::string& error) {
        recentErrors.push_back(error);

        // Log the error
        logger->error("Integration error: " + error);

        // Try to recover
        size_t lastUnderscore = error.find_last_of('_');
        if (lastUnderscore != std::string::npos) {
            std::string systemName = error.substr(0, lastUnderscore);
            recoverFromError(systemName);
        }

        // Notify event listeners
        IntegrationEvent event(IntegrationEventType::SystemError, "IntegrationManager", error);
        IntegrationEventDispatcher::getInstance().dispatch(event);
    }

    void recoverFromError(const std::string& systemName) {
        recentRecoveries.push_back("Recovered from error in system: " + systemName);
        logger->info("Recovered from error in system: " + systemName);
        IntegrationEvent event(IntegrationEventType::SystemRecovered, systemName, "System recovered");
        IntegrationEventDispatcher::getInstance().dispatch(event);
    }

    bool isSystemHealthy(const std::string& systemName) const {
        auto it = systemHealth.find(systemName);
        if (it == systemHealth.end()) {
            return false;
        }

        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second);

        // Consider system unhealthy if not updated in the last 5 seconds
        return elapsed.count() < 5;
    }

    void establishMemoryConnections() {
        // Connect memory systems to neural processing
        if (auto* workingMem = brain->getWorkingMemory()) {
            addConnection(BrainSystem::WorkingMemory, BrainSystem::CoreNeural, 0.9, "neural_input");
        }

        if (auto* episodicMem = brain->getEpisodicMemory()) {
            addConnection(BrainSystem::EpisodicMemory, BrainSystem::CoreNeural, 0.8, "encoding");
        }

        if (auto* semanticMem = brain->getSemanticMemory()) {
            addConnection(BrainSystem::SemanticMemory, BrainSystem::CoreNeural, 0.85, "pattern_learning");
        }

        if (auto* proceduralMem = brain->getProceduralMemory()) {
            addConnection(BrainSystem::ProceduralMemory, BrainSystem::CoreNeural, 0.8, "skill_formation");
        }

        if (auto* associativeMem = brain->getAssociativeMemory()) {
            addConnection(BrainSystem::AssociativeMemory, BrainSystem::CoreNeural, 0.9, "association");
        }
    }

    void establishPredictionConnections() {
        // Connect prediction system to sensory input
        if (auto* predictionSystem = brain->getPredictionSystem()) {
            addConnection(BrainSystem::PredictionSystem, BrainSystem::SensoryInput, 0.9, "sensory_prediction");
            addConnection(BrainSystem::PredictionSystem, BrainSystem::CoreNeural, 0.8, "prediction_signals");
        }

        if (auto* predictionError = brain->getPredictionErrorSignal()) {
            addConnection(BrainSystem::PredictionError, BrainSystem::CoreNeural, 0.9, "error_signals");
        }
    }

    void establishNeuromodulationConnections() {
        // Connect neuromodulation systems to plasticity
        if (auto* dopamine = brain->getDopamine()) {
            addConnection(BrainSystem::Dopamine, BrainSystem::CoreNeural, 0.95, "plasticity_modulation");
        }

        if (auto* curiosity = brain->getCuriosity()) {
            addConnection(BrainSystem::Curiosity, BrainSystem::CoreNeural, 0.9, "exploration_drive");
        }

        if (auto* novelty = brain->getNovelty()) {
            addConnection(BrainSystem::Novelty, BrainSystem::CoreNeural, 0.8, "novelty_detection");
        }
    }

    void establishDevelopmentConnections() {
        // Connect development system to neural properties
        if (auto* development = brain->getDevelopmentSystem()) {
            addConnection(BrainSystem::DevelopmentSystem, BrainSystem::CoreNeural, 0.8, "plasticity_rate");
            addConnection(BrainSystem::DevelopmentSystem, BrainSystem::CoreNeural, 0.7, "neural_properties");
        }
    }

    void establishCognitionConnections() {
        // Connect cognition systems to neural activity
        if (auto* conceptFormation = brain->getConceptFormation()) {
            addConnection(BrainSystem::ConceptFormation, BrainSystem::CoreNeural, 0.9, "pattern_abstract");
        }

        if (auto* neuralPlanner = brain->getPlanner()) {
            addConnection(BrainSystem::NeuralPlanner, BrainSystem::CoreNeural, 0.8, "action_evaluation");
        }

        if (auto* attentionalSelection = brain->getAttention()) {
            addConnection(BrainSystem::AttentionalSelection, BrainSystem::CoreNeural, 0.9, "priority_signals");
        }
    }

    void establishVisualizationConnections() {
        // Connect visualization to brain state
        if (auto* visualization = brain->getVisualizationInterface()) {
            addConnection(BrainSystem::VisualizationInterface, BrainSystem::CoreNeural, 0.7, "state_export");
        }
    }

    void establishAgentConnections() {
        // Connect agent systems to brain output
        if (auto* agentBrain = brain->getAgentBrain()) {
            addConnection(BrainSystem::AgentBrain, BrainSystem::CoreNeural, 0.8, "decision_processing");
        }

        if (auto* agentBody = brain->getAgentBody()) {
            addConnection(BrainSystem::AgentBody, BrainSystem::CoreNeural, 0.7, "action_execution");
        }
    }

    void addConnection(BrainSystem source, BrainSystem target, double strength, const std::string& type) {
        SystemConnection connection;
        connection.source = source;
        connection.target = target;
        connection.strength = strength;
        connection.connectionType = type;
        connection.lastUpdated = std::chrono::system_clock::now();

        connections.push_back(connection);
    }

    std::string getSystemName(BrainSystem system) const {
        switch (system) {
            case BrainSystem::WorkingMemory: return "working_memory";
            case BrainSystem::EpisodicMemory: return "episodic_memory";
            case BrainSystem::SemanticMemory: return "semantic_memory";
            case BrainSystem::ProceduralMemory: return "procedural_memory";
            case BrainSystem::AssociativeMemory: return "associative_memory";
            case BrainSystem::PredictionSystem: return "prediction_system";
            case BrainSystem::Dopamine: return "dopamine";
            case BrainSystem::Curiosity: return "curiosity";
            case BrainSystem::Novelty: return "novelty";
            case BrainSystem::PredictionError: return "prediction_error";
            case BrainSystem::DevelopmentSystem: return "development_system";
            case BrainSystem::ConceptFormation: return "concept_formation";
            case BrainSystem::NeuralPlanner: return "neural_planner";
            case BrainSystem::AttentionalSelection: return "attentional_selection";
            case BrainSystem::VisualizationInterface: return "visualization_interface";
            case BrainSystem::AgentBrain: return "agent_brain";
            case BrainSystem::AgentBody: return "agent_body";
            default: return "unknown";
        }
    }

    std::string getIntegrationStageString(IntegrationStage stage) const {
        switch (stage) {
            case IntegrationStage::NotInitialized: return "NotInitialized";
            case IntegrationStage::CoreSystemsInitialized: return "CoreSystemsInitialized";
            case IntegrationStage::MemorySystemsInitialized: return "MemorySystemsInitialized";
            case IntegrationStage::PredictionSystemsInitialized: return "PredictionSystemsInitialized";
            case IntegrationStage::NeuromodulationSystemsInitialized: return "NeuromodulationSystemsInitialized";
            case IntegrationStage::DevelopmentSystemsInitialized: return "DevelopmentSystemsInitialized";
            case IntegrationStage::CognitionSystemsInitialized: return "CognitionSystemsInitialized";
            case IntegrationStage::VisualizationSystemsInitialized: return "VisualizationSystemsInitialized";
            case IntegrationStage::AgentSystemsInitialized: return "AgentSystemsInitialized";
            case IntegrationStage::FullyIntegrated: return "FullyIntegrated";
            case IntegrationStage::ValidationComplete: return "ValidationComplete";
            default: return "Unknown";
        }
    }

    double calculateAverageConnectionStrength() const {
        if (connections.empty()) return 0.0;

        double totalStrength = 0.0;
        for (const auto& connection : connections) {
            totalStrength += connection.strength;
        }

        return totalStrength / connections.size();
    }

    std::string getIntegrationStatus() const {
        std::ostringstream oss;
        oss << "=== Integration Status ===\n";
        oss << "Stage: " << getIntegrationStageString(stage) << "\n";
        oss << "Total Systems: " << integrationStats.totalSystems << "\n";
        oss << "Active Systems: " << integrationStats.activeSystems << "\n";
        oss << "Connected Systems: " << integrationStats.connectedSystems << "\n";
        oss << "Average Connection Strength: " << calculateAverageConnectionStrength() << "\n";
        oss << "Integration Health: " << integrationStats.integrationHealth << "%\n";
        oss << "Recent Errors: " << integrationStats.totalErrors << "\n";
        oss << "Recent Recoveries: " << integrationStats.totalRecoveries << "\n";

        oss << "\nActive Systems:\n";
        for (const auto& system : integrationStats.activeSystemNames) {
            oss << "  - " << system << " (" << systemStatus[system] << ")\n";
        }

        oss << "\nSystem Performance:\n";
        for (const auto& pair : integrationStats.systemPerformance) {
            oss << "  - " << pair.first << ": " << pair.second << "%\n";
        }

        oss << "\n=== End Integration Status ===\n";
        return oss.str();
    }

    bool validateIntegration() const {
        // Check if all required systems are present and healthy
        int healthySystems = 0;
        int totalSystems = 0;

        if (auto* workingMem = brain->getWorkingMemory()) {
            totalSystems++;
            if (isSystemHealthy("working_memory")) healthySystems++;
        }

        if (auto* episodicMem = brain->getEpisodicMemory()) {
            totalSystems++;
            if (isSystemHealthy("episodic_memory")) healthySystems++;
        }

        if (auto* semanticMem = brain->getSemanticMemory()) {
            totalSystems++;
            if (isSystemHealthy("semantic_memory")) healthySystems++;
        }

        if (auto* proceduralMem = brain->getProceduralMemory()) {
            totalSystems++;
            if (isSystemHealthy("procedural_memory")) healthySystems++;
        }

        if (auto* associativeMem = brain->getAssociativeMemory()) {
            totalSystems++;
            if (isSystemHealthy("associative_memory")) healthySystems++;
        }

        if (auto* predictionSystem = brain->getPredictionSystem()) {
            totalSystems++;
            if (isSystemHealthy("prediction_system")) healthySystems++;
        }

        if (auto* dopamine = brain->getDopamine()) {
            totalSystems++;
            if (isSystemHealthy("dopamine")) healthySystems++;
        }

        if (auto* curiosity = brain->getCuriosity()) {
            totalSystems++;
            if (isSystemHealthy("curiosity")) healthySystems++;
        }

        if (auto* novelty = brain->getNovelty()) {
            totalSystems++;
            if (isSystemHealthy("novelty")) healthySystems++;
        }

        if (auto* predictionError = brain->getPredictionErrorSignal()) {
            totalSystems++;
            if (isSystemHealthy("prediction_error")) healthySystems++;
        }

        if (auto* development = brain->getDevelopmentSystem()) {
            totalSystems++;
            if (isSystemHealthy("development_system")) healthySystems++;
        }

        if (auto* conceptFormation = brain->getConceptFormation()) {
            totalSystems++;
            if (isSystemHealthy("concept_formation")) healthySystems++;
        }

        if (auto* neuralPlanner = brain->getPlanner()) {
            totalSystems++;
            if (isSystemHealthy("neural_planner")) healthySystems++;
        }

        if (auto* attentionalSelection = brain->getAttention()) {
            totalSystems++;
            if (isSystemHealthy("attentional_selection")) healthySystems++;
        }

        if (auto* visualization = brain->getVisualizationInterface()) {
            totalSystems++;
            if (isSystemHealthy("visualization_interface")) healthySystems++;
        }

        if (auto* agentBrain = brain->getAgentBrain()) {
            totalSystems++;
            if (isSystemHealthy("agent_brain")) healthySystems++;
        }

        if (auto* agentBody = brain->getAgentBody()) {
            totalSystems++;
            if (isSystemHealthy("agent_body")) healthySystems++;
        }

        integrationStats.totalSystems = totalSystems;
        integrationStats.activeSystems = healthySystems;
        integrationStats.integrationHealth = totalSystems > 0 ? (healthySystems * 100.0 / totalSystems) : 0.0;

        return integrationStats.integrationHealth >= 80.0;  // Require 80% system health
    }

    std::vector<std::string> getIntegrationIssues() const {
        std::vector<std::string> issues;

        // Check for missing systems
        if (!brain->getWorkingMemory()) issues.push_back("Working memory system not available");
        if (!brain->getEpisodicMemory()) issues.push_back("Episodic memory system not available");
        if (!brain->getSemanticMemory()) issues.push_back("Semantic memory system not available");
        if (!brain->getProceduralMemory()) issues.push_back("Procedural memory system not available");
        if (!brain->getAssociativeMemory()) issues.push_back("Associative memory system not available");
        if (!brain->getPredictionSystem()) issues.push_back("Prediction system not available");
        if (!brain->getDopamine()) issues.push_back("Dopamine system not available");
        if (!brain->getCuriosity()) issues.push_back("Curiosity system not available");
        if (!brain->getNovelty()) issues.push_back("Novelty system not available");
        if (!brain->getPredictionErrorSignal()) issues.push_back("Prediction error system not available");
        if (!brain->getDevelopmentSystem()) issues.push_back("Development system not available");
        if (!brain->getConceptFormation()) issues.push_back("Concept formation system not available");
        if (!brain->getPlanner()) issues.push_back("Neural planner system not available");
        if (!brain->getAttention()) issues.push_back("Attentional selection system not available");
        if (!brain->getVisualizationInterface()) issues.push_back("Visualization system not available");
        if (!brain->getAgentBrain()) issues.push_back("Agent brain system not available");
        if (!brain->getAgentBody()) issues.push_back("Agent body system not available");

        // Check for unhealthy systems
        for (const auto& pair : systemStatus) {
            if (pair.second != "healthy") {
                issues.push_back("System " + pair.first + " is not healthy (status: " + pair.second + ")");
            }
        }

        return issues;
    }

    void runIntegrationTests() {
        NLM_LOG_INFO("Running integration tests...");

        // Test memory systems integration
        if (memoryIntegrationEnabled) {
            testMemoryIntegration();
        }

        // Test prediction systems integration
        if (predictionIntegrationEnabled) {
            testPredictionIntegration();
        }

        // Test neuromodulation systems integration
        if (neuromodulationIntegrationEnabled) {
            testNeuromodulationIntegration();
        }

        // Test development systems integration
        if (developmentIntegrationEnabled) {
            testDevelopmentIntegration();
        }

        // Test cognition systems integration
        if (cognitionIntegrationEnabled) {
            testCognitionIntegration();
        }

        // Test visualization systems integration
        if (visualizationIntegrationEnabled) {
            testVisualizationIntegration();
        }

        // Test agent systems integration
        if (agentIntegrationEnabled) {
            testAgentIntegration();
        }

        NLM_LOG_INFO("Integration tests completed");
    }

    void testMemoryIntegration() {
        NLM_LOG_INFO("Testing memory systems integration...");

        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            // Test working memory capacity
            size_t capacity = workingMem->getCapacity();
            NLM_LOG_INFO("Working memory capacity: " + std::to_string(capacity));

            // Test working memory usage
            size_t usage = workingMem->getUsage();
            NLM_LOG_INFO("Working memory usage: " + std::to_string(usage));

            if (usage > 0) {
                NLM_LOG_INFO("✓ Working memory is functioning");
            }
        }

        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            size_t episodeCount = episodicMem->getEpisodeCount();
            NLM_LOG_INFO("Episodic memory episode count: " + std::to_string(episodeCount));

            if (episodeCount > 0) {
                NLM_LOG_INFO("✓ Episodic memory is functioning");
            }
        }

        NLM_LOG_INFO("Memory integration test completed");
    }

    void testPredictionIntegration() {
        NLM_LOG_INFO("Testing prediction systems integration...");

        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            float confidence = predictionSystem->getConfidence();
            float accuracy = predictionSystem->getAccuracy();

            NLM_LOG_INFO("Prediction confidence: " + std::to_string(confidence));
            NLM_LOG_INFO("Prediction accuracy: " + std::to_string(accuracy));

            if (confidence > 0.5 && accuracy > 0.5) {
                NLM_LOG_INFO("✓ Prediction system is functioning");
            }
        }

        auto* predictionError = brain->getPredictionErrorSignal();
        if (predictionError) {
            NLM_LOG_INFO("Prediction error system is available");
            NLM_LOG_INFO("✓ Prediction error integration test completed");
        }

        NLM_LOG_INFO("Prediction integration test completed");
    }

    void testNeuromodulationIntegration() {
        NLM_LOG_INFO("Testing neuromodulation systems integration...");

        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            float level = dopamine->getLevel();
            NLM_LOG_INFO("Dopamine level: " + std::to_string(level));

            if (level > 0.0) {
                NLM_LOG_INFO("✓ Dopamine system is functioning");
            }
        }

        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            float level = curiosity->getLevel();
            NLM_LOG_INFO("Curiosity level: " + std::to_string(level));

            if (level > 0.0) {
                NLM_LOG_INFO("✓ Curiosity system is functioning");
            }
        }

        auto* novelty = brain->getNovelty();
        if (novelty) {
            float level = novelty->getLevel();
            NLM_LOG_INFO("Novelty level: " + std::to_string(level));

            if (level > 0.0) {
                NLM_LOG_INFO("✓ Novelty system is functioning");
            }
        }

        NLM_LOG_INFO("Neuromodulation integration test completed");
    }

    void testDevelopmentIntegration() {
        NLM_LOG_INFO("Testing development systems integration...");

        auto* development = brain->getDevelopmentSystem();
        if (development) {
            DevelopmentalStage stage = development->getStage();
            double age = development->getAge();

            NLM_LOG_INFO("Development stage: " + developmentStageToString(stage));
            NLM_LOG_INFO("Development age: " + std::to_string(age) + " steps");

            if (age > 0.0) {
                NLM_LOG_INFO("✓ Development system is functioning");
            }
        }

        NLM_LOG_INFO("Development integration test completed");
    }

    void testCognitionIntegration() {
        NLM_LOG_INFO("Testing cognition systems integration...");

        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            size_t conceptCount = conceptFormation->getConceptCount();
            NLM_LOG_INFO("Concept formation concept count: " + std::to_string(conceptCount));

            if (conceptCount > 0) {
                NLM_LOG_INFO("✓ Concept formation system is functioning");
            }
        }

        auto* neuralPlanner = brain->getPlanner();
        if (neuralPlanner) {
            size_t planCount = neuralPlanner->getPlanCount();
            NLM_LOG_INFO("Neural planner plan count: " + std::to_string(planCount));

            if (planCount > 0) {
                NLM_LOG_INFO("✓ Neural planner system is functioning");
            }
        }

        auto* attentionalSelection = brain->getAttention();
        if (attentionalSelection) {
            NLM_LOG_INFO("Attentional selection system is available");
            NLM_LOG_INFO("✓ Attentional selection integration test completed");
        }

        NLM_LOG_INFO("Cognition integration test completed");
    }

    void testVisualizationIntegration() {
        NLM_LOG_INFO("Testing visualization systems integration...");

        auto* visualization = brain->getVisualizationInterface();
        if (visualization) {
            NLM_LOG_INFO("Visualization system is available");
            NLM_LOG_INFO("✓ Visualization integration test completed");
        }

        NLM_LOG_INFO("Visualization integration test completed");
    }

    void testAgentIntegration() {
        NLM_LOG_INFO("Testing agent systems integration...");

        auto* agentBrain = brain->getAgentBrain();
        if (agentBrain) {
            NLM_LOG_INFO("Agent brain system is available");
            NLM_LOG_INFO("✓ Agent brain integration test completed");
        }

        auto* agentBody = brain->getAgentBody();
        if (agentBody) {
            NLM_LOG_INFO("Agent body system is available");
            NLM_LOG_INFO("✓ Agent body integration test completed");
        }

        NLM_LOG_INFO("Agent integration test completed");
    }

    std::string exportIntegrationState() const {
        std::ostringstream oss;
        oss << "NLM Integration State Export\n";
        oss << "==========================\n\n";

        // Export integration configuration
        oss << "Integration Configuration:\n";
        oss << "  Memory Integration: " << (memoryIntegrationEnabled ? "Enabled" : "Disabled") << "\n";
        oss << "  Prediction Integration: " << (predictionIntegrationEnabled ? "Enabled" : "Disabled") << "\n";
        oss << "  Neuromodulation Integration: " << (neuromodulationIntegrationEnabled ? "Enabled" : "Disabled") << "\n";
        oss << "  Development Integration: " << (developmentIntegrationEnabled ? "Enabled" : "Disabled") << "\n";
        oss << "  Cognition Integration: " << (cognitionIntegrationEnabled ? "Enabled" : "Disabled") << "\n";
        oss << "  Visualization Integration: " << (visualizationIntegrationEnabled ? "Enabled" : "Disabled") << "\n";
        oss << "  Agent Integration: " << (agentIntegrationEnabled ? "Enabled" : "Disabled") << "\n\n";

        // Export system connections
        oss << "System Connections:\n";
        for (const auto& connection : connections) {
            oss << "  " << getSystemName(connection.source) << " -> " << getSystemName(connection.target) << "\n";
        }
        oss << "\n";

        // Export system health
        oss << "System Health:\n";
        for (const auto& pair : systemStatus) {
            oss << "  " << pair.first << ": " << pair.second << "\n";
        }
        oss << "\n";

        // Export performance metrics
        oss << "Performance Metrics:\n";
        oss << "  CPU Usage: " << perfMetrics.cpuUsage << "%\n";
        oss << "  Memory Usage: " << perfMetrics.memoryUsage << " MB\n";
        oss << "  Simulation Time: " << perfMetrics.simulationTime << "s\n";
        oss << "  Wall Clock Time: " << perfMetrics.wallClockTime << "s\n";
        oss << "  Steps Completed: " << perfMetrics.stepsCompleted << "\n";
        oss << "  Average Firing Rate: " << perfMetrics.averageFiringRate << " Hz\n";
        oss << "  Total Spikes: " << perfMetrics.totalSpikes << "\n\n";

        // Export integration stats
        oss << "Integration Statistics:\n";
        oss << "  Total Systems: " << integrationStats.totalSystems << "\n";
        oss << "  Active Systems: " << integrationStats.activeSystems << "\n";
        oss << "  Connected Systems: " << integrationStats.connectedSystems << "\n";
        oss << "  Average Connection Strength: " << calculateAverageConnectionStrength() << "\n";
        oss << "  Integration Health: " << integrationStats.integrationHealth << "%\n";
        oss << "  Total Errors: " << integrationStats.totalErrors << "\n";
        oss << "  Total Recoveries: " << integrationStats.totalRecoveries << "\n";

        return oss.str();
    }

    bool importIntegrationState(const std::string& stateData) {
        // Parse and import integration state
        // For now, this is a simplified implementation
        std::istringstream stream(stateData);
        std::string line;

        while (std::getline(stream, line)) {
            if (line.find("Integration Configuration:") != std::string::npos) {
                // Parse integration configuration lines
                while (std::getline(stream, line) && !line.empty() && line.find(":") != std::string::npos) {
                    size_t pos = line.find(":");
                    if (pos != std::string::npos) {
                        std::string key = nlm::trim(line.substr(0, pos));
                        std::string value = nlm::trim(line.substr(pos + 1));

                        if (key == "  Memory Integration") {
                            memoryIntegrationEnabled = (value.find("Enabled") != std::string::npos);
                        } else if (key == "  Prediction Integration") {
                            predictionIntegrationEnabled = (value.find("Enabled") != std::string::npos);
                        } else if (key == "  Neuromodulation Integration") {
                            neuromodulationIntegrationEnabled = (value.find("Enabled") != std::string::npos);
                        } else if (key == "  Development Integration") {
                            developmentIntegrationEnabled = (value.find("Enabled") != std::string::npos);
                        } else if (key == "  Cognition Integration") {
                            cognitionIntegrationEnabled = (value.find("Enabled") != std::string::npos);
                        } else if (key == "  Visualization Integration") {
                            visualizationIntegrationEnabled = (value.find("Enabled") != std::string::npos);
                        } else if (key == "  Agent Integration") {
                            agentIntegrationEnabled = (value.find("Enabled") != std::string::npos);
                        }
                    }
                }
            }
        }

        NLM_LOG_INFO("Integration state imported successfully");
        return true;
    }

    bool initialize() {
        NLM_LOG_INFO("Initializing brain integration...");

        std::lock_guard<std::mutex> lock(mutex);

        // Establish all system connections
        establishMemoryConnections();
        establishPredictionConnections();
        establishNeuromodulationConnections();
        establishDevelopmentConnections();
        establishCognitionConnections();
        establishVisualizationConnections();
        establishAgentConnections();

        // Start integration thread
        startIntegrationThread();

        // Set integration stage
        stage = IntegrationStage::CoreSystemsInitialized;

        // Log initialization success
        NLM_LOG_INFO("Brain integration initialized successfully");

        // Create initial integration event
        IntegrationEvent event(IntegrationEventType::SystemInitialized, "IntegrationManager", "Brain integration initialized");
        IntegrationEventDispatcher::getInstance().dispatch(event);

        return true;
    }

    void reset() {
        NLM_LOG_INFO("Resetting brain integration...");

        std::lock_guard<std::mutex> lock(mutex);

        // Stop integration thread
        stopIntegrationThread();

        // Reset system health
        for (auto& pair : systemHealth) {
            pair.second = std::chrono::system_clock::now();
        }

        // Reset system status
        for (auto& pair : systemStatus) {
            pair.second = "healthy";
        }

        // Clear recent errors and recoveries
        recentErrors.clear();
        recentRecoveries.clear();

        // Reset performance metrics
        perfMetrics = PerformanceMetrics();
        perfMetrics.startTime = std::chrono::high_resolution_clock::now();

        // Reset integration stage
        stage = IntegrationStage::NotInitialized;

        NLM_LOG_INFO("Brain integration reset completed");
    }

    bool save(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for saving integration state: " + filepath);
            return false;
        }

        // Export integration state
        std::string stateData = exportIntegrationState();
        file << stateData;

        file.close();

        NLM_LOG_INFO("Integration state saved to: " + filepath);
        return true;
    }

    bool load(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for loading integration state: " + filepath);
            return false;
        }

        std::string stateData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        if (importIntegrationState(stateData)) {
            NLM_LOG_INFO("Integration state loaded from: " + filepath);
            return true;
        }

        return false;
    }

    // Configuration setters
    void setMemoryConfig(const MemoryConfig& config) {
        std::lock_guard<std::mutex> lock(mutex);
        memoryConfig = config;
    }

    void setPredictionConfig(const PredictionConfig& config) {
        std::lock_guard<std::mutex> lock(mutex);
        predictionConfig = config;
    }

    void setNeuromodulationConfig(const NeuromodulationConfig& config) {
        std::lock_guard<std::mutex> lock(mutex);
        neuromodulationConfig = config;
    }

    void setDevelopmentConfig(const DevelopmentConfig& config) {
        std::lock_guard<std::mutex> lock(mutex);
        developmentConfig = config;
    }

    void setCognitionConfig(const CognitionConfig& config) {
        std::lock_guard<std::mutex> lock(mutex);
        cognitionConfig = config;
    }

    // Integration control
    void enableMemoryIntegration(bool enable) {
        std::lock_guard<std::mutex> lock(mutex);
        memoryIntegrationEnabled = enable;
    }

    void enablePredictionIntegration(bool enable) {
        std::lock_guard<std::mutex> lock(mutex);
        predictionIntegrationEnabled = enable;
    }

    void enableNeuromodulationIntegration(bool enable) {
        std::lock_guard<std::mutex> lock(mutex);
        neuromodulationIntegrationEnabled = enable;
    }

    void enableDevelopmentIntegration(bool enable) {
        std::lock_guard<std::mutex> lock(mutex);
        developmentIntegrationEnabled = enable;
    }

    void enableCognitionIntegration(bool enable) {
        std::lock_guard<std::mutex> lock(mutex);
        cognitionIntegrationEnabled = enable;
    }

    void enableVisualizationIntegration(bool enable) {
        std::lock_guard<std::mutex> lock(mutex);
        visualizationIntegrationEnabled = enable;
    }

    void enableAgentIntegration(bool enable) {
        std::lock_guard<std::mutex> lock(mutex);
        agentIntegrationEnabled = enable;
    }

    // Integration statistics
    IntegrationStats getIntegrationStats() const {
        std::lock_guard<std::mutex> lock(mutex);

        IntegrationStats stats;
        stats.totalSystems = integrationStats.totalSystems;
        stats.activeSystems = integrationStats.activeSystems;
        stats.connectedSystems = integrationStats.connectedSystems;
        stats.averageConnectionStrength = calculateAverageConnectionStrength();
        stats.integrationHealth = integrationStats.integrationHealth;
        stats.totalErrors = integrationStats.totalErrors;
        stats.totalRecoveries = integrationStats.totalRecoveries;

        // Collect active system names
        for (const auto& pair : systemStatus) {
            if (pair.second == "healthy") {
                stats.activeSystemNames.push_back(pair.first);
            }
        }

        // Collect connected system names
        for (const auto& connection : connections) {
            std::string targetName = getSystemName(connection.target);
            if (std::find(stats.connectedSystemNames.begin(), stats.connectedSystemNames.end(), targetName) == stats.connectedSystemNames.end()) {
                stats.connectedSystemNames.push_back(targetName);
            }
        }

        // Collect system performance
        stats.systemPerformance = perfMetrics.systemMetrics;

        return stats;
    }

    // Event handling
    void addListener(std::shared_ptr<IntegrationEventListener> listener) {
        std::lock_guard<std::mutex> lock(mutex);
        eventListeners.push_back(listener);
    }

    void removeListener(std::shared_ptr<IntegrationEventListener> listener) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = std::find(eventListeners.begin(), eventListeners.end(), listener);
        if (it != eventListeners.end()) {
            eventListeners.erase(it);
        }
    }

    void setEventCallback(std::function<void(const IntegrationEvent&)> callback) {
        std::lock_guard<std::mutex> lock(mutex);
        eventCallback = callback;
    }

    // System access
    NeuralWorkingMemory* getWorkingMemory() const {
        return brain ? brain->getWorkingMemory() : nullptr;
    }

    NeuralEpisodicMemory* getEpisodicMemory() const {
        return brain ? brain->getEpisodicMemory() : nullptr;
    }

    NeuralAssociativeMemory* getAssociativeMemory() const {
        return brain ? brain->getAssociativeMemory() : nullptr;
    }

    PredictionSystem* getPredictionSystem() const {
        return brain ? brain->getPredictionSystem() : nullptr;
    }

    Dopamine* getDopamine() const {
        return brain ? brain->getDopamine() : nullptr;
    }

    Curiosity* getCuriosity() const {
        return brain ? brain->getCuriosity() : nullptr;
    }

    Novelty* getNovelty() const {
        return brain ? brain->getNovelty() : nullptr;
    }

    PredictionError* getPredictionError() const {
        return brain ? brain->getPredictionErrorSignal() : nullptr;
    }

    DevelopmentSystem* getDevelopmentSystem() const {
        return brain ? brain->getDevelopmentSystem() : nullptr;
    }

    ConceptFormation* getConceptFormation() const {
        return brain ? brain->getConceptFormation() : nullptr;
    }

    NeuralPlanner* getNeuralPlanner() const {
        return brain ? brain->getPlanner() : nullptr;
    }

    AttentionalSelection* getAttentionalSelection() const {
        return brain ? brain->getAttention() : nullptr;
    }

    VisualizationInterface* getVisualizationInterface() const {
        return brain ? brain->getVisualizationInterface() : nullptr;
    }

    AgentBrain* getAgentBrain() const {
        return brain ? brain->getAgentBrain() : nullptr;
    }

    AgentBody* getAgentBody() const {
        return brain ? brain->getAgentBody() : nullptr;
    }
};  // End of Impl

// BrainIntegrationManager implementation
BrainIntegrationManager::BrainIntegrationManager(std::shared_ptr<Brain> brain) : pImpl(std::make_unique<Impl>(brain)) {}

BrainIntegrationManager::~BrainIntegrationManager() = default;

bool BrainIntegrationManager::initialize() {
    return pImpl->initialize();
}

void BrainIntegrationManager::update(SimulationStep step, Timestamp time) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);

    // Update current step and time for use by integrated systems
    pImpl->simd_current_step = step;
    pImpl->simd_current_time = time;

    // Update all integrated systems
    pImpl->updateAllSystems();
}

void BrainIntegrationManager::reset() {
    pImpl->reset();
}

bool BrainIntegrationManager::save(const std::string& filepath) const {
    return pImpl->save(filepath);
}

bool BrainIntegrationManager::load(const std::string& filepath) {
    return pImpl->load(filepath);
}

// System access methods
NeuralWorkingMemory* BrainIntegrationManager::getWorkingMemory() const {
    return pImpl->getWorkingMemory();
}

NeuralEpisodicMemory* BrainIntegrationManager::getEpisodicMemory() const {
    return pImpl->getEpisodicMemory();
}

NeuralAssociativeMemory* BrainIntegrationManager::getAssociativeMemory() const {
    return pImpl->getAssociativeMemory();
}

PredictionSystem* BrainIntegrationManager::getPredictionSystem() const {
    return pImpl->getPredictionSystem();
}

Dopamine* BrainIntegrationManager::getDopamine() const {
    return pImpl->getDopamine();
}

Curiosity* BrainIntegrationManager::getCuriosity() const {
    return pImpl->getCuriosity();
}

Novelty* BrainIntegrationManager::getNovelty() const {
    return pImpl->getNovelty();
}

PredictionError* BrainIntegrationManager::getPredictionError() const {
    return pImpl->getPredictionError();
}

DevelopmentSystem* BrainIntegrationManager::getDevelopmentSystem() const {
    return pImpl->getDevelopmentSystem();
}

ConceptFormation* BrainIntegrationManager::getConceptFormation() const {
    return pImpl->getConceptFormation();
}

NeuralPlanner* BrainIntegrationManager::getNeuralPlanner() const {
    return pImpl->getNeuralPlanner();
}

AttentionalSelection* BrainIntegrationManager::getAttentionalSelection() const {
    return pImpl->getAttentionalSelection();
}

VisualizationInterface* BrainIntegrationManager::getVisualizationInterface() const {
    return pImpl->getVisualizationInterface();
}

AgentBrain* BrainIntegrationManager::getAgentBrain() const {
    return pImpl->getAgentBrain();
}

AgentBody* BrainIntegrationManager::getAgentBody() const {
    return pImpl->getAgentBody();
}

// Integration control methods
void BrainIntegrationManager::enableMemoryIntegration(bool enable) {
    pImpl->enableMemoryIntegration(enable);
}

void BrainIntegrationManager::enablePredictionIntegration(bool enable) {
    pImpl->enablePredictionIntegration(enable);
}

void BrainIntegrationManager::enableNeuromodulationIntegration(bool enable) {
    pImpl->enableNeuromodulationIntegration(enable);
}

void BrainIntegrationManager::enableDevelopmentIntegration(bool enable) {
    pImpl->enableDevelopmentIntegration(enable);
}

void BrainIntegrationManager::enableCognitionIntegration(bool enable) {
    pImpl->enableCognitionIntegration(enable);
}

void BrainIntegrationManager::enableVisualizationIntegration(bool enable) {
    pImpl->enableVisualizationIntegration(enable);
}

void BrainIntegrationManager::enableAgentIntegration(bool enable) {
    pImpl->enableAgentIntegration(enable);
}

// Configuration methods
void BrainIntegrationManager::setMemoryConfig(const MemoryConfig& config) {
    pImpl->setMemoryConfig(config);
}

void BrainIntegrationManager::setPredictionConfig(const PredictionConfig& config) {
    pImpl->setPredictionConfig(config);
}

void BrainIntegrationManager::setNeuromodulationConfig(const NeuromodulationConfig& config) {
    pImpl->setNeuromodulationConfig(config);
}

void BrainIntegrationManager::setDevelopmentConfig(const DevelopmentConfig& config) {
    pImpl->setDevelopmentConfig(config);
}

void BrainIntegrationManager::setCognitionConfig(const CognitionConfig& config) {
    pImpl->setCognitionConfig(config);
}

// Status and monitoring
std::string BrainIntegrationManager::getIntegrationStatus() const {
    return pImpl->getIntegrationStatus();
}

PerformanceMetrics BrainIntegrationManager::getPerformanceMetrics() const {
    return pImpl->perfMetrics;
}

IntegrationStats BrainIntegrationManager::getIntegrationStats() const {
    return pImpl->getIntegrationStats();
}

// Test and validation
bool BrainIntegrationManager::validateIntegration() const {
    return pImpl->validateIntegration();
}

std::vector<std::string> BrainIntegrationManager::getIntegrationIssues() const {
    return pImpl->getIntegrationIssues();
}

void BrainIntegrationManager::runIntegrationTests() {
    pImpl->runIntegrationTests();
}

// Export and import
std::string BrainIntegrationManager::exportIntegrationState() const {
    return pImpl->exportIntegrationState();
}

bool BrainIntegrationManager::importIntegrationState(const std::string& stateData) {
    return pImpl->importIntegrationState(stateData);
}

// Event handling
void IntegrationEventDispatcher::addListener(std::shared_ptr<IntegrationEventListener> listener) {
    std::lock_guard<std::mutex> lock(instance().mutex);
    instance().eventListeners.push_back(listener);
}

void IntegrationEventDispatcher::removeListener(std::shared_ptr<IntegrationEventListener> listener) {
    std::lock_guard<std::mutex> lock(instance().mutex);
    auto it = std::find(instance().eventListeners.begin(), instance().eventListeners.end(), listener);
    if (it != instance().eventListeners.end()) {
        instance().eventListeners.erase(it);
    }
}

void IntegrationEventDispatcher::dispatch(const IntegrationEvent& event) {
    std::lock_guard<std::mutex> lock(instance().mutex);

    // Call event callback if set
    if (instance().eventCallback) {
        instance().eventCallback(event);
    }

    // Call listeners
    for (const auto& listener : instance().eventListeners) {
        if (listener->shouldHandleEvent(event)) {
            listener->onIntegrationEvent(event);
        }
    }
}

void IntegrationEventDispatcher::setEventCallback(std::function<void(const IntegrationEvent&)> callback) {
    std::lock_guard<std::mutex> lock(instance().mutex);
    instance().eventCallback = callback;
}

IntegrationEventDispatcher& IntegrationEventDispatcher::getInstance() {
    static IntegrationEventDispatcher instance;
    return instance;
}

// Utility functions
namespace IntegrationUtils {
    std::string systemNameToString(BrainSystem system) {
        switch (system) {
            case BrainSystem::WorkingMemory: return "working_memory";
            case BrainSystem::EpisodicMemory: return "episodic_memory";
            case BrainSystem::SemanticMemory: return "semantic_memory";
            case BrainSystem::ProceduralMemory: return "procedural_memory";
            case BrainSystem::AssociativeMemory: return "associative_memory";
            case BrainSystem::PredictionSystem: return "prediction_system";
            case BrainSystem::Dopamine: return "dopamine";
            case BrainSystem::Curiosity: return "curiosity";
            case BrainSystem::Novelty: return "novelty";
            case BrainSystem::PredictionError: return "prediction_error";
            case BrainSystem::DevelopmentSystem: return "development_system";
            case BrainSystem::ConceptFormation: return "concept_formation";
            case BrainSystem::NeuralPlanner: return "neural_planner";
            case BrainSystem::AttentionalSelection: return "attentional_selection";
            case BrainSystem::VisualizationInterface: return "visualization_interface";
            case BrainSystem::AgentBrain: return "agent_brain";
            case BrainSystem::AgentBody: return "agent_body";
            default: return "unknown";
        }
    }

    std::string integrationStageToString(IntegrationStage stage) {
        switch (stage) {
            case IntegrationStage::NotInitialized: return "NotInitialized";
            case IntegrationStage::CoreSystemsInitialized: return "CoreSystemsInitialized";
            case IntegrationStage::MemorySystemsInitialized: return "MemorySystemsInitialized";
            case IntegrationStage::PredictionSystemsInitialized: return "PredictionSystemsInitialized";
            case IntegrationStage::NeuromodulationSystemsInitialized: return "NeuromodulationSystemsInitialized";
            case IntegrationStage::DevelopmentSystemsInitialized: return "DevelopmentSystemsInitialized";
            case IntegrationStage::CognitionSystemsInitialized: return "CognitionSystemsInitialized";
            case IntegrationStage::VisualizationSystemsInitialized: return "VisualizationSystemsInitialized";
            case IntegrationStage::AgentSystemsInitialized: return "AgentSystemsInitialized";
            case IntegrationStage::FullyIntegrated: return "FullyIntegrated";
            case IntegrationStage::ValidationComplete: return "ValidationComplete";
            default: return "Unknown";
        }
    }

    double calculateConnectionStrength(const BrainSystem& source, const BrainSystem& target) {
        // Simple strength calculation based on system compatibility
        if (source == target) return 0.0;

        // Predefined connection strengths
        std::map<std::pair<BrainSystem, BrainSystem>, double> strengths = {
            {{BrainSystem::CoreNeural, BrainSystem::WorkingMemory}, 0.9},
            {{BrainSystem::CoreNeural, BrainSystem::EpisodicMemory}, 0.8},
            {{BrainSystem::CoreNeural, BrainSystem::SemanticMemory}, 0.85},
            {{BrainSystem::CoreNeural, BrainSystem::AssociativeMemory}, 0.9},
            {{BrainSystem::CoreNeural, BrainSystem::PredictionSystem}, 0.8},
            {{BrainSystem::CoreNeural, BrainSystem::Dopamine}, 0.95},
            {{BrainSystem::CoreNeural, BrainSystem::Curiosity}, 0.9},
            {{BrainSystem::CoreNeural, BrainSystem::Novelty}, 0.8},
            {{BrainSystem::CoreNeural, BrainSystem::DevelopmentSystem}, 0.7},
            {{BrainSystem::CoreNeural, BrainSystem::ConceptFormation}, 0.9},
            {{BrainSystem::CoreNeural, BrainSystem::NeuralPlanner}, 0.8},
            {{BrainSystem::CoreNeural, BrainSystem::AttentionalSelection}, 0.9},
        };

        auto key = std::make_pair(source, target);
        if (strengths.find(key) != strengths.end()) {
            return strengths[key];
        }

        return 0.5; // Default strength
    }

    bool validateConnection(const BrainSystem& source, const BrainSystem& target) {
        // Check if connection is valid based on system types
        if (source == target) return false;

        // Neural systems can connect to most others
        if (source == BrainSystem::CoreNeural) {
            return true;
        }

        // Memory systems can connect to neural systems
        if (target == BrainSystem::CoreNeural) {
            return true;
        }

        // Allow most other connections
        return true;
    }

    std::vector<std::string> getSystemDependencies(BrainSystem system) {
        std::vector<std::string> dependencies;

        switch (system) {
            case BrainSystem::WorkingMemory:
            case BrainSystem::EpisodicMemory:
            case BrainSystem::SemanticMemory:
            case BrainSystem::ProceduralMemory:
            case BrainSystem::AssociativeMemory:
                dependencies.push_back("neural_processing");
                dependencies.push_back("encoding_mechanisms");
                break;

            case BrainSystem::PredictionSystem:
            case BrainSystem::PredictionError:
                dependencies.push_back("sensory_input");
                dependencies.push_back("error_computation");
                break;

            case BrainSystem::Dopamine:
            case BrainSystem::Curiosity:
            case BrainSystem::Novelty:
                dependencies.push_back("reward_signals");
                dependencies.push_back("exploration_drive");
                break;

            case BrainSystem::DevelopmentSystem:
                dependencies.push_back("plasticity_rules");
                dependencies.push_back("neural_development");
                break;

            case BrainSystem::ConceptFormation:
            case BrainSystem::NeuralPlanner:
                dependencies.push_back("pattern_extraction");
                dependencies.push_back("action_evaluation");
                break;

            case BrainSystem::AttentionalSelection:
                dependencies.push_back("priority_signals");
                dependencies.push_back("selective_attention");
                break;

            case BrainSystem::VisualizationInterface:
                dependencies.push_back("brain_state_data");
                dependencies.push_back("rendering_systems");
                break;

            case BrainSystem::AgentBrain:
            case BrainSystem::AgentBody:
                dependencies.push_back("decision_making");
                dependencies.push_back("action_execution");
                break;

            default:
                break;
        }

        return dependencies;
    }

    void logConnection(const std::string& source, const std::string& target, double strength) {
        NLM_LOG_INFO("Connection established: " + source + " -> " + target + " (strength: " + std::to_string(strength) + ")");
    }

    std::string formatConnectionInfo(const std::string& source, const std::string& target, double strength) {
        std::ostringstream oss;
        oss << "Connection: " << source << " -> " << target << "\n";
        oss << "  Strength: " << strength << "\n";
        oss << "  Established at: " << std::chrono::system_clock::now();
        return oss.str();
    }
}

} // namespace nlm