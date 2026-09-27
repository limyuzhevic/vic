#include "AblationStudyExperiment.hpp"
#include <iostream>
#include <chrono>
#include <cmath>

namespace nlm {

struct AblationStudyExperiment::Impl {
    std::chrono::high_resolution_clock::time_point startTime;
    double baselineSpikeCount;
    double ablatedSpikeCount;
    
    Impl() : baselineSpikeCount(0.0), ablatedSpikeCount(0.0) {}
};

AblationStudyExperiment::AblationStudyExperiment(const std::string& name) 
    : Experiment(name) {
    setVersion("1.0.0");
    setDescription("Ablation study experiment testing importance of different brain components");
    
    // Define ablation study parameters
    addParameterDefinition({"component_to_ablate", "string", "Brain component to ablate (working_memory, episodic_memory, prediction, neuromodulation)", "working_memory", true});
    addParameterDefinition({"baseline_neuron_count", "int", "Baseline neuron count for comparison", static_cast<size_t>(1000), false});
    addParameterDefinition({"ablation_impact_threshold", "double", "Threshold for detecting ablation impact", 0.1, false});
    addParameterDefinition({"run_control_condition", "bool", "Run control condition (normal brain) first", true, false});
    addParameterDefinition({"ablation_intensity", "double", "Intensity of ablation (0.0 to 1.0)", 0.5, false});
    
    pImpl = std::make_unique<Impl>();
}

bool AblationStudyExperiment::run() {
    setStartStep(0);
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    
    // Get ablation parameters
    std::string componentToAblate = getParameter<std::string>("component_to_ablate");
    size_t baselineNeuronCount = getParameter<size_t>("baseline_neuron_count");
    double impactThreshold = getParameter<double>("ablation_impact_threshold");
    bool runControl = getParameter<bool>("run_control_condition");
    double ablationIntensity = getParameter<double>("ablation_intensity");
    
    addNote("Starting ablation study for component: " + componentToAblate);
    addNote("Ablation intensity: " + std::to_string(ablationIntensity * 100) + "%");
    addNote("Baseline neuron count: " + std::to_string(baselineNeuronCount));
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", baselineNeuronCount, ConfigSource::Default);
    config->set("random_seed", getSeed(), ConfigSource::Default);
    
    // Run control condition if requested
    if (runControl) {
        addNote("Running control condition (normal brain)...");
        auto controlBrain = std::make_shared<Brain>(config);
        if (controlBrain->initialize()) {
            // Run control simulation
            for (SimulationStep step = 0; step < 500; ++step) {
                controlBrain->step(step, static_cast<Timestamp>(step * 0.001));
            }
            
            // Record control metrics
            double controlSpikes = static_cast<double>(controlBrain->getTotalSpikeCount());
            double controlRate = controlBrain->getAverageFiringRate();
            double controlEiRatio = controlBrain->getExcitationInhibitionRatio();
            
            pImpl->baselineSpikeCount = controlSpikes;
            
            recordMetric("control_total_spikes", controlSpikes);
            recordMetric("control_average_firing_rate", controlRate);
            recordMetric("control_ei_ratio", controlEiRatio);
            recordMetric("control_steps_completed", 500.0);
            
            addNote("Control condition completed - Total spikes: " + std::to_string(static_cast<size_t>(controlSpikes)));
            addNote("Control firing rate: " + std::to_string(controlRate) + " Hz");
        } else {
            addNote("Control condition failed to initialize brain");
            return false;
        }
    }
    
    // Run ablation condition
    addNote("Running ablation condition for: " + componentToAblate);
    
    // Create ablated brain configuration
    auto ablatedConfig = std::make_shared<Config>(*config);
    
    // Simulate ablation effects based on component
    if (componentToAblate == "working_memory") {
        addNote("Ablating working memory system - removing memory storage capacity");
        ablatedConfig->set("working_memory_capacity", 0, ConfigSource::Runtime);
    } else if (componentToAblate == "episodic_memory") {
        addNote("Ablating episodic memory - disabling experience replay");
        ablatedConfig->set("enable_replay", false, ConfigSource::Runtime);
    } else if (componentToAblate == "prediction") {
        addNote("Ablating prediction system - removing predictive coding");
        ablatedConfig->set("prediction_enabled", false, ConfigSource::Runtime);
    } else if (componentToAblate == "neuromodulation") {
        addNote("Ablating neuromodulation - removing dopamine and curiosity systems");
        ablatedConfig->set("dopamine_enabled", false, ConfigSource::Runtime);
        ablatedConfig->set("curiosity_enabled", false, ConfigSource::Runtime);
    } else {
        addNote("Unknown component: " + componentToAblate + ", running normal brain");
    }
    
    // Add ablation intensity scaling
    double effectiveIntensity = ablationIntensity;
    if (effectiveIntensity > 1.0) effectiveIntensity = 1.0;
    if (effectiveIntensity < 0.0) effectiveIntensity = 0.0;
    
    // Create ablated brain
    auto ablatedBrain = std::make_shared<Brain>(ablatedConfig);
    if (!ablatedBrain->initialize()) {
        addNote("Failed to initialize ablated brain");
        return false;
    }
    
    addNote("Ablated brain initialized successfully");
    
    // Run simulation with modified parameters based on ablation intensity
    size_t simulationSteps = static_cast<size_t>(1000.0 * (1.0 - effectiveIntensity) + 100.0 * effectiveIntensity);
    
    for (SimulationStep step = 0; step < simulationSteps; ++step) {
        ablatedBrain->step(step, static_cast<Timestamp>(step * 0.001));
        
        recordStep(step,
                  ablatedBrain->getTotalNeuronCount(),
                  ablatedBrain->getTotalSynapseCount(),
                  ablatedBrain->getFiringNeuronCount(),
                  ablatedBrain->getAverageFiringRate());
        
        // Record intermediate metrics
        recordMetric("ablated_total_spikes", static_cast<double>(ablatedBrain->getTotalSpikeCount()));
        recordMetric("ablated_average_firing_rate", ablatedBrain->getAverageFiringRate());
        recordMetric("ablated_ei_ratio", ablatedBrain->getExcitationInhibitionRatio());
    }
    
    setEndStep(simulationSteps);
    
    // Get ablated metrics
    double ablatedSpikes = static_cast<double>(ablatedBrain->getTotalSpikeCount());
    double ablatedRate = ablatedBrain->getAverageFiringRate();
    double ablatedEiRatio = ablatedBrain->getExcitationInhibitionRatio();
    
    pImpl->ablatedSpikeCount = ablatedSpikes;
    
    // Calculate ablation impact
    double impact = 0.0;
    if (pImpl->baselineSpikeCount > 0) {
        impact = (pImpl->baselineSpikeCount - ablatedSpikes) / pImpl->baselineSpikeCount;
    }
    
    // Record final metrics
    recordMetric("ablated_total_spikes", ablatedSpikes);
    recordMetric("ablated_average_firing_rate", ablatedRate);
    recordMetric("ablated_ei_ratio", ablatedEiRatio);
    recordMetric("ablation_impact", impact);
    recordMetric("simulation_steps", static_cast<double>(simulationSteps));
    
    // Calculate performance metrics
    double totalTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - pImpl->startTime).count();
    recordMetric("total_time_seconds", totalTime);
    
    // Determine impact significance
    std::string impactLevel;
    if (impact > impactThreshold * 2.0) {
        impactLevel = "severe";
    } else if (impact > impactThreshold) {
        impactLevel = "moderate";
    } else if (impact > 0) {
        impactLevel = "mild";
    } else {
        impactLevel = "none";
    }
    
    addNote("Ablation study completed");
    addNote("Component ablated: " + componentToAblate);
    addNote("Ablation intensity: " + std::to_string(effectiveIntensity * 100) + "%");
    addNote("Total spikes (control): " + std::to_string(static_cast<size_t>(pImpl->baselineSpikeCount)));
    addNote("Total spikes (ablated): " + std::to_string(static_cast<size_t>(ablatedSpikes)));
    addNote("Impact: " + std::to_string(impact * 100) + "% (" + impactLevel + ")");
    
    // Record additional analysis metrics
    if (pImpl->baselineSpikeCount > 0) {
        recordMetric("spike_reduction_percentage", impact * 100.0);
        recordMetric("relative_firing_rate", (ablatedRate / (pImpl->baselineSpikeCount / simulationSteps)) * 100.0);
    }
    
    return true;
}

} // namespace nlm
