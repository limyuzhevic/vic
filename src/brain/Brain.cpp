// Integration test implementations for Phase 6
source nlm/Brain.cpp

// Include all integration headers
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/SemanticMemory.hpp"
#include "memory/ProceduralMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "development/DevelopmentSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "visualisation/VisualizationInterface.hpp"

using namespace nlm;

void Brain::initializeIntegration() {
    // Initialize all integrated systems with proper error handling
    
    // Memory systems
    if (auto* workingMem = getWorkingMemory()) {
        workingMem->initialize();
        NLM_LOG_INFO("Working memory system initialized");
    }
    
    if (auto* episodicMem = getEpisodicMemory()) {
        episodicMem->initialize();
        NLM_LOG_INFO("Episodic memory system initialized");
    }
    
    if (auto* semanticMem = getSemanticMemory()) {
        semanticMem->initialize();
        NLM_LOG_INFO("Semantic memory system initialized");
    }
    
    if (auto* proceduralMem = getProceduralMemory()) {
        proceduralMem->initialize();
        NLM_LOG_INFO("Procedural memory system initialized");
    }
    
    if (auto* associativeMem = getAssociativeMemory()) {
        associativeMem->initialize();
        NLM_LOG_INFO("Associative memory system initialized");
    }
    
    // Neural memory systems
    if (auto* neuralWorkingMem = getNeuralWorkingMemory()) {
        neuralWorkingMem->initialize();
        NLM_LOG_INFO("Neural working memory system initialized");
    }
    
    if (auto* neuralEpisodicMem = getNeuralEpisodicMemory()) {
        neuralEpisodicMem->initialize();
        NLM_LOG_INFO("Neural episodic memory system initialized");
    }
    
    // Prediction system
    if (auto* predictionSystem = getPredictionSystem()) {
        predictionSystem->initialize();
        NLM_LOG_INFO("Prediction system initialized");
    }
    
    // Neuromodulation systems
    if (auto* dopamine = getDopamine()) {
        dopamine->initialize();
        NLM_LOG_INFO("Dopamine system initialized");
    }
    
    if (auto* curiosity = getCuriosity()) {
        curiosity->initialize();
        NLM_LOG_INFO("Curiosity system initialized");
    }
    
    if (auto* novelty = getNovelty()) {
        novelty->initialize();
        NLM_LOG_INFO("Novelty system initialized");
    }
    
    if (auto* predictionError = getPredictionErrorSignal()) {
        predictionError->initialize();
        NLM_LOG_INFO("Prediction error system initialized");
    }
    
    // Development system
    if (auto* development = getDevelopmentSystem()) {
        development->initialize();
        NLM_LOG_INFO("Development system initialized");
    }
    
    // Cognition systems
    if (auto* conceptFormation = getConceptFormation()) {
        conceptFormation->initialize();
        NLM_LOG_INFO("Concept formation system initialized");
    }
    
    if (auto* neuralPlanner = getPlanner()) {
        neuralPlanner->initialize();
        NLM_LOG_INFO("Neural planner system initialized");
    }
    
    if (auto* attentionalSelection = getAttention()) {
        attentionalSelection->initialize();
        NLM_LOG_INFO("Attentional selection system initialized");
    }
    
    // Visualization system
    if (auto* visualization = getVisualizationInterface()) {
        visualization->initialize();
        NLM_LOG_INFO("Visualization system initialized");
    }
    
    NLM_LOG_INFO("All cognitive systems integration initialized successfully");
}

void Brain::connectMemorySystems() {
    // Connect memory systems to neural processing
    
    if (auto* workingMem = getWorkingMemory()) {
        // Working memory should receive input from neural activity
        workingMem->setInputSource(this);
    }
    
    if (auto* episodicMem = getEpisodicMemory()) {
        // Episodic memory should encode experiences from neural processing
        episodicMem->setEncodingSource(this);
    }
    
    if (auto* semanticMem = getSemanticMemory()) {
        // Semantic memory should learn patterns from neural activity
        semanticMem->setLearningSource(this);
    }
    
    if (auto* associativeMem = getAssociativeMemory()) {
        // Associative memory should form connections based on co-activation
        associativeMem->setAssociationSource(this);
    }
}

void Brain::connectPredictionSystem() {
    if (auto* predictionSystem = getPredictionSystem()) {
        // Connect prediction system to sensory input
        predictionSystem->setSensoryInputSource(this);
        
        // Connect prediction errors to learning
        predictionSystem->setLearningTarget(this);
    }
    
    if (auto* predictionError = getPredictionErrorSignal()) {
        // Prediction error should modulate learning rates
        predictionError->setModulationTarget(this);
    }
}

void Brain::connectNeuromodulationSystems() {
    if (auto* dopamine = getDopamine()) {
        // Dopamine should modulate plasticity based on reward
        dopamine->setPlasticityTarget(this);
    }
    
    if (auto* curiosity = getCuriosity()) {
        // Curiosity should drive exploration
        curiosity->setExplorationTarget(this);
    }
    
    if (auto* novelty = getNovelty()) {
        // Novelty should trigger learning
        novelty->setLearningTrigger(this);
    }
}

void Brain::connectDevelopmentSystem() {
    if (auto* development = getDevelopmentSystem()) {
        // Development should affect plasticity over time
        development->setPlasticityTarget(this);
        
        // Development should affect neural properties
        development->setNeuralTarget(this);
    }
}

void Brain::connectCognitionSystems() {
    if (auto* conceptFormation = getConceptFormation()) {
        // Concept formation should discover patterns from neural activity
        conceptFormation->setPatternSource(this);
    }
    
    if (auto* neuralPlanner = getPlanner()) {
        // Neural planner should select actions based on neural evaluation
        neuralPlanner->setActionSource(this);
    }
    
    if (auto* attentionalSelection = getAttention()) {
        // Attentional selection should prioritize neural signals
        attentionalSelection->setPrioritySource(this);
    }
}

void Brain::connectVisualizationSystem() {
    if (auto* visualization = getVisualizationInterface()) {
        // Visualization should receive brain state updates
        visualization->setDataSource(this);
        
        // Visualization should provide feedback
        visualization->setFeedbackTarget(this);
    }
}

void Brain::connectAgentSystems() {
    // Connect brain to agent systems for embodied cognition
    if (auto* agentBrain = getAgentBrain()) {
        agentBrain->setBrainSource(this);
        agentBrain->setActionTarget(this);
    }
    
    if (auto* agentBody = getAgentBody()) {
        agentBody->setBehaviorSource(this);
        agentBody->setPerceptionTarget(this);
    }
}

void Brain::updateIntegratedSystems(SimulationStep step, Timestamp time) {
    // Update all integrated systems in the correct order
    
    // 1. Update core neural dynamics first
    updateNeuralDynamics(step, time);
    
    // 2. Update memory systems (they need neural activity)
    updateMemorySystems(step, time);
    
    // 3. Update prediction systems
    updatePredictionSystems(step, time);
    
    // 4. Update neuromodulation systems
    updateNeuromodulationSystems(step, time);
    
    // 5. Update development systems
    updateDevelopmentSystems(step, time);
    
    // 6. Update cognition systems
    updateCognitionSystems(step, time);
    
    // 7. Update visualization systems
    updateVisualizationSystems(step, time);
    
    // 8. Update agent systems
    updateAgentSystems(step, time);
}

void Brain::updateMemorySystems(SimulationStep step, Timestamp time) {
    if (auto* workingMem = getWorkingMemory()) {
        workingMem->update(step, time);
    }
    
    if (auto* episodicMem = getEpisodicMemory()) {
        episodicMem->update(step, time);
    }
    
    if (auto* semanticMem = getSemanticMemory()) {
        semanticMem->update(step, time);
    }
    
    if (auto* proceduralMem = getProceduralMemory()) {
        proceduralMem->update(step, time);
    }
    
    if (auto* associativeMem = getAssociativeMemory()) {
        associativeMem->update(step, time);
    }
    
    if (auto* neuralWorkingMem = getNeuralWorkingMemory()) {
        neuralWorkingMem->update(step, time);
    }
    
    if (auto* neuralEpisodicMem = getNeuralEpisodicMemory()) {
        neuralEpisodicMem->update(step, time);
    }
}

void Brain::updatePredictionSystems(SimulationStep step, Timestamp time) {
    if (auto* predictionSystem = getPredictionSystem()) {
        predictionSystem->update(step, time);
    }
    
    if (auto* predictionError = getPredictionErrorSignal()) {
        predictionError->update(step, time);
    }
}

void Brain::updateNeuromodulationSystems(SimulationStep step, Timestamp time) {
    if (auto* dopamine = getDopamine()) {
        dopamine->update(step, time);
    }
    
    if (auto* curiosity = getCuriosity()) {
        curiosity->update(step, time);
    }
    
    if (auto* novelty = getNovelty()) {
        novelty->update(step, time);
    }
}

void Brain::updateDevelopmentSystems(SimulationStep step, Timestamp time) {
    if (auto* development = getDevelopmentSystem()) {
        development->update(step, time);
    }
}

void Brain::updateCognitionSystems(SimulationStep step, Timestamp time) {
    if (auto* conceptFormation = getConceptFormation()) {
        conceptFormation->update(step, time);
    }
    
    if (auto* neuralPlanner = getPlanner()) {
        neuralPlanner->update(step, time);
    }
    
    if (auto* attentionalSelection = getAttention()) {
        attentionalSelection->update(step, time);
    }
}

void Brain::updateVisualizationSystems(SimulationStep step, Timestamp time) {
    if (auto* visualization = getVisualizationInterface()) {
        visualization->update(step, time);
    }
}

void Brain::updateAgentSystems(SimulationStep step, Timestamp time) {
    if (auto* agentBrain = getAgentBrain()) {
        agentBrain->update(step, time);
    }
    
    if (auto* agentBody = getAgentBody()) {
        agentBody->update(step, time);
    }
}

bool Brain::saveIntegratedState(const std::string& filepath) const {
    // Save integrated state including all connected systems
    
    // Create state data structure
    std::map<std::string, std::string> stateData;
    
    // Core brain state
    stateData["brain_neurons"] = std::to_string(getTotalNeuronCount());
    stateData["brain_synapses"] = std::to_string(getTotalSynapseCount());
    stateData["brain_spikes"] = std::to_string(getTotalSpikeCount());
    stateData["brain_firing_rate"] = std::to_string(getAverageFiringRate());
    
    // Memory system states
    if (auto* workingMem = getWorkingMemory()) {
        stateData["memory_working_capacity"] = std::to_string(workingMem->getCapacity());
        stateData["memory_working_usage"] = std::to_string(workingMem->getUsage());
    }
    
    if (auto* episodicMem = getEpisodicMemory()) {
        stateData["memory_episodic_episodes"] = std::to_string(episodicMem->getEpisodeCount());
    }
    
    // Prediction system state
    if (auto* predictionSystem = getPredictionSystem()) {
        stateData["prediction_confidence"] = std::to_string(predictionSystem->getConfidence());
        stateData["prediction_accuracy"] = std::to_string(predictionSystem->getAccuracy());
    }
    
    // Neuromodulation states
    if (auto* dopamine = getDopamine()) {
        stateData["neuromodulation_dopamine"] = std::to_string(dopamine->getLevel());
    }
    
    if (auto* curiosity = getCuriosity()) {
        stateData["neuromodulation_curiosity"] = std::to_string(curiosity->getLevel());
    }
    
    // Development state
    if (auto* development = getDevelopmentSystem()) {
        stateData["development_stage"] = std::to_string(static_cast<int>(development->getStage()));
        stateData["development_age"] = std::to_string(development->getAge());
    }
    
    // Cognition states
    if (auto* conceptFormation = getConceptFormation()) {
        stateData["cognition_concepts"] = std::to_string(conceptFormation->getConceptCount());
    }
    
    if (auto* neuralPlanner = getPlanner()) {
        stateData["cognition_plans"] = std::to_string(neuralPlanner->getPlanCount());
    }
    
    // Export to file (simplified implementation)
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : stateData) {
        file << pair.first << " = " << pair.second << "\n";
    }
    
    file.close();
    return true;
}

bool Brain::loadIntegratedState(const std::string& filepath) {
    // Load integrated state from file
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::map<std::string, std::string> loadedData;
    
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = nlm::trim(line.substr(0, pos));
            std::string value = nlm::trim(line.substr(pos + 1));
            loadedData[key] = value;
        }
    }
    
    file.close();
    
    // Restore brain state
    if (loadedData.find("brain_neurons") != loadedData.end()) {
        // Reset and reinitialize brain with saved state
        reset();
        // Note: In a full implementation, we would restore actual neural connections
        // For now, we just reinitialize with the same configuration
    }
    
    // Note: In a complete implementation, each integrated system would
    // have its own save/load mechanism to restore its specific state
    
    NLM_LOG_INFO("Integrated state loaded from: " + filepath);
    return true;
}

void Brain::exportForVisualization() {
    if (auto* visualization = getVisualizationInterface()) {
        visualization->exportBrainState(*this);
    }
}

void Brain::importFromVisualization(const VisualizationData& data) {
    if (auto* visualization = getVisualizationInterface()) {
        visualization->importBrainState(data, *this);
    }
}

std::string Brain::getIntegratedStatus() const {
    std::ostringstream oss;
    oss << "=== Integrated Brain Status ===\n";
    oss << "Core Systems:\n";
    oss << "  - Neurons: " << getTotalNeuronCount() << "\n";
    oss << "  - Synapses: " << getTotalSynapseCount() << "\n";
    oss << "  - Spikes: " << getTotalSpikeCount() << "\n";
    oss << "  - Firing Rate: " << getAverageFiringRate() << " Hz\n";
    
    oss << "\nMemory Systems:\n";
    if (auto* workingMem = getWorkingMemory()) {
        oss << "  - Working Memory: " << workingMem->getUsage() << "/" << workingMem->getCapacity() << "\n";
    }
    if (auto* episodicMem = getEpisodicMemory()) {
        oss << "  - Episodic Memory: " << episodicMem->getEpisodeCount() << " episodes\n";
    }
    
    oss << "\nPrediction Systems:\n";
    if (auto* predictionSystem = getPredictionSystem()) {
        oss << "  - Prediction Confidence: " << predictionSystem->getConfidence() << "\n";
        oss << "  - Prediction Accuracy: " << predictionSystem->getAccuracy() << "\n";
    }
    
    oss << "\nNeuromodulation:\n";
    if (auto* dopamine = getDopamine()) {
        oss << "  - Dopamine Level: " << dopamine->getLevel() << "\n";
    }
    if (auto* curiosity = getCuriosity()) {
        oss << "  - Curiosity Level: " << curiosity->getLevel() << "\n";
    }
    
    oss << "\nDevelopment:\n";
    if (auto* development = getDevelopmentSystem()) {
        oss << "  - Stage: " << static_cast<int>(development->getStage()) << " (" 
            << developmentStageToString(development->getStage()) << ")\n";
        oss << "  - Age: " << development->getAge() << " steps\n";
    }
    
    oss << "\nCognition:\n";
    if (auto* conceptFormation = getConceptFormation()) {
        oss << "  - Concepts: " << conceptFormation->getConceptCount() << "\n";
    }
    if (auto* neuralPlanner = getPlanner()) {
        oss << "  - Plans: " << neuralPlanner->getPlanCount() << "\n";
    }
    
    oss << "\n=== End Integrated Status ===\n";
    return oss.str();
}

std::string Brain::developmentStageToString(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}
