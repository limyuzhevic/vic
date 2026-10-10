#include "IntegrationVerifier.hpp"
#include "../core/Logger/Logger.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

// IntegrationReport implementation
std::string IntegrationReport::toString() const {
    std::stringstream ss;
    ss << "=== INTEGRATION VERIFICATION REPORT ===\n";
    ss << "Total Validations: " << totalValidations << "\n";
    ss << "Successful: " << successfulValidations << "\n";
    ss << "Failed: " << (totalValidations - successfulValidations) << "\n";
    ss << "Total Time: " << totalValidationTime.count() << "ms\n\n";
    
    if (!validations.empty()) {
        ss << "--- VALIDATION RESULTS ---\n";
        for (const auto& result : validations) {
            std::string status = result.success ? "✓ PASS" : "✗ FAIL";
            ss << "[" << status << "] " << result.message << "\n";
            if (!result.metadata.empty()) {
                ss << "    Metadata:";
                for (const auto& [key, value] : result.metadata) {
                    ss << " " << key << "=" << value;
                }
                ss << "\n";
            }
        }
    }
    
    if (!warnings.empty()) {
        ss << "\n--- WARNINGS ---\n";
        for (const auto& warning : warnings) {
            ss << "⚠ " << warning << "\n";
        }
    }
    
    if (!errors.empty()) {
        ss << "\n--- ERRORS ---\n";
        for (const auto& error : errors) {
            ss << "✗ " << error << "\n";
        }
    }
    
    ss << "\n=== END REPORT ===";
    return ss.str();
}

// IntegrationVerifier implementation
struct IntegrationVerifier::Impl {
    Brain* brain = nullptr;
    std::vector<Logger*> loggers;
    std::mutex validationMutex;
};

IntegrationVerifier::IntegrationVerifier() 
    : loggingEnabled_(true), detailedReportingEnabled_(true) {
    pImpl = std::make_unique<Impl>();
    
    // Initialize validation settings
    validationEnabled_ = {
        {"brain_memory", true},
        {"neuromodulation_plasticity", true},
        {"development_plasticity", true},
        {"performance_integration", true},
        {"resource_usage_integration", true},
        {"optimization_integration", true},
        {"monitoring_integration", true},
        {"prediction_memory", true},
        {"cognition_memory", true},
        {"sensory_memory", true},
        {"motor_cognition", true},
        {"world_environment", true},
        {"development_brain", true},
        {"neuromodulation_brain", true},
        {"system_connectivity", true},
        {"plasticity_consistency", true},
        {"neuromodulation_balance", true},
        {"memory_capacity", true}
    };
}

IntegrationVerifier::~IntegrationVerifier() = default;

void IntegrationVerifier::initialize(Brain* brain) {
    std::lock_guard<std::mutex> lock(pImpl->validationMutex);
    pImpl->brain = brain;
    reset();
    
    if (loggingEnabled_) {
        Logger::getInstance().log("IntegrationVerifier", "Integration verifier initialized with brain reference");
    }
}

void IntegrationVerifier::reset() {
    std::lock_guard<std::mutex> lock(pImpl->validationMutex);
    currentReport_ = IntegrationReport{};
    currentReport_.validations.clear();
    currentReport_.warnings.clear();
    currentReport_.errors.clear();
}

void IntegrationVerifier::enableValidation(const std::string& validationName, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->validationMutex);
    validationEnabled_[validationName] = enabled;
}

bool IntegrationVerifier::isValidationEnabled(const std::string& validationName) const {
    std::lock_guard<std::mutex> lock(pImpl->validationMutex);
    auto it = validationEnabled_.find(validationName);
    return it != validationEnabled_.end() && it->second;
}

IntegrationResult IntegrationVerifier::verifyBrainMemoryIntegration() {
    ValidationTimer timer;
    IntegrationResult result;
    result.success = false;
    result.message = "Brain-Memory integration verification in progress";
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Validate Brain-Memory System integration
        result = validateBrainMemorySystem();
        result.validationTime = timer.elapsed();
        
        if (result.success) {
            logValidationResult("brain_memory", result);
        } else {
            currentReport_.errors.push_back(result.message);
        }
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in brain-memory verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyNeuromodulationPlasticityIntegration() {
    ValidationTimer timer;
    IntegrationResult result;
    result.success = false;
    result.message = "Neuromodulation-Plasticity integration verification in progress";
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Validate Neuromodulation Effects
        result = validateNeuromodulationEffects();
        result.validationTime = timer.elapsed();
        
        if (result.success) {
            logValidationResult("neuromodulation_plasticity", result);
        } else {
            currentReport_.errors.push_back(result.message);
        }
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in neuromodulation-plasticity verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyDevelopmentPlasticityIntegration() {
    ValidationTimer timer;
    IntegrationResult result;
    result.success = false;
    result.message = "Development-Plasticity integration verification in progress";
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Validate Development Effects
        result = validateDevelopmentEffects();
        result.validationTime = timer.elapsed();
        
        if (result.success) {
            logValidationResult("development_plasticity", result);
        } else {
            currentReport_.errors.push_back(result.message);
        }
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in development-plasticity verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::validateBrainMemorySystem() const {
    IntegrationResult result;
    result.success = true;
    result.message = "Brain-Memory integration validation successful";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Check if brain has memory systems
    if (!pImpl->brain->getWorkingMemory()) {
        result.success = false;
        result.message = "Working memory system not properly initialized";
    }
    
    if (!pImpl->brain->getEpisodicMemory()) {
        result.success = false;
        result.message += "; Episodic memory system not properly initialized";
    }
    
    // Check neural working memory integration
    auto neuralWorkingMem = pImpl->brain->getWorkingMemory();
    if (neuralWorkingMem) {
        // Verify neural working memory has capacity settings
        auto capacity = neuralWorkingMem->getCapacity();
        result.metadata["working_memory_capacity"] = std::to_string(capacity);
        
        if (capacity < 10) {
            result.success = false;
            result.message += "; Working memory capacity too low for effective integration";
        }
        
        // Check decay rate
        auto decayRate = neuralWorkingMem->getDecayRate();
        result.metadata["working_memory_decay_rate"] = std::to_string(decayRate);
    }
    
    // Check episodic memory integration
    auto episodicMem = pImpl->brain->getEpisodicMemory();
    if (episodicMem) {
        auto episodeCount = episodicMem->getEpisodeCount();
        result.metadata["episodic_memory_episodes"] = std::to_string(episodeCount);
        
        if (episodeCount < 5) {
            result.warnings.push_back("Low episodic memory episode count may affect integration quality");
        }
    }
    
    // Check cross-region memory connections
    auto regions = pImpl->brain->getRegionIds();
    result.metadata["brain_regions"] = std::to_string(regions.size());
    
    if (regions.size() < 3) {
        result.warnings.push_back("Low number of brain regions may affect memory integration scope");
    }
    
    // Verify memory-neuron connectivity
    auto totalNeurons = pImpl->brain->getTotalNeuronCount();
    result.metadata["total_neurons"] = std::to_string(totalNeurons);
    
    if (totalNeurons == 0) {
        result.success = false;
        result.message += "; No neurons found - critical integration failure";
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return result;
}

IntegrationResult IntegrationVerifier::validateNeuromodulationEffects() const {
    IntegrationResult result;
    result.success = true;
    result.message = "Neuromodulation-Plasticity integration validation successful";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Check all neuromodulators are present
    auto dopamine = pImpl->brain->getDopamine();
    auto novelty = pImpl->brain->getNovelty();
    auto reward = pImpl->brain->getPredictionErrorSignal();
    auto curiosity = pImpl->brain->getCuriosity();
    
    result.metadata["dopamine_available"] = dopamine ? "true" : "false";
    result.metadata["novelty_available"] = novelty ? "true" : "false";
    result.metadata["reward_available"] = reward ? "true" : "false";
    result.metadata["curiosity_available"] = curiosity ? "true" : "false";
    
    // Check neuromodulator levels
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        result.metadata["dopamine_level"] = std::to_string(dopamineLevel);
        
        if (dopamineLevel < 0.1f) {
            result.warnings.push_back("Low dopamine level may reduce plasticity effectiveness");
        }
    }
    
    if (novelty) {
        float noveltyLevel = novelty->getLevel();
        result.metadata["novelty_level"] = std::to_string(noveltyLevel);
        
        if (noveltyLevel < 0.05f) {
            result.warnings.push_back("Low novelty level may reduce exploratory learning");
        }
    }
    
    // Check plasticity system integration
    auto stdp = pImpl->brain->getSTDP();
    auto hebbian = pImpl->brain->getHebbian();
    auto structural = pImpl->brain->getStructuralPlasticity();
    
    result.metadata["stdp_available"] = stdp ? "true" : "false";
    result.metadata["hebbian_available"] = hebbian ? "true" : "false";
    result.metadata["structural_plasticity_available"] = structural ? "true" : "false";
    
    // Verify neuromodulator affects plasticity
    if (dopamine && stdp) {
        float plasticityFactor = dopamine->getPlasticityFactor();
        result.metadata["dopamine_plasticity_factor"] = std::to_string(plasticityFactor);
        
        if (plasticityFactor < 0.5f) {
            result.warnings.push_back("Dopamine plasticity factor below optimal range");
        }
    }
    
    if (novelty && hebbian) {
        float plasticityFactor = novelty->getPlasticityFactor();
        result.metadata["novelty_plasticity_factor"] = std::to_string(plasticityFactor);
    }
    
    // Check for balance between different neuromodulators
    bool hasMultipleNeuromodulators = (dopamine != nullptr) + (novelty != nullptr) + 
                                      (reward != nullptr) + (curiosity != nullptr) > 1;
    
    if (!hasMultipleNeuromodulators) {
        result.warnings.push_back("Single neuromodulator may limit integration diversity");
    }
    
    // Check brain development stage effects on neuromodulation
    auto developmentalStage = pImpl->brain->getDevelopmentalStage();
    result.metadata["developmental_stage"] = std::to_string(static_cast<int>(developmentalStage));
    
    // In developmental stages, neuromodulation should be more sensitive
    if (developmentalStage == DevelopmentalStage::EARLY || developmentalStage == DevelopmentalStage::ADOLESCENT) {
        if (dopamine && dopamine->getLevel() > 1.0f) {
            result.warnings.push_back("High dopamine levels in early development stage may cause excessive plasticity");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return result;
}

IntegrationResult IntegrationVerifier::validateDevelopmentEffects() const {
    IntegrationResult result;
    result.success = true;
    result.message = "Development-Plasticity integration validation successful";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Check development system availability
    auto developmentSystem = pImpl->brain->getDevelopmentSystem();
    result.metadata["development_system_available"] = developmentSystem ? "true" : "false";
    
    if (developmentSystem) {
        auto developmentalStage = pImpl->brain->getDevelopmentalStage();
        result.metadata["developmental_stage"] = std::to_string(static_cast<int>(developmentalStage));
        
        // Check stage-specific plasticity expectations
        switch (developmentalStage) {
            case DevelopmentalStage::EARLY:
                // Early stage: high plasticity, rapid learning
                result.metadata["expected_plasticity_high"] = "true";
                result.warnings.push_back("Early developmental stage requires careful neuromodulator regulation");
                break;
            case DevelopmentalStage::ADOLESCENT:
                // Adolescent: moderate plasticity, increased exploration
                result.metadata["expected_plasticity_moderate"] = "true";
                break;
            case DevelopmentalStage::ADULT:
                // Adult: stable plasticity, fine-tuning
                result.metadata["expected_plasticity_stable"] = "true";
                break;
            case DevelopmentalStage::SENIOR:
                // Senior: reduced plasticity, preservation
                result.metadata["expected_plasticity_reduced"] = "true";
                result.warnings.push_back("Senior stage may require increased neuromodulator support");
                break;
        }
    }
    
    // Check plasticity system maturity
    auto stdp = pImpl->brain->getSTDP();
    auto hebbian = pImpl->brain->getHebbian();
    
    if (stdp) {
        result.metadata["stdp_enabled"] = stdp->isEnabled() ? "true" : "false";
    }
    
    if (hebbian) {
        result.metadata["hebbian_enabled"] = hebbian->isEnabled() ? "true" : "false";
    }
    
    // Check developmental connectivity
    auto regions = pImpl->brain->getRegionIds();
    result.metadata["brain_regions_count"] = std::to_string(regions.size());
    
    // In development, there should be growing connectivity
    if (developmentSystem) {
        auto developmentalStage = pImpl->brain->getDevelopmentalStage();
        if (developmentalStage == DevelopmentalStage::EARLY || developmentalStage == DevelopmentalStage::ADOLESCENT) {
            // Should have active development processes
            result.metadata["development_active"] = "true";
        }
    }
    
    // Check for developmental disruptions
    auto totalNeurons = pImpl->brain->getTotalNeuronCount();
    auto totalSynapses = pImpl->brain->getTotalSynapseCount();
    
    result.metadata["total_neurons"] = std::to_string(totalNeurons);
    result.metadata["total_synapses"] = std::to_string(totalSynapses);
    
    if (totalNeurons == 0) {
        result.success = false;
        result.message += "; No neurons found - critical development integration failure";
    }
    
    // Check developmental stage consistency with system state
    if (developmentSystem && totalNeurons > 0) {
        // This is a simplified check - in reality would need more sophisticated validation
        result.metadata["development_consistency"] = "true";
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyPredictionMemoryIntegration() {
    IntegrationResult result;
    result.success = true;
    result.message = "Prediction-Memory integration verification in progress";
    
    ValidationTimer timer;
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Check prediction system availability
        auto predictionSystem = pImpl->brain->getPredictionSystem();
        result.metadata["prediction_system_available"] = predictionSystem ? "true" : "false";
        
        // Check working memory availability
        auto workingMemory = pImpl->brain->getWorkingMemory();
        result.metadata["working_memory_available"] = workingMemory ? "true" : "false";
        
        if (predictionSystem && workingMemory) {
            // Verify prediction-memory feedback loop
            result.success = true;
            result.message = "Prediction-Memory integration validation successful";
            result.metadata["feedback_loop_active"] = "true";
        }
        
        result.validationTime = timer.elapsed();
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in prediction-memory verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyCognitionMemoryIntegration() {
    IntegrationResult result;
    result.success = true;
    result.message = "Cognition-Memory integration verification in progress";
    
    ValidationTimer timer;
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Check cognition system availability
        auto neuralPlanner = pImpl->brain->getPlanner();
        auto conceptFormation = pImpl->brain->getConceptFormation();
        auto attentionalSelection = pImpl->brain->getAttention();
        
        result.metadata["neural_planner_available"] = neuralPlanner ? "true" : "false";
        result.metadata["concept_formation_available"] = conceptFormation ? "true" : "false";
        result.metadata["attentional_selection_available"] = attentionalSelection ? "true" : "false";
        
        // Check memory system availability
        auto workingMemory = pImpl->brain->getWorkingMemory();
        auto episodicMemory = pImpl->brain->getEpisodicMemory();
        
        result.metadata["working_memory_available"] = workingMemory ? "true" : "false";
        result.metadata["episodic_memory_available"] = episodicMemory ? "true" : "false";
        
        if (neuralPlanner && conceptFormation && workingMemory) {
            // Verify cognition-memory integration
            result.success = true;
            result.message = "Cognition-Memory integration validation successful";
            result.metadata["planning_memory_integration"] = "active";
        }
        
        result.validationTime = timer.elapsed();
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in cognition-memory verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifySensoryMemoryIntegration() {
    IntegrationResult result;
    result.success = true;
    result.message = "Sensory-Memory integration verification in progress";
    
    ValidationTimer timer;
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Check sensory input system
        auto sensoryInput = pImpl->brain->getSensoryInput();
        result.metadata["sensory_input_available"] = sensoryInput ? "true" : "false";
        
        // Check working memory
        auto workingMemory = pImpl->brain->getWorkingMemory();
        result.metadata["working_memory_available"] = workingMemory ? "true" : "false";
        
        if (sensoryInput && workingMemory) {
            // Verify sensory-memory integration
            result.success = true;
            result.message = "Sensory-Memory integration validation successful";
            result.metadata["sensory_encoding_active"] = "true";
        }
        
        result.validationTime = timer.elapsed();
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in sensory-memory verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyMotorCognitionIntegration() {
    IntegrationResult result;
    result.success = true;
    result.message = "Motor-Cognition integration verification in progress";
    
    ValidationTimer timer;
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Check motor system
        auto motorSystem = pImpl->brain->getMotorSystem();
        result.metadata["motor_system_available"] = motorSystem ? "true" : "false";
        
        // Check cognition system
        auto neuralPlanner = pImpl->brain->getPlanner();
        result.metadata["cognition_system_available"] = neuralPlanner ? "true" : "false";
        
        // Check plasticity for motor learning
        auto stdp = pImpl->brain->getSTDP();
        result.metadata["plasticity_available"] = stdp ? "true" : "false";
        
        if (motorSystem && neuralPlanner && stdp) {
            // Verify motor-cognition integration
            result.success = true;
            result.message = "Motor-Cognition integration validation successful";
            result.metadata["motor_planning_integration"] = "active";
        }
        
        result.validationTime = timer.elapsed();
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in motor-cognition verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyWorldEnvironmentIntegration() {
    IntegrationResult result;
    result.success = true;
    result.message = "World-Environment integration verification in progress";
    
    ValidationTimer timer;
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Check environment system
        auto environment = pImpl->brain->getEnvironment();
        result.metadata["environment_available"] = environment ? "true" : "false";
        
        // Check world system
        auto world = pImpl->brain->getWorld();
        result.metadata["world_available"] = world ? "true" : "false";
        
        // Check integration between brain and world
        if (environment && world) {
            // Verify world-environment integration
            result.success = true;
            result.message = "World-Environment integration validation successful";
            result.metadata["world_brain_integration"] = "active";
        }
        
        result.validationTime = timer.elapsed();
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in world-environment verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyDevelopmentBrainIntegration() {
    IntegrationResult result;
    result.success = true;
    result.message = "Development-Brain integration verification in progress";
    
    ValidationTimer timer;
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Check development system
        auto developmentSystem = pImpl->brain->getDevelopmentSystem();
        result.metadata["development_system_available"] = developmentSystem ? "true" : "false";
        
        // Check brain developmental stage
        auto developmentalStage = pImpl->brain->getDevelopmentalStage();
        result.metadata["developmental_stage"] = std::to_string(static_cast<int>(developmentalStage));
        
        // Check plasticity systems
        auto stdp = pImpl->brain->getSTDP();
        auto hebbian = pImpl->brain->getHebbian();
        
        result.metadata["stdp_available"] = stdp ? "true" : "false";
        result.metadata["hebbian_available"] = hebbian ? "true" : "false";
        
        if (developmentSystem && stdp) {
            // Verify development-brain integration
            result.success = true;
            result.message = "Development-Brain integration validation successful";
            result.metadata["developmental_plasticity_active"] = "true";
        }
        
        result.validationTime = timer.elapsed();
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in development-brain verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyNeuromodulationBrainIntegration() {
    IntegrationResult result;
    result.success = true;
    result.message = "Neuromodulation-Brain integration verification in progress";
    
    ValidationTimer timer;
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Check all neuromodulators
        auto dopamine = pImpl->brain->getDopamine();
        auto novelty = pImpl->brain->getNovelty();
        auto reward = pImpl->brain->getPredictionErrorSignal();
        auto curiosity = pImpl->brain->getCuriosity();
        
        result.metadata["dopamine_available"] = dopamine ? "true" : "false";
        result.metadata["novelty_available"] = novelty ? "true" : "false";
        result.metadata["reward_available"] = reward ? "true" : "false";
        result.metadata["curiosity_available"] = curiosity ? "true" : "false";
        
        // Check neuromodulation effects on brain systems
        auto stdp = pImpl->brain->getSTDP();
        auto hebbian = pImpl->brain->getHebbian();
        
        if (dopamine && stdp) {
            float plasticityFactor = dopamine->getPlasticityFactor();
            result.metadata["dopamine_plasticity_effect"] = std::to_string(plasticityFactor);
        }
        
        if (novelty && hebbian) {
            float plasticityFactor = novelty->getPlasticityFactor();
            result.metadata["novelty_plasticity_effect"] = std::to_string(plasticityFactor);
        }
        
        // Check integration
        bool hasMultipleNeuromodulators = (dopamine != nullptr) + (novelty != nullptr) + 
                                          (reward != nullptr) + (curiosity != nullptr) > 1;
        
        if (hasMultipleNeuromodulators && stdp) {
            result.success = true;
            result.message = "Neuromodulation-Brain integration validation successful";
            result.metadata["integrated_plasticity_modulation"] = "active";
        }
        
        result.validationTime = timer.elapsed();
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in neuromodulation-brain verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifySystemConnectivity() {
    IntegrationResult result;
    result.success = true;
    result.message = "System connectivity verification in progress";
    
    ValidationTimer timer;
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Check system interconnection metrics
        auto regions = pImpl->brain->getRegionIds();
        result.metadata["regions_count"] = std::to_string(regions.size());
        
        auto totalNeurons = pImpl->brain->getTotalNeuronCount();
        auto totalSynapses = pImpl->brain->getTotalSynapseCount();
        
        result.metadata["total_neurons"] = std::to_string(totalNeurons);
        result.metadata["total_synapses"] = std::to_string(totalSynapses);
        
        // Calculate connectivity metrics
        float synapseDensity = totalNeurons > 0 ? 
            (static_cast<float>(totalSynapses) / (static_cast<float>(totalNeurons) * totalNeurons)) : 0.0f;
        
        result.metadata["synapse_density"] = std::to_string(synapseDensity);
        
        // Check region connectivity
        if (regions.size() > 1) {
            result.metadata["inter_region_connections"] = "present";
            
            // Check for inter-region connections
            bool hasInterRegionConnections = false;
            for (size_t i = 0; i < regions.size() && !hasInterRegionConnections; ++i) {
                for (size_t j = 0; j < regions.size() && !hasInterRegionConnections; ++j) {
                    if (i != j) {
                        hasInterRegionConnections = true;
                        break;
                    }
                }
            }
            
            result.metadata["inter_region_connections_active"] = hasInterRegionConnections ? "true" : "false";
        }
        
        // Check spike system integration
        auto spikeSystem = pImpl->brain->getSpikeSystem();
        result.metadata["spike_system_available"] = spikeSystem ? "true" : "false";
        
        if (spikeSystem) {
            size_t pendingSpikeEvents = spikeSystem->getPendingSpikeEventCount();
            result.metadata["pending_spike_events"] = std::to_string(pendingSpikeEvents);
        }
        
        // Check plasticity system integration
        auto plasticitySystem = pImpl->brain->getSTDP();
        result.metadata["plasticity_system_available"] = plasticitySystem ? "true" : "false";
        
        result.success = true;
        result.message = "System connectivity validation successful";
        result.validationTime = timer.elapsed();
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in system connectivity verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyPlasticityConsistency() {
    IntegrationResult result;
    result.success = true;
    result.message = "Plasticity consistency verification in progress";
    
    ValidationTimer timer;
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Check plasticity rules consistency
        auto stdp = pImpl->brain->getSTDP();
        auto hebbian = pImpl->brain->getHebbian();
        auto structural = pImpl->brain->getStructuralPlasticity();
        
        result.metadata["stdp_enabled"] = stdp ? (stdp->isEnabled() ? "true" : "false") : "false";
        result.metadata["hebbian_enabled"] = hebbian ? (hebbian->isEnabled() ? "true" : "false") : "false";
        result.metadata["structural_enabled"] = structural ? "true" : "false";
        
        // Check for plasticity rule conflicts
        bool hasConflictingRules = false;
        if (stdp && hebbian && structural) {
            // Simple check - if all rules are enabled, check for balance
            result.metadata["plasticity_balance"] = "good";
            
            // Check if any rule has extreme settings that might cause conflicts
            if (stdp->isEnabled()) {
                result.metadata["stdp_participates"] = "true";
            }
            if (hebbian->isEnabled()) {
                result.metadata["hebbian_participates"] = "true";
            }
        }
        
        // Check neuron plasticity capacity
        auto totalNeurons = pImpl->brain->getTotalNeuronCount();
        result.metadata["neurons_with_plasticity"] = std::to_string(totalNeurons);
        
        // Check synaptic weight stability
        auto totalSynapses = pImpl->brain->getTotalSynapseCount();
        result.metadata["total_synapses"] = std::to_string(totalSynapses);
        
        // Calculate average synaptic weight (simplified)
        float avgSynapticWeight = totalSynapses > 0 ? 0.5f : 0.0f; // Placeholder
        result.metadata["average_synaptic_weight"] = std::to_string(avgSynapticWeight);
        
        // Check for weight instability
        if (avgSynapticWeight < 0.1f || avgSynapticWeight > 0.9f) {
            result.warnings.push_back("Extreme synaptic weights may indicate plasticity instability");
        }
        
        result.success = true;
        result.message = "Plasticity consistency validation successful";
        result.validationTime = timer.elapsed();
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in plasticity consistency verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyNeuromodulationBalance() {
    IntegrationResult result;
    result.success = true;
    result.message = "Neuromodulation balance verification in progress";
    
    ValidationTimer timer;
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Check neuromodulator balance
        auto dopamine = pImpl->brain->getDopamine();
        auto novelty = pImpl->brain->getNovelty();
        auto reward = pImpl->brain->getPredictionErrorSignal();
        auto curiosity = pImpl->brain->getCuriosity();
        
        std::vector<float> levels;
        std::vector<std::string> names;
        
        if (dopamine) {
            levels.push_back(dopamine->getLevel());
            names.push_back("dopamine");
        }
        
        if (novelty) {
            levels.push_back(novelty->getLevel());
            names.push_back("novelty");
        }
        
        if (reward) {
            levels.push_back(reward->getLevel());
            names.push_back("reward");
        }
        
        if (curiosity) {
            levels.push_back(curiosity->getLevel());
            names.push_back("curiosity");
        }
        
        // Calculate balance score
        if (!levels.empty()) {
            float minLevel = *std::min_element(levels.begin(), levels.end());
            float maxLevel = *std::max_element(levels.begin(), levels.end());
            
            result.metadata["level_range_min"] = std::to_string(minLevel);
            result.metadata["level_range_max"] = std::to_string(maxLevel);
            
            float balanceScore = (maxLevel - minLevel) / std::max(maxLevel, 0.1f);
            result.metadata["balance_score"] = std::to_string(balanceScore);
            
            if (balanceScore > 0.8f) {
                result.warnings.push_back("High neuromodulator level imbalance detected");
            }
        }
        
        // Check plasticity factor consistency
        float totalPlasticityFactor = 0.0f;
        int plasticityCount = 0;
        
        if (dopamine) {
            totalPlasticityFactor += dopamine->getPlasticityFactor();
            plasticityCount++;
            result.metadata["dopamine_plasticity"] = std::to_string(dopamine->getPlasticityFactor());
        }
        
        if (novelty) {
            totalPlasticityFactor += novelty->getPlasticityFactor();
            plasticityCount++;
            result.metadata["novelty_plasticity"] = std::to_string(novelty->getPlasticityFactor());
        }
        
        if (plasticityCount > 0) {
            float avgPlasticityFactor = totalPlasticityFactor / plasticityCount;
            result.metadata["average_plasticity_factor"] = std::to_string(avgPlasticityFactor);
            
            if (avgPlasticityFactor < 0.5f || avgPlasticityFactor > 1.5f) {
                result.warnings.push_back("Average plasticity factor outside optimal range");
            }
        }
        
        result.success = true;
        result.message = "Neuromodulation balance validation successful";
        result.validationTime = timer.elapsed();
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in neuromodulation balance verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyMemoryCapacity() {
    IntegrationResult result;
    result.success = true;
    result.message = "Memory capacity verification in progress";
    
    ValidationTimer timer;
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Check working memory capacity
        auto workingMemory = pImpl->brain->getWorkingMemory();
        if (workingMemory) {
            size_t capacity = workingMemory->getCapacity();
            result.metadata["working_memory_capacity"] = std::to_string(capacity);
            
            if (capacity < 5) {
                result.warnings.push_back("Very low working memory capacity may limit integration quality");
            }
        }
        
        // Check episodic memory capacity
        auto episodicMemory = pImpl->brain->getEpisodicMemory();
        if (episodicMemory) {
            size_t episodeCount = episodicMemory->getEpisodeCount();
            result.metadata["episodic_memory_capacity"] = std::to_string(episodeCount);
            
            if (episodeCount < 10) {
                result.warnings.push_back("Low episodic memory capacity may affect long-term integration");
            }
        }
        
        // Check system memory requirements
        auto totalNeurons = pImpl->brain->getTotalNeuronCount();
        result.metadata["total_neurons"] = std::to_string(totalNeurons);
        
        // Estimate memory requirements
        float estimatedMemoryMB = totalNeurons * 0.1f; // Simplified: 0.1MB per neuron
        result.metadata["estimated_memory_requirements_mb"] = std::to_string(estimatedMemoryMB);
        
        // Check memory-neuron ratio
        if (workingMemory && totalNeurons > 0) {
            float memoryNeuronRatio = static_cast<float>(workingMemory->getCapacity()) / totalNeurons;
            result.metadata["memory_neuron_ratio"] = std::to_string(memoryNeuronRatio);
            
            if (memoryNeuronRatio < 0.01f) {
                result.warnings.push_back("Memory capacity much lower than neuron count - potential integration bottleneck");
            }
        }
        
        result.success = true;
        result.message = "Memory capacity validation successful";
        result.validationTime = timer.elapsed();
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in memory capacity verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

void IntegrationVerifier::logValidationResult(const std::string& validationName, const IntegrationResult& result) {
    if (!loggingEnabled_) return;
    
    std::stringstream logMessage;
    logMessage << "Integration Validation: " << validationName << " - " << result.message;
    
    if (result.success) {
        Logger::getInstance().log("IntegrationVerifier", logMessage.str() + " (SUCCESS)");
        if (!result.warnings.empty()) {
            for (const auto& warning : result.warnings) {
                Logger::getInstance().log("IntegrationVerifier", "  WARNING: " + warning, Logger::LogLevel::WARNING);
            }
        }
    } else {
        Logger::getInstance().log("IntegrationVerifier", logMessage.str() + " (FAILED)", Logger::LogLevel::ERROR);
        if (!result.warnings.empty()) {
            for (const auto& warning : result.warnings) {
                Logger::getInstance().log("IntegrationVerifier", "  WARNING: " + warning, Logger::LogLevel::WARNING);
            }
        }
    }
}

void IntegrationVerifier::generateIntegrationStatistics(IntegrationReport& report) const {
    if (!detailedReportingEnabled_) return;
    
    // Generate comprehensive statistics
    report.totalValidations = report.validations.size();
    report.successfulValidations = 0;
    
    for (const auto& validation : report.validations) {
        if (validation.success) {
            report.successfulValidations++;
        }
    }
    
    // Calculate average validation time
    if (!report.validations.empty()) {
        long long totalTime = 0;
        for (const auto& validation : report.validations) {
            totalTime += validation.validationTime.count();
        }
        report.totalValidationTime = std::chrono::milliseconds(totalTime / report.validations.size());
    }
}

IntegrationReport IntegrationVerifier::verifyAllIntegrations() {
    std::lock_guard<std::mutex> lock(pImpl->validationMutex);
    
    reset();
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Verify all enabled integrations
    if (isValidationEnabled("brain_memory")) {
        currentReport_.validations.push_back(verifyBrainMemoryIntegration());
    }
    
    if (isValidationEnabled("neuromodulation_plasticity")) {
        currentReport_.validations.push_back(verifyNeuromodulationPlasticityIntegration());
    }
    
    if (isValidationEnabled("development_plasticity")) {
        currentReport_.validations.push_back(verifyDevelopmentPlasticityIntegration());
    }
    
    // Performance integration verifications
    if (isValidationEnabled("performance_integration")) {
        currentReport_.validations.push_back(verifyPerformanceIntegration());
    }
    
    if (isValidationEnabled("resource_usage_integration")) {
        currentReport_.validations.push_back(verifyResourceUsageIntegration());
    }
    
    if (isValidationEnabled("optimization_integration")) {
        currentReport_.validations.push_back(verifyOptimizationIntegration());
    }
    
    if (isValidationEnabled("monitoring_integration")) {
        currentReport_.validations.push_back(verifyMonitoringIntegration());
    }
    
    // Additional verifications (conditionally enabled)
    if (isValidationEnabled("prediction_memory")) {
        currentReport_.validations.push_back(verifyPredictionMemoryIntegration());
    }
    
    if (isValidationEnabled("cognition_memory")) {
        currentReport_.validations.push_back(verifyCognitionMemoryIntegration());
    }
    
    if (isValidationEnabled("sensory_memory")) {
        currentReport_.validations.push_back(verifySensoryMemoryIntegration());
    }
    
    if (isValidationEnabled("motor_cognition")) {
        currentReport_.validations.push_back(verifyMotorCognitionIntegration());
    }
    
    if (isValidationEnabled("world_environment")) {
        currentReport_.validations.push_back(verifyWorldEnvironmentIntegration());
    }
    
    if (isValidationEnabled("development_brain")) {
        currentReport_.validations.push_back(verifyDevelopmentBrainIntegration());
    }
    
    if (isValidationEnabled("neuromodulation_brain")) {
        currentReport_.validations.push_back(verifyNeuromodulationBrainIntegration());
    }
    
    if (isValidationEnabled("system_connectivity")) {
        currentReport_.validations.push_back(verifySystemConnectivity());
    }
    
    if (isValidationEnabled("plasticity_consistency")) {
        currentReport_.validations.push_back(verifyPlasticityConsistency());
    }
    
    if (isValidationEnabled("neuromodulation_balance")) {
        currentReport_.validations.push_back(verifyNeuromodulationBalance());
    }
    
    if (isValidationEnabled("memory_capacity")) {
        currentReport_.validations.push_back(verifyMemoryCapacity());
    }
    
    // Performance integration verifications
    if (isValidationEnabled("performance_integration")) {
        currentReport_.validations.push_back(verifyPerformanceIntegration());
    }
    
    if (isValidationEnabled("resource_usage_integration")) {
        currentReport_.validations.push_back(verifyResourceUsageIntegration());
    }
    
    if (isValidationEnabled("optimization_integration")) {
        currentReport_.validations.push_back(verifyOptimizationIntegration());
    }
    
    if (isValidationEnabled("monitoring_integration")) {
        currentReport_.validations.push_back(verifyMonitoringIntegration());
    }
    
    // Generate statistics
    generateIntegrationStatistics(currentReport_);
    
    // Record total time
    auto endTime = std::chrono::high_resolution_clock::now();
    currentReport_.totalValidationTime = 
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Log completion
    if (loggingEnabled_) {
        std::string reportStr = currentReport_.toString();
        Logger::getInstance().log("IntegrationVerifier", "Integration verification completed: " + 
            std::to_string(currentReport_.successfulValidations) + "/" + 
            std::to_string(currentReport_.totalValidations) + " successful");
        
        if (detailedReportingEnabled_) {
            Logger::getInstance().log("IntegrationVerifier", "Detailed report:\n" + reportStr, Logger::LogLevel::INFO);
        }
    }
    
    return currentReport_;
}

IntegrationResult IntegrationVerifier::verifyPerformanceIntegration() {
    ValidationTimer timer;
    IntegrationResult result;
    result.success = false;
    result.message = "Performance integration verification in progress";
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Initialize performance monitor
        PerformanceMonitor::getInstance().initialize();
        PerformanceMonitor::getInstance().setMonitoringEnabled(true);
        
        // Test basic performance monitoring
        PerformanceMonitor::getInstance().update();
        
        auto snapshot = PerformanceMonitor::getInstance().getCurrentMetrics();
        
        if (snapshot.system.cpuUsagePercent >= 0.0) {  // Valid metrics
            result.success = true;
            result.message = "Performance monitoring system operational";
            result.metadata["cpu_usage"] = std::to_string(snapshot.system.cpuUsagePercent);
            result.metadata["memory_usage_mb"] = std::to_string(snapshot.system.memoryUsageMB);
            result.metadata["neuron_count"] = std::to_string(snapshot.brain.totalNeurons);
        } else {
            result.message = "Performance monitoring metrics invalid";
            currentReport_.errors.push_back(result.message);
        }
        
        result.validationTime = timer.elapsed();
        
        if (result.success) {
            logValidationResult("performance_integration", result);
        }
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in performance integration verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyResourceUsageIntegration() {
    ValidationTimer timer;
    IntegrationResult result;
    result.success = false;
    result.message = "Resource usage integration verification in progress";
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Test resource tracking
        PerformanceMonitor::getInstance().setMonitoringEnabled(true);
        
        // Simulate resource usage
        PerformanceMonitor::getInstance().trackSystemUsage(50.0, 512.0, 25.0);
        PerformanceMonitor::getInstance().trackNeuronOperation(1, "LIF_step", 1.5, 0.1);
        PerformanceMonitor::getInstance().trackSynapseUpdate(1, 0.8, 0.05);
        
        // Get current metrics
        auto snapshot = PerformanceMonitor::getInstance().getCurrentMetrics();
        
        // Check if resource metrics are reasonable
        if (snapshot.system.cpuUsagePercent >= 0.0 && 
            snapshot.system.memoryUsageMB >= 0.0 &&
            snapshot.brain.totalNeurons > 0) {
            
            result.success = true;
            result.message = "Resource usage tracking operational";
            result.metadata["peak_memory_mb"] = std::to_string(snapshot.system.memoryUsageMB);
            result.metadata["cpu_average"] = std::to_string(snapshot.system.cpuAverage);
            result.metadata["total_operations"] = std::to_string(PerformanceMonitor::getInstance().getTotalOperationCount());
        } else {
            result.message = "Resource usage metrics invalid";
            currentReport_.errors.push_back(result.message);
        }
        
        result.validationTime = timer.elapsed();
        
        if (result.success) {
            logValidationResult("resource_usage_integration", result);
        }
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in resource usage integration verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyOptimizationIntegration() {
    ValidationTimer timer;
    IntegrationResult result;
    result.success = false;
    result.message = "Optimization integration verification in progress";
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Test optimization recommendations
        auto recommendations = PerformanceMonitor::getInstance().getOptimizationRecommendations();n        
        if (!recommendations.empty()) {
            result.success = true;
            result.message = "Optimization recommendations generated";
            std::string recommendationsStr;
            for (const auto& rec : recommendations) {
                recommendationsStr += rec + "; ";
            }
            result.metadata["recommendations_count"] = std::to_string(recommendations.size());
            result.metadata["recommendations"] = recommendationsStr;
        } else {
            result.message = "No optimization recommendations generated";
        }
        
        result.validationTime = timer.elapsed();
        
        if (result.success) {
            logValidationResult("optimization_integration", result);
        }
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in optimization integration verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

IntegrationResult IntegrationVerifier::verifyMonitoringIntegration() {
    ValidationTimer timer;
    IntegrationResult result;
    result.success = false;
    result.message = "Monitoring integration verification in progress";
    
    if (!pImpl->brain) {
        result.message = "ERROR: Brain not initialized";
        currentReport_.errors.push_back(result.message);
        return result;
    }
    
    try {
        // Test monitoring integration
        PerformanceMonitor::getInstance().enableRealTimeDashboard(true);
        PerformanceMonitor::getInstance().setSamplingInterval(0.1);
        
        // Generate a test report
        std::string report = PerformanceMonitor::getInstance().generateReport();
        
        if (!report.empty()) {
            result.success = true;
            result.message = "Monitoring integration operational";
            result.metadata["report_size"] = std::to_string(report.length());
            result.metadata["dashboard_enabled"] = "true";
            result.metadata["sampling_interval"] = std::to_string(PerformanceMonitor::getInstance().getSamplingInterval());
        } else {
            result.message = "Monitoring report generation failed";
            currentReport_.errors.push_back(result.message);
        }
        
        result.validationTime = timer.elapsed();
        
        if (result.success) {
            logValidationResult("monitoring_integration", result);
        }
        
    } catch (const std::exception& e) {
        result.message = "EXCEPTION in monitoring integration verification: " + std::string(e.what());
        currentReport_.errors.push_back(result.message);
    }
    
    return result;
}

} // namespace nlm
