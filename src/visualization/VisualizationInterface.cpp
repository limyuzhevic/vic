#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <atomic>
#include <json.hpp>
#include <string>
#include <map>
#include <chrono>

using json = nlohmann::json;

namespace nlm {

class VisualizationInterface::Impl {
public:
    std::string outputDirectory;
    int updateIntervalMs;
    bool autoSave;
    int saveIntervalSteps;
    QualityLevel qualityLevel;
    bool interactive;
    std::function<void(const std::string& event)> interactiveCallback;
    
    // Real-time visualization
    std::thread visualizationThread;
    std::atomic<bool> visualizationRunning;
    std::shared_ptr<Brain> currentBrain;
    std::chrono::steady_clock::time_point lastUpdate;
    
    // Performance tracking
    size_t totalFramesRendered;
    double totalRenderTime;
    size_t updateCount;
    
    Impl() : updateIntervalMs(100), autoSave(false), saveIntervalSteps(1000), 
             qualityLevel(QualityLevel::Medium), interactive(false), 
             visualizationRunning(false), totalFramesRendered(0), 
             totalRenderTime(0.0), updateCount(0) {
        outputDirectory = "./visualization_output";
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() {
    stopRealTimeVisualization();
}

bool VisualizationInterface::initialize(std::shared_ptr<Config> config) {
    try {
        // Load configuration
        pImpl->outputDirectory = config->getOr<std::string>("visualization_output_dir", "./visualization_output");
        pImpl->updateIntervalMs = config->getOr<int>("visualization_update_interval_ms", 100);
        pImpl->autoSave = config->getOr<bool>("visualization_auto_save", false);
        pImpl->saveIntervalSteps = config->getOr<int>("visualization_save_interval_steps", 1000);
        pImpl->qualityLevel = static_cast<QualityLevel>(config->getOr<int>("visualization_quality_level", 1));
        pImpl->interactive = config->getOr<bool>("visualization_interactive", false);
        
        // Create output directory
        std::filesystem::create_directories(pImpl->outputDirectory);
        
        NLM_LOG_INFO("Visualization interface initialized successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize visualization interface: " + std::string(e.what()));
        return false;
    }
}

void VisualizationInterface::renderNeuralNetwork(const Brain& brain, const std::string& outputFile) {
    // Generate HTML visualization of neural network structure
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for neural network visualization: " + outputFile);
        return;
    }
    
    // Generate JavaScript visualization code
    file << generateNeuralNetworkVisualization(brain);
    file.close();
    
    NLM_LOG_INFO("Neural network visualization rendered to: " + outputFile);
}

void VisualizationInterface::renderActivityMap(const Brain& brain, const std::string& outputFile) {
    // Generate visualization of neural activity over time
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for activity map visualization: " + outputFile);
        return;
    }
    
    // Generate activity map data
    auto activityData = generateActivityMapData(brain);
    file << generateActivityMapHTML(activityData);
    file.close();
    
    NLM_LOG_INFO("Activity map visualization rendered to: " + outputFile);
}

void VisualizationInterface::renderMemoryTrajectories(const Brain& brain, const std::string& outputFile) {
    // Generate visualization of memory formation and recall
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for memory trajectory visualization: " + outputFile);
        return;
    }
    
    // Generate memory trajectory visualization
    auto memoryData = generateMemoryTrajectoryData(brain);
    file << generateMemoryTrajectoryHTML(memoryData);
    file.close();
    
    NLM_LOG_INFO("Memory trajectory visualization rendered to: " + outputFile);
}

void VisualizationInterface::renderTrainingProgress(const std::string& outputFile) {
    // Generate visualization of training progress
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for training progress visualization: " + outputFile);
        return;
    }
    
    // Generate training progress visualization
    auto trainingData = generateTrainingProgressData();
    file << generateTrainingProgressHTML(trainingData);
    file.close();
    
    NLM_LOG_INFO("Training progress visualization rendered to: " + outputFile);
}

void VisualizationInterface::startRealTimeVisualization(const Brain& brain, int updateIntervalMs) {
    pImpl->visualizationRunning = true;
    pImpl->currentBrain = std::const_pointer_cast<Brain>(&brain); // This should be const_cast for simplicity
    pImpl->visualizationThread = std::thread(&VisualizationInterface::visualizationThreadFunc, this);
    
    NLM_LOG_INFO("Started real-time visualization");
}

void VisualizationInterface::stopRealTimeVisualization() {
    pImpl->visualizationRunning = false;
    
    if (pImpl->visualizationThread.joinable()) {
        pImpl->visualizationThread.join();
    }
    
    NLM_LOG_INFO("Stopped real-time visualization");
}

bool VisualizationInterface::isRealTimeVisualizationRunning() const {
    return pImpl->visualizationRunning;
}

void VisualizationInterface::exportToJSON(const Brain& brain, const std::string& filepath) {
    json j = generateBrainStateJSON(brain);
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << j.dump(2);
        NLM_LOG_INFO("Brain state exported to JSON: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to export brain state to JSON: " + filepath);
    }
}

void VisualizationInterface::exportToCSV(const Brain& brain, const std::string& filepath) {
    auto csvData = generateBrainStateCSV(brain);
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        for (const auto& row : csvData) {
            for (size_t i = 0; i < row.size(); ++i) {
                file << row[i];
                if (i < row.size() - 1) file << ",";
            }
            file << "\n";
        }
        NLM_LOG_INFO("Brain state exported to CSV: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to export brain state to CSV: " + filepath);
    }
}

void VisualizationInterface::exportToNeuroML(const Brain& brain, const std::string& filepath) {
    // Generate NeuroML format export
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << generateNeuroMLExport(brain);
        NLM_LOG_INFO("Brain state exported to NeuroML: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to export brain state to NeuroML: " + filepath);
    }
}

void VisualizationInterface::setOutputDirectory(const std::string& directory) {
    pImpl->outputDirectory = directory;
    std::filesystem::create_directories(pImpl->outputDirectory);
}

void VisualizationInterface::setUpdateInterval(int intervalMs) {
    pImpl->updateIntervalMs = intervalMs;
}

void VisualizationInterface::enableAutoSave(bool enable, int saveIntervalSteps) {
    pImpl->autoSave = enable;
    pImpl->saveIntervalSteps = saveIntervalSteps;
}

void VisualizationInterface::setQualityLevel(QualityLevel level) {
    pImpl->qualityLevel = level;
}

std::string VisualizationInterface::getPerformanceMetrics() const {
    std::stringstream ss;
    ss << "Performance Metrics:" << std::endl;
    ss << "===================" << std::endl;
    ss << "Total frames rendered: " << pImpl->totalFramesRendered << std::endl;
    ss << "Average render time: " << (pImpl->totalRenderTime / pImpl->totalFramesRendered) * 1000 << " ms" << std::endl;
    ss << "Updates per second: " << (pImpl->updateCount / 60.0) << std::endl;
    return ss.str();
}

std::string VisualizationInterface::getVisualizationStats() const {
    std::stringstream ss;
    ss << "Visualization Statistics:" << std::endl;
    ss << "======================" << std::endl;
    ss << "Quality level: " << static_cast<int>(pImpl->qualityLevel) << std::endl;
    ss << "Update interval: " << pImpl->updateIntervalMs << " ms" << std::endl;
    ss << "Auto-save: " << (pImpl->autoSave ? "enabled" : "disabled") << std::endl;
    ss << "Interactive: " << (pImpl->interactive ? "enabled" : "disabled") << std::endl;
    return ss.str();
}

void VisualizationInterface::enableInteractivity(bool enable) {
    pImpl->interactive = enable;
}

void VisualizationInterface::setInteractiveCallback(std::function<void(const std::string& event)> callback) {
    pImpl->interactiveCallback = callback;
}

// Private implementation methods
std::string VisualizationInterface::generateNeuralNetworkVisualization(const Brain& brain) {
    std::stringstream ss;
    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html>" << std::endl;
    ss << "<head>" << std::endl;
    ss << "    <title>NLM Neural Network Visualization</title>" << std::endl;
    ss << "    <script src=\"https://cdn.jsdelivr.net/npm/d3@7.8.5/dist/d3.min.js\"></script>" << std::endl;
    ss << "</head>" << std::endl;
    ss << "<body>" << std::endl;
    ss << "    <div id=\"network-container\"></div>" << std::endl;
    ss << "    <script>" << std::endl;
    ss << "        // Neural network visualization code" << std::endl;
    ss << "        const brainData = {" << std::endl;
    ss << "            neuronCount: " << brain.getTotalNeuronCount() << "," << std::endl;
    ss << "            regionCount: " << brain.getRegionCount() << "," << std::endl;
    ss << "            synapseCount: " << brain.getTotalSynapseCount() << std::endl;
    ss << "        };" << std::endl;
    ss << "        // D3.js visualization implementation..." << std::endl;
    ss << "    </script>" << std::endl;
    ss << "</body>" << std::endl;
    ss << "</html>" << std::endl;
    return ss.str();
}

json VisualizationInterface::generateBrainStateJSON(const Brain& brain) {
    json j;
    j["neuron_count"] = brain.getTotalNeuronCount();
    j["synapse_count"] = brain.getTotalSynapseCount();
    j["region_count"] = brain.getRegionCount();
    j["active_neurons"] = brain.getActiveNeuronCount();
    j["firing_neurons"] = brain.getFiringNeuronCount();
    j["average_firing_rate"] = brain.getAverageFiringRate();
    j["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
    
    return j;
}

std::vector<std::vector<std::string>> VisualizationInterface::generateBrainStateCSV(const Brain& brain) {
    return {
        {"Metric", "Value"},
        {"Total Neurons", std::to_string(brain.getTotalNeuronCount())},
        {"Total Synapses", std::to_string(brain.getTotalSynapseCount())},
        {"Regions", std::to_string(brain.getRegionCount())},
        {"Active Neurons", std::to_string(brain.getActiveNeuronCount())},
        {"Firing Neurons", std::to_string(brain.getFiringNeuronCount())},
        {"Average Firing Rate", std::to_string(brain.getAverageFiringRate())},
        {"E/I Ratio", std::to_string(brain.getExcitationInhibitionRatio())}
    };
}

std::string VisualizationInterface::generateNeuroMLExport(const Brain& brain) {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    ss << "<neuroML xmlns=\"http://www.neuroml.org/version1.8.0\">" << std::endl;
    ss << "    <network id=\"nlm_network\" separator=\"_\">" << std::endl;
    ss << "        <notes>Generated from NLM (熙然) artificial brain simulation</notes>" << std::endl;
    ss << "    </network>" << std::endl;
    ss << "</neuroML>" << std::endl;
    return ss.str();
}

void VisualizationInterface::visualizationThreadFunc() {
    auto lastTime = std::chrono::steady_clock::now();
    
    while (pImpl->visualizationRunning && pImpl->currentBrain) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
        
        if (elapsed.count() >= pImpl->updateIntervalMs) {
            // Update visualization
            // (In a real implementation, this would render frames)
            
            pImpl->totalFramesRendered++;
            pImpl->updateCount++;
            
            // Auto-save if enabled
            if (pImpl->autoSave && pImpl->updateCount % pImpl->saveIntervalSteps == 0) {
                std::string filepath = pImpl->outputDirectory + "/auto_save_" + 
                                      std::to_string(pImpl->updateCount) + ".html";
                renderNeuralNetwork(*pImpl->currentBrain, filepath);
            }
            
            lastTime = currentTime;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

std::map<std::string, std::vector<std::vector<double>>> VisualizationInterface::generateActivityMapData(const Brain& brain) {
    return std::map<std::string, std::vector<std::vector<double>>>();
}

std::map<std::string, std::vector<std::vector<double>>> VisualizationInterface::generateMemoryTrajectoryData(const Brain& brain) {
    return std::map<std::string, std::vector<std::vector<double>>>();
}

std::map<std::string, std::vector<std::vector<double>>> VisualizationInterface::generateTrainingProgressData() {
    return std::map<std::string, std::vector<std::vector<double>>>();
}

std::string VisualizationInterface::generateActivityMapHTML(const std::map<std::string, std::vector<std::vector<double>>>& data) {
    return "<!DOCTYPE html><html><body>Activity Map Visualization</body></html>";
}

std::string VisualizationInterface::generateMemoryTrajectoryHTML(const std::map<std::string, std::vector<std::vector<double>>>& data) {
    return "<!DOCTYPE html><html><body>Memory Trajectory Visualization</body></html>";
}

std::string VisualizationInterface::generateTrainingProgressHTML(const std::map<std::string, std::vector<std::vector<double>>>& data) {
    return "<!DOCTYPE html><html><body>Training Progress Visualization</body></html>";
}

} // namespace nlm
