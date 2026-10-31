// Brain visualization implementation
// Provides terminal-based visualization for NLM brain state monitoring
// Phase 6: Enhanced monitoring and analysis capabilities

#include "BrainVisualizer.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

class BrainVisualizer::Impl {
public:
    std::shared_ptr<Brain> brain;
    VisualizationConfig config;
    bool running;
    std::chrono::steady_clock::time_point lastUpdate;
    int frameCount;
    double fps;
    double averageFrameTime;
    std::chrono::steady_clock::time_point startTime;
    TextNeuronVisualizer textVisualizer;
    
    Impl() : running(false), frameCount(0), fps(0.0), averageFrameTime(0.0) {
        startTime = std::chrono::steady_clock::now();
    }
};

void BrainVisualizer::startVisualization(std::shared_ptr<Brain> brain, const VisualizationConfig& config) {
    pImpl->brain = brain;
    pImpl->config = config;
    pImpl->running = true;
    pImpl->frameCount = 0;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastUpdate = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("Starting NLM brain visualization...");
    
    while (pImpl->running) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastUpdate);
        
        if (elapsed.count() >= pImpl->config.updateInterval) {
            visualizeFrame(pImpl->brain, pImpl->config);
            pImpl->lastUpdate = now;
            pImpl->frameCount++;
            
            // Update FPS calculation
            auto totalElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->startTime);
            if (totalElapsed.count() > 0) {
                pImpl->fps = pImpl->frameCount / totalElapsed.count();
            }
        }
        
        // Small sleep to prevent CPU spinning
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void BrainVisualizer::visualizeFrame(std::shared_ptr<Brain> brain, const VisualizationConfig& config) {
    if (config.outputFile.empty()) {
        // Terminal visualization
        renderTerminalView(brain, config);
    } else {
        // File export
        exportVisualization(brain, config.outputFile);
    }
}

void BrainVisualizer::renderTerminalView(std::shared_ptr<Brain> brain, const VisualizationConfig& config) {
    system("clear");  // Clear screen (Unix-like systems)
    
    std::cout << vis::createHeader("NLM Brain Visualization - Phase 6") << std::endl;
    std::cout << "FPS: " << std::fixed << std::setprecision(1) << pImpl->fps << " | ";
    std::cout << "Frame Time: " << std::fixed << std::setprecision(3) << pImpl->averageFrameTime << "ms" << std::endl;
    std::cout << vis::createSeparator() << std::endl;
    
    if (config.showNeurons) {
        std::cout << "=== NEURAL NETWORK ===" << std::endl;
        pImpl->textVisualizer.renderNetworkState(brain);
        std::cout << std::endl;
    }
    
    if (config.showActivity) {
        std::cout << "=== NEURAL ACTIVITY ===" << std::endl;
        pImpl->textVisualizer.renderActivityMap(brain);
        std::cout << std::endl;
    }
    
    if (config.showMemory) {
        std::cout << "=== MEMORY SYSTEMS ===" << std::endl;
        pImpl->textVisualizer.renderMemoryState(brain);
        std::cout << std::endl;
    }
    
    if (config.showStats) {
        std::cout << "=== SYSTEM STATISTICS ===" << std::endl;
        pImpl->textVisualizer.renderStatistics(brain);
        std::cout << std::endl;
    }
    
    std::cout << vis::createFooter() << std::endl;
    std::cout << "Press Ctrl+C to stop visualization" << std::endl;
}

void BrainVisualizer::exportVisualization(std::shared_ptr<Brain> brain, const std::string& filename) {
    NLM_LOG_INFO("Exporting visualization to: " + filename);
    // Implementation would write visualization data to file
    // For now, just log the action
    std::cout << "Exporting visualization to: " << filename << std::endl;
}

void BrainVisualizer::startPerformanceMonitoring() {
    pImpl->startTime = std::chrono::steady_clock::now();
}

double BrainVisualizer::getCurrentFPS() const {
    return pImpl->fps;
}

double BrainVisualizer::getAverageFrameTime() const {
    return pImpl->averageFrameTime;
}

void BrainVisualizer::stopVisualization() {
    pImpl->running = false;
}

bool BrainVisualizer::isRunning() const {
    return pImpl->running;
}

void BrainVisualizer::setConfig(const VisualizationConfig& config) {
    pImpl->config = config;
}

const VisualizationConfig& BrainVisualizer::getConfig() const {
    return pImpl->config;
}

// TextNeuronVisualizer implementation
void TextNeuronVisualizer::renderNeuronState(const Neuron* neuron, bool highlight) {
    if (!neuron) return;
    
    auto state = neuron->getState();
    float activity = std::abs(state.membranePotential - state.restingPotential);
    
    if (highlight) {
        setColor(33);  // Highlight color (blue background)
    }
    
    std::cout << "Neuron " << std::setw(3) << neuron->getId().index()
              << " [" << std::fixed << std::setprecision(1) << activity << "]"
              << " V:" << std::setw(6) << std::setprecision(1) << state.membranePotential
              << " R:" << std::setw(6) << std::setprecision(1) << state.restingPotential
              << " T:" << std::setw(6) << std::setprecision(1) << state.threshold;
    
    if (highlight) {
        resetColor();
    }
    
    std::cout << std::endl;
}

void TextNeuronVisualizer::renderNetworkState(std::shared_ptr<Brain> brain) {
    std::cout << "Total Neurons: " << brain->getTotalNeuronCount()
              << " | Total Synapses: " << brain->getTotalSynapseCount()
              << " | Firing Neurons: " << brain->getFiringNeuronCount()
              << " | Activity Ratio: " << std::fixed << std::setprecision(3)
              << (brain->getTotalNeuronCount() > 0 ? 
                  static_cast<float>(brain->getFiringNeuronCount()) / brain->getTotalNeuronCount() : 0.0f)
              << std::endl;
    
    // Show some example neurons
    size_t showCount = std::min(size_t(5), brain->getTotalNeuronCount());
    for (size_t i = 0; i < showCount; ++i) {
        auto* region = brain->getRegion(RegionId(i % brain->getRegionCount() + 1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty() && i < neurons.size()) {
                renderNeuronState(neurons[i], neurons[i]->isFiring());
            }
        }
    }
}

void TextNeuronVisualizer::renderActivityMap(std::shared_ptr<Brain> brain) {
    std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(3)
              << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Pending Spike Events: " << brain->getPendingSpikeEventCount() << std::endl;
    
    // Activity histogram (simplified)
    size_t firingNeurons = brain->getFiringNeuronCount();
    if (firingNeurons > 0) {
        std::cout << "\nActivity Histogram (sample of firing neurons):" << std::endl;
        for (int i = 0; i < 10; ++i) {
            float percentage = static_cast<float>(i + 1) * 10.0f;
            std::cout << "  " << std::setw(3) << i*10 << "-" << std::setw(3) << (i+1)*10 << "%: ";
            
            int bars = static_cast<int>(percentage / 100.0f * 40.0f);
            for (int j = 0; j < bars; ++j) {
                std::cout << "█";
            }
            std::cout << std::endl;
        }
    }
}

void TextNeuronVisualizer::renderMemoryState(std::shared_ptr<Brain> brain) {
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* associativeMemory = brain->getAssociativeMemory();
    
    if (workingMemory) {
        std::cout << "Working Memory: " << workingMemory->getMemoryNeuronCount()
                  << " active neurons, capacity: " << workingMemory->getCapacity()
                  << " (" << std::fixed << std::setprecision(1)
                  << (workingMemory->getCapacity() > 0 ? 
                      static_cast<float>(workingMemory->getMemoryNeuronCount()) / workingMemory->getCapacity() * 100.0f : 0.0f)
                  << "% full)" << std::endl;
    }
    
    if (episodicMemory) {
        std::cout << "Episodic Memory: " << episodicMemory->getEpisodeCount()
                  << " episodes stored" << std::endl;
    }
    
    if (associativeMemory) {
        // Associative memory doesn't have a simple getCount() method
        std::cout << "Associative Memory: Active pattern associations" << std::endl;
    }
}

void TextNeuronVisualizer::renderStatistics(std::shared_ptr<Brain> brain) {
    std::cout << "Brain Statistics:" << std::endl;
    std::cout << "  Version: 0.1.0 (Phase 6)" << std::endl;
    std::cout << "  Development Stage: " << (brain ? brain->getDevelopmentalStage() : "Unknown") << std::endl;
    std::cout << "  System Status: Operational" << std::endl;
    std::cout << "  Total Neurons: " << (brain ? brain->getTotalNeuronCount() : 0) << std::endl;
    std::cout << "  Average Firing Rate: " << (brain ? brain->getAverageFiringRate() : 0.0f) << " Hz" << std::endl;
}

void TextNeuronVisualizer::clearScreen() {
    // ANSI escape code to clear screen
    std::cout << "\033[2J\033[1;1H";
}

void TextNeuronVisualizer::setColor(int colorCode) {
    // ANSI escape code for colors
    std::cout << "\033[" << colorCode << "m";
}

void TextNeuronVisualizer::resetColor() {
    std::cout << "\033[0m";
}

// vis namespace implementations
namespace vis {
    std::string formatActivityLevel(float firingRate) {
        if (firingRate < 1.0f) return "LOW";
        if (firingRate < 10.0f) return "MEDIUM";
        if (firingRate < 50.0f) return "HIGH";
        return "VERY HIGH";
    }
    
    std::string formatWeight(float weight) {
        if (weight < 0.0f) return "INHIB";
        if (weight < 0.5f) return "WEAK";
        if (weight < 1.0f) return "MODERATE";
        if (weight < 2.0f) return "STRONG";
        return "VERY STRONG";
    }
    
    std::string formatMemorySize(size_t size) {
        if (size < 1024) return std::to_string(size) + " bytes";
        if (size < 1024 * 1024) return std::to_string(size / 1024) + " KB";
        if (size < 1024 * 1024 * 1024) return std::to_string(size / (1024 * 1024)) + " MB";
        return std::to_string(size / (1024 * 1024 * 1024)) + " GB";
    }
    
    std::string formatTime(double seconds) {
        if (seconds < 60.0) return std::to_string(seconds) + "s";
        if (seconds < 3600.0) return std::to_string(seconds / 60.0) + "min";
        return std::to_string(seconds / 3600.0) + "h";
    }
    
    int getActivityColor(float firingRate) {
        if (firingRate < 1.0f) return 32;   // Green
        if (firingRate < 10.0f) return 33;  // Blue
        if (firingRate < 50.0f) return 208; // Orange
        return 31;                         // Red
    }
    
    int getWeightColor(float weight) {
        if (weight < 0.0f) return 90;      // Cyan
        if (weight < 0.5f) return 32;      // Green
        if (weight < 1.0f) return 33;      // Blue
        if (weight < 2.0f) return 208;     // Orange
        return 31;                         // Red
    }
    
    int getMemoryColor(float utilization) {
        if (utilization < 0.5f) return 32;   // Green
        if (utilization < 0.8f) return 33;   // Blue
        if (utilization < 0.95f) return 208; // Orange
        return 31;                         // Red
    }
    
    std::string createHeader(const std::string& title) {
        return "╔═══════════════════════════════════════════════════════════════╗\n"
               "║                   " + title + "                   ║\n"
               "╚═══════════════════════════════════════════════════════════════╝";
    }
    
    std::string createSeparator(char separator) {
        return std::string(72, separator);
    }
    
    std::string createFooter() {
        return "═══════════════════════════════════════════════════════════════";
    }
}

} // namespace nlm
