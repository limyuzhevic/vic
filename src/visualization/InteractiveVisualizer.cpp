#include "InteractiveVisualizer.hpp"
#include "Brain.hpp"
#include "VisualizationInterface.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "core/Types/Types.hpp"
#include "development/DevelopmentSystem.hpp"
#include <ncurses.h>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <map>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {

struct InteractiveVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<VisualizationInterface> vizInterface;
    
    // State
    bool running;
    bool active;
    bool debugInfo;
    VisualizationMode mode;
    int speed;
    double zoom;
    std::pair<int, int> pan;
    
    // Selection
    std::vector<RegionId> selectedRegions;
    float activityThreshold;
    DevelopmentalStage stageFilter;
    
    // View focus
    std::string viewFocus;
    
    // Colors and rendering
    bool useColor;
    std::map<NeuronType, int> neuronColors;
    std::map<FiringState, int> firingColors;
    std::map<DevelopmentalStage, int> stageColors;
    
    // Metrics and history
    std::vector<std::chrono::system_clock::time_point> timeHistory;
    std::vector<double> neuralActivityHistory;
    std::vector<double> memoryStateHistory;
    std::vector<double> neuromodulationHistory;
    std::vector<double> connectivityHistory;
    
    // Command processing
    std::string currentCommand;
    std::vector<std::string> commandHistory;
    
    // Window dimensions
    int screenHeight;
    int screenWidth;
    int infoPanelWidth;
    int mainAreaHeight;
    int mainAreaWidth;
    
    // Timing
    double updateInterval;
    double lastUpdateTime;
    double simulationTime;
    
    // Threads
    std::thread updateThread;
    bool threadRunning;
    std::mutex mutex;
    
    // Performance
    int fps;
    double deltaTime;
    
    Impl() : running(false), active(false), debugInfo(true), mode(VisualizationMode::NetworkView),
             speed(1), zoom(1.0), pan(0, 0), activityThreshold(0.5), stageFilter(DevelopmentalStage::Initial),
             useColor(true), screenHeight(24), screenWidth(80), infoPanelWidth(20),
             mainAreaHeight(18), mainAreaWidth(60), updateInterval(0.1), lastUpdateTime(0.0),
             simulationTime(0.0), threadRunning(false), fps(60), deltaTime(0.016) {
        
        // Initialize colors
        if (useColor) {
            neuronColors[NeuronType::Excitatory] = COLOR_RED;
            neuronColors[NeuronType::Inhibitory] = COLOR_BLUE;
            neuronColors[NeuronType::Sensory] = COLOR_YELLOW;
            neuronColors[NeuronType::Motor] = COLOR_GREEN;
            neuronColors[NeuronType::Modulatory] = COLOR_MAGENTA;
            
            firingColors[FiringState::Resting] = COLOR_WHITE;
            firingColors[FiringState::Active] = COLOR_YELLOW;
            firingColors[FiringState::Firing] = COLOR_RED;
            firingColors[FiringState::Refractory] = COLOR_CYAN;
            
            stageColors[DevelopmentalStage::Initial] = COLOR_RED;
            stageColors[DevelopmentalStage::Development] = COLOR_YELLOW;
            stageColors[DevelopmentalStage::Mature] = COLOR_GREEN;
            stageColors[DevelopmentalStage::Aging] = COLOR_BLUE;
        }
    }
};

InteractiveVisualizer::InteractiveVisualizer() : pImpl(std::make_unique<Impl>()) {}

InteractiveVisualizer::~InteractiveVisualizer() {
    stop();
}

bool InteractiveVisualizer::initialize(std::shared_ptr<Brain> brain, std::shared_ptr<VisualizationInterface> vizInterface) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->brain = brain;
    pImpl->vizInterface = vizInterface;
    
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    
    if (has_colors()) {
        start_color();
        use_default_colors();
    }
    
    // Get screen dimensions
    getmaxyx(stdscr, pImpl->screenHeight, pImpl->screenWidth);
    
    if (pImpl->screenWidth < 70 || pImpl->screenHeight < 30) {
        NLM_LOG_WARNING("Screen too small for interactive visualization. Minimum 70x30 required.");
        // Try to continue with scaled values
        pImpl->infoPanelWidth = 15;
        pImpl->mainAreaHeight = 20;
        pImpl->mainAreaWidth = 55;
    }
    
    pImpl->active = true;
    pImpl->running = true;
    pImpl->threadRunning = true;
    
    // Initialize visualization interface
    if (pImpl->vizInterface) {
        pImpl->vizInterface->initialize();
    }
    
    NLM_LOG_INFO("InteractiveVisualizer initialized successfully");
    return true;
}

void InteractiveVisualizer::run() {
    // Start update thread
    pImpl->updateThread = std::thread([this]() {
        while (pImpl->threadRunning && pImpl->running) {
            double now = pImpl->simulationTime;
            
            {
                std::lock_guard<std::mutex> lock(pImpl->mutex);
                
                // Update metrics history
                if (pImpl->timeHistory.size() > 100) {
                    pImpl->timeHistory.erase(pImpl->timeHistory.begin());
                    pImpl->neuralActivityHistory.erase(pImpl->neuralActivityHistory.begin());
                    pImpl->memoryStateHistory.erase(pImpl->memoryStateHistory.begin());
                    pImpl->neuromodulationHistory.erase(pImpl->neuromodulationHistory.begin());
                    pImpl->connectivityHistory.erase(pImpl->connectivityHistory.begin());
                }
                
                // Record current state
                pImpl->timeHistory.push_back(std::chrono::system_clock::now());
                
                if (pImpl->brain) {
                    pImpl->neuralActivityHistory.push_back(
                        static_cast<double>(pImpl->brain->getFiringNeuronCount()) / 
                        std::max(1.0, static_cast<double>(pImpl->brain->getTotalNeuronCount()))
                    );
                    
                    // Get memory states (simplified - would need actual memory system access)
                    pImpl->memoryStateHistory.push_back(0.5); // Placeholder
                    
                    // Get neuromodulation levels (simplified)
                    pImpl->neuromodulationHistory.push_back(0.3); // Placeholder
                    
                    // Get connectivity patterns (simplified)
                    pImpl->connectivityHistory.push_back(
                        static_cast<double>(pImpl->brain->getTotalSynapseCount()) /
                        std::max(1.0, static_cast<double>(pImpl->brain->getTotalNeuronCount() * 5))
                    );
                }
                
                pImpl->simulationTime += pImpl->deltaTime * pImpl->speed;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
    });
    
    // Main event loop
    NLM_LOG_INFO("Starting interactive visualization mode. Press '?' for help.");
    
    while (pImpl->running) {
        clear();
        
        // Handle input
        int ch = getch();
        if (ch != ERR) {
            handleInput(ch);
        }
        
        // Update display
        render();
        
        // Calculate and display FPS
        refresh();
        
        // Small delay to prevent excessive CPU usage
        usleep(16667); // ~60 FPS
    }
    
    // Stop update thread
    pImpl->threadRunning = false;
    if (pImpl->updateThread.joinable()) {
        pImpl->updateThread.join();
    }
    
    // Cleanup
    if (pImpl->vizInterface) {
        pImpl->vizInterface->close();
    }
    
    endwin();
    NLM_LOG_INFO("Interactive visualization stopped");
}

void InteractiveVisualizer::stop() {
    if (pImpl->running) {
        pImpl->running = false;
        pImpl->threadRunning = false;
        NLM_LOG_INFO("Stopping interactive visualization...");
    }
}

bool InteractiveVisualizer::isRunning() const {
    return pImpl->running;
}

void InteractiveVisualizer::handleInput(int ch) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    switch (ch) {
        // Movement and navigation
        case KEY_UP:
            if (pImpl->mode == VisualizationMode::NetworkView) {
                pImpl->pan.first = std::max(0, pImpl->pan.first - 1);
            }
            break;
        case KEY_DOWN:
            if (pImpl->mode == VisualizationMode::NetworkView) {
                pImpl->pan.first = std::min(10, pImpl->pan.first + 1);
            }
            break;
        case KEY_LEFT:
            if (pImpl->mode == VisualizationMode::NetworkView) {
                pImpl->pan.second = std::max(0, pImpl->pan.second - 1);
            }
            break;
        case KEY_RIGHT:
            if (pImpl->mode == VisualizationMode::NetworkView) {
                pImpl->pan.second = std::min(20, pImpl->pan.second + 1);
            }
            break;
        
        // Zoom controls
        case '+':
        case '=':
            pImpl->zoom = std::min(3.0, pImpl->zoom + 0.1);
            break;
        case '-':
            pImpl->zoom = std::max(0.5, pImpl->zoom - 0.1);
            break;
        
        // Speed control
        case '8':
            adjustSpeed(1);
            break;
        case '2':
            adjustSpeed(-1);
            break;
        
        // Mode switching
        case '1':
            setMode(VisualizationMode::NetworkView);
            break;
        case '2':
            setMode(VisualizationMode::ActivityView);
            break;
        case '3':
            setMode(VisualizationMode::SpikeView);
            break;
        case '4':
            setMode(VisualizationMode::WeightView);
            break;
        case '5':
            setMode(VisualizationMode::MetricsView);
            break;
        
        // Selection and inspection
        case 'n': {
            // Select next region
            if (pImpl->brain) {
                auto regionIds = pImpl->brain->getRegionIds();
                if (!regionIds.empty()) {
                    RegionId current = pImpl->selectedRegions.empty() ? RegionId(0) : *pImpl->selectedRegions.rbegin();
                    auto it = std::find(regionIds.begin(), regionIds.end(), current);
                    if (it != regionIds.end()) {
                        ++it;
                        if (it == regionIds.end()) it = regionIds.begin();
                        selectRegion(*it);
                    }
                }
            }
            break;
        }
        
        // Neuron inspection
        case 'i': {
            // Inspect current selection
            if (!pImpl->selectedRegions.empty()) {
                RegionId regionId = pImpl->selectedRegions.back();
                if (pImpl->brain) {
                    if (auto* region = pImpl->brain->getRegion(regionId)) {
                        auto neurons = region->getAllNeurons();
                        if (!neurons.empty()) {
                            NLM_LOG_INFO("Region " + std::to_string(regionId) + 
                                       " neurons: " + std::to_string(neurons.size()) +
                                       " active: " + std::to_string(region->getFiringNeuronCount()));
                        }
                    }
                }
            }
            break;
        }
        
        // Parameter adjustment
        case '[':
            setActivityFilter(std::max(0.0f, pImpl->activityThreshold - 0.1f));
            break;
        case ']':
            setActivityFilter(std::min(1.0f, pImpl->activityThreshold + 0.1f));
            break;
        
        // Simulation control
        case ' ':  // Space bar
            // Pause/resume simulation
            break;
        
        // File operations
        case 's': {
            std::string filename = "viz_snapshot_" + std::to_string(pImpl->simulationTime) + ".txt";
            saveState(filename);
            break;
        }
        case 'e': {
            std::string filename = "viz_data_" + std::to_string(pImpl->simulationTime) + ".csv";
            exportData(filename, "csv");
            break;
        }
        
        // Debug and help
        case '=':
            toggleDebugInfo();
            break;
        case '?':
            // Show help
            break;
        
        // Exit
        case 'q':
        case 'Q':
            stop();
            break;
        
        // Default
        default:
            pImpl->currentCommand += static_cast<char>(ch);
            break;
    }
}

void InteractiveVisualizer::update(double dt) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->deltaTime = dt;
}

void InteractiveVisualizer::render() {
    clear();
    
    // Draw main visualization area
    drawMainArea();
    
    // Draw info panel
    drawInfoPanel();
    
    // Draw command line
    drawCommandLine();
    
    // Draw help if needed
    if (pImpl->debugInfo) {
        drawHelp();
    }
}

void InteractiveVisualizer::drawMainArea() {
    // Implementation depends on current mode
    switch (pImpl->mode) {
        case VisualizationMode::NetworkView:
            drawNetworkView();
            break;
        case VisualizationMode::ActivityView:
            drawActivityView();
            break;
        case VisualizationMode::SpikeView:
            drawSpikeView();
            break;
        case VisualizationMode::WeightView:
            drawWeightView();
            break;
        case VisualizationMode::MetricsView:
            drawMetricsView();
            break;
    }
}

void InteractiveVisualizer::drawInfoPanel() {
    // Draw metrics and status
    int y = 0;
    
    // Header
    if (pImpl->useColor) {
        attron(COLOR_PAIR(1)); // Highlight
    }
    mvprintw(y++, 0, "=== NLM Interactive Visualization ===");
    
    if (pImpl->useColor) {
        attroff(COLOR_PAIR(1));
    }
    
    // Basic stats
    y++;
    if (pImpl->brain) {
        mvprintw(y++, 0, "Neurons: %zu", pImpl->brain->getTotalNeuronCount());
        mvprintw(y++, 0, "Synapses: %zu", pImpl->brain->getTotalSynapseCount());
        mvprintw(y++, 0, "Firing: %zu (%.1f%%)", 
                 pImpl->brain->getFiringNeuronCount(),
                 static_cast<double>(pImpl->brain->getFiringNeuronCount()) /
                 std::max(1.0, static_cast<double>(pImpl->brain->getTotalNeuronCount())) * 100.0);
        mvprintw(y++, 0, "Total Spikes: %zu", pImpl->brain->getTotalSpikeCount());
        mvprintw(y++, 0, "Speed: %d x", pImpl->speed);
        mvprintw(y++, 0, "Zoom: %.1f", pImpl->zoom);
        mvprintw(y++, 0, "Pan: (%d, %d)", pImpl->pan.first, pImpl->pan.second);
    }
    
    // Metrics
    y++;
    mvprintw(y++, 0, "Activity: %.2f", getMetrics().at("Neural Activity"));
    mvprintw(y++, 0, "Memory: %.2f", getMetrics().at("Memory State"));
    mvprintw(y++, 0, "Neuromod: %.2f", getMetrics().at("Neuromodulation"));
    mvprintw(y++, 0, "Connectivity: %.2f", getMetrics().at("Connectivity"));
    
    // Current mode
    y++;
    mvprintw(y++, 0, "Mode: %d", static_cast<int>(pImpl->mode));
    
    // Selected regions
    if (!pImpl->selectedRegions.empty()) {
        y++;
        mvprintw(y++, 0, "Selected Regions:");
        for (size_t i = 0; i < pImpl->selectedRegions.size() && i < 5; ++i) {
            mvprintw(y++, 2, "  [%zu] %u", i, pImpl->selectedRegions[i]);
        }
    }
}

void InteractiveVisualizer::drawNetworkView() {
    // Draw simplified neural network diagram
    int centerX = 40 + pImpl->pan.second;
    int centerY = 12 + pImpl->pan.first;
    
    if (pImpl->useColor) {
        attron(COLOR_PAIR(2)); // Network elements
    }
    
    // Draw central brain region
    for (int dy = -3; dy <= 3; ++dy) {
        for (int dx = -3; dx <= 3; ++dx) {
            if (std::abs(dx) + std::abs(dy) <= 3) {
                int x = centerX + dx * 2 * static_cast<int>(pImpl->zoom);
                int y = centerY + dy * 2 * static_cast<int>(pImpl->zoom);
                
                if (x >= 0 && x < pImpl->screenWidth && y >= 0 && y < pImpl->screenHeight) {
                    // Color based on neuron type and state
                    mvprintw(y, x, "*");
                }
            }
        }
    }
    
    if (pImpl->useColor) {
        attroff(COLOR_PAIR(2));
    }
    
    // Draw connections
    if (pImpl->brain) {
        auto regions = pImpl->brain->getRegions();
        for (size_t i = 0; i < regions.size(); ++i) {
            for (size_t j = i + 1; j < regions.size(); ++j) {
                RegionId from = regions[i]->getId();
                RegionId to = regions[j]->getId();
                
                // Draw connection line (simplified)
                int x1 = 5 + static_cast<int>(i * 5 * pImpl->zoom);
                int y1 = centerY - 5;
                int x2 = 5 + static_cast<int>(j * 5 * pImpl->zoom);
                int y2 = centerY + 5;
                
                // Simple line drawing
                if (std::abs(x1 - x2) > std::abs(y1 - y2)) {
                    for (int x = std::min(x1, x2); x <= std::max(x1, x2); ++x) {
                        if (x >= 0 && x < pImpl->screenWidth) {
                            int y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
                            if (y >= 0 && y < pImpl->screenHeight) {
                                mvprintw(y, x, ".");
                            }
                        }
                    }
                } else {
                    for (int y = std::min(y1, y2); y <= std::max(y1, y2); ++y) {
                        if (y >= 0 && y < pImpl->screenHeight) {
                            int x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                            if (x >= 0 && x < pImpl->screenWidth) {
                                mvprintw(y, x, ".");
                            }
                        }
                    }
                }
            }
        }
    }
}

void InteractiveVisualizer::drawActivityView() {
    // Draw activity heatmap
    int height = pImpl->mainAreaHeight - 2;
    int width = pImpl->mainAreaWidth - 2;
    
    if (pImpl->useColor) {
        attron(COLOR_PAIR(3)); // Activity
    }
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Draw activity based on history
            double activity = 0.0;
            if (!pImpl->neuralActivityHistory.empty() && 
                pImpl->neuralActivityHistory.size() > static_cast<size_t>(y % 10)) {
                activity = pImpl->neuralActivityHistory[pImpl->neuralActivityHistory.size() - 1 - (y % 10)];
            }
            
            int charY = 2 + y;
            int charX = 2 + x;
            
            if (charY >= 0 && charY < pImpl->screenHeight && 
                charX >= 0 && charX < pImpl->screenWidth) {
                
                // Map activity to character and color
                if (activity < 0.2f) {
                    mvprintw(charY, charX, ".");
                } else if (activity < 0.4f) {
                    mvprintw(charY, charX, "*");
                } else if (activity < 0.6f) {
                    mvprintw(charY, charX, "#");
                } else if (activity < 0.8f) {
                    mvprintw(charY, charX, "@");
                } else {
                    mvprintw(charY, charX, "%");
                }
            }
        }
    }
    
    if (pImpl->useColor) {
        attroff(COLOR_PAIR(3));
    }
    
    // Draw activity legend
    mvprintw(pImpl->mainAreaHeight, 0, "Activity Legend: . (0-20%) * (20-40%) # (40-60%) @ (60-80%) %% (80-100%)");
}

void InteractiveVisualizer::drawSpikeView() {
    // Draw spike raster plot
    int spikeCount = pImpl->brain ? pImpl->brain->getTotalSpikeCount() : 0;
    
    if (pImpl->useColor) {
        attron(COLOR_PAIR(4)); // Spikes
    }
    
    mvprintw(2, 2, "Spike Raster Plot (Total: %zu)", spikeCount);
    
    // Draw simple spike histogram
    int histogramHeight = 10;
    int histogramWidth = pImpl->mainAreaWidth - 10;
    
    for (int y = 0; y < histogramHeight; ++y) {
        int threshold = (histogramHeight - 1 - y) * spikeCount / histogramHeight;
        int barHeight = std::min(histogramHeight - y, threshold / 10);
        
        for (int x = 0; x < histogramWidth; ++x) {
            int charY = 5 + y;
            int charX = 4 + x;
            
            if (charY >= 0 && charY < pImpl->screenHeight && 
                charX >= 0 && charX < pImpl->screenWidth) {
                
                if (y < barHeight) {
                    mvprintw(charY, charX, "#");
                } else {
                    mvprintw(charY, charX, "-");
                }
            }
        }
    }
    
    if (pImpl->useColor) {
        attroff(COLOR_PAIR(4));
    }
    
    mvprintw(pImpl->mainAreaHeight, 0, "Spike count over time (simplified histogram)");
}

void InteractiveVisualizer::drawWeightView() {
    // Draw weight matrix visualization
    if (pImpl->brain && pImpl->brain->getRegionCount() > 0) {
        auto* region = pImpl->brain->getRegion(RegionId(1));
        if (region) {
            auto synapses = region->getSynapses();
            if (!synapses.empty()) {
                int weightCount = std::min(size_t(20), synapses.size());
                
                if (pImpl->useColor) {
                    attron(COLOR_PAIR(5)); // Weights
                }
                
                mvprintw(2, 2, "Weight Matrix Sample (showing %zu of %zu synapses)", 
                         weightCount, synapses.size());
                
                // Draw weight distribution
                std::vector<float> weights;
                for (size_t i = 0; i < weightCount; ++i) {
                    weights.push_back(synapses[i]->getWeight());
                }
                
                float minWeight = *std::min_element(weights.begin(), weights.end());
                float maxWeight = *std::max_element(weights.begin(), weights.end());
                
                for (int i = 0; i < 20 && i < weightCount; ++i) {
                    float weight = weights[i];
                    int barWidth = static_cast<int>((weight - minWeight) / (maxWeight - minWeight + 0.0001f) * 40);
                    
                    if (pImpl->useColor) {
                        // Color weight based on value
                        if (weight < 0.0f) {
                            attron(COLOR_PAIR(9)); // Negative
                        } else if (weight < 0.5f) {
                            attron(COLOR_PAIR(6)); // Low positive
                        } else if (weight < 1.0f) {
                            attron(COLOR_PAIR(10)); // Medium positive
                        } else {
                            attron(COLOR_PAIR(11)); // High positive
                        }
                    }
                    
                    mvprintw(5 + (i % 10), 4 + (i / 10) * 42, "[");
                    for (int x = 0; x < barWidth && x < 40; ++x) {
                        mvprintw(5 + (i % 10), 4 + (i / 10) * 42 + 1 + x, "=");
                    }
                    mvprintw(5 + (i % 10), 4 + (i / 10) * 42 + 41, "]");
                    
                    if (pImpl->useColor) {
                        attroff(COLOR_PAIR(5));
                        attroff(COLOR_PAIR(6));
                        attroff(COLOR_PAIR(9));
                        attroff(COLOR_PAIR(10));
                        attroff(COLOR_PAIR(11));
                    }
                    
                    mvprintw(5 + (i % 10), 4 + (i / 10) * 42 + 42, " W%d: %.3f", i, weight);
                }
                
                if (pImpl->useColor) {
                    attroff(COLOR_PAIR(5));
                }
                
                mvprintw(pImpl->mainAreaHeight, 0, "Weight distribution (simplified)");
            }
        }
    }
}

void InteractiveVisualizer::drawMetricsView() {
    // Draw real-time metrics display
    if (pImpl->useColor) {
        attron(COLOR_PAIR(7)); // Metrics
    }
    
    mvprintw(2, 2, "Real-time Metrics Dashboard");
    
    // Draw metrics history plots
    int plotHeight = 8;
    int plotWidth = 40;
    
    // Neural activity plot
    mvprintw(5, 2, "Neural Activity:");
    for (int y = 0; y < plotHeight; ++y) {
        int index = std::max(0, static_cast<int>(pImpl->neuralActivityHistory.size()) - plotHeight + y);
        double activity = 0.0;
        if (index >= 0 && static_cast<size_t>(index) < pImpl->neuralActivityHistory.size()) {
            activity = pImpl->neuralActivityHistory[index];
        }
        
        int barHeight = static_cast<int>(activity * plotHeight);
        for (int x = 0; x < plotWidth; ++x) {
            if (y < barHeight) {
                mvprintw(5 + y, 20 + x, "#");
            } else {
                mvprintw(5 + y, 20 + x, "-");
            }
        }
    }
    
    // Memory state plot
    mvprintw(5, 45, "Memory State:");
    for (int y = 0; y < plotHeight; ++y) {
        int index = std::max(0, static_cast<int>(pImpl->memoryStateHistory.size()) - plotHeight + y);
        double memory = 0.0;
        if (index >= 0 && static_cast<size_t>(index) < pImpl->memoryStateHistory.size()) {
            memory = pImpl->memoryStateHistory[index];
        }
        
        int barHeight = static_cast<int>(memory * plotHeight);
        for (int x = 0; x < plotWidth; ++x) {
            if (y < barHeight) {
                mvprintw(5 + y, 65 + x, "#");
            } else {
                mvprintw(5 + y, 65 + x, "-");
            }
        }
    }
    
    // Neuromodulation plot
    mvprintw(5, 88, "Neuromodulation:");
    for (int y = 0; y < plotHeight; ++y) {
        int index = std::max(0, static_cast<int>(pImpl->neuromodulationHistory.size()) - plotHeight + y);
        double neuromod = 0.0;
        if (index >= 0 && static_cast<size_t>(index) < pImpl->neuromodulationHistory.size()) {
            neuromod = pImpl->neuromodulationHistory[index];
        }
        
        int barHeight = static_cast<int>(neuromod * plotHeight);
        for (int x = 0; x < plotWidth; ++x) {
            if (y < barHeight) {
                mvprintw(5 + y, 105 + x, "#");
            } else {
                mvprintw(5 + y, 105 + x, "-");
            }
        }
    }
    
    if (pImpl->useColor) {
        attroff(COLOR_PAIR(7));
    }
    
    mvprintw(pImpl->mainAreaHeight, 0, "Real-time metrics with historical plots");
}

void InteractiveVisualizer::drawCommandLine() {
    // Draw command input line
    int y = pImpl->screenHeight - 2;
    
    if (pImpl->useColor) {
        attron(COLOR_PAIR(8)); // Command
    }
    
    mvprintw(y, 0, ">>> %s_", pImpl->currentCommand.c_str());
    
    if (pImpl->useColor) {
        attroff(COLOR_PAIR(8));
    }
}

void InteractiveVisualizer::drawHelp() {
    // Draw help text
    int y = 0;
    
    if (pImpl->useColor) {
        attron(COLOR_PAIR(9)); // Help
    }
    
    mvprintw(y++, 0, "=== Interactive Visualization Help ===");
    y++;
    mvprintw(y++, 0, "Navigation:");
    mvprintw(y++, 2, "  Arrow keys: Pan view");
    mvprintw(y++, 2, "  +/- or =: Zoom in/out");
    mvprintw(y++, 2, "  8/2: Speed control");
    y++;
    mvprintw(y++, 0, "View Modes:");
    mvprintw(y++, 2, "  1: Network View (default)");
    mvprintw(y++, 2, "  2: Activity View (heatmap)");
    mvprintw(y++, 2, "  3: Spike View (raster)");
    mvprintw(y++, 2, "  4: Weight View (matrix)");
    mvprintw(y++, 2, "  5: Metrics View (dashboard)");
    y++;
    mvprintw(y++, 0, "Interaction:");
    mvprintw(y++, 2, "  n: Select next region");
    mvprintw(y++, 2, "  i: Inspect selection");
    mvprintw(y++, 2, "  [: Decrease activity threshold");
    mvprintw(y++, 2, "  ]: Increase activity threshold");
    mvprintw(y++, 2, "  s: Save visualization state");
    mvprintw(y++, 2, "  e: Export data");
    mvprintw(y++, 2, "  =: Toggle debug info");
    mvprintw(y++, 2, "  ?: Show help");
    y++;
    mvprintw(y++, 0, "Controls:");
    mvprintw(y++, 2, "  q/Q: Quit");
    mvprintw(y++, 2, "  Space: Pause/Resume");
    
    if (pImpl->useColor) {
        attroff(COLOR_PAIR(9));
    }
}

void InteractiveVisualizer::adjustSpeed(int delta) {
    pImpl->speed = std::max(0, pImpl->speed + delta);
    pImpl->speed = std::min(10, pImpl->speed);
    
    NLM_LOG_INFO("Simulation speed: " + std::to_string(pImpl->speed) + "x");
}

void InteractiveVisualizer::setMode(VisualizationMode mode) {
    pImpl->mode = mode;
    NLM_LOG_INFO("Visualization mode: " + std::to_string(static_cast<int>(mode)));
}

void InteractiveVisualizer::toggleDebugInfo() {
    pImpl->debugInfo = !pImpl->debugInfo;
}

void InteractiveVisualizer::setZoom(double level) {
    pImpl->zoom = level;
}

void InteractiveVisualizer::pan(int dx, int dy) {
    pImpl->pan.first += dy;
    pImpl->pan.second += dx;
}

void InteractiveVisualizer::selectRegion(RegionId regionId) {
    pImpl->selectedRegions.push_back(regionId);
    if (pImpl->selectedRegions.size() > 5) {
        pImpl->selectedRegions.erase(pImpl->selectedRegions.begin());
    }
}

std::string InteractiveVisualizer::getHelpText() const {
    return "Interactive visualization mode - press ? for help";
}

std::map<std::string, std::string> InteractiveVisualizer::getMetrics() const {
    std::map<std::string, std::string> metrics;
    
    if (pImpl->brain) {
        double neuralActivity = static_cast<double>(pImpl->brain->getFiringNeuronCount()) /
                               std::max(1.0, static_cast<double>(pImpl->brain->getTotalNeuronCount()));
        
        metrics["Neural Activity"] = std::to_string(neuralActivity);
        metrics["Memory State"] = "0.5"; // Placeholder
        metrics["Neuromodulation"] = "0.3"; // Placeholder
        metrics["Connectivity"] = std::to_string(
            static_cast<double>(pImpl->brain->getTotalSynapseCount()) /
            std::max(1.0, static_cast<double>(pImpl->brain->getTotalNeuronCount() * 5))
        );
        metrics["Developmental Stage"] = "Mature"; // Placeholder
        metrics["Simulation Time"] = std::to_string(pImpl->simulationTime) + "s";
    }
    
    return metrics;
}

const std::vector<RegionId>& InteractiveVisualizer::getSelectedRegions() const {
    return pImpl->selectedRegions;
}

void InteractiveVisualizer::setActivityFilter(float threshold) {
    pImpl->activityThreshold = threshold;
}

void InteractiveVisualizer::setDevelopmentalStageFilter(DevelopmentalStage stage) {
    pImpl->stageFilter = stage;
}

DevelopmentalStage InteractiveVisualizer::getDevelopmentalStage() const {
    if (pImpl->brain) {
        return pImpl->brain->getDevelopmentalStage();
    }
    return DevelopmentalStage::Initial;
}

void InteractiveVisualizer::setViewFocus(const std::string& focusType) {
    pImpl->viewFocus = focusType;
}

int InteractiveVisualizer::getSpeed() const {
    return pImpl->speed;
}

double InteractiveVisualizer::getZoom() const {
    return pImpl->zoom;
}

std::pair<int, int> InteractiveVisualizer::getPan() const {
    return pImpl->pan;
}

bool InteractiveVisualizer::saveState(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for saving: " + filepath);
        return false;
    }
    
    file << "NLM Interactive Visualization State\n";
    file << "==============================\n\n";
    file << "Simulation Time: " << pImpl->simulationTime << "s\n";
    file << "Speed: " << pImpl->speed << "x\n";
    file << "Zoom: " << pImpl->zoom << "\n";
    file << "Pan: (" << pImpl->pan.first << ", " << pImpl->pan.second << ")\n";
    file << "Mode: " << static_cast<int>(pImpl->mode) << "\n";
    file << "Debug Info: " << (pImpl->debugInfo ? "Enabled" : "Disabled") << "\n";
    
    if (pImpl->brain) {
        file << "\nBrain State:\n";
        file << "  Total Neurons: " << pImpl->brain->getTotalNeuronCount() << "\n";
        file << "  Total Synapses: " << pImpl->brain->getTotalSynapseCount() << "\n";
        file << "  Firing Neurons: " << pImpl->brain->getFiringNeuronCount() << "\n";
        file << "  Total Spikes: " << pImpl->brain->getTotalSpikeCount() << "\n";
        file << "  Average Firing Rate: " << pImpl->brain->getAverageFiringRate() << "\n";
    }
    
    file << "\nMetrics History:\n";
    file << "  Neural Activity samples: " << pImpl->neuralActivityHistory.size() << "\n";
    file << "  Memory State samples: " << pImpl->memoryStateHistory.size() << "\n";
    file << "  Neuromodulation samples: " << pImpl->neuromodulationHistory.size() << "\n";
    file << "  Connectivity samples: " << pImpl->connectivityHistory.size() << "\n";
    
    file.close();
    NLM_LOG_INFO("Visualization state saved to: " + filepath);
    return true;
}

bool InteractiveVisualizer::exportData(const std::string& filepath, const std::string& format) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for export: " + filepath);
        return false;
    }
    
    if (format == "csv") {
        file << "Time,NeuralActivity,MemoryState,Neuromodulation,Connectivity,FiringNeurons,TotalSpikes\n";
        
        size_t minSize = std::min(std::min(pImpl->neuralActivityHistory.size(), pImpl->memoryStateHistory.size()),
                                 std::min(pImpl->neuromodulationHistory.size(), pImpl->connectivityHistory.size()));
        
        for (size_t i = 0; i < minSize; ++i) {
            file << pImpl->timeHistory[i].time_since_epoch().count() << ","; // Use time_t for CSV
            file << pImpl->neuralActivityHistory[i] << ",";
            file << pImpl->memoryStateHistory[i] << ",";
            file << pImpl->neuromodulationHistory[i] << ",";
            file << pImpl->connectivityHistory[i] << ",";
            
            if (pImpl->brain) {
                file << (static_cast<double>(pImpl->brain->getFiringNeuronCount()) / 
                         std::max(1.0, static_cast<double>(pImpl->brain->getTotalNeuronCount())) * 100.0) << ",";
                file << pImpl->brain->getTotalSpikeCount();
            }
            
            file << "\n";
        }
    } else {
        file << "NLM Interactive Visualization Data\n";
        file << "==============================\n\n";
        
        if (pImpl->brain) {
            file << "Current Brain State:\n";
            file << "  Total Neurons: " << pImpl->brain->getTotalNeuronCount() << "\n";
            file << "  Total Synapses: " << pImpl->brain->getTotalSynapseCount() << "\n";
            file << "  Firing Neurons: " << pImpl->brain->getFiringNeuronCount() << "\n";
            file << "  Total Spikes: " << pImpl->brain->getTotalSpikeCount() << "\n";
            file << "  Average Firing Rate: " << pImpl->brain->getAverageFiringRate() << "\n";
        }
        
        file << "\nMetrics at current time:\n";
        auto metrics = getMetrics();
        for (const auto& pair : metrics) {
            file << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    file.close();
    NLM_LOG_INFO("Data exported to: " + filepath + " (format: " + format + ")");
    return true;
}