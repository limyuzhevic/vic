// Visualization and Progress Tracking System Implementation
//
// This file contains the implementation of the visualization and progress tracking
// system for NLM experiments.

#include "../src/nlm/VisualizationSystem.h"
#include <fstream>
#include <iostream>

namespace nlm {

// Implementation of ProgressTracker methods
void ProgressTracker::printProgress() const {
    auto now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - startTime).count();
    
    std::cout << "=== " << title << " ===" << std::endl;
    std::cout << "Progress: [";
    
    int barWidth = 50;
    int filledWidth = static_cast<int>(progressUpdates.back().progress * barWidth);
    
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            std::cout << "=";
        } else if (i == filledWidth) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
    }
    
    std::cout << "] " << std::fixed << std::setprecision(1) 
              << (progressUpdates.back().progress * 100.0) << "%" << std::endl;
    
    std::cout << "Step: " << currentStep << " / " << totalSteps << std::endl;
    std::cout << "Elapsed: " << std::fixed << std::setprecision(2) << elapsed << "s" << std::endl;
    
    if (!progressUpdates.empty()) {
        std::cout << "Message: " << progressUpdates.back().message << std::endl;
    }
    
    for (const auto& pair : currentMetrics) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    
    std::cout << "======================" << std::endl;
}

// Implementation of ConsoleVisualizer methods
void ConsoleVisualizer::visualizationLoop() {
    auto lastUpdate = std::chrono::steady_clock::now();
    
    while (running) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastUpdate).count();
        
        if (elapsed >= updateInterval) {
            updateVisualization();
            lastUpdate = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void ConsoleVisualizer::updateVisualization() {
    if (!tracker) return;
    
    // Clear screen (platform-specific)
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║                     NLM — 熙然 VISUALIZATION                  ║
    ║                     Neural Learning Machine                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl << std::endl;
    
    if (visualizationMode == "compact") {
        printCompactView();
    } else if (visualizationMode == "detailed") {
        printDetailedView();
    } else {
        printStandardView();
    }
    
    tracker->printProgress();
    std::cout << std::endl;
}

void ConsoleVisualizer::printStandardView() {
    std::cout << "=== Simulation Overview ===" << std::endl;
    std::cout << "Phase 6 Integration Demo" << std::endl;
    std::cout << "Running comprehensive brain integration tests" << std::endl << std::endl;
}

void ConsoleVisualizer::printCompactView() {
    std::cout << "Progress: " << std::fixed << std::setprecision(1)
              << (tracker->getProgressHistory().empty() ? 0.0 : 
                  tracker->getProgressHistory().back().progress * 100.0) << "%" << std::endl;
}

void ConsoleVisualizer::printDetailedView() {
    std::cout << "=== Detailed System Status ===" << std::endl;
    std::cout << "Memory Systems: Working, Episodic, Associative" << std::endl;
    std::cout << "Neuromodulation: Dopamine, Curiosity, Novelty" << std::endl;
    std::cout << "Prediction System: Active" << std::endl;
    std::cout << "Cognition: Planner, Concept Formation, Attention" << std::endl;
    std::cout << "Development: Stage 6 (Integration)" << std::endl;
    std::cout << "Plasticity: STDP, Hebbian, Structural" << std::endl << std::endl;
}

// Implementation of ResultExporter methods
bool ResultExporter::exportToJSON(const ExperimentResult& result, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{" << std::endl;
    file << "  \"experiment_name\": \"" << result.experimentName << "\"," << std::endl;
    file << "  \"phase\": " << result.phase << "," << std::endl;
    file << "  \"neuron_count\": " << result.neuronCount << "," << std::endl;
    file << "  \"max_steps\": " << result.maxSteps << "," << std::endl;
    file << "  \"total_time\": " << std::fixed << std::setprecision(3) << result.totalTime << "," << std::endl;
    file << "  \"spike_count\": " << result.spikeCount << "," << std::endl;
    file << "  \"firing_rate\": " << std::fixed << std::setprecision(3) << result.firingRate << "," << std::endl;
    file << "  \"total_reward\": " << std::fixed << std::setprecision(3) << result.totalReward << ",\n";
    file << "  \"avg_firing_rate\": " << std::fixed << std::setprecision(3) << result.avgFiringRate << "," << std::endl;
    file << "  \"memory_episodes_stored\": " << std::fixed << std::setprecision(3) << result.memoryEpisodesStored << "," << std::endl;
    file << "  \"dopamine_level\": " << std::fixed << std::setprecision(3) << result.dopamineLevel << "," << std::endl;
    file << "  \"novelty_level\": " << std::fixed << std::setprecision(3) << result.noveltyLevel << "," << std::endl;
    file << "  \"curiosity_level\": " << std::fixed << std::setprecision(3) << result.curiosityLevel << "," << std::endl;
    
    file << "  \"integration_results\": {" << std::endl;
    bool first = true;
    for (const auto& pair : result.integrationResults) {
        if (!first) file << "    ," << std::endl;
        file << "    \"" << pair.first << "\": " << std::fixed << std::setprecision(3) << pair.second;
        first = false;
    }
    file << std::endl << "  }," << std::endl;
    
    file << "  \"start_time\": \"" << std::chrono::system_clock::to_time_t(result.startTime) << "\"," << std::endl;
    file << "  \"end_time\": \"" << std::chrono::system_clock::to_time_t(result.endTime) << "\"" << std::endl;
    file << "}" << std::endl;
    
    file.close();
    return true;
}

bool ResultExporter::exportToCSV(const ExperimentResult& result, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Metric,Value" << std::endl;
    file << "Experiment Name," << result.experimentName << std::endl;
    file << "Phase," << result.phase << std::endl;
    file << "Neurons," << result.neuronCount << std::endl;
    file << "Max Steps," << result.maxSteps << std::endl;
    file << "Total Time (s)," << std::fixed << std::setprecision(3) << result.totalTime << std::endl;
    file << "Spike Count," << result.spikeCount << std::endl;
    file << "Firing Rate (Hz)," << std::fixed << std::setprecision(3) << result.firingRate << std::endl;
    file << "Total Reward," << std::fixed << std::setprecision(3) << result.totalReward << std::endl;
    file << "Avg Firing Rate (Hz)," << std::fixed << std::setprecision(3) << result.avgFiringRate << std::endl;
    file << "Memory Episodes Stored," << std::fixed << std::setprecision(3) << result.memoryEpisodesStored << std::endl;
    file << "Dopamine Level," << std::fixed << std::setprecision(3) << result.dopamineLevel << std::endl;
    file << "Novelty Level," << std::fixed << std::setprecision(3) << result.noveltyLevel << std::endl;
    file << "Curiosity Level," << std::fixed << std::setprecision(3) << result.curiosityLevel << std::endl;
    
    for (const auto& pair : result.integrationResults) {
        file << pair.first << "," << std::fixed << std::setprecision(3) << pair.second << std::endl;
    }
    
    file.close();
    return true;
}

bool ResultExporter::exportToXML(const ExperimentResult& result, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    file << "<nlm_experiment>" << std::endl;
    file << "  <experiment_name>" << result.experimentName << "</experiment_name>" << std::endl;
    file << "  <phase>" << result.phase << "</phase>" << std::endl;
    file << "  <neuron_count>" << result.neuronCount << "</neuron_count>" << std::endl;
    file << "  <max_steps>" << result.maxSteps << "</max_steps>" << std::endl;
    file << "  <total_time>" << std::fixed << std::setprecision(3) << result.totalTime << "</total_time>" << std::endl;
    file << "  <spike_count>" << result.spikeCount << "</spike_count>" << std::endl;
    file << "  <firing_rate>" << std::fixed << std::setprecision(3) << result.firingRate << "</firing_rate>" << std::endl;
    file << "  <total_reward>" << std::fixed << std::setprecision(3) << result.totalReward << "</total_reward>" << std::endl;
    file << "  <avg_firing_rate>" << std::fixed << std::setprecision(3) << result.avgFiringRate << "</avg_firing_rate>" << std::endl;
    file << "  <memory_episodes_stored>" << std::fixed << std::setprecision(3) << result.memoryEpisodesStored << "</memory_episodes_stored>" << std::endl;
    file << "  <dopamine_level>" << std::fixed << std::setprecision(3) << result.dopamineLevel << "</dopamine_level>" << std::endl;
    file << "  <novelty_level>" << std::fixed << std::setprecision(3) << result.noveltyLevel << "</novelty_level>" << std::endl;
    file << "  <curiosity_level>" << std::fixed << std::setprecision(3) << result.curiosityLevel << "</curiosity_level>" << std::endl;
    
    file << "  <integration_results>" << std::endl;
    for (const auto& pair : result.integrationResults) {
        file << "    <" << pair.first << ">" << std::fixed << std::setprecision(3) << pair.second << "</" << pair.first << ">" << std::endl;
    }
    file << "  </integration_results>" << std::endl;
    
    file << "</nlm_experiment>" << std::endl;
    
    file.close();
    return true;
}

} // namespace nlm
