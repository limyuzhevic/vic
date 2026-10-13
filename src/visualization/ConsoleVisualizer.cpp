#include "ConsoleVisualizer.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>

namespace nlm {

struct ConsoleVisualizer::Impl {
    bool initialized = false;
    ConsoleVisualizationMode mode = ConsoleVisualizationMode::Activity;
    int refreshRate = 30;  // FPS
    size_t maxNeuronsToDisplay = 100;
    bool highlightActiveOnly = true;
    bool showStatistics = true;
    float animationSpeed = 1.0f;
    std::chrono::system_clock::time_point lastRenderTime;
    mutable std::mutex renderMutex;
    
    // Visualization state
    double currentTime = 0.0;
    int frameCount = 0;
    std::chrono::duration<double> lastFpsUpdate;
    double fps = 0.0;
    
    // Brain reference for visualization
    std::shared_ptr<Brain> currentBrain;
    
    // Cached neuron and synapse data
    std::vector<Neuron*> visibleNeurons;
    std::vector<Synapse*> visibleSynapses;
    
    // Console colors (ANSI escape codes)
    struct Colors {
        std::string reset = "\033[0m";
        std::string red = "\033[31m";
        std::string green = "\033[32m";
        std::string yellow = "\033[33m";
        std::string blue = "\033[34m";
        std::string magenta = "\033[35m";
        std::string cyan = "\033[36m";
        std::string white = "\033[37m";
        std::string gray = "\033[90m";
        std::string darkGray = "\033[90m";
        std::string brightRed = "\033[91m";
        std::string brightGreen = "\033[92m";
        std::string brightYellow = "\033[93m";
        std::string brightBlue = "\033[94m";
        std::string brightMagenta = "\033[95m";
        std::string brightCyan = "\033[96m";
        std::string brightWhite = "\033[97m";
    };
    
    Colors colors;
};

ConsoleVisualizer::ConsoleVisualizer() : pImpl(std::make_unique<Impl>()) {
}

ConsoleVisualizer::~ConsoleVisualizer() = default;

bool ConsoleVisualizer::initialize() {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    
    NLM_LOG_INFO("Initializing ConsoleVisualizer...");
    
    pImpl->initialized = true;
    pImpl->lastRenderTime = std::chrono::system_clock::now();
    pImpl->lastFpsUpdate = std::chrono::duration<double>::zero();
    pImpl->frameCount = 0;
    
    NLM_LOG_INFO("ConsoleVisualizer initialized successfully");
    return true;
}

void ConsoleVisualizer::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    
    NLM_LOG_INFO("Shutting down ConsoleVisualizer");
    pImpl->initialized = false;
    pImpl->currentBrain.reset();
    clearScreen();
}

void ConsoleVisualizer::update(double time, const std::shared_ptr<Brain>& brain) {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    
    pImpl->currentBrain = brain;
    pImpl->currentTime = time;
    
    // Update FPS counter
    pImpl->frameCount++;
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->lastFpsUpdate);
    
    if (elapsed.count() >= 1.0) {  // Update FPS every second
        pImpl->fps = pImpl->frameCount / elapsed.count();
        pImpl->frameCount = 0;
        pImpl->lastFpsUpdate = now;
    }
}

void ConsoleVisualizer::render() {
    if (!pImpl->initialized || !pImpl->currentBrain) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    
    clearScreen();
    
    switch (pImpl->mode) {
        case ConsoleVisualizationMode::Basic:
            drawNeuronGrid(pImpl->currentBrain->getAllNeurons());
            break;
        case ConsoleVisualizationMode::Activity:
            drawActivityHeatmap(pImpl->currentBrain->getAllNeurons());
            break;
        case ConsoleVisualizationMode::Network:
            drawNetworkConnections(pImpl->currentBrain->getAllSynapses());
            break;
        case ConsoleVisualizationMode::Learning:
            drawActivityHeatmap(pImpl->currentBrain->getAllNeurons());
            break;
        case ConsoleVisualizationMode::Detailed:
            drawNeuronGrid(pImpl->currentBrain->getAllNeurons());
            if (pImpl->showStatistics) {
                drawStatistics(pImpl->currentBrain, pImpl->currentTime);
            }
            break;
    }
    
    if (pImpl->showStatistics) {
        drawPerformanceMetrics();
    }
    
    updateDisplay();
}

bool ConsoleVisualizer::isInitialized() const {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    return pImpl->initialized;
}

void ConsoleVisualizer::setMode(VisualizationMode mode) {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    
    // Convert VisualizationMode to ConsoleVisualizationMode
    switch (mode) {
        case VisualizationMode::Basic:
            pImpl->mode = ConsoleVisualizationMode::Basic;
            break;
        case VisualizationMode::3D:
            pImpl->mode = ConsoleVisualizationMode::Detailed;
            break;
        case VisualizationMode::Interactive:
            pImpl->mode = ConsoleVisualizationMode::Network;
            break;
        default:
            pImpl->mode = ConsoleVisualizationMode::Activity;
            break;
    }
}

void ConsoleVisualizer::setScale(float scale) {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    // Scale would affect rendering size (console-based scaling is limited)
    NLM_LOG_INFO("ConsoleVisualizer scale set to: " + std::to_string(scale));
}

void ConsoleVisualizer::setColorScheme(const std::string& scheme) {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    NLM_LOG_INFO("ConsoleVisualizer color scheme set to: " + scheme);
    // Color schemes would modify the colors used in rendering
}

void ConsoleVisualizer::setRefreshRate(int fps) {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    pImpl->refreshRate = fps;
    double interval = 1.0 / fps;
    NLM_LOG_INFO("ConsoleVisualizer refresh rate set to: " + std::to_string(fps) + " FPS");
}

void ConsoleVisualizer::setMaxNeuronsToDisplay(size_t maxNeurons) {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    pImpl->maxNeuronsToDisplay = maxNeurons;
}

void ConsoleVisualizer::setHighlightActiveOnly(bool highlightActive) {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    pImpl->highlightActiveOnly = highlightActive;
}

void ConsoleVisualizer::setShowStatistics(bool showStats) {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    pImpl->showStatistics = showStats;
}

void ConsoleVisualizer::setAnimationSpeed(float speed) {
    std::lock_guard<std::mutex> lock(pImpl->renderMutex);
    pImpl->animationSpeed = speed;
}

void ConsoleVisualizer::clearScreen() const {
    std::cout << "\033[2J\033[H";  // Clear screen and move cursor to top-left
}

void ConsoleVisualizer::drawNeuronGrid(const std::vector<Neuron*>& neurons) const {
    int screenWidth, screenHeight;
    // Get console dimensions (simplified - assume reasonable size)
    screenWidth = 120;
    screenHeight = 40;
    
    // Select neurons to display (limit for performance)
    std::vector<Neuron*> displayNeurons = neurons;
    if (displayNeurons.size() > pImpl->maxNeuronsToDisplay) {
        // Sample neurons to display
        size_t step = displayNeurons.size() / pImpl->maxNeuronsToDisplay;
        std::vector<Neuron*> sampled;
        sampled.reserve(pImpl->maxNeuronsToDisplay);
        for (size_t i = 0; i < displayNeurons.size() && sampled.size() < pImpl->maxNeuronsToDisplay; i += step) {
            sampled.push_back(displayNeurons[i]);
        }
        displayNeurons = sampled;
    }
    
    // Draw neurons in a grid
    int neuronsPerRow = std::min(static_cast<size_t>(screenWidth / 4), displayNeurons.size());
    int neuronsPerCol = (displayNeurons.size() + neuronsPerRow - 1) / neuronsPerRow;
    
    for (size_t i = 0; i < displayNeurons.size(); ++i) {
        int row = static_cast<int>(i / neuronsPerRow);
        int col = static_cast<int>(i % neuronsPerRow);
        
        int x = col * 4;
        int y = row * 2;
        
        // Limit to screen bounds
        if (row < screenHeight / 2 && col < screenWidth / 4) {
            drawNeuron(displayNeurons[i], x, y, 3);
        }
    }
    
    // Draw title
    drawText(2, 0, "NLM Console Visualization - Basic Mode", pImpl->colors.brightCyan);
}

void ConsoleVisualizer::drawActivityHeatmap(const std::vector<Neuron*>& neurons) const {
    clearScreen();
    
    int screenWidth = 100;
    int screenHeight = 30;
    
    // Calculate activity range
    float maxActivity = 0.0f;
    for (Neuron* neuron : neurons) {
        float activity = neuron->getFiringRate();
        if (activity > maxActivity) maxActivity = activity;
    }
    
    // Draw heatmap
    for (int row = 0; row < screenHeight; ++row) {
        for (int col = 0; col < screenWidth; ++col) {
            // Map position to neuron index
            size_t neuronIndex = static_cast<size_t>((row * screenWidth + col) * neurons.size()) / (screenHeight * screenWidth);
            if (neuronIndex >= neurons.size()) continue;
            
            Neuron* neuron = neurons[neuronIndex];
            float activity = neuron->getFiringRate();
            
            // Map activity to color intensity
            if (maxActivity > 0.0f) {
                float intensity = activity / maxActivity;
                if (intensity > 0.8f) {
                    std::cout << pImpl->colors.brightRed;
                } else if (intensity > 0.5f) {
                    std::cout << pImpl->colors.yellow;
                } else if (intensity > 0.2f) {
                    std::cout << pImpl->colors.brightGreen;
                } else {
                    std::cout << pImpl->colors.blue;
                }
            }
            
            std::cout << "#";
            std::cout << pImpl->colors.reset;
        }
        std::cout << "\n";
    }
    
    // Draw statistics
    drawText(2, screenHeight + 1, "Activity Heatmap", pImpl->colors.brightCyan);
    drawText(2, screenHeight + 2, "Max Activity: " + std::to_string(maxActivity), pImpl->colors.white);
    drawText(2, screenHeight + 3, "Total Neurons: " + std::to_string(neurons.size()), pImpl->colors.white);
}

void ConsoleVisualizer::drawNetworkConnections(const std::vector<Synapse*>& synapses) const {
    clearScreen();
    
    int screenWidth = 80;
    int screenHeight = 25;
    
    // Draw synaptic connections
    for (size_t i = 0; i < std::min(static_cast<size_t>(screenHeight - 2), synapses.size()); ++i) {
        Synapse* synapse = synapses[i];
        
        // Get source and destination neurons (simplified - would need actual neuron references)
        // For now, just draw a simple connection indicator
        std::cout << pImpl->colors.gray << "Synapse " << i << ": " << std::fixed << std::setprecision(3) 
                  << synapse->getWeight() << " (LTP: " 
                  << (synapse->getPlasticityFlags().enableSTDP ? "Yes" : "No") << ")" << pImpl->colors.reset;
        std::cout << "\n";
    }
    
    drawText(2, 0, "NLM Console Visualization - Network Connections", pImpl->colors.brightCyan);
}

void ConsoleVisualizer::drawStatistics(const std::shared_ptr<Brain>& brain, double time) const {
    if (!brain) return;
    
    int screenHeight = 40;
    int currentLine = screenHeight + 2;
    
    drawText(currentLine++, 2, "Brain Statistics:", pImpl->colors.brightCyan);
    drawText(currentLine++, 2, "  Time: " + std::to_string(time) + "s", pImpl->colors.white);
    drawText(currentLine++, 2, "  Total Neurons: " + std::to_string(brain->getTotalNeuronCount()), pImpl->colors.white);
    drawText(currentLine++, 2, "  Total Synapses: " + std::to_string(brain->getTotalSynapseCount()), pImpl->colors.white);
    drawText(currentLine++, 2, "  Active Neurons: " + std::to_string(brain->getFiringNeuronCount()), pImpl->colors.white);
    drawText(currentLine++, 2, "  Total Spikes: " + std::to_string(brain->getTotalSpikeCount()), pImpl->colors.white);
    drawText(currentLine++, 2, "  Current Step: " + std::to_string(brain->getCurrentStep()), pImpl->colors.white);
    drawText(currentLine++, 2, "  Regions: " + std::to_string(brain->getRegionCount()), pImpl->colors.white);
}

void ConsoleVisualizer::drawPerformanceMetrics() const {
    int screenHeight = 60;  // Assume terminal height
    
    drawText(screenHeight - 8, 2, "Performance:", pImpl->colors.brightYellow);
    drawText(screenHeight - 7, 2, "  FPS: " + std::to_string(pImpl->fps), pImpl->colors.white);
    drawText(screenHeight - 6, 2, "  Mode: " + (pImpl->mode == ConsoleVisualizationMode::Basic ? "Basic" :
                                        pImpl->mode == ConsoleVisualizationMode::Activity ? "Activity" :
                                        pImpl->mode == ConsoleVisualizationMode::Network ? "Network" :
                                        pImpl->mode == ConsoleVisualizationMode::Learning ? "Learning" : "Detailed"), pImpl->colors.white);
    drawText(screenHeight - 5, 2, "  Neurons Displayed: " + std::to_string(pImpl->visibleNeurons.size()), pImpl->colors.white);
    drawText(screenHeight - 4, 2, "  Synapses Displayed: " + std::to_string(pImpl->visibleSynapses.size()), pImpl->colors.white);
}

void ConsoleVisualizer::drawNeuron(Neuron* neuron, int x, int y, int size) const {
    if (!neuron) return;
    
    std::string color = getNeuronColor(neuron);
    
    for (int dy = -size; dy <= size; ++dy) {
        for (int dx = -size; dx <= size; ++dx) {
            int screenX = x + dx;
            int screenY = y + dy;
            
            if (screenX >= 0 && screenX < 120 && screenY >= 0 && screenY < 40) {
                std::cout << color << "#";
            }
        }
        if (y + dy < 39) std::cout << "\n";
    }
    std::cout << pImpl->colors.reset;
}

void ConsoleVisualizer::drawSynapse(Synapse* synapse, int x1, int y1, int x2, int y2) const {
    std::string color = getSynapseColor(synapse);
    
    // Simplified line drawing
    for (int t = 0; t < 10; ++t) {
        float t1 = static_cast<float>(t) / 10.0f;
        int x = static_cast<int>(x1 * (1.0f - t1) + x2 * t1);
        int y = static_cast<int>(y1 * (1.0f - t1) + y2 * t1);
        
        if (x >= 0 && x < 120 && y >= 0 && y < 40) {
            // Move to position (simplified)
            std::cout << "\033[" << y << ";" << x << "H" << color << "|" << pImpl->colors.reset;
        }
    }
}

void ConsoleVisualizer::drawText(int x, int y, const std::string& text, const std::string& color) const {
    std::cout << "\033[" << y << ";" << x << "H" << color << text << pImpl->colors.reset;
}

std::string ConsoleVisualizer::getNeuronColor(Neuron* neuron) const {
    if (!neuron) return pImpl->colors.gray;
    
    if (pImpl->highlightActiveOnly && neuron->getFiringState() != FiringState::Spiking) {
        return pImpl->colors.gray;  // Dim inactive neurons
    }
    
    // Color based on neuron type
    switch (neuron->getType()) {
        case NeuronType::Sensory:
            return pImpl->colors.brightBlue;
        case NeuronType::Motor:
            return pImpl->colors.brightGreen;
        case NeuronType::Internal:
            return pImpl->colors.brightYellow;
        case NeuronType::Modulatory:
            return pImpl->colors.brightMagenta;
        default:
            return pImpl->colors.white;
    }
}

std::string ConsoleVisualizer::getActivityColor(float activity) const {
    if (activity > 0.8f) return pImpl->colors.brightRed;
    if (activity > 0.5f) return pImpl->colors.yellow;
    if (activity > 0.2f) return pImpl->colors.brightGreen;
    return pImpl->colors.blue;
}

std::string ConsoleVisualizer::getSynapseColor(Synapse* synapse) const {
    if (!synapse) return pImpl->colors.gray;
    
    float weight = synapse->getWeight();
    if (weight > 0.5f) return pImpl->colors.brightGreen;
    if (weight < -0.5f) return pImpl->colors.brightRed;
    return pImpl->colors.yellow;
}

void ConsoleVisualizer::flushOutput() const {
    std::cout.flush();
}

void ConsoleVisualizer::updateDisplay() const {
    // Synchronize with refresh rate
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->lastRenderTime);
    
    if (elapsed.count() < 1.0 / pImpl->refreshRate) {
        std::this_thread::sleep_for(std::chrono::duration<double>(1.0 / pImpl->refreshRate - elapsed.count()));
    }
    
    pImpl->lastRenderTime = std::chrono::system_clock::now();
}

} // namespace nlm
