// Visualization and Progress Tracking System
//
// This module provides real-time visualization and progress tracking
// for NLM experiments, making it easier to monitor and debug simulations.

#ifndef NLM_VISUALIZATION_H
#define NLM_VISUALIZATION_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <memory>
#include <mutex>
#include <thread>
#include <functional>

namespace nlm {

class ProgressTracker {
public:
    struct ProgressUpdate {
        double progress;  // 0.0 to 1.0
        std::string message;
        std::chrono::steady_clock::time_point timestamp;
        uint64_t step;    // Current simulation step
        uint64_t maxSteps;
        float progressPercent;
        std::map<std::string, std::string> metrics;
    };
    
    using ProgressCallback = std::function<void(const ProgressUpdate&)>;
    
    ProgressTracker() : totalUpdates(0), lastUpdateTime(std::chrono::steady_clock::now()) {}
    
    void start(const std::string& title, uint64_t totalSteps, 
               const std::string& description = "") {
        this->title = title;
        this->totalSteps = totalSteps;
        this->description = description;
        currentStep = 0;
        progressUpdates.clear();
        totalUpdates = 0;
        startTime = std::chrono::steady_clock::now();
        
        ProgressUpdate initialUpdate;
        initialUpdate.progress = 0.0;
        initialUpdate.message = "Starting " + title;
        initialUpdate.timestamp = startTime;
        initialUpdate.step = 0;
        initialUpdate.maxSteps = totalSteps;
        initialUpdate.progressPercent = 0.0f;
        initialUpdate.metrics["title"] = title;
        initialUpdate.metrics["steps"] = std::to_string(totalSteps);
        
        notifyCallbacks(initialUpdate);
        addUpdate(initialUpdate);
    }
    
    void update(uint64_t step, const std::string& message = "") {
        if (step > totalSteps) return;
        
        currentStep = step;
        
        ProgressUpdate update;
        update.progress = static_cast<double>(step) / totalSteps;
        update.message = message;
        update.timestamp = std::chrono::steady_clock::now();
        update.step = step;
        update.maxSteps = totalSteps;
        update.progressPercent = (step * 100.0f) / totalSteps;
        update.metrics["step"] = std::to_string(step);
        update.metrics["total_steps"] = std::to_string(totalSteps);
        
        if (!message.empty()) {
            update.metrics["message"] = message;
        }
        
        notifyCallbacks(update);
        addUpdate(update);
    }
    
    void setMetric(const std::string& name, const std::string& value) {
        currentMetrics[name] = value;
        ProgressUpdate update;
        update.progress = static_cast<double>(currentStep) / totalSteps;
        update.message = "Metric update: " + name + " = " + value;
        update.timestamp = std::chrono::steady_clock::now();
        update.step = currentStep;
        update.maxSteps = totalSteps;
        update.progressPercent = (currentStep * 100.0f) / totalSteps;
        update.metrics["metric"] = name;
        update.metrics["value"] = value;
        
        notifyCallbacks(update);
        addUpdate(update);
    }
    
    void finish(const std::string& finalMessage = "Experiment completed") {
        ProgressUpdate finalUpdate;
        finalUpdate.progress = 1.0;
        finalUpdate.message = finalMessage;
        finalUpdate.timestamp = std::chrono::steady_clock::now();
        finalUpdate.step = totalSteps;
        finalUpdate.maxSteps = totalSteps;
        finalUpdate.progressPercent = 100.0f;
        finalUpdate.metrics["final"] = "true";
        
        notifyCallbacks(finalUpdate);
        addUpdate(finalUpdate);
        
        auto endTime = std::chrono::steady_clock::now();
        totalDuration += std::chrono::duration<double>(endTime - startTime).count();
    }
    
    void addCallback(ProgressCallback callback) {
        std::lock_guard<std::mutex> lock(callbackMutex);
        callbacks.push_back(callback);
    }
    
    std::vector<ProgressUpdate> getProgressHistory() const {
        std::lock_guard<std::mutex> lock(historyMutex);
        return progressUpdates;
    }
    
    double getTotalDuration() const { return totalDuration; }
    
    void printProgress() const {
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
    
    void exportProgress(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        file << "NLM Experiment Progress Report" << std::endl;
        file << "===============================" << std::endl;
        file << "Title: " << title << std::endl;
        file << "Total Steps: " << totalSteps << std::endl;
        file << "Start Time: " << std::chrono::system_clock::to_time_t(startTime) << std::endl;
        file << "End Time: " << std::chrono::system_clock::to_time_t(lastUpdateTime) << std::endl;
        file << "Total Duration: " << totalDuration << "s" << std::endl;
        file << std::endl;
        
        for (const auto& update : progressUpdates) {
            file << "Step " << update.step << "/" << update.maxSteps << ": "
                 << update.progressPercent << "% - " << update.message << std::endl;
            
            for (const auto& metric : update.metrics) {
                if (metric.first != "step" && metric.first != "total_steps" && 
                    metric.first != "message" && metric.first != "metric" && 
                    metric.first != "value" && metric.first != "final") {
                    file << "  " << metric.first << ": " << metric.second << std::endl;
                }
            }
            file << std::endl;
        }
        
        file.close();
    }
    
private:
    void notifyCallbacks(const ProgressUpdate& update) {
        std::lock_guard<std::mutex> lock(callbackMutex);
        for (const auto& callback : callbacks) {
            callback(update);
        }
    }
    
    void addUpdate(const ProgressUpdate& update) {
        std::lock_guard<std::mutex> lock(historyMutex);
        progressUpdates.push_back(update);
        lastUpdateTime = update.timestamp;
        totalUpdates++;
    }
    
    std::string title;
    uint64_t totalSteps;
    uint64_t currentStep;
    std::string description;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::vector<ProgressUpdate> progressUpdates;
    std::map<std::string, std::string> currentMetrics;
    double totalDuration = 0.0;
    uint64_t totalUpdates;
    
    std::vector<ProgressCallback> callbacks;
    mutable std::mutex callbackMutex;
    mutable std::mutex historyMutex;
};

// Real-time console visualization
class ConsoleVisualizer {
public:
    ConsoleVisualizer() : running(false), updateInterval(0.1) {}
    
    void start(ProgressTracker& tracker) {
        this->tracker = &tracker;
        running = true;
        visualizationThread = std::thread(&ConsoleVisualizer::visualizationLoop, this);
    }
    
    void stop() {
        running = false;
        if (visualizationThread.joinable()) {
            visualizationThread.join();
        }
    }
    
    void setUpdateInterval(double interval) {
        updateInterval = interval;
    }
    
    void setVisualizationMode(const std::string& mode) {
        this->visualizationMode = mode;
    }
    
private:
    void visualizationLoop() {
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
    
    void updateVisualization() {
        if (!tracker) return;
        
        system("clear");  // Clear screen (Linux/Mac)
        // system("cls"); // Uncomment for Windows
        
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
    
    void printStandardView() {
        std::cout << "=== Simulation Overview ===" << std::endl;
        std::cout << "Phase 6 Integration Demo" << std::endl;
        std::cout << "Running comprehensive brain integration tests" << std::endl << std::endl;
    }
    
    void printCompactView() {
        std::cout << "Progress: " << std::fixed << std::setprecision(1)
                  << (tracker->getProgressHistory().empty() ? 0.0 : 
                      tracker->getProgressHistory().back().progress * 100.0) << "%" << std::endl;
    }
    
    void printDetailedView() {
        std::cout << "=== Detailed System Status ===" << std::endl;
        std::cout << "Memory Systems: Working, Episodic, Associative" << std::endl;
        std::cout << "Neuromodulation: Dopamine, Curiosity, Novelty" << std::endl;
        std::cout << "Prediction System: Active" << std::endl;
        std::cout << "Cognition: Planner, Concept Formation, Attention" << std::endl;
        std::cout << "Development: Stage 6 (Integration)" << std::endl;
        std::cout << "Plasticity: STDP, Hebbian, Structural" << std::endl << std::endl;
    }
    
    ProgressTracker* tracker = nullptr;
    std::string visualizationMode = "standard";
    bool running = false;
    double updateInterval;
    std::thread visualizationThread;
};

// File-based result exporter
class ResultExporter {
public:
    struct ExperimentResult {
        std::string experimentName;
        int phase;
        uint64_t neuronCount;
        uint64_t maxSteps;
        double totalTime;
        uint64_t spikeCount;
        float firingRate;
        double totalReward;
        float avgFiringRate;
        float memoryEpisodesStored;
        float dopamineLevel;
        float noveltyLevel;
        float curiosityLevel;
        std::map<std::string, double> integrationResults;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
    };
    
    static bool exportToJSON(const ExperimentResult& result, const std::string& filename) {
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
    
    static bool exportToCSV(const ExperimentResult& result, const std::string& filename) {
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
    
    static bool exportToXML(const ExperimentResult& result, const std::string& filename) {
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
};

// Unified visualization and progress reporting system
class VisualizationSystem {
public:
    VisualizationSystem() : progressTracker(), visualizer(), resultExporter() {}
    
    void startProgress(const std::string& title, uint64_t totalSteps, 
                       const std::string& description = "") {
        progressTracker.start(title, totalSteps, description);
        
        // Add visualization callback for real-time updates
        progressTracker.addCallback([this](const ProgressTracker::ProgressUpdate& update) {
            onProgressUpdate(update);
        });
        
        // Start real-time visualization
        visualizer.start(progressTracker);
        visualizer.setUpdateInterval(0.5);  // Update every 0.5 seconds
        visualizer.setVisualizationMode("standard");
    }
    
    void updateProgress(uint64_t step, const std::string& message = "") {
        progressTracker.update(step, message);
    }
    
    void setMetric(const std::string& name, const std::string& value) {
        progressTracker.setMetric(name, value);
    }
    
    void finishProgress(const std::string& finalMessage = "Experiment completed") {
        progressTracker.finish(finalMessage);
        
        // Stop visualization
        visualizer.stop();
        
        // Export progress data
        progressTracker.exportProgress("/tmp/nlm_progress.txt");
    }
    
    void exportResults(const ResultExporter::ExperimentResult& result, const std::string& baseFilename) {
        // Export in multiple formats
        ResultExporter::exportToJSON(result, baseFilename + ".json");
        ResultExporter::exportToCSV(result, baseFilename + ".csv");
        ResultExporter::exportToXML(result, baseFilename + ".xml");
    }
    
    ProgressTracker& getProgressTracker() { return progressTracker; }
    
private:
    void onProgressUpdate(const ProgressTracker::ProgressUpdate& update) {
        // Log update and potentially trigger other actions
        std::cout << "[VISUALIZATION] Update: Step " << update.step << "/" 
                  << update.maxSteps << " - " << update.progressPercent << "% - " 
                  << update.message << std::endl;
    }
    
    ProgressTracker progressTracker;
    ConsoleVisualizer visualizer;
    ResultExporter resultExporter;
};

} // namespace nlm

#endif // NLM_VISUALIZATION_H
