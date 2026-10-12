#include "ActivityVisualizer.hpp"
#include "../core/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>

namespace nlm {

struct ActivityVisualizer::Impl {
    // Spike data
    std::vector<SpikeEventInfo> recentSpikes;
    std::deque<SpikeEventInfo> spikeHistory;
    size_t maxSpikesPerNeuron;
    double timeWindow; // seconds
    
    // Membrane potential traces
    std::vector<MembranePotentialTrace> traces;
    std::map<NeuronId, std::deque<MembranePotentialTrace>> neuronTraces;
    size_t traceHistorySize;
    
    // Heatmap data
    std::vector<FiringRateHeatmap> heatmapData;
    int heatmapWidth;
    int heatmapHeight;
    
    // Activity pattern
    BrainActivityPattern currentPattern;
    
    // Configuration
    bool autoUpdate;
    
    // Timing
    Timestamp lastTimestamp;
    bool initialized;
    
    Impl() : maxSpikesPerNeuron(1000), timeWindow(10.0), traceHistorySize(1000),
             heatmapWidth(100), heatmapHeight(100), autoUpdate(true),
             lastTimestamp(0), initialized(false) {}
};

ActivityVisualizer::ActivityVisualizer() : pImpl(std::make_unique<Impl>()) {}
ActivityVisualizer::~ActivityVisualizer() = default;

bool ActivityVisualizer::initialize() {
    try {
        pImpl->initialized = true;
        Logger::getInstance().logInfo("Activity visualizer initialized");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to initialize activity visualizer: " + std::string(e.what()));
        return false;
    }
}

void ActivityVisualizer::update(double deltaTime) {
    if (!pImpl->initialized) return;
    
    // Clean old data
    Timestamp currentTime = pImpl->lastTimestamp + static_cast<Timestamp>(deltaTime * 1000.0); // Convert to milliseconds
    
    // Remove spikes outside time window
    auto it = std::remove_if(pImpl->spikeHistory.begin(), pImpl->spikeHistory.end(),
        [currentTime, window = pImpl->timeWindow](const SpikeEventInfo& spike) {
            return (currentTime - spike.timestamp) > (window * 1000);
        });
    pImpl->spikeHistory.erase(it, pImpl->spikeHistory.end());
    
    // Update recent spikes
    pImpl->recentSpikes.clear();
    pImpl->recentSpikes.assign(pImpl->spikeHistory.begin(), pImpl->spikeHistory.end());
    
    // Clean old traces
    for (auto& [neuronId, traces] : pImpl->neuronTraces) {
        auto it = std::remove_if(traces.begin(), traces.end(),
            [currentTime, window = pImpl->timeWindow](const MembranePotentialTrace& trace) {
                return (currentTime - trace.timestamp) > (window * 1000);
            });
        traces.erase(it, traces.end());
        if (traces.size() > pImpl->traceHistorySize) {
            traces.pop_front();
        }
    }
    
    // Update flat traces vector
    pImpl->traces.clear();
    for (const auto& [neuronId, traces] : pImpl->neuronTraces) {
        pImpl->traces.insert(pImpl->traces.end(), traces.begin(), traces.end());
    }
    
    pImpl->lastTimestamp = currentTime;
}

void ActivityVisualizer::recordSpike(const SpikeEventInfo& spike) {
    if (pImpl->initialized) {
        pImpl->spikeHistory.push_back(spike);
        
        // Keep only recent spikes
        if (pImpl->spikeHistory.size() > pImpl->maxSpikesPerNeuron * 10) {
            pImpl->spikeHistory.pop_front();
        }
    }
}

void ActivityVisualizer::recordSpikes(const std::vector<SpikeEventInfo>& spikes) {
    for (const auto& spike : spikes) {
        recordSpike(spike);
    }
}

void ActivityVisualizer::recordMembranePotential(const MembranePotentialTrace& trace) {
    if (pImpl->initialized) {
        pImpl->neuronTraces[trace.neuronId].push_back(trace);
        
        // Limit trace history per neuron
        auto& traces = pImpl->neuronTraces[trace.neuronId];
        if (traces.size() > pImpl->traceHistorySize) {
            traces.pop_front();
        }
    }
}

void ActivityVisualizer::recordMembranePotential(const std::vector<MembranePotentialTrace>& traces) {
    for (const auto& trace : traces) {
        recordMembranePotential(trace);
    }
}

void ActivityVisualizer::updateFiringRateHeatmap(const std::vector<FiringRateHeatmap>& heatmapData) {
    pImpl->heatmapData = heatmapData;
}

void ActivityVisualizer::updateBrainActivityPattern(const BrainActivityPattern& pattern) {
    pImpl->currentPattern = pattern;
}

void ActivityVisualizer::renderSpikeRaster(const std::vector<SpikeEventInfo>& spikes) {
    renderSpikeRaster(spikes, "Spike Raster Plot");
}

void ActivityVisualizer::renderSpikeRaster(const std::vector<SpikeEventInfo>& spikes, const std::string& title) {
    // Simple text-based raster plot
    std::cout << "=== " << title << " ===" << std::endl;
    std::cout << "Total Spikes: " << spikes.size() << std::endl;
    
    if (spikes.empty()) {
        std::cout << "No spikes recorded." << std::endl;
        return;
    }
    
    // Group by neuron
    std::map<NeuronId, std::vector<Timestamp>> neuronSpikes;
    for (const auto& spike : spikes) {
        neuronSpikes[spike.neuronId].push_back(spike.timestamp);
    }
    
    // Sort neurons by type and id for better visualization
    std::vector<NeuronId> neuronIds;
    for (const auto& pair : neuronSpikes) {
        neuronIds.push_back(pair.first);
    }
    std::sort(neuronIds.begin(), neuronIds.end());
    
    // Generate raster
    for (size_t i = 0; i < neuronIds.size(); ++i) {
        const auto& neuronId = neuronIds[i];
        const auto& timestamps = neuronSpikes[neuronId];
        
        std::cout << "Neuron " << neuronId << ": ";
        for (const auto& ts : timestamps) {
            std::cout << ts << " ";
        }
        std::cout << std::endl;
    }
}

void ActivityVisualizer::renderMembranePotentialTraces(const std::vector<MembranePotentialTrace>& traces) {
    std::cout << "=== Membrane Potential Traces ===" << std::endl;
    
    if (traces.empty()) {
        std::cout << "No traces recorded." << std::endl;
        return;
    }
    
    // Group by neuron
    std::map<NeuronId, std::vector<MembranePotentialTrace>> neuronTraces;
    for (const auto& trace : traces) {
        neuronTraces[trace.neuronId].push_back(trace);
    }
    
    // Render traces
    for (const auto& [neuronId, traceVector] : neuronTraces) {
        std::cout << "Neuron " << neuronId << " (Type: " << static_cast<int>(traceVector[0].neuronType) 
                  << ", Region: " << traceVector[0].regionName << "):" << std::endl;
        
        for (const auto& trace : traceVector) {
            std::cout << "  Time: " << trace.timestamp << ", Potential: " << trace.potential 
                      << ", Derivative: " << trace.derivative << std::endl;
        }
    }
}

void ActivityVisualizer::renderMembranePotentialTraces(const std::vector<NeuronId>& neuronIds) {
    std::cout << "=== Membrane Potential Traces (Selected Neurons) ===" << std::endl;
    
    for (const auto& neuronId : neuronIds) {
        auto it = pImpl->neuronTraces.find(neuronId);
        if (it != pImpl->neuronTraces.end()) {
            std::cout << "Neuron " << neuronId << ": " << it->second.size() << " traces recorded" << std::endl;
        }
    }
}

void ActivityVisualizer::renderFiringRateHeatmap(const std::vector<FiringRateHeatmap>& heatmapData) {
    std::cout << "=== Firing Rate Heatmap ===" << std::endl;
    
    if (heatmapData.empty()) {
        std::cout << "No heatmap data available." << std::endl;
        return;
    }
    
    // Simple text-based heatmap
    std::cout << "Heatmap dimensions: " << pImpl->heatmapWidth << " x " << pImpl->heatmapHeight << std::endl;
    std::cout << "Data points: " << heatmapData.size() << std::endl;
    
    // Create a simple ASCII heatmap
    std::map<int, std::map<int, float>> heatmapGrid;
    for (const auto& point : heatmapData) {
        if (point.x >= 0 && point.x < pImpl->heatmapWidth && 
            point.y >= 0 && point.y < pImpl->heatmapHeight) {
            heatmapGrid[point.y][point.x] = point.firingRate;
        }
    }
    
    std::cout << "Heatmap visualization:" << std::endl;
    for (int y = pImpl->heatmapHeight - 1; y >= 0; --y) {
        std::cout << "  Row " << y << ": ";
        for (int x = 0; x < pImpl->heatmapWidth; ++x) {
            auto it_y = heatmapGrid.find(y);
            if (it_y != heatmapGrid.end()) {
                auto it_x = it_y->second.find(x);
                if (it_x != it_y->second.end()) {
                    float rate = it_x->second;
                    if (rate > 0.8) std::cout << "*";
                    else if (rate > 0.6) std::cout << "+";
                    else if (rate > 0.4) std::cout << ".";
                    else std::cout << " ";
                } else {
                    std::cout << " ";
                }
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

void ActivityVisualizer::renderFiringRateHeatmap(int width, int height, const std::vector<RegionId>& regions) {
    std::cout << "=== Firing Rate Heatmap (Configuration) ===" << std::endl;
    std::cout << "Width: " << width << ", Height: " << height << std::endl;
    std::cout << "Regions: " << regions.size() << std::endl;
    // This would typically render a configurable heatmap
}

void ActivityVisualizer::renderActivityPattern(const BrainActivityPattern& pattern) {
    std::cout << "=== Brain Activity Pattern ===" << std::endl;
    std::cout << "Timestamp: " << pattern.timestamp << std::endl;
    std::cout << "Global Firing Rate: " << pattern.globalFiringRate << std::endl;
    std::cout << "Coherence: " << pattern.coherence << std::endl;
    std::cout << "Complexity: " << pattern.complexity << std::endl;
    std::cout << "Sparsity: " << pattern.sparsity << std::endl;
    std::cout << "Regions: " << pattern.regionFiringRates.size() << std::endl;
    
    for (const auto& [regionId, rate] : pattern.regionFiringRates) {
        std::cout << "  Region " << regionId << ": Rate=" << rate << ", "
                  << "Coherence=" << pattern.regionCoherence[regionId] << ", "
                  << "Complexity=" << pattern.regionComplexity[regionId] << std::endl;
    }
}

void ActivityVisualizer::setTimeWindow(double seconds) {
    pImpl->timeWindow = seconds;
}

void ActivityVisualizer::setMaxSpikesPerNeuron(size_t maxSpikes) {
    pImpl->maxSpikesPerNeuron = maxSpikes;
}

void ActivityVisualizer::setTraceHistorySize(size_t size) {
    pImpl->traceHistorySize = size;
    for (auto& [neuronId, traces] : pImpl->neuronTraces) {
        if (traces.size() > size) {
            auto excess = traces.size() - size;
            traces.erase(traces.begin(), traces.begin() + excess);
        }
    }
}

void ActivityVisualizer::setHeatmapResolution(int width, int height) {
    pImpl->heatmapWidth = width;
    pImpl->heatmapHeight = height;
}

void ActivityVisualizer::setAutoUpdate(bool autoUpdate) {
    pImpl->autoUpdate = autoUpdate;
}

size_t ActivityVisualizer::getTotalSpikeCount() const {
    return pImpl->spikeHistory.size();
}

float ActivityVisualizer::getAverageFiringRate() const {
    if (pImpl->spikeHistory.empty()) return 0.0f;
    
    float totalRate = 0.0f;
    for (const auto& spike : pImpl->spikeHistory) {
        totalRate += spike.burstFrequency;
    }
    
    return totalRate / pImpl->spikeHistory.size();
}

float ActivityVisualizer::getPeakFiringRate() const {
    if (pImpl->spikeHistory.empty()) return 0.0f;
    
    float maxRate = 0.0f;
    for (const auto& spike : pImpl->spikeHistory) {
        maxRate = std::max(maxRate, spike.burstFrequency);
    }
    
    return maxRate;
}

size_t ActivityVisualizer::getActiveNeuronCount() const {
    std::unordered_set<NeuronId> activeNeurons;
    for (const auto& spike : pImpl->spikeHistory) {
        activeNeurons.insert(spike.neuronId);
    }
    return activeNeurons.size();
}

float ActivityVisualizer::getCoherence() const {
    return pImpl->currentPattern.coherence;
}

bool ActivityVisualizer::exportSpikeData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "SpikeEventData:Timestamp,NeuronId,NeuronType,RegionId,RegionName,MembranePotential,IsBurst,BurstFrequency\n";
    for (const auto& spike : pImpl->spikeHistory) {
        file << spike.timestamp << "," << spike.neuronId << "," 
             << static_cast<int>(spike.neuronType) << "," << spike.regionId << "," 
             << spike.regionName << "," << spike.membranePotential << "," 
             << spike.isBurst << "," << spike.burstFrequency << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Spike data exported to: " + filename);
    return true;
}

bool ActivityVisualizer::exportTimeSeriesData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "TimeSeriesData:NeuronId,Timestamp,Potential,Derivative,ThresholdCrossings\n";
    for (const auto& trace : pImpl->traces) {
        file << trace.neuronId << "," << trace.timestamp << "," 
             << trace.potential << "," << trace.derivative << "," 
             << trace.thresholdCrossings << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Time series data exported to: " + filename);
    return true;
}

bool ActivityVisualizer::exportActivityPattern(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "ActivityPatternData:Timestamp,GlobalFiringRate,Coherence,Complexity,Sparsity\n";
    file << pImpl->currentPattern.timestamp << "," << pImpl->currentPattern.globalFiringRate 
         << "," << pImpl->currentPattern.coherence << "," << pImpl->currentPattern.complexity 
         << "," << pImpl->currentPattern.sparsity << "\n";
    
    file << "\nRegionData:RegionId,FiringRate,Coherence,Complexity\n";
    for (const auto& [regionId, rate] : pImpl->currentPattern.regionFiringRates) {
        file << regionId << "," << rate << "," 
             << pImpl->currentPattern.regionCoherence[regionId] << "," 
             << pImpl->currentPattern.regionComplexity[regionId] << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Activity pattern exported to: " + filename);
    return true;
}

} // namespace nlm