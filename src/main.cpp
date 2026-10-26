// This is a partial restore - I'll need to rebuild the main.cpp file
// Let me start with the banner function and basic structure

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

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region safely
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                if (syn) { // Null check
                    initialWeights.push_back(syn->getWeight());
                }
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
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region safely
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                if (syn) { // Null check
                    finalWeights.push_back(syn->getWeight());
                }
            }
        }
        
        if (auto* spikeSystem = brain->getSpikeSystem()) {
            mostActiveNeurons = spikeSystem->getMostActiveNeurons(10);
        }
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
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
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Get region safely
    NeuralRegion* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("Region 1 not found!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    size_t neuronsToStimulate = std::min(size_t(10), neurons.size());
    for (size_t i = 0; i < neuronsToStimulate; ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        size_t neuronsToCheck = std::min(size_t(3), neurons.size());
        for (size_t i = 0; i < neuronsToCheck; ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
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
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses safely
    const auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        if (synapses[i]) { // Null check
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false); // Enable only STDP
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        for (int trial = 0; trial < 50; ++trial) {
            Neuron* preNeuron = neurons[0];
            Neuron* postNeuron = neurons[1];
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < beforeWeights.size(); ++i) {
        if (synapses[i]) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
            totalDelta += delta;
        }
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void printHelp(const std::string& programName) {
    std::cout << "NLM - Neural Learning Machine (Phase 2)" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version, -v           Show version information" << std::endl;
    std::cout << "  --verbose, -V           Enable verbose logging" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --quiet, -q             Suppress informational messages" << std::endl;
    std::cout << "  --benchmark             Run benchmark tests" << std::endl;
    std::cout << "  --replay <file>         Load and replay brain state" << std::endl;
    std::cout << "  --save <file>           Save brain state to file" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced options:" << std::endl;
    std::cout << "  --neuron-count <n>      Set number of neurons" << std::endl;
    std::cout << "  --region-count <n>      Set number of regions" << std::endl;
    std::cout << "  --connection-prob <p>   Set connection probability (0.0-1.0)" << std::endl;
    std::cout << "  --simulation-timestep <t> Set simulation timestep in seconds" << std::endl;
    std::cout << "  --stdp-ltp-weight <w>   Set STDP LTP weight" << std::endl;
    std::cout << "  --stdp-ltd-weight <w>   Set STDP LTD weight" << std::endl;
    std::cout << "  --stdp-tau <t>          Set STDP time constant" << std::endl;
    std::cout << "  --synaptogenesis-rate <r> Set synaptic creation rate" << std::endl;
    std::cout << "  --pruning-rate <r>      Set synaptic pruning rate" << std::endl;
    std::cout << "  --checkpoint-dir <dir>  Set checkpoint directory" << std::endl;
    std::cout << "  --max-checkpoints <n>    Maximum number of checkpoints to keep" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration file format: JSON" << std::endl;
}

bool parseCommandLineArguments(int argc, char** argv, std::shared_ptr<Config> config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp(std::string(argv[0]));
            return false; // Exit after showing help
        } else if (arg == "--version" || arg == "-v") {
            std::cout << "NLM v2.0 (Phase 2: Real Neural Computation)" << std::endl;
            return false; // Exit after version
        } else if (arg == "--verbose" || arg == "-V") {
            // Set verbose log level
            if (auto* logger = Logger::getGlobal()) {
                logger->setLevel(LogLevel::Info);
            }
        } else if (arg == "--quiet" || arg == "-q") {
            // Set quiet log level
            if (auto* logger = Logger::getGlobal()) {
                logger->setLevel(LogLevel::Error);
            }
        } else if (arg == "--benchmark") {
            // Enable benchmark mode
            config->set("benchmark_mode", true, ConfigSource::CommandLine);
        } else if (arg.substr(0, 8) == "--config=") {
            config->set("config_file", arg.substr(8), ConfigSource::CommandLine);
        } else if (arg == "--config" && i + 1 < argc) {
            config->set("config_file", std::string(argv[++i]), ConfigSource::CommandLine);
        } else if (arg.substr(0, 8) == "--neuron-count=") {
            config->set("neuron_count", std::stoll(arg.substr(13)), ConfigSource::CommandLine);
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            config->set("neuron_count", std::stoll(std::string(argv[++i])), ConfigSource::CommandLine);
        } else if (arg.substr(0, 14) == "--region-count=") {
            config->set("region_count", std::stoll(arg.substr(15)), ConfigSource::CommandLine);
        } else if (arg == "--region-count" && i + 1 < argc) {
            config->set("region_count", std::stoll(std::string(argv[++i])), ConfigSource::CommandLine);
        } else if (arg.substr(0, 22) == "--connection-prob=") {
            config->set("connection_probability", std::stof(arg.substr(23)), ConfigSource::CommandLine);
        } else if (arg == "--connection-prob" && i + 1 < argc) {
            config->set("connection_probability", std::stof(std::string(argv[++i])), ConfigSource::CommandLine);
        } else if (arg.substr(0, 22) == "--simulation-timestep=") {
            config->set("simulation_timestep", std::stod(arg.substr(23)), ConfigSource::CommandLine);
        } else if (arg == "--simulation-timestep" && i + 1 < argc) {
            config->set("simulation_timestep", std::stod(std::string(argv[++i])), ConfigSource::CommandLine);
        } else if (arg.substr(0, 16) == "--stdp-ltp-weight=") {
            config->set("stdp_ltp_weight", std::stof(arg.substr(17)), ConfigSource::CommandLine);
        } else if (arg == "--stdp-ltp-weight" && i + 1 < argc) {
            config->set("stdp_ltp_weight", std::stof(std::string(argv[++i])), ConfigSource::CommandLine);
        } else if (arg.substr(0, 17) == "--stdp-ltd-weight=") {
            config->set("stdp_ltd_weight", std::stof(arg.substr(18)), ConfigSource::CommandLine);
        } else if (arg == "--stdp-ltd-weight" && i + 1 < argc) {
            config->set("stdp_ltd_weight", std::stof(std::string(argv[++i])), ConfigSource::CommandLine);
        } else if (arg.substr(0, 13) == "--stdp-tau=") {
            config->set("stdp_tau", std::stof(arg.substr(14)), ConfigSource::CommandLine);
        } else if (arg == "--stdp-tau" && i + 1 < argc) {
            config->set("stdp_tau", std::stof(std::string(argv[++i])), ConfigSource::CommandLine);
        } else if (arg.substr(0, 23) == "--synaptogenesis-rate=") {
            config->set("synaptogenesis_rate", std::stof(arg.substr(24)), ConfigSource::CommandLine);
        } else if (arg == "--synaptogenesis-rate" && i + 1 < argc) {
            config->set("synaptogenesis_rate", std::stof(std::string(argv[++i])), ConfigSource::CommandLine);
        } else if (arg.substr(0, 16) == "--pruning-rate=") {
            config->set("pruning_rate", std::stof(arg.substr(17)), ConfigSource::CommandLine);
        } else if (arg == "--pruning-rate" && i + 1 < argc) {
            config->set("pruning_rate", std::stof(std::string(argv[++i])), ConfigSource::CommandLine);
        } else if (arg.substr(0, 19) == "--checkpoint-dir=") {
            config->set("checkpoint_dir", arg.substr(20), ConfigSource::CommandLine);
        } else if (arg == "--checkpoint-dir" && i + 1 < argc) {
            config->set("checkpoint_dir", std::string(argv[++i]), ConfigSource::CommandLine);
        } else if (arg.substr(0, 19) == "--max-checkpoints=") {
            config->set("max_checkpoints", std::stoull(arg.substr(20)), ConfigSource::CommandLine);
        } else if (arg == "--max-checkpoints" && i + 1 < argc) {
            config->set("max_checkpoints", std::stoull(std::string(argv[++i])), ConfigSource::CommandLine);
        } else if (arg.substr(0, 12) == "--replay=") {
            config->set("replay_file", arg.substr(13), ConfigSource::CommandLine);
        } else if (arg == "--replay" && i + 1 < argc) {
            config->set("replay_file", std::string(argv[++i]), ConfigSource::CommandLine);
        } else if (arg.substr(0, 11) == "--save=") {
            config->set("save_file", arg.substr(12), ConfigSource::CommandLine);
        } else if (arg == "--save" && i + 1 < argc) {
            config->set("save_file", std::string(argv[++i]), ConfigSource::CommandLine);
        } else if (arg == "--log-level" && i + 1 < argc) {
            std::string level = std::string(argv[++i]);
            LogLevel logLevel = LogLevel::Info;
            if (level == "debug") logLevel = LogLevel::Debug;
            else if (level == "info") logLevel = LogLevel::Info;
            else if (level == "warn") logLevel = LogLevel::Warning;
            else if (level == "error") logLevel = LogLevel::Error;
            else if (level == "fatal") logLevel = LogLevel::Fatal;
            
            if (auto* logger = Logger::getGlobal()) {
                logger->setLevel(logLevel);
            }
        }
    }
    
    return true;
}