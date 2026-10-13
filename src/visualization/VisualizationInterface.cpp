// Network Visualization System for NLM
// Enhanced visualization capabilities for Phase 6 integration

#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <fstream>

namespace nlm {

class NetworkVisualizer {
public:
    struct VisualizationOptions {
        bool showNeurons = true;
        bool showConnections = true;
        bool showSpikes = true;
        bool showWeights = true;
        bool showActivity = true;
        double neuronSize = 1.0;
        double connectionThickness = 0.5;
        std::string colorScheme = "default";
        bool exportData = false;
        std::string outputFile = "visualization.png";
        std::string dataFile = "visualization_data.json";
    };
    
    struct NeuronVisualization {
        NeuronId id;
        NeuronType type;
        double x, y;
        double radius;
        std::string color;
        double activityLevel;
        size_t spikeCount;
        bool active;
        
        NeuronVisualization(NeuronId nid, NeuronType ntype, double rx, double ry, double r)
            : id(nid), type(ntype), x(rx), y(ry), radius(r), activityLevel(0.0), 
              spikeCount(0), active(false) {
            color = typeToColor(type);
        }
        
        std::string typeToColor(NeuronType type) const {
            switch (type) {
                case NeuronType::Excitatory: return "#FF6B6B";
                case NeuronType::Inhibitory: return "#4ECDC4";
                case NeuronType::Modulatory: return "#45B7D1";
                case NeuronType::Sensory: return "#96CEB4";
                case NeuronType::Motor: return "#FFEAA7";
                case NeuronType::Internal: return "#DDA0DD";
                default: return "#CCCCCC";
            }
        }
    };
    
    struct ConnectionVisualization {
        NeuronId source;
        NeuronId target;
        double weight;
        double thickness;
        std::string color;
        bool active;
        
        ConnectionVisualization(NeuronId src, NeuronId tgt, double w)
            : source(src), target(tgt), weight(w), thickness(0.5), active(false) {
            color = weightToColor(w);
        }
        
        std::string weightToColor(double weight) const {
            if (weight > 0.5) return "#FF4444";
            else if (weight > 0.0) return "#FF8844";
            else if (weight < -0.5) return "#4444FF";
            else if (weight < 0.0) return "#4488FF";
            else return "#888888";
        }
    };
    
    struct ActivityData {
        std::vector<double> neuronActivities;
        std::vector<size_t> neuronSpikeCounts;
        std::vector<std::vector<double>> weightMatrix;
        std::chrono::steady_clock::time_point timestamp;
        
        ActivityData(size_t neuronCount) {
            neuronActivities.resize(neuronCount, 0.0);
            neuronSpikeCounts.resize(neuronCount, 0);
            weightMatrix.resize(neuronCount, std::vector<double>(neuronCount, 0.0));
            timestamp = std::chrono::steady_clock::now();
        }
    };
    
    NetworkVisualizer() = default;
    
    bool initialize() {
        // Initialize visualization systems
        active_ = true;
        lastUpdateTime_ = std::chrono::steady_clock::now();
        return true;
    }
    
    void update(const Brain& brain, double timestep) {
        if (!active_) return;
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastUpdateTime_).count();
        
        if (elapsed >= 1.0 / options_.updateRate) {
            updateNetworkVisualization(brain);
            updateActivityData(brain);
            lastUpdateTime_ = now;
            
            if (options_.exportData) {
                exportData();
            }
        }
    }
    
    void render() {
        if (!active_) return;
        
        // Generate visualization output
        std::string visualization = generateVisualization();
        
        // Output based on configuration
        if (options_.outputFile.empty()) {
            // Console output for debugging
            std::cout << "Visualization generated at " << getTimestamp() << "\n";
            std::cout << visualization.substr(0, 500) << "...\n";
        } else {
            // File output
            std::ofstream file(options_.outputFile);
            if (file.is_open()) {
                file << visualization;
            }
        }
    }
    
    void close() {
        active_ = false;
        activityData_.clear();
    }
    
    bool isActive() const { return active_; }
    
    void setOptions(const VisualizationOptions& opts) { options_ = opts; }
    const VisualizationOptions& getOptions() const { return options_; }
    
    // Visualization generation methods
    void visualizeNetwork(const Brain& brain) {
        generateNetworkLayout(brain);
        generateConnections(brain);
        generateNeuronLabels(brain);
    }
    
    void visualizeActivity(const Brain& brain) {
        updateActivityLevels(brain);
        generateActivityHeatmap(brain);
        generateSpikeRaster(brain);
    }
    
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
        generateSpikeVisualization(spikes);
    }
    
    void visualizeWeights(const Brain& brain) {
        generateWeightMatrixVisualization(brain);
    }
    
    // Data export methods
    void exportData() {
        if (activityData_.empty()) return;
        
        std::string jsonData = generateJSONData();
        std::ofstream file(options_.dataFile);
        if (file.is_open()) {
            file << jsonData;
        }
    }
    
    // Statistics and analysis
    double getNetworkDensity() const {
        if (neurons_.empty() || connections_.empty()) return 0.0;
        return static_cast<double>(connections_.size()) / 
               (static_cast<double>(neurons_.size()) * neurons_.size());
    }
    
    double getAverageWeight() const {
        if (connections_.empty()) return 0.0;
        double total = 0.0;
        for (const auto& conn : connections_) {
            total += conn.weight;
        }
        return total / connections_.size();
    }
    
    size_t getActiveNeuronCount() const {
        size_t count = 0;
        for (const auto& neuron : neurons_) {
            if (neuron.active) count++;
        }
        return count;
    }
    
    double getAverageActivityLevel() const {
        if (neurons_.empty()) return 0.0;
        double total = 0.0;
        for (const auto& neuron : neurons_) {
            total += neuron.activityLevel;
        }
        return total / neurons_.size();
    }
    
private:
    // Private helper methods
    void updateNetworkVisualization(const Brain& brain) {
        neurons_.clear();
        connections_.clear();
        
        // Generate network layout using force-directed algorithm
        generateForceDirectedLayout(brain);
        
        // Create neuron visualizations
        createNeuronVisualizations(brain);
        
        // Create connection visualizations
        createConnectionVisualizations(brain);
    }
    
    void generateForceDirectedLayout(const Brain& brain) {
        // Simple force-directed layout for demonstration
        size_t neuronCount = brain.getTotalNeuronCount();
        double width = 800.0;
        double height = 600.0;
        
        for (size_t i = 0; i < neuronCount; ++i) {
            double x = (static_cast<double>(rand()) / RAND_MAX) * width;
            double y = (static_cast<double>(rand()) / RAND_MAX) * height;
            neurons_.emplace_back(NeuronId(i), NeuronType::Internal, x, y, 5.0);
        }
    }
    
    void createNeuronVisualizations(const Brain& brain) {
        for (const auto& region : brain.getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                for (Neuron* neuron : pop->getNeurons()) {
                    neurons_.emplace_back(neuron->getId(), type, 
                                         neuron->getMembranePotential(), 
                                         neuron->getMembranePotential(), 5.0);
                    neurons_.back().activityLevel = calculateNeuronActivity(neuron);
                    neurons_.back().active = isNeuronActive(neuron);
                }
            }
        }
    }
    
    void createConnectionVisualizations(const Brain& brain) {
        // Create connection visualizations based on brain connectivity
        for (const auto& region : brain.getRegions()) {
            for (const auto& synapse : region->getSynapses()) {
                connections_.emplace_back(synapse->getSourceNeuron(), 
                                         synapse->getDestinationNeuron(), 
                                         synapse->getWeight());
                connections_.back().active = isConnectionActive(synapse);
            }
        }
    }
    
    void updateActivityData(const Brain& brain) {
        ActivityData data(brain.getTotalNeuronCount());
        
        // Update activity levels
        for (size_t i = 0; i < brain.getTotalNeuronCount(); ++i) {
            // This is a simplified activity calculation
            // In a real implementation, this would use actual brain state
            data.neuronActivities[i] = (static_cast<double>(rand()) / RAND_MAX) * 0.8 + 0.1;
            data.neuronSpikeCounts[i] = (static_cast<size_t>(rand()) % 10);
        }
        
        // Update weight matrix (simplified)
        for (size_t i = 0; i < brain.getTotalNeuronCount(); ++i) {
            for (size_t j = 0; j < brain.getTotalNeuronCount(); ++j) {
                data.weightMatrix[i][j] = (static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0;
            }
        }
        
        activityData_.push_back(std::move(data));
        
        // Keep only last 10 seconds of data
        auto cutoff = std::chrono::steady_clock::now() - std::chrono::seconds(10);
        while (!activityData_.empty() && activityData_.front().timestamp < cutoff) {
            activityData_.pop_front();
        }
    }
    
    std::string generateVisualization() {
        std::stringstream ss;
        ss << "NLM Network Visualization\n";
        ss << "===========================\n\n";
        
        ss << "Network Statistics:\n";
        ss << "  - Neurons: " << neurons_.size() << "\n";
        ss << "  - Connections: " << connections_.size() << "\n";
        ss << "  - Network Density: " << getNetworkDensity() << "\n";
        ss << "  - Average Weight: " << getAverageWeight() << "\n";
        ss << "  - Active Neurons: " << getActiveNeuronCount() << "\n";
        ss << "  - Average Activity: " << getAverageActivityLevel() << "\n\n";
        
        ss << "Neuron Types:\n";
        std::unordered_map<NeuronType, size_t> typeCount;
        for (const auto& neuron : neurons_) {
            typeCount[neuron.type]++;
        }
        for (const auto& [type, count] : typeCount) {
            ss << "  - " << neuronTypeToString(type) << ": " << count << "\n";
        }
        
        ss << "\nConnection Weights:\n";
        ss << "  - Positive: " << countConnectionsByWeight(1.0, std::numeric_limits<double>::max()) << "\n";
        ss << "  - Negative: " << countConnectionsByWeight(std::numeric_limits<double>::min(), -1.0) << "\n";
        ss << "  - Zero: " << countConnectionsByWeight(-0.1, 0.1) << "\n";
        
        return ss.str();
    }
    
    std::string generateJSONData() {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"timestamp\": \"" << getTimestamp() << "\",\n";
        ss << "  \"neurons\": [";
        
        for (size_t i = 0; i < neurons_.size(); ++i) {
            const auto& neuron = neurons_[i];
            ss << "{\"id\": " << neuron.id.value << ", \"type\": \" << neuronTypeToString(neuron.type)
               << "\", \"x\": " << neuron.x << ", \"y\": " << neuron.y
               << ", \"activity\": " << neuron.activityLevel
               << ", \"active\": " << (neuron.active ? "true" : "false") << "}";
            if (i < neurons_.size() - 1) ss << ", ";
        }
        
        ss << "],\n";
        ss << "  \"connections\": [";
        
        for (size_t i = 0; i < connections_.size(); ++i) {
            const auto& conn = connections_[i];
            ss << "{\"source\": " << conn.source.value << ", \"target\": " << conn.target.value
               << ", \"weight\": " << conn.weight << ", \"active\": " << (conn.active ? "true" : "false") << "}";
            if (i < connections_.size() - 1) ss << ", ";
        }
        
        ss << "],\n";
        ss << "  \"statistics\": {";
        ss << "\"density\": " << getNetworkDensity() << ",\n";
        ss << "\"avg_weight\": " << getAverageWeight() << ",\n";
        ss << "\"active_neurons\": " << getActiveNeuronCount() << ",\n";
        ss << "\"avg_activity\": " << getAverageActivityLevel() << "\n";
        ss << "}\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string neuronTypeToString(NeuronType type) const {
        switch (type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    }
    
    size_t countConnectionsByWeight(double min, double max) const {
        size_t count = 0;
        for (const auto& conn : connections_) {
            if (conn.weight >= min && conn.weight <= max) count++;
        }
        return count;
    }
    
    double calculateNeuronActivity(Neuron* neuron) const {
        // Simplified activity calculation
        return std::abs(neuron->getMembranePotential() - neuron->getThreshold()) / 100.0;
    }
    
    bool isNeuronActive(Neuron* neuron) const {
        // Simplified activity check
        return neuron->getState().firingState == FiringState::Active;
    }
    
    bool isConnectionActive(Synapse* synapse) const {
        // Simplified connection activity check
        return synapse->getWeight() != 0.0;
    }
    
    void generateNetworkLayout(const Brain& brain) {
        // Placeholder for network layout generation
    }
    
    void generateConnections(const Brain& brain) {
        // Placeholder for connection visualization
    }
    
    void generateNeuronLabels(const Brain& brain) {
        // Placeholder for neuron label generation
    }
    
    void updateActivityLevels(const Brain& brain) {
        // Placeholder for activity level updates
    }
    
    void generateActivityHeatmap(const Brain& brain) {
        // Placeholder for activity heatmap generation
    }
    
    void generateSpikeRaster(const Brain& brain) {
        // Placeholder for spike raster plot generation
    }
    
    void generateWeightMatrixVisualization(const Brain& brain) {
        // Placeholder for weight matrix visualization
    }
    
    void generateSpikeVisualization(const std::vector<SpikeEvent>& spikes) {
        // Placeholder for spike visualization
    }
    
    void generateForceDirectedLayout(const Brain& brain) {
        // Placeholder for force-directed layout
    }
    
    void createNeuronVisualizations(const Brain& brain) {
        // Placeholder for neuron visualization creation
    }
    
    void createConnectionVisualizations(const Brain& brain) {
        // Placeholder for connection visualization creation
    }
    
    void updateActivityData(const Brain& brain) {
        // Placeholder for activity data updates
    }
    
    std::string generateVisualization() const {
        // Placeholder for visualization generation
        return "Visualization not implemented yet.";
    }
    
    std::string generateJSONData() const {
        // Placeholder for JSON data generation
        return "{\"error\": \"JSON generation not implemented\"}";
    }
    
    void generateNetworkLayout(const Brain& brain) {
        // Placeholder for network layout
    }
    
    void generateConnections(const Brain& brain) {
        // Placeholder for connection visualization
    }
    
    void generateNeuronLabels(const Brain& brain) {
        // Placeholder for neuron label generation
    }
    
    void updateActivityLevels(const Brain& brain) {
        // Placeholder for activity level updates
    }
    
    void generateActivityHeatmap(const Brain& brain) {
        // Placeholder for activity heatmap generation
    }
    
    void generateSpikeRaster(const Brain& brain) {
        // Placeholder for spike raster plot generation
    }
    
    void generateWeightMatrixVisualization(const Brain& brain) {
        // Placeholder for weight matrix visualization
    }
    
    void generateSpikeVisualization(const std::vector<SpikeEvent>& spikes) {
        // Placeholder for spike visualization
    }
    
    void generateForceDirectedLayout(const Brain& brain) {
        // Placeholder for force-directed layout
    }
    
    void createNeuronVisualizations(const Brain& brain) {
        // Placeholder for neuron visualization creation
    }
    
    void createConnectionVisualizations(const Brain& brain) {
        // Placeholder for connection visualization creation
    }
    
    void updateActivityData(const Brain& brain) {
        // Placeholder for activity data updates
    }
    
    // Helper types
    using ActivityDataDeque = std::deque<ActivityData>;
    
    bool active_ = false;
    double lastUpdateTime_ = 0.0;
    VisualizationOptions options_;
    std::vector<NeuronVisualization> neurons_;
    std::vector<ConnectionVisualization> connections_;
    ActivityDataDeque activityData_;
};

// Global visualization instance
static std::unique_ptr<NetworkVisualizer> g_visualizer;

// Public API functions
bool initializeVisualization() {
    if (!g_visualizer) {
        g_visualizer = std::make_unique<NetworkVisualizer>();
    }
    return g_visualizer->initialize();
}

void updateVisualization(const Brain& brain, double timestep) {
    if (g_visualizer) {
        g_visualizer->update(brain, timestep);
    }
}

void renderVisualization() {
    if (g_visualizer) {
        g_visualizer->render();
    }
}

void closeVisualization() {
    if (g_visualizer) {
        g_visualizer->close();
        g_visualizer.reset();
    }
}

bool isVisualizationActive() {
    return g_visualizer && g_visualizer->isActive();
}

void setVisualizationOptions(const NetworkVisualizer::VisualizationOptions& opts) {
    if (g_visualizer) {
        g_visualizer->setOptions(opts);
    }
}

const NetworkVisualizer::VisualizationOptions& getVisualizationOptions() {
    static NetworkVisualizer::VisualizationOptions defaultOpts;
    if (g_visualizer) {
        return g_visualizer->getOptions();
    }
    return defaultOpts;
}

void visualizeNetwork(const Brain& brain) {
    if (g_visualizer) {
        g_visualizer->visualizeNetwork(brain);
    }
}

void visualizeActivity(const Brain& brain) {
    if (g_visualizer) {
        g_visualizer->visualizeActivity(brain);
    }
}

void visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (g_visualizer) {
        g_visualizer->visualizeSpikes(spikes);
    }
}

void visualizeWeights(const Brain& brain) {
    if (g_visualizer) {
        g_visualizer->visualizeWeights(brain);
    }
}

// Python bindings for visualization
PYBIND11_MODULE(pynlm_visualization, m) {
    m.doc() = R"pbdoc(
        NLM Visualization Module
        ---------------------------
        Enhanced visualization capabilities for the NLM neural simulation framework.
        Provides network visualization, activity monitoring, and data export.
    )pbdoc";
    
    py::class_<NetworkVisualizer::VisualizationOptions>(m, "VisualizationOptions")
        .def(py::init<>())
        .def_readwrite("showNeurons", &NetworkVisualizer::VisualizationOptions::showNeurons)
        .def_readwrite("showConnections", &NetworkVisualizer::VisualizationOptions::showConnections)
        .def_readwrite("showSpikes", &NetworkVisualizer::VisualizationOptions::showSpikes)
        .def_readwrite("showWeights", &NetworkVisualizer::VisualizationOptions::showWeights)
        .def_readwrite("showActivity", &NetworkVisualizer::VisualizationOptions::showActivity)
        .def_readwrite("neuronSize", &NetworkVisualizer::VisualizationOptions::neuronSize)
        .def_readwrite("connectionThickness", &NetworkVisualizer::VisualizationOptions::connectionThickness)
        .def_readwrite("colorScheme", &NetworkVisualizer::VisualizationOptions::colorScheme)
        .def_readwrite("exportData", &NetworkVisualizer::VisualizationOptions::exportData)
        .def_readwrite("outputFile", &NetworkVisualizer::VisualizationOptions::outputFile)
        .def_readwrite("dataFile", &NetworkVisualizer::VisualizationOptions::dataFile);
    
    m.def("initializeVisualization", &initializeVisualization, "Initialize visualization system");
    m.def("updateVisualization", &updateVisualization, py::arg("brain"), py::arg("timestep"), 
          "Update visualization with current brain state");
    m.def("renderVisualization", &renderVisualization, "Render visualization");
    m.def("closeVisualization", &closeVisualization, "Close visualization system");
    m.def("isVisualizationActive", &isVisualizationActive, "Check if visualization is active");
    m.def("setVisualizationOptions", &setVisualizationOptions, py::arg("opts"), 
          "Set visualization options");
    m.def("getVisualizationOptions", &getVisualizationOptions, 
          "Get current visualization options");
    m.def("visualizeNetwork", &visualizeNetwork, py::arg("brain"), "Visualize network structure");
    m.def("visualizeActivity", &visualizeActivity, py::arg("brain"), "Visualize network activity");
    m.def("visualizeSpikes", &visualizeSpikes, py::arg("spikes"), "Visualize spike activity");
    m.def("visualizeWeights", &visualizeWeights, py::arg("brain"), "Visualize connection weights");
}

} // namespace nlm
