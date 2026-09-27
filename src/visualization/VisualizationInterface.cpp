// Visualization Interface Implementation
#include "VisualizationInterface.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace nlm {

// ASCIIArtVisualization implementation
bool ASCIIArtVisualization::initialize(const VisualizationSettings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    settings_ = settings;
    running_ = false;
    overlays_.clear();
    dataHistory_.clear();
    return true;
}

void ASCIIArtVisualization::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    overlays_.clear();
    dataHistory_.clear();
}

void ASCIIArtVisualization::start() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = true;
}

void ASCIIArtVisualization::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
}

void ASCIIArtVisualization::pause() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
}

void ASCIIArtVisualization::resume() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = true;
}

bool ASCIIArtVisualization::isRunning() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return running_;
}

void ASCIIArtVisualization::visualize(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!running_) return;
    
    dataHistory_.push_back(data);
    
    // Generate ASCII representation
    std::string asciiOutput;
    
    // Determine visualization type from data
    if (data.data.find("neuron_activity") != data.data.end()) {
        asciiOutput = generateNeuronActivity(data.data.at("neuron_activity"), 80, 40);
    } else if (data.data.find("synapse_weights") != data.data.end()) {
        asciiOutput = generateSynapseWeights(data.data.at("synapse_weights"), 80, 40);
    } else if (data.data.find("brain_states") != data.data.end()) {
        asciiOutput = generateBrainState(data.data.at("brain_states"));
    } else {
        // Generic visualization
        asciiOutput = generateGenericVisualization(data);
    }
    
    // Add overlays
    for (const auto& overlay : overlays_) {
        if (overlay.visible) {
            asciiOutput = addOverlayToOutput(asciiOutput, overlay);
        }
    }
    
    // Output
    std::cout << asciiOutput << std::endl;
    
    if (settings_.saveToFile) {
        saveToFile(settings_.outputFile, asciiOutput);
    }
}

void ASCIIArtVisualization::addOverlay(const VisualizationOverlay& overlay) {
    std::lock_guard<std::mutex> lock(mutex_);
    overlays_.push_back(overlay);
}

void ASCIIArtVisualization::removeOverlay(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    overlays_.erase(std::remove_if(overlays_.begin(), overlays_.end(),
                                  [&id](const VisualizationOverlay& o) { return o.id == id; }),
                    overlays_.end());
}

void ASCIIArtVisualization::setSettings(const VisualizationSettings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    settings_ = settings;
}

VisualizationSettings ASCIIArtVisualization::getSettings() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return settings_;
}

bool ASCIIArtVisualization::saveVisualization(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Generate current visualization
    std::string output;
    if (!dataHistory_.empty()) {
        output = generateGenericVisualization(dataHistory_.back());
    }
    
    file << output;
    return true;
}

std::string ASCIIArtVisualization::generateOutput() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    
    for (const auto& data : dataHistory_) {
        oss << generateGenericVisualization(data) << std::endl;
    }
    
    return oss.str();
}

bool ASCIIArtVisualization::configure(const std::map<std::string, std::string>& config) {
    // Configure based on settings
    for (const auto& pair : config) {
        if (pair.first == "format") {
            // Parse format string
        } else if (pair.first == "updateInterval") {
            settings_.updateInterval = std::stod(pair.second);
        } else if (pair.first == "outputFile") {
            settings_.outputFile = pair.second;
        } else if (pair.first == "showLegend") {
            settings_.showLegend = (pair.second == "true" || pair.second == "1");
        }
    }
    return true;
}

std::map<std::string, std::string> ASCIIArtVisualization::getConfig() const {
    std::map<std::string, std::string> config;
    config["format"] = "ASCII_ART";
    config["updateInterval"] = std::to_string(settings_.updateInterval);
    config["outputFile"] = settings_.outputFile;
    config["showLegend"] = settings_.showLegend ? "true" : "false";
    return config;
}

std::string ASCIIArtVisualization::generateNeuronActivity(const std::vector<float>& activity, int width, int height) const {
    std::ostringstream oss;
    oss << "NEURON ACTIVITY HEATMAP" << std::endl;
    oss << std::string(width, '=') << std::endl;
    
    // Normalize activity
    std::vector<float> normalized = normalizeData(activity);
    
    // Generate heatmap
    int index = 0;
    for (int y = height - 1; y >= 0; --y) {
        oss << "|";
        for (int x = 0; x < width; ++x) {
            if (index < normalized.size()) {
                int intensity = static_cast<int>(normalized[index] * 10);
                oss << getColorForIntensity(intensity);
                index++;
            } else {
                oss << " ";
            }
        }
        oss << "|" << std::endl;
    }
    
    oss << std::string(width, '-') << std::endl;
    oss << "Legend: " << getColorForIntensity(0) << " (low) " << getColorForIntensity(10) << " (high)" << std::endl;
    
    return oss.str();
}

std::string ASCIIArtVisualization::generateSynapseWeights(const std::vector<float>& weights, int width, int height) const {
    std::ostringstream oss;
    oss << "SYNAPSE WEIGHTS" << std::endl;
    oss << std::string(width, '=') << std::endl;
    
    // Normalize weights
    std::vector<float> normalized = normalizeData(weights);
    
    // Generate bar chart
    int index = 0;
    for (int y = height - 1; y >= 0; --y) {
        oss << "|";
        for (int x = 0; x < width; ++x) {
            if (index < normalized.size()) {
                int barHeight = static_cast<int>(normalized[index] * (height - 1));
                if (y <= barHeight) {
                    oss << "#";
                } else {
                    oss << " ";
                }
                index++;
            } else {
                oss << " ";
            }
        }
        oss << "|" << std::endl;
    }
    
    oss << std::string(width, '-') << std::endl;
    oss << "Weights: " << formatNumber(normalized.empty() ? 0.0 : *std::max_element(normalized.begin(), normalized.end())) << std::endl;
    
    return oss.str();
}

std::string ASCIIArtVisualization::generateBrainState(const std::map<std::string, std::vector<float>>& states) const {
    std::ostringstream oss;
    oss << "BRAIN STATE MONITOR" << std::endl;
    oss << std::string(50, '=') << std::endl;
    
    for (const auto& pair : states) {
        oss << pair.first << ": ";
        
        if (pair.second.empty()) {
            oss << "No data" << std::endl;
        } else {
            float avg = std::accumulate(pair.second.begin(), pair.second.end(), 0.0f) / pair.second.size();
            float min = *std::min_element(pair.second.begin(), pair.second.end());
            float max = *std::max_element(pair.second.begin(), pair.second.end());
            
            oss << "avg=" << formatNumber(avg) << " "
                << "min=" << formatNumber(min) << " "
                << "max=" << formatNumber(max) << std::endl;
        }
    }
    
    return oss.str();
}

std::string ASCIIArtVisualization::generateGenericVisualization(const VisualizationData& data) const {
    std::ostringstream oss;
    oss << "VISUALIZATION: " << data.title << std::endl;
    oss << "Timestamp: " << data.timestamp << std::endl;
    oss << std::string(50, '-') << std::endl;
    
    for (const auto& pair : data.data) {
        oss << pair.first << ": ";
        
        if (pair.second.empty()) {
            oss << "No data" << std::endl;
        } else {
            oss << "[";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << formatNumber(pair.second[i]);
            }
            oss << "]" << std::endl;
        }
    }
    
    return oss.str();
}

std::string ASCIIArtVisualization::addOverlayToOutput(const std::string& output, const VisualizationOverlay& overlay) const {
    std::ostringstream oss;
    oss << output << std::endl;
    oss << "OVERLAY [" << overlay.id << "] " << overlay.text << std::endl;
    return oss.str();
}

std::string ASCIIArtVisualization::getColorForIntensity(int intensity) const {
    if (intensity <= 0) return "  ";
    if (intensity <= 3) return "░";
    if (intensity <= 6) return "▒";
    if (intensity <= 9) return "▓";
    return "█";
}

bool createDirectory(const std::string& path) {
    return std::filesystem::create_directories(path);
}

bool fileExists(const std::string& filename) {
    return std::filesystem::exists(filename);
}

std::string getFileExtension(const std::string& filename) {
    std::filesystem::path path(filename);
    return path.extension().string();
}

std::string getTimestampString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

void saveToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
    }
}

// JSONVisualization implementation
bool JSONVisualization::initialize(const VisualizationSettings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    settings_ = settings;
    running_ = false;
    overlays_.clear();
    dataHistory_.clear();
    return true;
}

void JSONVisualization::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    overlays_.clear();
    dataHistory_.clear();
}

void JSONVisualization::start() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = true;
}

void JSONVisualization::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
}

void JSONVisualization::pause() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
}

void JSONVisualization::resume() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = true;
}

bool JSONVisualization::isRunning() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return running_;
}

void JSONVisualization::visualize(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!running_) return;
    
    dataHistory_.push_back(data);
    
    // Generate JSON
    std::string jsonOutput = generateJSONFrame(data);
    
    // Output
    std::cout << jsonOutput << std::endl;
    
    if (settings_.saveToFile) {
        saveToFile(settings_.outputFile, jsonOutput);
    }
}

void JSONVisualization::addOverlay(const VisualizationOverlay& overlay) {
    std::lock_guard<std::mutex> lock(mutex_);
    overlays_.push_back(overlay);
}

void JSONVisualization::removeOverlay(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    overlays_.erase(std::remove_if(overlays_.begin(), overlays_.end(),
                                  [&id](const VisualizationOverlay& o) { return o.id == id; }),
                    overlays_.end());
}

void JSONVisualization::setSettings(const VisualizationSettings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    settings_ = settings;
}

VisualizationSettings JSONVisualization::getSettings() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return settings_;
}

bool JSONVisualization::saveVisualization(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Generate current visualization
    std::string output;
    if (!dataHistory_.empty()) {
        output = generateJSONFrame(dataHistory_.back());
    }
    
    file << output;
    return true;
}

std::string JSONVisualization::generateOutput() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    
    oss << "[" << std::endl;
    
    for (size_t i = 0; i < dataHistory_.size(); ++i) {
        oss << generateJSONFrame(dataHistory_[i]);
        if (i < dataHistory_.size() - 1) {
            oss << "," << std::endl;
        }
    }
    
    oss << std::endl << "]" << std::endl;
    
    return oss.str();
}

bool JSONVisualization::configure(const std::map<std::string, std::string>& config) {
    // Similar implementation to ASCIIArtVisualization
    for (const auto& pair : config) {
        if (pair.first == "format") {
            // Parse format string
        } else if (pair.first == "updateInterval") {
            settings_.updateInterval = std::stod(pair.second);
        } else if (pair.first == "outputFile") {
            settings_.outputFile = pair.second;
        }
    }
    return true;
}

std::map<std::string, std::string> JSONVisualization::getConfig() const {
    std::map<std::string, std::string> config;
    config["format"] = "JSON";
    config["updateInterval"] = std::to_string(settings_.updateInterval);
    config["outputFile"] = settings_.outputFile;
    return config;
}

std::string JSONVisualization::generateJSONFrame(const VisualizationData& data) const {
    std::ostringstream oss;
    oss << "  {" << std::endl;
    oss << "    \"timestamp\": " << data.timestamp << "," << std::endl;
    oss << "    \"title\": \"" << data.title << "\"" << std::endl;
    
    if (!data.data.empty()) {
        oss << "    \"data\": {" << std::endl;
        bool first = true;
        for (const auto& pair : data.data) {
            if (!first) oss << "," << std::endl;
            first = false;
            
            oss << "      \"" << pair.first << "\": [";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << pair.second[i];
            }
            oss << "]";
        }
        oss << std::endl;
    }
    
    if (!data.metadata.empty()) {
        if (!data.data.empty()) {
            oss << "    ," << std::endl;
        }
        oss << "    \"metadata\": {" << std::endl;
        bool first = true;
        for (const auto& pair : data.metadata) {
            if (!first) oss << "," << std::endl;
            first = false;
            oss << "      \"" << pair.first << "\": \"" << pair.second << "\"";
        }
        oss << std::endl;
    }
    
    if (!data.labels.empty()) {
        if (!data.data.empty() || !data.metadata.empty()) {
            oss << "    ," << std::endl;
        }
        oss << "    \"labels\": [";
        for (size_t i = 0; i < data.labels.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << data.labels[i] << "\"";
        }
        oss << "]" << std::endl;
    }
    
    oss << "  }" << std::endl;
    
    return oss.str();
}

// CSVVisualization implementation
bool CSVVisualization::initialize(const VisualizationSettings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    settings_ = settings;
    running_ = false;
    overlays_.clear();
    dataHistory_.clear();
    return true;
}

void CSVVisualization::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (csvFile_.is_open()) {
        csvFile_.close();
    }
    running_ = false;
    overlays_.clear();
    dataHistory_.clear();
}

void CSVVisualization::start() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = true;
    
    if (settings_.saveToFile && !settings_.outputFile.empty()) {
        csvFile_.open(settings_.outputFile);
        if (csvFile_.is_open()) {
            csvFile_ << "Timestamp,Title,DataType,Value" << std::endl;
        }
    }
}

void CSVVisualization::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    if (csvFile_.is_open()) {
        csvFile_.close();
    }
}

void CSVVisualization::pause() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
}

void CSVVisualization::resume() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = true;
}

bool CSVVisualization::isRunning() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return running_;
}

void CSVVisualization::visualize(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!running_) return;
    
    dataHistory_.push_back(data);
    
    // Generate CSV
    std::string csvOutput = generateCSVData(data);
    
    // Output
    std::cout << csvOutput << std::endl;
    
    // Write to file if open
    if (csvFile_.is_open()) {
        csvFile_ << csvOutput;
    }
}

void CSVVisualization::addOverlay(const VisualizationOverlay& overlay) {
    std::lock_guard<std::mutex> lock(mutex_);
    overlays_.push_back(overlay);
}

void CSVVisualization::removeOverlay(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    overlays_.erase(std::remove_if(overlays_.begin(), overlays_.end(),
                                  [&id](const VisualizationOverlay& o) { return o.id == id; }),
                    overlays_.end());
}

void CSVVisualization::setSettings(const VisualizationSettings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    settings_ = settings;
}

VisualizationSettings CSVVisualization::getSettings() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return settings_;
}

bool CSVVisualization::saveVisualization(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (csvFile_.is_open()) {
        csvFile_.close();
    }
    
    if (!filename.empty()) {
        csvFile_.open(filename);
        if (!csvFile_.is_open()) {
            return false;
        }
        
        csvFile_ << "Timestamp,Title,DataType,Value" << std::endl;
    }
    
    return true;
}

std::string CSVVisualization::generateOutput() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "CSV Output:" << std::endl;
    oss << "Timestamps: ";
    for (size_t i = 0; i < dataHistory_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << dataHistory_[i].timestamp;
    }
    oss << std::endl;
    
    oss << "Titles: ";
    for (size_t i = 0; i < dataHistory_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << dataHistory_[i].title;
    }
    oss << std::endl;
    
    return oss.str();
}

bool CSVVisualization::configure(const std::map<std::string, std::string>& config) {
    // Similar implementation
    return true;
}

std::map<std::string, std::string> CSVVisualization::getConfig() const {
    std::map<std::string, std::string> config;
    config["format"] = "CSV";
    return config;
}

std::string CSVVisualization::generateCSVData(const VisualizationData& data) const {
    std::ostringstream oss;
    oss << data.timestamp << "," << data.title << ",";
    
    if (!data.data.empty()) {
        bool firstData = true;
        for (const auto& pair : data.data) {
            if (!firstData) oss << ",";
            firstData = false;
            oss << pair.first << ":";
            
            for (size_t i = 0; i < pair.second.size(); ++i) {
                if (i > 0) oss << ";";
                oss << pair.second[i];
            }
        }
    }
    
    return oss.str();
}

// DebugVisualization implementation
bool DebugVisualization::initialize(const VisualizationSettings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    settings_ = settings;
    running_ = false;
    overlays_.clear();
    debugMessages_.clear();
    performanceStats_.clear();
    return true;
}

void DebugVisualization::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    overlays_.clear();
    debugMessages_.clear();
    performanceStats_.clear();
}

void DebugVisualization::start() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = true;
}

void DebugVisualization::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
}

void DebugVisualization::pause() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
}

void DebugVisualization::resume() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = true;
}

bool DebugVisualization::isRunning() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return running_;
}

void DebugVisualization::visualize(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!running_) return;
    
    // Generate debug output
    std::string debugOutput = generateDebugInfo(data.title, "Data visualization requested");
    
    // Output debug info
    std::cout << debugOutput << std::endl;
    
    // Show performance stats
    std::string perfOutput = generatePerformanceStats(std::map<std::string, double>());
    std::cout << perfOutput << std::endl;
}

void DebugVisualization::addOverlay(const VisualizationOverlay& overlay) {
    std::lock_guard<std::mutex> lock(mutex_);
    overlays_.push_back(overlay);
}

void DebugVisualization::removeOverlay(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    overlays_.erase(std::remove_if(overlays_.begin(), overlays_.end(),
                                  [&id](const VisualizationOverlay& o) { return o.id == id; }),
                    overlays_.end());
}

void DebugVisualization::setSettings(const VisualizationSettings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    settings_ = settings;
}

VisualizationSettings DebugVisualization::getSettings() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return settings_;
}

bool DebugVisualization::saveVisualization(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    // Save debug info to file
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Debug Visualization Output" << std::endl;
    file << std::string(50, '=') << std::endl;
    
    for (const auto& msg : debugMessages_) {
        file << "[" << msg.first << "] " << msg.second << std::endl;
    }
    
    return true;
}

std::string DebugVisualization::generateOutput() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "Debug Visualization Output" << std::endl;
    oss << std::string(50, '=') << std::endl;
    
    for (const auto& msg : debugMessages_) {
        oss << "[" << msg.first << "] " << msg.second << std::endl;
    }
    
    return oss.str();
}

bool DebugVisualization::configure(const std::map<std::string, std::string>& config) {
    return true;
}

std::map<std::string, std::string> DebugVisualization::getConfig() const {
    std::map<std::string, std::string> config;
    config["format"] = "DEBUG";
    return config;
}

std::string DebugVisualization::generateDebugInfo(const std::string& title, const std::string& info) const {
    std::ostringstream oss;
    oss << "=== DEBUG INFO ===" << std::endl;
    oss << "Title: " << title << std::endl;
    oss << "Info: " << info << std::endl;
    oss << "==================" << std::endl;
    return oss.str();
}

std::string DebugVisualization::generatePerformanceStats(const std::map<std::string, double>& stats) const {
    std::ostringstream oss;
    oss << "=== PERFORMANCE STATS ===" << std::endl;
    
    for (const auto& pair : stats) {
        oss << pair.first << ": " << pair.second << std::endl;
    }
    
    oss << "========================" << std::endl;
    return oss.str();
}

// VisualizationManager implementation
bool VisualizationManager::addVisualization(VisualizationInterface* visualization) {
    if (!visualization) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Generate unique ID
    std::string id = "viz_" + std::to_string(visualizations_.size());
    
    VisualizationEntry entry;
    entry.id = id;
    entry.visualization = visualization;
    entry.enabled = true;
    entry.settings = globalSettings_;
    
    visualizations_.push_back(entry);
    
    if (activeVisualizationId_.empty()) {
        activeVisualizationId_ = id;
    }
    
    return true;
}

bool VisualizationManager::removeVisualization(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find_if(visualizations_.begin(), visualizations_.end(),
                          [&id](const VisualizationEntry& entry) { return entry.id == id; });
    
    if (it == visualizations_.end()) {
        return false;
    }
    
    if (it->visualization) {
        it->visualization->shutdown();
        delete it->visualization;
    }
    
    visualizations_.erase(it);
    
    if (activeVisualizationId_ == id) {
        activeVisualizationId_ = visualizations_.empty() ? "" : visualizations_[0].id;
    }
    
    return true;
}

bool VisualizationManager::enableVisualization(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find_if(visualizations_.begin(), visualizations_.end(),
                          [&id](const VisualizationEntry& entry) { return entry.id == id; });
    
    if (it == visualizations_.end()) {
        return false;
    }
    
    it->enabled = true;
    return true;
}

bool VisualizationManager::disableVisualization(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find_if(visualizations_.begin(), visualizations_.end(),
                          [&id](const VisualizationEntry& entry) { return entry.id == id; });
    
    if (it == visualizations_.end()) {
        return false;
    }
    
    it->enabled = false;
    return true;
}

bool VisualizationManager::setActiveVisualization(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find_if(visualizations_.begin(), visualizations_.end(),
                          [&id](const VisualizationEntry& entry) { return entry.id == id; });
    
    if (it == visualizations_.end()) {
        return false;
    }
    
    activeVisualizationId_ = id;
    return true;
}

std::string VisualizationManager::getActiveVisualization() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeVisualizationId_;
}

void VisualizationManager::startAllVisualizations() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& entry : visualizations_) {
        if (entry.enabled && entry.visualization) {
            entry.visualization->start();
        }
    }
}

void VisualizationManager::stopAllVisualizations() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& entry : visualizations_) {
        if (entry.enabled && entry.visualization) {
            entry.visualization->stop();
        }
    }
}

void VisualizationManager::pauseAllVisualizations() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& entry : visualizations_) {
        if (entry.enabled && entry.visualization) {
            entry.visualization->pause();
        }
    }
}

void VisualizationManager::resumeAllVisualizations() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& entry : visualizations_) {
        if (entry.enabled && entry.visualization) {
            entry.visualization->resume();
        }
    }
}

void VisualizationManager::visualize(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& entry : visualizations_) {
        if (entry.enabled && entry.visualization) {
            entry.visualization->visualize(data);
        }
    }
}

void VisualizationManager::addDataSource(const std::string& id, const std::function<VisualizationData()>& source) {
    std::lock_guard<std::mutex> lock(mutex_);
    dataSources_[id] = source;
}

void VisualizationManager::setGlobalSettings(const VisualizationSettings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    globalSettings_ = settings;
    
    for (auto& entry : visualizations_) {
        entry.settings = settings;
    }
}

VisualizationSettings VisualizationManager::getGlobalSettings() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return globalSettings_;
}

bool VisualizationManager::saveAllVisualizations(const std::string& directory) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!createDirectory(directory)) {
        return false;
    }
    
    for (size_t i = 0; i < visualizations_.size(); ++i) {
        std::string filename = directory + "/visualization_" + std::to_string(i) + ".txt";
        if (visualizations_[i].visualization) {
            visualizations_[i].visualization->saveVisualization(filename);
        }
    }
    
    return true;
}

bool VisualizationManager::exportAllData(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Visualization Export" << std::endl;
    file << std::string(50, '=') << std::endl;
    file << "Total Visualizations: " << visualizations_.size() << std::endl;
    file << "Active Visualization: " << activeVisualizationId_ << std::endl;
    
    for (size_t i = 0; i < visualizations_.size(); ++i) {
        file << "\nVisualization " << i << ":" << std::endl;
        if (visualizations_[i].visualization) {
            std::string output = visualizations_[i].visualization->generateOutput();
            file << output;
        }
    }
    
    return true;
}

void VisualizationManager::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& entry : visualizations_) {
        if (entry.visualization) {
            entry.visualization->shutdown();
            delete entry.visualization;
        }
    }
    
    visualizations_.clear();
    activeVisualizationId_.clear();
    dataSources_.clear();
}

size_t VisualizationManager::getVisualizationCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return visualizations_.size();
}

size_t VisualizationManager::getEnabledVisualizationCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t count = 0;
    for (const auto& entry : visualizations_) {
        if (entry.enabled) {
            count++;
        }
    }
    return count;
}

bool VisualizationManager::loadConfiguration(const std::string& filename) {
    // In real implementation, would load from JSON/XML config file
    return true;
}

bool VisualizationManager::saveConfiguration(const std::string& filename) const {
    // In real implementation, would save to JSON/XML config file
    return true;
}

// VisualizationFactory implementation
VisualizationInterface* VisualizationFactory::createVisualization(const std::string& type) {
    for (const auto& info : visualizations_) {
        if (info.type == type) {
            return info.creator();
        }
    }
    return nullptr;
}

VisualizationInterface* VisualizationFactory::createVisualization(const std::string& type, const VisualizationSettings& settings) {
    VisualizationInterface* visualization = createVisualization(type);
    if (visualization) {
        visualization->initialize(settings);
    }
    return visualization;
}

std::vector<std::string> VisualizationFactory::getAvailableVisualizations() {
    std::vector<std::string> types;
    for (const auto& info : visualizations_) {
        types.push_back(info.type);
    }
    return types;
}

bool VisualizationFactory::isVisualizationAvailable(const std::string& type) {
    for (const auto& info : visualizations_) {
        if (info.type == type) {
            return true;
        }
    }
    return false;
}

bool VisualizationFactory::registerVisualization(const std::string& type, const std::string& description) {
    if (isVisualizationAvailable(type)) {
        return false; // Already registered
    }
    
    // In real implementation, would create factory function
    VisualizationInfo info;
    info.type = type;
    info.description = description;
    // info.creator = ...; // Would be provided by plugin
    
    visualizations_.push_back(info);
    return true;
}

bool VisualizationFactory::unregisterVisualization(const std::string& type) {
    for (auto it = visualizations_.begin(); it != visualizations_.end(); ++it) {
        if (it->type == type) {
            visualizations_.erase(it);
            return true;
        }
    }
    return false;
}

bool VisualizationFactory::loadVisualizationPlugin(const std::string& path) {
    // In real implementation, would load dynamic library
    return true;
}

bool VisualizationFactory::unloadVisualizationPlugin(const std::string& name) {
    // In real implementation, would unload dynamic library
    return true;
}

// Utility functions
namespace visualization_utils {
    std::string getColor(const std::string& colorName) {
        // Color mapping
        static std::map<std::string, std::string> colors = {
            {"red", "#FF0000"},
            {"green", "#00FF00"},
            {"blue", "#0000FF"},
            {"white", "#FFFFFF"},
            {"black", "#000000"},
            {"gray", "#808080"},
            {"yellow", "#FFFF00"},
            {"cyan", "#00FFFF"},
            {"magenta", "#FF00FF"}
        };
        
        auto it = colors.find(colorName);
        if (it != colors.end()) {
            return it->second;
        }
        return "#000000"; // Default black
    }
    
    std::string interpolateColor(const std::string& color1, const std::string& color2, double ratio) {
        // In real implementation, would interpolate between two hex colors
        return color1; // Placeholder
    }
    
    std::string hexToRGB(const std::string& hex) {
        // In real implementation, would convert hex to RGB
        return hex;
    }
    
    std::string RGBToHex(int r, int g, int b) {
        std::ostringstream oss;
        oss << "#";
        oss << std::hex << std::setfill('0') << std::setw(2) << r;
        oss << std::hex << std::setfill('0') << std::setw(2) << g;
        oss << std::hex << std::setfill('0') << std::setw(2) << b;
        return oss.str();
    }
    
    std::string scaleText(const std::string& text, int scale) {
        std::string result;
        for (char c : text) {
            result += std::string(scale, c);
        }
        return result;
    }
    
    std::string centerText(const std::string& text, int width) {
        int padding = (width - text.length()) / 2;
        return std::string(padding, ' ') + text + std::string(padding, ' ');
    }
    
    std::string wrapText(const std::string& text, int width) {
        std::ostringstream oss;
        std::string currentLine;
        std::istringstream iss(text);
        std::string word;
        
        while (iss >> word) {
            if (currentLine.empty()) {
                currentLine = word;
            } else if (currentLine.length() + 1 + word.length() <= static_cast<size_t>(width)) {
                currentLine += " " + word;
            } else {
                oss << currentLine << std::endl;
                currentLine = word;
            }
        }
        
        if (!currentLine.empty()) {
            oss << currentLine;
        }
        
        return oss.str();
    }
    
    std::vector<float> normalizeData(const std::vector<float>& data) {
        std::vector<float> normalized = data;
        
        if (!normalized.empty()) {
            float min = *std::min_element(normalized.begin(), normalized.end());
            float max = *std::max_element(normalized.begin(), normalized.end());
            float range = max - min;
            
            if (range > 0) {
                for (auto& value : normalized) {
                    value = (value - min) / range;
                }
            }
        }
        
        return normalized;
    }
    
    std clonedata::clampData(const std::vector<float>& data, float min, float max) {
        std::vector<float> clamped = data;
        
        for (auto& value : clamped) {
            value = std::max(min, std::min(value, max));
        }
        
        return clamped;
    }
    
    std::vector<float> smoothData(const std::vector<float>& data, int windowSize) {
        std::vector<float> smoothed;
        
        if (data.empty() || windowSize <= 0) {
            return smoothed;
        }
        
        for (size_t i = 0; i < data.size(); ++i) {
            int start = std::max<int>(0, static_cast<int>(i) - windowSize / 2);
            int end = std::min<int>(static_cast<int>(data.size()), static_cast<int>(i) + windowSize / 2 + 1);
            
            float sum = std::accumulate(data.begin() + start, data.begin() + end, 0.0f);
            float avg = sum / (end - start);
            
            smoothed.push_back(avg);
        }
        
        return smoothed;
    }
    
    bool createDirectory(const std::string& path) {
        return std::filesystem::create_directories(path);
    }
    
    bool fileExists(const std::string& filename) {
        return std::filesystem::exists(filename);
    }
    
    std::string getFileExtension(const std::string& filename) {
        std::filesystem::path path(filename);
        return path.extension().string();
    }
    
    std::string getTimestampString() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    std::string formatNumber(double number, int precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << number;
        return oss.str();
    }
    
    std::string formatPercentage(double value, int precision) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << (value * 100) << "%";
        return oss.str();
    }
    
    std::string formatTime(double seconds) {
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds / 60) - hours * 60);
        int secs = static_cast<int>(seconds - hours * 3600 - minutes * 60);
        
        std::ostringstream oss;
        if (hours > 0) {
            oss << hours << "h ";
        }
        oss << minutes << "m " << secs << "s";
        return oss.str();
    }
    
    std::string generateHeatmap(const std::vector<float>& data, int width, int height) {
        std::ostringstream oss;
        oss << "HEATMAP" << std::endl;
        oss << std::string(width, '=') << std::endl;
        
        // Generate heatmap
        int index = 0;
        for (int y = height - 1; y >= 0; --y) {
            oss << "|";
            for (int x = 0; x < width; ++x) {
                if (index < data.size()) {
                    int intensity = static_cast<int>(data[index] * 10);
                    oss << getColorForIntensity(intensity);
                    index++;
                } else {
                    oss << " ";
                }
            }
            oss << "|" << std::endl;
        }
        
        oss << std::string(width, '-') << std::endl;
        return oss.str();
    }
    
    std::string generateBarChart(const std::vector<float>& values, const std::vector<std::string>& labels) {
        std::ostringstream oss;
        oss << "BAR CHART" << std::endl;
        oss << std::string(50, '-') << std::endl;
        
        for (size_t i = 0; i < std::min(values.size(), labels.size()); ++i) {
            oss << labels[i] << ": ";
            int barLength = static_cast<int>(values[i] * 50);
            for (int j = 0; j < barLength; ++j) {
                oss << "*";
            }
            oss << " (" << formatNumber(values[i]) << ")" << std::endl;
        }
        
        return oss.str();
    }
    
    std::string generateLineChart(const std::vector<float>& values, const std::vector<Timestamp>& timestamps) {
        std::ostringstream oss;
        oss << "LINE CHART" << std::endl;
        oss << std::string(50, '-') << std::endl;
        
        // In real implementation, would create proper line chart
        for (size_t i = 0; i < values.size(); ++i) {
            oss << "Step " << i << ": " << formatNumber(values[i]);
            if (i < timestamps.size()) {
                oss << " (" << timestamps[i] << ")";
            }
            oss << std::endl;
        }
        
        return oss.str();
    }
    
    VisualizationSettings parseSettingsFromString(const std::string& settingsStr) {
        VisualizationSettings settings;
        // In real implementation, would parse from string
        return settings;
    }
    
    std::string generateSettingsString(const VisualizationSettings& settings) {
        std::ostringstream oss;
        oss << "Visualization Settings:" << std::endl;
        oss << "Format: " << static_cast<int>(settings.format) << std::endl;
        oss << "Update Interval: " << settings.updateInterval << std::endl;
        oss << "Enabled: " << (settings.enabled ? "true" : "false") << std::endl;
        oss << "Save to File: " << (settings.saveToFile ? "true" : "false") << std::endl;
        if (!settings.outputFile.empty()) {
            oss << "Output File: " << settings.outputFile << std::endl;
        }
        return oss.str();
    }
    
    std::string getColorForIntensity(int intensity) {
        if (intensity <= 0) return "  ";
        if (intensity <= 3) return "░";
        if (intensity <= 6) return "▒";
        if (intensity <= 9) return "▓";
        return "█";
    }
} // namespace visualization_utils

} // namespace nlm