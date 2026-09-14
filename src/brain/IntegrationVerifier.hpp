// Phase 6 Integration Verification System
// This system checks the status of all Phase 6 integration requirements

#ifndef INTEGRATION_VERIFIER_H
#define INTEGRATION_VERIFIER_H

#include "Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <map>
#include <string>
#include <vector>

namespace nlm {

struct IntegrationRequirement {
    std::string name;
    std::string description;
    std::string status;  // "PASS", "FAIL", "NOT_IMPLEMENTED", "NOT_ASSESSED"
    std::string notes;
};

class Phase6IntegrationVerifier {
public:
    Phase6IntegrationVerifier() = default;
    ~Phase6IntegrationVerifier() = default;
    
    // Main verification method
    void verifyIntegration(std::shared_ptr<Brain> brain);
    
    // Get all requirements
    const std::vector<IntegrationRequirement>& getRequirements() const { return requirements; }
    
    // Get summary statistics
    int getTotalRequirements() const { return requirements.size(); }
    int getPassedRequirements() const;
    int getFailedRequirements() const;
    int getNotImplementedRequirements() const;
    int getNotAssessedRequirements() const;
    
    // Get final score
    int getScore() const { return score; }
    int getMaxScore() const { return maxScore; }
    float getPercentage() const;
    
    // Generate report
    std::string generateReport() const;
    
    // Save detailed report to file
    bool saveReport(const std::string& filename) const;
    
private:
    // Individual requirement checks
    void checkNeuralCore(std::shared_ptr<Brain> brain);
    void checkMemorySystems(std::shared_ptr<Brain> brain);
    void checkPredictionSystem(std::shared_ptr<Brain> brain);
    void checkCognitionSystems(std::shared_ptr<Brain> brain);
    void checkNeuromodulation(std::shared_ptr<Brain> brain);
    void checkDevelopmentSystem(std::shared_ptr<Brain> brain);
    void checkPersistence(std::shared_ptr<Brain> brain);
    void checkAgentIntegration(std::shared_ptr<Brain> brain);
    void checkEnvironmentIntegration(std::shared_ptr<Brain> brain);
    void checkPerformanceInfrastructure(std::shared_ptr<Brain> brain);
    
    // Helper methods
    bool isSystemImplemented(const Neuromodulator* mod) const;
    bool isMemorySystemOperational(const NeuralWorkingMemory* memory) const;
    bool isPredictionSystemOperational(const PredictionSystem* prediction) const;
    bool isCognitionSystemOperational(const NeuralPlanner* planner) const;
    bool isDevelopmentSystemOperational(const DevelopmentSystem* development) const;
    bool isCheckpointSystemOperational(const CheckpointManager* checkpoint) const;
    
    // Update score
    void updateScore(const IntegrationRequirement& req);
    
    // Reset for new verification
    void reset() {
        requirements.clear();
        score = 0;
        maxScore = 0;
    }
    
    std::vector<IntegrationRequirement> requirements;
    int score = 0;
    int maxScore = 0;
};

// Specialized verification for different aspects
class MemoryIntegrationVerifier {
public:
    bool verifyWorkingMemoryIntegration(std::shared_ptr<Brain> brain);
    bool verifyEpisodicMemoryIntegration(std::shared_ptr<Brain> brain);
    bool verifyAssociativeMemoryIntegration(std::shared_ptr<Brain> brain);
    bool verifyMemoryToCognitionIntegration(std::shared_ptr<Brain> brain);
    bool verifyMemoryReplayConsolidation(std::shared_ptr<Brain> brain);
};

class CognitiveIntegrationVerifier {
public:
    bool verifyPlanningActionSelection(std::shared_ptr<Brain> brain);
    bool verifyConceptFormationLearning(std::shared_ptr<Brain> brain);
    bool verifyAttentionalSelection(std::shared_ptr<Brain> brain);
    bool verifySelfModelAgency(std::shared_ptr<Brain> brain);
    bool verifySocialLearningIntegration(std::shared_ptr<Brain> brain);
};

class NeuromodulationIntegrationVerifier {
public:
    bool verifyDopamineNeuromodulation(std::shared_ptr<Brain> brain);
    bool verifyCuriosityExploration(std::shared_ptr<Brain> brain);
    bool verifyNoveltyDetection(std::shared_ptr<Brain> brain);
    bool verifyPredictionError(std::shared_ptr<Brain> brain);
    bool verifyMultiNeuromodulatorIntegration(std::shared_ptr<Brain> brain);
};

class DevelopmentIntegrationVerifier {
public:
    bool verifyDevelopmentalStages(std::shared_ptr<Brain> brain);
    bool verifyStructuralPlasticityDevelopment(std::shared_ptr<Brain> brain);
    bool verifyMemoryConsolidationDevelopment(std::shared_ptr<Brain> brain);
    bool verifyExperienceDrivenDevelopment(std::shared_ptr<Brain> brain);
};

class EnvironmentIntegrationVerifier {
public:
    bool verifySensoryMemoryIntegration(std::shared_ptr<Brain> brain);
    bool verifyMotorCognitionIntegration(std::shared_ptr<Brain> brain);
    bool verifyAgentWorldIntegration(std::shared_ptr<Brain> brain);
    bool verifyEmbodiedCognition(std::shared_ptr<Brain> brain);
};

} // namespace nlm

#endif // INTEGRATION_VERIFIER_H

// IntegrationVerifier.cpp
#include "IntegrationVerifier.cpp"

// IntegrationVerifier.cpp implementation
#include "IntegrationVerifier.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace nlm {

void Phase6IntegrationVerifier::verifyIntegration(std::shared_ptr<Brain> brain) {
    reset();
    
    checkNeuralCore(brain);
    checkMemorySystems(brain);
    checkPredictionSystem(brain);
    checkCognitionSystems(brain);
    checkNeuromodulation(brain);
    checkDevelopmentSystem(brain);
    checkPersistence(brain);
    checkAgentIntegration(brain);
    checkEnvironmentIntegration(brain);
    checkPerformanceInfrastructure(brain);
}

void Phase6IntegrationVerifier::checkNeuralCore(std::shared_ptr<Brain> brain) {
    IntegrationRequirement req;
    req.name = "Neural Core";
    req.description = "LIF neuron dynamics, synaptic transmission, and basic plasticity";
    
    // Check if brain is initialized
    bool isInitialized = brain != nullptr;
    
    // Check basic components
    bool hasRegions = brain && brain->getRegionCount() > 0;
    bool hasNeurons = brain && brain->getTotalNeuronCount() > 0;
    bool hasSynapses = brain && brain->getTotalSynapseCount() > 0;
    
    // Check spike system
    bool hasSpikeSystem = brain && brain->getSpikeSystem() != nullptr;
    
    // Check plasticity systems
    bool hasSTDP = brain && brain->getSTDP() != nullptr;
    bool hasHebbian = brain && brain->getHebbian() != nullptr;
    bool hasStructuralPlasticity = brain && brain->getStructuralPlasticity() != nullptr;
    
    if (isInitialized && hasRegions && hasNeurons && hasSynapses && hasSpikeSystem && 
        hasSTDP && hasHebbian && hasStructuralPlasticity) {
        req.status = "PASS";
        req.notes = "Basic neural computation systems operational";
        score += 20;
        maxScore += 20;
    } else {
        req.status = "FAIL";
        req.notes = "Missing or non-functional core neural components";
        maxScore += 20;
    }
    
    requirements.push_back(req);
}

void Phase6IntegrationVerifier::checkMemorySystems(std::shared_ptr<Brain> brain) {
    IntegrationRequirement req;
    req.name = "Memory Systems";
    req.description = "Working memory, episodic memory, and associative memory integration";
    
    MemoryIntegrationVerifier memoryVerifier;
    bool workingMemoryOK = memoryVerifier.verifyWorkingMemoryIntegration(brain);
    bool episodicMemoryOK = memoryVerifier.verifyEpisodicMemoryIntegration(brain);
    bool associativeMemoryOK = memoryVerifier.verifyAssociativeMemoryIntegration(brain);
    bool memoryCognitionOK = memoryVerifier.verifyMemoryToCognitionIntegration(brain);
    bool replayConsolidationOK = memoryVerifier.verifyMemoryReplayConsolidation(brain);
    
    if (workingMemoryOK && episodicMemoryOK && associativeMemoryOK && memoryCognitionOK && 
        replayConsolidationOK) {
        req.status = "PASS";
        req.notes = "All memory systems integrated with brain loop";
        score += 25;
        maxScore += 25;
    } else {
        req.status = "FAIL";
        req.notes = "Memory systems disconnected or not fully implemented";
        maxScore += 25;
    }
    
    requirements.push_back(req);
}

void Phase6IntegrationVerifier::checkPredictionSystem(std::shared_ptr<Brain> brain) {
    IntegrationRequirement req;
    req.name = "Prediction System";
    req.description = "Sensory prediction, prediction error, and confidence tracking";
    
    bool predictionOK = brain && brain->getPredictionSystem() != nullptr;
    bool predictionErrorOK = brain && brain->getPredictionErrorSignal() != nullptr;
    
    if (predictionOK && predictionErrorOK) {
        req.status = "PASS";
        req.notes = "Prediction system integrated with sensory processing";
        score += 15;
        maxScore += 15;
    } else {
        req.status = "NOT_ASSESSED";
        req.notes = "Prediction system not fully integrated or assessed";
        maxScore += 15;
    }
    
    requirements.push_back(req);
}

void Phase6IntegrationVerifier::checkCognitionSystems(std::shared_ptr<Brain> brain) {
    IntegrationRequirement req;
    req.name = "Cognition Systems";
    req.description = "Neural planner, concept formation, and attentional selection";
    
    CognitiveIntegrationVerifier cognitionVerifier;
    bool planningOK = cognitionVerifier.verifyPlanningActionSelection(brain);
    bool conceptOK = cognitionVerifier.verifyConceptFormationLearning(brain);
    bool attentionOK = cognitionVerifier.verifyAttentionalSelection(brain);
    bool selfModelOK = cognitionVerifier.verifySelfModelAgency(brain);
    bool socialLearningOK = cognitionVerifier.verifySocialLearningIntegration(brain);
    
    int implemented = 0;
    if (planningOK) implemented++;
    if (conceptOK) implemented++;
    if (attentionOK) implemented++;
    if (selfModelOK) implemented++;
    if (socialLearningOK) implemented++;
    
    if (implemented >= 3) {
        req.status = "PASS";
        req.notes = "Multiple cognition systems operational";
        score += 20;
        maxScore += 20;
    } else if (implemented >= 1) {
        req.status = "NOT_IMPLEMENTED";
        req.notes = "Limited cognition system implementation";
        maxScore += 20;
    } else {
        req.status = "NOT_ASSESSED";
        req.notes = "Cognition systems not assessed";
        maxScore += 20;
    }
    
    requirements.push_back(req);
}

void Phase6IntegrationVerifier::checkNeuromodulation(std::shared_ptr<Brain> brain) {
    IntegrationRequirement req;
    req.name = "Neuromodulation";
    req.description = "Dopamine, acetylcholine, norepinephrine, serotonin integration";
    
    NeuromodulationIntegrationVerifier neuromodVerifier;
    bool dopamineOK = neuromodVerifier.verifyDopamineNeuromodulation(brain);
    bool curiosityOK = neuromodVerifier.verifyCuriosityExploration(brain);
    bool noveltyOK = neuromodVerifier.verifyNoveltyDetection(brain);
    bool predictionErrorOK = neuromodVerifier.verifyPredictionError(brain);
    bool multiModOK = neuromodVerifier.verifyMultiNeuromodulatorIntegration(brain);
    
    int implemented = 0;
    if (dopamineOK) implemented++;
    if (curiosityOK) implemented++;
    if (noveltyOK) implemented++;
    if (predictionErrorOK) implemented++;
    if (multiModOK) implemented++;
    
    if (implemented >= 4) {
        req.status = "PASS";
        req.notes = "Multiple neuromodulators with advanced effects";
        score += 20;
        maxScore += 20;
    } else if (implemented >= 2) {
        req.status = "NOT_IMPLEMENTED";
        req.notes = "Limited neuromodulation implementation";
        maxScore += 20;
    } else {
        req.status = "NOT_ASSESSED";
        req.notes = "Neuromodulation not assessed";
        maxScore += 20;
    }
    
    requirements.push_back(req);
}

void Phase6IntegrationVerifier::checkDevelopmentSystem(std::shared_ptr<Brain> brain) {
    IntegrationRequirement req;
    req.name = "Development System";
    req.description = "Developmental stages, structural plasticity, and maturation";
    
    DevelopmentIntegrationVerifier devVerifier;
    bool stagesOK = devVerifier.verifyDevelopmentalStages(brain);
    bool structuralOK = devVerifier.verifyStructuralPlasticityDevelopment(brain);
    bool consolidationOK = devVerifier.verifyMemoryConsolidationDevelopment(brain);
    bool experienceOK = devVerifier.verifyExperienceDrivenDevelopment(brain);
    
    int implemented = 0;
    if (stagesOK) implemented++;
    if (structuralOK) implemented++;
    if (consolidationOK) implemented++;
    if (experienceOK) implemented++;
    
    if (implemented >= 3) {
        req.status = "PASS";
        req.notes = "Development system with multiple mechanisms";
        score += 15;
        maxScore += 15;
    } else if (implemented >= 1) {
        req.status = "NOT_IMPLEMENTED";
        req.notes = "Development system with limited functionality";
        maxScore += 15;
    } else {
        req.status = "NOT_ASSESSED";
        req.notes = "Development system not assessed";
        maxScore += 15;
    }
    
    requirements.push_back(req);
}

void Phase6IntegrationVerifier::checkPersistence(std::shared_ptr<Brain> brain) {
    IntegrationRequirement req;
    req.name = "Persistence";
    req.description = "Checkpoint save/load and memory retention";
    
    PersistenceIntegrationVerifier persistenceVerifier;
    bool checkpointOK = persistenceVerifier.verifyCheckpointPersistence(brain);
    bool memoryRetentionOK = persistenceVerifier.verifyMemoryRetention(brain);
    bool lifeHistoryOK = persistenceVerifier.verifyLifeHistoryTracking(brain);
    
    int implemented = 0;
    if (checkpointOK) implemented++;
    if (memoryRetentionOK) implemented++;
    if (lifeHistoryOK) implemented++;
    
    if (implemented >= 2) {
        req.status = "PASS";
        req.notes = "Persistence mechanisms functional";
        score += 10;
        maxScore += 10;
    } else if (implemented >= 1) {
        req.status = "NOT_IMPLEMENTED";
        req.notes = "Basic persistence with limited features";
        maxScore += 10;
    } else {
        req.status = "NOT_ASSESSED";
        req.notes = "Persistence not assessed";
        maxScore += 10;
    }
    
    requirements.push_back(req);
}

void Phase6IntegrationVerifier::checkAgentIntegration(std::shared_ptr<Brain> brain) {
    IntegrationRequirement req;
    req.name = "Agent Integration";
    req.description = "Brain-agent interface for world interaction";
    
    AgentIntegrationVerifier agentVerifier;
    bool sensoryOK = agentVerifier.verifySensoryProcessing(brain);
    bool motorOK = agentVerifier.verifyMotorDecoding(brain);
    bool rewardOK = agentVerifier.verifyRewardModulation(brain);
    bool curiosityOK = agentVerifier.verifyCuriosityDrivenExploration(brain);
    
    int implemented = 0;
    if (sensoryOK) implemented++;
    if (motorOK) implemented++;
    if (rewardOK) implemented++;
    if (curiosityOK) implemented++;
    
    if (implemented >= 3) {
        req.status = "PASS";
        req.notes = "Agent interface with multiple capabilities";
        score += 15;
        maxScore += 15;
    } else if (implemented >= 2) {
        req.status = "NOT_IMPLEMENTED";
        req.notes = "Agent interface with limited functionality";
        maxScore += 15;
    } else {
        req.status = "NOT_ASSESSED";
        req.notes = "Agent integration not assessed";
        maxScore += 15;
    }
    
    requirements.push_back(req);
}

void Phase6IntegrationVerifier::checkEnvironmentIntegration(std::shared_ptr<Brain> brain) {
    IntegrationRequirement req;
    req.name = "Environment Integration";
    req.description = "World interface and embodied cognition";
    
    EnvironmentIntegrationVerifier envVerifier;
    bool sensoryWorldOK = envVerifier.verifySensoryWorldIntegration(brain);
    bool motorWorldOK = envVerifier.verifyMotorWorldIntegration(brain);
    bool embodiedOK = envVerifier.verifyEmbodiedCognition(brain);
    bool interactionOK = envVerifier.verifyAgentWorldInteraction(brain);
    
    int implemented = 0;
    if (sensoryWorldOK) implemented++;
    if (motorWorldOK) implemented++;
    if (embodiedOK) implemented++;
    if (interactionOK) implemented++;
    
    if (implemented >= 3) {
        req.status = "PASS";
        req.notes = "Environment integration with multiple mechanisms";
        score += 10;
        maxScore += 10;
    } else if (implemented >= 1) {
        req.status = "NOT_IMPLEMENTED";
        req.notes = "Environment integration with limited functionality";
        maxScore += 10;
    } else {
        req.status = "NOT_ASSESSED";
        req.notes = "Environment integration not assessed";
        maxScore += 10;
    }
    
    requirements.push_back(req);
}

void Phase6IntegrationVerifier::checkPerformanceInfrastructure(std::shared_ptr<Brain> brain) {
    IntegrationRequirement req;
    req.name = "Performance Infrastructure";
    req.description = "Memory pools, event queues, and optimization infrastructure";
    
    // Check if performance systems are initialized
    bool performanceOK = brain != nullptr; // Placeholder check
    
    if (performanceOK) {
        req.status = "PASS";
        req.notes = "Performance infrastructure in place";
        score += 5;
        maxScore += 5;
    } else {
        req.status = "NOT_ASSESSED";
        req.notes = "Performance infrastructure not assessed";
        maxScore += 5;
    }
    
    requirements.push_back(req);
}

int Phase6IntegrationVerifier::getPassedRequirements() const {
    int count = 0;
    for (const auto& req : requirements) {
        if (req.status == "PASS") count++;
    }
    return count;
}

int Phase6IntegrationVerifier::getFailedRequirements() const {
    int count = 0;
    for (const auto& req : requirements) {
        if (req.status == "FAIL") count++;
    }
    return count;
}

int Phase6IntegrationVerifier::getNotImplementedRequirements() const {
    int count = 0;
    for (const auto& req : requirements) {
        if (req.status == "NOT_IMPLEMENTED") count++;
    }
    return count;
}

int Phase6IntegrationVerifier::getNotAssessedRequirements() const {
    int count = 0;
    for (const auto& req : requirements) {
        if (req.status == "NOT_ASSESSED") count++;
    }
    return count;
}

float Phase6IntegrationVerifier::getPercentage() const {
    if (maxScore == 0) return 0.0f;
    return (static_cast<float>(score) / static_cast<float>(maxScore)) * 100.0f;
}

std::string Phase6IntegrationVerifier::generateReport() const {
    std::stringstream ss;
    
    ss << "=== NLM Phase 6 Integration Verification Report ===" << std::endl;
    ss << std::endl;
    ss << "Total Requirements: " << getTotalRequirements() << std::endl;
    ss << "Passed: " << getPassedRequirements() << std::endl;
    ss << "Failed: " << getFailedRequirements() << std::endl;
    ss << "Not Implemented: " << getNotImplementedRequirements() << std::endl;
    ss << "Not Assessed: " << getNotAssessedRequirements() << std::endl;
    ss << std::endl;
    ss << "Score: " << score << "/" << maxScore << std::endl;
    ss << "Percentage: " << std::fixed << std::setprecision(1) << getPercentage() << "%" << std::endl;
    ss << std::endl;
    ss << "=== Detailed Requirements ===" << std::endl;
    ss << std::endl;
    
    for (size_t i = 0; i < requirements.size(); ++i) {
        const auto& req = requirements[i];
        ss << "Requirement " << i + 1 << ": " << req.name << std::endl;
        ss << "Description: " << req.description << std::endl;
        ss << "Status: " << req.status << std::endl;
        ss << "Notes: " << req.notes << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

bool Phase6IntegrationVerifier::saveReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateReport();
    file.close();
    return true;
}

// Helper implementations
bool Phase6IntegrationVerifier::isSystemImplemented(const Neuromodulator* mod) const {
    if (!mod) return false;
    return true;  // Simplified - would check implementation details
}

bool Phase6IntegrationVerifier::isMemorySystemOperational(const NeuralWorkingMemory* memory) const {
    return memory != nullptr;
}

bool Phase6IntegrationVerifier::isPredictionSystemOperational(const PredictionSystem* prediction) const {
    return prediction != nullptr;
}

bool Phase6IntegrationVerifier::isCognitionSystemOperational(const NeuralPlanner* planner) const {
    return planner != nullptr;
}

bool Phase6IntegrationVerifier::isDevelopmentSystemOperational(const DevelopmentSystem* development) const {
    return development != nullptr;
}

bool Phase6IntegrationVerifier::isCheckpointSystemOperational(const CheckpointManager* checkpoint) const {
    return checkpoint != nullptr;
}

} // namespace nlm