// Debug and Profiling Tools
namespace nlm {

// Debug configuration for development
struct DebugConfig {
    bool enableStepTracing = false;
    bool enableMemoryTracking = false;
    bool enablePerformanceProfiling = false;
    bool enableNeuronsMonitoring = false;
    bool enableConnectionsMonitoring = false;
    bool enablePlasticityMonitoring = false;
    int traceLevel = 1;  // 0: ERROR, 1: WARN, 2: INFO, 3: DEBUG
    size_t maxMemorySnapshots = 100;
    float memoryUsageThreshold = 0.8f;
    size_t neuronSamplingRate = 100;  // Sample every N steps
    bool enableNeuronHeatmap = false;
    bool enableSpiketrainRecording = false;
    std::string outputDirectory = "./debug_output";
};

// Performance metrics collected during simulation
struct PerformanceMetrics {
    double totalSimulationTime = 0.0;
    size_t totalSteps = 0;
    size_t totalSpikes = 0;
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    double averageStepTime = 0.0;
    double spikesPerSecond = 0.0;
    double neuronsPerMillisecond = 0.0;
    std::vector<double> stepTimes;
    std::vector<size_t> spikeCounts;
    std::unordered_map<std::string, double> customMetrics;
    
    void reset() {
        totalSimulationTime = 0.0;
        totalSteps = 0;
        totalSpikes = 0;
        totalNeurons = 0;
        totalSynapses = 0;
        averageStepTime = 0.0;
        spikesPerSecond = 0.0;
        neuronsPerMillisecond = 0.0;
        stepTimes.clear();
        spikeCounts.clear();
        customMetrics.clear();
    }
    
    void addStep(double stepTime, size_t spikes) {
        stepTimes.push_back(stepTime);
        spikeCounts.push_back(spikes);
        totalSteps++;
        totalSpikes += spikes;
        if (!stepTimes.empty()) {
            averageStepTime = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0) / stepTimes.size();
        }
        if (stepTimes.size() > 0) {
            spikesPerSecond = totalSpikes / (totalSimulationTime / 1000.0);
        }
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== PERFORMANCE METRICS REPORT ===\n";
        oss << "Total simulation time: " << totalSimulationTime << " ms\n";
        oss << "Total steps: " << totalSteps << "\n";
        oss << "Total spikes: " << totalSpikes << "\n";
        oss << "Average step time: " << averageStepTime << " ms\n";
        oss << "Steps per second: " << (totalSteps / (totalSimulationTime / 1000.0)) << "\n";
        oss << "Spikes per second: " << spikesPerSecond << "\n";
        oss << "Neurons per millisecond: " << neuronsPerMillisecond << "\n";
        oss << "\nStep time statistics:\n";
        if (!stepTimes.empty()) {
            double minTime = *std::min_element(stepTimes.begin(), stepTimes.end());
            double maxTime = *std::max_element(stepTimes.begin(), stepTimes.end());
            double meanTime = averageStepTime;
            double variance = 0.0;
            for (double time : stepTimes) {
                variance += (time - meanTime) * (time - meanTime);
            }
            variance /= stepTimes.size();
            double stdDev = std::sqrt(variance);
            oss << "  Min: " << minTime << " ms\n";
            oss << "  Max: " << maxTime << " ms\n";
            oss << "  Mean: " << meanTime << " ms\n";
            oss << "  Std Dev: " << stdDev << " ms\n";
            oss << "  95th percentile: " << getPercentile(stepTimes, 0.95) << " ms\n";
        }
        oss << "\nSpike count statistics:\n";
        if (!spikeCounts.empty()) {
            size_t minSpikes = *std::min_element(spikeCounts.begin(), spikeCounts.end());
            size_t maxSpikes = *std::max_element(spikeCounts.begin(), spikeCounts.end());
            double meanSpikes = static_cast<double>(totalSpikes) / spikeCounts.size();
            oss << "  Min: " << minSpikes << " spikes\n";
            oss << "  Max: " << maxSpikes << " spikes\n";
            oss << "  Mean: " << meanSpikes << " spikes\n";
        }
        oss << "\nCustom metrics:\n";
        for (const auto& metric : customMetrics) {
            oss << "  " << metric.first << ": " << metric.second << "\n";
        }
        return oss.str();
    }
    
private:
    static double getPercentile(const std::vector<double>& data, double percentile) {
        if (data.empty()) return 0.0;
        size_t index = static_cast<size_t>(percentile * (data.size() - 1));
        std::vector<double> sorted = data;
        std::sort(sorted.begin(), sorted.end());
        return sorted[index];
    }
};

// Memory snapshot for debugging
struct MemorySnapshot {
    std::chrono::high_resolution_clock::time_point timestamp;
    size_t neuronCount;
    size_t synapseCount;
    size_t totalMemoryUsage;
    std::vector<float> neuronMemoryUsage;
    std::vector<float> synapseMemoryUsage;
    std::unordered_map<std::string, float> systemMetrics;
    
    MemorySnapshot(size_t neurons, size_t synapses, size_t totalMem) {
        timestamp = std::chrono::high_resolution_clock::now();
        neuronCount = neurons;
        synapseCount = synapses;
        totalMemoryUsage = totalMem;
    }
};

// Neuron heatmap for activity visualization
struct NeuronHeatmap {
    std::unordered_map<NeuronId, std::vector<float>> neuronActivity;
    std::vector<std::vector<float>> spatialMap;
    float maxActivity = 0.0f;
    size_t width = 0;
    size_t height = 0;
    
    void addActivity(NeuronId neuron, float activity, size_t x, size_t y) {
        neuronActivity[neuron].push_back(activity);
        if (activity > maxActivity) maxActivity = activity;
        if (width > 0 && height > 0 && x < width && y < height) {
            spatialMap[y][x] += activity;
        }
    }
    
    std::string generateHeatmapData() const {
        std::ostringstream oss;
        oss << "Neuron Heatmap Data\n";
        oss << "Max activity: " << maxActivity << "\n";
        oss << "Total neurons: " << neuronActivity.size() << "\n";
        oss << "Activity statistics:\n";
        for (const auto& [neuron, activities] : neuronActivity) {
            if (!activities.empty()) {
                float meanActivity = std::accumulate(activities.begin(), activities.end(), 0.0f) / activities.size();
                oss << "  Neuron " << neuron.index() << ": mean=" << meanActivity << ", max=" 
                    << *std::max_element(activities.begin(), activities.end()) << "\n";
            }
        }
        return oss.str();
    }
};

// Spike train recording for detailed spike analysis
struct SpikeTrainRecorder {
    std::unordered_map<NeuronId, std::vector<std::pair<Timestamp, float>>> spikeHistory;
    std::vector<std::pair<Timestamp, NeuronId>> allSpikes;
    bool recording = false;
    size_t maxSpikes = 100000;
    
    void recordSpike(NeuronId neuron, Timestamp time, float weight = 1.0f) {
        if (!recording) return;
        
        spikeHistory[neuron].emplace_back(time, weight);
        allSpikes.emplace_back(time, neuron);
        
        // Trim if exceeds maximum
        if (spikeHistory.size() > maxSpikes) {
            spikeHistory.clear();
            allSpikes.clear();
        }
    }
    
    std::string generateSpikeReport() const {
        std::ostringstream oss;
        oss << "=== SPIKE TRAIN ANALYSIS ===\n";
        oss << "Total recorded spikes: " << allSpikes.size() << "\n";
        oss << "Unique neurons: " << spikeHistory.size() << "\n";
        
        if (!allSpikes.empty()) {
            oss << "\nTime statistics:\n";
            Timestamp firstTime = allSpikes.front().first;
            Timestamp lastTime = allSpikes.back().first;
            double duration = lastTime - firstTime;
            oss << "  First spike: " << firstTime << " ms\n";
            oss << "  Last spike: " << lastTime << " ms\n";
            oss << "  Duration: " << duration << " ms\n";
            oss << "  Overall rate: " << (allSpikes.size() / (duration / 1000.0)) << " spikes/second\n";
            
            // Analyze firing rates per neuron
            oss << "\nPer-neuron firing statistics:\n";
            for (const auto& [neuron, spikes] : spikeHistory) {
                if (spikes.size() > 1) {
                    double neuronDuration = spikes.back().first - spikes.front().first;
                    double firingRate = (spikes.size() - 1) / neuronDuration * 1000.0;  // spikes/sec
                    oss << "  Neuron " << neuron.index() << ": " << spikes.size() 
                        << " spikes, " << firingRate << " Hz, duration=" << neuronDuration << " ms\n";
                }
            }
            
            // Inter-spike interval analysis
            oss << "\nInter-spike interval analysis:\n";
            std::vector<double> intervals;
            for (const auto& [time, neuron] : allSpikes) {
                if (neuron != NeuronId()) {
                    auto it = std::find_if(allSpikes.begin(), allSpikes.end(),
                        [neuron, time](const auto& p) {
                            return p.second == neuron && p.first < time;
                        });
                    if (it != allSpikes.end()) {
                        double interval = time - it->first;
                        intervals.push_back(interval);
                    }
                }
            }
            if (!intervals.empty()) {
                double meanInterval = std::accumulate(intervals.begin(), intervals.end(), 0.0) / intervals.size();
                double stdDev = 0.0;
                for (double interval : intervals) {
                    stdDev += (interval - meanInterval) * (interval - meanInterval);
                }
                stdDev = std::sqrt(stdDev / intervals.size());
                oss << "  Mean ISI: " << meanInterval << " ms\n";
                oss << "  Std Dev: " << stdDev << " ms\n";
                oss << "  Coefficient of variation: " << (stdDev / meanInterval) << "\n";
            }
        }
        
        return oss.str();
    }
};

// Comprehensive debugger for NLM development
class Debugger {
private:
    DebugConfig config;
    PerformanceMetrics performanceMetrics;
    std::vector<MemorySnapshot> memorySnapshots;
    NeuronHeatmap neuronHeatmap;
    SpikeTrainRecorder spikeRecorder;
    bool isRecording = false;
    
public:
    Debugger() = default;
    
    void configure(const DebugConfig& cfg) {
        config = cfg;
        if (config.outputDirectory.empty()) {
            config.outputDirectory = "./debug_output";
        }
        // Create output directory
        std::filesystem::create_directories(config.outputDirectory);
        
        NLM_LOG_INFO("Debugger configured with output directory: " + config.outputDirectory);
    }
    
    void startRecording() {
        isRecording = true;
        performanceMetrics.reset();
        memorySnapshots.clear();
        spikeRecorder.recording = true;
        
        NLM_LOG_INFO("Debug recording started");
    }
    
    void stopRecording() {
        isRecording = false;
        spikeRecorder.recording = false;
        
        NLM_LOG_INFO("Debug recording stopped");
        saveDebugData();
    }
    
    void recordStep(const Brain& brain, double stepTime, size_t spikes, SimulationStep step, Timestamp time) {
        if (!isRecording) return;
        
        performanceMetrics.addStep(stepTime, spikes);
        
        if (config.enableMemoryTracking) {
            memorySnapshots.emplace_back(
                brain.getTotalNeuronCount(),
                brain.getTotalSynapseCount(),
                0  // Placeholder for memory usage
            );
            if (memorySnapshots.size() > config.maxMemorySnapshots) {
                memorySnapshots.erase(memorySnapshots.begin());
            }
        }
        
        if (config.enableNeuronsMonitoring && step % config.neuronSamplingRate == 0) {
            auto* region = brain.getRegion(RegionId(1));
            if (region) {
                for (auto* neuron : region->getAllNeurons()) {
                    float activity = neuron->getFiringRate() * 0.1f;  // Normalize to 0-1
                    if (config.enableNeuronHeatmap && neuron->isFiring()) {
                        neuronHeatmap.addActivity(neuron->getId(), activity, 0, 0);  // TODO: get actual position
                    }
                }
            }
        }
        
        if (config.enableSpiketrainRecording) {
            auto* region = brain.getRegion(RegionId(1));
            if (region) {
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron->isFiring()) {
                        spikeRecorder.recordSpike(neuron->getId(), time);
                    }
                }
            }
        }
    }
    
    void logStep(const Brain& brain, SimulationStep step, Timestamp time) {
        if (!config.enableStepTracing) return;
        
        std::ostringstream oss;
        oss << "[STEP " << step << "] Time: " << time << " ms\n";
        oss << "  Neurons: " << brain.getTotalNeuronCount() << "\n";
        oss << "  Synapses: " << brain.getTotalSynapseCount();
        oss << "\n  Active neurons: " << brain.getActiveNeuronCount();
        oss << "\n  Firing neurons: " << brain.getFiringNeuronCount();
        oss << "\n  Total spikes: " << performanceMetrics.totalSpikes;
        
        NLM_LOG_DEBUG(oss.str());
    }
    
    void generateReports(const std::string& filename) {
        std::string reportDir = config.outputDirectory;
        
        // Performance report
        std::ofstream perfFile(reportDir + "/performance_" + filename + ".txt");
        perfFile << performanceMetrics.generateReport();
        
        // Memory report
        std::ofstream memFile(reportDir + "/memory_" + filename + ".txt");
        memFile << "=== MEMORY SNAPSHOTS ===\n";
        for (size_t i = 0; i < memorySnapshots.size(); ++i) {
            const auto& snapshot = memorySnapshots[i];
            auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
                snapshot.timestamp.time_since_epoch()).count();
            memFile << "Snapshot " << i << ": time=" << time << 
                   " ms, neurons=" << snapshot.neuronCount << 
                   " synapses=" << snapshot.synapseCount << "\n";
        }
        
        // Neuron heatmap report
        std::ofstream heatmapFile(reportDir + "/heatmap_" + filename + ".txt");
        heatmapFile << neuronHeatmap.generateHeatmapData();
        
        // Spike train report
        std::ofstream spikeFile(reportDir + "/spiketrain_" + filename + ".txt");
        spikeFile << spikeRecorder.generateSpikeReport();
        
        // Debug log
        std::ofstream debugFile(reportDir + "/debug_log_" + filename + ".txt");
        debugFile << "=== DEBUG LOG ===\n";
        debugFile << "Recording started at: " << (isRecording ? "yes" : "no") << "\n";
        debugFile << "Debug config:\n";
        debugFile << "  Step tracing: " << (config.enableStepTracing ? "yes" : "no") << "\n";
        debugFile << "  Memory tracking: " << (config.enableMemoryTracking ? "yes" : "no") << "\n";
        debugFile << "  Performance profiling: " << (config.enablePerformanceProfiling ? "yes" : "no") << "\n";
        debugFile << "  Neuron monitoring: " << (config.enableNeuronsMonitoring ? "yes" : "no") << "\n";
        debugFile << "  Connections monitoring: " << (config.enableConnectionsMonitoring ? "yes" : "no") << "\n";
        debugFile << "  Plasticity monitoring: " << (config.enablePlasticityMonitoring ? "yes" : "no") << "\n";
        
        NLM_LOG_INFO("Debug reports saved to: " + reportDir);
    }
    
    void saveDebugData() {
        if (!isRecording) {
            generateReports("final");
        }
    }
    
    void exportToPython(const std::string& filename) {
        // Create Python-compatible data structures for visualization
        std::map<std::string, py::object> pythonData;
        
        // Performance metrics
        std::vector<py::object> performancePy;
        for (double time : performanceMetrics.stepTimes) {
            performancePy.push_back(py::cast(time));
        }
        pythonData["step_times"] = py::cast(performancePy);
        
        // Memory snapshots
        std::vector<py::object> memoryPy;
        for (const auto& snapshot : memorySnapshots) {
            std::map<std::string, py::object> snapPy;
            snapPy["timestamp"] = py::cast(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    snapshot.timestamp.time_since_epoch()).count());
            snapPy["neuron_count"] = py::cast(snapshot.neuronCount);
            snapPy["synapse_count"] = py::cast(snapshot.synapseCount);
            memoryPy.push_back(py::cast(snapPy));
        }
        pythonData["memory_snapshots"] = py::cast(memoryPy);
        
        // Neuron heatmap
        std::map<std::string, py::object> heatmapPy;
        heatmapPy["max_activity"] = py::cast(neuronHeatmap.maxActivity);
        heatmapPy["neuron_count"] = py::cast(neuronHeatmap.neuronActivity.size());
        pythonData["heatmap"] = py::cast(heatmapPy);
        
        // Spike train
        std::vector<py::object> spikesPy;
        for (const auto& spike : spikeRecorder.allSpikes) {
            std::map<std::string, py::object> spikePy;
            spikePy["time"] = py::cast(spike.first);
            spikePy["neuron"] = py::cast(spike.second.index());
            spikesPy.push_back(py::cast(spikePy));
        }
        pythonData["spikes"] = py::cast(spikesPy);
        
        // Save as JSON (simplified implementation)
        std::ofstream file(config.outputDirectory + "/" + filename + ".json");
        file << "{\n";
        bool first = true;
        for (const auto& [key, value] : pythonData) {
            if (!first) file << ",\n";
            file << "  \"" << key << "\": ";
            // Simplified JSON serialization (would need proper JSON library for full implementation)
            file << "null";  // Placeholder
            first = false;
        }
        file << "\n}\n";
        
        NLM_LOG_INFO("Debug data exported to Python format: " + filename + ".json");
    }
};

// Global debugger instance
Debugger gDebugger;

// Helper functions for development
void initializeDebugger(const DebugConfig& config) {
    gDebugger.configure(config);
}

void startDebugRecording() {
    gDebugger.startRecording();
}

void stopDebugRecording() {
    gDebugger.stopRecording();
}

void recordStep(const Brain& brain, double stepTime, size_t spikes, SimulationStep step, Timestamp time) {
    gDebugger.recordStep(brain, stepTime, spikes, step, time);
}

void generateDebugReport(const std::string& filename) {
    gDebugger.generateReports(filename);
}

void exportDebugDataToPython(const std::string& filename) {
    gDebugger.exportToPython(filename);
}

void quickDebug(const Brain& brain, SimulationStep step, Timestamp time) {
    // Quick debugging with minimal overhead
    if (gDebugger.isRecording) {
        gDebugger.logStep(brain, step, time);
    }
}

} // namespace nlm