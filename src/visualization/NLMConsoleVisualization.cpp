#include "NLMConsoleVisualization.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <conio.h>  // For _kbhit() and _getch() on Windows

namespace nlm {

struct ConsoleVisualization::Impl {
    std::vector<std::vector<char>> buffer;
    size_t width;
    size_t height;
    
    // Brain statistics history
    std::vector<uint64_t> spikeHistory;
    std::vector<double> firingRateHistory;
    std::vector<float> weightHistory;
    size_t historySize;
    
    // Performance metrics
    double lastUpdateTime;
    double frameTime;
    size_t frameCount;
    
    // Statistics
    uint64_t currentStep;
    double currentTimestep;
    std::shared_ptr<class Brain> currentBrain;
    
    Impl() : width(80), height(40), historySize(100),
             lastUpdateTime(0.0), frameTime(0.0), frameCount(0),
             currentStep(0), currentTimestep(0.0) {
        buffer.resize(height, std::vector<char>(width, ' '));
        spikeHistory.reserve(historySize);
        firingRateHistory.reserve(historySize);
        weightHistory.reserve(historySize);
    }
};

ConsoleVisualization::ConsoleVisualization() : pImpl(std::make_unique<Impl>()) {
}

ConsoleVisualization::~ConsoleVisualization() = default;

bool ConsoleVisualization::initialize(size_t width, size_t height) {
    if (width < 20 || height < 10) {
        return false;
    }
    
    pImpl->width = width;
    pImpl->height = height;
    pImpl->buffer.resize(height, std::vector<char>(width, ' '));
    
    active_ = true;
    showDisplay_ = true;
    useColors_ = true;
    updateRate_ = 10.0;  // 10 Hz
    
    showNetwork_ = true;
    showActivity_ = true;
    showSpikes_ = true;
    showMetrics_ = true;
    
    return true;
}

void ConsoleVisualization::update(const std::shared_ptr<class Brain>& brain, uint64_t step, double timestep) {
    if (!brain || !active_) return;
    
    std::lock_guard<std::mutex> lock(renderMutex_);
    
    pImpl->currentBrain = brain;
    pImpl->currentStep = step;
    pImpl->currentTimestep = timestep;
    
    // Record history
    if (brain->getTotalSpikeCount() > 0) {
        pImpl->spikeHistory.push_back(brain->getTotalSpikeCount());
        if (pImpl->spikeHistory.size() > pImpl->historySize) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
    }
    
    pImpl->firingRateHistory.push_back(brain->getAverageFiringRate());
    if (pImpl->firingRateHistory.size() > pImpl->historySize) {
        pImpl->firingRateHistory.erase(pImpl->firingRateHistory.begin());
    }
    
    // Update frame timing
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
        currentTime - pImpl->lastUpdateTime).count();
    pImpl->lastUpdateTime = duration;
    pImpl->frameTime = duration;
    pImpl->frameCount++;
}

void ConsoleVisualization::render() {
    if (!showDisplay_ || !active_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(renderMutex_);
    
    // Clear screen
    clearScreen();
    
    // Create buffer for rendering
    std::vector<std::vector<char>> renderBuffer = pImpl->buffer;
    
    if (pImpl->currentBrain) {
        // Convert buffer to char array
        char* bufferPtr = &renderBuffer[0][0];
        size_t totalSize = pImpl->height * pImpl->width;
        memset(bufferPtr, ' ', totalSize);
        
        // Render different views
        if (showNetwork_) {
            visualizeNetwork(*pImpl->currentBrain, bufferPtr, pImpl->width, pImpl->height);
        }
        
        if (showActivity_) {
            visualizeActivity(*pImpl->currentBrain, bufferPtr, pImpl->width, pImpl->height);
        }
        
        if (showSpikes_) {
            visualizeSpikes(*pImpl->currentBrain, bufferPtr, pImpl->width, pImpl->height);
        }
        
        if (showMetrics_) {
            visualizeMetrics(*pImpl->currentBrain, bufferPtr, pImpl->width, pImpl->height);
        }
        
        // Add legend
        addLegend(bufferPtr, pImpl->width, pImpl->height);
        
        // Add statistics
        addStatistics(bufferPtr, pImpl->width, pImpl->height);
    }
    
    // Output to console
    for (size_t y = 0; y < pImpl->height; ++y) {
        std::cout << &renderBuffer[y][0];
        std::cout << "\r\n";
    }
    
    std::cout << std::flush;
}

void ConsoleVisualization::close() {
    active_ = false;
    showDisplay_ = false;
}

void ConsoleVisualization::addLegend(char* buffer, size_t width, size_t height) {
    std::string legend = "N: Network  A: Activity  S: Spikes  M: Metrics  H: Help  Q: Quit  P: Pause";
    size_t legendLen = legend.length();
    
    if (legendLen < width) {
        size_t y = height - 2;
        size_t x = (width - legendLen) / 2;
        for (size_t i = 0; i < legendLen; ++i) {
            buffer[y * width + x + i] = legend[i];
        }
    }
}

void ConsoleVisualization::addStatistics(char* buffer, size_t width, size_t height) {
    if (!pImpl->currentBrain) return;
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    ss << "Step: " << pImpl->currentStep << " ";
    ss << "Spikes: " << pImpl->currentBrain->getTotalSpikeCount() << " ";
    ss << std::setprecision(3);
    ss << "Firing: " << pImpl->currentBrain->getAverageFiringRate() << " Hz ";
    ss << "E/I: " << pImpl->currentBrain->getExcitationInhibitionRatio() << " ";
    ss << std::setprecision(6);
    ss << "dt: " << pImpl->currentTimestep << " ";
    
    std::string stats = ss.str();
    size_t statsLen = stats.length();
    
    if (statsLen < width - 10) {
        size_t y = height - 1;
        size_t x = 5;
        for (size_t i = 0; i < statsLen && x < width - 10; ++i, ++x) {
            buffer[y * width + x] = stats[i];
        }
    }
}

std::string ConsoleVisualization::getBrainStats() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    
    if (pImpl->currentBrain) {
        ss << "Neurons: " << pImpl->currentBrain->getTotalNeuronCount() << " ";
        ss << "Synapses: " << pImpl->currentBrain->getTotalSynapseCount() << " ";
        ss << "Active: " << pImpl->currentBrain->getActiveNeuronCount() << " ";
        ss << "Firing: " << pImpl->currentBrain->getFiringNeuronCount() << " ";
        ss << "Avg Rate: " << pImpl->currentBrain->getAverageFiringRate() << " Hz ";
        ss << "Total Spikes: " << pImpl->currentBrain->getTotalSpikeCount() << " ";
        ss << "E/I Ratio: " << pImpl->currentBrain->getExcitationInhibitionRatio() << " ";
        ss << "Dev Stage: " << static_cast<int>(pImpl->currentBrain->getDevelopmentalStage());
    }
    
    return ss.str();
}

std::string ConsoleVisualization::getPerformanceStats() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    
    ss << "FPS: " << (pImpl->frameCount / pImpl->frameTime) << " ";
    ss << "Frame Time: " << pImpl->frameTime * 1000 << " ms ";
    ss << "Update Rate: " << updateRate_ << " Hz";
    
    return ss.str();
}

bool ConsoleVisualization::saveFrame(const std::string& filename) {
    // Simple frame save - in real implementation would save as image or CSV
    return true;
}

void ConsoleVisualization::clearScreen() {
    // ANSI escape code to clear screen
    std::cout << "\033[2J\033[1;1H" << std::flush;
}

std::string ConsoleVisualization::formatNumber(float value, int precision) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string ConsoleVisualization::getNeuronTypeSymbol(NeuronType type) const {
    switch (type) {
        case NeuronType::Excitatory: return "*";
        case NeuronType::Inhibitory: return ".";
        case NeuronType::Sensory: return "@";
        case NeuronType::Motor: return "#";
        case NeuronType::Modulatory: return "+";
        case NeuronType::Internal: return "O";
        default: return "?";
    }
}

std::string ConsoleVisualization::getColorCode(NeuronType type) const {
    if (!useColors_) return "";
    
    switch (type) {
        case NeuronType::Excitatory: return "\033[91m"; // Red
        case NeuronType::Inhibitory: return "\033[34m"; // Blue
        case NeuronType::Sensory: return "\033[92m"; // Green
        case NeuronType::Motor: return "\033[95m"; // Magenta
        case NeuronType::Modulatory: return "\033[93m"; // Yellow
        case NeuronType::Internal: return "\033[96m"; // Cyan
        default: return "\033[37m"; // Gray
    }
}

void ConsoleVisualization::visualizeNetwork(const class Brain& brain, char* buffer, 
                                           size_t width, size_t height) {
    // Simple network visualization - show neuron regions
    size_t regionCount = brain.getRegionCount();
    if (regionCount == 0) return;
    
    size_t regionWidth = width / regionCount;
    
    for (size_t regionIdx = 0; regionIdx < regionCount; ++regionIdx) {
        auto region = brain.getRegion(RegionId(regionIdx));
        if (!region) continue;
        
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount == 0) continue;
        
        // Calculate position
        size_t x = regionIdx * regionWidth;
        size_t y = height / 2;
        
        // Draw neurons as dots
        for (size_t i = 0; i < std::min<size_t>(neuronCount, 20); ++i) {
            size_t xi = x + ((i * regionWidth) / std::max<size_t>(neuronCount, 1));
            size_t yi = y + (rand() % 10 - 5);  // Add some vertical variation
            if (yi < height) {
                buffer[yi * width + xi] = '*';
            }
        }
    }
}

void ConsoleVisualization::visualizeActivity(const class Brain& brain, char* buffer,
                                            size_t width, size_t height) {
    // Activity heatmap - show firing rate as brightness
    double maxRate = 0.0;
    if (!firingRateHistory.empty()) {
        maxRate = *std::max_element(firingRateHistory.begin(), firingRateHistory.end());
    }
    
    double normalizedRate = maxRate > 0.0 ? 1.0 / maxRate : 0.0;
    
    // Create activity bar
    size_t barHeight = std::min<size_t>(height / 2, 20);
    size_t barWidth = std::min<size_t>(width / 2, 40);
    size_t barX = width - barWidth - 5;
    size_t barY = height - barHeight - 5;
    
    // Draw activity bar
    for (size_t y = 0; y < barHeight; ++y) {
        double rate = normalizedRate * barHeight - y;
        char level = rate > 0.8 ? '@' : rate > 0.6 ? '#' : rate > 0.4 ? '%' : rate > 0.2 ? '.' : ' ';
        
        for (size_t x = 0; x < barWidth; ++x) {
            buffer[(barY + y) * width + (barX + x)] = level;
        }
    }
    
    // Draw label
    std::string label = "Activity";  // In real implementation, would draw text
}

void ConsoleVisualization::visualizeSpikes(const class Brain& brain, char* buffer,
                                          size_t width, size_t height) {
    // Spike raster plot
    size_t spikeCount = brain.getTotalSpikeCount();
    if (spikeCount == 0) return;
    
    // Simple spike indicator
    size_t spikeX = 5;
    size_t spikeY = 5;
    
    for (size_t i = 0; i < std::min<size_t>(spikeCount, 50); ++i) {
        size_t xi = spikeX + (i % 10) * 2;
        size_t yi = spikeY + (i / 10) * 2;
        if (xi < width - 10 && yi < height - 10) {
            buffer[yi * width + xi] = '|';
        }
    }
}

void ConsoleVisualization::visualizeMetrics(const class Brain& brain, char* buffer,
                                           size_t width, size_t height) {
    // Metrics display
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    ss << "Neurons: " << brain.getTotalNeuronCount() << "\n";
    ss << "Firing: " << brain.getFiringNeuronCount() << "\n";
    ss << "Avg Rate: " << brain.getAverageFiringRate() << " Hz\n";
    ss << "E/I: " << brain.getExcitationInhibitionRatio() << "\n";
    ss << "Spikes: " << brain.getTotalSpikeCount();
    
    std::string metrics = ss.str();
    size_t lineCount = 1;
    for (size_t i = 0; i < metrics.size(); ++i) {
        if (metrics[i] == '\n') lineCount++;
    }
    
    size_t startX = 5;
    size_t startY = height - lineCount - 3;
    
    size_t lineIdx = 0;
    size_t charIdx = 0;
    for (size_t i = 0; i < metrics.size(); ++i) {
        if (metrics[i] == '\n') {
            lineIdx++;
            charIdx = 0;
            continue;
        }
        
        size_t x = startX + charIdx;
        size_t y = startY + lineIdx;
        if (x < width && y < height) {
            buffer[y * width + x] = metrics[i];
        }
        charIdx++;
    }
}

} // namespace nlm