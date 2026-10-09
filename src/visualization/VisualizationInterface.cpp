#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <map>
#include <set>

namespace nlm {

class NetworkGraph {
public:
    struct Node {
        std::string id;
        float x, y;
        float size;
        std::string type; // "neuron", "synapse", "region"
        float activity;    // 0.0 to 1.0
        
        Node() : x(0), y(0), size(10), activity(0) {}
        Node(const std::string& id) : id(id), x(0), y(0), size(10), activity(0) {}
    };
    
    struct Edge {
        std::string source;
        std::string target;
        float weight;
        bool directed;
        std::string style; // "solid", "dashed", "dotted"
        
        Edge() : weight(1), directed(false) {}
        Edge(const std::string& src, const std::string& tgt) 
            : source(src), target(tgt), weight(1), directed(false) {}
    };
    
public:
    NetworkGraph() = default;
    
    void addNode(const Node& node) {
        nodes_[node.id] = node;
    }
    
    void addEdge(const Edge& edge) {
        edges_.push_back(edge);
    }
    
    const Node* getNode(const std::string& id) const {
        auto it = nodes_.find(id);
        return it != nodes_.end() ? &it->second : nullptr;
    }
    
    void updateNodeActivity(const std::string& id, float activity) {
        auto it = nodes_.find(id);
        if (it != nodes_.end()) {
            it->second.activity = std::max(0.0f, std::min(1.0f, activity));
        }
    }
    
    const std::vector<Node>& getNodes() const {
        std::vector<Node> sortedNodes;
        sortedNodes.reserve(nodes_.size());
        for (const auto& pair : nodes_) {
            sortedNodes.push_back(pair.second);
        }
        return sortedNodes;
    }
    
    const std::vector<Edge>& getEdges() const {
        return edges_;
    }
    
    std::vector<std::string> getNodeIds() const {
        std::vector<std::string> ids;
        ids.reserve(nodes_.size());
        for (const auto& pair : nodes_) {
            ids.push_back(pair.first);
        }
        return ids;
    }
    
    // Force-directed layout update
    void updateLayout(double timestep) {
        // Simple force-directed algorithm for organic layout
        std::map<std::string, std::pair<float, float>> forces;
        
        for (auto& pair : nodes_) {
            forces[pair.first] = {0.0f, 0.0f};
        }
        
        // Repulsion between all nodes
        for (size_t i = 0; i < edges_.size(); ++i) {
            const auto& edge = edges_[i];
            if (nodes_.find(edge.source) != nodes_.end() && 
                nodes_.find(edge.target) != nodes_.end()) {
                
                const auto& nodeA = nodes_[edge.source];
                const auto& nodeB = nodes_[edge.target];
                
                float dx = nodeA.x - nodeB.x;
                float dy = nodeA.y - nodeB.y;
                float dist = sqrt(dx * dx + dy * dy) + 0.1f;
                float force = 1000.0f / (dist * dist);
                
                forces[edge.source].first += force * dx / dist;
                forces[edge.source].second += force * dy / dist;
                forces[edge.target].first -= force * dx / dist;
                forces[edge.target].second -= force * dy / dist;
                
                // Attraction for connected nodes
                float attraction = edge.weight * 10.0f;
                forces[edge.source].first -= attraction * dx / dist;
                forces[edge.source].second -= attraction * dy / dist;
                forces[edge.target].first += attraction * dx / dist;
                forces[edge.target].second += attraction * dy / dist;
            }
        }
        
        // Apply forces
        for (auto& pair : nodes_) {
            auto& force = forces[pair.first];
            pair.second.x += force.first * timestep;
            pair.second.y += force.second * timestep;
            
            // Keep nodes within bounds
            pair.second.x = std::max(-500.0f, std::min(500.0f, pair.second.x));
            pair.second.y = std::max(-500.0f, std::min(500.0f, pair.second.y));
            
            // Dampen movement
            pair.second.x *= 0.9f;
            pair.second.y *= 0.9f;
        }
    }
    
    void clear() {
        nodes_.clear();
        edges_.clear();
    }
    
    size_t getNodeCount() const { return nodes_.size(); }
    size_t getEdgeCount() const { return edges_.size(); }
    
private:
    std::map<std::string, Node> nodes_;
    std::vector<Edge> edges_;
};

class ActivityPlotter {
public:
    struct TimeSeries {
        std::string name;
        std::vector<float> values;
        std::string color;
        bool visible = true;
        
        TimeSeries() : values(), color("") {}
        TimeSeries(const std::string& name) : name(name), color("") {}
    };
    
    struct SpikeData {
        double time;
        int neuronId;
        float amplitude;
        
        SpikeData() : time(0), neuronId(0), amplitude(0) {}
        SpikeData(double t, int id, float amp) : time(t), neuronId(id), amplitude(amp) {}
    };
    
public:
    ActivityPlotter() = default;
    
    void addTimeSeries(const std::string& name, const std::vector<float>& values, const std::string& color = "") {
        TimeSeries ts;
        ts.name = name;
        ts.values = values;
        ts.color = color;
        timeSeries_.push_back(ts);
    }
    
    void addTimeSeries(const TimeSeries& ts) {
        timeSeries_.push_back(ts);
    }
    
    void addSpike(double time, int neuronId, float amplitude = 1.0f) {
        SpikeData spike(time, neuronId, amplitude);
        spikes_.push_back(spike);
    }
    
    void clear() {
        timeSeries_.clear();
        spikes_.clear();
    }
    
    void setSeriesVisible(const std::string& name, bool visible) {
        for (auto& ts : timeSeries_) {
            if (ts.name == name) {
                ts.visible = visible;
                break;
            }
        }
    }
    
    const std::vector<TimeSeries>& getTimeSeries() const {
        return timeSeries_;
    }
    
    const std::vector<SpikeData>& getSpikes() const {
        return spikes_;
    }
    
    size_t getSpikeCount() const { return spikes_.size(); }
    
    // Find spike time for a specific neuron at or before given time
    double getLastSpikeTime(int neuronId, double beforeTime) const {
        double lastTime = -1.0;
        for (const auto& spike : spikes_) {
            if (spike.neuronId == neuronId && spike.time <= beforeTime) {
                lastTime = spike.time;
            }
        }
        return lastTime;
    }
    
    std::vector<double> getSpikeTimes(int neuronId) const {
        std::vector<double> times;
        for (const auto& spike : spikes_) {
            if (spike.neuronId == neuronId) {
                times.push_back(spike.time);
            }
        }
        return times;
    }
    
    float getMaxActivity() const {
        float max = 0.0f;
        for (const auto& ts : timeSeries_) {
            if (ts.visible) {
                for (float val : ts.values) {
                    max = std::max(max, val);
                }
            }
        }
        return max;
    }
    
    float getMinActivity() const {
        float min = 1.0f;
        for (const auto& ts : timeSeries_) {
            if (ts.visible) {
                for (float val : ts.values) {
                    min = std::min(min, val);
                }
            }
        }
        return min;
    }
    
private:
    std::vector<TimeSeries> timeSeries_;
    std::vector<SpikeData> spikes_;
};

class ConsoleVisualizer {
public:
    ConsoleVisualizer() = default;
    
    void printNetwork(NetworkGraph& network) {
        std::cout << "=== NLM Network Visualization ===" << std::endl;
        std::cout << "Nodes: " << network.getNodeCount() << ", Edges: " << network.getEdgeCount() << std::endl;
        std::cout << std::endl;
        
        // Print edges first
        std::cout << "Connections:" << std::endl;
        for (const auto& edge : network.getEdges()) {
            const auto* src = network.getNode(edge.source);
            const auto* tgt = network.getNode(edge.target);
            if (src && tgt) {
                std::cout << "  " << edge.source << " -> " << edge.target 
                         << " (weight: " << edge.weight << ")";
                if (!edge.directed) std::cout << " [undirected]";
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
        
        // Group nodes by type
        std::map<std::string, std::vector<NetworkGraph::Node>> nodesByType;
        for (const auto& node : network.getNodes()) {
            nodesByType[node.type].push_back(node);
        }
        
        for (const auto& pair : nodesByType) {
            std::cout << "Nodes of type " << pair.first << ":" << std::endl;
            for (const auto& node : pair.second) {
                std::cout << "  " << node.id 
                         << " (pos: " << node.x << ", " << node.y << ") "
                         << "activity: " << node.activity;
                if (node.activity > 0.7) std::cout << " [HIGH]";
                else if (node.activity > 0.3) std::cout << " [MEDIUM]";
                else std::cout << " [LOW]";
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    void printActivityPlot(const ActivityPlotter& plotter, size_t maxPoints = 50) {
        std::cout << "=== NLM Activity Visualization ===" << std::endl;
        
        if (plotter.getTimeSeries().empty() && plotter.getSpikeCount() == 0) {
            std::cout << "No activity data to display." << std::endl;
            return;
        }
        
        // Find global max for scaling
        float globalMax = 0.0f;
        for (const auto& ts : plotter.getTimeSeries()) {
            if (ts.visible) {
                for (float val : ts.values) {
                    globalMax = std::max(globalMax, val);
                }
            }
        }
        
        if (globalMax == 0.0f) globalMax = 1.0f;
        
        std::cout << "Time series (showing last " << maxPoints << " points):" << std::endl;
        
        // Print time series
        size_t startIndex = std::max(0, (int)plotter.getTimeSeries()[0].values.size() - (int)maxPoints);
        for (size_t i = startIndex; i < plotter.getTimeSeries()[0].values.size(); ++i) {
            std::cout << std::fixed << std::setprecision(2) << i << ": ";
            
            for (const auto& ts : plotter.getTimeSeries()) {
                if (ts.visible && i < ts.values.size()) {
                    float value = ts.values[i];
                    int barWidth = (int)((value / globalMax) * 50);
                    std::cout << "[";
                    for (int j = 0; j < barWidth; ++j) {
                        std::cout << "#";
                    }
                    std::cout << "]";
                }
                std::cout << " ";
            }
            std::cout << std::endl;
        }
        
        // Print spike information
        if (plotter.getSpikeCount() > 0) {
            std::cout << std::endl << "Spike events:" << std::endl;
            std::map<int, std::vector<double>> spikesByNeuron;
            for (const auto& spike : plotter.getSpikes()) {
                spikesByNeuron[spike.neuronId].push_back(spike.time);
            }
            
            for (const auto& pair : spikesByNeuron) {
                std::cout << "  Neuron " << pair.first << ": " << pair.second.size() << " spikes";
                if (pair.second.size() <= 10) {
                    std::cout << " at times: ";
                    for (double time : pair.second) {
                        std::cout << time << " ";
                    }
                } else {
                    std::cout << " from " << pair.second.front() 
                             << " to " << pair.second.back();
                }
                std::cout << std::endl;
            }
        }
    }
    
    void printWeightMatrix(const NetworkGraph& network) {
        std::cout << "=== NLM Weight Matrix Visualization ===" << std::endl;
        
        // Build adjacency list for easier access
        std::map<std::string, std::vector<std::pair<std::string, float>>> adjacency;
        
        for (const auto& edge : network.getEdges()) {
            adjacency[edge.source].push_back({edge.target, edge.weight});
        }
        
        std::cout << "Weight matrix (node -> target node, weight):" << std::endl;
        for (const auto& pair : adjacency) {
            std::cout << pair.first << " -> {";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                const auto& target = pair.second[i];
                std::cout << target.first << ":" << target.second;
                if (i < pair.second.size() - 1) std::cout << ", ";
            }
            std::cout << "}" << std::endl;
        }
        
        // Show connectivity statistics
        std::cout << std::endl << "Connectivity statistics:" << std::endl;
        std::cout << "  Total connections: " << network.getEdgeCount() << std::endl;
        
        if (!adjacency.empty()) {
            float totalWeight = 0.0f;
            float maxWeight = 0.0f;
            size_t maxOutgoing = 0;
            
            for (const auto& pair : adjacency) {
                float nodeWeight = 0.0f;
                for (const auto& target : pair.second) {
                    nodeWeight += target.second;
                }
                totalWeight += nodeWeight;
                maxWeight = std::max(maxWeight, nodeWeight);
                maxOutgoing = std::max(maxOutgoing, pair.second.size());
            }
            
            std::cout << "  Average outgoing weight: " << (totalWeight / adjacency.size()) << std::endl;
            std::cout << "  Max outgoing weight: " << maxWeight << std::endl;
            std::cout << "  Max outgoing connections: " << maxOutgoing << std::endl;
        }
    }
    
    void printAll(const NetworkGraph& network, const ActivityPlotter& activity) {
        printNetwork(network);
        std::cout << std::endl;
        printActivityPlot(activity);
        std::cout << std::endl;
        printWeightMatrix(network);
    }
};

class VisualizationInterface {
public:
    VisualizationInterface() : active(false), updateRate(30.0), showNetwork(true), 
        showActivity(true), showWeights(true), autoLayout(true) {}
    
    bool initialize() {
        active = true;
        return true;
    }
    
    void update() {
        if (active && updateRate > 0) {
            // Update graph layout if auto-layout is enabled
            if (autoLayout) {
                network.updateLayout(1.0 / updateRate);
            }
        }
    }
    
    void render() {
        if (!active) return;
        
        // Clear screen (simplified for console)
        std::cout << "\033[2J\033[H";  // ANSI escape sequences for clear screen
        
        // Header
        std::cout << "=== NLM Phase 6 - Visualization ===" << std::endl;
        std::cout << "Update rate: " << updateRate << " Hz" << std::endl;
        std::cout << "Active components: Network, Activity, Weights" << std::endl;
        std::cout << std::endl;
        
        // Render components
        if (showNetwork) {
            std::cout << "--- Network Topology ---" << std::endl;
            visualizer.printNetwork(network);
            std::cout << std::endl;
        }
        
        if (showActivity) {
            std::cout << "--- Neural Activity ---" << std::endl;
            visualizer.printActivityPlot(activity, 30);
            std::cout << std::endl;
        }
        
        if (showWeights) {
            std::cout << "--- Synaptic Weights ---" << std::endl;
            visualizer.printWeightMatrix(network);
            std::cout << std::endl;
        }
        
        // Footer with controls
        std::cout << "--- Controls ---" << std::endl;
        std::cout << "Press 'w' to toggle network view, 'a' for activity, 's' for weights" << std::endl;
        std::cout << "Press 'l' to toggle auto-layout, 'r' to reset layout" << std::endl;
        std::cout << "Press 'q' to quit" << std::endl;
    }
    
    void close() {
        active = false;
    }
    
    bool isActive() const { return active; }
    
    void setUpdateRate(double hz) { updateRate = hz; }
    double getUpdateRate() const { return updateRate; }
    
    void enableNetwork(bool enable) { showNetwork = enable; }
    void enableActivity(bool enable) { showActivity = enable; }
    void enableWeights(bool enable) { showWeights = enable; }
    
    void setAutoLayout(bool autoLayout) { this->autoLayout = autoLayout; }
    
    // Add data to visualization
    void visualizeNetwork(const NetworkGraph& graph) {
        network = graph;
    }
    
    void visualizeActivity(const ActivityPlotter& plotter) {
        activity = plotter;
    }
    
    // Simulate adding spike data
    void addSpikeEvent(double time, int neuronId, float amplitude = 1.0f) {
        activity.addSpike(time, neuronId, amplitude);
    }
    
    void addTimeSeries(const std::string& name, const std::vector<float>& values, const std::string& color = "") {
        activity.addTimeSeries(name, values, color);
    }
    
    // Update neuron activity based on brain simulation
    void updateFromBrain(const class Brain& brain) {
        // Extract network data
        NetworkGraph graph;
        
        // Get regions from brain
        auto regions = brain.getRegions();
        for (size_t r = 0; r < regions.size(); ++r) {
            RegionId regionId(r + 1);
            auto region = brain.getRegion(regionId);
            if (!region) continue;
            
            // Add region node
            NetworkGraph::Node regionNode;
            regionNode.id = "Region_" + std::to_string(r + 1);
            regionNode.type = "region";
            regionNode.activity = 0.5f; // Simplified - could compute actual activity
            graph.addNode(regionNode);
            
            // Add neurons
            auto populations = region->getPopulations();
            for (size_t p = 0; p < populations.size(); ++p) {
                PopulationId popId(p + 1);
                auto population = region->getPopulation(popId);
                if (!population) continue;
                
                auto neurons = population->getNeurons();
                for (size_t n = 0; n < neurons.size() && n < 10; ++n) { // Limit to 10 neurons for display
                    NeuronId neuronId(n + 1);
                    auto neuron = neurons[n];
                    if (!neuron) continue;
                    
                    // Neuron node
                    NetworkGraph::Node neuronNode;
                    neuronNode.id = "Neuron_" + std::to_string(r + 1) + "_" + std::to_string(p + 1) + "_" + std::to_string(n + 1);
                    neuronNode.type = "neuron";
                    neuronNode.activity = neuron->getMembranePotential() / 100.0f; // Normalize to 0-1
                    graph.addNode(neuronNode);
                    
                    // Region -> Neuron connection
                    NetworkGraph::Edge edge;
                    edge.source = regionNode.id;
                    edge.target = neuronNode.id;
                    edge.weight = 1.0f;
                    edge.directed = true;
                    edge.style = "solid";
                    graph.addEdge(edge);
                    
                    // Add some simulated spike data
                    float spikeRate = neuron->getFiringRate() / 100.0f;
                    if (spikeRate > 0.1f) {
                        addSpikeEvent(graph.getNodeCount() * 0.01, n, spikeRate);
                    }
                }
            }
        }
        
        // Add some simulated synaptic connections
        auto regions = brain.getRegions();
        for (size_t srcR = 0; srcR < regions.size(); ++srcR) {
            auto srcRegion = brain.getRegion(RegionId(srcR + 1));
            if (!srcRegion) continue;
            
            auto srcPopulations = srcRegion->getPopulations();
            for (size_t srcP = 0; srcP < srcPopulations.size(); ++srcP) {
                auto srcPopulation = srcRegion->getPopulation(PopulationId(srcP + 1));
                if (!srcPopulation) continue;
                
                auto srcNeurons = srcPopulation->getNeurons();
                for (size_t srcN = 0; srcN < srcNeurons.size() && srcN < 5; ++srcN) {
                    NeuronId srcNeuronId(srcN + 1);
                    auto srcNeuron = srcNeurons[srcN];
                    if (!srcNeuron) continue;
                    
                    std::string srcNodeId = "Neuron_src_" + std::to_string(srcR + 1) + "_" + std::to_string(srcP + 1) + "_" + std::to_string(srcN + 1);
                    
                    // Simulate some target connections
                    for (size_t tgtR = 0; tgtR < regions.size(); ++tgtR) {
                        if (srcR == tgtR) continue; // Skip self-connections for this demo
                        
                        auto tgtRegion = brain.getRegion(RegionId(tgtR + 1));
                        if (!tgtRegion) continue;
                        
                        auto tgtPopulations = tgtRegion->getPopulations();
                        for (size_t tgtP = 0; tgtP < tgtPopulations.size(); ++tgtP) {
                            auto tgtPopulation = tgtRegion->getPopulation(PopulationId(tgtP + 1));
                            if (!tgtPopulation) continue;
                            
                            auto tgtNeurons = tgtPopulation->getNeurons();
                            for (size_t tgtN = 0; tgtN < tgtNeurons.size() && tgtN < 3; ++tgtN) {
                                NeuronId tgtNeuronId(tgtN + 1);
                                auto tgtNeuron = tgtNeurons[tgtN];
                                if (!tgtNeuron) continue;
                                
                                // Create a connection
                                NetworkGraph::Edge edge;
                                edge.source = srcNodeId;
                                edge.target = "Neuron_tgt_" + std::to_string(tgtR + 1) + "_" + std::to_string(tgtP + 1) + "_" + std::to_string(tgtN + 1);
                                edge.weight = (float)(rand() % 100) / 100.0f; // Random weight 0.0-1.0
                                edge.directed = true;
                                edge.style = (edge.weight > 0.7) ? "solid" : "dashed";
                                graph.addEdge(edge);
                            }
                        }
                    }
                }
            }
        }
        
        visualizeNetwork(graph);
        
        // Add some activity time series
        std::vector<float> firingRateData;
        auto spikeSystem = brain.getSpikeSystem();
        if (spikeSystem) {
            size_t totalSpikeCount = spikeSystem->getTotalSpikeCount();
            double timeSeconds = totalSpikeCount * 0.001; // Approximate time in seconds
            
            for (size_t i = 0; i < 50; ++i) {
                double t = (i / 50.0) * timeSeconds;
                // Simulate firing rate over time
                float rate = (float)(sin(t * 3.14 / timeSeconds) * 0.5 + 0.5); // Oscillating
                firingRateData.push_back(rate);
            }
        }
        
        if (!firingRateData.empty()) {
            addTimeSeries("Firing Rate", firingRateData, "green");
        }
    }
    
private:
    bool active;
    double updateRate;
    bool showNetwork;
    bool showActivity;
    bool showWeights;
    bool autoLayout;
    
    NetworkGraph network;
    ActivityPlotter activity;
    ConsoleVisualizer visualizer;
};

} // namespace nlm
