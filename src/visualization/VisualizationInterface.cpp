#include "Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Define character sets for simple visualization
static constexpr const char* NEURON_CHAR = "●";
static constexpr const char* SYNAPSE_CHAR = "·";
static constexpr const char* MEMORY_CHAR = "♦";

VisualizationInterface::VisualizationInterface(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , windowWidth_(0)
    , windowHeight_(0)
    , neuronRadius_(10)
    , fontSize_(12)
    , showConnections_(true)
    , showMemoryNeurons_(true)
    , showAttentionWinners_(false)
    , showPredictions_(false)
    , animationEnabled_(false)
{
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    NLM_LOG_INFO("VisualizationInterface created");
}

VisualizationInterface::~VisualizationInterface() {
    NLM_LOG_INFO("VisualizationInterface destroyed");
}

void VisualizationInterface::initialize(int width, int height) {
    if (!brain_) return;
    
    windowWidth_ = width;
    windowHeight_ = height;
    
    // Calculate region for brain visualization
    const auto& regions = brain_->getRegions();
    if (regions.empty()) return;
    
    // Calculate bounding box
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        if (!neurons) continue;
        
        for (const auto* neuron : *neurons) {
            const auto& state = neuron->getState();
            float x = state.position.x;
            float y = state.position.y;
            
            minX = std::min(minX, x);
            maxX = std::max(maxX, x);
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);
        }
    }
    
    if (maxX > minX && maxY > minY) {
        regionMinX_ = minX;
        regionMaxX_ = maxX;
        regionMinY_ = minY;
        regionMaxY_ = maxY;
        
        // Add padding
        float padding = std::max((maxX - minX) * 0.1f, (maxY - minY) * 0.1f);
        regionMinX_ -= padding;
        regionMaxX_ += padding;
        regionMinY_ -= padding;
        regionMaxY_ += padding;
    }
    
    NLM_LOG_INFO("VisualizationInterface initialized: " + std::to_string(width) + "x" + std::to_string(height) +
                 ", brain region: [" + std::to_string(regionMinX_) + "," + std::to_string(regionMinY_) +
                 "] to [" + std::to_string(regionMaxX_) + "," + std::to_string(regionMaxY_) + "]");
}

void VisualizationInterface::render(float timeStep) {
    if (!brain_) return;
    
    // Update animation state
    if (animationEnabled_) {
        animationTime_ += timeStep;
    }
    
    // Clear screen (would use actual graphics library)
    clearScreen(0.1f, 0.1f, 0.1f, 1.0f);
    
    // Draw coordinate system
    drawCoordinateSystem();
    
    // Draw brain regions
    drawBrainRegions();
    
    // Draw neuron connections
    if (showConnections_) {
        drawNeuronConnections();
    }
    
    // Draw working memory neurons
    if (showMemoryNeurons_) {
        drawWorkingMemoryNeurons();
    }
    
    // Draw attention winners
    if (showAttentionWinners_) {
        drawAttentionWinners();
    }
    
    // Draw predictions
    if (showPredictions_) {
        drawPredictions();
    }
    
    // Draw UI elements
    drawUI();
}

void VisualizationInterface::drawCoordinateSystem() {
    // Draw axes
    drawLine(regionMinX_, 0, regionMaxX_, 0, 0.5f, 0.5f, 0.5f, 0.5f);  // X-axis
    drawLine(0, regionMinY_, 0, regionMaxY_, 0.5f, 0.5f, 0.5f, 0.5f);  // Y-axis
    
    // Draw origin
    drawCircle(0, 0, 5, 1.0f, 1.0f, 1.0f, 0.8f);
    
    // Draw labels
    drawText("-X", regionMinX_, -10, 0.5f, 0.5f, 0.5f, 1.0f);
    drawText("X", regionMaxX_ - 20, -10, 0.5f, 0.5f, 0.5f, 1.0f);
    drawText("-Y", 10, regionMaxY_ - 10, 0.5f, 0.5f, 0.5f, 1.0f);
    drawText("Y", 10, regionMinY_ + 10, 0.5f, 0.5f, 0.5f, 1.0f);
}

void VisualizationInterface::drawBrainRegions() {
    const auto& regions = brain_->getRegions();
    
    for (size_t i = 0; i < regions.size(); ++i) {
        auto neurons = regions[i]->getAllNeurons();
        if (!neurons) continue;
        
        std::vector<float> xs;
        std::vector<float> ys;
        std::vector<float> activations;
        std::vector<NeuronType> types;
        
        for (const auto* neuron : *neurons) {
            const auto& state = neuron->getState();
            float x = state.position.x;
            float y = state.position.y;
            float activation = (state.membranePotential - state.restingPotential) / 10.0f;
            NeuronType type = neuron->getType();
            
            xs.push_back(x);
            ys.push_back(y);
            activations.push_back(activation);
            types.push_back(type);
        }
        
        // Determine region color based on type (using first neuron as reference)
        if (!types.empty()) {
            switch (types[0]) {
                case NeuronType::Excitatory:
                    drawRegion(xs, ys, activations, 0.3f, 1.0f, 0.3f, 0.7f);  // Green
                    break;
                case NeuronType::Inhibitory:
                    drawRegion(xs, ys, activations, 1.0f, 0.3f, 0.3f, 0.7f);  // Red
                    break;
                case NeuronType::Sensory:
                    drawRegion(xs, ys, activations, 0.3f, 1.0f, 1.0f, 0.7f);  // Cyan
                    break;
                case NeuronType::Motor:
                    drawRegion(xs, ys, activations, 1.0f, 0.3f, 1.0f, 0.7f);  // Magenta
                    break;
                case NeuronType::Modulatory:
                    drawRegion(xs, ys, activations, 1.0f, 1.0f, 0.3f, 0.7f);  // Yellow
                    break;
                case NeuronType::Internal:
                    drawRegion(xs, ys, activations, 0.7f, 0.7f, 0.7f, 0.7f);  // Gray
                    break;
                default:
                    drawRegion(xs, ys, activations, 0.5f, 0.5f, 0.5f, 0.7f);  // Default
                    break;
            }
        }
    }
}

void VisualizationInterface::drawNeuronConnections() {
    const auto& regions = brain_->getRegions();
    
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        if (!synapses) continue;
        
        for (const auto* synapse : *synapses) {
            auto preNeuron = region->getNeuron(synapse->getPreNeuronId());
            auto postNeuron = region->getNeuron(synapse->getPostNeuronId());
            
            if (!preNeuron || !postNeuron) continue;
            
            const auto& preState = preNeuron->getState();
            const auto& postState = postNeuron->getState();
            
            float weight = synapse->getWeight();
            float opacity = std::abs(weight) / 5.0f;  // Normalize to [0, 1]
            opacity = std::clamp(opacity, 0.0f, 1.0f);
            
            // Color based on weight
            if (weight > 0) {
                drawLine(preState.position.x, preState.position.y,
                        postState.position.x, postState.position.y,
                        0.3f, 1.0f, 0.3f, opacity);  // Green for excitatory
            } else {
                drawLine(preState.position.x, preState.position.y,
                        postState.position.x, postState.position.y,
                        1.0f, 0.3f, 0.3f, opacity);  // Red for inhibitory
            }
        }
    }
}

void VisualizationInterface::drawWorkingMemoryNeurons() {
    auto workingMemory = brain_->getWorkingMemory();
    if (!workingMemory) return;
    
    const auto& memoryNeurons = workingMemory->getMemoryNeurons();
    
    for (const auto& neuronId : memoryNeurons) {
        // Find neuron with this ID
        bool found = false;
        const auto& regions = brain_->getRegions();
        
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            if (!neurons) continue;
            
            for (const auto* neuron : *neurons) {
                if (neuron->getId() == neuronId) {
                    found = true;
                    const auto& state = neuron->getState();
                    float activation = (state.membranePotential - state.restingPotential) / 10.0f;
                    
                    // Draw as special memory neuron (diamond shape)
                    drawMemoryNeuron(state.position.x, state.position.y, activation);
                    break;
                }
            }
            
            if (found) break;
        }
    }
}

void VisualizationInterface::drawAttentionWinners() {
    auto attention = brain_->getAttention();
    if (!attention) return;
    
    const auto& winners = attention->getWinners();
    
    for (const auto& neuronId : winners) {
        // Find neuron with this ID and draw it with special highlighting
        bool found = false;
        const auto& regions = brain_->getRegions();
        
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            if (!neurons) continue;
            
            for (const auto* neuron : *neurons) {
                if (neuron->getId() == neuronId) {
                    found = true;
                    const auto& state = neuron->getState();
                    float activation = (state.membranePotential - state.restingPotential) / 10.0f;
                    
                    // Draw as highlighted attention winner (glowing effect)
                    drawAttentionNeuron(state.position.x, state.position.y, activation);
                    break;
                }
            }
            
            if (found) break;
        }
    }
}

void VisualizationInterface::drawPredictions() {
    auto predictionSystem = brain_->getPredictionSystem();
    if (!predictionSystem) return;
    
    float error = predictionSystem->getPredictionError();
    float confidence = predictionSystem->getConfidence();
    
    // Draw prediction error and confidence as UI elements
    float uiY = regionMaxY_ + 20;
    
    // Draw error bar
    float errorWidth = confidence * 100;
    drawRect(regionMinX_, uiY, regionMinX_ + errorWidth, uiY + 10, 1.0f, 1.0f, 1.0f, 0.8f);
    
    // Draw error value
    std::string errorText = "Pred Error: " + std::to_string(error);
    drawText(regionMinX_, uiY + 15, errorText, 1.0f, 1.0f, 1.0f, 1.0f);
    
    std::string confidenceText = "Confidence: " + std::to_string(confidence);
    drawText(regionMinX_, uiY + 30, confidenceText, 1.0f, 1.0f, 1.0f, 1.0f);
}

void VisualizationInterface::drawUI() {
    float uiX = regionMaxX_ + 10;
    float uiY = regionMaxY_;
    
    // Draw brain statistics
    size_t totalNeurons = brain_->getTotalNeuronCount();
    size_t totalSynapses = brain_->getTotalSynapseCount();
    size_t firingNeurons = brain_->getFiringNeuronCount();
    size_t pendingSpikes = brain_->getPendingSpikeEventCount();
    
    std::string stats = "Statistics:\n" +
                        "Neurons: " + std::to_string(totalNeurons) + "\n" +
                        "Synapses: " + std::to_string(totalSynapses) + "\n" +
                        "Firing: " + std::to_string(firingNeurons) + "\n" +
                        "Pending: " + std::to_string(pendingSpikes);
    
    drawText(uiX, uiY, stats, 0.7f, 0.7f, 0.7f, 1.0f);
    
    // Draw controls
    float controlsY = uiY + 150;
    
    std::string controls = "Controls:\n" +
                           "SPACE - Toggle animation\n" +
                           "M - Toggle memory neurons\n" +
                           "A - Toggle attention winners\n" +
                           "P - Toggle predictions\n" +
                           "C - Toggle connections\n" +
                           "R - Reset visualization\n" +
                           "+ - Zoom in\n" +
                           "- - Zoom out\n" +
                           "Arrow keys - Pan view";
    
    drawText(uiX, controlsY, controls, 0.7f, 0.7f, 0.7f, 1.0f);
}

void VisualizationInterface::drawRegion(const std::vector<float>& xs,
                                         const std::vector<float>& ys,
                                         const std::vector<float>& activations,
                                         float r, float g, float b, float alpha) {
    if (xs.empty()) return;
    
    // Draw filled polygon for region
    for (size_t i = 0; i < xs.size(); ++i) {
        float x1 = xs[i];
        float y1 = ys[i];
        float x2 = (i + 1 < xs.size()) ? xs[i + 1] : xs[0];
        float y2 = (i + 1 < ys.size()) ? ys[i + 1] : ys[0];
        
        // Fade color based on activation (but keep original color for now)
        float actOpacity = 0.5f + activations[i] * 0.5f;
        
        drawLine(x1, y1, x2, y2, r, g, b, actOpacity * alpha);
    }
    
    // Draw neurons
    for (size_t i = 0; i < xs.size(); ++i) {
        float activation = activations[i];
        float brightness = 0.5f + activation * 0.5f;
        
        float nr = r * brightness;
        float ng = g * brightness;
        float nb = b * brightness;
        
        drawCircle(xs[i], ys[i], neuronRadius_, nr, ng, nb, alpha);
    }
}

void VisualizationInterface::drawCircle(float x, float y, float radius,
                                         float r, float g, float b, float alpha) {
    // Simple circle drawing using lines
    int segments = 12;
    for (int i = 0; i < segments; ++i) {
        float angle1 = 2.0f * M_PI * i / segments;
        float angle2 = 2.0f * M_PI * (i + 1) / segments;
        
        float x1 = x + radius * cos(angle1);
        float y1 = y + radius * sin(angle1);
        float x2 = x + radius * cos(angle2);
        float y2 = y + radius * sin(angle2);
        
        drawLine(x1, y1, x2, y2, r, g, b, alpha);
    }
}

void VisualizationInterface::drawLine(float x1, float y1, float x2, float y2,
                                       float r, float g, float b, float alpha) {
    // Simple line drawing
    // In a real implementation, this would use OpenGL/Vulkan or other graphics API
    // For now, we'll just log the line
    std::string logMsg = "Line: (" + std::to_string(x1) + "," + std::to_string(y1) + ") -> (" +
                        std::to_string(x2) + "," + std::to_string(y2) + ") color: (" +
                        std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) +
                        ") alpha: " + std::to_string(alpha);
    NLM_LOG_INFO(logMsg);
}

void VisualizationInterface::drawRect(float x1, float y1, float x2, float y2,
                                       float r, float g, float b, float alpha) {
    drawLine(x1, y1, x2, y1, r, g, b, alpha);
    drawLine(x2, y1, x2, y2, r, g, b, alpha);
    drawLine(x2, y2, x1, y2, r, g, b, alpha);
    drawLine(x1, y2, x1, y1, r, g, b, alpha);
}

void VisualizationInterface::drawText(float x, float y, const std::string& text,
                                       float r, float g, float b, float alpha) {
    // Simple text drawing - log it for now
    std::string logMsg = "Text: (" + std::to_string(x) + "," + std::to_string(y) + ") \"" + text + "\" color: (" +
                        std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + ")";
    NLM_LOG_INFO(logMsg);
}

void VisualizationInterface::drawMemoryNeuron(float x, float y, float activation) {
    // Draw as diamond shape for memory neurons
    float size = neuronRadius_ * (1.0f + activation);
    
    drawLine(x, y - size, x + size, y, 1.0f, 1.0f, 0.0f, 0.8f);  // Top edge
    drawLine(x + size, y, x, y + size, 1.0f, 1.0f, 0.0f, 0.8f);  // Right edge
    drawLine(x, y + size, x - size, y, 1.0f, 1.0f, 0.0f, 0.8f);  // Bottom edge
    drawLine(x - size, y, x, y - size, 1.0f, 1.0f, 0.0f, 0.8f);  // Left edge
    
    drawCircle(x, y, size * 0.3f, 1.0f, 1.0f, 0.0f, 0.8f * (0.5f + activation * 0.5f));
}

void VisualizationInterface::drawAttentionNeuron(float x, float y, float activation) {
    // Draw with glowing effect for attention winners
    float baseSize = neuronRadius_ * (1.0f + activation);
    
    // Draw outer glow
    for (int i = 3; i >= 0; --i) {
        float glowSize = baseSize + i * 2.0f;
        float glowOpacity = 0.1f * (4.0f - i) * (0.5f + activation * 0.5f);
        drawCircle(x, y, glowSize, 1.0f, 1.0f, 1.0f, glowOpacity);
    }
    
    // Draw main neuron
    drawCircle(x, y, baseSize, 0.0f, 1.0f, 0.0f, 0.9f);
    
    // Draw cross for emphasis
    drawLine(x - baseSize * 0.7f, y, x + baseSize * 0.7f, y, 0.0f, 1.0f, 0.0f, 0.9f);
    drawLine(x, y - baseSize * 0.7f, x, y + baseSize * 0.7f, 0.0f, 1.0f, 0.0f, 0.9f);
}

void VisualizationInterface::drawCoordinateSystem() {
    // Draw coordinate system with proper scaling
    float axisThickness = 2.0f;
    
    // X-axis
    drawLine(regionMinX_, 0, regionMaxX_, 0, 0.8f, 0.8f, 0.8f, axisThickness);
    
    // Y-axis  
    drawLine(0, regionMinY_, 0, regionMaxY_, 0.8f, 0.8f, 0.8f, axisThickness);
    
    // Arrow heads
    drawLine(regionMaxX_ - 10, -5, regionMaxX_, 0, 0.8f, 0.8f, 0.8f, axisThickness);
    drawLine(regionMaxX_ - 10, 5, regionMaxX_, 0, 0.8f, 0.8f, 0.8f, axisThickness);
    
    drawLine(5, regionMaxY_, 0, regionMaxY_ - 10, 0.8f, 0.8f, 0.8f, axisThickness);
    drawLine(-5, regionMaxY_, 0, regionMaxY_ - 10, 0.8f, 0.8f, 0.8f, axisThickness);
    
    // Origin marker
    drawCircle(0, 0, 5, 1.0f, 1.0f, 1.0f, axisThickness);
}

void VisualizationInterface::clearScreen(float r, float g, float b, float alpha) {
    // Clear screen background
    NLM_LOG_INFO("Clear screen: background color (" + std::to_string(r) + "," + std::to_string(g) +
                 "," + std::to_string(b) + "," + std::to_string(alpha) + ")");
}

void VisualizationInterface::setZoom(float zoom) {
    zoomLevel_ = zoom;
    NLM_LOG_INFO("VisualizationInterface zoom set to: " + std::to_string(zoomLevel_));
}

void VisualizationInterface::panView(float dx, float dy) {
    viewOffsetX_ += dx;
    viewOffsetY_ += dy;
    NLM_LOG_INFO("VisualizationInterface pan: dx=" + std::to_string(dx) + ", dy=" + std::to_string(dy));
}

void VisualizationInterface::toggleAnimation() {
    animationEnabled_ = !animationEnabled_;
    NLM_LOG_INFO("Animation " + std::string(animationEnabled_ ? "enabled" : "disabled"));
}

void VisualizationInterface::toggleShowConnections() {
    showConnections_ = !showConnections_;
    NLM_LOG_INFO("Connections display " + std::string(showConnections_ ? "enabled" : "disabled"));
}

void VisualizationInterface::toggleShowMemoryNeurons() {
    showMemoryNeurons_ = !showMemoryNeurons_;
    NLM_LOG_INFO("Memory neurons display " + std::string(showMemoryNeurons_ ? "enabled" : "disabled"));
}

void VisualizationInterface::toggleShowAttentionWinners() {
    showAttentionWinners_ = !showAttentionWinners_;
    NLM_LOG_INFO("Attention winners display " + std::string(showAttentionWinners_ ? "enabled" : "disabled"));
}

void VisualizationInterface::toggleShowPredictions() {
    showPredictions_ = !showPredictions_;
    NLM_LOG_INFO("Predictions display " + std::string(showPredictions_ ? "enabled" : "disabled"));
}

void VisualizationInterface::reset() {
    // Reset view
    zoomLevel_ = 1.0f;
    viewOffsetX_ = 0.0f;
    viewOffsetY_ = 0.0f;
    animationTime_ = 0.0f;
    
    showConnections_ = true;
    showMemoryNeurons_ = true;
    showAttentionWinners_ = false;
    showPredictions_ = false;
    animationEnabled_ = false;
    
    NLM_LOG_INFO("VisualizationInterface reset to default state");
}