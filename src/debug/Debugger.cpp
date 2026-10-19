// Include the header
#include "Debugger.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <mutex>

namespace nlm {

// Global debugging instance
std::unique_ptr<Debugger> globalDebugger;

struct Debugger::Impl {
    std::shared_ptr<Config> config;
    DebugLevel debugLevel;
    std::string currentSessionId;
    std::vector<DebugContext> sessions;
    std::vector<std::string> eventLog;
    MemoryProfile memoryProfile;
    std::mutex mutex;
    
    // Performance tracking
    struct PerformanceMetric {
        std::string name;
        std::vector<double> values;
        double sum;
        double min;
        double max;
        std::chrono::system_clock::time_point timestamp;
        
        PerformanceMetric(const std::string& n) : name(n), sum(0), min(0), max(0) {}
    };
    
    std::unordered_map<std::string, PerformanceMetric> metrics;
    std::chrono::system_clock::time_point lastMetricUpdate;
};

Debugger::Debugger(std::shared_ptr<Config> config) : config_(config), currentLevel_(DebugLevel::Info) {
    pImpl_ = std::make_unique<Impl>();
    pImpl_->config = config;
    pImpl_->debugLevel = currentLevel_;
    
    // Apply debug configuration from settings
    applyDebugConfiguration("debug.");
    
    NLM_LOG_INFO("Debugger initialized with level: " + std::to_string(static_cast<int>(currentLevel_)));
}

Debugger::~Debugger() {
    NLM_LOG_INFO("Debugger shutting down");
}

std::string Debugger::startDebugSession(DebugLevel level, const std::string& description) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    std::string sessionId = "session_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    pImpl_->sessions.emplace_back(sessionId, level, description);
    pImpl_->currentSessionId = sessionId;
    
    std::stringstream ss;
    ss << "Debug session started: " << sessionId << " - " << description
       << " (Level: " << static_cast<int>(level) << ")";
    logDebugMessage(DebugLevel::Info, ss.str());
    
    return sessionId;
}

void Debugger::endDebugSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (auto& session : pImpl_->sessions) {
        if (session.sessionId == sessionId) {
            auto duration = std::chrono::system_clock::now() - session.startTime;
            auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            logDebugMessage(DebugLevel::Info, "Debug session ended: " + sessionId + " (Duration: " + std::to_string(durationMs) + " ms)");
            session.sessionId.clear(); // Mark as ended
            break;
        }
    }
}

void Debugger::logDebugMessage(DebugLevel level, const std::string& message) const {
    if (shouldLog(level)) {
        std::string levelStr;
        switch (level) {
            case DebugLevel::Error: levelStr = "[ERROR]"; break;
            case DebugLevel::Warning: levelStr = "[WARNING]"; break;
            case DebugLevel::Info: levelStr = "[INFO]"; break;
            case DebugLevel::Debug: levelStr = "[DEBUG]"; break;
            case DebugLevel::Verbose: levelStr = "[VERBOSE]"; break;
            case DebugLevel::Trace: levelStr = "[TRACE]"; break;
            default: levelStr = "[" + std::to_string(static_cast<int>(level)) + "]"; break;
        }
        
        NLM_LOG_INFO(levelStr + " " + message);
        
        if (pImpl_->currentSessionId != "") {
            pImpl_->eventLog.push_back("DEBUG: " + message);
        }
    }
}

bool Debugger::shouldLog(DebugLevel level) const {
    return currentLevel_ <= level;
}

std::vector<NeuronDebugInfo> Debugger::getAllNeuronDebugInfo() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    std::vector<NeuronDebugInfo> debugInfo;
    
    if (!pImpl_->config) return debugInfo;
    
    // Get brain from config (simplified - in real implementation, would need brain reference)
    // This is a placeholder implementation
    debugInfo.reserve(1000); // Typical large network size
    
    // Create mock neuron debug info for demonstration
    for (uint64_t i = 0; i < 10; ++i) {
        NeuronDebugInfo info;
        info.id.value = i;
        info.type = NeuronType::Excitatory;
        info.state = FiringState::Resting;
        info.membranePotential = -70.0f;
        info.restingPotential = -70.0f;
        info.threshold = -50.0f;
        info.resetPotential = -75.0f;
        info.leakConductance = 0.1f;
        info.firingRate = 10.0f;
        info.refractoryRemaining = 0;
        info.refractoryPeriod = 2;
        info.adaptationVariable = 1.0f;
        info.lastSpikeTime = 0.0f;
        info.regionId = 0;
        info.populationId = 0;
        info.lastPreSpike = 0;
        info.lastPostSpike = 0;
        info.eligibilityTrace = 0.0f;
        info.efficacy = 1.0f;
        
        debugInfo.push_back(info);
    }
    
    return debugInfo;
}

std::vector<SynapseDebugInfo> Debugger::getAllSynapseDebugInfo() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    std::vector<SynapseDebugInfo> debugInfo;
    
    // Create mock synapse debug info for demonstration
    for (uint64_t i = 0; i < 50; ++i) {
        SynapseDebugInfo info;
        info.id.value = i;
        info.sourceNeuron.value = i % 10;
        info.destinationNeuron.value = (i + 1) % 10;
        info.weight = 0.5f + static_cast<float>(rand()) / RAND_MAX * 0.5f;
        info.delay = 1 + (i % 10);
        info.type = SynapseType::Excitatory;
        info.plasticityFlags = PlasticityFlags::All;
        info.shortTermDepression = 0.0f;
        info.shortTermFacilitation = 0.0f;
        info.lastPreSpikeTime = 0.0f;
        info.lastPostSpikeTime = 0.0f;
        info.eligibilityTrace = 0.0f;
        info.efficacy = info.weight;
        info.regionId = 0;
        info.populationId = 0;
        
        debugInfo.push_back(info);
    }
    
    return debugInfo;
}

std::vector<NeuronDebugInfo> Debugger::getNeuronDebugInfo(const std::vector<NeuronId>& ids) const {
    std::vector<NeuronDebugInfo> result;
    // Filter from all neurons based on provided IDs
    auto allNeurons = getAllNeuronDebugInfo();
    for (const auto& neuronId : ids) {
        for (const auto& neuron : allNeurons) {
            if (neuron.id.value == neuronId.value) {
                result.push_back(neuron);
                break;
            }
        }
    }
    return result;
}

std::vector<SynapseDebugInfo> Debugger::getSynapseDebugInfo(const std::vector<SynapseId>& ids) const {
    std::vector<SynapseDebugInfo> result;
    // Filter from all synapses based on provided IDs
    auto allSynapses = getAllSynapseDebugInfo();
    for (const auto& synapseId : ids) {
        for (const auto& synapse : allSynapses) {
            if (synapse.id.value == synapseId.value) {
                result.push_back(synapse);
                break;
            }
        }
    }
    return result;
}

std::vector<NeuronDebugInfo> Debugger::getActiveNeurons() const {
    std::vector<NeuronDebugInfo> result;
    auto allNeurons = getAllNeuronDebugInfo();
    
    // Filter neurons that are firing or recently active
    for (const auto& neuron : allNeurons) {
        if (neuron.state == FiringState::Active || neuron.firingRate > 5.0f) {
            result.push_back(neuron);
        }
    }
    
    return result;
}

std::vector<SynapseDebugInfo> Debugger::getHighActivitySynapses(float threshold) const {
    std::vector<SynapseDebugInfo> result;
    auto allSynapses = getAllSynapseDebugInfo();
    
    // Filter synapses with high efficacy or activity
    for (const auto& synapse : allSynapses) {
        if (synapse.efficacy > threshold || synapse.eligibilityTrace > threshold) {
            result.push_back(synapse);
        }
    }
    
    return result;
}

std::unordered_map<RegionId, size_t> Debugger::getRegionActivity() const {
    std::unordered_map<RegionId, size_t> result;
    auto allNeurons = getAllNeuronDebugInfo();
    
    for (const auto& neuron : allNeurons) {
        RegionId regionId;
        regionId.value = neuron.regionId;
        result[regionId]++;
    }
    
    return result;
}

MemoryProfile Debugger::getMemoryProfile() const {
    return pImpl_->memoryProfile;
}

void Debugger::recordMemoryAllocation(size_t size, const std::string& type) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    pImpl_->memoryProfile.recordAllocation(size, type);
}

void Debugger::recordMemoryDeallocation(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    pImpl_->memoryProfile.recordDeallocation(size);
}

void Debugger::logEvent(const std::string& eventName, const std::string& details) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    std::stringstream ss;
    ss << "[" << std::chrono::system_clock::now().time_since_epoch().count()
       << "] " << eventName << ": " << details;
    
    pImpl_->eventLog.push_back(ss.str());
    logDebugMessage(DebugLevel::Info, "Event: " + eventName + (details.empty() ? "" : " - " + details));
}

std::vector<std::string> Debugger::getEventTimeline() const {
    return pImpl_->eventLog;
}

std::vector<std::string> Debugger::validateConfiguration() const {
    std::vector<std::string> issues;
    
    if (!config_) {
        issues.push_back("No configuration available");
        return issues;
    }
    
    // Check for common configuration issues
    if (config_->getOr<int64_t>("brain.neuron_count", 0) <= 0) {
        issues.push_back("Invalid neuron count in configuration");
    }
    
    if (config_->getOr<float>("brain.connection_probability", 0.0f) < 0.0f || 
        config_->getOr<float>("brain.connection_probability", 0.0f) > 1.0f) {
        issues.push_back("Invalid connection probability in configuration");
    }
    
    if (config_->getOr<double>("simulation_timestep", 0.0) <= 0.0) {
        issues.push_back("Invalid simulation timestep in configuration");
    }
    
    return issues;
}

std::vector<std::string> Debugger::checkForIssues() const {
    std::vector<std::string> issues;
    
    // Check for basic network issues
    auto allNeurons = getAllNeuronDebugInfo();
    auto allSynapses = getAllSynapseDebugInfo();
    
    if (allNeurons.empty()) {
        issues.push_back("No neurons found in the network");
    }
    
    if (allSynapses.empty()) {
        issues.push_back("No synapses found - network may be disconnected");
    }
    
    // Check for potential memory issues
    if (pImpl_->memoryProfile.peakUsage > 1024 * 1024 * 1024) { // 1GB
        issues.push_back("High memory usage detected: " + 
                        std::to_string(pImpl_->memoryProfile.peakUsage) + " bytes");
    }
    
    return issues;
}

std::string Debugger::getPerformanceReport() const {
    std::stringstream ss;
    ss << "=== NLM Performance Report ===\n";
    ss << "Memory Profile:\n";
    ss << "  Total Allocated: " << pImpl_->memoryProfile.totalAllocated << " bytes\n";
    ss << "  Current Usage: " << pImpl_->memoryProfile.currentUsage << " bytes\n";
    ss << "  Peak Usage: " << pImpl_->memoryProfile.peakUsage << " bytes\n";
    ss << "  Utilization: " << pImpl_->memoryProfile.getMemoryUtilization() * 100 << "%\n";
    ss << "  Allocation Count: " << pImpl_->memoryProfile.allocationCount << "\n";
    
    ss << "\nDebug Sessions:\n";
    for (const auto& session : pImpl_->sessions) {
        if (!session.sessionId.empty()) {
            auto duration = std::chrono::system_clock::now() - session.startTime;
            auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            ss << "  " << session.sessionId << ": " << session.description
               << " (Duration: " << durationMs << " ms)\n";
        }
    }
    
    ss << "\nEvent Log (last " << std::min(pImpl_->eventLog.size(), static_cast<size_t>(10)) << "):\n";
    for (size_t i = std::max(pImpl_->eventLog.size() - 10, size_t(0)); i < pImpl_->eventLog.size(); ++i) {
        ss << "  " << pImpl_->eventLog[i] << "\n";
    }
    
    return ss.str();
}

double Debugger::getAverageFiringRate() const {
    auto neurons = getAllNeuronDebugInfo();
    if (neurons.empty()) return 0.0;
    
    double totalRate = 0.0;
    for (const auto& neuron : neurons) {
        totalRate += neuron.firingRate;
    }
    
    return totalRate / neurons.size();
}

double Debugger::getSynapseDensity() const {
    auto neurons = getAllNeuronDebugInfo();
    auto synapses = getAllSynapseDebugInfo();
    
    if (neurons.empty()) return 0.0;
    
    return static_cast<double>(synapses.size()) / neurons.size();
}

bool Debugger::exportDebugState(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open file for writing debug state: " + filepath);
            return false;
        }
        
        // Create JSON-like output for debug state
        file << "{\n";
        file << "  \"debug_session_count\": " << pImpl_->sessions.size() << ",\n";
        file << "  \"current_session_id\": \"" << pImpl_->currentSessionId << "\",\n";
        file << "  \"memory_profile\": {\n";
        file << "    \"total_allocated\": " << pImpl_->memoryProfile.totalAllocated << ",\n";
        file << "    \"current_usage\": " << pImpl_->memoryProfile.currentUsage << ",\n";
        file << "    \"peak_usage\": " << pImpl_->memoryProfile.peakUsage << ",\n";
        file << "    \"allocation_count\": " << pImpl_->memoryProfile.allocationCount << "\n";
        file << "  },\n";
        file << "  \"neuron_count\": " << getAllNeuronDebugInfo().size() << ",\n";
        file << "  \"synapse_count\": " << getAllSynapseDebugInfo().size() << ",\n";
        file << "  \"average_firing_rate\": " << getAverageFiringRate() << ",\n";
        file << "  \"synapse_density\": " << getSynapseDensity() << "\n";
        file << "}\n";
        
        logDebugMessage(DebugLevel::Info, "Debug state exported to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting debug state: " + std::string(e.what()));
        return false;
    }
}

bool Debugger::importDebugState(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open file for reading debug state: " + filepath);
            return false;
        }
        
        // For now, just read the file and validate
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (content.empty()) {
            NLM_LOG_ERROR("Empty debug state file: " + filepath);
            return false;
        }
        
        logDebugMessage(DebugLevel::Info, "Debug state imported from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error importing debug state: " + std::string(e.what()));
        return false;
    }
}

void Debugger::printNeuronStats() const {
    auto neurons = getAllNeuronDebugInfo();
    NLM_LOG_INFO("=== Neuron Statistics ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(neurons.size()));
    
    if (neurons.empty()) return;
    
    double totalFiringRate = 0.0;
    size_t maxRefractory = 0;
    for (const auto& neuron : neurons) {
        totalFiringRate += neuron.firingRate;
        maxRefractory = std::max(maxRefractory, neuron.refractoryRemaining);
    }
    
    NLM_LOG_INFO("Average firing rate: " + std::to_string(totalFiringRate / neurons.size()));
    NLM_LOG_INFO("Max refractory remaining: " + std::to_string(maxRefractory));
    
    // Count by type
    std::unordered_map<NeuronType, size_t> typeCount;
    for (const auto& neuron : neurons) {
        typeCount[neuron.type]++;
    }
    
    for (const auto& pair : typeCount) {
        NLM_LOG_INFO("  " + std::to_string(static_cast<int>(pair.first)) + ": " + std::to_string(pair.second));
    }
}

void Debugger::printSynapseStats() const {
    auto synapses = getAllSynapseDebugInfo();
    NLM_LOG_INFO("=== Synapse Statistics ===");
    NLM_LOG_INFO("Total synapses: " + std::to_string(synapses.size()));
    
    if (synapses.empty()) return;
    
    double totalWeight = 0.0;
    double avgDelay = 0.0;
    for (const auto& synapse : synapses) {
        totalWeight += synapse.weight;
        avgDelay += synapse.delay;
    }
    
    NLM_LOG_INFO("Average weight: " + std::to_string(totalWeight / synapses.size()));
    NLM_LOG_INFO("Average delay: " + std::to_string(avgDelay / synapses.size()));
    
    // Count by type
    std::unordered_map<SynapseType, size_t> typeCount;
    for (const auto& synapse : synapses) {
        typeCount[synapse.type]++;
    }
    
    for (const auto& pair : typeCount) {
        NLM_LOG_INFO("  " + std::to_string(static_cast<int>(pair.first)) + ": " + std::to_string(pair.second));
    }
}

void Debugger::printMemoryStats() const {
    const auto& profile = pImpl_->memoryProfile;
    NLM_LOG_INFO("=== Memory Statistics ===");
    NLM_LOG_INFO("Total allocated: " + std::to_string(profile.totalAllocated) + " bytes");
    NLM_LOG_INFO("Current usage: " + std::to_string(profile.currentUsage) + " bytes");
    NLM_LOG_INFO("Peak usage: " + std::to_string(profile.peakUsage) + " bytes");
    NLM_LOG_INFO("Utilization: " + std::to_string(profile.getMemoryUtilization() * 100) + "%");
    NLM_LOG_INFO("Allocation count: " + std::to_string(profile.allocationCount));
    
    if (!profile.allocationsByType.empty()) {
        NLM_LOG_INFO("Allocations by type:");
        for (const auto& pair : profile.allocationsByType) {
            NLM_LOG_INFO("  " + pair.first + ": " + std::to_string(pair.second));
        }
    }
}

void Debugger::printPerformanceStats() const {
    NLM_LOG_INFO("=== Performance Statistics ===");
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("Synapse density: " + std::to_string(getSynapseDensity()));
    NLM_LOG_INFO("Debug level: " + std::to_string(static_cast<int>(currentLevel_)));
    NLM_LOG_INFO("Active debug sessions: " + std::to_string(pImpl_->sessions.size()));
}

void Debugger::printAllStats() {
    printNeuronStats();
    printSynapseStats();
    printMemoryStats();
    printPerformanceStats();
    
    NLM_LOG_INFO("\n=== Full Debug Report ===");
    NLM_LOG_INFO(getPerformanceReport());
}

void Debugger::applyDebugConfiguration(const std::string& configKeyPrefix) {
    if (!config_) return;
    
    // Apply debug-related configuration settings
    std::string debugLevelKey = configKeyPrefix + "debug.level";
    if (config_->has(debugLevelKey)) {
        int level = config_->getOr<int>(debugLevelKey, 1);
        currentLevel_ = static_cast<DebugLevel>(std::max(0, std::min(level, 6)));
        NLM_LOG_INFO("Debug level set to: " + std::to_string(static_cast<int>(currentLevel_)));
    }
    
    // Enable/disable debug features
    std::string enableNeuronsKey = configKeyPrefix + "debug.neurons.enable";
    if (config_->has(enableNeuronsKey) && config_->getOr<bool>(enableNeuronsKey, false)) {
        NLM_LOG_INFO("Per-neuron debugging enabled");
    }
    
    std::string enableSynapsesKey = configKeyPrefix + "debug.synapses.enable";
    if (config_->has(enableSynapsesKey) && config_->getOr<bool>(enableSynapsesKey, false)) {
        NLM_LOG_INFO("Per-synapse debugging enabled");
    }
    
    std::string enableMemoryKey = configKeyPrefix + "debug.memory.enable";
    if (config_->has(enableMemoryKey) && config_->getOr<bool>(enableMemoryKey, false)) {
        NLM_LOG_INFO("Memory debugging enabled");
    }
}

void Debugger::updateDebugLevel(DebugLevel level) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    currentLevel_ = level;
    pImpl_->debugLevel = level;
    
    logDebugMessage(DebugLevel::Info, "Debug level updated to: " + std::to_string(static_cast<int>(level)));
}

void Debugger::debugNeuronConnections(NeuronId neuronId) const {
    logDebugMessage(DebugLevel::Info, "Debugging connections for neuron " + std::to_string(neuronId.value));
    
    auto neurons = getNeuronDebugInfo({neuronId});
    if (neurons.empty()) {
        logDebugMessage(DebugLevel::Warning, "Neuron not found: " + std::to_string(neuronId.value));
        return;
    }
    
    const auto& neuron = neurons[0];
    NLM_LOG_INFO("Neuron " + std::to_string(neuronId.value) +
                 ": V=" + std::to_string(neuron.membranePotential) +
                 ", rate=" + std::to_string(neuron.firingRate) +
                 ", ref=" + std::to_string(neuron.refractoryRemaining));
    
    // In a real implementation, this would show incoming/outgoing connections
    NLM_LOG_INFO("Incoming connections: Would be shown here");
}

void Debugger::debugSynapseDynamics(SynapseId synapseId) const {
    logDebugMessage(DebugLevel::Info, "Debugging dynamics for synapse " + std::to_string(synapseId.value));
    
    auto synapses = getSynapseDebugInfo({synapseId});
    if (synapses.empty()) {
        logDebugMessage(DebugLevel::Warning, "Synapse not found: " + std::to_string(synapseId.value));
        return;
    }
    
    const auto& synapse = synapses[0];
    NLM_LOG_INFO("Synapse " + std::to_string(synapseId.value) +
                 ": src=" + std::to_string(synapse.sourceNeuron.value) +
                 ", dst=" + std::to_string(synapse.destinationNeuron.value) +
                 ", weight=" + std::to_string(synapse.weight) +
                 ", efficacy=" + std::to_string(synapse.efficacy));
}

void Debugger::debugNetworkTopology() const {
    logDebugMessage(DebugLevel::Info, "Analyzing network topology");
    
    auto neurons = getAllNeuronDebugInfo();
    auto synapses = getAllSynapseDebugInfo();
    
    std::unordered_map<RegionId, size_t> regions;
    for (const auto& neuron : neurons) {
        RegionId regionId;
        regionId.value = neuron.regionId;
        regions[regionId]++;
    }
    
    NLM_LOG_INFO("Network topology analysis:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(neurons.size()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(synapses.size()));
    NLM_LOG_INFO("  Regions: " + std::to_string(regions.size()));
    NLM_LOG_INFO("  Average synapses per neuron: " + 
                std::to_string(neurons.empty() ? 0.0 : static_cast<double>(synapses.size()) / neurons.size()));
}

void Debugger::debugMemoryUsage() const {
    logDebugMessage(DebugLevel::Info, "Analyzing memory usage");
    printMemoryStats();
}

// Global debugger instance management
void initializeGlobalDebugger(std::shared_ptr<Config> config) {
    globalDebugger = std::make_unique<Debugger>(config);
}

void shutdownGlobalDebugger() {
    globalDebugger.reset();
}

Debugger& getGlobalDebugger() {
    if (!globalDebugger) {
        throw std::runtime_error("Global debugger not initialized. Call initializeGlobalDebugger() first.");
    }
    return *globalDebugger;
}

} // namespace nlm
