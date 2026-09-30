// nlm_metrics - Display performance metrics
// Command-line tool for displaying NLM performance metrics

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>

using namespace nlm;

void printUsage(const std::string& programName) {
    std::cout << "nlm_metrics - Display NLM performance metrics" << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                  Show this help message" << std::endl;
    std::cout << "  --config <file>             Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --format <format>           Output format: text, csv, json" << std::endl;
    std::cout << "  --interval <seconds>        Interval between metric updates (0 = single shot)" << std::endl;
    std::cout << "  --duration <seconds>        Duration to monitor metrics (0 = unlimited)" << std::endl;
    std::cout << "  --verbose, -v               Enable verbose output" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                                    Show current metrics" << std::endl;
    std::cout << "  " << programName << " --interval 30                      Update metrics every 30 seconds" << std::endl;
    std::cout << "  " << programName << " --format csv                    Output metrics in CSV format" << std::endl;
    std::cout << "  " << programName << " --duration 300                   Monitor for 5 minutes" << std::endl;
}

void printMetrics(const std::shared_ptr<Brain>& brain, std::ostringstream& output, const std::string& format) {
    auto startTime = std::chrono::system_clock::now();
    auto currentTime = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

    if (format == "csv") {
        output << "timestamp,neurons,synapses,active_neurons,firing_neurons,total_spikes,pending_spike_events,average_firing_rate,e_i_ratio,development_stage" << std::endl;
        output << std::chrono::system_clock::now().time_since_epoch().count() << ",";
        output << brain->getTotalNeuronCount() << ",";
        output << brain->getTotalSynapseCount() << ",";
        output << brain->getActiveNeuronCount() << ",";
        output << brain->getFiringNeuronCount() << ",";
        output << brain->getTotalSpikeCount() << ",";
        output << brain->getPendingSpikeEventCount() << ",";
        output << brain->getAverageFiringRate() << ",";
        output << brain->getExcitationInhibitionRatio() << ",";
        output << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    } else if (format == "json") {
        output << "{\n";
        output << "  \"timestamp\": \"" << std::chrono::system_clock::now().time_since_epoch().count() << "\",\n";
        output << "  \"neurons\": " << brain->getTotalNeuronCount() << ",\n";
        output << "  \"synapses\": " << brain->getTotalSynapseCount() << ",\n";
        output << "  \"active_neurons\": " << brain->getActiveNeuronCount() << ",\n";
        output << "  \"firing_neurons\": " << brain->getFiringNeuronCount() << ",\n";
        output << "  \"total_spikes\": " << brain->getTotalSpikeCount() << ",\n";
        output << "  \"pending_spike_events\": " << brain->getPendingSpikeEventCount() << ",\n";
        output << "  \"average_firing_rate\": " << brain->getAverageFiringRate() << ",\n";
        output << "  \"e_i_ratio\": " << brain->getExcitationInhibitionRatio() << ",\n";
        output << "  \"development_stage\": " << static_cast<int>(brain->getDevelopmentalStage()) << ",\n";

        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            output << "  \"working_memory_traces\": " << workingMem->getActiveTraces() << ",\n";
        }

        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            output << "  \"episodic_memory_episodes\": " << episodicMem->getEpisodeCount() << ",\n";
        }

        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            output << "  \"dopamine_level\": " << dopamine->getLevel() << "\n";
        } else {
            output << "  \"dopamine_level\": null\n";
        }

        output << "}\n";
    } else { // text format
        auto currentTime = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
        output << "=== NLM Performance Metrics ===" << std::endl;
        output << "Timestamp: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
        output << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
        output << "" << std::endl;

        output << "--- Core Performance ---" << std::endl;
        output << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        output << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        output << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        output << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        output << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        output << "Pending spike events: " << brain->getPendingSpikeEventCount() << std::endl;
        output << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        output << "Excitatory/Inhibitory ratio: " << brain->getExcitationInhibitionRatio() << std::endl;

        output << "\n--- Memory System Performance ---" << std::endl;
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            output << "Working memory capacity: " << workingMem->getCapacity() << " traces" << std::endl;
            output << "Active memory traces: " << workingMem->getActiveTraces() << std::endl;
            output << "Working memory utilization: " << std::fixed << std::setprecision(2) << (workingMem->getActiveTraces() * 100.0 / workingMem->getCapacity()) << "%" << std::endl;
        }

        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            output << "Episodic memory max episodes: " << episodicMem->getMaxEpisodes() << std::endl;
            output << "Current episodes: " << episodicMem->getEpisodeCount() << std::endl;
            if (episodicMem->getMaxEpisodes() > 0) {
                output << "Episodic memory utilization: " << std::fixed << std::setprecision(2) << (episodicMem->getEpisodeCount() * 100.0 / episodicMem->getMaxEpisodes()) << "%" << std::endl;
            }
        }

        output << "\n--- Neuromodulation Performance ---" << std::endl;
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            output << "Dopamine level: " << dopamine->getLevel() << std::endl;
            output << "Dopamine plasticity factor: " << dopamine->getPlasticityFactor() << std::endl;
        }

        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            output << "Curiosity level: " << curiosity->getLevel() << std::endl;
        }

        output << "\n--- Development Status ---" << std::endl;
        output << "Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;

        output << "\n--- System Health ---" << std::endl;
        double firingRate = brain->getAverageFiringRate();
        if (firingRate > 0) {
            output << "Firing rate (normalized): " << std::fixed << std::setprecision(4) << firingRate << std::endl;
        }

        double eiRatio = brain->getExcitationInhibitionRatio();
        if (eiRatio > 0) {
            output << "E/I ratio (normalized): " << std::fixed << std::setprecision(4) << eiRatio << std::endl;
        }

        output << "\n=== Metrics Collection Complete ===" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::string configFile = "configs/default.cfg";
    std::string format = "text";
    int interval = 0;  // 0 = single shot
    int duration = 0;  // 0 = unlimited
    bool verbose = false;

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
        } else if (arg == "--format") {
            if (i + 1 < argc) {
                format = argv[++i];
            }
        } else if (arg == "--interval") {
            if (i + 1 < argc) {
                interval = std::stoi(argv[++i]);
            }
        } else if (arg == "--duration") {
            if (i + 1 < argc) {
                duration = std::stoi(argv[++i]);
            }
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
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

    std::cout << "=== NLM Brain Performance Metrics ===" << std::endl;
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

    auto startTime = std::chrono::system_clock::now();

    if (interval > 0) {
        std::cout << "Monitoring metrics every " << interval << " seconds" << std::endl;
        if (duration > 0) {
            std::cout << "Monitoring for " << duration << " seconds" << std::endl;
        } else {
            std::cout << "Press Ctrl+C to stop monitoring" << std::endl;
        }

        try {
            while (true) {
                // Print metrics
                std::ostringstream output;
                printMetrics(brain, output, format);
                std::cout << output.str();

                // Sleep for specified interval
                std::this_thread::sleep_for(std::chrono::seconds(interval));

                // Check duration limit
                auto currentTime = std::chrono::system_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
                if (duration > 0 && elapsed.count() >= duration) {
                    break;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Monitoring stopped (exception: " << e.what() << ")" << std::endl;
        }

        std::cout << "\n=== Monitoring Complete ===" << std::endl;
    } else {
        // Single shot mode
        std::ostringstream output;
        printMetrics(brain, output, format);
        std::cout << output.str();
    }

    return 0;
}
