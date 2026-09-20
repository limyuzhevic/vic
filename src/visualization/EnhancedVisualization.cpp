#include "EnhancedVisualization.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/Performance.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <cmath>

namespace nlm {

struct EnhancedVisualization::Impl {
    // Window and rendering
    sf::RenderWindow* window;
    sf::View view;
    sf::Font font;
    
    // State management
    bool active;
    double updateRate;
    bool paused;
    double simulationSpeed;
    bool debugMode;
    bool showGrid;
    bool showLabels;
    int colorScheme;
    
    // Data storage
    std::vector<std::vector<float>> neuronPositions;
    std::vector<std::vector<int>> neuronConnections;
    std::vector<float> neuronFiringRates;
    std::vector<double> neuronMembranePotentials;
    std::vector<int> neuronTypes;
    
    // Spike data
    std::vector<SpikeEvent> spikeEvents;
    std::vector<std::vector<double>> spikeHistory;
    
    // Performance and profiling
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double simulationTime;
        int frameRate;
        std::vector<double> bottleneckTimers;
    };
    
    PerformanceMetrics perfMetrics;
    
    // Event timeline
    struct TimelineEvent {
        double timestamp;
        std::string type;
        std::string description;
        sf::Color color;
    };
    std::vector<TimelineEvent> timelineEvents;
    
    // Debugging info
    struct DebugInfo {
        int selectedNeuronId;
        int hoveredNeuronId;
        double zoomLevel;
        sf::Vector2f panOffset;
        std::map<int, std::string> neuronInfo;
        std::map<int, std::string> synapseInfo;
    } debugInfo;
    
    // Colors and styling
    struct Colors {
        sf::Color neuronBackground;
        sf::Color neuronActive;
        sf::Color neuronInhibited;
        sf::Color synapseExcitatory;
        sf::Color synapseInhibitory;
        sf::Color spikeColor;
        sf::Color gridColor;
        sf::Color textColor;
        sf::Color backgroundColor;
    } colors;
    
    // Performance optimization
    sf::VertexArray connectionsBatch;
    sf::VertexArray neuronsBatch;
    sf::VertexArray spikesBatch;
    
    // Initialization
    Impl() : active(false), updateRate(30.0), paused(false), 
             simulationSpeed(1.0), debugMode(false), showGrid(true), 
             showLabels(true), colorScheme(0), window(nullptr), 
             selectedNeuronId(-1), hoveredNeuronId(-1), zoomLevel(1.0),
             panOffset(0, 0) {
        // Initialize colors
        colors.neuronBackground = sf::Color(50, 50, 80, 200);
        colors.neuronActive = sf::Color(255, 100, 100);
        colors.neuronInhibited = sf::Color(100, 100, 255);
        colors.synapseExcitatory = sf::Color(200, 200, 255, 150);
        colors.synapseInhibitory = sf::Color(255, 200, 200, 150);
        colors.spikeColor = sf::Color(255, 255, 0);
        colors.gridColor = sf::Color(100, 100, 100, 100);
        colors.textColor = sf::Color(240, 240, 240);
        colors.backgroundColor = sf::Color(30, 30, 40);
        
        // Setup performance metrics
        perfMetrics.cpuUsage = 0.0;
        perfMetrics.memoryUsage = 0.0;
        perfMetrics.simulationTime = 0.0;
        perfMetrics.frameRate = 0;
        perfMetrics.bottleneckTimers = std::vector<double>(10, 0.0);
    }
    
    ~Impl() {
        if (window) delete window;
    }
};

EnhancedVisualization::EnhancedVisualization() : pImpl(std::make_unique<Impl>()) {
}

EnhancedVisualization::~EnhancedVisualization() = default;

bool EnhancedVisualization::initialize(int width, int height, const std::string& title) {
    try {
        // Create window with SFML
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        sf::ContextSettings settings;
        settings.antialiasingLevel = 8;
        
        window = new sf::RenderWindow(desktop, title, sf::Style::Default, settings);
        window->setFramerateLimit(144);
        window->setVerticalSyncEnabled(true);
        
        // Load font (or create fallback)
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            // Create a simple font from characters
            font.loadFromMemory(nullptr, 0);
        }
        
        pImpl->active = true;
        pImpl->view.setSize(width, height);
        pImpl->view.setCenter(width / 2.0f, height / 2.0f);
        
        Logger::getInstance().log("Enhanced visualization initialized successfully", LogLevel::INFO);
        return true;
        
    } catch (const std::exception& e) {
        Logger::getInstance().log(std::string("Failed to initialize visualization: ") + e.what(), LogLevel::ERROR);
        pImpl->active = false;
        return false;
    }
}

void EnhancedVisualization::update() {
    if (!pImpl->active || !window || pImpl->paused) {
        return;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Update visualization data
    updatePerformanceMetrics();
    updateMemoryUsage();
    
    // Process SFML events
    handleInput();
    
    // Update neuron positions and states (from brain data)
    // This would integrate with the actual brain simulation
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    pImpl->perfMetrics.simulationTime = duration.count() / 1000.0;
    pImpl->perfMetrics.frameRate = static_cast<int>(1000000.0 / std::max(1.0, pImpl->perfMetrics.simulationTime));
}

void EnhancedVisualization::render() {
    if (!pImpl->active || !window) {
        return;
    }
    
    window->clear(pImpl->colors.backgroundColor);
    window->setView(pImpl->view);
    
    // Render grid if enabled
    if (pImpl->showGrid) {
        renderGrid();
    }
    
    // Render neural network
    if (!neuronPositions.empty()) {
        renderNeuralNetwork();
    }
    
    // Render spikes
    if (!pImpl->spikeEvents.empty()) {
        renderSpikes();
    }
    
    // Render weight matrix if debugging
    if (pImpl->debugMode) {
        // Would need brain data for this
    }
    
    // Render UI elements
    renderUI();
    renderDebugInfo();
    renderPerformanceDashboard();
    renderEventTimeline();
    
    window->display();
}

void EnhancedVisualization::close() {
    pImpl->active = false;
    if (window) {
        window->close();
        delete window;
        window = nullptr;
    }
    Logger::getInstance().log("Enhanced visualization closed", LogLevel::INFO);
}

void EnhancedVisualization::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    if (window) {
        window->setFramerateLimit(static_cast<unsigned int>(hz));
    }
}

void EnhancedVisualization::setSimulationSpeed(double speed) {
    pImpl->simulationSpeed = speed;
}

void EnhancedVisualization::togglePause() {
    pImpl->paused = !pImpl->paused;
}

bool EnhancedVisualization::isPaused() const {
    return pImpl->paused;
}

void EnhancedVisualization::visualizeNetwork(const Brain& brain) {
    visualizeNetworkGraph(brain);
    visualizeRegionConnections(brain);
}

void EnhancedVisualization::visualizeActivity(const Brain& brain) {
    visualizeFiringRateHeatmap(brain);
    visualizeSpikingActivity(brain);
    visualizeSynapticActivity(brain);
}

void EnhancedVisualization::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    pImpl->spikeEvents = spikes;
    renderSpikeRasterPlot(spikes);
    renderSpikingHeatmap(spikes, 10.0); // 10 second window
}

void EnhancedVisualization::visualizeWeights(const Brain& brain) {
    renderWeightMatrix(brain);
    renderConnectionDensityMap(brain);
}

void EnhancedVisualization::inspectNeuralState(const Brain& brain) {
    // Display detailed information about neurons and synapses
    if (pImpl->debugInfo.selectedNeuronId >= 0 && 
        pImpl->debugInfo.selectedNeuronId < brain.getTotalNeuronCount()) {
        auto neuron = brain.getNeuron(pImpl->debugInfo.selectedNeuronId);
        displayNeuronDetails(neuron);
    }
    
    // Display synapse information if available
}

void EnhancedVisualization::updateMemoryUsage() {
    // Get memory statistics from performance system
    auto& perf = Performance::getInstance();
    pImpl->perfMetrics.memoryUsage = perf.getMemoryUsageMB();
}

void EnhancedVisualization::renderMemoryProfiler() {
    // Render memory usage visualization
}

void EnhancedVisualization::updatePerformanceMetrics() {
    // Update performance metrics from performance system
    auto& perf = Performance::getInstance();
    pImpl->perfMetrics.cpuUsage = perf.getCPUUsage();
    detectPerformanceBottlenecks();
}

void EnhancedVisualization::detectPerformanceBottlenecks() {
    // Simple bottleneck detection based on operation times
    for (auto& timer : pImpl->perfMetrics.bottleneckTimers) {
        timer *= 0.9; // Decay
    }
    
    // Would integrate with actual performance profiling
}

void EnhancedVisualization::addEvent(const std::string& eventType, const std::string& description, double timestamp) {
    TimelineEvent event;
    event.timestamp = timestamp;
    event.type = eventType;
    event.description = description;
    
    if (eventType == "spike") {
        event.color = pImpl->colors.spikeColor;
    } else if (eventType == "error") {
        event.color = sf::Color::Red;
    } else if (eventType == "performance") {
        event.color = sf::Color::Green;
    } else {
        event.color = sf::Color::White;
    }
    
    pImpl->timelineEvents.push_back(event);
    
    // Keep only last 100 events
    if (pImpl->timelineEvents.size() > 100) {
        pImpl->timelineEvents.erase(pImpl->timelineEvents.begin());
    }
}

void EnhancedVisualization::toggleDebugMode() {
    pImpl->debugMode = !pImpl->debugMode;
}

void EnhancedVisualization::toggleGrid(bool show) {
    pImpl->showGrid = show;
}

void EnhancedVisualization::toggleLabels(bool show) {
    pImpl->showLabels = show;
}

void EnhancedVisualization::setColorScheme(int scheme) {
    pImpl->colorScheme = scheme;
    // Update colors based on scheme
}

void EnhancedVisualization::exportVisualizationData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "{" << std::endl;
    file << "  \"timestamp\": \"" << std::chrono::duration_cast<std::chrono::milliseconds>
         (std::chrono::high_resolution_clock::now().time_since_epoch()).count() 
         << "\"" << std::endl;
    file << "  \"neuronCount\": " << neuronPositions.size() << std::endl;
    file << "  \"spikeCount\": " << pImpl->spikeEvents.size() << std::endl;
    file << "  \
firingRates\": [";
    
    for (size_t i = 0; i < neuronFiringRates.size(); ++i) {
        file << neuronFiringRates[i];
        if (i < neuronFiringRates.size() - 1) file << ", ";
    }
    
    file << "]" << std::endl;
    file << "  \"events\": [";
    
    for (size_t i = 0; i < pImpl->timelineEvents.size(); ++i) {
        auto& event = pImpl->timelineEvents[i];
        file << "{\"timestamp\": " << event.timestamp 
             << ", \"type\": \"" << event.type << "\", \"description\": \"" 
             << event.description << "\"}";
        if (i < pImpl->timelineEvents.size() - 1) file << ", ";
    }
    
    file << "]" << std::endl;
    file << "}" << std::endl;
    
    file.close();
    Logger::getInstance().log("Visualization data exported to: " + filename, LogLevel::INFO);
}

// Helper rendering methods
void EnhancedVisualization::renderGrid() {
    // Render grid lines
    sf::RectangleShape line;
    line.setFillColor(pImpl->colors.gridColor);
    line.setSize(sf::Vector2f(window->getSize().x, 1));
    
    // Horizontal lines
    for (int y = 0; y <= window->getSize().y; y += 50) {
        line.setPosition(0, y);
        window->draw(line);
    }
    
    // Vertical lines
    line.setSize(sf::Vector2f(1, window->getSize().y));
    for (int x = 0; x <= window->getSize().x; x += 50) {
        line.setPosition(x, 0);
        window->draw(line);
    }
}

void EnhancedVisualization::renderNeuralNetwork() {
    // Render neurons
    for (size_t i = 0; i < neuronPositions.size(); ++i) {
        renderNeuron(neuronPositions[i], neuronFiringRates[i], neuronTypes[i]);
    }
    
    // Render connections
    for (size_t i = 0; i < neuronConnections.size(); ++i) {
        for (size_t j = 0; j < neuronConnections[i].size(); ++j) {
            if (neuronConnections[i][j] > 0) {
                renderConnection(neuronPositions[i], neuronPositions[j], 
                               neuronConnections[i][j]);
            }
        }
    }
}

void EnhancedVisualization::renderNeuron(const std::vector<float>& position, float firingRate, int type) {
    sf::CircleShape neuron(5 * (1.0f + firingRate / 50.0f)); // Scale by firing rate
    neuron.setPosition(position[0] - neuron.getRadius(), 
                      position[1] - neuron.getRadius());
    
    if (type == 1) { // Inhibitory
        neuron.setFillColor(pImpl->colors.neuronInhibited);
    } else if (firingRate > 20.0f) { // Active
        neuron.setFillColor(pImpl->colors.neuronActive);
    } else {
        neuron.setFillColor(pImpl->colors.neuronBackground);
    }
    
    neuron.setOutlineColor(sf::Color::White);
    neuron.setOutlineThickness(1);
    window->draw(neuron);
}

void EnhancedVisualization::renderConnection(const std::vector<float>& from, const std::vector<float>& to, int strength) {
    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(from[0], from[1]), 
                  strength > 0 ? pImpl->colors.synapseExcitatory : pImpl->colors.synapseInhibitory),
        sf::Vertex(sf::Vector2f(to[0], to[1]), 
                  strength > 0 ? pImpl->colors.synapseExcitatory : pImpl->colors.synapseInhibitory)
    };
    
    line[0].color.a = static_cast<sf::Uint8>(255 * std::min(1.0f, strength / 10.0f));
    line[1].color.a = static_cast<sf::Uint8>(255 * std::min(1.0f, strength / 10.0f));
    
    window->draw(line, 2, sf::Lines);
}

void EnhancedVisualization::renderSpikes() {
    // Render spike events as dots or lines
    for (const auto& spike : pImpl->spikeEvents) {
        // Convert spike time to screen position
        float screenX = spike.timestamp * 100.0f; // Scale time
        float screenY = spike.neuronId % 100 * 10.0f; // Spread neurons vertically
        
        sf::CircleShape spikeDot(2);
        spikeDot.setPosition(screenX - 2, screenY - 2);
        spikeDot.setFillColor(pImpl->colors.spikeColor);
        window->draw(spikeDot);
    }
}

void EnhancedVisualization::renderUI() {
    // Render basic UI elements
    if (pImpl->showLabels) {
        sf::Text pausedText;
        pausedText.setFont(font);
        pausedText.setString(pImpl->paused ? "PAUSED" : "RUNNING");
        pausedText.setCharacterSize(20);
        pausedText.setFillColor(pImpl->colors.textColor);
        pausedText.setPosition(10, 10);
        window->draw(pausedText);
    }
}

void EnhancedVisualization::renderDebugInfo() {
    if (!pImpl->debugMode) return;
    
    // Render debug information
    std::stringstream ss;
    ss << "FPS: " << pImpl->perfMetrics.frameRate << std::endl;
    ss << "CPU: " << pImpl->perfMetrics.cpuUsage << "%" << std::endl;
    ss << "Memory: " << pImpl->perfMetrics.memoryUsage << " MB" << std::endl;
    ss << "Selected Neuron: " << pImpl->debugInfo.selectedNeuronId << std::endl;
    
    sf::Text debugText;
    debugText.setFont(font);
    debugText.setString(ss.str());
    debugText.setCharacterSize(14);
    debugText.setFillColor(pImpl->colors.textColor);
    debugText.setPosition(10, window->getSize().y - 150);
    window->draw(debugText);
}

void EnhancedVisualization::renderPerformanceDashboard() {
    // Render performance metrics visualization
    // Would draw graphs for CPU, memory, simulation time, etc.
}

void EnhancedVisualization::renderEventTimeline() {
    // Render timeline of events at the bottom of the screen
    sf::RectangleShape timelineBar;
    timelineBar.setSize(sf::Vector2f(window->getSize().x, 30));
    timelineBar.setPosition(0, window->getSize().y - 30);
    timelineBar.setFillColor(sf::Color(50, 50, 50, 200));
    window->draw(timelineBar);
    
    // Render events as dots on timeline
    for (const auto& event : pImpl->timelineEvents) {
        float x = event.timestamp * 10.0f; // Scale time
        if (x >= 0 && x < window->getSize().x) {
            sf::CircleShape eventDot(3);
            eventDot.setPosition(x - 3, window->getSize().y - 35);
            eventDot.setFillColor(event.color);
            window->draw(eventDot);
        }
    }
}

bool EnhancedVisualization::handleInput() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            close();
            return false;
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                togglePause();
            } else if (event.key.code == sf::Keyboard::D) {
                toggleDebugMode();
            } else if (event.key.code == sf::Keyboard::G) {
                pImpl->showGrid = !pImpl->showGrid;
            }
        }
        
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.delta > 0) {
                processZoom(1);
            } else {
                processZoom(-1);
            }
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Check if clicked on a neuron
                sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
                processSelection(mousePos);
            }
        }
    }
    return true;
}

void EnhancedVisualization::processZoom(int delta) {
    pImpl->view.zoom(1.0f + delta * 0.1f);
}

void EnhancedVisualization::processPan(float dx, float dy) {
    pImpl->view.move(dx, dy);
}

void EnhancedVisualization::processSelection(sf::Vector2f mousePos) {
    // Check if mouse is over a neuron
    for (size_t i = 0; i < neuronPositions.size(); ++i) {
        float dx = mousePos.x - neuronPositions[i][0];
        float dy = mousePos.y - neuronPositions[i][1];
        if (dx * dx + dy * dy < 25) { // 5 pixel radius
            pImpl->debugInfo.selectedNeuronId = i;
            break;
        }
    }
}

void EnhancedVisualization::clearEvents() {
    pImpl->timelineEvents.clear();
}

void EnhancedVisualization::resetMetrics() {
    pImpl->perfMetrics = Impl::PerformanceMetrics();
}

} // namespace nlm
