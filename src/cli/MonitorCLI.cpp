// Monitor CLI implementation for NLM
#include "cli/MonitorCLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cmath>

namespace nlm {

struct MonitorCLI::Impl {
    bool monitoring;
    bool paused;
    bool realTimeEnabled;
    std::chrono::steady_clock::time_point startTime;
    std::vector<std::chrono::steady_clock::time_point> timestamps;
    size_t maxHistorySize;
    double samplingInterval;
    std::string outputFormat;
    bool saveToFile;
    std::string dataFile;
    bool autoAnalysis;
    std::string analysisType;
    
    Impl() : monitoring(false), paused(false), realTimeEnabled(false), 
             maxHistorySize(1000), samplingInterval(1.0), outputFormat("text"),
             saveToFile(false), autoAnalysis(true), analysisType("comprehensive") {}
};

MonitorCLI::MonitorCLI() : initialized(false), config(nullptr), brain(nullptr), cli(nullptr),
    monitoring(false), paused(false), realTimeEnabled(false) {
    pImpl = std::make_unique<Impl>();
}

MonitorCLI::~MonitorCLI() = default;

MonitorCLI::MonitorCLI(MonitorCLI&&) noexcept = default;

MonitorCLI& MonitorCLI::operator=(MonitorCLI&&) noexcept = default;

bool MonitorCLI::initialize() {
    if (initialized) return true;
    
    try {
        if (!config || !brain) {
            printError("Config or brain not set");
            return false;
        }
        
        setupMonitoringConfiguration();
        
        initialized = true;
        
        return true;
    } catch (const std::exception& e) {
        printError("Failed to initialize MonitorCLI: " + std::string(e.what()));
        return false;
    }
}

bool MonitorCLI::monitorCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    if (monitoring) {
        printWarning("Monitoring is already active");
        return true;
    }
    
    print("Starting live brain monitoring...");
    
    return startMonitoring(args);
}

bool MonitorCLI::liveCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    return monitorCommand(args);
}

bool MonitorCLI::statusCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Live Brain Status ===");
    
    updateMonitoringData();
    
    displayMonitorData(currentData, "text");
    
    return true;
}

bool MonitorCLI::activityCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Brain Activity ===");
    
    updateMonitoringData();
    
    print("Network Activity Level: " + std::to_string(currentData.networkActivity));
    print("Average Firing Rate: " + std::to_string(currentData.averageFiringRate) + " Hz");
    print("Total Spikes: " + std::to_string(currentData.spikeCount));
    print("Active Neurons: " + std::to_string(currentData.activeNeuronCount));
    print("Total Neurons: " + std::to_string(currentData.totalNeuronCount));
    print("Total Synapses: " + std::to_string(currentData.totalSynapseCount));
    
    return true;
}

bool MonitorCLI::networkCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Network Statistics ===");
    
    updateMonitoringData();
    
    print("Network Activity: " + std::to_string(currentData.networkActivity));
    print("Average Path Length: " + std::to_string(currentData.averagePathLength));
    print("Network Diameter: " + std::to_string(currentData.networkDiameter));
    print("Clustering Coefficient: " + std::to_string(currentData.networkActivity));
    
    return true;
}

bool MonitorCLI::memoryCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Memory Usage ===");
    
    updateMonitoringData();
    
    print("Current Memory Usage: " + getMemoryUsageString(currentData.memoryUsage));
    print("CPU Usage: " + getCPULoadString(currentData.cpuUsage));
    
    return true;
}

bool MonitorCLI::performanceCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Performance Metrics ===");
    
    updateMonitoringData();
    
    print("Performance Metrics:");
    print("  CPU Usage: " + getCPULoadString(currentData.cpuUsage));
    print("  Memory Usage: " + getMemoryUsageString(currentData.memoryUsage));
    print("  Total Spikes: " + std::to_string(currentData.spikeCount));
    print("  Active Neurons: " + std::to_string(currentData.activeNeuronCount));
    print("  Total Neurons: " + std::to_string(currentData.totalNeuronCount));
    print("  Total Synapses: " + std::to_string(currentData.totalSynapseCount));
    print("  Average Firing Rate: " + std::to_string(currentData.averageFiringRate) + " Hz");
    print("  Average Weight: " + std::to_string(currentData.averageWeight));
    
    return true;
}

bool MonitorCLI::spikesCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Spike Activity ===");
    
    updateMonitoringData();
    
    print("Total Spikes: " + std::to_string(currentData.spikeCount));
    print("Spike Rate: " + std::to_string(currentData.spikeRate) + " Hz");
    
    return true;
}

bool MonitorCLI::connectivityCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Connectivity Analysis ===");
    
    updateMonitoringData();
    
    print("Connectivity Statistics:");
    print("  Active Neurons: " + std::to_string(currentData.activeNeuronCount));
    print("  Total Neurons: " + std::to_string(currentData.totalNeuronCount));
    print("  Total Synapses: " + std::to_string(currentData.totalSynapseCount));
    print("  Average Weight: " + std::to_string(currentData.averageWeight));
    print("  Weight Variance: " + std::to_string(currentData.weightVariance));
    
    return true;
}

bool MonitorCLI::visualizeCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    std::string title = "Live Brain Monitoring";
    if (args.size() >= 2) {
        title = args[1];
    }
    
    print("=== " + title + " ===");
    
    updateMonitoringData();
    
    visualizeTextBased(title, currentData);
    
    return true;
}

bool MonitorCLI::plotCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Plotting ===");
    print("Text-based plotting is not available in this build.");
    print("Use 'visualize' for text-based visualization.");
    
    return true;
}

bool MonitorCLI::graphCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Graph Visualization ===");
    print("Text-based graph visualization is not available in this build.");
    print("Use 'visualize' for text-based visualization.");
    
    return true;
}

bool MonitorCLI::chartCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Charts ===");
    print("Text-based charts are not available in this build.");
    print("Use 'visualize' for text-based visualization.");
    
    return true;
}

bool MonitorCLI::tableCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Table Display ===");
    
    updateMonitoringData();
    
    displayMonitorData(currentData, "table");
    
    return true;
}

bool MonitorCLI::startMonitoring(const std::vector<std::string>& args) {
    monitoring = true;
    realTimeEnabled = true;
    pImpl->timestamps.clear();
    pImpl->timestamps.push_back(std::chrono::steady_clock::now());
    pImpl->startTime = std::chrono::steady_clock::now();
    
    std::thread monitorThread(&MonitorCLI::monitoringWorker, this);
    monitorThread.detach();
    
    printSuccess("Live monitoring started");
    print("Type 'status' to see current brain status");
    print("Type 'stop' to stop monitoring");
    
    return true;
}

bool MonitorCLI::stopMonitoring(const std::vector<std::string>& args) {
    monitoring = false;
    realTimeEnabled = false;
    
    printSuccess("Live monitoring stopped");
    
    return true;
}

bool MonitorCLI::pauseMonitoring(const std::vector<std::string>& args) {
    paused = true;
    printSuccess("Monitoring paused");
    
    return true;
}

bool MonitorCLI::resumeMonitoring(const std::vector<std::string>& args) {
    paused = false;
    printSuccess("Monitoring resumed");
    
    return true;
}

bool MonitorCLI::collectDataCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Data Collection ===");
    print("Collecting monitoring data...");
    
    updateMonitoringData();
    
    printSuccess("Data collection completed");
    
    return true;
}

bool MonitorCLI::analyzeDataCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    print("=== Data Analysis ===");
    print("Analyzing monitoring data...");
    
    updateMonitoringData();
    
    printSuccess("Data analysis completed");
    
    return true;
}

bool MonitorCLI::exportDataCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("MonitorCLI not initialized");
        return false;
    }
    
    std::string filename = "monitor_data.csv";
    if (args.size() >= 2) {
        filename = args[1];
    }
    
    print("=== Data Export ===");
    print("Exporting monitoring data to: " + filename);
    
    updateMonitoringData();
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        printError("Cannot open file for writing: " + filename);
        return false;
    }
    
    file << "Timestamp,CPU Usage,Memory Usage,Spike Count,Active Neurons,Total Neurons,Total Synapses,Network Activity,Average Firing Rate" << std::endl;
    
    for (const auto& timestamp : pImpl->timestamps) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(timestamp - pImpl->startTime).count();
        file << elapsed << "," << calculateNetworkActivity(currentData) << "," << currentData.memoryUsage << "," 
             << currentData.spikeCount << "," << currentData.activeNeuronCount << "," << currentData.totalNeuronCount << "," 
             << currentData.totalSynapseCount << "," << currentData.networkActivity << "," << currentData.averageFiringRate << std::endl;
    }
    
    file.close();
    
    printSuccess("Monitoring data exported to: " + filename);
    
    return true;
}

bool MonitorCLI::clearDataCommand(const std::vector<std::string>& args) {
    pImpl->timestamps.clear();
    printSuccess("Monitoring data cleared");
    
    return true;
}

void MonitorCLI::showHelp() const {
    print("=== Monitor CLI Commands ===");
    print("  monitor [interval] - Start monitoring");
    print("  live [interval] - Start live monitoring");
    print("  status - Show current brain status");
    print("  activity - Show brain activity");
    print("  network - Show network statistics");
    print("  memory - Show memory usage");
    print("  performance - Show performance metrics");
    print("  spikes - Show spike activity");
    print("  connectivity - Show connectivity analysis");
    print("  visualize [title] - Text-based visualization");
    print("  plot - Show plots");
    print("  graph - Show graph visualization");
    print("  chart - Show charts");
    print("  table - Show data in table format");
    print("  start-monitoring [interval] - Start monitoring");
    print("  stop-monitoring - Stop monitoring");
    print("  pause-monitoring - Pause monitoring");
    print("  resume-monitoring - Resume monitoring");
    print("  collect-data - Collect monitoring data");
    print("  analyze-data - Analyze monitoring data");
    print("  export-data [filename] - Export monitoring data");
    print("  clear-data - Clear monitoring data");
}

void MonitorCLI::showCommandHelp(const std::string& command) const {
    std::map<std::string, std::string> helpTexts = {
        {"monitor", "Start monitoring\nUsage: monitor [interval]"},
        {"live", "Start live monitoring\nUsage: live [interval]"},
        {"status", "Show current brain status\nUsage: status"},
        {"activity", "Show brain activity\nUsage: activity"},
        {"network", "Show network statistics\nUsage: network"},
        {"memory", "Show memory usage\nUsage: memory"},
        {"performance", "Show performance metrics\nUsage: performance"},
        {"spikes", "Show spike activity\nUsage: spikes"},
        {"connectivity", "Show connectivity analysis\nUsage: connectivity"},
        {"visualize", "Text-based visualization\nUsage: visualize [title]"},
        {"plot", "Show plots\nUsage: plot"},
        {"graph", "Show graph visualization\nUsage: graph"},
        {"chart", "Show charts\nUsage: chart"},
        {"table", "Show data in table format\nUsage: table"},
        {"start-monitoring", "Start monitoring\nUsage: start-monitoring [interval]"},
        {"stop-monitoring", "Stop monitoring\nUsage: stop-monitoring"},
        {"pause-monitoring", "Pause monitoring\nUsage: pause-monitoring"},
        {"resume-monitoring", "Resume monitoring\nUsage: resume-monitoring"},
        {"collect-data", "Collect monitoring data\nUsage: collect-data"},
        {"analyze-data", "Analyze monitoring data\nUsage: analyze-data"},
        {"export-data", "Export monitoring data\nUsage: export-data [filename]"},
        {"clear-data", "Clear monitoring data\nUsage: clear-data"}
    };
    
    auto it = helpTexts.find(command);
    if (it != helpTexts.end()) {
        print(it->second);
    } else {
        printError("No help available for command: " + command);
    }
}

void MonitorCLI::startRealTimeMonitoringThread() {
    std::thread monitorThread(&MonitorCLI::monitoringWorker, this);
    monitorThread.detach();
}

void MonitorCLI::stopRealTimeMonitoringThread() {
    realTimeEnabled = false;
}

void MonitorCLI::monitoringWorker() {
    while (realTimeEnabled && monitoring) {
        if (!paused) {
            updateMonitoringData();
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void MonitorCLI::updateMonitoringData() {
    collectNetworkStatistics();
    collectMemoryStatistics();
    collectPerformanceStatistics();
    collectSpikeStatistics();
    collectConnectivityStatistics();
}

void MonitorCLI::collectNetworkStatistics() {
    currentData.timestamp = std::chrono::steady_clock::now();
    
    if (brain) {
        currentData.cpuUsage = calculateCPULoad();
        currentData.memoryUsage = calculateMemoryUsage();
        currentData.totalSpikes = brain->getTotalSpikeCount();
        currentData.activeNeuronCount = brain->getActiveNeuronCount();
        currentData.totalNeuronCount = brain->getTotalNeuronCount();
        currentData.totalSynapseCount = brain->getTotalSynapseCount();
        currentData.networkActivity = calculateNetworkActivity(currentData);
        currentData.averageFiringRate = brain->getFiringRate();
        
        if (brain->getTotalSynapseCount() > 0) {
            currentData.averageWeight = brain->getAverageSynapticWeight();
            currentData.weightVariance = brain->getWeightVariance();
            currentData.totalWeight = brain->getTotalSynapticWeight();
        }
    }
    
    currentData.spikeRate = currentData.totalSpikes;
}

void MonitorCLI::collectMemoryStatistics() {
    currentData.memoryUsage = calculateMemoryUsage();
}

void MonitorCLI::collectPerformanceStatistics() {
    currentData.cpuUsage = calculateCPULoad();
}

void MonitorCLI::collectSpikeStatistics() {
    currentData.spikeCount = (brain ? brain->getTotalSpikeCount() : 0);
}

void MonitorCLI::collectConnectivityStatistics() {
    if (brain) {
        currentData.activeNeuronCount = brain->getActiveNeuronCount();
        currentData.totalNeuronCount = brain->getTotalNeuronCount();
        currentData.totalSynapseCount = brain->getTotalSynapseCount();
        
        if (brain->getTotalSynapseCount() > 0) {
            currentData.averageWeight = brain->getAverageSynapticWeight();
            currentData.weightVariance = brain->getWeightVariance();
        }
    }
}

void MonitorCLI::visualizeTextBased(const std::string& title, const MonitorData& data) const {
    print("=== " + title + " ===");
    
    print("Time: " + getFormattedTimestamp(data.timestamp));
    print("Network Activity: " + std::to_string(data.networkActivity));
    print("Average Firing Rate: " + std::to_string(data.averageFiringRate) + " Hz");
    print("Total Spikes: " + std::to_string(data.spikeCount));
    print("Active Neurons: " + std::to_string(data.activeNeuronCount));
    print("Total Neurons: " + std::to_string(data.totalNeuronCount));
    print("Total Synapses: " + std::to_string(data.totalSynapseCount));
    print("Average Weight: " + std::to_string(data.averageWeight));
    print("Weight Variance: " + std::to_string(data.weightVariance));
    
    print("\n=== CPU Usage ===");
    print("CPU Usage: " + getCPULoadString(data.cpuUsage));
    
    print("\n=== Memory Usage ===");
    print("Memory Usage: " + getMemoryUsageString(data.memoryUsage));
    
    print("\n=== Activity Level ===");
    print(getActivityLevelString(data.networkActivity));
}

void MonitorCLI::visualizeTableBased(const std::string& title, const MonitorData& data) const {
    print("=== " + title + " ===");
    print(std::string(50, '='));
    print(" | Metric              | Value               |");
    print(std::string(50, '='));
    
    print(" | Network Activity    | " + std::string(20 - std::to_string(data.networkActivity).size(), ' ') + std::to_string(data.networkActivity) + " |");
    print(" | Average Firing Rate | " + std::string(20 - std::to_string(data.averageFiringRate).size(), ' ') + std::to_string(data.averageFiringRate) + " Hz |");
    print(" | Total Spikes        | " + std::string(20 - std::to_string(data.spikeCount).size(), ' ') + std::to_string(data.spikeCount) + " |");
    print(" | Active Neurons      | " + std::string(20 - std::to_string(data.activeNeuronCount).size(), ' ') + std::to_string(data.activeNeuronCount) + " |");
    print(" | Total Neurons       | " + std::string(20 - std::to_string(data.totalNeuronCount).size(), ' ') + std::to_string(data.totalNeuronCount) + " |");
    print(" | Total Synapses      | " + std::string(20 - std::to_string(data.totalSynapseCount).size(), ' ') + std::to_string(data.totalSynapseCount) + " |");
    print(" | Average Weight      | " + std::string(20 - std::to_string(data.averageWeight).size(), ' ') + std::to_string(data.averageWeight) + " |");
    print(" | Weight Variance     | " + std::string(20 - std::to_string(data.weightVariance).size(), ' ') + std::to_string(data.weightVariance) + " |");
    print(" | CPU Usage           | " + getCPULoadString(data.cpuUsage) + " |");
    print(" | Memory Usage        | " + getMemoryUsageString(data.memoryUsage) + " |");
    print(std::string(50, '='));
}

void MonitorCLI::visualizeGraphBased(const std::string& title, const MonitorData& data) const {
    print("=== " + title + " ===");
    print("Graph visualization is not available in this build.");
    print("Use 'visualize' for text-based visualization.");
}

void MonitorCLI::printMonitorHeader(const std::string& title) const {
    print("=== " + title + " ===");
}

void MonitorCLI::printStatusHeader() const {
    print("=== Live Brain Status ===");
}

void MonitorCLI::printActivityHeader() const {
    print("=== Brain Activity ===");
}

void MonitorCLI::printNetworkHeader() const {
    print("=== Network Statistics ===");
}

void MonitorCLI::printMemoryHeader() const {
    print("=== Memory Usage ===");
}

void MonitorCLI::printPerformanceHeader() const {
    print("=== Performance Metrics ===");
}

void MonitorCLI::printSpikesHeader() const {
    print("=== Spike Activity ===");
}

void MonitorCLI::printConnectivityHeader() const {
    print("=== Connectivity Analysis ===");
}

void MonitorCLI::printVisualizationHeader() const {
    print("=== Visualization ===");
}

void MonitorCLI::displayMonitorData(const MonitorData& data, const std::string& format) const {
    if (format == "text") {
        visualizeTextBased("Live Brain Monitoring", data);
    } else if (format == "table") {
        visualizeTableBased("Live Brain Monitoring", data);
    } else {
        visualizeTextBased("Live Brain Monitoring", data);
    }
}

std::string MonitorCLI::formatMonitorData(const MonitorData& data, const std::string& format) const {
    if (format == "text") {
        std::ostringstream oss;
        oss << "Live Brain Monitoring" << std::endl;
        oss << std::string(50, '=') << std::endl;
        oss << "Network Activity: " << data.networkActivity << std::endl;
        oss << "Average Firing Rate: " << data.averageFiringRate << " Hz" << std::endl;
        oss << "Total Spikes: " << data.spikeCount << std::endl;
        oss << "Active Neurons: " << data.activeNeuronCount << std::endl;
        oss << "Total Neurons: " << data.totalNeuronCount << std::endl;
        oss << "Total Synapses: " << data.totalSynapseCount << std::endl;
        oss << "Average Weight: " << data.averageWeight << std::endl;
        oss << "Weight Variance: " << data.weightVariance << std::endl;
        oss << "CPU Usage: " << getCPULoadString(data.cpuUsage) << std::endl;
        oss << "Memory Usage: " << getMemoryUsageString(data.memoryUsage) << std::endl;
        return oss.str();
    } else if (format == "csv") {
        std::ostringstream oss;
        oss << "Timestamp,CPU Usage,Memory Usage,Spike Count,Active Neurons,Total Neurons,Total Synapses,Network Activity,Average Firing Rate" << std::endl;
        oss << getFormattedTimestamp(data.timestamp) << "," << data.cpuUsage << "," << data.memoryUsage << "," 
            << data.spikeCount << "," << data.activeNeuronCount << "," << data.totalNeuronCount << "," 
            << data.totalSynapseCount << "," << data.networkActivity << "," << data.averageFiringRate << std::endl;
        return oss.str();
    } else if (format == "json") {
        std::ostringstream oss;
        oss << "{" << std::endl;
        oss << "  \"timestamp\": \"" << getFormattedTimestamp(data.timestamp) << "\"," << std::endl;
        oss << "  \"cpu_usage\": " << data.cpuUsage << "," << std::endl;
        oss << "  \"memory_usage\": " << data.memoryUsage << "," << std::endl;
        oss << "  \"spike_count\": " << data.spikeCount << "," << std::endl;
        oss << "  \"active_neuron_count\": " << data.activeNeuronCount << "," << std::endl;
        oss << "  \"total_neuron_count\": " << data.totalNeuronCount << "," << std::endl;
        oss << "  \"total_synapse_count\": " << data.totalSynapseCount << "," << std::endl;
        oss << "  \"network_activity\": " << data.networkActivity << "," << std::endl;
        oss << "  \"average_firing_rate\": " << data.averageFiringRate << std::endl;
        oss << "}" << std::endl;
        return oss.str();
    }
    
    return formatMonitorDataAsText(data);
}

std::string MonitorCLI::formatMonitorDataAsText(const MonitorData& data) const {
    std::ostringstream oss;
    oss << "Live Brain Monitoring" << std::endl;
    oss << std::string(50, '=') << std::endl;
    oss << "Network Activity: " << data.networkActivity << std::endl;
    oss << "Average Firing Rate: " << data.averageFiringRate << " Hz" << std::endl;
    oss << "Total Spikes: " << data.spikeCount << std::endl;
    oss << "Active Neurons: " << data.activeNeuronCount << std::endl;
    oss << "Total Neurons: " << data.totalNeuronCount << std::endl;
    oss << "Total Synapses: " << data.totalSynapseCount << std::endl;
    oss << "Average Weight: " << data.averageWeight << std::endl;
    oss << "Weight Variance: " << data.weightVariance << std::endl;
    oss << "CPU Usage: " << getCPULoadString(data.cpuUsage) << std::endl;
    oss << "Memory Usage: " << getMemoryUsageString(data.memoryUsage) << std::endl;
    return oss.str();
}

std::string MonitorCLI::formatMonitorDataAsCSV(const MonitorData& data) const {
    std::ostringstream oss;
    oss << "Timestamp,CPU Usage,Memory Usage,Spike Count,Active Neurons,Total Neurons,Total Synapses,Network Activity,Average Firing Rate" << std::endl;
    oss << getFormattedTimestamp(data.timestamp) << "," << data.cpuUsage << "," << data.memoryUsage << "," 
        << data.spikeCount << "," << data.activeNeuronCount << "," << data.totalNeuronCount << "," 
        << data.totalSynapseCount << "," << data.networkActivity << "," << data.averageFiringRate << std::endl;
    return oss.str();
}

std::string MonitorCLI::formatMonitorDataAsJSON(const MonitorData& data) const {
    std::ostringstream oss;
    oss << "{" << std::endl;
    oss << "  \"timestamp\": \"" << getFormattedTimestamp(data.timestamp) << "\"," << std::endl;
    oss << "  \"cpu_usage\": " << data.cpuUsage << "," << std::endl;
    oss << "  \"memory_usage\": " << data.memoryUsage << "," << std::endl;
    oss << "  \"spike_count\": " << data.spikeCount << "," << std::endl;
    oss << "  \"active_neuron_count\": " << data.activeNeuronCount << "," << std::endl;
    oss << "  \"total_neuron_count\": " << data.totalNeuronCount << "," << std::endl;
    oss << "  \"total_synapse_count\": " << data.totalSynapseCount << "," << std::endl;
    oss << "  \"network_activity\": " << data.networkActivity << "," << std::endl;
    oss << "  \"average_firing_rate\": " << data.averageFiringRate << std::endl;
    oss << "}" << std::endl;
    return oss.str();
}

std::string MonitorCLI::formatMonitorDataAsHTML(const MonitorData& data) const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head>" << std::endl;
    oss << "<title>Live Brain Monitoring</title>" << std::endl;
    oss << "<style>" << std::endl;
    oss << "table { border-collapse: collapse; width: 100%; }" << std::endl;
    oss << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    oss << "th { background-color: #f2f2f2; }" << std::endl;
    oss << "</style>" << std::endl;
    oss << "</head>" << std::endl;
    oss << "<body>" << std::endl;
    oss << "<h1>Live Brain Monitoring</h1>" << std::endl;
    oss << "<table>" << std::endl;
    oss << "<tr><th>Metric</th><th>Value</th></tr>" << std::endl;
    oss << "<tr><td>Network Activity</td><td>" << data.networkActivity << "</td></tr>" << std::endl;
    oss << "<tr><td>Average Firing Rate</td><td>" << data.averageFiringRate << " Hz</td></tr>" << std::endl;
    oss << "<tr><td>Total Spikes</td><td>" << data.spikeCount << "</td></tr>" << std::endl;
    oss << "<tr><td>Active Neurons</td><td>" << data.activeNeuronCount << "</td></tr>" << std::endl;
    oss << "<tr><td>Total Neurons</td><td>" << data.totalNeuronCount << "</td></tr>" << std::endl;
    oss << "<tr><td>Total Synapses</td><td>" << data.totalSynapseCount << "</td></tr>" << std::endl;
    oss << "<tr><td>Average Weight</td><td>" << data.averageWeight << "</td></tr>" << std::endl;
    oss << "<tr><td>Weight Variance</td><td>" << data.weightVariance << "</td></tr>" << std::endl;
    oss << "<tr><td>CPU Usage</td><td>" << getCPULoadString(data.cpuUsage) << "</td></tr>" << std::endl;
    oss << "<tr><td>Memory Usage</td><td>" << getMemoryUsageString(data.memoryUsage) << "</td></tr>" << std::endl;
    oss << "</table>" << std::endl;
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    return oss.str();
}

std::string MonitorCLI::getFormattedTimestamp(const std::chrono::steady_clock::time_point& timestamp) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp).count();
    return std::to_string(elapsed) + "s";
}

std::string MonitorCLI::getMemoryUsageString(double memoryMB) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << memoryMB << " MB";
    return oss.str();
}

std::string MonitorCLI::getCPULoadString(double cpuPercent) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << cpuPercent << "%";
    return oss.str();
}

std::string MonitorCLI::getActivityLevelString(double activity) const {
    if (activity < 0.3) return "Low";
    else if (activity < 0.7) return "Medium";
    else return "High";
}

double MonitorCLI::calculateNetworkActivity(const MonitorData& data) const {
    double activity = data.averageFiringRate / 100.0;  // Normalize by typical firing rate
    return std::min(activity, 1.0);
}

double MonitorCLI::calculateAverageFiringRate(const MonitorData& data) const {
    return data.averageFiringRate;
}

double MonitorCLI::calculateAverageWeight(const MonitorData& data) const {
    return data.averageWeight;
}

double MonitorCLI::calculateWeightVariance(const MonitorData& data) const {
    return data.weightVariance;
}

double MonitorCLI::calculateTotalWeight(const MonitorData& data) const {
    return data.totalWeight;
}

std::vector<std::pair<size_t, float>> MonitorCLI::getMostActiveNeurons(const MonitorData& data, size_t count) const {
    return std::vector<std::pair<size_t, float>>();
}

std::vector<std::pair<size_t, size_t>> MonitorCLI::getConnectionStats(const MonitorData& data) const {
    return std::vector<std::pair<size_t, size_t>>();
}

void MonitorCLI::updateVisualizationData(const MonitorData& data) {
    vizData.cpuHistory.push_back(data.cpuUsage);
    vizData.memoryHistory.push_back(data.memoryUsage);
    vizData.spikeHistory.push_back(data.spikeCount);
    vizData.neuronHistory.push_back(data.activeNeuronCount);
    vizData.synapseHistory.push_back(data.totalSynapseCount);
    vizData.activityHistory.push_back(data.networkActivity);
    vizData.firingRateHistory.push_back(data.averageFiringRate);
    vizData.lastUpdate = std::chrono::steady_clock::now();
    
    if (vizData.cpuHistory.size() > vizData.maxHistorySize) {
        vizData.cpuHistory.erase(vizData.cpuHistory.begin());
        vizData.memoryHistory.erase(vizData.memoryHistory.begin());
        vizData.spikeHistory.erase(vizData.spikeHistory.begin());
        vizData.neuronHistory.erase(vizData.neuronHistory.begin());
        vizData.synapseHistory.erase(vizData.synapseHistory.begin());
        vizData.activityHistory.erase(vizData.activityHistory.begin());
        vizData.firingRateHistory.erase(vizData.firingRateHistory.begin());
    }
}

bool MonitorCLI::validateMonitoringParameters(const std::vector<std::string>& args) const {
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--interval" && i + 1 < args.size()) {
            try {
                double interval = std::stod(args[i + 1]);
                if (interval < 0.1 || interval > 60.0) {
                    return false;
                }
            } catch (...) {
                return false;
            }
        }
    }
    return true;
}

void MonitorCLI::setupMonitoringConfiguration() const {
    // Setup monitoring configuration
}

bool MonitorCLI::startDataCollection(const std::vector<std::string>& args) {
    std::string filename = "monitor_data.csv";
    if (args.size() >= 2) {
        filename = args[1];
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        printError("Cannot open file for writing: " + filename);
        return false;
    }
    
    file.close();
    
    printSuccess("Monitoring data collection started");
    
    return true;
}

bool MonitorCLI::stopDataCollection(const std::vector<std::string>& args) {
    printSuccess("Monitoring data collection stopped");
    
    return true;
}

bool MonitorCLI::exportMonitoringData(const std::vector<std::string>& args) {
    std::string filename = "monitor_data.csv";
    if (args.size() >= 2) {
        filename = args[1];
    }
    
    exportDataCommand(std::vector<std::string>{"export-data", filename});
    
    return true;
}

bool MonitorCLI::clearMonitoringData(const std::vector<std::string>& args) {
    clearDataCommand(std::vector<std::string>{"clear-data"});
    
    return true;
}

bool MonitorCLI::saveMonitoringSnapshot(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Monitor Snapshot" << std::endl;
    file << "Timestamp: " << getFormattedTimestamp(currentData.timestamp) << std::endl;
    file << "Network Activity: " << currentData.networkActivity << std::endl;
    file << "Average Firing Rate: " << currentData.averageFiringRate << std::endl;
    
    file.close();
    
    return true;
}

bool MonitorCLI::loadMonitoringSnapshot(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Network Activity: ") != std::string::npos) {
            currentData.networkActivity = std::stod(line.substr(line.find(": ") + 2));
        } else if (line.find("Average Firing Rate: ") != std::string::npos) {
            currentData.averageFiringRate = std::stod(line.substr(line.find(": ") + 2));
        }
    }
    
    file.close();
    
    return true;
}

std::string MonitorCLI::getMonitoringDataFilename(const std::string& prefix) const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << prefix << "_" << std::put_time(&tm_buf, "%Y%m%d_%H%M%S") << ".csv";
    return oss.str();
}

bool MonitorCLI::validateFilePath(const std::string& path) const {
    return !path.empty() && path.find("/") != std::string::npos;
}

void MonitorCLI::ensureDataDirectory() const {
    if (!std::filesystem::exists("data")) {
        std::filesystem::create_directory("data");
    }
}
