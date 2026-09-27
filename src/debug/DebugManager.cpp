// Debug Manager implementation

#include "debug/DebugManager.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

DebugManager::DebugManager() : eventThreadRunning(false) {
    // Load default configuration
    loadConfiguration();
    
    // Start event processing thread
    eventThreadRunning = true;
    eventThread = std::thread(&DebugManager::eventProcessingThread, this);
}

void DebugManager::logEvent(DebugEventType type, const std::string& source, const std::string& message) {
    logEvent(type, source, message, {}, 0);
}

void DebugManager::logEvent(DebugEventType type, const std::string& source, const std::string& message,
                          const std::map<std::string, std::string>& metadata) {
    logEvent(type, source, message, metadata, 0);
}

void DebugManager::logEvent(DebugEventType type, const std::string& source, const std::string& message,
                          const std::map<std::string, std::string>& metadata, uint64_t step) {
    std::lock_guard<std::mutex> lock(mutex);
    
    DebugEvent event(type, source, message);
    event.metadata = metadata;
    event.step = step;
    
    // Add to queue for processing
    eventQueue.push_back(event);
    eventCv.notify_one();
    
    // Add to history
    eventHistory.push_back(event);
    
    // Cleanup if buffer is full
    if (eventHistory.size() > eventBufferSize) {
        eventHistory.pop_front();
    }
    
    // Process immediately if not in processing thread
    if (std::this_thread::get_id() != eventThread.get_id()) {
        processEvent(event);
    }
}

void DebugManager::recordTrace(const std::string& eventType, const std::string& component, const std::string& details) {
    recordTrace(eventType, component, details, 0, 0);
}

void DebugManager::recordTrace(const std::string& eventType, const std::string& component, const std::string& details,
                             uint64_t step) {
    recordTrace(eventType, component, details, step, 0);
}

void DebugManager::recordTrace(const std::string& eventType, const std::string& component, const std::string& details,
                             uint64_t step, int threadId) {
    std::lock_guard<std::mutex> lock(mutex);
    
    TracePoint trace(eventType, component, details);
    trace.step = step;
    trace.threadId = threadId;
    
    traceHistory.push_back(trace);
    
    if (traceHistory.size() > traceBufferSize) {
        traceHistory.pop_front();
    }
    
    // Notify listeners
    for (const auto& listener : traceListeners) {
        listener(trace);
    }
}

void DebugManager::recordPerformance(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(mutex);
    
    performanceHistory.push_back(metrics);
    
    if (performanceHistory.size() > performanceBufferSize) {
        performanceHistory.pop_front();
    }
    
    // Notify listeners
    for (const auto& listener : performanceListeners) {
        listener(metrics);
    }
}

void DebugManager::recordMemoryStats(const MemoryStats& stats) {
    std::lock_guard<std::mutex> lock(mutex);
    
    memoryHistory.push_back(stats);
    
    if (memoryHistory.size() > memoryStatsBufferSize) {
        memoryHistory.pop_front();
    }
    
    // Notify listeners
    for (const auto& listener : memoryListeners) {
        listener(stats);
    }
}

void DebugManager::recordNeuromodulatorStats(const NeuromodulatorStats& stats) {
    std::lock_guard<std::mutex> lock(mutex);
    
    neuromodulatorHistory.push_back(stats);
    
    if (neuromodulatorHistory.size() > neuromodulatorStatsBufferSize) {
        neuromodulatorHistory.pop_front();
    }
    
    // Notify listeners
    for (const auto& listener : neuromodulatorListeners) {
        listener(stats);
    }
}

void DebugManager::recordPredictionStats(const PredictionStats& stats) {
    std::lock_guard<std::mutex> lock(mutex);
    
    predictionHistory.push_back(stats);
    
    if (predictionHistory.size() > predictionStatsBufferSize) {
        predictionHistory.pop_front();
    }
    
    // Notify listeners
    for (const auto& listener : predictionListeners) {
        listener(stats);
    }
}

void DebugManager::recordNeuralActivity(const NeuralActivity& activity) {
    std::lock_guard<std::mutex> lock(mutex);
    
    neuralActivityHistory.push_back(activity);
    
    if (neuralActivityHistory.size() > neuralActivityBufferSize) {
        neuralActivityHistory.pop_front();
    }
    
    // Notify listeners
    for (const auto& listener : neuralActivityListeners) {
        listener(activity);
    }
}

void DebugManager::addEventListener(const EventListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    eventListeners.push_back(listener);
}

void DebugManager::removeEventListener(const EventListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    eventListeners.erase(std::remove(eventListeners.begin(), eventListeners.end(), listener), eventListeners.end());
}

void DebugManager::addTraceListener(const TraceListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    traceListeners.push_back(listener);
}

void DebugManager::removeTraceListener(const TraceListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    traceListeners.erase(std::remove(traceListeners.begin(), traceListeners.end(), listener), traceListeners.end());
}

void DebugManager::addPerformanceListener(const PerformanceListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    performanceListeners.push_back(listener);
}

void DebugManager::removePerformanceListener(const PerformanceListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    performanceListeners.erase(std::remove(performanceListeners.begin(), performanceListeners.end(), listener), performanceListeners.end());
}

void DebugManager::addMemoryListener(const MemoryListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    memoryListeners.push_back(listener);
}

void DebugManager::removeMemoryListener(const MemoryListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    memoryListeners.erase(std::remove(memoryListeners.begin(), memoryListeners.end(), listener), memoryListeners.end());
}

void DebugManager::addNeuromodulatorListener(const NeuromodulatorListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    neuromodulatorListeners.push_back(listener);
}

void DebugManager::removeNeuromulatorListener(const NeuromodulatorListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    neuromodulatorListeners.erase(std::remove(neuromodulatorListeners.begin(), neuromodulatorListeners.end(), listener), neuromodulatorListeners.end());
}

void DebugManager::addPredictionListener(const PredictionListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    predictionListeners.push_back(listener);
}

void DebugManager::removePredictionListener(const PredictionListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    predictionListeners.erase(std::remove(predictionListeners.begin(), predictionListeners.end(), listener), predictionListeners.end());
}

void DebugManager::addNeuralActivityListener(const NeuralActivityListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    neuralActivityListeners.push_back(listener);
}

void DebugManager::removeNeuralActivityListener(const NeuralActivityListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    neuralActivityListeners.erase(std::remove(neuralActivityListeners.begin(), neuralActivityListeners.end(), listener), neuralActivityListeners.end());
}

void DebugManager::setDebugLevel(int level) {
    std::lock_guard<std::mutex> lock(mutex);
    debugLevel = std::max(0, std::min(level, 3));
}

int DebugManager::getDebugLevel() const {
    std::lock_guard<std::mutex> lock(mutex);
    return debugLevel;
}

void DebugManager::enableTracing(const std::vector<std::string>& targets) {
    std::lock_guard<std::mutex> lock(mutex);
    traceTargets.insert(traceTargets.end(), targets.begin(), targets.end());
}

void DebugManager::disableTracing(const std::vector<std::string>& targets) {
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& target : targets) {
        auto it = std::remove(traceTargets.begin(), traceTargets.end(), target);
        traceTargets.erase(it, traceTargets.end());
    }
}

bool DebugManager::isTracing(const std::string& target) const {
    std::lock_guard<std::mutex> lock(mutex);
    return std::find(traceTargets.begin(), traceTargets.end(), target) != traceTargets.end();
}

void DebugManager::enableProfiling(const std::vector<std::string>& targets) {
    std::lock_guard<std::mutex> lock(mutex);
    profileTargets.insert(profileTargets.end(), targets.begin(), targets.end());
}

void DebugManager::disableProfiling(const std::vector<std::string>& targets) {
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& target : targets) {
        auto it = std::remove(profileTargets.begin(), profileTargets.end(), target);
        profileTargets.erase(it, profileTargets.end());
    }
}

bool DebugManager::isProfiling(const std::string& target) const {
    std::lock_guard<std::mutex> lock(mutex);
    return std::find(profileTargets.begin(), profileTargets.end(), target) != profileTargets.end();
}

void DebugManager::setEventBufferSize(size_t size) {
    std::lock_guard<std::mutex> lock(mutex);
    eventBufferSize = size;
    cleanupBuffers();
}

std::vector<DebugEvent> DebugManager::getRecentEvents(size_t count) {
    std::lock_guard<std::mutex> lock(mutex);
    size_t actualCount = std::min(count, eventHistory.size());
    return std::vector<DebugEvent>(eventHistory.end() - actualCount, eventHistory.end());
}

void DebugManager::clearEventHistory() {
    std::lock_guard<std::mutex> lock(mutex);
    eventHistory.clear();
}

void DebugManager::setTraceBufferSize(size_t size) {
    std::lock_guard<std::mutex> lock(mutex);
    traceBufferSize = size;
    cleanupBuffers();
}

std::vector<TracePoint> DebugManager::getRecentTraces(size_t count) {
    std::lock_guard<std::mutex> lock(mutex);
    size_t actualCount = std::min(count, traceHistory.size());
    return std::vector<TracePoint>(traceHistory.end() - actualCount, traceHistory.end());
}

void DebugManager::clearTraceHistory() {
    std::lock_guard<std::mutex> lock(mutex);
    traceHistory.clear();
}

void DebugManager::setPerformanceBufferSize(size_t size) {
    std::lock_guard<std::mutex> lock(mutex);
    performanceBufferSize = size;
    cleanupBuffers();
}

std::vector<PerformanceMetrics> DebugManager::getRecentPerformanceMetrics(size_t count) {
    std::lock_guard<std::mutex> lock(mutex);
    size_t actualCount = std::min(count, performanceHistory.size());
    return std::vector<PerformanceMetrics>(performanceHistory.end() - actualCount, performanceHistory.end());
}

void DebugManager::clearPerformanceHistory() {
    std::lock_guard<std::mutex> lock(mutex);
    performanceHistory.clear();
}

void DebugManager::setMemoryStatsBufferSize(size_t size) {
    std::lock_guard<std::mutex> lock(mutex);
    memoryStatsBufferSize = size;
    cleanupBuffers();
}

std::vector<MemoryStats> DebugManager::getRecentMemoryStats(size_t count) {
    std::lock_guard<std::mutex> lock(mutex);
    size_t actualCount = std::min(count, memoryHistory.size());
    return std::vector<MemoryStats>(memoryHistory.end() - actualCount, memoryHistory.end());
}

void DebugManager::clearMemoryStatsHistory() {
    std::lock_guard<std::mutex> lock(mutex);
    memoryHistory.clear();
}

void DebugManager::setNeuromodulatorStatsBufferSize(size_t size) {
    std::lock_guard<std::mutex> lock(mutex);
    neuromodulatorStatsBufferSize = size;
    cleanupBuffers();
}

std::vector<NeuromodulatorStats> DebugManager::getRecentNeuromodulatorStats(size_t count) {
    std::lock_guard<std::mutex> lock(mutex);
    size_t actualCount = std::min(count, neuromodulatorHistory.size());
    return std::vector<NeuromodulatorStats>(neuromodulatorHistory.end() - actualCount, neuromodulatorHistory.end());
}

void DebugManager::clearNeuromodulatorStatsHistory() {
    std::lock_guard<std::mutex> lock(mutex);
    neuromodulatorHistory.clear();
}

void DebugManager::setPredictionStatsBufferSize(size_t size) {
    std::lock_guard<std::mutex> lock(mutex);
    predictionStatsBufferSize = size;
    cleanupBuffers();
}

std::vector<PredictionStats> DebugManager::getRecentPredictionStats(size_t count) {
    std::lock_guard<std::mutex> lock(mutex);
    size_t actualCount = std::min(count, predictionHistory.size());
    return std::vector<PredictionStats>(predictionHistory.end() - actualCount, predictionHistory.end());
}

void DebugManager::clearPredictionStatsHistory() {
    std::lock_guard<std::mutex> lock(mutex);
    predictionHistory.clear();
}

void DebugManager::setNeuralActivityBufferSize(size_t size) {
    std::lock_guard<std::mutex> lock(mutex);
    neuralActivityBufferSize = size;
    cleanupBuffers();
}

std::vector<NeuralActivity> DebugManager::getRecentNeuralActivity(size_t count) {
    std::lock_guard<std::mutex> lock(mutex);
    size_t actualCount = std::min(count, neuralActivityHistory.size());
    return std::vector<NeuralActivity>(neuralActivityHistory.end() - actualCount, neuralActivityHistory.end());
}

void DebugManager::clearNeuralActivityHistory() {
    std::lock_guard<std::mutex> lock(mutex);
    neuralActivityHistory.clear();
}

bool DebugManager::saveDebugState(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::string filename = filepath.empty() ? "debug_state.json" : filepath;
    
    try {
        nlohmann::json jsonData;
        
        // Export events
        std::vector<nlohmann::json> eventsJson;
        for (const auto& event : eventHistory) {
            nlohmann::json eventJson;
            eventJson["type"] = static_cast<int>(event.type);
            eventJson["timestamp"] = event.timestamp.time_since_epoch().count();
            eventJson["source"] = event.source;
            eventJson["message"] = event.message;
            eventJson["step"] = event.step;
            eventJson["metadata"] = event.metadata;
            eventsJson.push_back(eventJson);
        }
        jsonData["events"] = eventsJson;
        
        // Export traces
        std::vector<nlohmann::json> tracesJson;
        for (const auto& trace : traceHistory) {
            nlohmann::json traceJson;
            traceJson["eventType"] = trace.eventType;
            traceJson["component"] = trace.component;
            traceJson["details"] = trace.details;
            traceJson["timestamp"] = trace.timestamp.time_since_epoch().count();
            traceJson["step"] = trace.step;
            traceJson["threadId"] = trace.threadId;
            tracesJson.push_back(traceJson);
        }
        jsonData["traces"] = tracesJson;
        
        // Export performance metrics
        std::vector<nlohmann::json> perfMetricsJson;
        for (const auto& metric : performanceHistory) {
            nlohmann::json metricJson;
            metricJson["neuronCount"] = metric.neuronCount;
            metricJson["synapseCount"] = metric.synapseCount;
            metricJson["spikeCount"] = metric.spikeCount;
            metricJson["memoryUsage"] = metric.memoryUsage;
            metricJson["cpuUsage"] = metric.cpuUsage;
            metricJson["stepDuration"] = metric.stepDuration.count();
            metricJson["timestamp"] = metric.timestamp.time_since_epoch().count();
            metricJson["step"] = metric.step;
            perfMetricsJson.push_back(metricJson);
        }
        jsonData["performanceMetrics"] = perfMetricsJson;
        
        // Export memory stats
        std::vector<nlohmann::json> memoryStatsJson;
        for (const auto& stats : memoryHistory) {
            nlohmann::json statsJson;
            statsJson["workingMemorySize"] = stats.workingMemorySize;
            statsJson["workingMemoryUsed"] = stats.workingMemoryUsed;
            statsJson["episodicMemorySize"] = stats.episodicMemorySize;
            statsJson["episodicMemoryUsed"] = stats.episodicMemoryUsed;
            statsJson["semanticMemorySize"] = stats.semanticMemorySize;
            statsJson["semanticMemoryUsed"] = stats.semanticMemoryUsed;
            statsJson["proceduralMemorySize"] = stats.proceduralMemorySize;
            statsJson["proceduralMemoryUsed"] = stats.proceduralMemoryUsed;
            statsJson["associativeMemorySize"] = stats.associativeMemorySize;
            statsJson["associativeMemoryUsed"] = stats.associativeMemoryUsed;
            statsJson["timestamp"] = stats.timestamp.time_since_epoch().count();
            statsJson["step"] = stats.step;
            memoryStatsJson.push_back(statsJson);
        }
        jsonData["memoryStats"] = memoryStatsJson;
        
        // Export neuromodulator stats
        std::vector<nlohmann::json> neuromodulatorStatsJson;
        for (const auto& stats : neuromodulatorHistory) {
            nlohmann::json statsJson;
            statsJson["dopamine"] = stats.dopamine;
            statsJson["curiosity"] = stats.curiosity;
            statsJson["novelty"] = stats.novelty;
            statsJson["acetylcholine"] = stats.acetylcholine;
            statsJson["norepinephrine"] = stats.norepinephrine;
            statsJson["serotonin"] = stats.serotonin;
            statsJson["nicotine"] = stats.nicotine;
            statsJson["orexin"] = stats.orexin;
            statsJson["timestamp"] = stats.timestamp.time_since_epoch().count();
            statsJson["step"] = stats.step;
            neuromodulatorStatsJson.push_back(statsJson);
        }
        jsonData["neuromodulatorStats"] = neuromodulatorStatsJson;
        
        // Export prediction stats
        std::vector<nlohmann::json> predictionStatsJson;
        for (const auto& stats : predictionHistory) {
            nlohmann::json statsJson;
            statsJson["predictionError"] = stats.predictionError;
            statsJson["predictionAccuracy"] = stats.predictionAccuracy;
            statsJson["confidence"] = stats.confidence;
            statsJson["learningRate"] = stats.learningRate;
            statsJson["totalPredictions"] = stats.totalPredictions;
            statsJson["correctPredictions"] = stats.correctPredictions;
            statsJson["timestamp"] = stats.timestamp.time_since_epoch().count();
            statsJson["step"] = stats.step;
            predictionStatsJson.push_back(statsJson);
        }
        jsonData["predictionStats"] = predictionStatsJson;
        
        // Export neural activity
        std::vector<nlohmann::json> neuralActivityJson;
        for (const auto& activity : neuralActivityHistory) {
            nlohmann::json activityJson;
            activityJson["neuronId"] = activity.neuronId;
            activityJson["membranePotential"] = activity.membranePotential;
            activityJson["threshold"] = activity.threshold;
            activityJson["inputCurrent"] = activity.inputCurrent;
            activityJson["outputCurrent"] = activity.outputCurrent;
            activityJson["isSpiking"] = activity.isSpiking;
            activityJson["timestamp"] = activity.timestamp.time_since_epoch().count();
            activityJson["step"] = activity.step;
            neuralActivityJson.push_back(activityJson);
        }
        jsonData["neuralActivity"] = neuralActivityJson;
        
        // Export configuration
        nlohmann::json configJson;
        configJson["debugLevel"] = debugLevel;
        configJson["traceTargets"] = traceTargets;
        configJson["profileTargets"] = profileTargets;
        configJson["eventBufferSize"] = eventBufferSize;
        configJson["traceBufferSize"] = traceBufferSize;
        configJson["performanceBufferSize"] = performanceBufferSize;
        configJson["memoryStatsBufferSize"] = memoryStatsBufferSize;
        configJson["neuromodulatorStatsBufferSize"] = neuromodulatorStatsBufferSize;
        configJson["predictionStatsBufferSize"] = predictionStatsBufferSize;
        configJson["neuralActivityBufferSize"] = neuralActivityBufferSize;
        jsonData["configuration"] = configJson;
        
        std::ofstream file(filename);
        if (file.is_open()) {
            file << jsonData.dump(2);
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving debug state: " << e.what() << std::endl;
    }
    
    return false;
}

bool DebugManager::loadDebugState(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::string filename = filepath.empty() ? "debug_state.json" : filepath;
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData = nlohmann::json::parse(file);
        
        // Clear existing history
        clearEventHistory();
        clearTraceHistory();
        clearPerformanceHistory();
        clearMemoryStatsHistory();
        clearNeuromodulatorStatsHistory();
        clearPredictionStatsHistory();
        clearNeuralActivityHistory();
        
        // Import events
        if (jsonData.contains("events")) {
            for (const auto& eventJson : jsonData["events"]) {
                DebugEvent event(static_cast<DebugEventType>(eventJson["type"].get<int>()),
                               eventJson["source"].get<std::string>(),
                               eventJson["message"].get<std::string>());
                event.metadata = eventJson["metadata"].get<std::map<std::string, std::string>>();
                event.step = eventJson["step"].get<uint64_t>();
                event.timestamp = std::chrono::system_clock::time_point(
                    std::chrono::duration_cast<std::chrono::system_clock::duration>(
                        std::chrono::duration<int64_t>(eventJson["timestamp"].get<int64_t>())));
                
                // Process event
                processEvent(event);
            }
        }
        
        // Import traces (similar for other types)
        // ... (additional import code)
        
        // Import configuration
        if (jsonData.contains("configuration")) {
            debugLevel = jsonData["configuration"]["debugLevel"].get<int>();
            traceTargets = jsonData["configuration"]["traceTargets"].get<std::vector<std::string>>();
            profileTargets = jsonData["configuration"]["profileTargets"].get<std::vector<std::string>>();
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading debug state: " << e.what() << std::endl;
    }
    
    return false;
}

void DebugManager::addEventFilter(DebugEventType type, const std::function<bool(const DebugEvent&)>& filter) {
    std::lock_guard<std::mutex> lock(mutex);
    eventFilters[type].push_back(filter);
}

void DebugManager::removeEventFilter(DebugEventType type) {
    std::lock_guard<std::mutex> lock(mutex);
    eventFilters.erase(type);
}

void DebugManager::clearEventFilters() {
    std::lock_guard<std::mutex> lock(mutex);
    eventFilters.clear();
}

std::string DebugManager::exportEvents(const std::string& format) const {
    if (format == "json") {
        return exportToJSON(eventHistory);
    } else if (format == "csv") {
        // CSV export implementation
        return "CSV export not implemented yet";
    } else {
        return exportToJSON(eventHistory);
    }
}

std::string DebugManager::exportTraces(const std::string& format) const {
    if (format == "json") {
        return exportToJSON(traceHistory);
    } else if (format == "csv") {
        // CSV export implementation
        return "CSV export not implemented yet";
    } else {
        return exportToJSON(traceHistory);
    }
}

std::string DebugManager::exportPerformanceMetrics(const std::string& format) const {
    if (format == "json") {
        return exportToJSON(performanceHistory);
    } else if (format == "csv") {
        // CSV export implementation
        return "CSV export not implemented yet";
    } else {
        return exportToJSON(performanceHistory);
    }
}

std::string DebugManager::exportMemoryStats(const std::string& format) const {
    if (format == "json") {
        return exportToJSON(memoryHistory);
    } else if (format == "csv") {
        // CSV export implementation
        return "CSV export not implemented yet";
    } else {
        return exportToJSON(memoryHistory);
    }
}

std::string DebugManager::exportNeuromodulatorStats(const std::string& format) const {
    if (format == "json") {
        return exportToJSON(neuromodulatorHistory);
    } else if (format == "csv") {
        // CSV export implementation
        return "CSV export not implemented yet";
    } else {
        return exportToJSON(neuromodulatorHistory);
    }
}

std::string DebugManager::exportPredictionStats(const std::string& format) const {
    if (format == "json") {
        return exportToJSON(predictionHistory);
    } else if (format == "csv") {
        // CSV export implementation
        return "CSV export not implemented yet";
    } else {
        return exportToJSON(predictionHistory);
    }
}

std::string DebugManager::exportNeuralActivity(const std::string& format) const {
    if (format == "json") {
        return exportToJSON(neuralActivityHistory);
    } else if (format == "csv") {
        // CSV export implementation
        return "CSV export not implemented yet";
    } else {
        return exportToJSON(neuralActivityHistory);
    }
}

void DebugManager::processEvent(const DebugEvent& event) {
    // Check if event should be filtered
    if (shouldFilterEvent(event)) {
        return;
    }
    
    // Notify all listeners
    for (const auto& listener : eventListeners) {
        try {
            listener(event);
        } catch (const std::exception& e) {
            std::cerr << "Error in event listener: " << e.what() << std::endl;
        }
    }
}

bool DebugManager::shouldFilterEvent(const DebugEvent& event) const {
    auto it = eventFilters.find(event.type);
    if (it == eventFilters.end()) {
        return false;
    }
    
    for (const auto& filter : it->second) {
        if (filter(event)) {
            return true;
        }
    }
    
    return false;
}

void DebugManager::cleanupBuffers() {
    // Remove old events from history to maintain buffer size
    while (eventHistory.size() > eventBufferSize) {
        eventHistory.pop_front();
    }
    while (traceHistory.size() > traceBufferSize) {
        traceHistory.pop_front();
    }
    while (performanceHistory.size() > performanceBufferSize) {
        performanceHistory.pop_front();
    }
    while (memoryHistory.size() > memoryStatsBufferSize) {
        memoryHistory.pop_front();n    }
    while (neuromodulatorHistory.size() > neuromodulatorStatsBufferSize) {
        neuromodulatorHistory.pop_front();
    }
    while (predictionHistory.size() > predictionStatsBufferSize) {
        predictionHistory.pop_front();
    }
    while (neuralActivityHistory.size() > neuralActivityBufferSize) {
        neuralActivityHistory.pop_front();
    }
}

void DebugManager::eventProcessingThread() {
    while (eventThreadRunning) {
        std::unique_lock<std::mutex> lock(mutex);
        
        // Wait for events
        if (eventQueue.empty()) {
            eventCv.wait_for(lock, std::chrono::milliseconds(100));
            continue;
        }
        
        // Process events
        while (!eventQueue.empty()) {
            DebugEvent event = eventQueue.front();
            eventQueue.pop_front();
            
            processEvent(event);
        }
    }
}

void DebugManager::loadConfiguration(const std::string& filepath) {
    // Load debug configuration from file
    // This would load settings for debug levels, tracing, etc.
    debugLevel = 2; // Default debug level
    traceTargets = {"neuron", "synapse", "memory"};
    profileTargets = {"simulation", "memory"};
    eventBufferSize = 10000;
    traceBufferSize = 100000;
    performanceBufferSize = 1000;
    memoryStatsBufferSize = 1000;
    neuromodulatorStatsBufferSize = 1000;
    predictionStatsBufferSize = 1000;
    neuralActivityBufferSize = 10000;
}

void DebugManager::saveConfiguration(const std::string& filepath) const {
    // Save debug configuration to file
    // This would save current debug settings
}

std::string DebugManager::exportToJSON(const std::vector<DebugEvent>& events) const {
    nlohmann::json jsonData;
    
    for (const auto& event : events) {
        nlohmann::json eventJson;
        eventJson["type"] = static_cast<int>(event.type);
        eventJson["timestamp"] = event.timestamp.time_since_epoch().count();
        eventJson["source"] = event.source;
        eventJson["message"] = event.message;
        eventJson["step"] = event.step;
        eventJson["metadata"] = event.metadata;
        jsonData.push_back(eventJson);
    }
    
    return jsonData.dump(2);
}

std::string DebugManager::exportToJSON(const std::vector<TracePoint>& traces) const {
    nlohmann::json jsonData;
    
    for (const auto& trace : traces) {
        nlohmann::json traceJson;
        traceJson["eventType"] = trace.eventType;
        traceJson["component"] = trace.component;
        traceJson["details"] = trace.details;
        traceJson["timestamp"] = trace.timestamp.time_since_epoch().count();
        traceJson["step"] = trace.step;
        traceJson["threadId"] = trace.threadId;
        jsonData.push_back(traceJson);
    }
    
    return jsonData.dump(2);
}

std::string DebugManager::exportToJSON(const std::vector<PerformanceMetrics>& metrics) const {
    nlohmann::json jsonData;
    
    for (const auto& metric : metrics) {
        nlohmann::json metricJson;
        metricJson["neuronCount"] = metric.neuronCount;
        metricJson["synapseCount"] = metric.synapseCount;
        metricJson["spikeCount"] = metric.spikeCount;
        metricJson["memoryUsage"] = metric.memoryUsage;
        metricJson["cpuUsage"] = metric.cpuUsage;
        metricJson["stepDuration"] = metric.stepDuration.count();
        metricJson["timestamp"] = metric.timestamp.time_since_epoch().count();
        metricJson["step"] = metric.step;
        jsonData.push_back(metricJson);
    }
    
    return jsonData.dump(2);
}

std::string DebugManager::exportToJSON(const std::vector<MemoryStats>& stats) const {
    nlohmann::json jsonData;
    
    for (const auto& stat : stats) {
        nlohmann::json statJson;
        statJson["workingMemorySize"] = stat.workingMemorySize;
        statJson["workingMemoryUsed"] = stat.workingMemoryUsed;
        statJson["episodicMemorySize"] = stat.episodicMemorySize;
        statJson["episodicMemoryUsed"] = stat.episodicMemoryUsed;
        statJson["semanticMemorySize"] = stat.semanticMemorySize;
        statJson["semanticMemoryUsed"] = stat.semanticMemoryUsed;
        statJson["proceduralMemorySize"] = stat.proceduralMemorySize;
        statJson["proceduralMemoryUsed"] = stat.proceduralMemoryUsed;
        statJson["associativeMemorySize"] = stat.associativeMemorySize;
        statJson["associativeMemoryUsed"] = stat.associativeMemoryUsed;
        statJson["timestamp"] = stat.timestamp.time_since_epoch().count();
        statJson["step"] = stat.step;
        jsonData.push_back(statJson);
    }
    
    return jsonData.dump(2);
}

std::string DebugManager::exportToJSON(const std::vector<NeuromodulatorStats>& stats) const {
    nlohmann::json jsonData;
    
    for (const auto& stat : stats) {
        nlohmann::json statJson;
        statJson["dopamine"] = stat.dopamine;
        statJson["curiosity"] = stat.curiosity;
        statJson["novelty"] = stat.novelty;
        statJson["acetylcholine"] = stat.acetylcholine;
        statJson["norepinephrine"] = stat.norepinephrine;
        statJson["serotonin"] = stat.serotonin;
        statJson["nicotine"] = stat.nicotine;
        statJson["orexin"] = stat.orexin;
        statJson["timestamp"] = stat.timestamp.time_since_epoch().count();
        statJson["step"] = stat.step;
        jsonData.push_back(statJson);
    }
    
    return jsonData.dump(2);
}

std::string DebugManager::exportToJSON(const std::vector<PredictionStats>& stats) const {
    nlohmann::json jsonData;
    
    for (const auto& stat : stats) {
        nlohmann::json statJson;
        statJson["predictionError"] = stat.predictionError;
        statJson["predictionAccuracy"] = stat.predictionAccuracy;
        statJson["confidence"] = stat.confidence;
        statJson["learningRate"] = stat.learningRate;
        statJson["totalPredictions"] = stat.totalPredictions;
        statJson["correctPredictions"] = stat.correctPredictions;
        statJson["timestamp"] = stat.timestamp.time_since_epoch().count();
        statJson["step"] = stat.step;
        jsonData.push_back(statJson);
    }
    
    return jsonData.dump(2);
}

std::string DebugManager::exportToJSON(const std::vector<NeuralActivity>& activity) const {
    nlohmann::json jsonData;
    
    for (const auto& act : activity) {
        nlohmann::json actJson;
        actJson["neuronId"] = act.neuronId;
        actJson["membranePotential"] = act.membranePotential;
        actJson["threshold"] = act.threshold;
        actJson["inputCurrent"] = act.inputCurrent;
        actJson["outputCurrent"] = act.outputCurrent;
        actJson["isSpiking"] = act.isSpiking;
        actJson["timestamp"] = act.timestamp.time_since_epoch().count();
        actJson["step"] = act.step;
        jsonData.push_back(actJson);
    }
    
    return jsonData.dump(2);
}

} // namespace nlm
