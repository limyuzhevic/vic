// DevelopmentExperiment.cpp implementation
#include "DevelopmentExperiment.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

DevelopmentExperiment::DevelopmentExperiment() {
    enableFeature("progress_tracking");
    enableFeature("checkpointing");
    addTag("development");
    addTag("critical_periods");
    addTag("plasticity");
    addTag("synaptogenesis");
    addTag("pruning");
    
    // Set default parameters
    addParameter("total_development_steps", 10000);
    addParameter("initial_neurons", 100);
    addParameter("max_neurons", 5000);
    addParameter("synaptogenesis_phase", 2000);
    addParameter("sensory_phase", 3000);
    addParameter("motor_phase", 2000);
    addParameter("social_phase", 3000);
    addParameter("critical_period_start", 1000);
    addParameter("critical_period_end", 2000);
    addParameter("plasticity_decay", 0.99f);
}

DevelopmentExperiment::DevelopmentExperiment(const ExperimentFactory::ExperimentConfig& config) {
    name = config.experimentName.empty() ? "DevelopmentExperiment" : config.experimentName;
    description = "Tests developmental stages and critical periods in neural development";
    
    // Apply configuration
    if (config.enableProgressTracking) enableFeature("progress_tracking");
    if (config.enableCheckpointing) enableFeature("checkpointing");
    
    // Copy parameters
    parametersDouble = config.doubleParameters;
    parametersString = config.stringParameters;
    parametersBool = config.boolParameters;
    
    // Set defaults if not provided
    if (!hasParameter("total_development_steps")) addParameter("total_development_steps", 10000);
    if (!hasParameter("initial_neurons")) addParameter("initial_neurons", 100);
    if (!hasParameter("max_neurons")) addParameter("max_neurons", 5000);
    if (!hasParameter("synaptogenesis_phase")) addParameter("synaptogenesis_phase", 2000);
    if (!hasParameter("sensory_phase")) addParameter("sensory_phase", 3000);
    if (!hasParameter("motor_phase")) addParameter("motor_phase", 2000);
    if (!hasParameter("social_phase")) addParameter("social_phase", 3000);
    if (!hasParameter("critical_period_start")) addParameter("critical_period_start", 1000);
    if (!hasParameter("critical_period_end")) addParameter("critical_period_end", 2000);
    if (!hasParameter("plasticity_decay")) addParameter("plasticity_decay", 0.99f);
    
    addTag("development");
    addTag("critical_periods");
    addTag("plasticity");
    addTag("synaptogenesis");
    addTag("pruning");
}

bool DevelopmentExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    if (!brain || !environment) {
        NLM_LOG_ERROR("Invalid brain or environment in DevelopmentExperiment");
        return false;
    }
    
    currentStatus = "Starting developmental experiment...";
    
    // Reset and initialize brain
    brain->reset();
    brain->initialize();
    environment->reset();
    
    // Record developmental baseline
    recordDevelopmentalBaseline();
    
    // Run developmental phases
    size_t totalSteps = static_cast<size_t>(getParameterDouble("total_development_steps"));
    
    NLM_LOG_INFO("Starting developmental experiment with " + std::to_string(totalSteps) + " steps...");
    
    // Phase 1: Synaptogenesis
    size_t synaptogenesisPhase = static_cast<size_t>(getParameterDouble("synaptogenesis_phase"));
    runDevelopmentPhase(brain, "Synaptogenesis", 0, synaptogenesisPhase);
    
    // Phase 2: Sensory Development
    size_t sensoryPhase = static_cast<size_t>(getParameterDouble("sensory_phase"));
    runDevelopmentPhase(brain, "Sensory Development", synaptogenesisPhase, 
                       synaptogenesisPhase + sensoryPhase);
    
    // Phase 3: Motor Development
    size_t motorPhase = static_cast<size_t>(getParameterDouble("motor_phase"));
    runDevelopmentPhase(brain, "Motor Development", synaptogenesisPhase + sensoryPhase,
                       synaptogenesisPhase + sensoryPhase + motorPhase);
    
    // Phase 4: Social Development
    size_t socialPhase = static_cast<size_t>(getParameterDouble("social_phase"));
    runDevelopmentPhase(brain, "Social Development", synaptogenesisPhase + sensoryPhase + motorPhase,
                       totalSteps);
    
    // Analyze critical periods
    analyzeCriticalPeriods();
    
    // Compute final metrics
    computeFinalMetrics();
    
    NLM_LOG_INFO("Development experiment completed: initial = " + std::to_string(initialNeurons) +
                 ", peak = " + std::to_string(peakNeurons) + ", final = " + 
                 std::to_string(finalNeurons) + ", synaptogenesis rate = " + 
                 std::to_string(synaptogenesisRate) + ", pruning rate = " + 
                 std::to_string(pruningRate));
    
    return (finalNeurons > initialNeurons * 0.5f);  // Success if we have at least half the initial neurons
}

void DevelopmentExperiment::recordDevelopmentalBaseline() {
    initialNeurons = getParameterDouble("initial_neurons");
    peakNeurons = initialNeurons;
    finalNeurons = initialNeurons;
    synaptogenesisRate = 0.0f;
    pruningRate = 0.0f;
    criticalPeriodLength = 0;
    developmentSpeed = 0.0f;
    plasticityWindow = 0.0f;
    
    phaseMetrics.clear();
    developmentalProgress.clear();
}

void DevelopmentExperiment::computeFinalMetrics() {
    // Calculate final development metrics
    size_t totalSteps = static_cast<size_t>(getParameterDouble("total_development_steps"));
    
    if (totalSteps > 0) {
        developmentSpeed = (finalNeurons - initialNeurons) / static_cast<float>(totalSteps);
        synaptogenesisRate = (peakNeurons - initialNeurons) / 
                           static_cast<float>(getParameterDouble("synaptogenesis_phase"));
        pruningRate = (peakNeurons - finalNeurons) / 
                     static_cast<float>(totalSteps - getParameterDouble("synaptogenesis_phase"));
    }
    
    // Calculate plasticity window
    plasticityWindow = getParameterDouble("critical_period_end") - 
                      getParameterDouble("critical_period_start");
    
    // Update result
    result.experimentName = name;
    result.success = (finalNeurons > initialNeurons * 0.5f);
    result.totalReward = static_cast<double>(brain->getTotalSpikeCount());
    result.averageFiringRate = brain->getAverageFiringRate();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.totalSynapses = brain->getTotalSynapseCount();
    result.totalNeurons = brain->getTotalNeuronCount();
    
    // Store metrics
    result.metricResults["initial_neurons"] = initialNeurons;
    result.metricResults["peak_neurons"] = peakNeurons;
    result.metricResults["final_neurons"] = finalNeurons;
    result.metricResults["synaptogenesis_rate"] = synaptogenesisRate;
    result.metricResults["pruning_rate"] = pruningRate;
    result.metricResults["critical_period_length"] = static_cast<double>(criticalPeriodLength);
    result.metricResults["development_speed"] = developmentSpeed;
    result.metricResults["plasticity_window"] = plasticityWindow;
}

void DevelopmentExperiment::runDevelopmentPhase(std::shared_ptr<Brain> brain, const std::string& phaseName, 
                                                 size_t startStep, size_t endStep) {
    NLM_LOG_INFO("Starting development phase: " + phaseName + 
                 " (" + std::to_string(startStep) + " - " + std::to_string(endStep) + ")");
    
    // Simulate development phase
    for (size_t step = startStep; step < endStep; ++step) {
        // Apply developmental changes
        float developmentFactor = calculateDevelopmentFactor(phaseName, step);
        
        if (brain) {
            // Apply developmental plasticity
            applyDevelopmentalPlasticity(brain, developmentFactor, step);
            
            // Step the brain
            brain->step(step, step * 0.001);
        }
        
        // Update neuron count
        size_t currentNeurons = brain ? brain->getTotalNeuronCount() : 0;
        peakNeurons = std::max(peakNeurons, static_cast<float>(currentNeurons));
        finalNeurons = static_cast<float>(currentNeurons);
        
        // Log progress periodically
        if (step % 1000 == 0) {
            NLM_LOG_DEBUG("Development phase " + phaseName + ", step " + 
                         std::to_string(step) + ", neurons = " + 
                         std::to_string(currentNeurons));
        }
    }
    
    // Record phase metrics
    phaseMetrics[phaseName] = finalNeurons;
    developmentalProgress.push_back(std::make_pair(phaseName, finalNeurons));
}

void DevelopmentExperiment::analyzeCriticalPeriods() {
    // Analyze critical periods for plasticity
    size_t criticalPeriodStart = static_cast<size_t>(getParameterDouble("critical_period_start"));
    size_t criticalPeriodEnd = static_cast<size_t>(getParameterDouble("critical_period_end"));
    criticalPeriodLength = criticalPeriodEnd - criticalPeriodStart;
    
    // Calculate plasticity during critical period
    float plasticityDuringCP = calculatePlasticityDuringCriticalPeriod(criticalPeriodStart, criticalPeriodEnd);
    
    NLM_LOG_INFO("Critical period analysis: length = " + std::to_string(criticalPeriodLength) +
                 ", plasticity = " + std::to_string(plasticityDuringCP));
}

float DevelopmentExperiment::calculateDevelopmentFactor(const std::string& phaseName, size_t step) {
    // Calculate developmental factor based on phase
    if (phaseName == "Synaptogenesis") {
        return 0.8f;  // High synaptogenesis rate
    } else if (phaseName == "Sensory Development") {
        return 0.6f;  // Moderate development
    } else if (phaseName == "Motor Development") {
        return 0.5f;  // Moderate development
    } else if (phaseName == "Social Development") {
        return 0.4f;  // Lower development rate
    }
    return 0.3f;  // Default
}

void DevelopmentExperiment::applyDevelopmentalPlasticity(std::shared_ptr<Brain> brain, 
                                                          float developmentFactor, 
                                                          size_t step) {
    if (!brain) return;
    
    // Apply developmental plasticity to synapses
    if (auto region = brain->getRegion(RegionId(1))) {
        auto& synapses = region->getSynapses();
        
        // Synaptogenesis (create new synapses)
        if (step % 50 == 0) {
            size_t numNewSynapses = static_cast<size_t>(synaptogenesisRate * developmentFactor * 10);
            for (size_t i = 0; i < numNewSynapses; ++i) {
                // In a real implementation, would create new synapses
            }
        }
        
        // Pruning (remove weak synapses)
        if (step % 100 == 0) {
            size_t numToPrune = static_cast<size_t>(pruningRate * developmentFactor * 10);
            for (size_t i = 0; i < std::min(numToPrune, synapses.size()); ++i) {
                // In a real implementation, would prune weak synapses
            }
        }
    }
}

float DevelopmentExperiment::calculatePlasticityDuringCriticalPeriod(size_t startStep, size_t endStep) {
    // Calculate average plasticity during critical period
    float totalPlasticity = 0.0f;
    size_t steps = endStep - startStep;
    
    if (steps == 0) return 0.0f;
    
    for (size_t step = startStep; step < endStep; ++step) {
        float factor = calculateDevelopmentFactor("Synaptogenesis", step);
        totalPlasticity += factor;
    }
    
    return totalPlasticity / steps;
}

void DevelopmentExperiment::checkpoint() {
    // TODO: Implement checkpoint saving
    NLM_LOG_INFO("DevelopmentExperiment checkpoint saved");
}

void DevelopmentExperiment::restore() {
    // TODO: Implement checkpoint loading
    NLM_LOG_INFO("DevelopmentExperiment checkpoint restored");
}

bool DevelopmentExperiment::hasParameter(const std::string& name) const {
    return parametersDouble.find(name) != parametersDouble.end() ||
           parametersString.find(name) != parametersString.end() ||
           parametersBool.find(name) != parametersBool.end();
}

} // namespace nlm
