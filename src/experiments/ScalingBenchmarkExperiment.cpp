#include "ScalingBenchmarkExperiment.hpp"
#include <iostream>
#include <chrono>

namespace nlm {

struct ScalingBenchmarkExperiment::Impl {
    std::chrono::high_resolution_clock::time_point startTime;
    double totalCpuTime;
    size_t stepsCompleted;
    
    Impl() : totalCpuTime(0.0), stepsCompleted(0) {}
};

ScalingBenchmarkExperiment::ScalingBenchmarkExperiment(const std::string& name) 
    : Experiment(name) {
    setVersion("1.0.0");
    setDescription("Scaling benchmark experiment testing system performance with different neuron counts");
    
    // Define scaling benchmark parameters
    addParameterDefinition({"initial_neurons", "int", "Initial number of neurons", static_cast<size_t>(10000), true});
    addParameterDefinition({"max_neurons", "int", "Maximum number of neurons to scale to", static_cast<size_t>(100000), true});
    addParameterDefinition({"connection_probability", "double", "Probability of synaptic connections", 0.1, false});
    addParameterDefinition({"scale_factor", "double", "Scaling factor between neuron counts", 1.5, false});
    addParameterDefinition({"run_time_limit", "int", "Maximum runtime in seconds", 300, false});
    
    pImpl = std::make_unique<Impl>();
}

bool ScalingBenchmarkExperiment::run() {
    setStartStep(0);
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->stepsCompleted = 0;
    
    // Create brain configuration
    auto config = std::make_shared<Config>();
    
    // Set scaling parameters
    if (hasParameter("initial_neurons")) {
        config->set("neuron_count", getParameter<size_t>("initial_neurons"), ConfigSource::Default);
    }
    if (hasParameter("connection_probability")) {
        config->set("connection_probability", getParameter<double>("connection_probability"), ConfigSource::Default);
    }
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        addNote("Failed to initialize brain for scaling benchmark");
        return false;
    }
    
    // Get scaling parameters
    size_t maxNeurons = getParameter<size_t>("max_neurons");
    double scaleFactor = getParameter<double>("scale_factor");
    size_t initialNeurons = getParameter<size_t>("initial_neurons");
    int runTimeLimit = getParameter<int>("run_time_limit");
    
    addNote("Starting scaling benchmark from " + std::to_string(initialNeurons) + " neurons");
    addNote("Target: " + std::to_string(maxNeurons) + " neurons (factor: " + std::to_string(scaleFactor) + ")");
    addNote("Time limit: " + std::to_string(runTimeLimit) + " seconds");
    
    // Run multiple scaling tests
    size_t currentNeuronCount = initialNeurons;
    size_t stepCounter = 0;
    auto timeLimit = std::chrono::seconds(runTimeLimit);
    
    while (currentNeuronCount < maxNeurons) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        if (currentTime - pImpl->startTime > timeLimit) {
            addNote("Time limit reached, stopping scaling benchmark");
            break;
        }
        
        // Simulate scaling by adjusting configuration
        config->set("neuron_count", currentNeuronCount, ConfigSource::Runtime);
        
        // Create new brain with updated neuron count
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            addNote("Failed to initialize brain with " + std::to_string(currentNeuronCount) + " neurons");
            break;
        }
        
        // Run simulation for this configuration
        size_t targetSteps = std::min<size_t>(100, (maxNeurons - currentNeuronCount) / 100);
        for (size_t step = 0; step < targetSteps && stepCounter < 10000; ++step) {
            brain->step(stepCounter, static_cast<Timestamp>(stepCounter * 0.001));
            
            recordStep(stepCounter,
                      brain->getTotalNeuronCount(),
                      brain->getTotalSynapseCount(),
                      brain->getFiringNeuronCount(),
                      brain->getAverageFiringRate());
            
            recordMetric("neuron_count", static_cast<double>(brain->getTotalNeuronCount()));
            recordMetric("spike_rate", static_cast<double>(brain->getTotalSpikeCount()) / static_cast<double>(step + 1));
            recordMetric("memory_usage", static_cast<double>(brain->getTotalNeuronCount() * 1024)); // approximate
            
            stepCounter++;
            pImpl->stepsCompleted++;
        }
        
        // Increase neuron count for next iteration
        currentNeuronCount = static_cast<size_t>(currentNeuronCount * scaleFactor);
        if (currentNeuronCount > maxNeurons) {
            currentNeuronCount = maxNeurons;
        }
        
        addNote("Completed scaling to " + std::to_string(currentNeuronCount) + " neurons");
    }
    
    setEndStep(stepCounter);
    
    // Record final benchmark metrics
    recordMetric("final_neuron_count", static_cast<double>(brain->getTotalNeuronCount()));
    recordMetric("total_spikes", brain->getTotalSpikeCount());
    recordMetric("average_firing_rate", brain->getAverageFiringRate());
    recordMetric("steps_completed", static_cast<double>(stepCounter));
    
    // Calculate performance metrics
    double totalTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - pImpl->startTime).count();
    recordMetric("total_time_seconds", totalTime);
    recordMetric("spikes_per_second", static_cast<double>(brain->getTotalSpikeCount()) / totalTime);
    recordMetric("neurons_per_second", static_cast<double>(brain->getTotalNeuronCount()) / totalTime);
    
    addNote("Scaling benchmark completed");
    addNote("Final neuron count: " + std::to_string(brain->getTotalNeuronCount()));
    addNote("Total steps completed: " + std::to_string(stepCounter));
    addNote("Total spikes generated: " + std::to_string(brain->getTotalSpikeCount()));
    addNote("Total time: " + std::to_string(totalTime) + " seconds");
    
    return true;
}

} // namespace nlm
