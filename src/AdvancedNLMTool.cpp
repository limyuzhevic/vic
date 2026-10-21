#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "visualization/VisualizationInterface.hpp"

class AdvancedNLMTool {
public:
    AdvancedNLMTool() : config(std::make_shared<Config>()) {
        // Initialize with default advanced settings
        initializeAdvancedDefaults();
    }
    
    void initializeAdvancedDefaults() {
        // Set advanced configuration defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);  // More neurons for better resolution
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);  // Multiple regions for complexity
        config->set("connection_probability", 0.08f, ConfigSource::Default);
        
        // Advanced plasticity parameters
        config->set("stdp_ltp_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
        config->set("stdp_tau", 25.0f, ConfigSource::Default);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", 0.00005f, ConfigSource::Default);
        config->set("pruning_rate", 0.000005f, ConfigSource::Default);
        
        // Memory systems
        config->set("working_memory_capacity", 200, ConfigSource::Default);
        config->set("episodic_memory_max_episodes", 5000, ConfigSource::Default);
        config->set("replay_interval", 50, ConfigSource::Default);
        config->set("consolidation_interval", 500, ConfigSource::Default);
        
        // Neuromodulation
        config->set("dopamine_influence", 0.8f, ConfigSource::Default);
        config->set("curiosity_threshold", 0.3f, ConfigSource::Default);
        
        // Development
        config->set("plasticity_decay_rate", 0.05f, ConfigSource::Default);
        config->set("critical_period_length", 1000, ConfigSource::Default);
        
        // Checkpointing
        config->set("checkpoint_dir", "./advanced_checkpoints", ConfigSource::Default);
        config->set("checkpoint_interval", 100, ConfigSource::Default);
        config->set("max_checkpoints", 20, ConfigSource::Default);
        
        // Visualization
        config->set("enable_visualization", true, ConfigSource::Default);
        config->set("visualization_fps", 30, ConfigSource::Default);
        config->set("visualization_window_width", 1920, ConfigSource::Default);
        config->set("visualization_window_height", 1080, ConfigSource::Default);
    }
    
    void printAdvancedHelp() {
        std::cout << "=== Advanced NLM Tool - Power User Interface ===\n" << std::endl;
        std::cout << "EXPERIMENTS:\n" << std::endl;
        std::cout << "  --run-phase6          Run comprehensive Phase 6 integration test\n";
        std::cout << "  --run-plasticity      Run detailed plasticity learning experiment\n";
        std::cout << "  --run-neural-planner  Run neural planning demonstration\n";
        std::cout << "  --run-memory-demo     Demonstrate memory systems integration\n" << std::endl;
        
        std::cout << "CONFIGURATION:\n" << std::endl;
        std::cout << "  --config <file>       Load configuration from file\n";
        std::cout << "  --set <key>=<value>   Set configuration parameter\n";
        std::cout << "  --list-config         List all current configuration values\n";
        std::cout << "  --save-config <file>  Save current configuration to file\n" << std::endl;
        
        std::cout << "PERFORMANCE & MONITORING:\n" << std::endl;
        std::cout << "  --benchmark           Run performance benchmarks\n";
        std::cout << "  --profile <seconds>  Profile simulation for given duration\n";
        std::cout << "  --log-level <level>  Set logging level (Debug, Info, Warning, Error)\n";
        std::cout << "  --monitor <interval> Monitor brain statistics at intervals\n" << std::endl;
        
        std::cout << "VISUALIZATION:\n" << std::endl;
        std::cout << "  --visualize          Run with visualization interface\n";
        std::cout << "  --record-video <file> Record simulation to video file\n";
        std::cout << "  --export-neurons <file> Export neuron data for analysis\n" << std::endl;
        
        std::cout << "MEMORY & PERSISTENCE:\n" << std::endl;
        std::cout << "  --save-state <file>  Save brain state to file\n";
        std::cout << "  --load-state <file>  Load brain state from file\n";
        std::cout << "  --replay-episodes <count> Replay recent memory episodes\n";
        std::cout << "  --consolidate <threshold> Consolidate memories with threshold\n" << std::endl;
        
        std::cout << "DEVELOPMENT TOOLS:\n" << std::endl;
        std->cout << "  --dev-mode           Enable development mode with enhanced logging\n";
        std::cout << "  --export-neural-graph <file> Export neural connectivity graph\n";
        std::cout << "  --analyze-plasticity Analyze synaptic plasticity patterns\n" << std::endl;
        
        std::cout << "UTILITY:\n" << std::endl;
        std::cout << "  --version             Show version information\n";
        std::cout << "  --help               Show this help message\n";
        std::cout << "  --quiet              Minimize output\n" << std::endl;
    }
    
    bool loadConfiguration(const std::string& filepath) {
        if (config->loadFromFile(filepath)) {
            std::cout << "Configuration loaded from: " << filepath << std::endl;
            return true;
        } else {
            std::cout << "Warning: Could not load configuration from " << filepath << ". Using defaults." << std::endl;
            return false;
        }
    }
    
    void setConfiguration(const std::string& keyValue) {
        size_t pos = keyValue.find('=');
        if (pos != std::string::npos) {
            std::string key = keyValue.substr(0, pos);
            std::string value = keyValue.substr(pos + 1);
            config->set(key, value, ConfigSource::CommandLine);
            std::cout << "Set " << key << " = " << value << std::endl;
        } else {
            std::cout << "Error: Invalid key=value format. Use --set key=value" << std::endl;
        }
    }
    
    void listConfiguration() {
        std::cout << config->summary() << std::endl;
    }
    
    void saveConfiguration(const std::string& filepath) {
        if (config->saveToFile(filepath)) {
            std::cout << "Configuration saved to: " << filepath << std::endl;
        } else {
            std::cout << "Error: Could not save configuration to " << filepath << std::endl;
        }
    }
    
    bool runExperiment(const std::string& experimentType) {
        std::cout << "=== Running Experiment: " << experimentType << " ===\n" << std::endl;
        
        try {
            // Create brain with advanced configuration
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                std::cout << "Error: Failed to initialize brain" << std::endl;
                return false;
            }
            
            if (experimentType == "phase6") {
                return runPhase6Integration(brain);
            } else if (experimentType == "plasticity") {
                return runPlasticityExperiment(brain);
            } else if (experimentType == "neural-planner") {
                return runNeuralPlannerDemo(brain);
            } else if (experimentType == "memory") {
                return runMemoryDemo(brain);
            } else {
                std::cout << "Error: Unknown experiment type: " << experimentType << std::endl;
                return false;
            }
            
        } catch (const std::exception& e) {
            std::cout << "Error during experiment execution: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool runPhase6Integration(std::shared_ptr<Brain> brain) {
        std::cout << "Running Phase 6: Complete Brain Integration Test\n" << std::endl;
        
        // Run multiple demonstration steps
        size_t steps = config->getOr<size_t>("demo_steps", 100);
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * config->getOr<double>("simulation_timestep", 0.001));
            
            // Generate some sensory input to drive the brain
            if (i % 10 == 0) {
                std::vector<double> input(3, 0.5);  // Simple 3D sensory input
                // Simulate sensory processing
                brain->receiveSensoryInput(input);
            }
            
            // Log progress every 10 steps
            if (i % 10 == 0) {
                std::cout << "Step " << i << "/" << steps 
                          << " | Neurons: " << brain->getTotalNeuronCount()
                          << " | Synapses: " << brain->getTotalSynapseCount()
                          << " | Spikes: " << brain->getTotalSpikeCount()
                          << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        // Final status
        std::cout << "\n=== Phase 6 Integration Results ===\n" << std::endl;
        brain->logStatus();
        
        // Save state if configured
        std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        if (config->getOr<bool>("auto_save", false)) {
            std::string filename = checkpointDir + "/phase6_completion_" + 
                                 std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".bin";
            brain->save(filename);
            std::cout << "Brain state saved to: " << filename << std::endl;
        }
        
        return true;
    }
    
    bool runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        std::cout << "Running Plasticity Learning Experiment\n" << std::endl;
        
        // Enable plasticity
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run learning simulation
        size_t learningSteps = config->getOr<size_t>("learning_steps", 500);
        
        for (size_t i = 0; i < learningSteps; ++i) {
            // Inject learning patterns
            for (size_t j = 0; j < 20; ++j) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(i, i * config->getOr<double>("simulation_timestep", 0.001));
            
            if (i % 50 == 0) {
                std::cout << "Learning step " << i << "/" << learningSteps 
                          << " | Synapses: " << brain->getTotalSynapseCount()
                          << " | Spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
        }
        
        std::cout << "\n=== Plasticity Experiment Results ===\n" << std::endl;
        brain->logStatus();
        
        return true;
    }
    
    bool runNeuralPlannerDemo(std::shared_ptr<Brain> brain) {
        std::cout << "Running Neural Planning Demonstration\n" << std::endl;
        
        // This would integrate with the neural planning system
        // For demonstration, we'll run a simulation with planning enabled
        
        size_t demoSteps = config->getOr<size_t>("planning_steps", 200);
        
        for (size_t i = 0; i < demoSteps; ++i) {
            brain->step(i, i * config->getOr<double>("simulation_timestep", 0.001));
            
            if (i % 25 == 0) {
                auto action = brain->produceAction();
                std::cout << "Step " << i << " | Action: " << static_cast<int>(action->type) 
                          << " | Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        std::cout << "\n=== Neural Planning Demo Complete ===\n" << std::endl;
        return true;
    }
    
    bool runMemoryDemo(std::shared_ptr<Brain> brain) {
        std::cout << "Running Memory Systems Demonstration\n" << std::endl;
        
        // Demonstrate memory integration
        std::vector<double> sensoryInput(5, 0.8);
        brain->receiveSensoryInput(sensoryInput);
        
        for (size_t i = 0; i < 50; ++i) {
            brain->step(i, i * config->getOr<double>("simulation_timestep", 0.001));
        }
        
        // Check memory system status
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* associativeMemory = brain->getAssociativeMemory();
        
        std::cout << "\nMemory System Status:\n" << std::endl;
        if (workingMemory) {
            std::cout << "Working Memory: " << workingMemory->getCurrentSize() 
                      << "/" << workingMemory->getCapacity() << " items" << std::endl;
        }
        if (episodicMemory) {
            std::cout << "Episodic Memory: " << episodicMemory->getEpisodeCount() << " episodes" << std::endl;
        }
        if (associativeMemory) {
            // Count associations
            size_t totalAssocs = 0;
            for (const auto& region : brain->getRegions()) {
                totalAssocs += region->getSynapseCount();
            }
            std::cout << "Associative Memory: ~" << totalAssocs << " associations" << std::endl;
        }
        
        std::cout << "\n=== Memory Demo Complete ===\n" << std::endl;
        return true;
    }
    
    void exportNeuronData(const std::string& filename) {
        // This would integrate with neuron export functionality
        std::cout << "Exporting neuron data to: " << filename << std::endl;
        // Implementation would depend on available export functionality
    }
    
    void analyzePlasticity(std::shared_ptr<Brain> brain) {
        std::cout << "Analyzing plasticity patterns...\n" << std::endl;
        
        // Calculate various plasticity metrics
        float eiratio = brain->getExcitationInhibitionRatio();
        size_t totalSpikes = brain->getTotalSpikeCount();
        
        std::cout << "Plasticity Analysis:\n" << std::endl;
        std::cout << "  E/I Ratio: " << eiratio << std::endl;
        std::cout << "  Total Spikes: " << totalSpikes << std::endl;
        std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        
        // Analyze synaptic weight distribution
        float minWeight = std::numeric_limits<float>::max();
        float maxWeight = std::numeric_limits<float>::lowest();
        float totalWeight = 0.0f;
        size_t synapseCount = 0;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                float weight = syn->getWeight();
                minWeight = std::min(minWeight, weight);
                maxWeight = std::max(maxWeight, weight);
                totalWeight += weight;
                synapseCount++;
            }
        }
        
        if (synapseCount > 0) {
            std::cout << "  Synaptic Weight Distribution:\n";
            std::cout << "    Min: " << minWeight << "\n";
            std::cout << "    Max: " << maxWeight << "\n";
            std::cout << "    Mean: " << totalWeight / synapseCount << "\n";
            std::cout << "    StdDev: N/A (would require full implementation)\n";
        }
        
        std::cout << "\n=== Plasticity Analysis Complete ===\n" << std::endl;
    }
    
    std::shared_ptr<Config> config;
};