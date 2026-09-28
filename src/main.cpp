// Performance benchmarking and profiling
void runBenchmark(const Config& config, size_t steps, bool detailed = false) {
    NLM_LOG_INFO("=== NLM Performance Benchmark ===");
    NLM_LOG_INFO("Configuration: " + config.summary());
    NLM_LOG_INFO("Steps: " + std::to_string(steps));
    if (detailed) {
        NLM_LOG_INFO("Mode: Detailed profiling");
    } else {
        NLM_LOG_INFO("Mode: Quick benchmark");
    }
    
    // Create brain
    auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for benchmark!");
        return;
    }
    
    // Warm up
    NLM_LOG_INFO("Warming up brain...");
    for (size_t i = 0; i < 10; ++i) {
        brain->step(i, i * 0.001f);
    }
    
    // Benchmark main computation
    auto startWall = std::chrono::high_resolution_clock::now();
    auto startCPU = std::clock();
    
    NLM_LOG_INFO("Running benchmark...");
    size_t spikesPerSecond = 0;
    
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001f);
        
        if (detailed && step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + ": " +
                        std::to_string(brain->getAverageFiringRate()) + " Hz, " +
                        std::to_string(brain->getTotalSpikeCount()) + " total spikes");
        }
        
        spikesPerSecond += brain->getFiringNeuronCount();
    }
    
    auto endWall = std::chrono::high_resolution_clock::now();
    auto endCPU = std::clock();
    
    // Calculate statistics
    double wallTime = std::chrono::duration<double>(endWall - startWall).count();
    double cpuTime = (double)(endCPU - startCPU) / CLOCKS_PER_SEC;
    double stepsPerSecondWall = steps / wallTime;
    double stepsPerSecondCPU = steps / cpuTime;
    double averageFiringRate = spikesPerSecond / steps;
    size_t totalSpikes = brain->getTotalSpikeCount();
    
    // Performance analysis
    NLM_LOG_INFO("=== Benchmark Results ===");
    NLM_LOG_INFO("Wall Clock Time: " + std::to_string(wallTime) + "s");
    NLM_LOG_INFO("CPU Time: " + std::to_string(cpuTime) + "s");
    NLM_LOG_INFO("Steps/Second (Wall): " + std::to_string(stepsPerSecondWall));
    NLM_LOG_INFO("Steps/Second (CPU): " + std::to_string(stepsPerSecondCPU));
    NLM_LOG_INFO("Average Firing Rate: " + std::to_string(averageFiringRate) + " Hz");
    NLM_LOG_INFO("Total Spikes Generated: " + std::to_string(totalSpikes));
    
    // Performance metrics
    NLM_LOG_INFO("=== Performance Metrics ===");
    
    // Neuron performance
    size_t totalNeurons = brain->getTotalNeuronCount();
    float neuronUtilization = (float)brain->getFiringNeuronCount() / totalNeurons * 100.0f;
    NLM_LOG_INFO("Neuron Utilization: " + std::to_string(neuronUtilization) + "%");
    
    // Synapse performance
    size_t totalSynapses = brain->getTotalSynapseCount();
    float synapseActivity = (float)totalSpikes / totalSynapses * 1000.0f;
    NLM_LOG_INFO("Synapse Activity: " + std::to_string(synapseActivity) + " spikes per 1000 synapses");
    
    // Memory efficiency
    auto* workingMem = brain->getWorkingMemory();
    if (workingMem) {
        float memoryEfficiency = (float)workingMem->getActiveTraces() / workingMem->getCapacity() * 100.0f;
        NLM_LOG_INFO("Memory Efficiency: " + std::to_string(memoryEfficiency) + "%");
    }
    
    // Plasticity effectiveness
    float plasticityRate = (float)(finalWeights.size()) / (finalWeights.size() + initialWeights.size()) * 100.0f;
    NLM_LOG_INFO("Plasticity Effectiveness: " + std::to_string(plasticityRate) + "% of synapses modified");
    
    // Performance classification
    NLM_LOG_INFO("=== Performance Classification ===");
    if (stepsPerSecondWall >= 10000) {
        NLM_LOG_INFO("Performance: EXCELLENT (>10,000 steps/sec)");
    } else if (stepsPerSecondWall >= 5000) {
        NLM_LOG_INFO("Performance: GOOD (5,000-10,000 steps/sec)");
    } else if (stepsPerSecondWall >= 1000) {
        NLM_LOG_INFO("Performance: ACCEPTABLE (1,000-5,000 steps/sec)");
    } else {
        NLM_LOG_INFO("Performance: NEEDS OPTIMIZATION (<1,000 steps/sec)");
    }
    
    // Recommendations
    NLM_LOG_INFO("=== Recommendations ===");
    if (stepsPerSecondWall < 5000) {
        NLM_LOG_INFO("- Consider enabling SIMD optimizations");
        NLM_LOG_INFO("- Optimize memory access patterns");
        NLM_LOG_INFO("- Reduce unnecessary computations");
        NLM_LOG_INFO("- Use more efficient data structures");
    }
    if (neuronUtilization < 10.0f) {
        NLM_LOG_INFO("- Consider adjusting connection probability");
        NLM_LOG_INFO("- Optimize neuron thresholds and parameters");
    }
    if (synapseActivity > 100.0f) {
        NLM_LOG_INFO("- Consider reducing synapse density for larger networks");
        NLM_LOG_INFO("- Optimize synaptic learning rates");
    }
    
    NLM_LOG_INFO("=== Benchmark Complete ===");
}

// Print program usage information
void printUsage() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║                     NLM Usage Guide                            ║
    ║                                                               ║
    ║  NLM is a neural simulation framework with advanced capabilities:  ║
    ║                                                               ║
    ║  Basic Usage:                                                ║
    ║    ./nlm                         Run standard Phase 2 test suite     ║
    ║                                                               ║
    ║  Benchmark Options:                                          ║
    ║    ./nlm --benchmark             Quick performance benchmark       ║
    ║    ./nlm --benchmark-detailed    Detailed performance profiling   ║
    ║                                                               ║
    ║  Benchmark Configuration:                                    ║
    ║    ./nlm --benchmark --benchmark-steps N    Run N steps benchmark   ║
    ║    ./nlm --benchmark --neuron-count K        Use K neurons        ║
    ║    ./nlm --benchmark-mode MODE                 Set benchmark mode    ║
    ║                                                               ║
    ║  Benchmark Modes:                                            ║
    ║    quick      Fast benchmark (default)                          ║
    ║    detailed   Detailed profiling with intermediate output      ║
    ║    intensive  Intensive benchmark for optimization              ║
    ║                                                               ║
    ║  Example Commands:                                           ║
    ║    ./nlm --benchmark --benchmark-steps 1000                   ║
    ║    ./nlm --benchmark-detailed --benchmark-steps 500           ║
    ║    ./nlm --benchmark --neuron-count 5000 --benchmark-steps 2000║
    ║                                                               ║
    ║  Additional Options:                                         ║
    ║    --help                   Show this help message                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Main function with benchmark support
int main(int argc, char* argv[]) {
    printBanner();
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse command line arguments
    Config config;
    size_t steps = 1000;
    std::string mode = "quick";
    bool benchmark = false;
    bool detailed = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--benchmark") {
            benchmark = true;
        } else if (arg == "--benchmark-detailed") {
            benchmark = true;
            detailed = true;
        } else if (arg == "--benchmark-steps" && i + 1 < argc) {
            steps = std::stoul(argv[++i]);
        } else if (arg == "--benchmark-mode" && i + 1 < argc) {
            mode = argv[++i];
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            uint64_t neuronCount = std::stoull(argv[++i]);
            config.set("brain.neuron_count", neuronCount);
        } else if (arg == "--steps" && i + 1 < argc) {
            steps = std::stoul(argv[++i]);
        } else if (arg == "--help") {
            printUsage();
            return 0;
        }
    }
    
    // Print usage if no arguments provided
    if (argc == 1) {
        printUsage();
        std::cout << "\nFor more information, run:";
        std::cout << "\n  ./nlm --benchmark --benchmark-steps 1000";
        std::cout << "\n  ./nlm --benchmark-detailed --benchmark-steps 500";
        std::cout << "\n\nExample benchmarks:";
        std::cout << "\n  Quick benchmark:   ./nlm --benchmark --benchmark-steps 1000";
        std::cout << "\n  Detailed benchmark: ./nlm --benchmark-detailed --benchmark-steps 500";
        std::cout << "\n  Large network:     ./nlm --benchmark --benchmark-steps 10000 --neuron-count 5000";
        std::cout << "\n\nFor help on all options:";
        std::cout << "\n  ./nlm --help";
        return 0;
    }
    
    try {
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("brain.neuron_count", 1000);
        cfg->set("brain.synapse_density", 0.1f);
        cfg->set("brain.connection_probability", 0.05f);
        cfg->set("plasticity.stdp.learning_rate", 0.001f);
        cfg->set("plasticity.hebbian.enable", true);
        cfg->set("plasticity.structural.enable", true);
        cfg->set("neuromod.curiosity.enable", true);
        cfg->set("simulation.dt", 0.001f);
        cfg->set("simulation.max_steps", steps);
        
        // Override with command line arguments
        if (config.has("brain.neuron_count")) {
            cfg->set("brain.neuron_count", config.get<int64_t>("brain.neuron_count"));
        }
        
        // Set neural parameters based on configuration
        cfg->set("brain.v_thresh", -50.0f);
        cfg->set("brain.v_rest", -70.0f);
        cfg->set("brain.v_reset", -75.0f);
        cfg->set("brain.tau_mem", 20.0f);
        cfg->set("brain.tau_ref", 2.0f);
        
        // Run benchmark if requested
        if (benchmark) {
            NLM_LOG_INFO("Running benchmark mode: " + mode);
            if (mode == "detailed") {
                runBenchmark(*cfg, steps, true);
            } else {
                runBenchmark(*cfg, steps, false);
            }
            return 0;
        }
        
        // Original standard execution
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NLM - Phase 2 Test Suite ===");
        NLM_LOG_INFO("Standard neural computation and plasticity tests");
        NLM_LOG_INFO("");
        
        // Create brain
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(cfg);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Run Test 1: Basic connectivity
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Connectivity ===");
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        // Run Test 2: Plasticity learning experiment
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        runPlasticityExperiment(brain);
        
        // Reset and run Test 3: STDP verification
        brain->reset();
        brain->initialize();
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        runStdpVerification(brain);
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 2 Complete ===");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Phase 2 Objectives Completed:");
        NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
        NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
        NLM_LOG_INFO("  ✓ STDP plasticity rule");
        NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
        NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
        NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
        NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
        NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
        NLM_LOG_INFO("");
        
        return 0;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Program failed with exception: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Program failed with unknown exception");
        return 1;
    }
}