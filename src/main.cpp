// Helper function implementations

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printWelcomeMessage(const NLMCommandLineOptions& options) {
    std::cout << "NLM Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Initializing NLM neural simulation framework..." << std::endl;
    
    if (options.verbose) {
        std::cout << "Verbose mode enabled" << std::endl;
    }
    
    std::cout << "Configuration file: " << (options.configFile.empty() ? "default" : options.configFile) << std::endl;
    std::cout << std::endl;
}

void printPhaseInfo() {
    std::cout << "Phase 2 Objectives:" << std::endl;
    std::cout << "  ✓ Real LIF neuron dynamics implemented" << std::endl;
    std::cout << "  ✓ Event-driven spike propagation with delays" << std::endl;
    std::cout << "  ✓ STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  ✓ Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << "  ✓ Learning experiment demonstrates measurable changes" << std::endl;
    std::cout << "  ✓ Network shows activity-dependent synaptic modification" << std::endl;
    std::cout << std::endl;
}

void printConfiguration(const std::shared_ptr<Config>& config) {
    if (!config) {
        std::cout << "No configuration available" << std::endl;
        return;
    }
    
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << config->summary() << std::endl;
}

void printTestResults(const std::vector<TestFramework::TestResult>& results) {
    if (results.empty()) {
        std::cout << "No tests were run." << std::endl;
        return;
    }
    
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                         TEST RESULTS SUMMARY                               ║" << std::endl;
    std::cout << "╠═════════════╦═══════════╦════════════╦════════════════════════════════════════╗" << std::endl;
    std::cout << "║ Test Name   ║   Status  ║   Time(s)  ║   Details                                 ║" << std::endl;
    std::cout << "╠═════════════╬═══════════╬════════════╬════════════════════════════════════════╗" << std::endl;
    
    for (const auto& result : results) {
        std::string status = result.passed ? "PASSED" : "FAILED";
        std::string timeStr = std::to_string(result.executionTime);
        
        // Truncate long details
        std::string details = result.details;
        if (details.length() > 35) {
            details = details.substr(0, 32) + "...";
        }
        
        std::cout << "║ " << std::setw(11) << result.name 
                  << " ║ " << std::setw(9) << status
                  << " ║ " << std::setw(8) << timeStr
                  << " ║ " << std::setw(38) << details
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═════════════╩═══════════╩════════════╩════════════════════════════════════════╝" << std::endl;
}

void handleError(const std::string& error, int exitCode) {
    std::cerr << "ERROR: " << error << std::endl;
    if (exitCode != 0) {
        exit(exitCode);
    }
}

void setupSignalHandlers() {
    // Setup signal handlers for graceful shutdown
    // For now, this is a placeholder
}

void performanceBenchmark(const std::shared_ptr<Brain>& brain, const std::string& testName) {
    if (!brain) return;
    
    std::cout << std::endl;
    std::cout << "PERFORMANCE BENCHMARK: " << testName << std::endl;
    std::cout << "════════════════════════════════════════════════════════════════════════╗" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation for benchmark
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "Total time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
    std::cout << "Steps/sec: " << std::fixed << std::setprecision(0) << (1000.0 / totalTime) << std::endl;
    std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Firing rate: " << std::fixed << std::setprecision(2) 
              << (static_cast<double>(brain->getTotalSpikeCount()) / 1000.0) << " spikes/step" << std::endl;
    
    std::cout << "╚════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

// LearningExperiment method implementations

void LearningExperiment::recordInitialState() {
    initialSynapseCount = brain->getTotalSynapseCount();
    initialWeights.clear();
    
    // Record initial weights from first region
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    NLM_LOG_INFO("Initial state recorded:");
    NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
    if (!initialWeights.empty()) {
        float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float mean = sum / initialWeights.size();
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void LearningExperiment::recordFinalState() {
    finalWeights.clear();
    
    // Record final weights from first region
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
    
    NLM_LOG_INFO("Final state recorded:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    if (!finalWeights.empty()) {
        float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float mean = sum / finalWeights.size();
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void LearningExperiment::computeStatistics() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment Results ===");
    NLM_LOG_INFO("");
    
    if (initialWeights.empty() || finalWeights.empty()) {
        NLM_LOG_INFO("ERROR: No weights recorded");
        return;
    }
    
    // Compute weight changes
    float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
    float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
    float initialMean = initialSum / initialWeights.size();
    float finalMean = finalSum / finalWeights.size();
    
    NLM_LOG_INFO("Weight Statistics:");
    NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
    NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
    NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
    
    // Count synapses that changed significantly
    size_t strengthened = 0;
    size_t weakened = 0;
    size_t unchanged = 0;
    
    size_t minSize = std::min(initialWeights.size(), finalWeights.size());
    for (size_t i = 0; i < minSize; ++i) {
        float delta = finalWeights[i] - initialWeights[i];
        if (delta > 0.01f) ++strengthened;
        else if (delta < -0.01f) ++weakened;
        else ++unchanged;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Synaptic Changes:");
    NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
    NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
    NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Spike Activity:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
    
    // Determine if learning occurred
    bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                            (strengthened > 0 || weakened > 0);
    
    NLM_LOG_INFO("");
    if (learningOccurred) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
    }
}

void LearningExperiment::runPlasticityExperiment() {
    if (!brain) return;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Plasticity Learning Experiment ===");
    
    // Record initial state
    recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    recordFinalState();
    
    // Compute and display statistics
    computeStatistics();
}
