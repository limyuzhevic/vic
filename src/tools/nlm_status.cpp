// nlm_status - Show current system state
// Command-line tool for displaying NLM system state

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <sstream>

using namespace nlm;

void printUsage(const std::string& programName) {
    std::cout << "nlm_status - Show NLM system state" << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                  Show this help message" << std::endl;
    std::cout << "  --config <file>             Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --verbose, -v               Enable verbose output" << std::endl;
    std::cout << "  --detailed, -d              Show detailed system information" << std::endl;
    std::cout << "  --format <format>           Output format: text, json, yaml" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                                    Show current system state" << std::endl;
    std::cout << "  " << programName << " --detailed                         Show detailed information" << std::endl;
    std::cout << "  " << programName << " --verbose                         Show verbose information" << std::endl;
}

void brainToJSON(const std::shared_ptr<Brain>& brain, std::ostringstream& json) {
    json << "{\n";
    json << "  \"brain_info\": {\n";
    json << "    \"neurons\": " << brain->getTotalNeuronCount() << ",\n";
    json << "    \"synapses\": " << brain->getTotalSynapseCount() << ",\n";
    json << "    \"regions\": " << brain->getRegionCount() << ",\n";
    json << "    \"active_neurons\": " << brain->getActiveNeuronCount() << ",\n";
    json << "    \"firing_neurons\": " << brain->getFiringNeuronCount() << ",\n";
    json << "    \"total_spikes\": " << brain->getTotalSpikeCount() << ",\n";
    json << "    \"pending_spike_events\": " << brain->getPendingSpikeEventCount() << ",\n";
    json << "    \"average_firing_rate\": " << brain->getAverageFiringRate() << ",\n";
    json << "    \"e_i_ratio\": " << brain->getExcitationInhibitionRatio() << "\n";
    json << "  },\n";

    auto config = brain->getConfig();
    if (config) {
        json << "  \"configuration\": {\n";
        json << "    \"random_seed\": " << config->getOr<int64_t>("random_seed", 42) << ",\n";
        json << "    \"simulation_timestep\": " << config->getOr<double>("simulation_timestep", 0.001) << ",\n";
        json << "    \"neuron_count\": " << config->getOr<int64_t>("neuron_count", 1000) << ",\n";
        json << "    \"region_count\": " << config->getOr<int64_t>("region_count", 2) << ",\n";
        json << "    \"connection_probability\": " << config->getOr<float>("connection_probability", 0.1f) << "\n";
        json << "  },\n";
    }

    json << "  \"development_stage\": " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
    json << "}\n";
}

int main(int argc, char* argv[]) {
    std::string configFile = "configs/default.cfg";
    bool verbose = false;
    bool detailed = false;
    std::string format = "text";

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--detailed" || arg == "-d") {
            detailed = true;
        } else if (arg == "--format") {
            if (i + 1 < argc) {
                format = argv[++i];
            }
        } else if (!arg.starts_with("--")) {
            // Extra argument, ignore or error
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    std::cout << "=== NLM System Status ===" << std::endl;
    std::cout << "Loading brain from configuration: " << configFile << std::endl;

    // Load configuration
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(configFile)) {
        std::cout << "✓ Configuration loaded successfully" << std::endl;
    } else {
        std::cout << "⚠ Using default configuration" << std::endl;
    }

    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "✗ Failed to initialize brain" << std::endl;
        return 1;
    }

    std::cout << "✓ Brain initialized successfully" << std::endl;

    if (format == "json") {
        std::ostringstream json;
        brainToJSON(brain, json);
        std::cout << json.str();
    } else if (format == "text") {
        std::cout << "\n--- Core System Information ---" << std::endl;
        std::cout << "System: NLM Phase 6 (Final Integration)" << std::endl;
        std::cout << "Brain initialized successfully with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        std::cout << "Brain contains " << brain->getTotalSynapseCount() << " synapses across " << brain->getRegionCount() << " regions" << std::endl;

        std::cout << "\n--- Brain Status ---" << std::endl;
        brain->logStatus();

        if (detailed) {
            std::cout << "\n--- Development Stage ---" << std::endl;
            std::cout << "Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;

            std::cout << "\n--- Memory Systems ---" << std::endl;
            auto* workingMem = brain->getWorkingMemory();
            auto* episodicMem = brain->getEpisodicMemory();
            auto* associativeMem = brain->getAssociativeMemory();

            if (workingMem) {
                std::cout << "Working memory capacity: " << workingMem->getCapacity() << " traces" << std::endl;
                std::cout << "Active traces: " << workingMem->getActiveTraces() << std::endl;
            }
            if (episodicMem) {
                std::cout << "Episodic memory max episodes: " << episodicMem->getMaxEpisodes() << std::endl;
                std::cout << "Current episodes: " << episodicMem->getEpisodeCount() << std::endl;
            }

            std::cout << "\n--- Neuromodulation ---" << std::endl;
            auto* dopamine = brain->getDopamine();
            auto* curiosity = brain->getCuriosity();
            auto* novelty = brain->getNovelty();
            auto* predictionError = brain->getPredictionErrorSignal();

            if (dopamine) {
                std::cout << "Dopamine level: " << dopamine->getLevel() << std::endl;
                std::cout << "Dopamine plasticity factor: " << dopamine->getPlasticityFactor() << std::endl;
            }
            if (curiosity) {
                std::cout << "Curiosity level: " << curiosity->getLevel() << std::endl;
            }
            if (novelty) {
                std::cout << "Novelty level: " << novelty->getLevel() << std::endl;
            }
            if (predictionError) {
                std::cout << "Prediction error level: " << predictionError->getLevel() << std::endl;
            }

            std::cout << "\n--- System Health ---" << std::endl;
            std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
            std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "Pending spike events: " << brain->getPendingSpikeEventCount() << std::endl;
            std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        }
    } else {
        std::cout << "Unsupported format: " << format << std::endl;
        std::cout << "Only 'text' and 'json' formats are currently supported." << std::endl;
        return 1;
    }

    std::cout << "\n=== System Status Complete ===" << std::endl;
    return 0;
}
