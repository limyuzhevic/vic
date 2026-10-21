#!/usr/bin/env g++
// NLM Plot Tool
// Generate visualization plots from simulation data

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <unordered_map>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

using namespace nlm;

struct PlotData {
    std::vector<double> xValues;
    std::vector<double> yValues;
    std::string title;
    std::string xLabel;
    std::string yLabel;
    std::string format;
    std::string outputFile;
};

class NLMPlot {
private:
    std::vector<PlotData> plotData;
    std::shared_ptr<Config> config;
    bool savePlots;
    std::string outputDirectory;
    bool showConsoleOutput;
    
public:
    NLMPlot() {
        config = std::make_shared<Config>();
        config->loadFromFile("configs/default.cfg");
        savePlots = false;
        outputDirectory = "plots";
        showConsoleOutput = true;
        
        // Create output directory
        if (!fs::exists(outputDirectory)) {
            fs::create_directory(outputDirectory);
        }
    }
    
    void generateBrainStatePlot() {
        std::cout << "=== Generating Brain State Plot ===" << std::endl;
        
        // Create a brain for demonstration
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        PlotData plot;
        plot.title = "Brain State Over Time";
        plot.xLabel = "Time (steps)";
        plot.yLabel = "Neuron Activity";
        plot.format = "text";
        
        // Collect data
        size_t maxSteps = 100;
        for (size_t step = 0; step <= maxSteps; ++step) {
            // Simulate some activity
            if (step % 10 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Excitatory, 20.0f);
            }
            
            brain->step(step, step * 0.001);
            
            double activity = (brain->getFiringNeuronCount() / (double)brain->getTotalNeuronCount()) * 100.0;
            plot.xValues.push_back(step);
            plot.yValues.push_back(activity);
        }
        
        plotData.push_back(plot);
        savePlot(plot);
        std::cout << "Brain state plot generated and saved." << std::endl;
    }
    
    void generateSpikeActivityPlot() {
        std::cout << "=== Generating Spike Activity Plot ===" << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        PlotData plot;
        plot.title = "Spike Activity Over Time";
        plot.xLabel = "Simulation Time (s)";
        plot.yLabel = "Total Spikes";
        plot.format = "text";
        
        // Collect spike data
        size_t maxSteps = 200;
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t step = 0; step <= maxSteps; ++step) {
            // Inject stimuli at various times
            if (step % 15 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            size_t currentSpikes = brain->getTotalSpikeCount();
            double spikeCount = currentSpikes - initialSpikes;
            
            plot.xValues.push_back(step * 0.001);
            plot.yValues.push_back(spikeCount);
        }
        
        plotData.push_back(plot);
        savePlot(plot);
        std::cout << "Spike activity plot generated and saved." << std::endl;
    }
    
    void generatePlasticityPlot() {
        std::cout << "=== Generating Plasticity Plot ===" << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Enable plasticity
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        PlotData plot;
        plot.title = "Plasticity Dynamics Over Time";
        plot.xLabel = "Time (steps)";
        plot.yLabel = "Average Synaptic Weight";
        plot.format = "text";
        
        // Collect plasticity data
        size_t maxSteps = 300;
        
        for (size_t step = 0; step <= maxSteps; ++step) {
            // Apply learning patterns
            if (step % 20 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
            }
            
            brain->step(step, step * 0.001);
            
            double avgWeight = getAverageSynapticWeight(brain);
            
            plot.xValues.push_back(step);
            plot.yValues.push_back(avgWeight);
        }
        
        plotData.push_back(plot);
        savePlot(plot);
        std::cout << "Plasticity plot generated and saved." << std::endl;
    }
    
    void generateMemoryPlot() {
        std::cout << "=== Generating Memory Plot ===" << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        PlotData plot;
        plot.title = "Memory System Activity";
        plot.xLabel = "Time (steps)";
        plot.yLabel = "Memory Utilization";
        plot.format = "text";
        
        // Collect memory data
        size_t maxSteps = 150;
        
        for (size_t step = 0; step <= maxSteps; ++step) {
            // Access memory systems
            auto* workingMemory = brain->getWorkingMemory();
            auto* episodicMemory = brain->getEpisodicMemory();
            
            double memoryUsage = 0.0;
            if (workingMemory) {
                memoryUsage += 50.0;
            }
            if (episodicMemory) {
                memoryUsage += 30.0;
            }
            
            plot.xValues.push_back(step);
            plot.yValues.push_back(memoryUsage);
            
            // Simulate memory consolidation occasionally
            if (step % 30 == 0 && step > 0) {
                logMemoryConsolidation(brain);
            }
        }
        
        plotData.push_back(plot);
        savePlot(plot);
        std::cout << "Memory plot generated and saved." << std::endl;
    }
    
    void generateBenchmarkPlot() {
        std::cout << "=== Generating Benchmark Plot ===" << std::endl;
        
        PlotData plot;
        plot.title = "NLM Benchmark Performance";
        plot.xLabel = "Configuration Size";
        plot.yLabel = "Execution Time (ms)";
        plot.format = "text";
        
        // Simulate benchmark data
        std::vector<double> configSizes = {100, 500, 1000, 2000, 5000};
        std::vector<double> executionTimes = {50.0, 250.0, 500.0, 1200.0, 3500.0};
        
        for (size_t i = 0; i < configSizes.size(); ++i) {
            plot.xValues.push_back(configSizes[i]);
            plot.yValues.push_back(executionTimes[i]);
        }
        
        plotData.push_back(plot);
        savePlot(plot);
        std::cout << "Benchmark plot generated and saved." << std::endl;
    }
    
    void savePlot(const PlotData& plot) {
        std::string filename = outputDirectory + "/" + sanitizeFilename(plot.title) + ".txt";
        
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "# " << plot.title << std::endl;
            file << "# X-Axis: " << plot.xLabel << std::endl;
            file << "# Y-Axis: " << plot.yLabel << std::endl;
            file << "# Format: " << plot.format << std::endl;
            file << "# Generated: " << time(0) << std::endl;
            file << std::endl;
            file << "# Time Series Data:" << std::endl;
            
            for (size_t i = 0; i < plot.xValues.size(); ++i) {
                file << plot.xValues[i] << " " << plot.yValues[i] << std::endl;
            }
            
            file.close();
            
            if (showConsoleOutput) {
                std::cout << "  Plot saved to: " << filename << std::endl;
                std::cout << "  Data points: " << plot.xValues.size() << std::endl;
            }
        } else {
            std::cerr << "Error: Cannot create plot file: " << filename << std::endl;
        }
    }
    
    void saveAllPlots() {
        std::cout << "=== Saving All Plots ===" << std::endl;
        
        for (const auto& plot : plotData) {
            savePlot(plot);
        }
        
        std::cout << "All plots saved successfully to: " << outputDirectory << std::endl;
    }
    
    double getAverageSynapticWeight(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().empty()) {
            return 0.0;
        }
        
        double totalWeight = 0.0;
        for (const auto& syn : region->getSynapses()) {
            totalWeight += syn->getWeight();
        }
        
        return totalWeight / region->getSynapses().size();
    }
    
    void logMemoryConsolidation(std::shared_ptr<Brain> brain) {
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "Memory consolidation at step " << std::endl;
        }
    }
    
    std::string sanitizeFilename(const std::string& filename) {
        std::string sanitized = filename;
        // Replace spaces with underscores
        std::replace(sanitized.begin(), sanitized.end(), ' ', '_');
        // Remove special characters
        sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(),
            [](char c) { return !std::isalnum(c) && c != '_'; }), sanitized.end());
        return sanitized;
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --brain-state            Generate brain state plot" << std::endl;
        std::cout << "  --spike-activity         Generate spike activity plot" << std::endl;
        std::cout << "  --plasticity             Generate plasticity plot" << std::endl;
        std::cout << "  --memory                 Generate memory plot" << std::endl;
        std::cout << "  --benchmark              Generate benchmark plot" << std::endl;
        std::cout << "  --all                    Generate all plots" << std::endl;
        std::cout << "  --output <dir>           Set output directory" << std::endl;
        std::cout << "  --console-off            Disable console output" << std::endl;
        std::cout << "  --save                   Save plots to disk" << std::endl;
        std::cout << "  --config <file>          Load configuration file" << std::endl;
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-plot";
    bool brainStatePlot = false;
    bool spikeActivityPlot = false;
    bool plasticityPlot = false;
    bool memoryPlot = false;
    bool benchmarkPlot = false;
    bool allPlots = false;
    bool saveAll = false;
    bool consoleOutput = true;
    std::string outputDir = "plots";
    std::string configFile;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            NLMPlot plotter;
            plotter.printUsage(programName);
            return 0;
        } else if (arg == "--brain-state") {
            brainStatePlot = true;
        } else if (arg == "--spike-activity") {
            spikeActivityPlot = true;
        } else if (arg == "--plasticity") {
            plasticityPlot = true;
        } else if (arg == "--memory") {
            memoryPlot = true;
        } else if (arg == "--benchmark") {
            benchmarkPlot = true;
        } else if (arg == "--all") {
            allPlots = true;
        } else if (arg == "--save") {
            saveAll = true;
        } else if (arg == "--console-off") {
            consoleOutput = false;
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                outputDir = argv[++i];
            } else {
                std::cerr << "Error: --output requires a directory argument" << std::endl;
                return 1;
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a filename argument" << std::endl;
                return 1;
            }
        }
    }
    
    NLMPlot plotter;
    plotter.showConsoleOutput = consoleOutput;
    plotter.outputDirectory = outputDir;
    
    if (!configFile.empty()) {
        if (!plotter.config->loadFromFile(configFile)) {
            std::cerr << "Error: Failed to load configuration file: " << configFile << std::endl;
            return 1;
        }
    }
    
    if (allPlots) {
        plotter.generateBrainStatePlot();
        plotter.generateSpikeActivityPlot();
        plotter.generatePlasticityPlot();
        plotter.generateMemoryPlot();
        plotter.generateBenchmarkPlot();
    } else {
        if (brainStatePlot) {
            plotter.generateBrainStatePlot();
        }
        if (spikeActivityPlot) {
            plotter.generateSpikeActivityPlot();
        }
        if (plasticityPlot) {
            plotter.generatePlasticityPlot();
        }
        if (memoryPlot) {
            plotter.generateMemoryPlot();
        }
        if (benchmarkPlot) {
            plotter.generateBenchmarkPlot();
        }
    }
    
    if (saveAll) {
        plotter.saveAllPlots();
    }
    
    if (!allPlots && !brainStatePlot && !spikeActivityPlot && !plasticityPlot && 
        !memoryPlot && !benchmarkPlot && !saveAll) {
        NLMPlot tempPlotter;
        tempPlotter.printUsage(programName);
    }
    
    return 0;
}
