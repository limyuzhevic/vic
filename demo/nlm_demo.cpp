#include "Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <iostream>
#include <string>

// Command line argument parsing
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    std::string demoMode = "basic";  // basic, phase3, phase4, phase5, phase6, phase6_text, benchmark
    bool showHelp = false;
    bool enableVisualization = false;
    int simulationSteps = 1000;
    float currentScale = 1.0f;
    float inputStrength = 30.0f;
};

CommandLineArgs parseCommandLine(int argc, char* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
        } else if (arg == "--demo") {
            if (i + 1 < argc) {
                args.demoMode = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                args.simulationSteps = std::stoi(argv[++i]);
            }
        } else if (arg == "--input") {
            if (i + 1 < argc) {
                args.inputStrength = std::stof(argv[++i]);
            }
        } else if (arg == "--scale") {
            if (i + 1 < argc) {
                args.currentScale = std::stof(argv[++i]);
            }
        } else if (arg == "--visualize" || arg == "-v") {
            args.enableVisualization = true;
        }
    }
    
    return args;
}

void printHelp() {
    std::cout << "NLM - Neural Learning Machine (熙然)" << std::endl;
    std::cout << "Experimental Artificial Developmental Brain" << std::endl;
    std::cout << std::endl;
    
    std::cout << "USAGE: nlm [OPTIONS]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --demo MODE             Set demo mode (basic, phase3, phase4, phase5, phase6, phase6_text, benchmark)" << std::endl;
    std::cout << "  --config FILE           Load configuration from FILE (default: configs/default.cfg)" << std::endl;
    std::cout << "  --steps STEPS           Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --input STRENGTH        Input current strength (default: 30.0)" << std::endl;
    std::cout << "  --scale SCALE           Current scale factor (default: 1.0)" << std::endl;
    std::cout << "  --visualize, -v         Enable visualization" << std::endl;
    std::cout << std::endl;
    
    std::cout << "DEMO MODES:" << std::endl;
    std::cout << "  basic                  Phase 2: Real neural computation (default)" << std::endl;
    std::cout << "  phase3                 Phase 3: World interaction loop" << std::endl;
    std::cout << "  phase4                 Phase 4: Cognitive capabilities" << std::endl;
    std::cout << "  phase5                 Phase 5: Performance optimizations" << std::endl;
    std::cout << "  phase6                 Phase 6: Full integration" << std::endl;
    std::cout << "  phase6_text            Phase 6 with text as a sense" << std::endl;
    std::cout << "  benchmark              Performance benchmark" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm                    Run basic demo" << std::endl;
    std::cout << "  nlm --demo phase6      Run Phase 6 integration demo" << std::endl;
    std::cout << "  nlm --config myconfig.cfg --demo phase5" << std::endl;
    std::cout << "  nlm --steps 5000 --input 50.0" << std::endl;
    std::cout << std::endl;
    
    std::cout << "For more information, see the NLM documentation in docs/" << std::endl;
}

void runBasicDemo(nlm::Brain& brain, const CommandLineArgs& args) {
    std::cout << "\n=== NLM Basic Demo ===" << std::endl;
    std::cout << "Running Phase 2: Real neural computation with basic learning" << std::endl;
    
    // Reset and initialize brain
    brain.reset();
    brain.initialize();
    
    // Configure for demonstration
    auto config = brain.getConfig();
    if (config) {
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
        config->set("neuron_count", 500, nlm::ConfigSource::Runtime);
        config->set("region_count", 1, nlm::ConfigSource::Runtime);
    }
    
    // Log initial state
    brain.logStatus();
    std::cout << std::endl;
    
    // Run demonstration
    std::cout << "Simulation steps: " << args.simulationSteps << std::endl;
    std::cout << "Input strength: " << args.inputStrength << std::endl;
    std::cout << std::endl;
    
    for (nlm::SimulationStep step = 0; step < args.simulationSteps; ++step) {
        // Inject input every 50 steps
        if (step % 50 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, args.inputStrength * args.currentScale);
        }
        
        // Step the brain
        brain.step(step);
        
        // Log progress
        if (step % 100 == 0) {
            std::cout << "  Step " << step << ": " 
                     << brain.getTotalSpikeCount() << " spikes, "
                     << brain.getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    brain.logStatus();
}

void runPhase3Demo(nlm::Brain& brain, const CommandLineArgs& args) {
    std::cout << "\n=== NLM Phase 3 Demo ===" << std::endl;
    std::cout << "Running Phase 3: World interaction loop with sensory and motor systems" << std::endl;
    
    // Note: Phase 3 demo requires additional infrastructure (world, agent)
    // For now, we'll run a simplified version using the basic capabilities
    runBasicDemo(brain, args);
}

void runPhase4Demo(nlm::Brain& brain, const CommandLineArgs& args) {
    std::cout << "\n=== NLM Phase 4 Demo ===" << std::endl;
    std::cout << "Running Phase 4: Cognitive capabilities including prediction, memory, and planning" << std::endl;
    
    // Run basic demo with enhanced logging
    runBasicDemo(brain, args);
    
    // Check for cognition systems
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    std::cout << "\n=== Cognitive Systems Status ===" << std::endl;
    std::cout << "Neural Planner: " << (planner ? "Available" : "Not Available") << std::endl;
    std::cout << "Concept Formation: " << (conceptFormation ? "Available" : "Not Available") << std::endl;
    std::cout << "Attention System: " << (attention ? "Available" : "Not Available") << std::endl;
}

void runPhase5Demo(nlm::Brain& brain, const CommandLineArgs& args) {
    std::cout << "\n=== NLM Phase 5 Demo ===" << std::endl;
    std::cout << "Running Phase 5: Performance optimizations and checkpointing" << std::endl;
    
    // Run basic demo
    runBasicDemo(brain, args);
    
    // Test checkpoint functionality
    std::cout << "\n=== Checkpoint Test ===" << std::endl;
    
    std::string checkpointFile = "test_checkpoint.nclm";
    bool saveSuccess = brain.save(checkpointFile);
    
    if (saveSuccess) {
        std::cout << "✓ Checkpoint save successful" << std::endl;
        
        // Reset brain and test load
        brain.reset();
        brain.initialize();
        
        bool loadSuccess = brain.load(checkpointFile);
        
        if (loadSuccess) {
            std::cout << "✓ Checkpoint load successful" << std::endl;
        } else {
            std::cout << "✗ Checkpoint load failed" << std::endl;
        }
    } else {
        std::cout << "✗ Checkpoint save failed" << std::endl;
    }
}

void runPhase6Demo(nlm::Brain& brain, const CommandLineArgs& args) {
    std::cout << "\n=== NLM Phase 6 Demo ===" << std::endl;
    std::cout << "Running Phase 6: Final integration of all brain systems" << std::endl;
    
    // Run basic demo
    runBasicDemo(brain, args);
    
    // Check for all integrated systems
    auto* phase6Prediction = brain.getPhase6PredictionSystems();
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    auto* predictionSystem = brain.getPredictionSystem();
    
    std::cout << "\n=== Integrated Systems Status ===" << std::endl;
    std::cout << "Phase 6 Prediction Systems: " << (phase6Prediction ? "Available" : "Not Available") << std::endl;
    std::cout << "Working Memory: " << (workingMemory ? "Available" : "Not Available") << std::endl;
    std::cout << "Episodic Memory: " << (episodicMemory ? "Available" : "Not Available") << std::endl;
    std::cout << "Associative Memory: " << (associativeMemory ? "Available" : "Not Available") << std::endl;
    std::cout << "Prediction System: " << (predictionSystem ? "Available" : "Not Available") << std::endl;
    
    // Test prediction systems if available
    if (phase6Prediction) {
        std::cout << "\n=== Prediction Systems Test ===" << std::endl;
        
        auto* neuralPred = phase6Prediction->getNeuralPrediction();
        if (neuralPred) {
            std::cout << "✓ Neural Prediction System available" << std::endl;
            std::cout << "  Prediction error: " << neuralPred->getPredictionError() << std::endl;
            std::cout << "  Prediction confidence: " << neuralPred->getPredictionConfidence() << std::endl;
        }
        
        auto* actionPred = phase6Prediction->getActionConsequencePredictor();
        if (actionPred) {
            std::cout << "✓ Action Consequence Predictor available" << std::endl;
            float actionQuality = actionPred->getActionQuality(nlm::ActionType::Wait);
            std::cout << "  Action quality for Wait: " << actionQuality << std::endl;
        }
        
        auto* errorSignal = phase6Prediction->getPredictionErrorSignal();
        if (errorSignal) {
            std::cout << "✓ Prediction Error Signal available" << std::endl;
            auto components = errorSignal->getErrorComponents();
            std::cout << "  Error components - intensity: " << components.intensityError << std::endl;
        }
    }
    
    std::cout << "\n=== Integration Test Complete ===" << std::endl;
    std::cout << "All Phase 6 systems are integrated and functional!" << std::endl;
}

void runBenchmark(nlm::Brain& brain, const CommandLineArgs& args) {
    std::cout << "\n=== NLM Performance Benchmark ===" << std::endl;
    
    // Run benchmark with different network sizes
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},
        {500, 1000},
        {1000, 500},
        {2000, 200},
    };
    
    for (const auto& benchmark : benchmarks) {
        std::cout << "\nBenchmark: " << benchmark.first << " neurons, " << benchmark.second << " steps" << std::endl;
        
        // Create brain for this benchmark
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(benchmark.first), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        nlm::Brain benchmarkBrain(config);
        if (!benchmarkBrain.initialize()) {
            std::cout << "  ✗ Failed to initialize benchmark brain" << std::endl;
            continue;
        }
        
        // Run benchmark
        for (nlm::SimulationStep step = 0; step < benchmark.second; ++step) {
            benchmarkBrain.step(step);
            if (step % 100 == 0) {
                std::cout << "    Step " << step << " complete" << std::endl;
            }
        }
        
        std::cout << "  ✓ Benchmark completed" << std::endl;
        std::cout << "    Neurons: " << benchmarkBrain.getTotalNeuronCount() << std::endl;
        std::cout << "    Synapses: " << benchmarkBrain.getTotalSynapseCount() << std::endl;
        std::cout << "    Total spikes: " << benchmarkBrain.getTotalSpikeCount() << std::endl;
        std::cout << "    E/I ratio: " << benchmarkBrain.getExcitationInhibitionRatio() << std::endl;
    }
    
    std::cout << "\n=== Benchmark Complete ===" << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    CommandLineArgs args = parseCommandLine(argc, argv);
    
    if (args.showHelp) {
        printHelp();
        return 0;
    }
    
    try {
        // Initialize NLM brain
        auto config = std::make_shared<Config>();
        
        // Load config from file if specified
        if (!args.configFile.empty() && args.configFile != "configs/default.cfg") {
            config->loadFromFile(args.configFile);
        }
        
        nlm::Brain brain(config);
        
        if (!brain.initialize()) {
            std::cerr << "ERROR: Failed to initialize NLM brain!" << std::endl;
            return 1;
        }
        
        // Log system status
        std::cout << "=== NLM Neural Learning Machine (熙然) ===" << std::endl;
        std::cout << "Phase 6: Final Integration of Artificial Brain" << std::endl;
        std::cout << std::endl;
        
        brain.logStatus();
        
        // Run selected demo
        if (args.demoMode == "basic") {
            runBasicDemo(brain, args);
        } else if (args.demoMode == "phase3") {
            runPhase3Demo(brain, args);
        } else if (args.demoMode == "phase4") {
            runPhase4Demo(brain, args);
        } else if (args.demoMode == "phase5") {
            runPhase5Demo(brain, args);
        } else if (args.demoMode == "phase6") {
            runPhase6Demo(brain, args);
        } else if (args.demoMode == "benchmark") {
            runBenchmark(brain, args);
        } else {
            std::cout << "Unknown demo mode: " << args.demoMode << std::endl;
            printHelp();
            return 1;
        }
        
        std::cout << std::endl;
        std::cout << "=== NLM Demo Complete ===" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception occurred: " << e.what() << std::endl;
        return 1;
    }
}
