// VisualizationInterface implementation
#include "visualization/VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <cmath>

namespace nlm {

// Forward declarations for implementation details
namespace detail {
    struct WebDashboardImpl;
    struct MonitorSystemImpl;
    struct TerminalVisualizationImpl;
}

// WebDashboard implementation
WebDashboard::WebDashboard(int port, const std::string& host)
    : port_(port), host_(host), running_(false) {
}

WebDashboard::~WebDashboard() {
    stop();
}

bool WebDashboard::initialize() {
    NLM_LOG_INFO("Initializing WebDashboard on " + host_ + ":" + std::to_string(port_));
    // TODO: Implement web server initialization
    // For now, just return success
    return true;
}

void WebDashboard::update(const Brain* brain, double timestep) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Update brain data
    if (brain) {
        brainData_ = {
            {"neurons", static_cast<double>(brain->getTotalNeuronCount())},
            {"synapses", static_cast<double>(brain->getTotalSynapseCount())},
            {"active_neurons", static_cast<double>(brain->getActiveNeuronCount())},
            {"firing_neurons", static_cast<double>(brain->getFiringNeuronCount())},
            {"avg_firing_rate", brain->getAverageFiringRate()},
            {"e_i_ratio", brain->getExcitationInhibitionRatio()},
            {"total_spikes", static_cast<double>(brain->getTotalSpikeCount())},
            {"pending_spikes", static_cast<double>(brain->getPendingSpikeEventCount())}
        };
        
        // Update memory system data
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            memoryData_["working_memory_traces"] = static_cast<double>(workingMemory->getActiveTraces());
        }
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            memoryData_["episodic_episodes"] = static_cast<double>(episodicMemory->getEpisodeCount());
        }
        
        // Update neuromodulation data
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            neuromodulationData_["dopamine"] = dopamine->getLevel();
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            neuromodulationData_["curiosity"] = curiosity->getLevel();
        }
        
        auto* novelty = brain->getNovelty();
        if (novelty) {
            neuromodulationData_["novelty"] = novelty->getLevel();
        }
        
        // Update development data
        developmentData_["developmental_stage"] = static_cast<double>(brain->getDevelopmentalStage());
    }
    
    // Notify WebSocket clients
    if (wsHandler_) {
        wsHandler_(getDashboardStateJSON());
    }
}

void WebDashboard::render() {
    // WebSocket server renders
    // For now, just log
    if (!quietMode_) {
        std::cout << "WebDashboard rendering with " << brainData_.size() << " metrics" << std::endl;
    }
}

void WebDashboard::start() {
    if (!running_) {
        running_ = true;
        // Start web server in background thread
        serverThread_ = std::thread(&WebDashboard::runServer, this);
        NLM_LOG_INFO("WebDashboard started on " + host_ + ":" + std::to_string(port_));
    }
}

void WebDashboard::stop() {
    if (running_) {
        running_ = false;
        if (serverThread_.joinable()) {
            serverThread_.join();
        }
        NLM_LOG_INFO("WebDashboard stopped");
    }
}

bool WebDashboard::isRunning() const {
    return running_;
}

void WebDashboard::setUpdateFrequency(double frequency) {
    updateFrequency_ = frequency;
    updateInterval_ = 1000.0 / frequency; // Convert to milliseconds
}

void WebDashboard::setWebSocketHandler(std::function<void(const std::string&)> handler) {
    wsHandler_ = std::move(handler);
}

void WebDashboard::serveStaticFiles(const std::string& path) {
    staticFilesPath_ = path;
    // TODO: Implement static file serving
}

void WebDashboard::addMetric(const std::string& name, double value) {
    customMetrics_[name] = value;
}

void WebDashboard::addBrainData(const std::string& key, const std::vector<float>& data) {
    brainData_[key] = data;
}

std::string WebDashboard::getStatus() const {
    std::ostringstream oss;
    oss << "WebDashboard Status:" << std::endl;
    oss << "  Running: " << (running_ ? "Yes" : "No") << std::endl;
    oss << "  Port: " << port_ << std::endl;
    oss << "  Host: " << host_ << std::endl;
    oss << "  Update Frequency: " << updateFrequency_ << " Hz" << std::endl;
    oss << "  Brain Metrics: " << brainData_.size() << std::endl;
    oss << "  Memory Metrics: " << memoryData_.size() << std::endl;
    oss << "  Neuromodulation Metrics: " << neuromodulationData_.size() << std::endl;
    oss << "  Development Metrics: " << developmentData_.size() << std::endl;
    oss << "  Custom Metrics: " << customMetrics_.size() << std::endl;
    return oss.str();
}

void WebDashboard::runServer() {
    // TODO: Implement web server with WebSocket support
    // This is a placeholder implementation
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // Update and broadcast data
        if (wsHandler_) {
            wsHandler_(getDashboardStateJSON());
        }
    }
}

std::string WebDashboard::getDashboardStateJSON() const {
    json j;
    
    j["timestamp"] = currentTime_; // TODO: Get current time
    j["version"] = "1.0.0";
    
    j["brain_data"] = brainData_;
    j["memory_data"] = memoryData_;
    j["neuromodulation_data"] = neuromodulationData_;
    j["development_data"] = developmentData_;
    j["custom_metrics"] = customMetrics_;
    
    return j.dump();
}

// MonitorSystem implementation
MonitorSystem::MonitorSystem()
    : running_(false), samplingInterval_(1000.0) { // 1 second default
}

MonitorSystem::~MonitorSystem() {
    stop();
}

bool MonitorSystem::initialize() {
    NLM_LOG_INFO("Initializing MonitorSystem");
    return true;
}

void MonitorSystem::start() {
    if (!running_) {
        running_ = true;
        monitorThread_ = std::thread(&MonitorSystem::monitorLoop, this);
        NLM_LOG_INFO("MonitorSystem started");
    }
}

void MonitorSystem::stop() {
    if (running_) {
        running_ = false;
        if (monitorThread_.joinable()) {
            monitorThread_.join();
        }
        NLM_LOG_INFO("MonitorSystem stopped");
    }
}

bool MonitorSystem::isRunning() const {
    return running_;
}

void MonitorSystem::addMetric(const std::string& name, const std::function<double()>& getter) {
    std::lock_guard<std::mutex> lock(mutex_);
    metricGetters_[name] = getter;
    NLM_LOG_INFO("Added metric: " + name);
}

void MonitorSystem::update() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate_);
    
    if (duration.count() >= samplingInterval_) {
        for (const auto& pair : metricGetters_) {
            try {
                metrics_[pair.first] = pair.second();
            } catch (const std::exception& e) {
                NLM_LOG_WARN("Error getting metric " + pair.first + ": " + std::string(e.what()));
            }
        }
        
        lastUpdate_ = now;
        
        // Log significant changes
        if (!quietMode_) {
            for (const auto& pair : metrics_) {
                if (pair.first == "neurons" || pair.first == "firing_neurons" ||
                    pair.first == "total_spikes") {
                    NLM_LOG_INFO(pair.first + ": " + std::to_string(pair.second));
                }
            }
        }
    }
}

double MonitorSystem::getMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = metrics_.find(name);
    return (it != metrics_.end()) ? it->second : 0.0;
}

std::unordered_map<std::string, double> MonitorSystem::getAllMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metrics_;
}

void MonitorSystem::setSamplingInterval(double interval) {
    std::lock_guard<std::mutex> lock(mutex_);
    samplingInterval_ = interval;
}

double MonitorSystem::getSamplingInterval() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return samplingInterval_;
}

void MonitorSystem::monitorBrain(const Brain* brain) {
    if (!brain) {
        NLM_LOG_WARN("Attempted to monitor null brain");
        return;
    }
    
    monitoredBrain_ = brain;
    
    // Add standard metrics
    addMetric("neurons", [brain]() { return static_cast<double>(brain->getTotalNeuronCount()); });
    addMetric("synapses", [brain]() { return static_cast<double>(brain->getTotalSynapseCount()); });
    addMetric("active_neurons", [brain]() { return static_cast<double>(brain->getActiveNeuronCount()); });
    addMetric("firing_neurons", [brain]() { return static_cast<double>(brain->getFiringNeuronCount()); });
    addMetric("avg_firing_rate", [brain]() { return brain->getAverageFiringRate(); });
    addMetric("e_i_ratio", [brain]() { return brain->getExcitationInhibitionRatio(); });
    addMetric("total_spikes", [brain]() { return static_cast<double>(brain->getTotalSpikeCount()); });
    addMetric("pending_spikes", [brain]() { return static_cast<double>(brain->getPendingSpikeEventCount()); });
    
    // Memory metrics
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        addMetric("working_memory_traces", [workingMemory]() { return static_cast<double>(workingMemory->getActiveTraces()); });
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        addMetric("episodic_episodes", [episodicMemory]() { return static_cast<double>(episodicMemory->getEpisodeCount()); });
    }
    
    // Neuromodulation metrics
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        addMetric("dopamine", [dopamine]() { return dopamine->getLevel(); });
    }
    
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        addMetric("curiosity", [curiosity]() { return curiosity->getLevel(); });
    }
    
    auto* novelty = brain->getNovelty();
    if (novelty) {
        addMetric("novelty", [novelty]() { return novelty->getLevel(); });
    }
    
    // Development metrics
    addMetric("developmental_stage", [brain]() { return static_cast<double>(brain->getDevelopmentalStage()); });
}

void MonitorSystem::stopMonitoringBrain() {
    if (monitoredBrain_) {
        // Remove all metrics
        std::lock_guard<std::mutex> lock(mutex_);
        metricGetters_.clear();
        metrics_.clear();
        monitoredBrain_ = nullptr;
        NLM_LOG_INFO("Stopped monitoring brain");
    }
}

bool MonitorSystem::exportMetrics(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open metrics file for writing: " + filepath);
        return false;
    }
    
    // Export as JSON
    json j;
    for (const auto& pair : metrics_) {
        j[pair.first] = pair.second;
    }
    
    file << std::setw(2) << j;
    
    NLM_LOG_INFO("Exported metrics to: " + filepath);
    return true;
}

std::string MonitorSystem::getPerformanceStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "MonitorSystem Performance Statistics:" << std::endl;
    oss << "  Sampling Interval: " << samplingInterval_ << " ms" << std::endl;
    oss << "  Monitored Metrics: " << metricGetters_.size() << std::endl;
    oss << "  Current Metrics: " << metrics_.size() << std::endl;
    oss << "  Brain Monitoring: " << (monitoredBrain_ ? "Active" : "Inactive") << std::endl;
    
    if (!metrics_.empty()) {
        oss << "  Sample Metric Values:" << std::endl;
        for (const auto& pair : metrics_) {
            if (pair.first.size() < 20) { // Only show short names
                oss << "    " << pair.first << ": " << pair.second << std::endl;
            }
        }
    }
    
    return oss.str();
}

void MonitorSystem::monitorLoop() {
    while (running_) {
        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(samplingInterval_)));
    }
}

// TerminalVisualization implementation
TerminalVisualization::TerminalVisualization()
    : running_(false), updateFrequency_(10.0), quietMode_(false) {
}

TerminalVisualization::~TerminalVisualization() {
    stop();
}

bool TerminalVisualization::initialize() {
    NLM_LOG_INFO("Initializing TerminalVisualization");
    return true;
}

void TerminalVisualization::update(const Brain* brain, double timestep) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentBrain_ = brain;
    currentTimestep_ = timestep;
}

void TerminalVisualization::render() {
    if (!currentBrain_ || !running_) {
        return;
    }
    
    // Clear screen
    std::cout << "\033[2J\033[H";
    
    // Header
    std::cout << "=== NLM Brain Visualization ===" << std::endl;
    std::cout << "Timestamp: " << currentTime_ << " ms" << std::endl;
    std::cout << std::endl;
    
    // Brain statistics
    std::cout << "Brain Statistics:" << std::endl;
    std::cout << "  Neurons: " << currentBrain_->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << currentBrain_->getTotalSynapseCount() << std::endl;
    std::cout << "  Active Neurons: " << currentBrain_->getActiveNeuronCount() << std::endl;
    std::cout << "  Firing Neurons: " << currentBrain_->getFiringNeuronCount() << std::endl;
    std::cout << "  Average Firing Rate: " << currentBrain_->getAverageFiringRate() << std::endl;
    std::cout << "  E/I Ratio: " << currentBrain_->getExcitationInhibitionRatio() << std::endl;
    std::cout << "  Total Spikes: " << currentBrain_->getTotalSpikeCount() << std::endl;
    std::cout << "  Pending Spikes: " << currentBrain_->getPendingSpikeEventCount() << std::endl;
    std::cout << std::endl;
    
    // Memory statistics
    auto* workingMemory = currentBrain_->getWorkingMemory();
    if (workingMemory) {
        std::cout << "Working Memory: " << workingMemory->getActiveTraces() << " traces" << std::endl;
    }
    
    auto* episodicMemory = currentBrain_->getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "Episodic Memory: " << episodicMemory->getEpisodeCount() << " episodes" << std::endl;
    }
    std::cout << std::endl;
    
    // Neuromodulation statistics
    auto* dopamine = currentBrain_->getDopamine();
    if (dopamine) {
        std::cout << "Dopamine Level: " << dopamine->getLevel() << std::endl;
    }
    
    auto* curiosity = currentBrain_->getCuriosity();
    if (curiosity) {
        std::cout << "Curiosity Level: " << curiosity->getLevel() << std::endl;
    }
    
    auto* novelty = currentBrain_->getNovelty();
    if (novelty) {
        std::cout << "Novelty Level: " << novelty->getLevel() << std::endl;
    }
    
    // Development statistics
    std::cout << "Development Stage: " << static_cast<int>(currentBrain_->getDevelopmentalStage()) << std::endl;
    std::cout << std::endl;
    
    // Simple activity bar
    int neuronCount = currentBrain_->getTotalNeuronCount();
    int firingCount = currentBrain_->getFiringNeuronCount();
    if (neuronCount > 0) {
        int barWidth = 50;
        float firingRate = static_cast<float>(firingCount) / neuronCount;
        int barLength = static_cast<int>(barWidth * firingRate);
        
        std::cout << "Firing Activity:" << std::endl;
        std::cout << "  [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < barLength) {
                std::cout << "#";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "] " << firingCount << "/" << neuronCount << " (" 
                  << (firingRate * 100) << "%)" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
}

void TerminalVisualization::start() {
    if (!running_) {
        running_ = true;
        renderThread_ = std::thread(&TerminalVisualization::renderLoop, this);
        NLM_LOG_INFO("TerminalVisualization started");
    }
}

void TerminalVisualization::stop() {
    if (running_) {
        running_ = false;
        if (renderThread_.joinable()) {
            renderThread_.join();
        }
        NLM_LOG_INFO("TerminalVisualization stopped");
    }
}

bool TerminalVisualization::isRunning() const {
    return running_;
}

void TerminalVisualization::setUpdateFrequency(double frequency) {
    updateFrequency_ = frequency;
    updateInterval_ = 1000.0 / frequency; // Convert to milliseconds
}

void TerminalVisualization::setOutputStream(std::ostream& output) {
    outputStream_ = &output;
}

void TerminalVisualization::enableColors(bool enable) {
    useColors_ = enable;
}

void TerminalVisualization::configureLayout(const std::string& layout) {
    layout_ = layout;
}

std::string TerminalVisualization::getLayout() const {
    return layout_;
}

void TerminalVisualization::renderLoop() {
    auto lastUpdate_ = std::chrono::high_resolution_clock::now();
    
    while (running_) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate_);
        
        if (duration.count() >= updateInterval_) {
            update(currentBrain_, currentTimestep_);
            lastUpdate_ = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// VisualizationFactory implementation
std::unique_ptr<VisualizationInterface> VisualizationFactory::createVisualization(
    const std::string& type,
    const std::unordered_map<std::string, std::string>& options) {
    
    if (type == "terminal" || type == "cli") {
        auto visualization = std::make_unique<TerminalVisualization>();
        
        // Apply options
        auto it = options.find("quiet");
        if (it != options.end() && it->second == "true") {
            visualization->setOutputStream(std::cout);
        }
        
        it = options.find("colors");
        if (it != options.end() && it->second == "true") {
            visualization->enableColors(true);
        }
        
        return visualization;
    }
    else if (type == "web" || type == "dashboard") {
        int port = 8080;
        auto it = options.find("port");
        if (it != options.end()) {
            port = std::stoi(it->second);
        }
        
        std::string host = "localhost";
        it = options.find("host");
        if (it != options.end()) {
            host = it->second;
        }
        
        return std::make_unique<WebDashboard>(port, host);
    }
    else {
        NLM_LOG_ERROR("Unknown visualization type: " + type);
        return nullptr;
    }
}

std::unique_ptr<MonitorSystem> VisualizationFactory::createMonitorSystem(
    const std::unordered_map<std::string, std::string>& options) {
    auto monitor = std::make_unique<MonitorSystem>();
    
    // Apply options
    auto it = options.find("sampling_interval");
    if (it != options.end()) {
        monitor->setSamplingInterval(std::stod(it->second));
    }
    
    return monitor;
}

std::vector<std::string> VisualizationFactory::getAvailableVisualizationTypes() {
    return {"terminal", "cli", "web", "dashboard"};
}

std::vector<std::string> VisualizationFactory::getAvailableMonitorTypes() {
    return {"basic", "detailed", "performance", "memory"};
}

} // namespace nlm

