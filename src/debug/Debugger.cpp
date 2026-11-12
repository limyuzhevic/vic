// Debug and Profiling Tools Implementation
#include "Debugger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Global debugger instance
Debugger gDebugger;

// Implementation of Debugger methods

void Debugger::generateReports(const std::string& filename) {
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

void Debugger::saveDebugData() {
    if (!isRecording) {
        generateReports("final");
    }
}

void Debugger::exportToPython(const std::string& filename) {
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
// Implementation of helper functions
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