} // namespace nlm

// Forward declarations for mode implementations
void runDebugMode(std::shared_ptr<Brain> brain, int focusNeuronId);
void runProfileMode(std::shared_ptr<Brain> brain, int duration);
void runBenchmarkMode(std::shared_ptr<Brain> brain, const std::vector<int>& neuronSizes, int iterations);

// Mode implementations
void runDebugMode(std::shared_ptr<Brain> brain, int focusNeuronId) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== DEBUG MODE ===");
    NLM_LOG_INFO("Running with verbose debugging output...");
    
    // Fix syntax error on line 14
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found for debugging");
        return;
    }
    
    NLM_LOG_INFO("Region found with " + std::to_string(region->getAllNeurons().size()) + " neurons");
    
    if (focusNeuronId >= 0) {
        auto neurons = region->getAllNeurons();
        if (static_cast<size_t>(focusNeuronId) < neurons.size()) {
            NLM_LOG_INFO("Focusing on neuron " + std::to_string(focusNeuronId));
            auto* neuron = neurons[focusNeuronId];
            NLM_LOG_INFO("  Membrane potential: " + std::to_string(neuron->getMembranePotential()));
            NLM_LOG_INFO("  Threshold: " + std::to_string(neuron->getThreshold()));
            NLM_LOG_INFO("  Refractory period: " + std::to_string(neuron->getRefractoryPeriod()));
            NLM_LOG_INFO("  Current: " + std::to_string(neuron->getInjectedCurrent()));
            NLM_LOG_INFO("  Is spiking: " + std::string(neuron->isSpiking() ? "YES" : "NO"));
        }
    }
    
    // Run with verbose logging
    NLM_LOG_INFO("Running simulation steps with verbose output...");
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 10 == 0) {
            NLM_LOG_DEBUG("Step " + std::to_string(step) + 
                        " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("Debug mode completed");
}

void runProfileMode(std::shared_ptr<Brain> brain, int duration) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== PROFILE MODE ===");
    NLM_LOG_INFO("Profiling for " + std::to_string(duration) + " seconds...");
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    // Enable detailed timing
    NLM_LOG_INFO("Starting performance profiling...");
    
    for (SimulationStep step = 0; step < static_cast<SimulationStep>(duration / 0.001); ++step) {
        // Vary input to create activity
        if (step % 100 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 500 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing rate: " + std::to_string(brain->getAverageFiringRate()));
        }
    }
    
    auto endWall = std::chrono::high_resolution_clock::now();
    auto totalWallTime = std::chrono::duration<double>(endWall - startWall).count();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== PROFILE RESULTS ===");
    NLM_LOG_INFO("Total wall time: " + std::to_string(totalWallTime) + "s");
    NLM_LOG_INFO("Total simulation steps: " + std::to_string(duration / 0.001));
    NLM_LOG_INFO("Average steps per second: " + std::to_string((duration / 0.001) / totalWallTime));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
}

void runBenchmarkMode(std::shared_ptr<Brain> brain, const std::vector<int>& neuronSizes, int iterations) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== BENCHMARK MODE ===");
    NLM_LOG_INFO("Running benchmarks for " + std::to_string(neuronSizes.size()) + " neuron sizes");
    NLM_LOG_INFO("Iterations per size: " + std::to_string(iterations));
    
    std::vector<float> results;
    
    for (size_t i = 0; i < neuronSizes.size(); ++i) {
        int neuronCount = neuronSizes[i];
        NLM_LOG_INFO("")
        NLM_LOG_INFO("=== Benchmark " + std::to_string(i + 1) + ": " + std::to_string(neuronCount) + " neurons ===");
        
        float totalTime = 0.0f;
        size_t totalSpikes = 0;
        
        for (int iter = 0; iter < iterations; ++iter) {
            // Reset and reinitialize brain
            brain->reset();
            brain->initialize();
            
            auto startWall = std::chrono::high_resolution_clock::now();
            
            // Run simulation
            for (SimulationStep step = 0; step < 500; ++step) {
                if (step % 50 == 0) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
                }
                brain->step(step, step * 0.001);
            }
            
            auto endWall = std::chrono::high_resolution_clock::now();
            auto wallTime = std::chrono::duration<double>(endWall - startWall).count();
            totalTime += wallTime;
            totalSpikes += brain->getTotalSpikeCount();
        }
        
        float avgTime = totalTime / iterations;
        float avgSpikes = static_cast<float>(totalSpikes) / iterations;
        
        NLM_LOG_INFO("Results for " + std::to_string(neuronCount) + " neurons:");
        NLM_LOG_INFO("  Average time per iteration: " + std::to_string(avgTime) + "s");
        NLM_LOG_INFO("  Average spikes: " + std::to_string(avgSpikes));
        NLM_LOG_INFO("  Spikes per second: " + std::to_string(avgSpikes / avgTime));
        NLM_LOG_INFO("  Simulation speed: " + std::to_string(500.0 / avgTime) + " steps/s");
        
        results.push_back(avgTime);
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== BENCHMARK SUMMARY ===");
    for (size_t i = 0; i < neuronSizes.size(); ++i) {
        NLM_LOG_INFO("Size " + std::to_string(neuronSizes[i]) + ": " + std::to_string(results[i]) + "s avg");
    }
}

// Additional utility functions
bool compareBrainStates(std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2) {
    if (!brain1 || !brain2) return false;
    
    bool neuronsMatch = (brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount());
    bool synapsesMatch = (brain1->getTotalSynapseCount() == brain2->getTotalSynapseCount());
    bool spikesMatch = (brain1->getTotalSpikeCount() == brain2->getTotalSpikeCount());
    
    if (neuronsMatch && synapsesMatch && spikesMatch) {
        NLM_LOG_INFO("[PASS] Brain states are compatible");
        return true;
    } else {
        NLM_LOG_INFO("[INFO] Brain states differ:");
        NLM_LOG_INFO("  Neurons: " + std::to_string(brain1->getTotalNeuronCount()) + " vs " + std::to_string(brain2->getTotalNeuronCount()));
        NLM_LOG_INFO("  Synapses: " + std::to_string(brain1->getTotalSynapseCount()) + " vs " + std::to_string(brain2->getTotalSynapseCount()));
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain1->getTotalSpikeCount()) + " vs " + std::to_string(brain2->getTotalSpikeCount()));
        return false;
    }
}

float benchmarkBrain(std::shared_ptr<Brain> brain, int steps) {
    auto startWall = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto endWall = std::chrono::high_resolution_clock::now();
    auto wallTime = std::chrono::duration<double>(endWall - startWall).count();
    
    return wallTime;
}

std::string getFormattedResults(const std::string& mode, const std::string& summary) {
    std::stringstream ss;
    ss << "=== " << mode << " RESULTS ===\n";
    ss << summary << "\n";
    ss << "Completed at: " << __TIMESTAMP__ << "\n";
    return ss.str();
}
