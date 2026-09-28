#include "EnhancedVisualizer.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace nlm {
namespace visualization {

// Implementation of EnhancedVisualizer::NeuralActivityVisualizer
EnhancedVisualizer::NeuralActivityVisualizer::NeuralActivityVisualizer(size_t maxHistorySize)
    : maxHistorySize_(maxHistorySize) {}

EnhancedVisualizer::NeuralActivityVisualizer::BrainActivitySnapshot 
EnhancedVisualizer::NeuralActivityVisualizer::captureBrainActivity(std::shared_ptr<Brain> brain) {
    BrainActivitySnapshot snapshot(0, 0.0); // placeholder step and time
    
    if (!brain) {
        return snapshot;
    }
    
    // Get brain metrics
    snapshot.totalSpikes = brain->getTotalSpikeCount();
    snapshot.averageFiringRate = brain->getAverageFiringRate();
    snapshot.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    snapshot.firingNeuronCount = brain->getFiringNeuronCount();
    snapshot.activeSynapseCount = brain->getTotalSynapseCount();
    
    // Capture neuron activities
    auto regions = brain->getRegions();
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            NeuronActivity activity(NeuronId(0), NeuronType::Excitatory); // placeholder
            activity.firingRate = 0.0f; // placeholder
            activity.membranePotential = neuron->getMembranePotential();
            activity.synapticConductance = 0.0f; // placeholder
            activity.lastSpikeTime = neuron->getLastSpikeTime();
            snapshot.neuronActivities.push_back(activity);
        }
    }
    
    // Capture connectivity matrix
    size_t numNeurons = brain->getTotalNeuronCount();
    if (numNeurons > 0) {
        snapshot.connectivityMatrix.resize(numNeurons, std::vector<float>(numNeurons, 0.0f));
        
        // Placeholder for connectivity matrix calculation
        // This would need access to actual synapse data
    }
    
    return snapshot;
}

void EnhancedVisualizer::NeuralActivityVisualizer::addActivity(
    const BrainActivitySnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(historyMutex_);
    
    activityHistory_.push_back(snapshot);
    
    // Maintain max history size
    if (activityHistory_.size() > maxHistorySize_) {
        activityHistory_.erase(activityHistory_.begin());
    }
}

std::vector<EnhancedVisualizer::NeuralActivityVisualizer::BrainActivitySnapshot> 
EnhancedVisualizer::NeuralActivityVisualizer::getActivityHistory() const {
    std::lock_guard<std::mutex> lock(historyMutex_);
    return activityHistory_;
}

std::vector<EnhancedVisualizer::NeuralActivityVisualizer::BrainActivitySnapshot> 
EnhancedVisualizer::NeuralActivityVisualizer::getActivityInRange(
    double startTime, double endTime) const {
    std::lock_guard<std::mutex> lock(historyMutex_);
    
    std::vector<BrainActivitySnapshot> result;
    
    for (const auto& snapshot : activityHistory_) {
        if (snapshot.time >= startTime && snapshot.time <= endTime) {
            result.push_back(snapshot);
        }
    }
    
    return result;
}

std::string EnhancedVisualizer::NeuralActivityVisualizer::generateNetworkVisualization(
    OutputFormat format, const std::string& style) {
    
    switch (format) {
        case OutputFormat::ASCII_ART:
            return generateASCIIArtNetworkVisualization();
        case OutputFormat::JSON:
            return generateJSONNetworkVisualization();
        case OutputFormat::HTML:
            return generateHTMLNetworkVisualization();
        case OutputFormat::CSV:
            return generateCSVActivityVisualization();
        case OutputFormat::PLAINTEXT:
            return generateMatrixVisualization();
        default:
            return "Unsupported format: " + std::to_string(static_cast<int>(format));
    }
}

std::string EnhancedVisualizer::NeuralActivityVisualizer::generateASCIIArtNetworkVisualization() const {
    std::ostringstream oss;
    oss << "=== Neural Network Visualization ===\n";
    oss << "Total Neurons: " << (activityHistory_.empty() ? 0 : activityHistory_.back().neuronActivities.size()) << "\n";
    oss << "Total Synapses: " << (activityHistory_.empty() ? 0 : activityHistory_.back().activeSynapseCount) << "\n\n";
    
    if (!activityHistory_.empty()) {
        const auto& snapshot = activityHistory_.back();
        
        // Simple ASCII representation of neural activity
        oss << "Neuron Activity (0 = resting, 1 = firing):\n";
        for (size_t i = 0; i < std::min(size_t(20), snapshot.neuronActivities.size()); ++i) {
            oss << "  Neuron " << i << ": ";
            for (int j = 0; j < 10; ++j) {
                bool firing = (i * 10 + j) < snapshot.firingNeuronCount;
                oss << (firing ? "*" : ".");
            }
            oss << "\n";
        }
    }
    
    oss << "\n=== End Visualization ===";
    return oss.str();
}

std::string EnhancedVisualizer::NeuralActivityVisualizer::generateJSONNetworkVisualization() const {
    py::dict jsonData;
    
    jsonData["network_type"] = "neural_network";
    jsonData["timestamp"] = std::chrono::duration<double>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    if (!activityHistory_.empty()) {
        const auto& snapshot = activityHistory_.back();
        jsonData["total_neurons"] = snapshot.neuronActivities.size();
        jsonData["total_synapses"] = snapshot.activeSynapseCount;
        jsonData["average_firing_rate"] = snapshot.averageFiringRate;
        jsonData["total_spikes"] = snapshot.totalSpikes;
        jsonData["excitation_inhibition_ratio"] = snapshot.excitationInhibitionRatio;
        
        // Add neuron data
        py::list neurons;
        for (const auto& neuron : snapshot.neuronActivities) {
            py::dict neuronData;
            neuronData["id"] = neuron.neuronId.value;
            neuronData["type"] = static_cast<int>(neuron.type);
            neuronData["firing_rate"] = neuron.firingRate;
            neuronData["membrane_potential"] = neuron.membranePotential;
            neuronData["last_spike_time"] = neuron.lastSpikeTime;
            neurons.append(neuronData);
        }
        jsonData["neurons"] = neurons;
        
        // Add connectivity matrix
        py::list adjacencyMatrix;
        for (const auto& row : snapshot.connectivityMatrix) {
            py::list matrixRow;
            for (float weight : row) {
                matrixRow.append(weight);
            }
            adjacencyMatrix.append(matrixRow);
        }
        jsonData["adjacency_matrix"] = adjacencyMatrix;
    }
    
    // Convert py::dict to JSON string
    // This is a simplified version - in practice, you'd use a proper JSON library
    std::ostringstream oss;
    oss << "{\n";
    bool first = true;
    for (auto item : jsonData) {
        if (!first) oss << ",\n";
        oss << "  \"" << py::str(item.first) << "\": ";
        
        if (py::isinstance<py::str>(item.second)) {
            oss << "\"" << py::str(item.second) << "\"";
        } else if (py::isinstance<py::int>(item.second)) {
            oss << py::int_(item.second);
        } else if (py::isinstance<py::float>(item.second)) {
            oss << std::fixed << std::setprecision(6) << py::float_(item.second);
        }
        
        first = false;
    }
    oss << "\n}";
    
    return oss.str();
}

std::string EnhancedVisualizer::NeuralActivityVisualizer::generateHTMLNetworkVisualization() const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "    <title>NLM Neural Network Visualization</title>\n";
    oss << "    <style>\n";
    oss << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
    oss << "        .neuron { fill: #e74c3c; stroke: #c0392b; stroke-width: 2; }\n";
    oss << "        .active { fill: #e74c3c; opacity: 0.8; }\n";
    oss << "        .exc { fill: #3498db; }\n";
    oss << "        .inh { fill: #2ecc71; }\n";
    oss << "        .text { fill: white; font-weight: bold; text-anchor: middle; }\n";
    oss << "        .container { display: flex; flex-wrap: wrap; }\n";
    oss << "        .neuron-box { margin: 10px; padding: 10px; border: 1px solid #ddd; border-radius: 5px; }\n";
    oss << "    </style>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << "    <h1>NLM Neural Network Visualization</h1>\n";
    oss << "    <p>Generated at: " 
        << std::chrono::duration<double>
           (std::chrono::high_resolution_clock::now().time_since_epoch()).count()
        << "</p>\n";
    
    if (!activityHistory_.empty()) {
        const auto& snapshot = activityHistory_.back();
        
        oss << "    <h2>Network Statistics</h2>\n";
        oss << "    <ul>\n";
        oss << "        <li>Total Neurons: " << snapshot.neuronActivities.size() << "</li>\n";
        oss << "        <li>Total Synapses: " << snapshot.activeSynapseCount << "</li>\n";
        oss << "        <li>Average Firing Rate: " << snapshot.averageFiringRate << " Hz</li>\n";
        oss << "        <li>Total Spikes: " << snapshot.totalSpikes << "</li>\n";
        oss << "        <li>Firing Neurons: " << snapshot.firingNeuronCount << "</li>\n";
        oss << "        <li>Excitation/Inhibition Ratio: " << snapshot.excitationInhibitionRatio << "</li>\n";
        oss << "    </ul>\n";
        
        oss << "    <h2>Neuron Activity</h2>\n";
        oss << "    <div class=\"container\">\n";
        for (size_t i = 0; i < std::min(size_t(10), snapshot.neuronActivities.size()); ++i) {
            const auto& neuron = snapshot.neuronActivities[i];
            oss << "        <div class=\"neuron-box\">\n";
            oss << "            <svg width=\"50\" height=\"50\">\n";
            oss << "                <circle class=\"neuron" << (neuron.firingRate > 0.1 ? " active" : "") 
                << " " << (neuron.type == NeuronType::Excitatory ? "exc" : 
                           neuron.type == NeuronType::Inhibitory ? "inh" : "")
                << "\" cx=\"25\" cy=\"25\" r=\"20\"/>\n";
            oss << "                <text class=\"text\" x=\"25\" y=\"30\" dy\".3em\">" << neuron.neuronId.value << "</text>\n";
            oss << "            </svg>\n";
            oss << "            <p>Firing Rate: " << neuron.firingRate << " Hz</p>\n";
            oss << "            <p>Membrane Potential: " << neuron.membranePotential << " mV</p>\n";
            oss << "        </div>\n";
        }
        oss << "    </div>\n";
    }
    
    oss << "</body>\n";
    oss << "</html>\n";
    
    return oss.str();
}

std::string EnhancedVisualizer::NeuralActivityVisualizer::generateCSVActivityVisualization() const {
    std::ostringstream oss;
    oss << "step,time,total_spikes,average_firing_rate,excitation_inhibition_ratio,firing_neuron_count,active_synapse_count\n";
    
    for (const auto& snapshot : activityHistory_) {
        oss << snapshot.step << ","
            << std::fixed << std::setprecision(6) << snapshot.time << ","
            << snapshot.totalSpikes << ","
            << snapshot.averageFiringRate << ","
            << snapshot.excitationInhibitionRatio << ","
            << snapshot.firingNeuronCount << ","
            << snapshot.activeSynapseCount << "\n";
    }
    
    return oss.str();
}

std::string EnhancedVisualizer::NeuralActivityVisualizer::generateMatrixVisualization() const {
    std::ostringstream oss;
    oss << "Connectivity Matrix:\n";
    
    if (!activityHistory_.empty() && !activityHistory_.back().connectivityMatrix.empty()) {
        const auto& matrix = activityHistory_.back().connectivityMatrix;
        size_t size = matrix.size();
        
        for (size_t i = 0; i < size; ++i) {
            for (size_t j = 0; j < size; ++j) {
                oss << std::setw(8) << matrix[i][j];
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::vector<float> EnhancedVisualizer::NeuralActivityVisualizer::extractFiringRates(
    const BrainActivitySnapshot& snapshot) const {
    std::vector<float> rates;
    for (const auto& neuron : snapshot.neuronActivities) {
        rates.push_back(neuron.firingRate);
    }
    return rates;
}

std::vector<float> EnhancedVisualizer::NeuralActivityVisualizer::extractMembranePotentials(
    const BrainActivitySnapshot& snapshot) const {
    std::vector<float> potentials;
    for (const auto& neuron : snapshot.neuronActivities) {
        potentials.push_back(neuron.membranePotential);
    }
    return potentials;
}

std::vector<float> EnhancedVisualizer::NeuralActivityVisualizer::extractSynapticWeights(
    const BrainActivitySnapshot& snapshot) const {
    std::vector<float> weights;
    
    if (!snapshot.connectivityMatrix.empty()) {
        size_t size = snapshot.connectivityMatrix.size();
        for (size_t i = 0; i < size; ++i) {
            for (size_t j = 0; j < size; ++j) {
                weights.push_back(snapshot.connectivityMatrix[i][j]);
            }
        }
    }
    
    return weights;
}

py::dict EnhancedVisualizer::NeuralActivityVisualizer::getConnectivityAnalysis() const {
    py::dict analysis;
    
    if (activityHistory_.empty()) {
        return analysis;
    }
    
    // Calculate basic network properties
    const auto& snapshot = activityHistory_.back();
    size_t numNeurons = snapshot.neuronActivities.size();
    size_t numConnections = snapshot.activeSynapseCount;
    
    analysis["total_neurons"] = numNeurons;
    analysis["total_connections"] = numConnections;
    analysis["connection_density"] = (numNeurons > 1) ? (numConnections * 2.0f) / (numNeurons * (numNeurons - 1)) : 0.0f;
    
    // Calculate firing statistics
    std::vector<float> firingRates;
    for (const auto& neuron : snapshot.neuronActivities) {
        firingRates.push_back(neuron.firingRate);
    }
    
    if (!firingRates.empty()) {
        float sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0f);
        float mean = sum / firingRates.size();
        
        float variance = 0.0f;
        for (float rate : firingRates) {
            variance += (rate - mean) * (rate - mean);
        }
        variance /= firingRates.size();
        
        float stddev = std::sqrt(variance);
        
        analysis["mean_firing_rate"] = mean;
        analysis["std_firing_rate"] = stddev;
        analysis["firing_coefficient_of_variation"] = (mean > 0.0f) ? stddev / mean : 0.0f;
    }
    
    return analysis;
}

py::dict EnhancedVisualizer::NeuralActivityVisualizer::getFiringStatistics() const {
    py::dict stats;
    
    if (activityHistory_.empty()) {
        return stats;
    }
    
    const auto& snapshot = activityHistory_.back();
    
    stats["total_spikes"] = snapshot.totalSpikes;
    stats["firing_neuron_count"] = snapshot.firingNeuronCount;
    stats["average_firing_rate"] = snapshot.averageFiringRate;
    stats["excitation_inhibition_ratio"] = snapshot.excitationInhibitionRatio;
    
    return stats;
}

py::dict EnhancedVisualizer::NeuralActivityVisualizer::getDevelopmentStatistics() const {
    py::dict stats;
    
    if (activityHistory_.empty()) {
        return stats;
    }
    
    const auto& snapshot = activityHistory_.back();
    
    // Calculate development-related metrics
    stats["development_progress"] = 0.5f; // Placeholder
    stats["network_maturity"] = snapshot.excitationInhibitionRatio;
    stats["activity_stability"] = 0.8f; // Placeholder - would calculate from time series
    
    return stats;
}

std::vector<RegionId> EnhancedVisualizer::NeuralActivityVisualizer::findActiveRegions(
    std::shared_ptr<Brain> brain) const {
    std::vector<RegionId> activeRegions;
    
    if (!brain) {
        return activeRegions;
    }
    
    auto regions = brain->getRegions();
    for (const auto& region : regions) {
        // Check if region has significant activity
        auto neurons = region->getAllNeurons();
        size_t activeNeurons = 0;
        
        for (const auto& neuron : neurons) {
            if (neuron->isFiring()) {
                activeNeurons++;
            }
        }
        
        if (static_cast<float>(activeNeurons) / neurons.size() > 0.1f) { // 10% threshold
            activeRegions.push_back(region->getId());
        }
    }
    
    return activeRegions;
}

py::dict EnhancedVisualizer::NeuralActivityVisualizer::analyzeNeuralEnsembles(
    std::shared_ptr<Brain> brain) const {
    py::dict ensembleData;
    
    if (!brain) {
        return ensembleData;
    }
    
    // Find correlated neuron groups (simplified)
    auto regions = brain->getRegions();
    
    py::list ensembles;
    for (size_t i = 0; i < std::min(size_t(5), regions.size()); ++i) {
        py::dict ensemble;
        
        auto neurons = regions[i]->getAllNeurons();
        size_t numNeurons = neurons.size();
        
        ensemble["region_id"] = regions[i]->getId().value;
        ensemble["neuron_count"] = numNeurons;
        ensemble["activity_level"] = static_cast<float>(i + 1) / regions.size();
        
        // Calculate ensemble coherence (simplified)
        float coherence = 0.0f;
        if (numNeurons > 1) {
            coherence = 0.5f + (static_cast<float>(i) / regions.size()) * 0.5f;
        }
        
        ensemble["coherence"] = coherence;
        
        ensembles.append(ensemble);
    }
    
    ensembleData["ensembles"] = ensembles;
    ensembleData["total_ensembles"] = static_cast<int>(ensembles.size());
    
    return ensembleData;
}

bool EnhancedVisualizer::NeuralActivityVisualizer::exportToFile(
    const std::string& filepath, OutputFormat format) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string data;
    switch (format) {
        case OutputFormat::CSV:
            data = generateCSVActivityVisualization();
            break;
        case OutputFormat::JSON:
            data = generateJSONNetworkVisualization();
            break;
        case OutputFormat::HTML:
            data = generateHTMLNetworkVisualization();
            break;
        case OutputFormat::PLAINTEXT:
            data = generateMatrixVisualization();
            break;
        default:
            data = "Unsupported format";
    }
    
    file << data;
    return true;
}

void EnhancedVisualizer::NeuralActivityVisualizer::clear() {
    std::lock_guard<std::mutex> lock(historyMutex_);
    activityHistory_.clear();
}

std::string EnhancedVisualizer::NeuralActivityVisualizer::getSummary() const {
    std::ostringstream oss;
    oss << "Neural Activity Visualizer: ";
    
    if (activityHistory_.empty()) {
        oss << "No data available";
    } else {
        oss << "Showing " << activityHistory_.size() << " snapshots\n";
        
        const auto& latest = activityHistory_.back();
        oss << "Latest: Step " << latest.step << ", Time " 
            << std::fixed << std::setprecision(3) << latest.time << "\n";
        oss << "Neurons: " << latest.neuronActivities.size() << "\n";
        oss << "Firing Rate: " << latest.averageFiringRate << " Hz\n";
        oss << "Total Spikes: " << latest.totalSpikes << "\n";
    }
    
    return oss.str();
}

// Implementation of EnhancedVisualizer main interface
EnhancedVisualizer::EnhancedVisualizer() : simulationStep_(0) {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    timestamps_.push_back(std::chrono::duration<double>(now.time_since_epoch()).count());
}

EnhancedVisualizer::NeuralActivityVisualizer EnhancedVisualizer::createNeuralActivityVisualizer(
    size_t maxHistorySize) {
    return NeuralActivityVisualizer(maxHistorySize);
}

EnhancedVisualizer::LearningProgressTracker EnhancedVisualizer::createLearningProgressTracker() {
    return LearningProgressTracker();
}

EnhancedVisualizer::PerformanceProfiler EnhancedVisualizer::createPerformanceProfiler() {
    return PerformanceProfiler();
}

EnhancedVisualizer::DebugConsole EnhancedVisualizer::createDebugConsole() {
    return DebugConsole();
}

void EnhancedVisualizer::runComprehensiveVisualization(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<AgentBrain> agent,
    std::shared_ptr<SimpleWorld> world,
    size_t numSteps,
    double timestep) {
    
    for (size_t step = 0; step < numSteps; ++step) {
        double time = step * timestep;
        timestamps_.push_back(time);
        simulationStep_ = step;
        
        // Execute simulation step
        if (brain) {
            brain->step(step, time);
        }
        
        if (world) {
            world->update(timestep);
        }
        
        // Capture data
        if (brain) {
            auto brainSnapshot = neuralVisualizer_.captureBrainActivity(brain);
            brainSnapshot.step = step;
            brainSnapshot.time = time;
            neuralVisualizer_.addActivity(brainSnapshot);
        }
        
        // Record learning metrics
        LearningProgressTracker::LearningMetrics learningMetrics(step, time);
        if (agent) {
            learningMetrics.learningRate = agent->getCuriosityLevel();
            learningMetrics.performance = agent->getNeuromodulationLevel();
            learningMetrics.curiosityLevel = agent->getCuriosityLevel();
            learningMetrics.noveltyLevel = agent->getNoveltyLevel();
            learningMetrics.predictionError = agent->getPredictionError();
        }
        
        learningTracker_.recordMetrics(learningMetrics);
        
        // Record performance metrics
        PerformanceProfiler::PerformanceMetrics perfMetrics(step, time);
        perfMetrics.simulationTime = 0.1f; // Placeholder
        perfMetrics.brainStepTime = 0.05f; // Placeholder
        perfMetrics.totalSynapseCount = brain ? brain->getTotalSynapseCount() : 0;
        perfMetrics.totalNeuronCount = brain ? brain->getTotalNeuronCount() : 0;
        perfMetrics.spikeEventsProcessed = brain ? brain->getTotalSpikeCount() : 0;
        
        profiler_.recordMetrics(perfMetrics);
        
        // Debug output
        debugConsole_.log("Step " + std::to_string(step) + ": Simulation completed");
    }
}

std::string EnhancedVisualizer::generateIntegratedReport(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<AgentBrain> agent,
    std::shared_ptr<SimpleWorld> world) {
    
    std::ostringstream oss;
    oss << "=== NLM Enhanced Visualization Report ===\n";
    oss << "Generated at: " 
        << std::chrono::duration<double>
           (std::chrono::high_resolution_clock::now().time_since_epoch()).count()
        << "\n\n";
    
    oss << "--- Neural Activity Summary ---\n";
    oss << neuralVisualizer_.getSummary() << "\n\n";
    
    oss << "--- Learning Progress ---\n";
    oss << learningTracker_.getSummary() << "\n\n";
    
    oss << "--- Performance Summary ---\n";
    oss << profiler_.getPerformanceStatistics().attr("summary").cast<std::string>() << "\n\n";
    
    oss << "--- Debug Log ---\n";
    auto debugLog = debugConsole_.getDebugLog();
    for (const auto& entry : debugLog) {
        oss << "[" << entry << "]\n";
    }
    
    oss << "\n=== End Report ===";
    
    return oss.str();
}

bool EnhancedVisualizer::exportAllData(const std::string& outputDirectory) {
    std::string dir = outputDirectory;
    if (!dir.empty() && dir.back() != '/') {
        dir += '/';
    }
    
    bool success = true;
    
    // Export neural activity data
    if (!neuralVisualizer_.exportToFile(dir + "neural_activity.csv", NeuralActivityVisualizer::OutputFormat::CSV)) {
        debugConsole_.log("Failed to export neural activity data", "ERROR");
        success = false;
    }
    
    // Export learning metrics
    if (!learningTracker_.exportMetrics(dir + "learning_metrics.json")) {
        debugConsole_.log("Failed to export learning metrics", "ERROR");
        success = false;
    }
    
    // Export performance data
    if (!profiler_.exportToFile(dir + "performance_data.csv")) {
        debugConsole_.log("Failed to export performance data", "ERROR");
        success = false;
    }
    
    return success;
}

py::dict EnhancedVisualizer::getVisualizationSummary() const {
    py::dict summary;
    
    summary["neural_activity_snapshots"] = static_cast<int>(neuralVisualizer_.getActivityHistory().size());
    summary["learning_metrics_points"] = static_cast<int>(learningTracker_.getLearningHistory().size());
    summary["performance_samples"] = static_cast<int>(profiler_.getPerformanceHistory().size());
    summary["debug_log_entries"] = static_cast<int>(debugConsole_.getDebugLog().size());
    
    // Add neural activity statistics
    auto neuralStats = neuralVisualizer_.getConnectivityAnalysis();
    for (auto item : neuralStats) {
        summary["neural_" + py::str(item.first)] = item.second;
    }
    
    // Add performance statistics
    auto perfStats = profiler_.getPerformanceStatistics();
    for (auto item : perfStats) {
        summary["performance_" + py::str(item.first)] = item.second;
    }
    
    return summary;
}

void EnhancedVisualizer::reset() {
    neuralVisualizer_.clear();
    learningTracker_.clear();
    profiler_.clear();
    debugConsole_.clearDebugLog();
    
    simulationStep_ = 0;
    timestamps_.clear();
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    timestamps_.push_back(std::chrono::duration<double>(now.time_since_epoch()).count());
}

// Implementation of other EnhancedVisualizer components (LearningProgressTracker, PerformanceProfiler, DebugConsole)
// would continue here...

} // namespace visualization
} // namespace nlm
