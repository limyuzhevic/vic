// NLM Analysis Tool
// Provides analysis capabilities for NLM simulations

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <memory>

#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"

using namespace nlm;

void printAnalysisBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                    NLM Analysis Tool                          ║
║                                                               ║
║  Analyze learning performance, metrics, and comparisons      ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

class NLMAnalyzer {
public:
    struct LearningMetrics {
        double totalLearningRate;
        double avgFiringRate;
        double peakFiringRate;
        size_t totalSpikes;
        size_t activeNeurons;
        double developmentProgression;
        double noveltyIntegration;
        double curiosityDrivenExploration;
        std::vector<double> firingRateHistory;
        std::vector<double> learningRateHistory;
    };

    struct ExperimentComparison {
        std::string name;
        size_t neuronCount;
        size_t synapseCount;
        double finalLearningRate;
        double finalFiringRate;
        size_t totalSpikes;
        double developmentTime;
        double simulationTime;
        bool learningAchieved;
    };

    NLMAnalyzer() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }

    LearningMetrics analyzeBrain(std::shared_ptr<Brain> brain) {
        LearningMetrics metrics;
        
        // Basic statistics
        metrics.totalSpikes = brain->getTotalSpikeCount();
        metrics.activeNeurons = brain->getActiveNeuronCount();
        metrics.avgFiringRate = brain->getAverageFiringRate();
        
        // Calculate learning rate (simplified)
        metrics.totalLearningRate = calculateLearningRate(brain);
        
        // Peak firing rate (we'd need history for this)
        metrics.peakFiringRate = metrics.avgFiringRate * 1.2; // Approximation
        
        // Development progression
        metrics.developmentProgression = static_cast<double>(brain->getDevelopmentalStage()) / 5.0;
        
        return metrics;
    }

    LearningMetrics analyzeAgent(std::shared_ptr<AgentBrain> agent) {
        LearningMetrics metrics;
        
        if (!agent) return metrics;
        
        auto brain = agent->getBrain();
        metrics = analyzeBrain(brain);
        
        // Agent-specific metrics
        metrics.noveltyIntegration = agent->getNoveltyLevel();
        metrics.curiosityDrivenExploration = agent->getCuriosityLevel();
        
        return metrics;
    }

    void exportMetrics(const LearningMetrics& metrics, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "NLM Learning Metrics Analysis" << std::endl;
        file << "===============================" << std::endl;
        file << std::endl;
        
        file << "Basic Statistics:" << std::endl;
        file << "  Total Learning Rate: " << metrics.totalLearningRate << std::endl;
        file << "  Average Firing Rate: " << metrics.avgFiringRate << std::endl;
        file << "  Peak Firing Rate: " << metrics.peakFiringRate << std::endl;
        file << "  Total Spikes: " << metrics.totalSpikes << std::endl;
        file << "  Active Neurons: " << metrics.activeNeurons << std::endl;
        file << std::endl;
        
        file << "Development Metrics:" << std::endl;
        file << "  Development Progression: " << metrics.developmentProgression << std::endl;
        file << "  Novelty Integration: " << metrics.noveltyIntegration << std::endl;
        file << "  Curiosity-Driven Exploration: " << metrics.curiosityDrivenExploration << std::endl;
        file << std::endl;
        
        file << "Performance History:" << std::endl;
        file << "  Firing Rate History Points: " << metrics.firingRateHistory.size() << std::endl;
        file << "  Learning Rate History Points: " << metrics.learningRateHistory.size() << std::endl;
        
        // Export history data
        if (!metrics.firingRateHistory.empty()) {
            file << "  Firing Rate Samples: ";
            for (size_t i = 0; i < std::min(size_t(10), metrics.firingRateHistory.size()); ++i) {
                file << std::fixed << std::setprecision(3) << metrics.firingRateHistory[i];
                if (i < metrics.firingRateHistory.size() - 1) file << ", ";
            }
            file << "..." << std::endl;
        }
        
        file << std::endl;
        file << "Analysis Summary:" << std::endl;
        
        // Determine analysis
        std::string analysis;
        if (metrics.totalSpikes > 1000) {
            analysis = "High activity - Potential learning detected";
        } else if (metrics.totalSpikes > 100) {
            analysis = "Moderate activity - Some learning possible";
        } else if (metrics.totalSpikes > 0) {
            analysis = "Low activity - Limited learning";
        } else {
            analysis = "No activity - No learning occurred";
        }
        
        file << "  " << analysis << std::endl;
        
        file.close();
        
        NLM_LOG_INFO("Metrics exported to " + filename);
    }

    double calculateLearningRate(std::shared_ptr<Brain> brain) {
        // Simplified learning rate calculation
        // In a real implementation, this would track synaptic weight changes
        size_t synapses = brain->getTotalSynapseCount();
        double spikeRate = brain->getAverageFiringRate();
        
        // Learning rate is proportional to spike activity
        return spikeRate * (synapses / 1000.0);
    }

    std::vector<ExperimentComparison> loadComparisons(const std::string& filename) {
        std::vector<ExperimentComparison> comparisons;
        
        // In a real implementation, this would read from a file
        // For now, return empty
        
        return comparisons;
    }

    void printLearningReport(const LearningMetrics& metrics, const std::string& title) {
        std::cout << "=== " << title << " ===" << std::endl;
        std::cout << "Learning Rate: " << std::fixed << std::setprecision(3) 
                  << metrics.totalLearningRate << std::endl;
        std::cout << "Average Firing Rate: " << metrics.avgFiringRate << std::endl;
        std::cout << "Total Spikes: " << metrics.totalSpikes << std::endl;
        std::cout << "Active Neurons: " << metrics.activeNeurons << std::endl;
        std::cout << "Development Stage: " << (int)(metrics.developmentProgression * 5) << std::endl;
        std::cout << std::endl;
    }
};

int main(int argc, char** argv) {
    printAnalysisBanner();
    
    if (argc < 2) {
        std::cout << "Usage: nlm_analysis <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  analyze <brain_file>     Analyze a saved brain state" << std::endl;
        std::cout << "  compare <file1> <file2>   Compare two experiment results" << std::endl;
        std::cout << "  metrics [brain_file]     Show current or loaded metrics" << std::endl;
        std::cout << std::endl;
        return 0;
    }

    std::string command(argv[1]);
    
    if (command == "analyze") {
        if (argc < 3) {
            std::cout << "Error: Missing brain file argument" << std::endl;
            return 1;
        }
        
        std::string brainFile(argv[2]);
        std::cout << "Loading brain from: " << brainFile << std::endl;
        
        // In real implementation, load brain from file
        std::cout << "Analysis functionality would load brain and analyze it" << std::endl;
        
    } else if (command == "compare") {
        if (argc < 4) {
            std::cout << "Error: Missing file arguments for comparison" << std::endl;
            return 1;
        }
        
        std::string file1(argv[2]);
        std::string file2(argv[3]);
        std::cout << "Comparing experiments: " << file1 << " vs " << file2 << std::endl;
        
        // In real implementation, compare the files
        std::cout << "Comparison functionality would analyze both files" << std::endl;
        
    } else if (command == "metrics") {
        std::string brainFile = (argc >= 3) ? argv[2] : "";
        if (brainFile.empty()) {
            std::cout << "Showing default metrics (no brain loaded)" << std::endl;
        } else {
            std::cout << "Loading brain: " << brainFile << std::endl;
        }
        
        std::cout << "Metrics would be displayed here" << std::endl;
        
    } else {
        std::cout << "Unknown command: " << command << std::endl;
        return 1;
    }
    
    return 0;
}
