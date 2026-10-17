#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Types/Types.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>
#include <set>
#include <chrono>
#include <ctime>

namespace nlm {

// ==================== UTILITY FUNCTIONS ====================

// Color codes for terminal output
namespace Colors {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string GRAY = "\033[90m";
    const std::string BG_RED = "\033[41m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_YELLOW = "\033[43m";
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_MAGENTA = "\033[45m";
    const std::string BG_CYAN = "\033[46m";
}

std::string colored(const std::string& text, const std::string& color) {
    return color + text + Colors::RESET;
}

// ==================== NETWORK GRAPH CLASS ====================

class NetworkGraph {
public:
    struct Node {
        NeuronId id;
        std::string label;
        NeuronType type;
        int regionId;
        float firingRate;
        float membranePotential;
        
        Node(NeuronId nid, const std::string& lbl, NeuronType t, int rid = 0)
            : id(nid), label(lbl), type(t), regionId(rid), firingRate(0.0f), membranePotential(0.0f) {}
    };
    
    struct Connection {
        NeuronId source;
        NeuronId target;
        SynapticWeight weight;
        SynapseType type;
        float delay;
        
        Connection(NeuronId src, NeuronId tgt, SynapticWeight w, SynapseType t, float d = 0.0f)
            : source(src), target(tgt), weight(w), type(t), delay(d) {}
    };
    
private:
    std::vector<Node> nodes_;
    std::vector<Connection> connections_;
    std::unordered_map<int, std::string> regionNames_;
    std::string layout_; // "circular", "radial", "hierarchical"
    float scale_;
    
public:
    NetworkGraph() : layout_("circular"), scale_(1.0f) {}
    
    void clear() {
        nodes_.clear();
        connections_.clear();
        regionNames_.clear();
    }
    
    void addNode(const Node& node) {
        nodes_.push_back(node);
    }
    
    void addConnection(const Connection& conn) {
        connections_.push_back(conn);
    }
    
    void addRegionName(int regionId, const std::string& name) {
        regionNames_[regionId] = name;
    }
    
    void setLayout(const std::string& layout) { layout_ = layout; }
    void setScale(float scale) { scale_ = scale; }
    
    // Render as ASCII/Unicode text graph
    void render(std::ostream& output) const {
        // Create node positions using layout algorithm
        std::vector<std::pair<float, float>> positions;
        if (layout_ == "circular") {
            positions = layoutCircular();
        } else if (layout_ == "radial") {
            positions = layoutRadial();
        } else {
            positions = layoutHierarchical();
        }
        
        output << "\n" << Colors::CYAN << "=== NEURAL NETWORK VISUALIZATION ===" << Colors::RESET << "\n\n";
        
        // Render connections (bottom-up)
        for (const auto& conn : connections_) {
            renderConnection(output, conn, positions);
        }
        
        // Render nodes (top)
        for (size_t i = 0; i < nodes_.size(); ++i) {
            renderNode(output, nodes_[i], positions[i]);
        }
        
        // Render legend
        renderLegend(output);
    }
    
    // Render as Graphviz DOT format
    void renderDOT(std::ostream& output) const {
        output << "digraph NeuralNetwork {\n";
        output << "  rankdir=LR;\n";
        output << "  node [shape=circle, style=filled];\n";
        output << "  edge [color=gray];\n\n";
        
        // Nodes
        for (const auto& node : nodes_) {
            std::string color;
            switch (node.type) {
                case NeuronType::Excitatory: color = "lightgreen"; break;
                case NeuronType::Inhibitory: color = "lightcoral"; break;
                case NeuronType::Sensory: color = "lightblue"; break;
                case NeuronType::Motor: color = "lightyellow"; break;
                default: color = "lightgray"; break;
            }
            
            output << "  \"" << node.label << "\" [fillcolor=" << color << "]\n";
        }
        
        output << "\n";
        
        // Connections
        for (const auto& conn : connections_) {
            std::string edgeColor;
            switch (conn.type) {
                case SynapseType::Excitatory: edgeColor = "green"; break;
                case SynapseType::Inhibitory: edgeColor = "red"; break;
                default: edgeColor = "gray"; break;
            }
            
            output << "  \"" << conn.source << "\" -> \"" << conn.target << "\" [color=" << edgeColor << ", label=\"" 
                   << std::fixed << std::setprecision(2) << conn.weight << "\"]\n";
        }
        
        output << "}\n";
    }
    
private:
    std::vector<std::pair<float, float>> layoutCircular() const {
        std::vector<std::pair<float, float>> positions;
        float angle = 2.0f * 3.14159f / std::max(1.0f, static_cast<float>(nodes_.size()));
        
        for (size_t i = 0; i < nodes_.size(); ++i) {
            float x = scale_ * std::cos(i * angle);
            float y = scale_ * std::sin(i * angle);
            positions.emplace_back(x, y);
        }
        
        return positions;
    }
    
    std::vector<std::pair<float, float>> layoutRadial() const {
        std::vector<std::pair<float, float>> positions;
        size_t regionCount = regionNames_.size();
        float angleStep = 2.0f * 3.14159f / std::max(1.0f, static_cast<float>(regionCount));
        
        for (size_t i = 0; i < nodes_.size(); ++i) {
            int regionId = nodes_[i].regionId;
            float radius = 50.0f + (regionId % 5) * 30.0f;
            float angle = angleStep * i + regionId * 0.5f;
            float x = radius * std::cos(angle);
            float y = radius * std::sin(angle);
            positions.emplace_back(x, y);
        }
        
        return positions;
    }
    
    std::vector<std::pair<float, float>> layoutHierarchical() const {
        std::vector<std::pair<float, float>> positions;
        
        // Group by region
        std::unordered_map<int, std::vector<size_t>> regionNodes;
        for (size_t i = 0; i < nodes_.size(); ++i) {
            regionNodes[nodes_[i].regionId].push_back(i);
        }
        
        float yStep = 60.0f;
        float xStep = 80.0f;
        int regionIndex = 0;
        
        for (const auto& regionEntry : regionNodes) {
            int regionId = regionEntry.first;
            float y = yStep * regionIndex;
            
            for (size_t nodeIdx : regionEntry.second) {
                float x = xStep * (nodeIdx % 3) - xStep;
                positions.emplace_back(x, y);
            }
        }
        
        return positions;
    }
    
    void renderConnection(std::ostream& output, const Connection& conn, 
                          const std::vector<std::pair<float, float>>& positions) const {
        // Find node indices
        auto findNodeIdx = [&](NeuronId id) {
            for (size_t i = 0; i < nodes_.size(); ++i) {
                if (nodes_[i].id == id) return i;
            }
            return size_t(-1);
        };
        
        size_t srcIdx = findNodeIdx(conn.source);
        size_t tgtIdx = findNodeIdx(conn.target);
        
        if (srcIdx == size_t(-1) || tgtIdx == size_t(-1)) return;
        
        float srcX = positions[srcIdx].first;
        float srcY = positions[srcIdx].second;
        float tgtX = positions[tgtIdx].first;
        float tgtY = positions[tgtIdx].second;
        
        // Draw line
        float weight = std::max(0.0f, std::min(1.0f, conn.weight / 10.0f));
        char lineChar = weight > 0.7f ? '*' : weight > 0.3f ? '-' : '.';
        std::string color;
        
        switch (conn.type) {
            case SynapseType::Excitatory: color = Colors::GREEN; break;
            case SynapseType::Inhibitory: color = Colors::RED; break;
            default: color = Colors::GRAY; break;
        }
        
        output << color << lineChar << Colors::RESET;
    }
    
    void renderNode(std::ostream& output, const Node& node, const std::pair<float, float>& pos) const {
        // Create node representation
        std::string statusChar;
        std::string statusColor;
        
        if (node.firingRate > 20.0f) {
            statusChar = "🔥";
            statusColor = Colors::BG_RED;
        } else if (node.firingRate > 10.0f) {
            statusChar = "⚡";
            statusColor = Colors::BG_YELLOW;
        } else if (node.firingRate > 5.0f) {
            statusChar = "✦";
            statusColor = Colors::BG_BLUE;
        } else {
            statusChar = "○";
            statusColor = Colors::BG_GREEN;
        }
        
        std::string typeSymbol;
        switch (node.type) {
            case NeuronType::Excitatory: typeSymbol = "E"; break;
            case NeuronType::Inhibitory: typeSymbol = "I"; break;
            case NeuronType::Sensory: typeSymbol = "S"; break;
            case NeuronType::Motor: typeSymbol = "M"; break;
            default: typeSymbol = "?"; break;
        }
        
        output << statusColor << " " << statusChar << " " << Colors::RESET;
        output << Colors::BOLD << typeSymbol << Colors::RESET;
        output << " " << node.label << " ";
        
        if (node.firingRate > 0) {
            output << " [" << std::fixed << std::setprecision(1) << node.firingRate << " Hz]";
        }
        
        // Add region info
        auto it = regionNames_.find(node.regionId);
        if (it != regionNames_.end()) {
            output << " " << Colors::GRAY << "(" << it->second << ")" << Colors::RESET;
        }
        
        output << "\n";
    }
    
    void renderLegend(std::ostream& output) const {
        output << "\n" << Colors::CYAN << "=== LEGEND ===" << Colors::RESET << "\n";
        output << "  " << Colors::BG_RED << " " << Colors::RESET << " High firing rate (>20 Hz)\n";
        output << "  " << Colors::BG_YELLOW << " " << Colors::RESET << " Medium firing rate (10-20 Hz)\n";
        output << "  " << Colors::BG_BLUE << " " << Colors::RESET << " Low firing rate (5-10 Hz)\n";
        output << "  " << Colors::BG_GREEN << " " << Colors::RESET << " Inactive (<5 Hz)\n";
        output << "  E = Excitatory, I = Inhibitory, S = Sensory, M = Motor\n";
        output << Colors::CYAN << "================" << Colors::RESET << "\n\n";
    }
};

// ==================== ACTIVITY PLOTTER CLASS ====================

class ActivityPlotter {
private:
    std::vector<std::vector<float>> spikeHistory_;
    std::vector<float> membranePotentials_;
    std::vector<float> firingRates_;
    size_t historySize_;
    float timeStep_;
    
public:
    ActivityPlotter(size_t historySize = 100) 
        : historySize_(historySize), timeStep_(0.01f) {
        spikeHistory_.resize(historySize, std::vector<float>(32, 0.0f)); // 32 neurons max
        membranePotentials_.resize(historySize, 0.0f);
        firingRates_.resize(historySize, 0.0f);
    }
    
    void addSpikeEvent(NeuronId neuron, float timestamp, size_t neuronIndex = 0) {
        if (neuronIndex < spikeHistory_[0].size()) {
            spikeHistory_[0][neuronIndex] = timestamp;
            // Shift history
            for (size_t i = spikeHistory_.size() - 1; i > 0; --i) {
                spikeHistory_[i] = spikeHistory_[i-1];
            }
            spikeHistory_[0][neuronIndex] = timestamp;
        }
    }
    
    void addMembranePotential(float potential, float timestamp) {
        membranePotentials_[0] = potential;
        for (size_t i = membranePotentials_.size() - 1; i > 0; --i) {
            membranePotentials_[i] = membranePotentials_[i-1];
        }
        membranePotentials_[0] = potential;
    }
    
    void addFiringRate(float rate, float timestamp) {
        firingRates_[0] = rate;
        for (size_t i = firingRates_.size() - 1; i > 0; --i) {
            firingRates_[i] = firingRates_[i-1];
        }
        firingRates_[0] = rate;
    }
    
    // Render spike raster plot
    void renderSpikeRaster(std::ostream& output, size_t maxNeurons = 32) const {
        output << "\n" << Colors::CYAN << "=== SPIKE RASTER PLOT ===" << Colors::RESET << "\n\n";
        
        // Collect all spikes
        std::vector<std::pair<float, size_t>> allSpikes;
        for (size_t t = 0; t < spikeHistory_.size(); ++t) {
            for (size_t n = 0; n < maxNeurons && n < spikeHistory_[t].size(); ++n) {
                if (spikeHistory_[t][n] > 0) {
                    allSpikes.emplace_back(spikeHistory_[t][n], n);
                }
            }
        }
        
        if (allSpikes.empty()) {
            output << "  No spikes recorded.\n";
            return;
        }
        
        // Sort by time
        std::sort(allSpikes.begin(), allSpikes.end());
        
        // Render axes
        output << "  Time (ms) →\n";
        output << "  ";
        for (size_t i = 0; i < 60; ++i) output << "-";
        output << "\n";
        
        // Render spikes
        for (const auto& spike : allSpikes) {
            float timeMs = spike.first * 1000.0f;
            size_t neuronIdx = spike.second;
            
            int column = static_cast<int>(timeMs / 10.0f); // 10ms per column
            if (column >= 0 && column < 60) {
                // Move cursor (simplified: we'll render in rows)
                output << "  ";
            }
        }
        
        // Alternative text-based raster
        output << "\n  Neuron | Time Stamps (ms)\n";
        output << "  -------|-------------------\n";
        
        for (size_t neuron = 0; neuron < maxNeurons && neuron < spikeHistory_[0].size(); ++neuron) {
            output << "  " << std::setw(3) << neuron << " | ";
            bool first = true;
            for (size_t t = 0; t < spikeHistory_.size(); ++t) {
                if (spikeHistory_[t][neuron] > 0) {
                    float timeMs = spikeHistory_[t][neuron] * 1000.0f;
                    if (!first) output << ", ";
                    output << std::fixed << std::setprecision(1) << timeMs;
                    first = false;
                }
            }
            output << "\n";
        }
        
        output << Colors::CYAN << "=====================" << Colors::RESET << "\n\n";
    }
    
    // Render membrane potential trace
    void renderMembranePotential(std::ostream& output) const {
        output << "\n" << Colors::CYAN << "=== MEMBRANE POTENTIAL TRACES ===" << Colors::RESET << "\n\n";
        
        // Find max value for scaling
        float maxPot = 0.0f;
        for (float pot : membranePotentials_) {
            if (pot > maxPot) maxPot = pot;
        }
        maxPot = std::max(maxPot, 1.0f);
        
        // Render trace
        output << "  Potential (0-" << maxPot << ")\n";
        output << "  ";
        for (size_t i = 0; i < 60; ++i) output << "-";
        output << "\n";
        
        for (size_t i = 0; i < membranePotentials_.size(); ++i) {
            float pot = membranePotentials_[i];
            int column = static_cast<int>((pot / maxPot) * 59.0f);
            column = std::max(0, std::min(59, static_cast<int>(column)));
            
            output << "  ";
            for (size_t j = 0; j < 60; ++j) {
                if (j == column) output << "*";
                else output << " ";
            }
            output << "  // Time\n";
        }
        
        output << Colors::CYAN << "==========================" << Colors::RESET << "\n\n";
    }
    
    // Render firing rate histogram
    void renderFiringRateHistogram(std::ostream& output) const {
        output << "\n" << Colors::CYAN << "=== FIRING RATE HISTOGRAM ===" << Colors::RESET << "\n\n";
        
        // Calculate rate histogram
        std::vector<size_t> rateBuckets(11, 0); // 0-10, 10-20, ..., 100+
        for (float rate : firingRates_) {
            if (rate < 10.0f) rateBuckets[0]++;
            else if (rate < 20.0f) rateBuckets[1]++;
            else if (rate < 30.0f) rateBuckets[2]++;
            else if (rate < 40.0f) rateBuckets[3]++;
            else if (rate < 50.0f) rateBuckets[4]++;
            else if (rate < 60.0f) rateBuckets[5]++;
            else if (rate < 70.0f) rateBuckets[6]++;
            else if (rate < 80.0f) rateBuckets[7]++;
            else if (rate < 90.0f) rateBuckets[8]++;
            else if (rate < 100.0f) rateBuckets[9]++;
            else rateBuckets[10]++;
        }
        
        // Find max for scaling
        size_t maxCount = *std::max_element(rateBuckets.begin(), rateBuckets.end());
        if (maxCount == 0) maxCount = 1;
        
        output << "  Rate (Hz) | Count\n";
        output << "  ----------|------\n";
        
        const std::vector<std::string> labels = {"0-10", "10-20", "20-30", "30-40", "40-50", 
                                                "50-60", "60-70", "70-80", "80-90", "90-100", ">100"};
        
        for (size_t i = 0; i < rateBuckets.size(); ++i) {
            size_t count = rateBuckets[i];
            size_t barLength = (count * 40) / maxCount;
            
            std::string bar(barLength, '█');
            output << "  " << std::setw(6) << labels[i] << " | " << bar << " " << count << "\n";
        }
        
        output << Colors::CYAN << "=======================" << Colors::RESET << "\n\n";
    }
};

// ==================== HEATMAP RENDERER CLASS ====================

class HeatmapRenderer {
private:
    std::vector<std::vector<float>> heatmapData_;
    size_t width_;
    size_t height_;
    std::string title_;
    
public:
    HeatmapRenderer(size_t width = 40, size_t height = 20) 
        : width_(width), height_(height) {}
    
    void setTitle(const std::string& title) { title_ = title; }
    
    void setData(const std::vector<std::vector<float>>& data) {
        heatmapData_ = data;
        if (heatmapData_.empty()) {
            heatmapData_ = std::vector<std::vector<float>>(height_, std::vector<float>(width_, 0.0f));
        }
    }
    
    // Render text-based heatmap
    void render(std::ostream& output) const {
        output << "\n" << Colors::CYAN << "=== " << title_ << " ===" << Colors::RESET << "\n\n";
        
        if (heatmapData_.empty() || heatmapData_[0].empty()) {
            output << "  No data available.\n";
            return;
        }
        
        // Normalize data for display
        float maxVal = 0.0f;
        for (const auto& row : heatmapData_) {
            for (float val : row) {
                if (val > maxVal) maxVal = val;
            }
        }
        maxVal = std::max(maxVal, 1.0f);
        
        // Render heatmap
        for (size_t row = 0; row < heatmapData_.size() && row < height_; ++row) {
            for (size_t col = 0; col < heatmapData_[row].size() && col < width_; ++col) {
                float val = heatmapData_[row][col];
                size_t normalizedVal = static_cast<size_t>((val / maxVal) * 10.0f);
                normalizedVal = std::min(normalizedVal, static_cast<size_t>(10));
                
                // Choose color based on value
                std::string color;
                if (normalizedVal >= 8) color = Colors::BG_RED;
                else if (normalizedVal >= 6) color = Colors::BG_YELLOW;
                else if (normalizedVal >= 4) color = Colors::BG_GREEN;
                else if (normalizedVal >= 2) color = Colors::BG_BLUE;
                else color = Colors::BG_MAGENTA;
                
                output << color << " " << normalizedVal << Colors::RESET << " ";
            }
            output << "\n";
        }
        
        output << Colors::CYAN << "======================" << Colors::RESET << "\n\n";
    }
    
    // Render heatmap with ASCII characters
    void renderTextHeatmap(std::ostream& output) const {
        output << "\n" << Colors::CYAN << "=== " << title_ << " (Text) ===" << Colors::RESET << "\n\n";
        
        if (heatmapData_.empty() || heatmapData_[0].empty()) {
            output << "  No data available.\n";
            return;
        }
        
        float maxVal = 0.0f;
        for (const auto& row : heatmapData_) {
            for (float val : row) {
                if (val > maxVal) maxVal = val;
            }
        }
        maxVal = std::max(maxVal, 1.0f);
        
        // Character-based heatmap
        const std::string chars = ".:-=+*#%@";
        
        for (size_t row = 0; row < heatmapData_.size() && row < height_; ++row) {
            for (size_t col = 0; col < heatmapData_[row].size() && col < width_; ++col) {
                float val = heatmapData_[row][col];
                size_t charIndex = static_cast<size_t>((val / maxVal) * (chars.size() - 1));
                charIndex = std::min(charIndex, static_cast<size_t>(chars.size() - 1));
                
                output << chars[charIndex];
            }
            output << "\n";
        }
        
        output << Colors::CYAN << "======================" << Colors::RESET << "\n\n";
    }
};

// ==================== DATA EXPORTER CLASS ====================

class DataExporter {
private:
    std::string outputDir_;
    std::string prefix_;
    bool overwrite_;
    
public:
    DataExporter(const std::string& outputDir = ".", const std::string& prefix = "nlm")
        : outputDir_(outputDir), prefix_(prefix), overwrite_(false) {}
    
    void setOutputDir(const std::string& dir) { outputDir_ = dir; }
    void setPrefix(const std::string& prefix) { prefix_ = prefix; }
    void setOverwrite(bool overwrite) { overwrite_ = overwrite; }
    
    // Export to CSV format
    bool exportToCSV(const std::string& filename, const std::vector<std::vector<float>>& data,
                    const std::vector<std::string>& headers) const {
        std::string fullPath = outputDir_ + "/" + prefix_ + "_" + filename + ".csv";
        
        std::ofstream file(fullPath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for CSV export: " + fullPath);
            return false;
        }
        
        // Write headers
        for (size_t i = 0; i < headers.size(); ++i) {
            file << headers[i];
            if (i < headers.size() - 1) file << ",";
        }
        file << "\n";
        
        // Write data
        for (size_t row = 0; row < data.size(); ++row) {
            for (size_t col = 0; col < data[row].size(); ++col) {
                file << std::fixed << std::setprecision(6) << data[row][col];
                if (col < data[row].size() - 1) file << ",";
            }
            if (row < data.size() - 1) file << "\n";
        }
        
        NLM_LOG_INFO("Data exported to CSV: " + fullPath);
        return true;
    }
    
    // Export to JSON format
    bool exportToJSON(const std::string& filename, const std::string& data) const {
        std::string fullPath = outputDir_ + "/" + prefix_ + "_" + filename + ".json";
        
        std::ofstream file(fullPath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for JSON export: " + fullPath);
            return false;
        }
        
        file << data;
        
        NLM_LOG_INFO("Data exported to JSON: " + fullPath);
        return true;
    }
    
    // Export network to Graphviz DOT format
    bool exportNetworkDOT(const std::string& filename, const NetworkGraph& network) const {
        std::string fullPath = outputDir_ + "/" + prefix_ + "_" + filename + ".dot";
        
        std::ofstream file(fullPath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for DOT export: " + fullPath);
            return false;
        }
        
        network.renderDOT(file);
        
        NLM_LOG_INFO("Network exported to Graphviz DOT: " + fullPath);
        return true;
    }
    
    // Export statistics to text file
    bool exportStatistics(const std::string& filename, const std::string& stats) const {
        std::string fullPath = outputDir_ + "/" + prefix_ + "_" + filename + ".txt";
        
        std::ofstream file(fullPath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for stats export: " + fullPath);
            return false;
        }
        
        file << stats;
        
        NLM_LOG_INFO("Statistics exported to: " + fullPath);
        return true;
    }
};

// ==================== CONSOLE VISUALIZER CLASS ====================

class ConsoleVisualizer {
private:
    bool active_;
    double updateRate_;
    std::chrono::steady_clock::time_point lastUpdate_;
    
    NetworkGraph networkGraph_;
    ActivityPlotter activityPlotter_;
    HeatmapRenderer heatmapRenderer_;
    DataExporter dataExporter_;
    
    std::string outputFile_;
    bool realTime_;
    bool fileOutput_;
    size_t frameCount_;
    
public:
    ConsoleVisualizer() 
        : active_(false), updateRate_(30.0), realTime_(true), fileOutput_(false), frameCount_(0) {
        lastUpdate_ = std::chrono::steady_clock::now();
        activityPlotter_ = ActivityPlotter(100);
        heatmapRenderer_ = HeatmapRenderer(40, 20);
    }
    
    void setOutputFile(const std::string& filename) {
        outputFile_ = filename;
        fileOutput_ = !filename.empty();
    }
    
    void setRealTime(bool realTime) { realTime_ = realTime; }
    void setUpdateRate(double hz) { updateRate_ = hz; }
    
    void initialize() {
        active_ = true;
        NLM_LOG_INFO("Console visualization system initialized");
    }
    
    void update() {
        if (!active_) return;
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate_).count();
        
        if (elapsed >= (1000.0 / updateRate_)) {
            if (realTime_) {
                renderFrame();
            }
            lastUpdate_ = now;
        }
        
        frameCount_++;
    }
    
    void renderFrame() {
        if (fileOutput_) {
            std::ofstream file(outputFile_, std::ios::app);
            if (file.is_open()) {
                renderAll(file);
            }
        } else {
            renderAll(std::cout);
        }
    }
    
    void renderAll(std::ostream& output) {
        output << "\n" << Colors::BOLD << "=== FRAME " << frameCount_ << " ===" << Colors::RESET << "\n";
        
        // Render different visualizations
        networkGraph_.render(output);
        activityPlotter_.renderSpikeRaster(output);
        activityPlotter_.renderMembranePotential(output);
        activityPlotter_.renderFiringRateHistogram(output);
        
        output << Colors::GREEN << "Frame rendered successfully" << Colors::RESET << "\n";
    }
    
    void visualizeNetwork(const Brain& brain) {
        networkGraph_.clear();
        
        // Extract neurons from brain regions
        for (const auto& region : brain.getRegions()) {
            int regionId = region->getId().index();
            std::string regionName = region->getName();
            networkGraph_.addRegionName(regionId, regionName);
            
            for (const auto& population : region->getPopulations()) {
                NeuronType type = population->getNeuronType();
                size_t neuronCount = population->getSize();
                
                for (size_t i = 0; i < neuronCount && i < 100; ++i) { // Limit for display
                    NeuronId neuronId(i + regionId * 1000);
                    std::string label = "N" + std::to_string(neuronId.index());
                    
                    NetworkGraph::Node node(neuronId, label, type, regionId);
                    node.firingRate = static_cast<float>(rand() % 100) / 10.0f; // Simulated
                    node.membranePotential = static_cast<float>(rand() % 100) / 100.0f;
                    
                    networkGraph_.addNode(node);
                }
            }
        }
        
        // Extract connections
        for (const auto& region : brain.getRegions()) {
            for (const auto& synapse : region->getSynapses()) {
                NetworkGraph::Connection conn(
                    synapse->getSourceNeuron(),
                    synapse->getDestinationNeuron(),
                    synapse->getWeight(),
                    synapse->getType()
                );
                networkGraph_.addConnection(conn);
            }
        }
        
        // Render network
        if (fileOutput_) {
            std::ofstream file(outputFile_, std::ios::app);
            if (file.is_open()) {
                networkGraph_.render(file);
            }
        } else {
            networkGraph_.render(std::cout);
        }
    }
    
    void visualizeActivity(const Brain& brain) {
        // Extract activity data from brain
        float avgFiringRate = brain.getAverageFiringRate();
        activityPlotter_.addFiringRate(avgFiringRate, 0.0f);
        
        // Create activity heatmap
        std::vector<std::vector<float>> heatmap(20, std::vector<float>(40, 0.0f));
        
        // Fill with simulated activity data
        for (size_t row = 0; row < heatmap.size(); ++row) {
            for (size_t col = 0; col < heatmap[row].size(); ++col) {
                heatmap[row][col] = (static_cast<float>(rand()) / RAND_MAX) * avgFiringRate;
            }
        }
        
        heatmapRenderer_.setTitle("Neural Activity Heatmap");
        heatmapRenderer_.setData(heatmap);
        
        if (fileOutput_) {
            std::ofstream file(outputFile_, std::ios::app);
            if (file.is_open()) {
                heatmapRenderer_.render(file);
            }
        } else {
            heatmapRenderer_.render(std::cout);
        }
    }
    
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
        for (const auto& spike : spikes) {
            activityPlotter_.addSpikeEvent(spike.source_neuron, spike.timestamp, spike.source_neuron.index());
        }
        
        if (fileOutput_) {
            std::ofstream file(outputFile_, std::ios::app);
            if (file.is_open()) {
                activityPlotter_.renderSpikeRaster(file);
            }
        } else {
            activityPlotter_.renderSpikeRaster(std::cout);
        }
    }
    
    void visualizeWeights(const Brain& brain) {
        // Create weight matrix heatmap
        std::vector<std::vector<float>> weightMatrix(20, std::vector<float>(40, 0.0f));
        
        size_t totalSynapses = brain.getTotalSynapseCount();
        float avgWeight = 0.0f;
        
        if (totalSynapses > 0) {
            // Calculate average weight
            for (const auto& region : brain.getRegions()) {
                for (const auto& synapse : region->getSynapses()) {
                    avgWeight += synapse->getWeight();
                }
            }
            avgWeight /= static_cast<float>(totalSynapses);
        }
        
        // Fill weight matrix
        for (size_t row = 0; row < weightMatrix.size(); ++row) {
            for (size_t col = 0; col < weightMatrix[row].size(); ++col) {
                weightMatrix[row][col] = avgWeight + ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * 0.5f;
            }
        }
        
        heatmapRenderer_.setTitle("Synaptic Weight Matrix");
        heatmapRenderer_.setData(weightMatrix);
        
        if (fileOutput_) {
            std::ofstream file(outputFile_, std::ios::app);
            if (file.is_open()) {
                heatmapRenderer_.renderTextHeatmap(file);
            }
        } else {
            heatmapRenderer_.renderTextHeatmap(std::cout);
        }
    }
    
    void exportData(const std::string& filename, const std::string& data, const std::string& format) {
        if (format == "csv") {
            dataExporter_.exportToCSV(filename, {}, {});
        } else if (format == "json") {
            dataExporter_.exportToJSON(filename, data);
        } else if (format == "dot") {
            // Need network graph for DOT export
            dataExporter_.exportNetworkDOT(filename, networkGraph_);
        }
    }
    
    void close() {
        active_ = false;
        if (fileOutput_ && outputFile_.empty() == false) {
            std::ofstream file(outputFile_, std::ios::app);
            if (file.is_open()) {
                file << "\n" << Colors::YELLOW << "Visualization session ended" << Colors::RESET << "\n";
            }
        }
        NLM_LOG_INFO("Console visualization system closed");
    }
    
    bool isActive() const { return active_; }
};

// ==================== VISUALIZATION INTERFACE IMPLEMENTATION ====================

struct VisualizationInterface::Impl {
    std::unique_ptr<ConsoleVisualizer> visualizer;
    bool active;
    double updateRate;
    
    Impl() : active(false), updateRate(30.0) {
        visualizer = std::make_unique<ConsoleVisualizer>();
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize console-based visualization system
    pImpl->visualizer->initialize();
    pImpl->visualizer->setUpdateRate(pImpl->updateRate);
    pImpl->active = true;
    
    NLM_LOG_INFO("VisualizationInterface: Console-based visualization system initialized");
    return true;
}

void VisualizationInterface::update() {
    if (pImpl->active && pImpl->visualizer) {
        pImpl->visualizer->update();
    }
}

void VisualizationInterface::render() {
    // Rendering is done in update() for console system
}

void VisualizationInterface::close() {
    if (pImpl->visualizer) {
        pImpl->visualizer->close();
    }
    pImpl->active = false;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    if (pImpl->visualizer) {
        pImpl->visualizer->setUpdateRate(hz);
    }
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (pImpl->active && pImpl->visualizer) {
        pImpl->visualizer->visualizeNetwork(brain);
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (pImpl->active && pImpl->visualizer) {
        pImpl->visualizer->visualizeActivity(brain);
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (pImpl->active && pImpl->visualizer) {
        pImpl->visualizer->visualizeSpikes(spikes);
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (pImpl->active && pImpl->visualizer) {
        pImpl->visualizer->visualizeWeights(brain);
    }
}

} // namespace nlm

