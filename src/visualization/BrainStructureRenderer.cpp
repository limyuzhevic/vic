#include "BrainStructureRenderer.hpp"
#include "ColorManager.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

BrainStructureRenderer::BrainStructureRenderer()
    : layoutMode_(LayoutMode::Hierarchical), connectionStyle_(ConnectionStyle::Weighted),
      showNeuronLabels_(true), showConnectionWeights_(true), showRegionBoundaries_(true),
      zoomLevel_(1.0f), panOffsetX_(0), panOffsetY_(0),
      totalNeurons_(0), totalSynapses_(0), totalPopulations_(0), totalRegions_(0),
      avgFiringRate_(0.0f), avgSynapticWeight_(0.0f),
      colorScheme_("default"), gridColumns_(10), gridRows_(10),
      forceRepulsion_(50.0f), forceAttraction_(0.1f), layoutIterations_(100) {
    initializeDefaultConfiguration();
}

void BrainStructureRenderer::initializeDefaultConfiguration() {
    // Initialize default layout parameters
    // Grid layout
    gridColumns_ = 10;
    gridRows_ = std::max(1, totalRegions_);
    
    // Force-directed layout parameters
    forceRepulsion_ = 50.0f;
    forceAttraction_ = 0.1f;
    layoutIterations_ = 100;
    
    // Default color scheme
    colorScheme_ = "default";
}

void BrainStructureRenderer::render(const Brain& brain) {
    updateLayout(brain);
    
    // Clear screen (placeholder for actual terminal clearing)
    std::cout << "\033[2J\033[H";
    
    // Render title
    ColorManager colorManager;
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightYellow) << "=== NEURAL NETWORK STRUCTURE ===" << "\033[0m" << std::endl;
    
    // Render nodes
    renderNodes(brain);
    
    // Render connections
    renderConnections(brain);
    
    // Render labels
    renderLabels(brain);
    
    // Render statistics
    showStatistics();
    
    // Render controls
    renderControls();
}

void BrainStructureRenderer::updateLayout(const Brain& brain) {
    extractBrainData(brain);
    calculateNodePositions();
    calculateConnectionPaths();
}

void BrainStructureRenderer::extractBrainData(const Brain& brain) {
    nodes_.clear();
    connections_.clear();
    nodeMap_.clear();
    connectionMap_.clear();
    
    // Reset statistics
    totalNeurons_ = 0;
    totalSynapses_ = 0;
    totalPopulations_ = 0;
    totalRegions_ = 0;
    avgFiringRate_ = 0.0f;
    avgSynapticWeight_ = 0.0f;
    
    // Extract regions
    auto regions = brain.getRegions();
    totalRegions_ = regions.size();
    
    for (const auto& region : regions) {
        Node regionNode;
        regionNode.id = "region_" + std::to_string(region->getId().index());
        regionNode.name = region->getName();
        regionNode.neuronCount = region->getTotalNeuronCount();
        regionNode.region = region.get();
        regionNode.radius = std::min(3, static_cast<int>(std::sqrt(regionNode.neuronCount) / 5));
        regionNode.activityLevel = region->getAverageFiringRate();
        
        nodes_.push_back(regionNode);
        nodeMap_[regionNode.id] = regionNode;
        
        // Extract populations
        auto populations = region->getPopulations();
        totalPopulations_ += populations.size();
        
        for (const auto& pop : populations) {
            Node popNode;
            popNode.id = "pop_" + std::to_string(pop->getId().index());
            popNode.name = std::string("Population_") + std::to_string(pop->getId().index());
            popNode.neuronCount = pop->getNeuronCount();
            popNode.population = pop.get();
            popNode.radius = std::min(2, static_cast<int>(std::sqrt(popNode.neuronCount) / 8));
            popNode.activityLevel = pop->getAverageFiringRate();
            
            nodes_.push_back(popNode);
            nodeMap_[popNode.id] = popNode;
            
            // Extract neurons
            auto neurons = pop->getNeurons();
            totalNeurons_ += neurons.size();
            
            for (size_t i = 0; i < std::min<size_t>(neurons.size(), 20); ++i) { // Sample neurons
                Neuron* neuron = neurons[i];
                Node neuronNode;
                neuronNode.id = "neuron_" + std::to_string(neuron->getId().index());
                neuronNode.name = "N" + std::to_string(neuron->getId().index());
                neuronNode.neuron = neuron;
                neuronNode.radius = 1;
                neuronNode.activityLevel = neuron->isFiring() ? 1.0f : 0.0f;
                
                nodes_.push_back(neuronNode);
                nodeMap_[neuronNode.id] = neuronNode;
            }
        }
    }
    
    // Extract synapses
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        totalSynapses_ += synapses.size();
        
        for (const auto* syn : synapses) {
            Connection connection;
            connection.sourceId = "neuron_" + std::to_string(syn->getSourceNeuron().index());
            connection.targetId = "neuron_" + std::to_string(syn->getDestinationNeuron().index());
            connection.weight = syn->getWeight();
            connection.directed = syn->getType() == SynapseType::Excitatory;
            connection.style = "weight";
            
            connections_.push_back(connection);
            connectionMap_[{connection.sourceId, connection.targetId}] = connection;
        }
    }
    
    // Calculate average firing rate
    if (totalNeurons_ > 0) {
        avgFiringRate_ = brain.getAverageFiringRate();
    }
    
    // Calculate average synaptic weight
    if (totalSynapses_ > 0) {
        avgSynapticWeight_ = 0.0f; // Placeholder
    }
}

void BrainStructureRenderer::calculateNodePositions() {
    switch (layoutMode_) {
        case LayoutMode::Grid:
            layoutGrid(Brain()); // Pass dummy brain for interface compliance
            break;
        case LayoutMode::Radial:
            layoutRadial(Brain());
            break;
        case LayoutMode::Hierarchical:
            layoutHierarchical(Brain());
            break;
        case LayoutMode::ForceDirected:
            layoutForceDirected(Brain());
            break;
        case LayoutMode::Geographic:
            layoutGeographic(Brain());
            break;
        case LayoutMode::Custom:
            // Use default positions
            break;
    }
}

void BrainStructureRenderer::calculateConnectionPaths() {
    // Calculate Bezier curves for connections if needed
    for (auto& connection : connections_) {
        auto sourceIt = nodeMap_.find(connection.sourceId);
        auto targetIt = nodeMap_.find(connection.targetId);
        
        if (sourceIt != nodeMap_.end() && targetIt != nodeMap_.end()) {
            const Node& source = sourceIt->second;
            const Node& target = targetIt->second;
            
            // Simple straight line for now
            connection.style = "straight";
        }
    }
}

void BrainStructureRenderer::layoutGrid(const Brain& brain) {
    // Simple grid layout - regions in rows, populations in columns
    int regionsPerRow = std::max(1, totalRegions_ / gridColumns_);
    
    for (size_t i = 0; i < nodes_.size(); ++i) {
        auto& node = nodes_[i];
        
        if (node.region) {
            // Region nodes
            int row = i / gridColumns_;
            int col = i % gridColumns_;
            
            node.x = (col * 20) + 10; // 20 chars spacing, 10 char offset
            node.y = (row * 15) + 5;  // 15 chars spacing, 5 char offset
        } else if (node.population) {
            // Population nodes - position relative to region
            // Find parent region
            for (auto& parentNode : nodes_) {
                if (parentNode.region == node.population->getRegion()) {
                    node.x = parentNode.x - 10 + (i % 3) * 8;
                    node.y = parentNode.y + (i / 3) * 8;
                    break;
                }
            }
        } else if (node.neuron) {
            // Neuron nodes - position relative to population
            for (auto& parentNode : nodes_) {
                if (parentNode.population == node.neuron->getPopulation()) {
                    node.x = parentNode.x + (i % 4) * 4 - 6;
                    node.y = parentNode.y + (i / 4) * 4;
                    break;
                }
            }
        }
    }
}

void BrainStructureRenderer::layoutRadial(const Brain& brain) {
    // Radial layout - regions on outer circle, neurons towards center
    float angleStep = 2.0f * M_PI / totalRegions_;
    
    for (size_t i = 0; i < nodes_.size(); ++i) {
        auto& node = nodes_[i];
        
        if (node.region) {
            float angle = i * angleStep;
            node.x = static_cast<int>(20.0f * cos(angle) + 40);
            node.y = static_cast<int>(20.0f * sin(angle) + 20);
        } else if (node.population) {
            // Position populations in middle rings
            // Find parent region
            for (auto& parentNode : nodes_) {
                if (parentNode.region == node.population->getRegion()) {
                    float angle = (i % 3) * M_PI / 3.0f + angleStep * parentNode.y / 30.0f;
                    node.x = parentNode.x + static_cast<int>(10.0f * cos(angle));
                    node.y = parentNode.y + static_cast<int>(10.0f * sin(angle));
                    break;
                }
            }
        } else if (node.neuron) {
            // Neurons in inner rings
            for (auto& parentNode : nodes_) {
                if (parentNode.population == node.neuron->getPopulation()) {
                    float angle = (i % 6) * M_PI / 3.0f;
                    node.x = parentNode.x + static_cast<int>(5.0f * cos(angle));
                    node.y = parentNode.y + static_cast<int>(5.0f * sin(angle));
                    break;
                }
            }
        }
    }
}

void BrainStructureRenderer::layoutHierarchical(const Brain& brain) {
    // Hierarchical layout - regions -> populations -> neurons
    int levelSpacing = 30;
    
    for (size_t i = 0; i < nodes_.size(); ++i) {
        auto& node = nodes_[i];
        
        if (node.region) {
            // Region at top level
            node.x = 40;
            node.y = i * levelSpacing + 10;
        } else if (node.population) {
            // Population level
            for (auto& parentNode : nodes_) {
                if (parentNode.region == node.population->getRegion()) {
                    node.x = parentNode.x + 40;
                    node.y = parentNode.y + (i % 3) * levelSpacing / 3;
                    break;
                }
            }
        } else if (node.neuron) {
            // Neuron level
            for (auto& parentNode : nodes_) {
                if (parentNode.population == node.neuron->getPopulation()) {
                    node.x = parentNode.x + 40;
                    node.y = parentNode.y + (i % 6) * levelSpacing / 6;
                    break;
                }
            }
        }
    }
}

void BrainStructureRenderer::layoutForceDirected(const Brain& brain) {
    // Simple force-directed layout - placeholder for full implementation
    // Use existing positions or simple random placement
    
    for (auto& node : nodes_) {
        if (node.region) {
            node.x = 40 + (rand() % 60);
            node.y = 20 + (rand() % 30);
        } else if (node.population) {
            node.x = 60 + (rand() % 40);
            node.y = 30 + (rand() % 20);
        } else if (node.neuron) {
            node.x = 80 + (rand() % 20);
            node.y = 40 + (rand() % 15);
        }
    }
}

void BrainStructureRenderer::layoutGeographic(const Brain& brain) {
    // Geographic-like layout based on brain regions
    // Simulate brain hemisphere positions
    
    for (size_t i = 0; i < nodes_.size(); ++i) {
        auto& node = nodes_[i];
        
        if (node.region) {
            // Simulate left/right hemispheres
            if (i % 2 == 0) {
                // Left hemisphere
                node.x = 15 + (i / 2) * 25;
                node.y = 25;
            } else {
                // Right hemisphere
                node.x = 70 + ((i - 1) / 2) * 25;
                node.y = 25;
            }
        } else if (node.population) {
            // Position populations relative to region
            for (auto& parentNode : nodes_) {
                if (parentNode.region == node.population->getRegion()) {
                    node.x = parentNode.x + (i % 2) * 15 - 7;
                    node.y = parentNode.y + 10;
                    break;
                }
            }
        } else if (node.neuron) {
            // Neurons in inner layers
            for (auto& parentNode : nodes_) {
                if (parentNode.population == node.neuron->getPopulation()) {
                    node.x = parentNode.x + (i % 3) * 5 - 5;
                    node.y = parentNode.y + 5;
                    break;
                }
            }
        }
    }
}

void BrainStructureRenderer::renderNodes(const Brain& brain) {
    ColorManager colorManager;
    
    // Sort nodes by level (regions, populations, neurons)
    std::vector<Node*> regionNodes, populationNodes, neuronNodes;
    
    for (auto& node : nodes_) {
        if (node.region) regionNodes.push_back(&node);
        else if (node.population) populationNodes.push_back(&node);
        else if (node.neuron) neuronNodes.push_back(&node);
    }
    
    // Render regions first (back layer)
    for (auto& node : regionNodes) {
        renderRegion(*node, colorManager);
    }
    
    // Render populations
    for (auto& node : populationNodes) {
        renderPopulation(*node, colorManager);
    }
    
    // Render neurons (front layer)
    for (auto& node : neuronNodes) {
        renderNeuron(*node, colorManager);
    }
}

void BrainStructureRenderer::renderConnections(const Brain& brain) {
    ColorManager colorManager;
    
    for (const auto& connection : connections_) {
        auto sourceIt = nodeMap_.find(connection.sourceId);
        auto targetIt = nodeMap_.find(connection.targetId);
        
        if (sourceIt != nodeMap_.end() && targetIt != nodeMap_.end()) {
            const Node& source = sourceIt->second;
            const Node& target = targetIt->second;
            
            // Draw connection
            renderSynapse(connection, colorManager);
        }
    }
}

void BrainStructureRenderer::renderLabels(const Brain& brain) {
    if (!showNeuronLabels_) return;
    
    ColorManager colorManager;
    std::string labelColor = colorManager.getColorCode(ColorManager::Color::White);
    
    for (const auto& node : nodes_) {
        if (!node.name.empty() && (node.neuron || node.population)) {
            int screenX = applyPanOffset(node.x);
            int screenY = applyPanOffset(node.y);
            
            std::cout << labelColor;
            if (node.neuron) {
                std::cout << node.name;
            } else if (node.population) {
                std::cout << node.name.substr(0, std::min<size_t>(10, node.name.length()));
            }
            std::cout << "\033[0m";
        }
    }
}

void BrainStructureRenderer::renderNeuron(const Node& neuron, const ColorManager& colorManager) {
    int screenX = applyPanOffset(neuron.x);
    int screenY = applyPanOffset(neuron.y);
    
    std::string neuronColor = getNodeColor(neuron, colorManager);
    
    // Draw neuron
    for (int dy = -neuron.radius; dy <= neuron.radius; ++dy) {
        for (int dx = -neuron.radius; dx <= neuron.radius; ++dx) {
            int x = screenX + dx * zoomLevel_;
            int y = screenY + dy * zoomLevel_;
            
            if (x >= 0 && x < 80 && y >= 0 && y < 40) {
                char c = (neuron.activityLevel > 0.5f) ? 'O' : 'o';
                std::cout << neuronColor << c << "\033[0m";
            }
        }
        if (screenY + (dy + 1) * zoomLevel_ >= 0 && screenY + (dy + 1) * zoomLevel_ < 40) {
            std::cout << "\n";
        }
    }
}

void BrainStructureRenderer::renderPopulation(const Node& population, const ColorManager& colorManager) {
    int screenX = applyPanOffset(population.x);
    int screenY = applyPanOffset(population.y);
    
    std::string popColor = getNodeColor(population, colorManager);
    
    // Draw population (larger circle)
    for (int dy = -population.radius; dy <= population.radius; ++dy) {
        for (int dx = -population.radius; dx <= population.radius; ++dx) {
            int x = screenX + dx * zoomLevel_ * 2;
            int y = screenY + dy * zoomLevel_ * 2;
            
            if (x >= 0 && x < 80 && y >= 0 && y < 40) {
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist <= population.radius) {
                    std::cout << popColor << "+" << "\033[0m";
                }
            }
        }
        if (screenY + (dy + 1) * zoomLevel_ * 2 >= 0 && screenY + (dy + 1) * zoomLevel_ * 2 < 40) {
            std::cout << "\n";
        }
    }
}

void BrainStructureRenderer::renderRegion(const Node& region, const ColorManager& colorManager) {
    int screenX = applyPanOffset(region.x);
    int screenY = applyPanOffset(region.y);
    
    std::string regionColor = getNodeColor(region, colorManager);
    
    // Draw region (larger filled circle)
    for (int dy = -region.radius * 3; dy <= region.radius * 3; ++dy) {
        for (int dx = -region.radius * 3; dx <= region.radius * 3; ++dx) {
            int x = screenX + dx * zoomLevel_ * 4;
            int y = screenY + dy * zoomLevel_ * 4;
            
            if (x >= 0 && x < 80 && y >= 0 && y < 40) {
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist <= region.radius * 3) {
                    std::cout << regionColor << "*" << "\033[0m";
                }
            }
        }
        if (screenY + (dy + 1) * zoomLevel_ * 4 >= 0 && screenY + (dy + 1) * zoomLevel_ * 4 < 40) {
            std::cout << "\n";
        }
    }
}

void BrainStructureRenderer::renderSynapse(const Connection& synapse, const ColorManager& colorManager) {
    auto sourceIt = nodeMap_.find(synapse.sourceId);
    auto targetIt = nodeMap_.find(synapse.targetId);
    
    if (sourceIt == nodeMap_.end() || targetIt == nodeMap_.end()) return;
    
    const Node& source = sourceIt->second;
    const Node& target = targetIt->second;
    
    int screenX1 = applyPanOffset(source.x);
    int screenY1 = applyPanOffset(source.y);
    int screenX2 = applyPanOffset(target.x);
    int screenY2 = applyPanOffset(target.y);
    
    std::string synColor = getConnectionColor(synapse, colorManager);
    
    // Draw line from source to target
    int dx = screenX2 - screenX1;
    int dy = screenY2 - screenY1;
    int steps = std::max(std::abs(dx), std::abs(dy));
    
    if (steps == 0) return;
    
    float xInc = static_cast<float>(dx) / steps;
    float yInc = static_cast<float>(dy) / steps;
    
    float x = screenX1;
    float y = screenY1;
    
    for (int i = 0; i <= steps; ++i) {
        int px = static_cast<int>(x);
        int py = static_cast<int>(y);
        
        if (px >= 0 && px < 80 && py >= 0 && py < 40) {
            std::cout << synColor << "|" << "\033[0m";
        }
        
        x += xInc;
        y += yInc;
    }
}

void BrainStructureRenderer::renderInterRegionConnection(const Connection& connection, const ColorManager& colorManager) {
    // Similar to synapse but with different rendering
    renderSynapse(connection, colorManager);
}

std::string BrainStructureRenderer::getNodeColor(const Node& node, const ColorManager& colorManager) const {
    if (node.activityLevel > 0.8f) {
        return colorManager.getColorCode(ColorManager::Color::BrightRed);
    } else if (node.activityLevel > 0.5f) {
        return colorManager.getColorCode(ColorManager::Color::Yellow);
    } else if (node.activityLevel > 0.2f) {
        return colorManager.getColorCode(ColorManager::Color::Green);
    } else {
        return colorManager.getColorCode(ColorManager::Color::Blue);
    }
}

std::string BrainStructureRenderer::getConnectionColor(const Connection& connection, const ColorManager& colorManager) const {
    if (connection.weight > 0.5f) {
        return colorManager.getColorCode(ColorManager::Color::BrightMagenta);
    } else if (connection.weight > 0.0f) {
        return colorManager.getColorCode(ColorManager::Color::Cyan);
    } else {
        return colorManager.getColorCode(ColorManager::Color::White);
    }
}

void BrainStructureRenderer::renderControls() const {
    std::cout << "\033[2J\033[H"; // Clear screen (temporary)
    
    ColorManager colorManager;
    std::string helpColor = colorManager.getColorCode(ColorManager::Color::BrightCyan);
    
    std::cout << helpColor << "=== NEURAL VISUALIZATION CONTROLS ===" << "\033[0m" << std::endl;
    std::cout << helpColor << "Z: Zoom In    X: Zoom Out    Arrow Keys: Pan" << "\033[0m" << std::endl;
    std::cout << helpColor << "M: Toggle Layout (Grid/Radial/Hierarchical)" << "\033[0m" << std::endl;
    std::cout << helpColor << "C: Toggle Connections" << "\033[0m" << std::endl;
    std::cout << helpColor << "L: Toggle Labels" << "\033[0m" << std::endl;
    std::cout << helpColor << "~: Commands" << "\033[0m" << std::endl;
    std::cout << helpColor << "Q: Quit" << "\033[0m" << std::endl;
    std::cout << std::endl;
    
    // Show current mode
    std::cout << helpColor << "Layout: " << static_cast<int>(layoutMode_) 
              << "  Connections: " << (showConnectionWeights_ ? "ON" : "OFF") 
              << "  Zoom: " << zoomLevel_ << "x" << "\033[0m" << std::endl;
}

void BrainStructureRenderer::showStatistics() const {
    ColorManager colorManager;
    std::string statColor = colorManager.getColorCode(ColorManager::Color::BrightWhite);
    
    std::cout << std::endl << statColor << "=== STATISTICS ===" << "\033[0m" << std::endl;
    std::cout << statColor << "Total Neurons: " << totalNeurons_ << "\033[0m" << std::endl;
    std::cout << statColor << "Total Synapses: " << totalSynapses_ << "\033[0m" << std::endl;
    std::cout << statColor << "Total Populations: " << totalPopulations_ << "\033[0m" << std::endl;
    std::cout << statColor << "Total Regions: " << totalRegions_ << "\033[0m" << std::endl;
    std::cout << statColor << "Average Firing Rate: " << avgFiringRate_ << " Hz\033[0m" << std::endl;
    std::cout << statColor << "Average Synaptic Weight: " << avgSynapticWeight_ << "\033[0m" << std::endl;
    std::cout << statColor << "Active Neurons: " << "NA" << "\033[0m" << std::endl;
    std::cout << statColor << "Current Step: " << "NA" << "\033[0m" << std::endl;
}

Node* BrainStructureRenderer::getNodeAt(int x, int y) {
    for (auto& node : nodes_) {
        int screenX = applyPanOffset(node.x);
        int screenY = applyPanOffset(node.y);
        
        if (x >= screenX - node.radius * zoomLevel_ && x <= screenX + node.radius * zoomLevel_ &&
            y >= screenY - node.radius * zoomLevel_ && y <= screenY + node.radius * zoomLevel_) {
            return &node;
        }
    }
    return nullptr;
}

Connection* BrainStructureRenderer::getConnectionAt(int x, int y) {
    // Check if click is near a connection line (simplified)
    for (auto& connection : connections_) {
        auto sourceIt = nodeMap_.find(connection.sourceId);
        auto targetIt = nodeMap_.find(connection.targetId);
        
        if (sourceIt != nodeMap_.end() && targetIt != nodeMap_.end()) {
            const Node& source = sourceIt->second;
            const Node& target = targetIt->second;
            
            // Simple line proximity check
            int screenX1 = applyPanOffset(source.x);
            int screenY1 = applyPanOffset(source.y);
            int screenX2 = applyPanOffset(target.x);
            int screenY2 = applyPanOffset(target.y);
            
            // Check if point is near line (simplified distance check)
            int dx = screenX2 - screenX1;
            int dy = screenY2 - screenY1;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist > 0) {
                float t = ((x - screenX1) * dx + (y - screenY1) * dy) / (dist * dist);
                if (t >= 0 && t <= 1) {
                    float projX = screenX1 + t * dx;
                    float projY = screenY1 + t * dy;
                    float pointDist = std::sqrt((x - projX) * (x - projX) + (y - projY) * (y - projY));
                    
                    if (pointDist < 2.0f) {
                        return &connection;
                    }
                }
            }
        }
    }
    return nullptr;
}

void BrainStructureRenderer::handleNodeSelection(const std::string& nodeId) {
    // Handle node selection (placeholder for interaction)
    auto it = nodeMap_.find(nodeId);
    if (it != nodeMap_.end()) {
        std::cout << "Selected node: " << it->second.name << std::endl;
    }
}

void BrainStructureRenderer::handleConnectionHighlight(const std::string& sourceId, const std::string& targetId) {
    // Handle connection highlighting (placeholder for interaction)
    std::cout << "Highlighted connection from " << sourceId << " to " << targetId << std::endl;
}

} // namespace nlm
