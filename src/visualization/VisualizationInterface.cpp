#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Synapse.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <map>
#include <vector>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    bool networkEnabled;
    bool activityEnabled;
    bool spikeEnabled;
    bool weightEnabled;
    bool statsEnabled;
    std::string colorTheme;
    
    // Data buffers for visualization
    std::vector<SpikeEvent> spikeHistory;
    std::vector<std::pair<double, double>> weightHistory;
    std::vector<std::pair<std::string, double>> networkStats;
    double currentProgress;
    std::string currentPhase;
    
    // Timing
    auto startTime = std::chrono::high_resolution_clock::now();
    size_t frameCount;
    double fps;
    
    // Mutex for thread safety
    mutable std::mutex dataMutex;
    
    Impl() : active(false), updateRate(30.0), 
             networkEnabled(true), activityEnabled(true),
             spikeEnabled(true), weightEnabled(true),
             statsEnabled(true), colorTheme("default"),
             currentProgress(0.0), frameCount(0), fps(0.0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    pImpl->active = true;
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->frameCount++;
    
    // Calculate FPS
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(currentTime - pImpl->startTime).count();
    if (elapsed > 0.0) {
        pImpl->fps = pImpl->frameCount / elapsed;
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    clearScreen();
    
    // Print header
    std::cout << getTimestamp() << " - NLM Real-Time Visualization" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    // Print FPS and performance
    std::cout << "Performance: " << std::fixed << std::setprecision(1 
        << pImpl->fps << " FPS | " << "Frame " << pImpl->frameCount" << std::endl;
    
    // Print progress
    if (!pImpl->currentPhase.empty()) {
        std::cout << "Progress: [" << std::string(static_cast<size_t>(pImpl->currentProgress * 20), '=')
                  << (pImpl->currentProgress < 1.0 ? ">" : "")
                  << std::string(static_cast<size_t>(20 * (1.0 - pImpl->currentProgress)), ' ')
                  << "] " << pImpl->currentProgress * 100 << "% ("
                  << pImpl->currentPhase << ")" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Render different visualization modes
    if (pImpl->networkEnabled) {
        visualizeNetwork();
        std::cout << std::endl;
    }
    
    if (pImpl->activityEnabled) {
        visualizeActivity();
        std::cout << std::endl;
    }
    
    if (pImpl->spikeEnabled && !pImpl->spikeHistory.empty()) {
        visualizeSpikes();
        std::cout << std::endl;
    }
    
    if (pImpl->weightEnabled && !pImpl->weightHistory.empty()) {
        visualizeWeights();
        std::cout << std::endl;
    }
    
    if (pImpl->statsEnabled && !pImpl->networkStats.empty()) {
        visualizeStatistics();
        std::cout << std::endl;
    }
    
    // Print controls
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Controls: " << getColorCode("yellow") << "NOT IMPLEMENTED" << getColorCode("reset") 
              << " | Press " << getColorCode("green") << "q" << getColorCode("reset") 
              << " to quit" << std::endl;
}

void VisualizationInterface::close() {
    pImpl->active = false;
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->spikeHistory.clear();
    pImpl->weightHistory.clear();
    pImpl->networkStats.clear();
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::enableNetworkVisualization(bool enable) {
    pImpl->networkEnabled = enable;
}

void VisualizationInterface::enableActivityVisualization(bool enable) {
    pImpl->activityEnabled = enable;
}

void VisualizationInterface::enableSpikeVisualization(bool enable) {
    pImpl->spikeEnabled = enable;
}

void VisualizationInterface::enableWeightVisualization(bool enable) {
    pImpl->weightEnabled = enable;
}

void VisualizationInterface::enableStatisticsVisualization(bool enable) {
    pImpl->statsEnabled = enable;
}

void VisualizationInterface::visualizeNetwork() {
    std::cout << getColorCode("cyan") << "=== Network Topology ===" << getColorCode("reset") << std::endl;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::cout << "  Regions: " << (pImpl->networkStats.empty() ? "0" : std::to_string(pImpl->networkStats.size())) << std::endl;
    
    // Show sample stats
    for (const auto& stat : pImpl->networkStats) {
        if (stat.first.find("region_") != std::string::npos) {
            std::cout << "    " << stat.first << ": " << stat.second << " neurons, " 
                      << getColorCode("green") << "●" << getColorCode("reset") << std::endl;
        }
    }
}

void VisualizationInterface::visualizeActivity() {
    std::cout << getColorCode("magenta") << "=== Neural Activity ===" << getColorCode("reset") << std::endl;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::cout << "  Active neurons: " << (pImpl->networkStats.empty() ? "0" : std::to_string(pImpl->networkStats.size())) << std::endl;
    
    for (const auto& stat : pImpl->networkStats) {
        if (stat.first.find("activity_") != std::string::npos) {
            double activity = stat.second;
            std::cout << "    " << stat.first << ": " << std::fixed << std::setprecision(2) 
                      << activity << (activity > 0.5 ? " " + getColorCode("red") + "🔥" + getColorCode("reset") : " " + getColorCode("green") + "●" + getColorCode("reset")) 
                      << std::endl;
        }
    }
}

void VisualizationInterface::visualizeSpikes() {
    std::cout << getColorCode("yellow") << "=== Spike Activity ===" << getColorCode("reset") << std::endl;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::cout << "  Total spikes in history: " << pImpl->spikeHistory.size() << std::endl;
    
    if (pImpl->spikeHistory.size() > 0) {
        // Show recent spikes
        size_t recent_count = std::min(size_t(5), pImpl->spikeHistory.size());
        std::cout << "  Recent spikes (last " << recent_count << "):" << std::endl;
        
        for (size_t i = pImpl->spikeHistory.size() - recent_count; i < pImpl->spikeHistory.size(); ++i) {
            const auto& spike = pImpl->spikeHistory[i];
            std::cout << "    Neuron " << spike.neuron_id.index() << " at t=" 
                      << std::fixed << std::setprecision(3) << spike.timestamp << getColorCode("green") << " ●" << getColorCode("reset") << std::endl;
        }
    }
}

void VisualizationInterface::visualizeWeights() {
    std::cout << getColorCode("blue") << "=== Synaptic Weights ===" << getColorCode("reset") << std::endl;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (pImpl->weightHistory.size() > 0) {
        // Show weight statistics
        double total_weight = 0.0;
        double max_weight = -1e6;
        double min_weight = 1e6;
        
        for (const auto& w : pImpl->weightHistory) {
            total_weight += w.second;
            max_weight = std::max(max_weight, w.second);
            min_weight = std::min(min_weight, w.second);
        }
        
        double avg_weight = total_weight / pImpl->weightHistory.size();
        
        std::cout << "  Total synapses: " << pImpl->weightHistory.size() << std::endl;
        std::cout << "  Average weight: " << std::fixed << std::setprecision(4) << avg_weight << std::endl;
        std::cout << "  Max weight: " << std::fixed << std::setprecision(4) << max_weight << getColorCode("red") << " 🔥" << getColorCode("reset") << std::endl;
        std::cout << "  Min weight: " << std::fixed << std::setprecision(4) << min_weight << getColorCode("green") << " 💚" << getColorCode("reset") << std::endl;
        
        // Show weight distribution bar chart
        std::cout << "  Weight distribution:" << std::endl;
        for (const auto& w : pImpl->weightHistory) {
            if (w.first.find("weight_0") != std::string::npos) {  // Simplified for example
                double norm_weight = (w.second - min_weight) / (max_weight - min_weight + 0.0001f);
                std::cout << "    " << w.first << ": [";
                for (size_t i = 0; i < size_t(norm_weight * 20); ++i) {
                    std::cout << getColorCode("red");
                    std::cout << "━";
                    std::cout << getColorCode("reset");
                }
                std::cout << getColorCode("reset") << "] " << std::fixed << std::setprecision(3) << w.second << std::endl;
                break;  // Just show first for now
            }
        }
    }
}

void VisualizationInterface::visualizeStatistics() {
    std::cout << getColorCode("green") << "=== System Statistics ===" << getColorCode("reset") << std::endl;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    for (const auto& stat : pImpl->networkStats) {
        if (stat.first.find("stat_") != std::string::npos) {
            std::cout << "  " << stat.first << ": " << std::fixed << std::setprecision(4) << stat.second << std::endl;
        }
    }
}

void VisualizationInterface::setProgress(const std::string& phase, double progress) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->currentPhase = phase;
    pImpl->currentProgress = std::max(0.0, std::min(1.0, progress));
}

void VisualizationInterface::setColorTheme(const std::string& theme) {
    pImpl->colorTheme = theme;
}

void VisualizationInterface::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

std::string VisualizationInterface::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string VisualizationInterface::getColorCode(const std::string& color) {
    if (pImpl->colorTheme == "monochrome") {
        return "";
    }
    
    static std::map<std::string, std::string> colors = {
        {"reset", "\033[0m"},
        {"red", "\033[91m"},
        {"green", "\033[92m"},
        {"yellow", "\033[93m"},
        {"blue", "\033[94m"},
        {"magenta", "\033[95m"},
        {"cyan", "\033[96m"},
        {"white", "\033[97m"}
    };
    
    return colors.count(color) ? colors[color] : "";
}

} // namespace nlm