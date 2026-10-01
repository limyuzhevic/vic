#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <chrono>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    size_t lastRenderedStep;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    // Visualization configuration
    bool showNetworkTopology;
    bool showActivityMaps;
    bool showSpikeRasters;
    bool showWeightDistributions;
    bool showStatistics;
    bool showRegionInfo;
    bool showNeuronsPerRegion;
    
    // Display options
    int consoleWidth;
    int consoleHeight;
    bool useColors;
    
    Impl() : 
        active(false),
        updateRate(30.0),
        lastRenderedStep(0),
        lastUpdateTime(std::chrono::steady_clock::now()),
        showNetworkTopology(true),
        showActivityMaps(true),
        showSpikeRasters(true),
        showWeightDistributions(true),
        showStatistics(true),
        showRegionInfo(true),
        showNeuronsPerRegion(true),
        consoleWidth(120),
        consoleHeight(40),
        useColors(true) {
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize visualization system
    pImpl->active = true;
    
    // Get console size (if possible)
    #ifdef _WIN32
    // Windows console info
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    pImpl->consoleWidth = csbi.srWindow.right - csbi.srWindow.left + 1;
    pImpl->consoleHeight = csbi.srWindow.bottom - csbi.srWindow.top + 1;
    #else
    // Try to get terminal size
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        pImpl->consoleWidth = w.ws_col;
        pImpl->consoleHeight = w.ws_row;
    }
    #endif
    
    NLM_LOG_INFO("Visualization interface initialized with " + 
                 std::to_string(pImpl->consoleWidth) + " x " + 
                 std::to_string(pImpl->consoleHeight) + " console dimensions");
    
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    // Check if it's time to update visualization
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastUpdateTime);
    
    if (elapsed.count() >= static_cast<int>(1.0 / pImpl->updateRate)) {
        pImpl->lastUpdateTime = now;
        renderFrame();
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    renderFrame();
}

void VisualizationInterface::renderFrame() {
    // Clear screen (ANSI escape code)
    std::cout << "\033[2J\033[H";
    
    // Print header
    printHeader();
    
    // Render different visualization panels based on configuration
    int currentLine = 2;
    
    if (pImpl->showNetworkTopology) {
        currentLine = renderNetworkTopology(currentLine);
    }
    
    if (pImpl->showActivityMaps) {
        currentLine = renderActivityMaps(currentLine);
    }
    
    if (pImpl->showSpikeRasters) {
        currentLine = renderSpikeRasters(currentLine);
    }
    
    if (pImpl->showWeightDistributions) {
        currentLine = renderWeightDistributions(currentLine);
    }
    
    if (pImpl->showStatistics) {
        currentLine = renderStatistics(currentLine);
    }
    
    if (pImpl->showRegionInfo) {
        currentLine = renderRegionInfo(currentLine);
    }
    
    if (pImpl->showNeuronsPerRegion) {
        currentLine = renderNeuronsPerRegion(currentLine);
    }
    
    // Print footer
    printFooter(currentLine);
}

void VisualizationInterface::printHeader() {
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                             NLM NEURAL LEARNING MACHINE VISUALIZATION                              ║\n";
    std::cout << "║                                Phase 6 - Integrated Brain                                           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

int VisualizationInterface::renderNetworkTopology(int startLine) {
    int currentLine = startLine;
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                              NETWORK TOPOLOGY VISUALIZATION                                      ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    
    if (!brain_) {
        std::cout << "║                              No brain data available                                             ║\n";
        return currentLine + 5;
    }
    
    // Simple ASCII representation of brain regions
    const auto& regions = brain_->getRegions();
    int maxNeurons = 0;
    for (const auto& region : regions) {
        int neuronCount = region->getTotalNeuronCount();
        maxNeurons = std::max(maxNeurons, neuronCount);
    }
    
    std::cout << "║                              Brain Regions: " << regions.size() << "                                           ║\n";
    std::cout << "║                              Total Neurons: " << brain_->getTotalNeuronCount() << "                                 ║\n";
    std::cout << "║                              Total Synapses: " << brain_->getTotalSynapseCount() << "                                 ║\n";
    std::cout << "║                                                                                                  ║\n";
    
    // Draw regions in a circular arrangement
    float angleStep = 2.0f * 3.14159f / regions.size();
    for (size_t i = 0; i < regions.size(); ++i) {
        float angle = i * angleStep;
        int x = static_cast<int>(40 + 25 * std::cos(angle));
        int y = static_cast<int>(10 + 15 * std::sin(angle));
        
        std::cout << "║";
        
        // Draw region name
        std::string regionName = regions[i]->getName();
        if (regionName.length() > 15) {
            regionName = regionName.substr(0, 12) + "...";
        }
        
        int nameX = x - static_cast<int>(regionName.length()) / 2;
        int neuronCount = regions[i]->getTotalNeuronCount();
        
        // Draw neurons as dots
        for (int dy = -3; dy <= 3; ++dy) {
            for (int dx = -8; dx <= 8; ++dx) {
                int drawX = nameX + dx;
                int drawY = y + dy;
                
                if (drawY >= 2 && drawY < pImpl->consoleHeight - 2 && 
                    drawX >= 2 && drawX < pImpl->consoleWidth - 2) {
                    
                    // Calculate activity level for display
                    float activityLevel = static_cast<float>(regions[i]->getActiveNeuronCount()) / 
                                        std::max(1.0f, static_cast<float>(regions[i]->getTotalNeuronCount()));
                    
                    if (activityLevel > 0.8f) {
                        std::cout << "▓";
                    } else if (activityLevel > 0.6f) {
                        std::cout << "▒";
                    } else if (activityLevel > 0.4f) {
                        std::cout << "░";
                    } else if (activityLevel > 0.2f) {
                        std::cout << "·";
                    } else {
                        std::cout << " ";
                    }
                }
            }
        }
        
        std::cout << " " << regionName << " (" << neuronCount << " neurons) ║\n";
    }
    
    std::cout << "╚═════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    
    return currentLine + 10;
}

int VisualizationInterface::renderActivityMaps(int startLine) {
    int currentLine = startLine;
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                 ACTIVITY MAPS VISUALIZATION                                     ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    
    if (!brain_) {
        std::cout << "║                              No brain data available                                             ║\n";
        return currentLine + 5;
    }
    
    // Display activity for each region
    const auto& regions = brain_->getRegions();
    
    for (size_t i = 0; i < regions.size(); ++i) {
        const auto& region = regions[i];
        int neuronCount = region->getTotalNeuronCount();
        int activeCount = region->getActiveNeuronCount();
        float activityLevel = static_cast<float>(activeCount) / std::max(1.0f, static_cast<float>(neuronCount));
        
        std::cout << "║  Region " << i+1 << ": ";
        
        // Draw activity bar
        int barWidth = 40;
        int filledWidth = static_cast<int>(activityLevel * barWidth);
        
        if (pImpl->useColors) {
            if (activityLevel > 0.8f) {
                std::cout << "\033[91m";  // Red
            } else if (activityLevel > 0.6f) {
                std::cout << "\033[93m";  // Yellow
            } else if (activityLevel > 0.4f) {
                std::cout << "\033[94m";  // Blue
            } else if (activityLevel > 0.2f) {
                std::cout << "\033[92m";  // Green
            } else {
                std::cout << "\033[90m";  // Gray
            }
        }
        
        for (int j = 0; j < barWidth; ++j) {
            if (j < filledWidth) {
                std::cout << "█";
            } else {
                std::cout << " ";
            }
        }
        
        if (pImpl->useColors) {
            std::cout << "\033[0m";  // Reset color
        }
        
        std::cout << " (" << activeCount << "/" << neuronCount << ") " << std::fixed << std::setprecision(2) 
                  << activityLevel * 100.0f << "%                               ║\n";
    }
    
    std::cout << "╚═════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    
    return currentLine + regions.size() + 4;
}

int VisualizationInterface::renderSpikeRasters(int startLine) {
    int currentLine = startLine;
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                   SPIKE RASTER PLOTS                                            ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    
    if (!brain_) {
        std::cout << "║                              No brain data available                                             ║\n";
        return currentLine + 5;
    }
    
    std::cout << "║  (Spike visualization requires spike history data)                                            ║\n";
    std::cout << "║  Current spike count: " << brain_->getTotalSpikeCount() << "                                                       ║\n";
    std::cout << "║  Firing neurons this step: " << brain_->getFiringNeuronCount() << "                                 ║\n";
    std::cout << "║                                                                                                  ║\n";
    
    // Simple ASCII spike raster (last 50 spikes)
    std::vector<float> recentSpikes;
    if (brain_->getSpikeSystem()) {
        size_t maxSpikes = std::min<size_t>(50, brain_->getSpikeSystem()->getPendingSpikeCount());
        for (size_t i = 0; i < maxSpikes; ++i) {
            recentSpikes.push_back(static_cast<float>(i) / maxSpikes);
        }
    }
    
    if (!recentSpikes.empty()) {
        std::cout << "║  Spike raster (last " << recentSpikes.size() << " spikes):                                     ║\n";
        for (size_t i = 0; i < std::min<size_t>(recentSpikes.size(), 10); ++i) {
            std::cout << "║    ";
            for (int j = 0; j < 50; ++j) {
                if (j < recentSpikes[i] * 50) {
                    std::cout << "*";
                } else {
                    std::cout << " ";
                }
            }
            std::cout << "                                                       ║\n";
        }
    }
    
    std::cout << "╚═════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    
    return currentLine + 12;
}

int VisualizationInterface::renderWeightDistributions(int startLine) {
    int currentLine = startLine;
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                  WEIGHT DISTRIBUTIONS                                            ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    
    if (!brain_) {
        std::cout << "║                              No brain data available                                             ║\n";
        return currentLine + 5;
    }
    
    std::cout << "║  Weight statistics:                                                             ║\n";
    
    // Calculate weight statistics
    float minWeight = std::numeric_limits<float>::max();
    float maxWeight = std::numeric_limits<float>::lowest();
    float sumWeights = 0.0f;
    int synapseCount = brain_->getTotalSynapseCount();
    
    if (synapseCount > 0) {
        for (const auto& region : brain_->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                float weight = syn->getWeight();
                minWeight = std::min(minWeight, weight);
                maxWeight = std::max(maxWeight, weight);
                sumWeights += weight;
            }
        }
        
        float avgWeight = sumWeights / synapseCount;
        
        std::cout << "║    Total synapses: " << synapseCount << "                                                   ║\n";
        std::cout << "║    Min weight: " << std::fixed << std::setprecision(3) << minWeight << "                                 ║\n";
        std::cout << "║    Max weight: " << std::fixed << std::setprecision(3) << maxWeight << "                                 ║\n";
        std::cout << "║    Average weight: " << std::fixed << std::setprecision(3) << avgWeight << "                          ║\n";
        std::cout << "║    Weight range: " << std::fixed << std::setprecision(3) << (maxWeight - minWeight) << "                     ║\n";
    }
    
    std::cout << "║                                                                                                  ║\n";
    
    // Draw weight histogram
    std::cout << "║    Weight distribution histogram:                                                   ║\n";
    
    // Simple histogram bars
    for (int bin = 0; bin < 10; ++bin) {
        std::cout << "║      ";
        
        // Count synapses in this bin
        float binMin = -1.0f + bin * 0.2f;
        float binMax = binMin + 0.2f;
        
        int count = 0;
        for (const auto& region : brain_->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                float weight = syn->getWeight();
                if (weight >= binMin && weight < binMax) {
                    count++;
                }
            }
        }
        
        int barHeight = (count * 20) / synapseCount;
        for (int h = 0; h < barHeight; ++h) {
            std::cout << "░";
        }
        std::cout << "  " << binMin << "-" << binMax << " : " << count << " synapses      ║\n";
    }
    
    std::cout << "╚═════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    
    return currentLine + 20;
}

int VisualizationInterface::renderStatistics(int startLine) {
    int currentLine = startLine;
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                      SYSTEM STATISTICS                                        ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    
    if (!brain_) {
        std::cout << "║                              No brain data available                                             ║\n";
        return currentLine + 5;
    }
    
    // Get simulation statistics
    float avgFiringRate = brain_->getAverageFiringRate();
    float eI = brain_->getExcitationInhibitionRatio();
    size_t pendingSpikes = brain_->getPendingSpikeEventCount();
    
    std::cout << "║    Simulation statistics:                                                            ║\n";
    std::cout << "║    Average firing rate: " << std::fixed << std::setprecision(3) << avgFiringRate << " spikes/ms   ║\n";
    std::cout << "║    Excitation/Inhibition ratio: " << std::fixed << std::setprecision(3) << eI << "                 ║\n";
    std::cout << "║    Pending spike events: " << pendingSpikes << "                              ║\n";
    std::cout << "║    Total spikes generated: " << brain_->getTotalSpikeCount() << "                ║\n";
    
    // Memory statistics
    if (brain_->getWorkingMemory()) {
        std::cout << "║                                                                                                  ║\n";
        std::cout << "║    Working memory: " << brain_->getWorkingMemory()->getActiveTraces() << " traces  ║\n";
    }
    
    if (brain_->getEpisodicMemory()) {
        std::cout << "║    Episodic memory: " << brain_->getEpisodicMemory()->getEpisodeCount() << " episodes      ║\n";
    }
    
    // Neuromodulation statistics
    if (brain_->getDopamine()) {
        std::cout << "║    Dopamine level: " << std::fixed << std::setprecision(3) << brain_->getDopamine()->getLevel() 
                  << "                        ║\n";
    }
    
    if (brain_->getCuriosity()) {
        std::cout << "║    Curiosity level: " << std::fixed << std::setprecision(3) << brain_->getCuriosity()->getLevel() 
                  << "                       ║\n";
    }
    
    if (brain_->getNovelty()) {
        std::cout << "║    Novelty level: " << std::fixed << std::setprecision(3) << brain_->getNovelty()->getLevel() 
                  << "                         ║\n";
    }
    
    std::cout << "╚═════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    
    return currentLine + 15;
}

int VisualizationInterface::renderRegionInfo(int startLine) {
    int currentLine = startLine;
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                        REGION INFORMATION                                       ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    
    if (!brain_) {
        std::cout << "║                              No brain data available                                             ║\n";
        return currentLine + 5;
    }
    
    const auto& regions = brain_->getRegions();
    
    for (size_t i = 0; i < regions.size(); ++i) {
        const auto& region = regions[i];
        
        std::cout << "║  Region " << i+1 << ": " << region->getName() << "                                           ║\n";
        std::cout << "║    Neurons: " << region->getTotalNeuronCount() << "                                              ║\n";
        std::cout << "║    Active neurons: " << region->getActiveNeuronCount() << "                                     ║\n";
        std::cout << "║    Synapses: " << region->getSynapseCount() << "                                              ║\n";
        
        float avgWeight = region->getAverageSynapticWeight();
        std::cout << "║    Average synaptic weight: " << std::fixed << std::setprecision(3) << avgWeight << "      ║\n";
        
        if (i < regions.size() - 1) {
            std::cout << "║                                                                                                  ║\n";
        }
    }
    
    std::cout << "╚═════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    
    return currentLine + regions.size() * 4 + 4;
}

int VisualizationInterface::renderNeuronsPerRegion(int startLine) {
    int currentLine = startLine;
    
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                      NEURONS PER REGION                                        ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    
    if (!brain_) {
        std::cout << "║                              No brain data available                                             ║\n";
        return currentLine + 5;
    }
    
    const auto& regions = brain_->getRegions();
    
    for (size_t i = 0; i < regions.size(); ++i) {
        const auto& region = regions[i];
        int neuronCount = region->getTotalNeuronCount();
        
        std::cout << "║  Region " << i+1 << ": ";
        
        // Draw neuron representation
        int maxNeurons = 20;
        int neuronsToDraw = std::min(neuronCount, maxNeurons);
        
        for (int j = 0; j < neuronsToDraw; ++j) {
            // Make some neurons "active" based on activity level
            float activityLevel = static_cast<float>(region->getActiveNeuronCount()) / 
                                std::max(1.0f, static_cast<float>(neuronCount));
            
            if (j < neuronsToDraw * activityLevel) {
                std::cout << "●";
            } else {
                std::cout << "○";
            }
        }
        
        if (neuronCount > maxNeurons) {
            std::cout << " ... +" << (neuronCount - maxNeurons) << " more";
        }
        
        std::cout << std::string(55 - neuronsToDraw, ' ') << "║\n";
    }
    
    std::cout << "╚═════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    
    return currentLine + regions.size() + 4;
}

void VisualizationInterface::printFooter(int currentLine) {
    std::cout << "\n"; // Add spacing
    
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    std::cout << "                               Visualization System v2.0                                          \n";
    std::cout << "                              Phase 6 - Real Neural Computation                                  \n";
    std::cout << "════════════════════════════════════════════════════════════════════════════════════════════════════════════════\n";
    
    // Control options
    std::cout << "\nControls:\n";
    std::cout << "  R - Toggle network topology visualization\n";
    std::cout << "  A - Toggle activity maps\n";
    std::cout << "  S - Toggle spike raster plots\n";
    std::cout << "  W - Toggle weight distributions\n";
    std::cout << "  T - Toggle statistics\n";
    std::cout << "  I - Toggle region information\n";
    std::cout << "  N - Toggle neurons per region\n";
    std::cout << "  C - Change update rate (slow/fast)\n";
    std::cout << "  X - Exit visualization\n";
    
    std::cout << "\nNote: This visualization requires console support for ANSI escape codes.\n";
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    brain_ = &brain;
    if (brain_) {
        NLM_LOG_INFO("Starting network visualization...");
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    brain_ = &brain;
    if (brain_) {
        NLM_LOG_INFO("Starting activity visualization...");
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // Spike events are handled in renderSpikeRasters
    // This is kept for API compatibility
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    brain_ = &brain;
    if (brain_) {
        NLM_LOG_INFO("Starting weight visualization...");
    }
}

} // namespace nlm
