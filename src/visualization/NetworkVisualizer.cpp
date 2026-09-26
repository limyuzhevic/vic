#include "NetworkVisualizer.hpp"
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>
#include <unordered_map>

namespace nlm {

struct NetworkVisualizer::Impl {
    // Network data
    std::vector<NetworkNode> nodes;
    std::vector<NetworkConnection> connections;
    NetworkLayout layout;
    NetworkVisualizationConfig config;
    
    // Physics simulation
    std::vector<std::array<float, 3>> velocities;
    std::vector<std::array<float, 3>> forces;
    bool layoutConverged;
    size_t iterations;
    
    // Rendering state
    float zoomLevel;
    float panX, panY;
    float rotationAngle;
    bool mousePressed;
    float lastMouseX, lastMouseY;
    
    // Performance optimization
    bool lodEnabled;
    size_t maxVisibleNodes;
    bool connectionCulling;
    std::vector<bool> nodeVisible;
    std::vector<bool> connectionVisible;
    
    // Colors and styling
    std::map<std::string, SDL_Color> colorMap;
    std::map<std::string, std::vector<float>> gradientStops;
    
    // Font rendering
    TTF_Font* font;
    SDL_Texture* textureCache;
    std::unordered_map<std::string, SDL_Texture*> textCache;
    
    // Interaction state
    NetworkNode* highlightedNode;
    std::vector<NetworkConnection*> highlightedConnections;
    float focusDistance;
    
    // Statistics
    size_t totalRenderedNodes;
    size_t totalRenderedConnections;
    float renderingTime;
    
    Impl() : zoomLevel(1.0f), panX(0.0f), panY(0.0f), rotationAngle(0.0f),
             mousePressed(false), lastMouseX(0.0f), lastMouseY(0.0f),
             lodEnabled(true), maxVisibleNodes(5000), connectionCulling(true),
             highlightedNode(nullptr), focusDistance(100.0f),
             totalRenderedNodes(0), totalRenderedConnections(0),
             renderingTime(0.0f), layoutConverged(false), iterations(0),
             font(nullptr), textureCache(nullptr) {
        // Initialize color map
        colorMap["excitatory"] = {255, 100, 100, 255};      // Red
        colorMap["inhibitory"] = {100, 100, 255, 255};      // Blue
        colorMap["sensory"] = {100, 255, 100, 255};         // Green
        colorMap["motor"] = {255, 100, 255, 255};          // Magenta
        colorMap["internal"] = {100, 200, 255, 255};       // Light blue
        colorMap["active"] = {255, 200, 0, 255};           // Yellow
        colorMap["plastic"] = {255, 150, 0, 255};          // Orange
        colorMap["background"] = {20, 20, 20, 255};        // Dark gray
        colorMap["grid"] = {40, 40, 40, 255};              // Grid lines
    }
};

NetworkVisualizer::NetworkVisualizer() : pImpl(std::make_unique<Impl>()) {
}

NetworkVisualizer::~NetworkVisualizer() {
    close();
}

bool NetworkVisualizer::initialize() {
    if (pImpl->font) {
        TTF_CloseFont(pImpl->font);
        pImpl->font = nullptr;
    }
    
    // Load font for labels
    pImpl->font = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 12);
    if (!pImpl->font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        // Continue without font support
    }
    
    pImpl->highlightedNode = nullptr;
    pImpl->layoutConverged = false;
    pImpl->iterations = 0;
    
    return true;
}

void NetworkVisualizer::update() {
    if (nodes.empty()) {
        return;
    }
    
    // Update positions based on layout
    updateNodePositions();
    
    // Apply physics if using force-directed layout
    if (pImpl->layout.type == NetworkLayout::LayoutType::ForceDirected) {
        applyPhysicsForces();
        pImpl->iterations++;
        
        // Check convergence
        if (pImpl->iterations > 1000) {
            pImpl->layoutConverged = true;
        }
    }
}

void NetworkVisualizer::render() {
    if (nodes.empty()) {
        return;
    }
    
    auto renderStart = std::chrono::high_resolution_clock::now();
    
    // Clear with background color
    SDL_SetRenderDrawColor(pImpl->renderer, pImpl->colorMap["background"].r,
                          pImpl->colorMap["background"].g, pImpl->colorMap["background"].b,
                          pImpl->colorMap["background"].a);
    SDL_RenderClear(pImpl->renderer);
    
    // Apply transformation matrix
    SDL_RenderSetScale(pImpl->renderer, pImpl->zoomLevel, pImpl->zoomLevel);
    SDL_RenderSetLogicalTranslation(pImpl->renderer, -pImpl->panX, -pImpl->panY);
    
    // Render connections first (behind nodes)
    renderConnections();
    
    // Render regions and populations
    if (pImpl->config.showRegionBoundaries || pImpl->config.showPopulationBoundaries) {
        renderRegions();
        renderPopulations();
    }
    
    // Render nodes
    renderNodes();
    
    // Render labels
    if (pImpl->config.showNeuronLabels) {
        renderLabels();
    }
    
    // Render debug info if enabled
    if (pImpl->debugMode) {
        renderDebugInfo();
    }
    
    // Reset transformation
    SDL_RenderSetScale(pImpl->renderer, 1.0f, 1.0f);
    SDL_RenderSetLogicalTranslation(pImpl->renderer, 0, 0);
    
    auto renderEnd = std::chrono::high_resolution_clock::now();
    pImpl->renderingTime = std::chrono::duration_cast<std::chrono::microseconds>(renderEnd - renderStart).count();
    
    pImpl->totalRenderedNodes = pImpl->nodeVisible.end() - 
                               std::remove_if(pImpl->nodeVisible.begin(), pImpl->nodeVisible.end(), 
                                             [](bool visible) { return !visible; });
    
    pImpl->totalRenderedConnections = pImpl->connectionVisible.end() - 
                                     std::remove_if(pImpl->connectionVisible.begin(), pImpl->connectionVisible.end(),
                                                   [](bool visible) { return !visible; });
}

void NetworkVisualizer::close() {
    if (pImpl->font) {
        TTF_CloseFont(pImpl->font);
        pImpl->font = nullptr;
    }
    
    for (auto& pair : pImpl->textCache) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
            pair.second = nullptr;
        }
    }
    
    if (pImpl->textureCache) {
        SDL_DestroyTexture(pImpl->textureCache);
        pImpl->textureCache = nullptr;
    }
}

bool NetworkVisualizer::isActive() const {
    return pImpl->initialized;
}

void NetworkVisualizer::setUpdateRate(double hz) {
    // Note: This is a no-op for NetworkVisualizer as it uses visualization engine timing
}

double NetworkVisualizer::getUpdateRate() const {
    return 60.0;  // Fixed rate for now
}

void NetworkVisualizer::visualizeNetwork(const Brain& brain) {
    // Convert brain data to network format
    // Note: Need to implement based on actual Brain API
    // For now, placeholder implementation
    
    // Extract neuron data
    auto neurons = brain.getAllNeurons();
    pImpl->nodes.clear();
    pImpl->nodes.reserve(neurons.size());
    
    for (const auto& neuron : neurons) {
        NetworkNode node;
        node.id = neuron->getId();
        node.type = neuron->getType();
        node.regionId = neuron->getRegionId();
        node.populationId = neuron->getPopulationId();
        node.firingRate = neuron->getFiringRate();
        node.developmentalStage = DevelopmentalStage::Adult;  // Placeholder
        
        // Get synapse counts
        node.synapseCount = neuron->getIncomingSynapses().size() + neuron->getOutgoingSynapses().size();
        
        pImpl->nodes.push_back(node);
    }
    
    // Extract connection data
    auto regions = brain.getRegions();
    pImpl->connections.clear();
    
    for (const auto& region : regions) {
        for (const auto& synapse : region->getSynapses()) {
            NetworkConnection conn;
            conn.id = synapse->getId();
            conn.sourceId = synapse->getSourceId();
            conn.targetId = synapse->getTargetId();
            conn.weight = synapse->getWeight();
            conn.type = synapse->getType();
            conn.delay = synapse->getDelay();
            
            // Find corresponding neurons
            for (auto& node : pImpl->nodes) {
                if (node.id == conn.sourceId || node.id == conn.targetId) {
                    if (node.id == conn.sourceId) {
                        conn.sourceId = node.id;
                    }
                    if (node.id == conn.targetId) {
                        conn.targetId = node.id;
                    }
                }
            }
            
            pImpl->connections.push_back(conn);
        }
    }
    
    // Initialize visualization state
    resetLayout();
    computeLayout();
}

void NetworkVisualizer::setNetworkData(const std::vector<NetworkNode>& nodes, const std::vector<NetworkConnection>& connections) {
    pImpl->nodes = nodes;
    pImpl->connections = connections;
    
    // Initialize velocities and forces
    pImpl->velocities.clear();
    pImpl->forces.clear();
    pImpl->velocities.reserve(nodes.size());
    pImpl->forces.reserve(nodes.size());
    
    for (size_t i = 0; i < nodes.size(); ++i) {
        pImpl->velocities.push_back({0.0f, 0.0f, 0.0f});
        pImpl->forces.push_back({0.0f, 0.0f, 0.0f});
    }
    
    resetLayout();
    computeLayout();
}

void NetworkVisualizer::updateNodePositions() {
    // Convert logical positions to screen coordinates
    for (auto& node : pImpl->nodes) {
        float screenX = node.x * pImpl->zoomLevel - pImpl->panX;
        float screenY = node.y * pImpl->zoomLevel - pImpl->panY;
        
        // Apply rotation
        float cosA = std::cos(pImpl->rotationAngle);
        float sinA = std::sin(pImpl->rotationAngle);
        float rotatedX = screenX * cosA - screenY * sinA;
        float rotatedY = screenX * sinA + screenY * cosA;
        
        node.x = rotatedX;
        node.y = rotatedY;
    }
}

void NetworkVisualizer::computeLayout() {
    if (pImpl->layout.type == NetworkLayout::LayoutType::ForceDirected) {
        // Initialize with random positions if not set
        if (pImpl->nodes.empty()) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dis(-100.0f, 100.0f);
            
            for (auto& node : pImpl->nodes) {
                node.x = dis(gen);
                node.y = dis(gen);
            }
        }
        
        pImpl->layoutConverged = false;
        pImpl->iterations = 0;
    } else if (pImpl->layout.type == NetworkLayout::LayoutType::Hierarchical) {
        // Hierarchical layout implementation
        // Group by region/population and arrange levels
        // Placeholder implementation
    }
}

void NetworkVisualizer::applyPhysicsForces() {
    if (!pImpl->layoutConverged) {
        // Clear forces
        for (auto& force : pImpl->forces) {
            force[0] = 0.0f;
            force[1] = 0.0f;
            force[2] = 0.0f;
        }
        
        // Calculate repulsion forces
        for (size_t i = 0; i < pImpl->nodes.size(); ++i) {
            for (size_t j = i + 1; j < pImpl->nodes.size(); ++j) {
                float forceMag = calculateRepulsionForce(pImpl->nodes[i], pImpl->nodes[j]);
                
                // Apply repulsion to both nodes
                pImpl->forces[i][0] += forceMag * (pImpl->nodes[i].x - pImpl->nodes[j].x);
                pImpl->forces[i][1] += forceMag * (pImpl->nodes[i].y - pImpl->nodes[j].y);
                pImpl->forces[j][0] -= forceMag * (pImpl->nodes[i].x - pImpl->nodes[j].x);
                pImpl->forces[j][1] -= forceMag * (pImpl->nodes[i].y - pImpl->nodes[j].y);
            }
        }
        
        // Calculate attraction forces for connected nodes
        for (const auto& conn : pImpl->connections) {
            if (connVisible[pImpl->findNodeIndex(conn.sourceId)] && 
                connVisible[pImpl->findNodeIndex(conn.targetId)]) {
                
                float forceMag = calculateAttractionForce(pImpl->nodes[pImpl->findNodeIndex(conn.sourceId)],
                                                         pImpl->nodes[pImpl->findNodeIndex(conn.targetId)]);
                
                pImpl->forces[pImpl->findNodeIndex(conn.sourceId)][0] -= 
                    forceMag * (pImpl->nodes[pImpl->findNodeIndex(conn.sourceId)].x - 
                               pImpl->nodes[pImpl->findNodeIndex(conn.targetId)].x);
                pImpl->forces[pImpl->findNodeIndex(conn.sourceId)][1] -= 
                    forceMag * (pImpl->nodes[pImpl->findNodeIndex(conn.sourceId)].y - 
                               pImpl->nodes[pImpl->findNodeIndex(conn.targetId)].y);
                
                pImpl->forces[pImpl->findNodeIndex(conn.targetId)][0] += 
                    forceMag * (pImpl->nodes[pImpl->findNodeIndex(conn.sourceId)].x - 
                               pImpl->nodes[pImpl->findNodeIndex(conn.targetId)].x);
                pImpl->forces[pImpl->findNodeIndex(conn.targetId)][1] += 
                    forceMag * (pImpl->nodes[pImpl->findNodeIndex(conn.sourceId)].y - 
                               pImpl->nodes[pImpl->findNodeIndex(conn.targetId)].y);
            }
        }
        
        // Update velocities and positions
        for (size_t i = 0; i < pImpl->nodes.size(); ++i) {
            // Update velocity
            pImpl->velocities[i][0] += pImpl->forces[i][0] * pImpl->layout.timeStep;
            pImpl->velocities[i][1] += pImpl->forces[i][1] * pImpl->layout.timeStep;
            
            // Apply damping
            pImpl->velocities[i][0] *= (1.0f - pImpl->layout.dampingFactor);
            pImpl->velocities[i][1] *= (1.0f - pImpl->layout.dampingFactor);
            
            // Update position
            pImpl->nodes[i].x += pImpl->velocities[i][0] * pImpl->layout.timeStep;
            pImpl->nodes[i].y += pImpl->velocities[i][1] * pImpl->layout.timeStep;
        }
    }
}

float NetworkVisualizer::calculateRepulsionForce(const NetworkNode& node1, const NetworkNode& node2) const {
    float dx = node1.x - node2.x;
    float dy = node1.y - node2.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance < 0.001f) {
        distance = 0.001f;
    }
    
    return pImpl->layout.repulsionStrength / (distance * distance);
}

float NetworkVisualizer::calculateAttractionForce(const NetworkNode& node1, const NetworkNode& node2) const {
    float dx = node1.x - node2.x;
    float dy = node1.y - node2.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance < 0.001f) {
        distance = 0.001f;
    }
    
    return pImpl->layout.attractionStrength * distance;
}

void NetworkVisualizer::renderNodes() {
    // Determine visible nodes based on LOD
    pImpl->nodeVisible.assign(pImpl->nodes.size(), true);
    
    if (pImpl->lodEnabled && pImpl->nodes.size() > pImpl->maxVisibleNodes) {
        // Simple LOD: render only some nodes
        std::mt19937 gen(42); // Fixed seed for deterministic rendering
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        size_t renderCount = std::min(pImpl->maxVisibleNodes, pImpl->nodes.size());
        std::vector<size_t> indices(renderCount);
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), gen);
        
        for (size_t i = 0; i < pImpl->nodes.size(); ++i) {
            pImpl->nodeVisible[i] = (std::find(indices.begin(), indices.end(), i) != indices.end());
        }
    }
    
    // Render each node
    for (size_t i = 0; i < pImpl->nodes.size(); ++i) {
        if (!pImpl->nodeVisible[i]) {
            continue;
        }
        
        const auto& node = pImpl->nodes[i];
        SDL_Color nodeColor = pImpl->colorMap["internal"]; // Default color
        
        // Override based on neuron type
        switch (node.type) {
            case NeuronType::Excitatory:
                nodeColor = pImpl->colorMap["excitatory"];
                break;
            case NeuronType::Inhibitory:
                nodeColor = pImpl->colorMap["inhibitory"];
                break;
            case NeuronType::Sensory:
                nodeColor = pImpl->colorMap["sensory"];
                break;
            case NeuronType::Motor:
                nodeColor = pImpl->colorMap["motor"];
                break;
            default:
                nodeColor = pImpl->colorMap["internal"];
                break;
        }
        
        // Highlight active neurons
        if (pImpl->config.highlightActive && node.firingRate > pImpl->config.highlightThreshold) {
            nodeColor = pImpl->colorMap["active"];
        }
        
        // Draw node
        float screenX = node.x;
        float screenY = node.y;
        float nodeRadius = node.radius * pImpl->zoomLevel;
        
        // Draw circle
        for (int w = 0; w < nodeRadius * 2; ++w) {
            int radiusW = static_cast<int>(std::sqrt(nodeRadius * nodeRadius - (w - nodeRadius) * (w - nodeRadius)));
            int cx = static_cast<int>(screenX);
            int cy = static_cast<int>(screenY);
            
            SDL_SetRenderDrawColor(pImpl->renderer, nodeColor.r, nodeColor.g, nodeColor.b, nodeColor.a);
            
            for (int xx = cx - radiusW; xx <= cx + radiusW; ++xx) {
                int dy = static_cast<int>(std::sqrt(radiusW * radiusW - (xx - cx) * (xx - cx)));
                SDL_RenderDrawLine(pImpl->renderer, xx, cy - dy, xx, cy + dy);
            }
        }
        
        // Draw outline
        if (node.developmentalStage == DevelopmentalStage::CriticalPeriod) {
            SDL_SetRenderDrawColor(pImpl->renderer, pImpl->colorMap["plastic"].r,
                                  pImpl->colorMap["plastic"].g, pImpl->colorMap["plastic"].b,
                                  pImpl->colorMap["plastic"].a);
            for (int w = 0; w < nodeRadius * 2 + 4; ++w) {
                int radiusW = static_cast<int>(std::sqrt((nodeRadius + 2) * (nodeRadius + 2) - (w - nodeRadius - 2) * (w - nodeRadius - 2)));
                int cx = static_cast<int>(screenX);
                int cy = static_cast<int>(screenY);
                
                for (int xx = cx - radiusW; xx <= cx + radiusW; ++xx) {
                    int dy = static_cast<int>(std::sqrt(radiusW * radiusW - (xx - cx) * (xx - cx)));
                    SDL_RenderDrawLine(pImpl->renderer, xx, cy - dy, xx, cy + dy);
                }
            }
        }
    }
}

void NetworkVisualizer::renderConnections() {
    // Determine visible connections based on culling
    pImpl->connectionVisible.assign(pImpl->connections.size(), true);
    
    if (pImpl->connectionCulling) {
        // Simple culling: hide connections between nodes that are too far apart
        for (size_t i = 0; i < pImpl->connections.size(); ++i) {
            size_t sourceIdx = pImpl->findNodeIndex(pImpl->connections[i].sourceId);
            size_t targetIdx = pImpl->findNodeIndex(pImpl->connections[i].targetId);
            
            if (sourceIdx != SIZE_MAX && targetIdx != SIZE_MAX) {
                float dx = pImpl->nodes[sourceIdx].x - pImpl->nodes[targetIdx].x;
                float dy = pImpl->nodes[sourceIdx].y - pImpl->nodes[targetIdx].y;
                float distance = std::sqrt(dx * dx + dy * dy);
                
                // Hide connections that are too long
                if (distance > 200.0f * pImpl->zoomLevel) {
                    pImpl->connectionVisible[i] = false;
                }
            } else {
                pImpl->connectionVisible[i] = false;
            }
        }
    }
    
    // Render connections
    for (size_t i = 0; i < pImpl->connections.size(); ++i) {
        if (!pImpl->connectionVisible[i]) {
            continue;
        }
        
        const auto& conn = pImpl->connections[i];
        size_t sourceIdx = pImpl->findNodeIndex(conn.sourceId);
        size_t targetIdx = pImpl->findNodeIndex(conn.targetId);
        
        if (sourceIdx == SIZE_MAX || targetIdx == SIZE_MAX) {
            continue;
        }
        
        const auto& sourceNode = pImpl->nodes[sourceIdx];
        const auto& targetNode = pImpl->nodes[targetIdx];
        
        float startX = sourceNode.x;
        float startY = sourceNode.y;
        float endX = targetNode.x;
        float endY = targetNode.y;
        
        // Calculate connection properties
        float weight = conn.weight;
        float connectionWidth = getConnectionWidth(weight) * pImpl->zoomLevel;
        SDL_Color connectionColor = pImpl->colorMap["background"]; // Default
        
        // Override based on connection type
        switch (conn.type) {
            case SynapseType::Excitatory:
                connectionColor = pImpl->colorMap["excitatory"];
                break;
            case SynapseType::Inhibitory:
                connectionColor = pImpl->colorMap["inhibitory"];
                break;
            case SynapseType::Modulatory:
                connectionColor = pImpl->colorMap["plastic"];
                break;
            default:
                connectionColor = pImpl->colorMap["background"];
                break;
        }
        
        // Apply transparency based on weight
        connectionColor.a = static_cast<Uint8>(255.0f * std::min(1.0f, weight));
        
        // Draw line
        SDL_SetRenderDrawColor(pImpl->renderer, connectionColor.r, connectionColor.g, 
                              connectionColor.b, connectionColor.a);
        
        // Draw line with thickness
        float dx = endX - startX;
        float dy = endY - startY;
        float length = std::sqrt(dx * dx + dy * dy);
        
        if (length > 0.001f) {
            float nx = dx / length;
            float ny = dy / length;
            
            float halfWidth = connectionWidth / 2.0f;
            
            // Calculate perpendicular vector
            float perpX = -ny * halfWidth;
            float perpY = nx * halfWidth;
            
            // Draw line with thickness
            int startIx = static_cast<int>(startX - perpX);
            int startIy = static_cast<int>(startY - perpY);
            int endIx = static_cast<int>(endX + perpX);
            int endIy = staticY + perpY);
            
            // Draw line segments
            for (int x = startIx; x <= endIx; ++x) {
                int yOffset = static_cast<int>(std::sqrt(halfWidth * halfWidth - (x - startIx) * (x - startIx)));
                SDL_RenderDrawLine(pImpl->renderer, x, startIy - yOffset, x, startIy + yOffset);
                SDL_RenderDrawLine(pImpl->renderer, x, endIy - yOffset, x, endIy + yOffset);
            }
            
            // Draw end caps
            SDL_RenderDrawLine(pImpl->renderer, startIx, startIy, endIx, endIy);
        }
    }
}

void NetworkVisualizer::renderRegions() {
    // Render region boundaries if enabled
    if (!pImpl->config.showRegionBoundaries) {
        return;
    }
    
    // Group nodes by region
    std::map<RegionId, std::vector<const NetworkNode*>> nodesByRegion;
    for (const auto& node : pImpl->nodes) {
        nodesByRegion[node.regionId].push_back(&node);
    }
    
    // Render region outlines
    SDL_SetRenderDrawColor(pImpl->renderer, pImpl->colorMap["grid"].r,
                          pImpl->colorMap["grid"].g, pImpl->colorMap["grid"].b,
                          pImpl->colorMap["grid"].a / 2);
    
    for (const auto& pair : nodesByRegion) {
        // Calculate bounding box
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float maxY = std::numeric_limits<float>::lowest();
        
        for (const auto* node : pair.second) {
            minX = std::min(minX, node->x);
            minY = std::min(minY, node->y);
            maxX = std::max(maxX, node->x);
            maxY = std::max(maxY, node->y);
        }
        
        // Add padding
        float padding = 10.0f;
        minX -= padding;
        minY -= padding;
        maxX += padding;
        maxY += padding;
        
        // Draw rectangle
        SDL_RenderDrawRect(pImpl->renderer, 
                          &(SDL_Rect){static_cast<int>(minX), static_cast<int>(minY),
                                     static_cast<int>(maxX - minX), static_cast<int>(maxY - minY)});
    }
}

void NetworkVisualizer::renderPopulations() {
    // Render population boundaries if enabled
    if (!pImpl->config.showPopulationBoundaries) {
        return;
    }
    
    // Similar to regions but for populations
    // Implementation placeholder
}

void NetworkVisualizer::renderLabels() {
    // Render neuron labels if enabled
    if (!pImpl->config.showNeuronLabels || !pImpl->font) {
        return;
    }
    
    for (const auto& node : pImpl->nodes) {
        if (!pImpl->nodeVisible[&node - &pImpl->nodes[0]]) {
            continue;
        }
        
        // Create label text
        std::string label = std::to_string(node.id.index());
        if (pImpl->config.showFiringRate) {
            label += " (" + std::to_string(static_cast<int>(node.firingRate)) + " Hz)";
        }
        
        // Create texture (placeholder - would need proper text rendering)
        // For now, just placeholder implementation
    }
}

void NetworkVisualizer::renderDebugInfo() {
    // Render debug information
    std::string debugText = 
        "Nodes: " + std::to_string(pImpl->totalRenderedNodes) + "/" + std::to_string(pImpl->nodes.size()) + "\n" +
        "Connections: " + std::to_string(pImpl->totalRenderedConnections) + "/" + std::to_string(pImpl->connections.size()) + "\n" +
        "Layout iterations: " + std::to_string(pImpl->iterations) + "\n" +
        "Layout converged: " + std::string(pImpl->layoutConverged ? "yes" : "no") + "\n" +
        "Rendering time: " + std::to_string(pImpl->renderingTime) + " μs\n" +
        "Zoom: " + std::to_string(pImpl->zoomLevel);
    
    // Note: Would need proper text rendering implementation
}

// Internal utility methods
void NetworkVisualizer::resetLayout() {
    pImpl->layoutConverged = false;
    pImpl->iterations = 0;
    pImpl->velocities.clear();
    pImpl->forces.clear();
    
    pImpl->velocities.resize(pImpl->nodes.size(), {0.0f, 0.0f, 0.0f});
    pImpl->forces.resize(pImpl->nodes.size(), {0.0f, 0.0f, 0.0f});
    
    computeLayout();
}

void NetworkVisualizer::setLayout(const NetworkLayout& layout) {
    pImpl->layout = layout;
    resetLayout();
}

void NetworkVisualizer::setVisualizationConfig(const NetworkVisualizationConfig& config) {
    pImpl->config = config;
}

void NetworkVisualizer::zoom(float factor) {
    pImpl->zoomLevel *= factor;
    pImpl->zoomLevel = std::max(0.1f, std::min(10.0f, pImpl->zoomLevel));
}

void NetworkVisualizer::pan(float dx, float dy) {
    pImpl->panX += dx;
    pImpl->panY += dy;
}

void NetworkVisualizer::rotate(float angle) {
    pImpl->rotationAngle += angle;
}

void NetworkVisualizer::setFocus(const NetworkNode& node) {
    // Center view on node
    pImpl->panX = -node.x;
    pImpl->panY = -node.y;
}

void NetworkVisualizer::resetView() {
    pImpl->zoomLevel = 1.0f;
    pImpl->panX = 0.0f;
    pImpl->panY = 0.0f;
    pImpl->rotationAngle = 0.0f;
}

size_t NetworkVisualizer::findNodeIndex(NeuronId id) const {
    for (size_t i = 0; i < pImpl->nodes.size(); ++i) {
        if (pImpl->nodes[i].id == id) {
            return i;
        }
    }
    return SIZE_MAX;
}

float NetworkVisualizer::getConnectionWidth(float weight) const {
    return pImpl->layout.connectionWidthFactor * (0.1f + weight * 2.0f);
}

std::string NetworkVisualizer::getNeuronColor(const NetworkNode& node) const {
    switch (node.type) {
        case NeuronType::Excitatory: return "excitatory";
        case NeuronType::Inhibitory: return "inhibitory";
        case NeuronType::Sensory: return "sensory";
        case NeuronType::Motor: return "motor";
        default: return "internal";
    }
}

void NetworkVisualizer::exportNetworkGraph(const std::string& filename, const std::string& format) {
    // Export network data to file
    // Placeholder implementation
    std::cout << "Exporting network graph to " << filename << " in format " << format << std::endl;
}

std::vector<std::string> NetworkVisualizer::analyzeNetworkTopology() const {
    std::vector<std::string> analysis;
    
    // Basic network analysis
    analysis.push_back("Network Topology Analysis:");
    analysis.push_back("  Node count: " + std::to_string(pImpl->nodes.size()));
    analysis.push_back("  Connection count: " + std::to_string(pImpl->connections.size()));
    
    if (!pImpl->connections.empty()) {
        float density = calculateNetworkDensity();
        analysis.push_back("  Network density: " + std::to_string(density));
        
        float clustering = calculateClusteringCoefficient();
        analysis.push_back("  Clustering coefficient: " + std::to_string(clustering));
    }
    
    return analysis;
}

float NetworkVisualizer::calculateNetworkDensity() const {
    size_t maxPossibleConnections = pImpl->nodes.size() * (pImpl->nodes.size() - 1) / 2;
    if (maxPossibleConnections == 0) return 0.0f;
    
    return static_cast<float>(pImpl->connections.size()) / maxPossibleConnections;
}

float NetworkVisualizer::calculateClusteringCoefficient() const {
    // Placeholder for clustering coefficient calculation
    return 0.5f;
}

void NetworkVisualizer::enableLOD(bool enable) {
    pImpl->lodEnabled = enable;
}

void NetworkVisualizer::setMaxNodes(size_t maxNodes) {
    pImpl->maxVisibleNodes = maxNodes;
}

void NetworkVisualizer::enableConnectionCulling(bool enable) {
    pImpl->connectionCulling = enable;
}

} // namespace nlm
