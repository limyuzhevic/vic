#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/RandomGenerator.hpp"
#include "../brain/NeuralRegion.hpp"
#include <chrono>
#include <thread>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>

namespace nlm {

// Helper functions for mathematical operations
namespace Utils {
    inline float clamp(float value, float min, float max) {
        return std::max(min, std::min(max, value));
    }
    
    inline float mapValue(float value, float inMin, float inMax, float outMin, float outMax) {
        return outMin + (outMax - outMin) * ((value - inMin) / (inMax - inMin));
    }
    
    inline float calculateDistance(float x1, float y1, float z1, float x2, float y2, float z2) {
        return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
    }
    
    inline void normalizeColor(float color[3]) {
        float maxVal = std::max({color[0], color[1], color[2]});
        if (maxVal > 1.0f) {
            color[0] /= maxVal;
            color[1] /= maxVal;
            color[2] /= maxVal;
        }
    }
    
    inline float calculateCorrelation(const std::vector<float>& data1, const std::vector<float>& data2) {
        if (data1.size() != data2.size() || data1.size() < 2) return 0.0f;
        
        float mean1 = std::accumulate(data1.begin(), data1.end(), 0.0f) / data1.size();
        float mean2 = std::accumulate(data2.begin(), data2.end(), 0.0f) / data2.size();
        
        float numerator = 0.0f, denom1 = 0.0f, denom2 = 0.0f;
        for (size_t i = 0; i < data1.size(); ++i) {
            float diff1 = data1[i] - mean1;
            float diff2 = data2[i] - mean2;
            numerator += diff1 * diff2;
            denom1 += diff1 * diff1;
            denom2 += diff2 * diff2;
        }
        
        return std::sqrt(denom1 * denom2) > 0.0f ? numerator / std::sqrt(denom1 * denom2) : 0.0f;
    }
}

// Network graph layout algorithms
class NetworkLayout {
public:
    static void forceDirectedLayout(
        std::vector<NeuralNode>& nodes,
        const std::vector<NeuralConnection>& connections,
        float width = 800.0f,
        float height = 600.0f,
        float height3D = 400.0f
    ) {
        const float K = std::sqrt(width * height) / 2.0f;  // Ideal link length
        const float repulsionStrength = 1000.0f;
        const float attractionStrength = 0.1f;
        const float damping = 0.9f;
        const size_t iterations = 50;
        
        // Convert to 3D for better visualization
        for (auto& node : nodes) {
            // Use 2D layout for 2D mode, 3D for 3D mode
            float angle = static_cast<float>(node.id.index()) * 0.1f;
            node.x = width * 0.5f + std::cos(angle) * 200.0f;
            node.y = height * 0.5f + std::sin(angle) * 200.0f;
            node.z = 0.0f;
        }
        
        // Initial positioning based on neuron type and region
        for (auto& node : nodes) {
            switch (node.type) {
                case NeuronType::Sensory:
                    node.x = width * 0.2f;
                    node.y = height * 0.3f;
                    break;
                case NeuronType::Motor:
                    node.x = width * 0.8f;
                    node.y = height * 0.3f;
                    break;
                case NeuronType::Internal:
                    node.x = width * 0.5f;
                    node.y = height * 0.7f;
                    break;
                default:
                    float angle = static_cast<float>(node.id.index()) * 0.1f;
                    node.x = width * 0.5f + std::cos(angle) * 150.0f;
                    node.y = height * 0.5f + std::sin(angle) * 150.0f;
            }
        }
        
        // Apply force-directed layout
        for (size_t iter = 0; iter < iterations; ++iter) {
            // Repulsion between nodes
            for (size_t i = 0; i < nodes.size(); ++i) {
                for (size_t j = i + 1; j < nodes.size(); ++j) {
                    float dx = nodes[i].x - nodes[j].x;
                    float dy = nodes[i].y - nodes[j].y;
                    float dz = nodes[i].z - nodes[j].z;
                    float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
                    if (dist < 0.001f) dist = 0.001f;
                    
                    float force = repulsionStrength / (dist * dist);
                    float fx = force * dx / dist;
                    float fy = force * dy / dist;
                    float fz = force * dz / dist;
                    
                    nodes[i].x += fx * damping;
                    nodes[i].y += fy * damping;
                    nodes[i].z += fz * damping;
                    nodes[j].x -= fx * damping;
                    nodes[j].y -= fy * damping;
                    nodes[j].z -= fz * damping;
                }
            }
            
            // Attraction along connections
            for (const auto& conn : connections) {
                auto itSrc = std::find_if(nodes.begin(), nodes.end(), 
                                         [conn](const NeuralNode& n) { return n.id == conn.sourceId; });
                auto itDst = std::find_if(nodes.begin(), nodes.end(), 
                                         [conn](const NeuralNode& n) { return n.id == conn.targetId; });
                
                if (itSrc != nodes.end() && itDst != nodes.end()) {
                    float dx = itDst->x - itSrc->x;
                    float dy = itDst->y - itSrc->y;
                    float dz = itDst->z - itSrc->z;
                    float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
                    
                    float force = attractionStrength * conn.weight * dist;
                    float fx = force * dx / dist;
                    float fy = force * dy / dist;
                    float fz = force * dz / dist;
                    
                    itSrc->x -= fx * damping;
                    itSrc->y -= fy * damping;
                    itSrc->z -= fz * damping;
                    itDst->x += fx * damping;
                    itDst->y += fy * damping;
                    itDst->z += fz * damping;
                }
            }
            
            // Boundary constraints
            for (auto& node : nodes) {
                float halfWidth = width * 0.5f;
                float halfHeight = height * 0.5f;
                float halfDepth = height3D * 0.5f;
                
                node.x = std::max(-halfWidth, std::min(halfWidth, node.x));
                node.y = std::max(-halfHeight, std::min(halfHeight, node.y));
                node.z = std::max(-halfDepth, std::min(halfDepth, node.z));
            }
        }
    }
    
    static void circularLayout(std::vector<NeuralNode>& nodes, float radius = 200.0f) {
        float angleStep = 2.0f * 3.14159f / nodes.size();
        for (size_t i = 0; i < nodes.size(); ++i) {
            float angle = static_cast<float>(i) * angleStep;
            nodes[i].x = std::cos(angle) * radius;
            nodes[i].y = std::sin(angle) * radius;
            nodes[i].z = 0.0f;
        }
    }
    
    static void hierarchicalLayout(
        std::vector<NeuralNode>& nodes,
        const std::vector<NeuralConnection>& connections,
        float layerSpacing = 100.0f
    ) {
        // Create layers based on connectivity
        std::map<RegionId, std::vector<NeuronId>> regionToNeurons;
        for (const auto& node : nodes) {
            regionToNeurons[node.regionId].push_back(node.id);
        }
        
        float currentX = 0.0f;
        for (const auto& regionPair : regionToNeurons) {
            float y = static_cast<float>(regionPair.first.index()) * layerSpacing;
            float angleStep = 2.0f * 3.14159f / regionPair.second.size();
            
            for (size_t i = 0; i < regionPair.second.size(); ++i) {
                auto it = std::find_if(nodes.begin(), nodes.end(),
                                      [&regionPair, i](const NeuralNode& n) {
                                          return n.regionId == regionPair.first &&
                                                 n.populationId.index() == i;
                                      });
                if (it != nodes.end()) {
                    float angle = static_cast<float>(i) * angleStep;
                    it->x = currentX;
                    it->y = y;
                    it->z = std::cos(angle) * 50.0f;
                }
            }
            currentX += layerSpacing;
        }
    }
};

// Spike train analysis and visualization
class SpikeAnalyzer {
public:
    static std::vector<TimeSeriesPoint> analyzeFiringRates(
        const Brain& brain,
        const std::vector<SpikeEvent>& spikes,
        Timestamp startTime,
        Timestamp endTime
    ) {
        std::vector<TimeSeriesPoint> rateData;
        rateData.reserve(1000);
        
        // Get all neurons
        size_t totalNeurons = brain.getTotalNeuronCount();
        std::vector<size_t> neuronSpikeCounts(totalNeurons, 0);
        
        // Count spikes per neuron
        for (const auto& spike : spikes) {
            if (spike.step >= startTime && spike.step <= endTime) {
                if (spike.source_neuron.index() < totalNeurons) {
                    neuronSpikeCounts[spike.source_neuron.index()]++;
                }
            }
        }
        
        // Calculate firing rates
        float dt = endTime - startTime;
        if (dt > 0.0f) {
            for (size_t i = 0; i < totalNeurons; ++i) {
                if (neuronSpikeCounts[i] > 0) {
                    TimeSeriesPoint point;
                    point.timestamp = static_cast<Timestamp>(i) * 0.1;  // Normalized time
                    point.value = static_cast<float>(neuronSpikeCounts[i]) / dt;  // Hz
                    point.label = "Neuron " + std::to_string(i);
                    point.color[0] = 0.2f; point.color[1] = 0.8f; point.color[2] = 0.2f;
                    rateData.push_back(point);
                }
            }
        }
        
        return rateData;
    }
    
    static std::vector<std::vector<float>> calculateCrossCorrelation(
        const std::vector<float>& spikeTrain1,
        const std::vector<float>& spikeTrain2,
        size_t maxLag = 100
    ) {
        std::vector<std::vector<float>> correlations;
        size_t size = std::min(spikeTrain1.size(), spikeTrain2.size());
        
        if (size < maxLag) maxLag = size;
        
        for (size_t lag = 0; lag < maxLag; ++lag) {
            float sum1 = 0.0f, sum2 = 0.0f, sum12 = 0.0f;
            size_t count = 0;
            
            for (size_t i = 0; i < size - lag; ++i) {
                sum1 += spikeTrain1[i];
                sum2 += spikeTrain2[i + lag];
                sum12 += spikeTrain1[i] * spikeTrain2[i + lag];
                count++;
            }
            
            float mean1 = sum1 / count;
            float mean2 = sum2 / count;
            float numerator = sum12 - count * mean1 * mean2;
            float denom1 = 0.0f, denom2 = 0.0f;
            
            for (size_t i = 0; i < size - lag; ++i) {
                float diff1 = spikeTrain1[i] - mean1;
                float diff2 = spikeTrain2[i + lag] - mean2;
                denom1 += diff1 * diff1;
                denom2 += diff2 * diff2;
            }
            
            float correlation = std::sqrt(denom1 * denom2) > 0.0f ? numerator / std::sqrt(denom1 * denom2) : 0.0f;
            correlations.push_back({correlation, static_cast<float>(lag), 0.0f});  // placeholder for p-value
        }
        
        return correlations;
    }
    
    static float calculateSynchronizability(const std::vector<std::vector<float>>& spikeTrains) {
        if (spikeTrains.empty()) return 0.0f;
        
        std::vector<float> correlations;
        for (size_t i = 0; i < spikeTrains.size(); ++i) {
            for (size_t j = i + 1; j < spikeTrains.size(); ++j) {
                correlations.push_back(Utils::calculateCorrelation(
                    spikeTrains[i], spikeTrains[j]));
            }
        }
        
        if (correlations.empty()) return 0.0f;
        
        float avgCorrelation = std::accumulate(correlations.begin(), correlations.end(), 0.0f) / correlations.size();
        return avgCorrelation;
    }
};

// Plasticity visualization data analyzer
class PlasticityAnalyzer {
public:
    static std::vector<PlasticityEvolution> analyzePlasticityEvolution(
        const Brain& brain,
        const std::vector<STDP*>& stdpSystems,
        Timestamp startTime,
        Timestamp endTime
    ) {
        std::vector<PlasticityEvolution> evolutionData;
        evolutionData.reserve(100);
        
        PlasticityEvolution evolution;
        evolution.averageWeightChange = 0.0f;
        evolution.learningRate = 0.01f;
        
        for (auto* stdp : stdpSystems) {
            evolution.stdpTraces[0].push_back(stdp->getLTPTrace());
            evolution.stdpTraces[1].push_back(stdp->getLTDTrace());
        }
        
        evolutionData.push_back(evolution);
        return evolutionData;
    }
    
    static std::vector<float> calculateWeightDistribution(
        const Brain& brain
    ) {
        std::vector<float> weights;
        // TODO: Extract synapse weights from brain
        return weights;
    }
    
    static std::vector<std::pair<NeuronId, NeuronId>> detectHebbianPairs(
        const Brain& brain,
        float correlationThreshold = 0.7f
    ) {
        std::vector<std::pair<NeuronId, NeuronId>> pairs;
        // TODO: Implement Hebbian pair detection
        return pairs;
    }
};

// Network topology analyzer
class NetworkTopologyAnalyzer {
public:
    static NetworkTopologyMetrics analyzeTopology(const Brain& brain) {
        NetworkTopologyMetrics metrics;
        metrics.neuronCount = brain.getTotalNeuronCount();
        metrics.synapseCount = brain.getTotalSynapseCount();
        
        if (metrics.synapseCount > 0) {
            metrics.connectionDensity = static_cast<float>(metrics.synapseCount) / 
                                        (metrics.neuronCount * metrics.neuronCount);
        }
        
        // TODO: Calculate actual metrics from brain data
        return metrics;
    }
    
    static std::vector<size_t> calculateDegreeDistribution(const Brain& brain) {
        std::vector<size_t> degreeDist;
        // TODO: Calculate actual degree distribution
        return degreeDist;
    }
    
    static float calculateClusteringCoefficient(const Brain& brain) {
        // TODO: Calculate actual clustering coefficient
        return 0.0f;
    }
    
    static std::vector<std::vector<size_t>> findCommunities(
        const Brain& brain,
        float resolution = 1.0f
    ) {
        std::vector<std::vector<size_t>> communities;
        // TODO: Implement community detection
        return communities;
    }
};

// Visualization renderer base class
class VisualizationRenderer {
public:
    virtual ~VisualizationRenderer() = default;
    virtual void render() = 0;
    virtual void update(double dt) = 0;
    virtual void resize(int width, int height) = 0;
};

// 2D visualization renderer
class Renderer2D : public VisualizationRenderer {
public:
    Renderer2D() : width_(800), height_(600) {}
    
    void setNodes(const std::vector<NeuralNode>& nodes) { nodes_ = nodes; }
    void setConnections(const std::vector<NeuralConnection>& connections) { connections_ = connections; }
    void setConfig(const VisualizationConfig& config) { config_ = config; }
    
    void render() override {
        // Clear background
        glClearColor(config_.backgroundColor[0], config_.backgroundColor[1], 
                     config_.backgroundColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render connections (behind nodes)
        renderConnections();
        
        // Render nodes
        renderNodes();
        
        // Render labels
        if (config_.showLabels) {
            renderLabels();
        }
        
        // Render weight information
        if (config_.showWeights) {
            renderWeights();
        }
        
        // Render firing rate information
        if (config_.showFiringRates) {
            renderFiringRates();
        }
    }
    
    void update(double dt) override {
        // Update animations
        time_ += dt;
    }
    
    void resize(int width, int height) override {
        width_ = width;
        height_ = height;
        glViewport(0, 0, width, height);
        setupProjection();
    }
    
    void setupProjection() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-width_ * 0.5f, width_ * 0.5f, -height_ * 0.5f, 
                height_ * 0.5f, -1000.0f, 1000.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    
private:
    void renderNodes() {
        for (const auto& node : nodes_) {
            glPushMatrix();
            glTranslatef(node.x, node.y, node.z);
            
            // Draw node
            glColor3f(node.color[0], node.color[1], node.color[2]);
            glBegin(GL_TRIANGLE_FAN);
            for (int i = 0; i <= 360; i += 30) {
                float angle = i * 3.14159f / 180.0f;
                float x = std::cos(angle) * node.radius;
                float y = std::sin(angle) * node.radius;
                glVertex2f(x, y);
            }
            glEnd();
            
            glPopMatrix();
        }
    }
    
    void renderConnections() {
        for (const auto& conn : connections_) {
            auto itSrc = std::find_if(nodes_.begin(), nodes_.end(),
                                     [conn](const NeuralNode& n) { return n.id == conn.sourceId; });
            auto itDst = std::find_if(nodes_.begin(), nodes_.end(),
                                     [conn](const NeuralNode& n) { return n.id == conn.targetId; });
            
            if (itSrc != nodes_.end() && itDst != nodes_.end()) {
                glColor4f(conn.color[0], conn.color[1], conn.color[2], conn.opacity);
                glLineWidth(conn.width);
                
                glBegin(GL_LINES);
                glVertex3f(itSrc->x, itSrc->y, itSrc->z);
                glVertex3f(itDst->x, itDst->y, itDst->z);
                glEnd();
            }
        }
    }
    
    void renderLabels() {
        // TODO: Render text labels using appropriate font rendering
    }
    
    void renderWeights() {
        for (const auto& conn : connections_) {
            auto itSrc = std::find_if(nodes_.begin(), nodes_.end(),
                                     [conn](const NeuralNode& n) { return n.id == conn.sourceId; });
            auto itDst = std::find_if(nodes_.begin(), nodes_.end(),
                                     [conn](const NeuralNode& n) { return n.id == conn.targetId; });
            
            if (itSrc != nodes_.end() && itDst != nodes_.end()) {
                float midX = (itSrc->x + itDst->x) * 0.5f;
                float midY = (itSrc->y + itDst->y) * 0.5f;
                
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << conn.weight;
                
                // TODO: Render weight text
            }
        }
    }
    
    void renderFiringRates() {
        for (const auto& node : nodes_) {
            if (node.firingRate > 0.0f) {
                glPushMatrix();
                glTranslatef(node.x, node.y, node.z + node.radius + 5.0f);
                
                // Draw firing rate indicator
                float rateColor[3] = {1.0f, 1.0f - node.firingRate * 0.1f, 0.0f};
                glColor3f(rateColor[0], rateColor[1], rateColor[2]);
                
                // TODO: Render firing rate indicator (bar or circle)
                
                glPopMatrix();
            }
        }
    }
    
protected:
    std::vector<NeuralNode> nodes_;
    std::vector<NeuralConnection> connections_;
    VisualizationConfig config_;
    int width_ = 800;
    int height_ = 600;
    double time_ = 0.0;
};

// Main implementation
struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    double currentFrame;
    double lastUpdateTime;
    
    VisualizationConfig config;
    
    // Data buffers
    std::vector<NeuralNode> nodes;
    std::vector<NeuralConnection> connections;
    std::vector<TimeSeriesPoint> populationActivity;
    std::vector<CorrelationResult> crossCorrelations;
    NetworkTopologyMetrics topologyMetrics;
    
    // Brain state cache
    const Brain* cachedBrain = nullptr;
    Timestamp lastBrainUpdate = -1.0;
    
    // Analysis results cache
    bool analysisDirty = true;
    
    // Renderer
    std::unique_ptr<Renderer2D> renderer;
    
    // Analysis components
    SpikeAnalyzer spikeAnalyzer;
    PlasticityAnalyzer plasticityAnalyzer;
    NetworkTopologyAnalyzer topologyAnalyzer;
    
    Impl() : active(false), updateRate(30.0), currentFrame(0), lastUpdateTime(0.0) {
        // Initialize with default configuration
        config.pointSize = 3.0f;
        config.lineWidth = 1.5f;
        config.fontSize = 12.0f;
        config.showLabels = true;
        config.showWeights = true;
        config.showFiringRates = true;
        config.showPlasticity = true;
        config.render2D = true;
        config.render3D = false;
        config.enableAnimation = true;
        config.enableZoom = true;
        config.enablePan = true;
        config.enableRotate = false;
        
        // Default colors
        config.excitatoryColor[0] = 0.2f; config.excitatoryColor[1] = 0.8f; config.excitatoryColor[2] = 0.2f;
        config.inhibitoryColor[0] = 0.8f; config.inhibitoryColor[1] = 0.2f; config.inhibitoryColor[2] = 0.2f;
        config.modulatoryColor[0] = 0.2f; config.modulatoryColor[1] = 0.2f; config.modulatoryColor[2] = 0.8f;
        config.activeColor[0] = 1.0f; config.activeColor[1] = 1.0f; config.activeColor[2] = 0.0f;
        config.backgroundColor[0] = 0.1f; config.backgroundColor[1] = 0.1f; config.backgroundColor[2] = 0.15f;
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize(const VisualizationConfig& config) {
    setConfig(config);
    
    try {
        // Initialize OpenGL context (placeholder)
        // In a real implementation, this would initialize a graphics backend
        
        pImpl->renderer = std::make_unique<Renderer2D>();
        pImpl->renderer->setConfig(config);
        
        pImpl->active = true;
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error(std::string("Visualization initialization failed: ") + e.what());
        pImpl->active = false;
        return false;
    }
}

void VisualizationInterface::update(const Brain& brain) {
    if (!pImpl->active) return;
    
    double currentTime = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    double dt = currentTime - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = currentTime;
    
    // Update brain state visualization data
    updateVisualizationData(brain);
    
    // Update renderer
    pImpl->renderer->update(dt);
    
    // Update frame counter
    pImpl->currentFrame++;
}

void VisualizationInterface::render() {
    if (!pImpl->active || !pImpl->renderer) return;
    
    pImpl->renderer->render();
}

void VisualizationInterface::close() {
    if (pImpl->renderer) {
        pImpl->renderer.reset();
    }
    pImpl->active = false;
    pImpl->cachedBrain = nullptr;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setConfig(const VisualizationConfig& config) {
    pImpl->config = config;
    if (pImpl->renderer) {
        pImpl->renderer->setConfig(config);
    }
}

const VisualizationConfig& VisualizationInterface::getConfig() const {
    return pImpl->config;
}

bool VisualizationInterface::exportToPNG(const std::string& filepath, int width, int height) {
    if (!pImpl->active) return false;
    
    try {
        // Placeholder for PNG export
        // In a real implementation, this would capture the current frame and save as PNG
        Logger::getInstance().info("PNG export to " + filepath + " (not implemented)");
        return false;
    } catch (const std::exception& e) {
        Logger::getInstance().error(std::string("PNG export failed: ") + e.what());
        return false;
    }
}

bool VisualizationInterface::exportToSVG(const std::string& filepath) {
    if (!pImpl->active) return false;
    
    try {
        // Generate SVG file with current visualization
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        file << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">\n";
        
        // Export nodes as circles
        for (const auto& node : pImpl->nodes) {
            file << "  <circle cx=\"" << node.x << "\" cy=\"" << node.y 
                 << "\" r=\"" << node.radius << "\" fill=\"rgb(" << 
                 << static_cast<int>(node.color[0] * 255) << "," << 
                 static_cast<int>(node.color[1] * 255) << "," << 
                 static_cast<int>(node.color[2] * 255) << ")\" />\n";
        }
        
        // Export connections as lines
        for (const auto& conn : pImpl->connections) {
            auto itSrc = std::find_if(pImpl->nodes.begin(), pImpl->nodes.end(),
                                     [conn](const NeuralNode& n) { return n.id == conn.sourceId; });
            auto itDst = std::find_if(pImpl->nodes.begin(), pImpl->nodes.end(),
                                     [conn](const NeuralNode& n) { return n.id == conn.targetId; });
            
            if (itSrc != pImpl->nodes.end() && itDst != pImpl->nodes.end()) {
                file << "  <line x1=\"" << itSrc->x << "\" y1=\"" << itSrc->y
                     << "\" x2=\"" << itDst->x << "\" y2=\"" << itDst->y
                     << "\" stroke=\"rgb(" << static_cast<int>(conn.color[0] * 255) << "," <<
                     static_cast<int>(conn.color[1] * 255) << "," <<
                     static_cast<int>(conn.color[2] * 255) << ")\" stroke-width=\"" 
                     << conn.width << "\" />\n";
            }
        }
        
        file << "</svg>\n";
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error(std::string("SVG export failed: ") + e.what());
        return false;
    }
}

bool VisualizationInterface::exportToPDF(const std::string& filepath) {
    // Placeholder for PDF export
    Logger::getInstance().info("PDF export to " + filepath + " (not implemented)");
    return false;
}

bool VisualizationInterface::exportToHTML(const std::string& filepath) {
    if (!pImpl->active) return false;
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "<!DOCTYPE html>\n";
        file << "<html>\n<head>\n";
        file << "<title>Neural Network Visualization</title>\n";
        file << "<style>\n";
        file << "body { margin: 0; padding: 20px; background-color: #1a1a1a; font-family: Arial, sans-serif; }\n";
        file << ".container { max-width: 1200px; margin: 0 auto; }\n";
        file << ".visualization { border: 1px solid #333; background: white; }\n";
        file << "</style>\n";
        file << "</head>\n<body>\n";
        file << "<div class=\"container\">\n";
        file << "<h1 style=\"color: white;\">Neural Network Visualization</h1>\n";
        file << "<div class=\"visualization\">\n";
        file << "<!-- SVG visualization will be embedded here -->\n";
        file << "</div>\n";
        file << "</div>\n";
        file << "<script>\n";
        file << "// JavaScript for interactive visualization\n";
        file << "console.log('Visualization loaded');\n";
        file << "</script>\n";
        file << "</body>\n</html>\n";
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error(std::string("HTML export failed: ") + e.what());
        return false;
    }
}

const std::vector<NeuralNode>& VisualizationInterface::getNodes() const {
    return pImpl->nodes;
}

const std::vector<NeuralConnection>& VisualizationInterface::getConnections() const {
    return pImpl->connections;
}

const std::vector<TimeSeriesPoint>& VisualizationInterface::getPopulationActivity() const {
    return pImpl->populationActivity;
}

const std::vector<CorrelationResult>& VisualizationInterface::getCrossCorrelations() const {
    return pImpl->crossCorrelations;
}

const NetworkTopologyMetrics& VisualizationInterface::getTopologyMetrics() const {
    return pImpl->topologyMetrics;
}

void VisualizationInterface::setCameraPosition(float x, float y, float z) {
    // Placeholder for camera control
}

void VisualizationInterface::setCameraLookAt(float x, float y, float z) {
    // Placeholder for camera control
}

void VisualizationInterface::zoom(float factor) {
    // Placeholder for zoom control
}

void VisualizationInterface::pan(float dx, float dy) {
    // Placeholder for pan control
}

void VisualizationInterface::rotate(float dx, float dy) {
    // Placeholder for rotation control
}

void VisualizationInterface::enableAnalysis(const std::string& analysisType, bool enable) {
    // TODO: Implement analysis type enabling
}

void VisualizationInterface::setAnalysisParameter(const std::string& paramName, float value) {
    // TODO: Implement parameter setting
}

void VisualizationInterface::computeAnalysis() {
    pImpl->analysisDirty = true;
}

void VisualizationInterface::startFrame() {
    // Placeholder for frame management
}

void VisualizationInterface::endFrame() {
    // Placeholder for frame management
}

size_t VisualizationInterface::getCurrentFrame() const {
    return static_cast<size_t>(pImpl->currentFrame);
}

void VisualizationInterface::updateVisualizationData(const Brain& brain) {
    // Update cached brain reference
    pImpl->cachedBrain = &brain;
    
    // Update nodes from brain
    pImpl->nodes.clear();
    extractNodesFromBrain(brain);
    
    // Update connections from brain
    pImpl->connections.clear();
    extractConnectionsFromBrain(brain);
    
    // Update population activity
    updatePopulationActivity(brain);
    
    // Update cross correlations if needed
    if (pImpl->analysisDirty) {
        computeCrossCorrelations();
        pImpl->analysisDirty = false;
    }
    
    // Update topology metrics
    pImpl->topologyMetrics = pImpl->topologyAnalyzer.analyzeTopology(brain);
    
    // Update renderer data
    if (pImpl->renderer) {
        pImpl->renderer->setNodes(pImpl->nodes);
        pImpl->renderer->setConnections(pImpl->connections);
    }
}

void VisualizationInterface::extractNodesFromBrain(const Brain& brain) {
    // Extract neurons from brain regions
    const auto& regions = brain.getRegions();
    
    for (size_t regionIdx = 0; regionIdx < regions.size(); ++regionIdx) {
        const auto& region = regions[regionIdx];
        if (!region) continue;
        
        const auto& populations = region->getPopulations();
        
        for (size_t popIdx = 0; popIdx < populations.size(); ++popIdx) {
            const auto& population = populations[popIdx];
            if (!population) continue;
            
            const auto& neurons = population->getNeurons();
            
            for (const auto& neuron : neurons) {
                if (!neuron) continue;
                
                NeuralNode node;
                node.id = neuron->getId();
                node.regionId = static_cast<RegionId>(regionIdx);
                node.populationId = static_cast<PopulationId>(popIdx);
                node.type = neuron->getType();
                
                // Calculate position (placeholder - will be refined by layout algorithms)
                float angle = static_cast<float>(neuron->getId().index()) * 0.1f;
                node.x = std::cos(angle) * 200.0f;
                node.y = std::sin(angle) * 200.0f;
                node.z = 0.0f;
                node.radius = pImpl->config.pointSize;
                node.intensity = 1.0f;
                node.firingRate = neuron->getFiringRate();
                node.plasticityStrength = 1.0f;  // Placeholder
                node.isActive = neuron->isFiring();
                node.isFiring = neuron->isFiring();
                
                // Update color based on neuron state
                node.updateColor();
                
                pImpl->nodes.push_back(node);
            }
        }
    }
    
    // Apply layout algorithm
    NetworkLayout::forceDirectedLayout(pImpl->nodes, pImpl->connections);
}

void VisualizationInterface::extractConnectionsFromBrain(const Brain& brain) {
    // Extract synapses from brain regions
    const auto& regions = brain.getRegions();
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        const auto& populations = region->getPopulations();
        
        for (const auto& population : populations) {
            if (!population) continue;
            
            const auto& neurons = population->getNeurons();
            
            for (const auto& neuron : neurons) {
                if (!neuron) continue;
                
                const auto& outgoing = neuron->getOutgoingSynapses();
                
                for (const auto& handle : outgoing) {
                    // Placeholder - need to get actual synapse from brain
                    // In a real implementation, we would look up the synapse
                    // by handle and extract its properties
                    NeuralConnection conn;
                    conn.sourceId = neuron->getId();
                    conn.targetId = static_cast<NeuronId>(handle);  // Placeholder
                    conn.weight = 1.0f;  // Placeholder
                    conn.delay = 1;  // Placeholder
                    conn.type = SynapseType::Excitatory;  // Placeholder
                    conn.opacity = 0.5f;
                    conn.width = pImpl->config.lineWidth;
                    
                    conn.updateColor();
                    pImpl->connections.push_back(conn);
                }
            }
        }
    }
}

void VisualizationInterface::updatePopulationActivity(const Brain& brain) {
    pImpl->populationActivity.clear();
    
    // Get spike events from spike system
    SpikeSystem* spikeSystem = brain.getSpikeSystem();
    if (!spikeSystem) return;
    
    const auto& spikes = spikeSystem->getSpikeEvents();
    
    // Calculate firing rates for each neuron type
    size_t totalSpikes = spikes.size();
    size_t excitatorySpikes = 0;
    size_t inhibitorySpikes = 0;
    size_t sensorySpikes = 0;
    size_t motorSpikes = 0;
    
    for (const auto& spike : spikes) {
        // Get neuron type (placeholder - need to access neuron data)
        switch (spike.source_neuron.index() % 5) {
            case 0: excitatorySpikes++; break;
            case 1: inhibitorySpikes++; break;
            case 2: sensorySpikes++; break;
            case 3: motorSpikes++; break;
            default: break;
        }
    }
    
    // Add population activity data points
    TimeSeriesPoint excitatoryPoint;
    excitatoryPoint.timestamp = static_cast<Timestamp>(pImpl->currentFrame) * 0.1;
    excitatoryPoint.value = static_cast<float>(excitatorySpikes);
    excitatoryPoint.label = "Excitatory";
    excitatoryPoint.color[0] = 0.2f; excitatoryPoint.color[1] = 0.8f; excitatoryPoint.color[2] = 0.2f;
    pImpl->populationActivity.push_back(excitatoryPoint);
    
    TimeSeriesPoint inhibitoryPoint;
    inhibitoryPoint.timestamp = static_cast<Timestamp>(pImpl->currentFrame) * 0.1;
    inhibitoryPoint.value = static_cast<float>(inhibitorySpikes);
    inhibitoryPoint.label = "Inhibitory";
    inhibitoryPoint.color[0] = 0.8f; inhibitoryPoint.color[1] = 0.2f; inhibitoryPoint.color[2] = 0.2f;
    pImpl->populationActivity.push_back(inhibitoryPoint);
}

void VisualizationInterface::computeCrossCorrelations() {
    pImpl->crossCorrelations.clear();
    
    // Get spike events from brain
    SpikeSystem* spikeSystem = pImpl->cachedBrain->getSpikeSystem();
    if (!spikeSystem) return;
    
    const auto& spikes = spikeSystem->getSpikeEvents();
    
    // Group spikes by neuron
    std::unordered_map<NeuronId, std::vector<float>> neuronSpikes;
    for (const auto& spike : spikes) {
        neuronSpikes[spike.source_neuron].push_back(spike.timestamp);
    }
    
    // Calculate cross correlations between neuron pairs
    size_t maxPairs = std::min(static_cast<size_t>(10), neuronSpikes.size());
    size_t pairCount = 0;
    
    for (auto it1 = neuronSpikes.begin(); it1 != neuronSpikes.end() && pairCount < maxPairs; ++it1) {
        for (auto it2 = std::next(it1); it2 != neuronSpikes.end() && pairCount < maxPairs; ++it2) {
            // Convert spike times to binary spike trains
            std::vector<float> train1, train2;
            float maxTime = 100.0f;  // Analyze over 100ms
            
            for (float t = 0.0f; t < maxTime; t += 0.1f) {
                train1.push_back(std::any_of(it1->second.begin(), it1->second.end(),
                    [t](float spikeTime) { return std::abs(spikeTime - t) < 0.1f; }) ? 1.0f : 0.0f);
                train2.push_back(std::any_of(it2->second.begin(), it2->second.end(),
                    [t](float spikeTime) { return std::abs(spikeTime - t) < 0.1f; }) ? 1.0f : 0.0f);
            }
            
            float correlation = Utils::calculateCorrelation(train1, train2);
            if (std::abs(correlation) > 0.1f) {  // Only keep significant correlations
                CorrelationResult result;
                result.neuron1 = it1->first;
                result.neuron2 = it2->first;
                result.crossCorrelation = correlation;
                result.timeLag = 0.0f;  // Placeholder
                result.significance = CorrelationSignificance::Significant;
                result.description = "Cross-correlation analysis";
                pImpl->crossCorrelations.push_back(result);
                
                pairCount++;
            }
        }
    }
}

} // namespace nlm