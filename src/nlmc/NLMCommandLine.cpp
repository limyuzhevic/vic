// NLM Command Line Interface (nlmc)
// Advanced debugging, analysis, and control commands for NLM users

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cctype>

// Forward declarations
class Config;
class Brain;

namespace nlmc {

// Command result structure
struct CommandResult {
    bool success;
    std::string output;
    std::string error;
    
    CommandResult(bool s = true, const std::string& out = "", const std::string& err = "")
        : success(s), output(out), error(err) {}
};

// Command handler function type
using CommandHandler = std::function<CommandResult(const std::vector<std::string>& args, const std::shared_ptr<Config>& config, const std::shared_ptr<Brain>& brain)>;

// Advanced CLI class
class NLMCommandLine {
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::map<std::string, CommandHandler> commands;
    
public:
    NLMCommandLine(const std::shared_ptr<Config>& cfg, const std::shared_ptr<Brain>& br) 
        : config(cfg), brain(br) {
        initializeCommands();
    }
    
    void initializeCommands() {
        // Debug commands
        commands["debug-neurons"] = [this](const std::vector<std::string>& args, auto, auto) {
            return debugNeurons(args);
        };
        
        commands["debug-synapses"] = [this](const std::vector<std::string>& args, auto, auto) {
            return debugSynapses(args);
        };
        
        commands["debug-region"] = [this](const std::vector<std::string>& args, auto, auto) {
            return debugRegion(args);
        };
        
        // Analysis commands
        commands["stats"] = [this](const std::vector<std::string>& args, auto, auto) {
            return analyzeStats(args);
        };
        
        commands["learning-curve"] = [this](const std::vector<std::string>& args, auto, auto) {
            return analyzeLearningCurve(args);
        };
        
        commands["network-analysis"] = [this](const std::vector<std::string>& args, auto, auto) {
            return analyzeNetwork(args);
        };
        
        // Control commands
        commands["save"] = [this](const std::vector<std::string>& args, auto, auto) {
            return saveState(args);
        };
        
        commands["load"] = [this](const std::vector<std::string>& args, auto, auto) {
            return loadState(args);
        };
        
        commands["reset"] = [this](const std::vector<std::string>& args, auto, auto) {
            return resetBrain(args);
        };
        
        // Configuration commands
        commands["config"] = [this](const std::vector<std::string>& args, auto, auto) {
            return configCommand(args);
        };
        
        commands["config-set"] = [this](const std::vector<std::string>& args, auto, auto) {
            return configSet(args);
        };
        
        // Experiment control
        commands["run-experiment"] = [this](const std::vector<std::string>& args, auto, auto) {
            return runExperiment(args);
        };
        
        // Visualization and export
        commands["export-neurons"] = [this](const std::vector<std::string>& args, auto, auto) {
            return exportNeurons(args);
        };
        
        commands["export-synapses"] = [this](const std::vector<std::string>& args, auto, auto) {
            return exportSynapses(args);
        };
        
        commands["heatmap"] = [this](const std::vector<std::string>& args, auto, auto) {
            return generateHeatmap(args);
        };
        
        // System information
        commands["help"] = [this](const std::vector<std::string>& args, auto, auto) {
            return showHelp(args);
        };
        
        commands["version"] = [this](const std::vector<std::string>& args, auto, auto) {
            return showVersion(args);
        };
    }
    
    // Debug commands
    CommandResult debugNeurons(const std::vector<std::string>& args) {
        std::stringstream output;
        
        if (brain->getTotalNeuronCount() == 0) {
            return CommandResult(false, "", "No neurons in brain");
        }
        
        output << "=== NLM Neuron Debug Information ===" << std::endl;
        output << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        output << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        output << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        output << "Average firing rate: " << std::fixed << std::setprecision(2) 
               << brain->getAverageFiringRate() << " Hz" << std::endl;
        
        // Show first few neurons in detail
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t showCount = std::min<size_t>(10, neuronCount);
        
        for (size_t i = 0; i < showCount; ++i) {
            auto* region = brain->getRegion(brain->getRegionIds()[i % brain->getRegionCount()]);
            if (region) {
                auto neurons = region->getAllNeurons();
                if (!neurons.empty()) {
                    auto* neuron = neurons[i % neurons.size()];
                    output << std::endl << "Neuron " << i << ":" << std::endl;
                    output << "  Type: " << static_cast<int>(neuron->getState().firingState) << std::endl;
                    output << "  Membrane potential: " << std::setprecision(2) 
                           << neuron->getState().membranePotential << " mV" << std::endl;
                    output << "  Threshold: " << neuron->getState().threshold << " mV" << std::endl;
                    output << "  Refractory remaining: " << neuron->getState().refractoryRemaining << std::endl;
                    output << "  Last spike time: " << neuron->getLastSpikeTime() << std::endl;
                }
            }
        }
        
        return CommandResult(true, output.str());
    }
    
    CommandResult debugSynapses(const std::vector<std::string>& args) {
        std::stringstream output;
        
        if (brain->getTotalSynapseCount() == 0) {
            return CommandResult(false, "", "No synapses in brain");
        }
        
        output << "=== NLM Synapse Debug Information ===" << std::endl;
        output << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        
        // Analyze synapse properties
        float totalExcitatory = 0.0f;
        float totalInhibitory = 0.0f;
        size_t excitatoryCount = 0;
        size_t inhibitoryCount = 0;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto* syn : region->getSynapses()) {
                float weight = syn->getWeight();
                if (weight > 0) {
                    totalExcitatory += weight;
                    ++excitatoryCount;
                } else {
                    totalInhibitory += std::abs(weight);
                    ++inhibitoryCount;
                }
            }
        }
        
        output << "Excitatory synapses: " << excitatoryCount << std::endl;
        output << "Inhibitory synapses: " << inhibitoryCount << std::endl;
        output << "Total excitatory weight: " << std::setprecision(4) << totalExcitatory << std::endl;
        output << "Total inhibitory weight: " << std::setprecision(4) << totalInhibitory << std::endl;
        output << "E/I ratio: " << (totalInhibitory > 0 ? totalExcitatory / totalInhibitory : 
                                 totalExcitatory > 0 ? std::numeric_limits<float>::infinity() : 0.0f) << std::endl;
        
        return CommandResult(true, output.str());
    }
    
    CommandResult debugRegion(const std::vector<std::string>& args) {
        std::stringstream output;
        
        if (args.empty()) {
            return CommandResult(false, "", "Region ID required for region debug");
        }
        
        try {
            size_t regionId = std::stoul(args[0]);
            auto* region = brain->getRegion(static_cast<RegionId>(regionId));
            
            if (!region) {
                return CommandResult(false, "", "Region not found");
            }
            
            output << "=== NLM Region Debug Information ===" << std::endl;
            output << "Region ID: " << regionId << std::endl;
            output << "Region name: " << region->getName() << std::endl;
            output << "Total neurons: " << region->getTotalNeuronCount() << std::endl;
            output << "Total synapses: " << region->getSynapseCount() << std::endl;
            output << "Average synaptic weight: " << std::setprecision(4) 
                   << region->getAverageSynapticWeight() << std::endl;
            
            // Show population breakdown
            output << "\nPopulation breakdown:" << std::endl;
            for (const auto& pop : region->getPopulations()) {
                output << "  " << static_cast<int>(pop->getNeuronType()) << ": " 
                       << pop->getNeuronCount() << " neurons" << std::endl;
            }
            
        } catch (const std::exception& e) {
            return CommandResult(false, "", std::string("Error: ") + e.what());
        }
        
        return CommandResult(true, output.str());
    }
    
    // Analysis commands
    CommandResult analyzeStats(const std::vector<std::string>& args) {
        std::stringstream output;
        
        output << "=== NLM Brain Statistics ===" << std::endl;
        output << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        output << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        output << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        output << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        output << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        output << "Average firing rate: " << std::setprecision(2) 
               << brain->getAverageFiringRate() << " Hz" << std::endl;
        output << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        output << "Pending events: " << brain->getPendingSpikeEventCount() << std::endl;
        
        // Memory system stats
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            output << "Working memory traces: " << workingMem->getActiveTraces() << std::endl;
        }
        
        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            output << "Episodic memory episodes: " << episodicMem->getEpisodeCount() << std::endl;
        }
        
        return CommandResult(true, output.str());
    }
    
    CommandResult analyzeLearningCurve(const std::vector<std::string>& args) {
        std::stringstream output;
        output << "=== Learning Curve Analysis ===" << std::endl;
        output << "Note: Learning curve data requires tracking across simulation steps." << std::endl;
        output << "Current spike learning indicator: " 
               << (brain->getTotalSpikeCount() > 100 ? "Active" : "Minimal") << std::endl;
        output << "Total weight changes can be analyzed by comparing synaptic states over time." << std::endl;
        
        // Simple learning assessment based on current state
        if (brain->getTotalSynapseCount() > 0) {
            output << "Synapse weight statistics available for longitudinal analysis." << std::endl;
        }
        
        return CommandResult(true, output.str());
    }
    
    CommandResult analyzeNetwork(const std::vector<std::string>& args) {
        std::stringstream output;
        output << "=== Network Analysis ===" << std::endl;
        
        // Calculate network metrics
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        
        if (totalNeurons > 0) {
            float connectivityDensity = static_cast<float>(totalSynapses) / 
                                       (totalNeurons * totalNeurons);
            output << "Connectivity density: " << std::setprecision(6) << connectivityDensity << std::endl;
            
            // Estimate clustering coefficient (simplified)
            float avgClustering = 0.0f; // Would require detailed connectivity analysis
            output << "Estimated clustering coefficient: N/A (requires detailed analysis)" << std::endl;
            
            // Calculate path length (simplified)
            output << "Network path length: N/A (requires full connectivity matrix)" << std::endl;
        }
        
        // Region analysis
        size_t regionCount = brain->getRegionCount();
        if (regionCount > 1) {
            output << "Multi-region network detected." << std::endl;
            for (size_t i = 0; i < regionCount; ++i) {
                auto* region = brain->getRegion(brain->getRegionIds()[i]);
                if (region) {
                    output << "  Region " << i << ": " << region->getTotalNeuronCount() 
                           << " neurons, " << region->getSynapseCount() << " synapses" << std::endl;
                }
            }
        } else {
            output << "Single-region network." << std::endl;
        }
        
        return CommandResult(true, output.str());
    }
    
    // Control commands
    CommandResult saveState(const std::vector<std::string>& args) {
        if (args.empty()) {
            return CommandResult(false, "", "Filename required for save command");
        }
        
        std::string filepath = args[0];
        if (brain->save(filepath)) {
            return CommandResult(true, "Brain state saved to: " + filepath);
        } else {
            return CommandResult(false, "", "Failed to save brain state to " + filepath);
        }
    }
    
    CommandResult loadState(const std::vector<std::string>& args) {
        if (args.empty()) {
            return CommandResult(false, "", "Filename required for load command");
        }
        
        std::string filepath = args[0];
        if (brain->load(filepath)) {
            return CommandResult(true, "Brain state loaded from: " + filepath);
        } else {
            return CommandResult(false, "", "Failed to load brain state from " + filepath);
        }
    }
    
    CommandResult resetBrain(const std::vector<std::string>& args) {
        brain->reset();
        brain->initialize();
        return CommandResult(true, "Brain reset and reinitialized");
    }
    
    // Configuration commands
    CommandResult configCommand(const std::vector<std::string>& args) {
        std::stringstream output;
        output << "=== NLM Configuration ===" << std::endl;
        output << config->summary() << std::endl;
        return CommandResult(true, output.str());
    }
    
    CommandResult configSet(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            return CommandResult(false, "", "Config key and value required");
        }
        
        std::string key = args[0];
        std::string value = args[1];
        
        try {
            if (key == "random_seed") {
                config->set(key, std::stoll(value), ConfigSource::CommandLine);
            } else if (key == "neuron_count" || key == "region_count") {
                config->set(key, std::stoll(value), ConfigSource::CommandLine);
            } else if (key == "connection_probability" || key == "stdp_ltp_weight" || 
                       key == "stdp_ltd_weight" || key == "stdp_tau" ||
                       key == "synaptogenesis_rate" || key == "pruning_rate") {
                config->set(key, std::stof(value), ConfigSource::CommandLine);
            } else if (key == "simulation_timestep" || key == "reward_discount_factor") {
                config->set(key, std::stod(value), ConfigSource::CommandLine);
            } else if (key == "log_level") {
                config->set(key, value, ConfigSource::CommandLine);
            } else {
                return CommandResult(false, "", "Unknown config key: " + key);
            }
            
            return CommandResult(true, "Config " + key + " set to " + value);
        } catch (const std::exception& e) {
            return CommandResult(false, "", std::string("Error setting config: ") + e.what());
        }
    }
    
    // Experiment control
    CommandResult runExperiment(const std::vector<std::string>& args) {
        if (args.empty()) {
            return CommandResult(false, "", "Experiment name required");
        }
        
        std::string experimentName = args[0];
        std::stringstream output;
        output << "Running experiment: " << experimentName << std::endl;
        output << "This would run a predefined experiment with name: " << experimentName << std::endl;
        output << "For now, performing basic brain operation..." << std::endl;
        
        // For demonstration, run a simple plasticity experiment
        if (brain->getTotalSynapseCount() > 0) {
            output << "Enabling plasticity and running simple learning test..." << std::endl;
            // Enable plasticity
            for (const auto& region : brain->getRegions()) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);
                }
            }
            
            // Inject some input to stimulate learning
            for (size_t step = 0; step < 100; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
                brain->step(step, step * 0.001);
            }
            
            output << "Experiment complete. Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        } else {
            output << "No synapses available for experiment." << std::endl;
        }
        
        return CommandResult(true, output.str());
    }
    
    // Export commands
    CommandResult exportNeurons(const std::vector<std::string>& args) {
        if (args.empty()) {
            return CommandResult(false, "", "Filename required for export");
        }
        
        std::string filename = args[0];
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            return CommandResult(false, "", "Cannot open file for writing: " + filename);
        }
        
        file << "Neuron Export" << std::endl;
        file << "==============" << std::endl;
        file << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        
        for (size_t i = 0; i < brain->getTotalNeuronCount(); ++i) {
            file << "Neuron " << i << ": Not implemented in current version" << std::endl;
        }
        
        file.close();
        return CommandResult(true, "Neurons exported to " + filename);
    }
    
    CommandResult exportSynapses(const std::vector<std::string>& args) {
        if (args.empty()) {
            return CommandResult(false, "", "Filename required for export");
        }
        
        std::string filename = args[0];
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            return CommandResult(false, "", "Cannot open file for writing: " + filename);
        }
        
        file << "Synapse Export" << std::endl;
        file << "===============" << std::endl;
        file << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto* syn : region->getSynapses()) {
                file << syn->getSourceNeuron().index() << " -> " 
                     << syn->getDestinationNeuron().index() << " : " 
                     << syn->getWeight() << " (type: " << static_cast<int>(syn->getType()) << ")" << std::endl;
            }
        }
        
        file.close();
        return CommandResult(true, "Synapses exported to " + filename);
    }
    
    CommandResult generateHeatmap(const std::vector<std::string>& args) {
        std::stringstream output;
        output << "=== Heatmap Generation ===" << std::endl;
        output << "Generating neural activity heatmap..." << std::endl;
        output << "This feature requires matplotlib or similar library for visualization." << std::endl;
        output << "Current neuron activity levels can be analyzed for visualization." << std::endl;
        
        return CommandResult(true, output.str());
    }
    
    // System information commands
    CommandResult showHelp(const std::vector<std::string>& args) {
        std::stringstream output;
        output << "=== NLM Command Line Help ===" << std::endl;
        output << "Available commands:" << std::endl;
        output << "  debug-neurons [args]   - Show neuron debug information" << std::endl;
        output << "  debug-synapses [args]  - Show synapse debug information" << std::endl;
        output << "  debug-region <id>      - Debug specific region" << std::endl;
        output << "  stats                  - Show brain statistics" << std::endl;
        output << "  learning-curve         - Analyze learning progress" << std::endl;
        output << "  network-analysis       - Analyze network properties" << std::endl;
        output << "  save <file>           - Save brain state to file" << std::endl;
        output << "  load <file>           - Load brain state from file" << std::endl;
        output << "  reset                 - Reset brain to initial state" << std::endl;
        output << "  config                - Show current configuration" << std::endl;
        output << "  config-set <k> <v>    - Set configuration value" << std::endl;
        output << "  run-experiment <name> - Run predefined experiment" << std::endl;
        output << "  export-neurons <file> - Export neurons to file" << std::endl;
        output << "  export-synapses <file> - Export synapses to file" << std::endl;
        output << "  heatmap                - Generate activity heatmap" << std::endl;
        output << "  help                  - Show this help message" << std::endl;
        output << "  version               - Show NLM version information" << std::endl;
        output << "" << std::endl;
        output << "Examples:" << std::endl;
        output << "  nlmc debug-neurons" << std::endl;
        output << "  nlmc debug-synapses" << std::endl;
        output << "  nlmc stats" << std::endl;
        output << "  nlmc save brain_state.json" << std::endl;
        output << "  nlmc load brain_state.json" << std::endl;
        output << "  nlmc config-set neuron_count 2000" << std::endl;
        
        return CommandResult(true, output.str());
    }
    
    CommandResult showVersion(const std::vector<std::string>& args) {
        std::stringstream output;
        output << "NLM (熙然) - Neural Learning Machine" << std::endl;
        output << "Version: 1.0.0 (Phase 6 - Integrated Artificial Brain)" << std::endl;
        output << "Built with C++20, SIMD optimization, and Python bindings" << std::endl;
        output << "Advanced neural computation framework with memory, cognition, and neuromodulation" << std::endl;
        return CommandResult(true, output.str());
    }
    
public:
    void processCommandLine(int argc, char** argv) {
        if (argc < 2) {
            std::cout << "Usage: nlmc <command> [args]" << std::endl;
            std::cout << "Use 'nlmc help' for available commands." << std::endl;
            return;
        }
        
        std::string command = argv[1];
        std::vector<std::string> args;
        
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        auto it = commands.find(command);
        if (it == commands.end()) {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Use 'nlmc help' for available commands." << std::endl;
            return;
        }
        
        CommandResult result = it->second(args);
        
        if (!result.success) {
            std::cerr << "Error: " << result.error << std::endl;
        }
        
        if (!result.output.empty()) {
            std::cout << result.output << std::endl;
        }
    }
};

} // namespace nlmc

// Global CLI instance
nlmc::NLMCommandLine* g_cli = nullptr;

// Integration function for main.cpp
void initializeCLI(const std::shared_ptr<Config>& config, const std::shared_ptr<Brain>& brain) {
    g_cli = new nlmc::NLMCommandLine(config, brain);
}

void processCommandLine(int argc, char** argv) {
    if (g_cli) {
        g_cli->processCommandLine(argc, argv);
    }
}

// Cleanup
void cleanupCLI() {
    if (g_cli) {
        delete g_cli;
        g_cli = nullptr;
    }
}