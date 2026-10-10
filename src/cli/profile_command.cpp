// NLM Advanced CLI - Profile Command Implementation
#pragma once

#include "cli/main_cli.hpp"
#include "cli/command_factory.hpp"
#include "brain/Brain.hpp"
#include "tools/debug.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

namespace nlm {

class ProfileCommand : public BaseCommand {
public:
    ProfileCommand() : BaseCommand("profile", "Performance profiling and benchmarking") {}
    
protected:
    CLIResult executeImpl(const std::map<std::string, std::string>& options) override {
        if (options.empty()) {
            displayHelpImpl();
            return CLIResult::HelpDisplayed;
        }
        
        std::string action = getOption(options, "action", "benchmark");
        
        if (action == "benchmark") {
            std::string durationStr = getOption(options, "duration", "60");
            std::string stepsStr = getOption(options, "steps", "0");
            bool detailed = getOption(options, "detailed", "false") == "true" || 
                           getOption(options, "detailed", "false") == "1" || 
                           getOption(options, "detailed", "false") == "yes";
            return handleBenchmark(std::stoi(durationStr), std::stoi(stepsStr), detailed);
        } else if (action == "validate") {
            return handleValidation();
        } else if (action == "analyze") {
            std::string profileFile = getOption(options, "profile", "");
            return handleAnalyze(profileFile);
        } else if (action == "compare") {
            std::string baseline = getOption(options, "baseline", "");
            return handleCompare(baseline);
        } else {
            std::cout << "Unknown profile action: " << action << std::endl;
            displayHelpImpl();
            return CLIResult::Error;
        }
    }
    
    void displayHelpImpl() const override {
        std::cout << "\n=== Profile Command Help ===\n" << std::endl;
        std::cout << "Usage: nlm profile [options]\n" << std::endl;
        std::cout << "Actions:" << std::endl;
        std::cout << "  --action=benchmark [--duration=<seconds>] [--steps=<n>] [--detailed]\n" << std::endl;
        std::cout << "  --action=validate\n" << std::endl;
        std::cout << "  --action=analyze [--profile=<file>]\n" << std::endl;
        std::cout << "  --action=compare [--baseline=<file>]\n" << std::endl;
        std::cout << "\nOptions:" << std::endl;
        std::cout << "  --duration=<seconds>   Benchmark duration in seconds (default: 60)" << std::endl;
        std::cout << "  --steps=<n>            Number of simulation steps to run (0 = until complete)" << std::endl;
        std::cout << "  --detailed             Enable detailed performance analysis" << std::endl;
        std::cout << "  --profile=<file>       Profile data file for analysis" << std::endl;
        std::cout << "  --baseline=<file>      Baseline profile file for comparison"" << std::endl;
    }
    
private:
    CLIResult handleBenchmark(int duration, int steps, bool detailed) {
        std::cout << "=== Performance Benchmark ===\n" << std::endl;
        std::cout << "Running performance benchmark for " << duration << " seconds" << std::endl;
        if (steps > 0) {
            std::cout << " (" << steps << " simulation steps)" << std::endl;
        }
        std::cout << "Detailed mode: " << (detailed ? "Yes" : "No") << std::endl;
        std::cout << "\nStarting benchmark...\n" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto endTime = startTime + std::chrono::seconds(duration);
        
        int stepCount = 0;
        int spikeCount = 0;
        std::vector<double> fpsHistory;
        
        while (std::chrono::high_resolution_clock::now() < endTime && globalState.running) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            if (globalState.brain) {
                globalState.brain->step(globalState.simulationStep, globalState.simulationTime);
                globalState.simulationStep++;
                globalState.simulationTime += 0.001;
                
                stepCount++;
                spikeCount += globalState.brain->getFiringNeuronCount();
                
                // Record FPS
                auto stepEnd = std::chrono::high_resolution_clock::now();
                auto stepDuration = std::chrono::duration_cast<std::chrono::duration<double>>(stepEnd - stepStart);
                if (stepDuration.count() > 0) {
                    fpsHistory.push_back(1.0 / stepDuration.count());
                }
            }
            
            // Sleep a bit to avoid hogging CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
            if (globalState.debugTools) {
                globalState.debugTools->logEvent("Benchmark step " + std::to_string(stepCount));
            }
        }
        
        auto endBenchmark = std::chrono::high_resolution_clock::now();
        double totalDuration = std::chrono::duration_cast<std::chrono::duration<double>>(endBenchmark - startTime).count();
        
        std::cout << "\n=== Benchmark Results ===\n" << std::endl;
        std::cout << "Total duration: " << totalDuration << " seconds" << std::endl;
        std::cout << "Total simulation steps: " << stepCount << std::endl;
        std::cout << "Total spikes generated: " << spikeCount << std::endl;
        
        if (!fpsHistory.empty()) {
            double avgFps = std::accumulate(fpsHistory.begin(), fpsHistory.end(), 0.0) / fpsHistory.size();
            double maxFps = *std::max_element(fpsHistory.begin(), fpsHistory.end());
            double minFps = *std::min_element(fpsHistory.begin(), fpsHistory.end());
            
            std::cout << "Average FPS: " << avgFps << std::endl;
            std::cout << "Max FPS: " << maxFps << std::endl;
            std::cout << "Min FPS: " << minFps << std::endl;
        }
        
        if (globalState.brain) {
            std::cout << "Final brain state:" << std::endl;
            std::cout << "  Neurons: " << globalState.brain->getTotalNeuronCount() << std::endl;
            std::cout << "  Synapses: " << globalState.brain->getTotalSynapseCount() << std::endl;
            std::cout << "  Firing neurons: " << globalState.brain->getFiringNeuronCount() << std::endl;
            std::cout << "  Total spikes: " << globalState.brain->getTotalSpikeCount() << std::endl;
            std::cout << "  Development stage: " << static_cast<int>(globalState.brain->getDevelopmentalStage()) << std::endl;
        }
        
        return CLIResult::Success;
    }
    
    CLIResult handleValidation() {
        std::cout << "=== Brain System Validation ===\n" << std::endl;
        std::cout << "Validating brain system integrity...\n" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // Basic validation checks
        if (globalState.brain->getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons");
        }
        
        if (globalState.brain->getTotalSynapseCount() == 0) {
            warnings.push_back("Brain has no synapses (might be expected)");
        }
        
        if (globalState.brain->getFiringNeuronCount() == 0) {
            warnings.push_back("No neurons are currently firing");
        }
        
        // Check for NaN or invalid values
        // This would require checking individual neuron/synapse parameters
        
        if (!errors.empty()) {
            std::cout << "VALIDATION FAILED:" << std::endl;
            for (const auto& error : errors) {
                std::cout << "  ERROR: " << error << std::endl;
            }
            return CLIResult::Error;
        }
        
        std::cout << "VALIDATION PASSED:" << std::endl;
        for (const auto& warning : warnings) {
            std::cout << "  WARNING: " << warning << std::endl;
        }
        
        if (warnings.empty()) {
            std::cout << "  No warnings." << std::endl;
        }
        
        return CLIResult::Success;
    }
    
    CLIResult handleAnalyze(const std::string& profileFile) {
        std::cout << "=== Brain State Analysis ===\n" << std::endl;
        
        if (!globalState.brain) {
            std::cout << "Error: Brain not initialized." << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "Analyzing brain state...\n" << std::endl;
        
        // Basic analysis
        std::cout << "Brain Analysis:" << std::endl;
        std::cout << "  Total neurons: " << globalState.brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << globalState.brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Firing neurons: " << globalState.brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Total spikes: " << globalState.brain->getTotalSpikeCount() << std::endl;
        std::cout << "  E/I ratio: " << globalState.brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "  Development stage: " << static_cast<int>(globalState.brain->getDevelopmentalStage()) << std::endl;
        std::cout << "  Plasticity enabled: " << (globalState.brain->isPlasticityEnabled() ? "Yes" : "No") << std::endl;
        
        // Get spike system statistics
        if (globalState.brain->getSpikeSystem()) {
            auto mostActive = globalState.brain->getSpikeSystem()->getMostActiveNeurons(5);
            std::cout << "  Most active neurons: " << mostActive.size() << std::endl;
        }
        
        std::cout << "\nAnalysis completed.\n" << std::endl;
        
        return CLIResult::Success;
    }
    
    CLIResult handleCompare(const std::string& baseline) {
        std::cout << "=== Performance Comparison ===\n" << std::endl;
        
        if (baseline.empty()) {
            std::cout << "Error: Baseline profile file not specified." << std::endl;
            return CLIResult::Error;
        }
        
        std::cout << "Comparing with baseline: " << baseline << std::endl;
        std::cout << "Comparison analysis would be performed here.\n" << std::endl;
        
        return CLIResult::Success;
    }
    
    // Helper methods
    std::string getOption(const std::map<std::string, std::string>& options, const std::string& key, const std::string& defaultValue) const {
        auto it = options.find(key);
        return it != options.end() ? it->second : defaultValue;
    }
};

} // namespace nlm

