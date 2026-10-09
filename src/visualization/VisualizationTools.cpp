// ============================================================================
// Visualization Tools Implementation
// ============================================================================

#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <filesystem>

namespace nlm {

VisualizationTools::VisualizationTools() {
    outputDirectory_ = ".";
}

VisualizationTools::~VisualizationTools() = default;

bool VisualizationTools::generateNeuralActivityVisualization(
    const Brain& brain,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        // Prepare visualization data
        std::vector<std::vector<float>> data;
        
        // Get brain statistics
        size_t totalNeurons = brain.getTotalNeuronCount();
        size_t activeNeurons = brain.getActiveNeuronCount();
        
        // Generate time series data
        for (size_t i = 0; i < config.maxDataPoints && i < totalNeurons; ++i) {
            std::vector<float> timeSeries;
            for (size_t j = 0; j < 100 && j < totalNeurons; ++j) {
                // Simulate activity data
                float activity = (i + j) % 10 / 10.0f;
                timeSeries.push_back(activity);
            }
            data.push_back(timeSeries);
        }
        
        return generateTimeSeriesVisualization(
            std::vector<Timestamp>(), data,
            std::vector<std::string>{"Neuron Activity"}, outputFile, config);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::generateMemoryUsageVisualization(
    const UnifiedMemorySystem& memorySystem,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        // Get memory system metrics
        std::vector<std::vector<float>> data;
        
        // Collect memory statistics
        size_t workingMemorySize = memorySystem.getWorkingMemory() ? 
                                  memorySystem.getWorkingMemory()->getActiveTraceCount() : 0;
        size_t episodicMemorySize = memorySystem.getEpisodicMemory() ? 
                                    memorySystem.getEpisodicMemory()->getEpisodeCount() : 0;
        size_t associativeMemorySize = memorySystem.getAssociativeMemory() ? 
                                       memorySystem.getAssociativeMemory()->getAssociationCount() : 0;
        
        // Create time series data
        for (size_t i = 0; i < config.maxDataPoints; ++i) {
            std::vector<float> memoryData;
            memoryData.push_back(static_cast<float>(workingMemorySize));
            memoryData.push_back(static_cast<float>(episodicMemorySize));
            memoryData.push_back(static_cast<float>(associativeMemorySize));
            data.push_back(memoryData);
        }
        
        return generateTimeSeriesVisualization(
            std::vector<Timestamp>(), data,
            std::vector<std::string>{"Working Memory", "Episodic Memory", "Associative Memory"},
            outputFile, config);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::generatePerformanceVisualization(
    const PerformanceMonitor& monitor,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        // Get performance metrics
        auto metrics = monitor.getMetrics();
        auto history = monitor.getHistory(config.maxDataPoints);
        
        if (history.empty()) {
            return false;
        }
        
        // Collect performance data
        std::vector<std::vector<float>> data;
        std::vector<std::string> labels;
        
        // CPU usage over time
        std::vector<float> cpuData;
        for (const auto& m : history) {
            cpuData.push_back(m.cpuUsage);
        }
        if (!cpuData.empty()) {
            data.push_back(cpuData);
            labels.push_back("CPU Usage");
        }
        
        // Memory usage over time
        std::vector<float> memoryData;
        for (const auto& m : history) {
            memoryData.push_back(static_cast<float>(m.memoryUsage));
        }
        if (!memoryData.empty()) {
            data.push_back(memoryData);
            labels.push_back("Memory Usage");
        }
        
        // System load over time
        std::vector<float> loadData;
        for (const auto& m : history) {
            loadData.push_back(m.systemLoad);
        }
        if (!loadData.empty()) {
            data.push_back(loadData);
            labels.push_back("System Load");
        }
        
        return generateTimeSeriesVisualization(
            std::vector<Timestamp>(), data, labels, outputFile, config);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::generate3DNeuralVisualization(
    const Brain& brain,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        // For now, just generate a 2D visualization
        // 3D visualization would require additional libraries
        return generateNeuralActivityVisualization(brain, outputFile, config);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::generateHeatMapVisualization(
    const std::vector<std::vector<float>>& data,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            return false;
        }
        
        // Generate simple heat map data
        file << "Heat Map Data" << std::endl;
        file << "=============" << std::endl;
        file << "Data points: " << data.size() << std::endl;
        
        for (size_t i = 0; i < data.size() && i < 100; ++i) {
            file << "Row " << i << ": ";
            for (size_t j = 0; j < data[i].size() && j < 100; ++j) {
                file << data[i][j];
                if (j < data[i].size() - 1) {
                    file << ", ";
                }
            }
            file << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::generateTimeSeriesVisualization(
    const std::vector<Timestamp>& timestamps,
    const std::vector<std::vector<float>>& data,
    const std::vector<std::string>& labels,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            return false;
        }
        
        // Determine output format based on file extension
        std::string extension = getFileExtension(outputFile);
        
        if (extension == "csv") {
            saveToCSV(outputFile, data);
        } else if (extension == "json") {
            saveToJSON(outputFile, data);
        } else if (extension == "bin") {
            saveToBinary(outputFile, data);
        } else {
            // Default to CSV
            saveToCSV(outputFile, data);
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::generateConnectivityMatrixVisualization(
    const Brain& brain,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        std::vector<std::vector<float>> connectivityData;
        
        // Get region connectivity data
        size_t regionCount = brain.getRegionCount();
        for (size_t i = 0; i < regionCount && i < 100; ++i) {
            std::vector<float> row;
            for (size_t j = 0; j < regionCount && j < 100; ++j) {
                // Simulate connectivity
                float connectivity = (i == j) ? 1.0f : (rand() % 100) / 100.0f * 0.5f;
                row.push_back(connectivity);
            }
            connectivityData.push_back(row);
        }
        
        return generateHeatMapVisualization(connectivityData, outputFile, config);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::generateSpikeTrainVisualization(
    const Brain& brain,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        std::vector<std::vector<float>> spikeData;
        
        // Get spike train data
        size_t totalSpikes = brain.getTotalSpikeCount();
        size_t maxTime = 1000; // Simulate 1000 time steps
        
        // Create spike train data
        for (size_t t = 0; t < maxTime; ++t) {
            std::vector<float> timeStepData;
            // Simulate spike probability
            float spikeProb = static_cast<float>(rand()) / RAND_MAX * 0.1f;
            timeStepData.push_back(spikeProb);
            spikeData.push_back(timeStepData);
        }
        
        return generateTimeSeriesVisualization(
            std::vector<Timestamp>(), spikeData, std::vector<std::string>{"Spike Probability"},
            outputFile, config);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::generateSynapticWeightVisualization(
    const Brain& brain,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        std::vector<std::vector<float>> weightData;
        
        // Get synaptic weight data
        size_t totalSynapses = brain.getTotalSynapseCount();
        
        // Create weight data
        for (size_t i = 0; i < 100; ++i) {
            std::vector<float> row;
            for (size_t j = 0; j < 100; ++j) {
                // Simulate synaptic weight
                float weight = (rand() % 100) / 100.0f * 2.0f - 1.0f; // -1.0 to 1.0
                row.push_back(weight);
            }
            weightData.push_back(row);
        }
        
        return generateHeatMapVisualization(weightData, outputFile, config);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::generatePopulationActivityVisualization(
    const Brain& brain,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        // Get population activity data
        std::vector<std::vector<float>> activityData;
        
        // Get activity from different regions
        for (size_t i = 0; i < brain.getRegionCount() && i < 10; ++i) {
            auto region = brain.getRegion(static_cast<RegionId>(i + 1));
            if (region) {
                std::vector<float> regionData;
                regionData.push_back(region->getAverageFiringRate());
                regionData.push_back(static_cast<float>(region->getActiveNeuronCount()));
                activityData.push_back(regionData);
            }
        }
        
        return generateTimeSeriesVisualization(
            std::vector<Timestamp>(), activityData,
            std::vector<std::string>{"Firing Rate", "Active Neurons"},
            outputFile, config);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::generatePlasticityVisualization(
    const Brain& brain,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        // Get plasticity data
        std::vector<std::vector<float>> plasticityData;
        
        // Get STDP data
        if (brain.getSTDP()) {
            std::vector<float> stdpData;
            stdpData.push_back(brain.getSTDP()->getLearningRate());
            stdpData.push_back(brain.getSTDP()->getMaxWeightChange());
            plasticityData.push_back(stddpData);
        }
        
        // Get Hebbian data
        if (brain.getHebbian()) {
            std::vector<float> hebbianData;
            hebbianData.push_back(brain.getHebbian()->getLearningRate());
            plasticityData.push_back(hebbianData);
        }
        
        return generateTimeSeriesVisualization(
            std::vector<Timestamp>(), plasticityData,
            std::vector<std::string>{"STDP Rate", "Hebbian Rate"},
            outputFile, config);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::generateNeuromodulationVisualization(
    const Brain& brain,
    const std::string& outputFile,
    const VisualizationConfig& config) {
    
    try {
        // Get neuromodulation data
        std::vector<std::vector<float>> neuromodulationData;
        
        if (brain.getDopamine()) {
            std::vector<float> dopamineData;
            dopamineData.push_back(brain.getDopamine()->getLevel());
            dopamineData.push_back(brain.getDopamine()->getPlasticityFactor());
            neuromodulationData.push_back(dopamineData);
        }
        
        if (brain.getCuriosity()) {
            std::vector<float> curiosityData;
            curiosityData.push_back(brain.getCuriosity()->getLevel());
            neuromodulationData.push_back(curiosityData);
        }
        
        if (brain.getNovelty()) {
            std::vector<float> noveltyData;
            noveltyData.push_back(brain.getNovelty()->getLevel());
            neuromodulationData.push_back(noveltyData);
        }
        
        return generateTimeSeriesVisualization(
            std::vector<Timestamp>(), neuromodulationData,
            std::vector<std::string>{"Dopamine", "Curiosity", "Novelty"},
            outputFile, config);
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool VisualizationTools::ensureOutputDirectory() {
    std::error_code ec;
    if (!std::filesystem::exists(outputDirectory_, ec) && !std::filesystem::create_directories(outputDirectory_, ec)) {
        return false;
    }
    return true;
}

std::vector<std::string> VisualizationTools::getSupportedFormats() const {
    return std::vector<std::string>{"CSV", "JSON", "BIN", "PNG", "SVG", "PDF"};
}

bool VisualizationTools::isFormatSupported(const std::string& format) const {
    std::string upperFormat = format;
    std::transform(upperFormat.begin(), upperFormat.end(), upperFormat.begin(), ::toupper);
    
    return std::find(getSupportedFormats().begin(), getSupportedFormats().end(), upperFormat) != getSupportedFormats().end();
}

std::string VisualizationTools::getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of(".");
    if (pos != std::string::npos && pos < filename.length() - 1) {
        return filename.substr(pos + 1);
    }
    return "";
}

void VisualizationTools::saveToCSV(const std::string& filepath, const std::vector<std::vector<float>>& data) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    // Write data to CSV
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << data[i][j];
            if (j < data[i].size() - 1) {
                file << ",";
            }
        }
        if (i < data.size() - 1) {
            file << std::endl;
        }
    }
    
    file.close();
}

void VisualizationTools::saveToJSON(const std::string& filepath, const std::vector<std::vector<float>>& data) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\n";
    file << "  \"data\": [\n";
    
    for (size_t i = 0; i < data.size(); ++i) {
        file << "    [";
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << data[i][j];
            if (j < data[i].size() - 1) {
                file << ", ";
            }
        }
        file << "]";
        if (i < data.size() - 1) {
            file << ",\n";
        }
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
}

void VisualizationTools::saveToBinary(const std::string& filepath, const std::vector<std::vector<float>>& data) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return;
    }
    
    // Write data in binary format
    for (const auto& row : data) {
        for (const auto& value : row) {
            file.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }
    }
    
    file.close();
}

} // namespace nlm
