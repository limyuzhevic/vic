// Expert UI implementation
#include "interfaces/AdvancedInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Experiment.hpp"
#include "performance/PerformanceMonitor.hpp"
#include "debugging/DebugTools.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <conio.h> // For Windows console input

namespace nlm {

class ExpertUI::Impl {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::shared_ptr<Experiment> experiment;
    std::shared_ptr<PerformanceMonitor> performanceMonitor;
    std::shared_ptr<DebugTools> debugTools;
    std::shared_ptr<VisualizationInterface> visualizationInterface;
    
    bool isActive;
    bool running;
    UIConfig uiConfig;
    
    std::thread uiThread;
    std::chrono::system_clock::time_point lastUpdate;
    double deltaTime;
    
    Impl() : isActive(false), running(false), deltaTime(0.0) {}
    
    void renderUI() const {
        system("cls"); // Clear screen (Windows)
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "NLM EXPERT USER INTERFACE - Phase 6" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << std::endl;
        
        // Status bar
        std::cout << "[STATUS] Running: " << (isActive ? "Yes" : "No") 
                  << " | Last Update: " << getTimeString(lastUpdate) << std::endl;
        std::cout << "[PERF] Firing Rate: " << (brain ? std::to_string(brain->getFiringNeuronCount()) : "N/A") 
                  << " | Synapses: " << (brain ? std::to_string(brain->getTotalSynapseCount()) : "N/A") << std::endl;
        std::cout << std::endl;
        
        // Main panels
        if (uiConfig.enableNeurons) {
            std::cout << "--- NEURAL SYSTEM ---" << std::endl;
            showNeuronsPanel();
            std::cout << std::endl;
        }
        
        if (uiConfig.enableSynapses) {
            std::cout << "--- SYNAPTIC SYSTEM ---" << std::endl;
            showSynapsesPanel();
            std::cout << std::endl;
        }
        
        if (uiConfig.enableMemory) {
            std::cout << "--- MEMORY SYSTEM ---" << std::endl;
            showMemoryPanel();
            std::cout << std::endl;
        }
        
        if (uiConfig.enablePrediction) {
            std::cout << "--- PREDICTION SYSTEM ---" << std::endl;
            showPredictionPanel();
            std::cout << std::endl;
        }
        
        if (uiConfig.enableNeuromodulation) {
            std::cout << "--- NEUROMODULATION SYSTEM ---" << std::endl;
            showNeuromodulationPanel();
            std::cout << std::endl;
        }
        
        if (uiConfig.enablePlasticity) {
            std::cout << "--- PLASTICITY SYSTEM ---" << std::endl;
            showPlasticityPanel();
            std::cout << std::endl;
        }
        
        if (uiConfig.enableVisualization) {
            std::cout << "--- VISUALIZATION ---" << std::endl;
            showVisualizationStatus();
            std::cout << std::endl;
        }
        
        if (uiConfig.enableDebug) {
            std::cout << "--- DEBUG SYSTEM ---" << std::endl;
            showDebugPanel();
            std::cout << std::endl;
        }
        
        // Configuration panel
        if (uiConfig.showAdvanced) {
            std::cout << "--- CONFIGURATION ---" << std::endl;
            showConfigurationPanel();
            std::cout << std::endl;
        }
        
        // Experiments panel
        if (experiment) {
            std::cout << "--- EXPERIMENTS ---" << std::endl;
            showExperimentsPanel();
            std::cout << std::endl;
        }
        
        // Performance panel
        if (performanceMonitor) {
            std::cout << "--- PERFORMANCE ---" << std::endl;
            showPerformancePanel();
            std::cout << std::endl;
        }
        
        // Help section
        std::cout << "--- HELP ---" << std::endl;
        std::cout << "[q] Quit | [h] Help | [c] Configuration | [e] Experiments" << std::endl;
        std::cout << "[v] Visualization | [d] Debug | [p] Performance | [f] Focus" << std::endl;
        std::cout << "[r] Refresh Rate: " << uiConfig.refreshRate << " Hz" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }
    
    std::string getTimeString(std::chrono::system_clock::time_point time) const {
        auto time_t = std::chrono::system_clock::to_time_t(time);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            time.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    void showNeuronsPanel() const {
        if (!brain) {
            std::cout << "  Brain not available" << std::endl;
            return;
        }
        
        auto neurons = brain->getAllNeurons();
        std::cout << "  Total Neurons: " << neurons.size() << std::endl;
        std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        if (!neurons.empty()) {
            std::cout << "  Sample Neurons:" << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                auto* neuron = neurons[i];
                std::cout << "    Neuron " << i << 
                          " | Type: " << static_cast<int>(neuron->getType()) <<
                          " | V: " << std::fixed << std::setprecision(3) << neuron->getMembranePotential() <<
                          " | Threshold: " << neuron->getThreshold() <<
                          " | Active: " << (neuron->isActive() ? "Yes" : "No") << std::endl;
            }
        }
    }
    
    void showSynapsesPanel() const {
        if (!brain) {
            std::cout << "  Brain not available" << std::endl;
            return;
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "  No regions found" << std::endl;
            return;
        }
        
        auto synapses = region->getSynapses();
        std::cout << "  Total Synapses: " << synapses.size() << std::endl;
        
        if (!synapses.empty()) {
            std::cout << "  Sample Synapses:" << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), synapses.size()); ++i) {
                auto* synapse = synapses[i];
                std::cout << "    Synapse " << i << 
                          " | Weight: " << std::fixed << std::setprecision(6) << synapse->getWeight() <<
                          " | Active: " << (synapse->isActive() ? "Yes" : "No") <<
                          " | Plasticity: " << (synapse->hasPlasticity() ? "Enabled" : "Disabled") << std::endl;
            }
        }
    }
    
    void showMemoryPanel() const {
        if (!brain) {
            std::cout << "  Brain not available" << std::endl;
            return;
        }
        
        // Memory information would come from the brain's memory systems
        std::cout << "  Memory systems available:" << std::endl;
        std::cout << "    - Working Memory" << std::endl;
        std::cout << "    - Episodic Memory" << std::endl;
        std::cout << "    - Semantic Memory" << std::endl;
        std::cout << "    - Procedural Memory" << std::endl;
        std::cout << "    - Associative Memory" << std::endl;
    }
    
    void showPredictionPanel() const {
        if (!brain) {
            std::cout << "  Brain not available" << std::endl;
            return;
        }
        
        // Prediction information would come from the prediction system
        std::cout << "  Prediction system status:" << std::endl;
        std::cout << "    - Prediction system: Active" << std::endl;
        std::cout << "    - Prediction accuracy: N/A" << std::endl;
        std::cout << "    - Confidence threshold: N/A" << std::endl;
    }
    
    void showNeuromodulationPanel() const {
        if (!brain) {
            std::cout << "  Brain not available" << std::endl;
            return;
        }
        
        // Neuromodulation information would come from neuromodulation system
        std::cout << "  Neuromodulation status:" << std::endl;
        std::cout << "    - Dopamine system: Active" << std::endl;
        std::cout << "    - Curiosity system: Active" << std::endl;
        std::cout << "    - Novelty detection: Active" << std::endl;
        std::cout << "    - Reward prediction error: Active" << std::endl;
    }
    
    void showPlasticityPanel() const {
        if (!brain) {
            std::cout << "  Brain not available" << std::endl;
            return;
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "  No regions found" << std::endl;
            return;
        }
        
        auto synapses = region->getSynapses();
        size_t plasticSynapses = 0;
        for (auto* synapse : synapses) {
            if (synapse->hasPlasticity()) {
                plasticSynapses++;
            }
        }
        
        std::cout << "  Plasticity status:" << std::endl;
        std::cout << "    - Total synapses: " << synapses.size() << std::endl;
        std::cout << "    - Plastic synapses: " << plasticSynapses << std::endl;
        std::cout << "    - Plasticity rate: " << (plasticSynapses / (double)synapses.size() * 100) << "%" << std::endl;
    }
    
    void showDebugPanel() const {
        if (!debugTools) {
            std::cout << "  Debug tools not available" << std::endl;
            return;
        }
        
        std::cout << "  Debug tools enabled:" << std::endl;
        std::cout << "    - Brain inspection available" << std::endl;
        std::cout << "    - Configuration inspection available" << std::endl;
        std::cout << "    - Experiment inspection available" << std::endl;
        std::cout << "    - Visualization inspection available" << std::endl;
        std::cout << "    - Performance monitoring available" << std::endl;
    }
    
    void showVisualizationStatus() const {
        if (!visualizationInterface) {
            std::cout << "  Visualization interface not available" << std::endl;
            return;
        }
        
        std::cout << "  Visualization status:" << std::endl;
        std::cout << "    - Active: " << (visualizationInterface->isActive() ? "Yes" : "No") << std::endl;
        std::cout << "    - Update rate: " << visualizationInterface->getUpdateRate() << " Hz" << std::endl;
        std::cout << "    - Network visualization: Available" << std::endl;
        std::cout << "    - Activity visualization: Available" << std::endl;
        std::cout << "    - Spike visualization: Available" << std::endl;
        std::cout << "    - Weight visualization: Available" << std::endl;
    }
    
    void showConfigurationPanel() const {
        if (!config) {
            std::cout << "  Configuration not available" << std::endl;
            return;
        }
        
        std::cout << "  Configuration status:" << std::endl;
        std::cout << "    - Valid: " << (config->validate() ? "Yes" : "No") << std::endl;
        std::cout << "    - Source count: " << config->getKeys().size() << std::endl;
        
        if (!config->getValidationErrors().empty()) {
            std::cout << "    - Errors: " << config->getValidationErrors().size() << std::endl;
            for (const auto& error : config->getValidationErrors()) {
                std::cout << "      * " << error << std::endl;
            }
        }
    }
    
    void showExperimentsPanel() const {
        if (!experiment) {
            std::cout << "  No experiment running" << std::endl;
            return;
        }
        
        std::cout << "  Experiment status:" << std::endl;
        std::cout << "    - Status: Running" << std::endl;
        std::cout << "    - Steps completed: N/A" << std::endl;
        std::cout << "    - Total reward: N/A" << std::endl;
        std::cout << "    - Current action: N/A" << std::endl;
    }
    
    void showPerformancePanel() const {
        if (!performanceMonitor) {
            std::cout << "  Performance monitor not available" << std::endl;
            return;
        }
        
        auto metrics = performanceMonitor->getMetrics();
        std::cout << "  Performance metrics:" << std::endl;
        std::cout << "    - CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
        std::cout << "    - Memory Usage: " << metrics.memoryUsage << "%" << std::endl;
        std::cout << "    - Network Traffic: " << metrics.networkTraffic << " KB/s" << std::endl;
        std::cout << "    - Simulation Steps: " << metrics.simulationSteps << std::endl;
        std::cout << "    - Avg Step Time: " << metrics.avgStepTime * 1000 << " ms" << std::endl;
    }
    
    void handleKeyboardInput() {
        if (_kbhit()) {
            int key = _getch();
            
            switch (key) {
                case 'q': case 'Q':
                    running = false;
                    break;
                case 'h': case 'H':
                    showHelp();
                    break;
                case 'c': case 'C':
                    showConfigurationPanel();
                    break;
                case 'e': case 'E':
                    // Toggle experiments panel
                    break;
                case 'v': case 'V':
                    // Toggle visualization
                    break;
                case 'd': case 'D':
                    // Toggle debug
                    break;
                case 'p': case 'P':
                    // Toggle performance
                    break;
                case 'f': case 'F':
                    // Focus mode
                    break;
                case '+': case '=':
                    uiConfig.refreshRate = std::min(uiConfig.refreshRate + 5.0, 60.0);
                    break;
                case '-':
                    uiConfig.refreshRate = std::max(uiConfig.refreshRate - 5.0, 1.0);
                    break;
                case '1':
                    uiConfig.enableNeurons = !uiConfig.enableNeurons;
                    break;
                case '2':
                    uiConfig.enableSynapses = !uiConfig.enableSynapses;
                    break;
                case '3':
                    uiConfig.enableMemory = !uiConfig.enableMemory;
                    break;
                case '4':
                    uiConfig.enablePrediction = !uiConfig.enablePrediction;
                    break;
                case '5':
                    uiConfig.enableNeuromodulation = !uiConfig.enableNeuromodulation;
                    break;
                case '6':
                    uiConfig.enablePlasticity = !uiConfig.enablePlasticity;
                    break;
                case '7':
                    uiConfig.enableVisualization = !uiConfig.enableVisualization;
                    break;
                case '8':
                    uiConfig.enableDebug = !uiConfig.enableDebug;
                    break;
                case '9':
                    uiConfig.showAdvanced = !uiConfig.showAdvanced;
                    break;
            }
        }
    }
};

ExpertUI::ExpertUI() : pImpl(std::make_unique<Impl>()) {}
ExpertUI::~ExpertUI() {
    stop();
}

bool ExpertUI::initialize(const UIConfig& config) {
    uiConfig = config;
    
    // Initialize components if available
    if (brain && visualizationInterface) {
        visualizationInterface->initialize();
    }
    
    isActive = true;
    return true;
}

void ExpertUI::run() {
    running = true;
    
    // Create UI thread
    uiThread = std::thread([this]() {
        while (running && isActive) {
            auto now = std::chrono::system_clock::now();
            deltaTime = std::chrono::duration<double>(now - pImpl->lastUpdate).count();
            pImpl->lastUpdate = now;
            
            // Handle input
            pImpl->handleKeyboardInput();
            
            // Update
            update(deltaTime);
            
            // Render
            render();
            
            // Sleep to control frame rate
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0 / uiConfig.refreshRate)));
        }
    });
}

void ExpertUI::stop() {
    running = false;
    isActive = false;
    
    if (uiThread.joinable()) {
        uiThread.join();
    }
}

bool ExpertUI::isActive() const {
    return isActive;
}

void ExpertUI::update(double deltaTime) {
    // Update UI state based on brain and other components
    if (brain) {
        // Update brain-related UI state
    }
}

void ExpertUI::render() {
    if (!isActive) return;
    
    renderUI();
}

bool ExpertUI::handleInput(const std::string& input) {
    // Handle command line input
    return true;
}

void ExpertUI::showHelp() const {
    std::cout << "\nExpert UI Help:" << std::endl;
    std::cout << "  Basic Commands:" << std::endl;
    std::cout << "    q - Quit" << std::endl;
    std::cout << "    h - Help" << std::endl;
    std::cout << "    c - Configuration" << std::endl;
    std::cout << "    e - Experiments" << std::endl;
    std::cout << "    v - Visualization" << std::endl;
    std::cout << "    d - Debug" << std::endl;
    std::cout << "    p - Performance" << std::endl;
    std::cout << "    f - Focus mode" << std::endl;
    std::cout << std::endl;
    std::cout << "  Panel Controls:" << std::endl;
    std::cout << "    1-9 - Toggle UI panels" << std::endl;
    std::cout << "    +/= - Increase refresh rate" << std::endl;
    std::cout << "    -   - Decrease refresh rate" << std::endl;
    std::cout << std::endl;
    std::cout << "  Configuration Panel:" << std::endl;
    std::cout << "    Shows advanced configuration options" << std::endl;
    std::cout << std::endl;
    std::cout << "  Experiments Panel:" << std::endl;
    std::cout << "    Shows experiment status and controls" << std::endl;
    std::cout << std::endl;
    std::cout << "  Performance Panel:" << std::endl;
    std::cout << "    Shows system performance metrics" << std::endl;
}

void ExpertUI::showDashboard() const {
    std::cout << "\nDashboard:" << std::endl;
    std::cout << "  System Status: " << (isActive ? "Active" : "Inactive") << std::endl;
    std::cout << "  Refresh Rate: " << uiConfig.refreshRate << " Hz" << std::endl;
    std::cout << "  Panels: " << (uiConfig.enableNeurons ? "1" : "0") 
              << (uiConfig.enableSynapses ? "1" : "0") 
              << (uiConfig.enableMemory ? "1" : "0")
              << (uiConfig.enablePrediction ? "1" : "0")
              << (uiConfig.enableNeuromodulation ? "1" : "0")
              << (uiConfig.enablePlasticity ? "1" : "0")
              << (uiConfig.enableVisualization ? "1" : "0")
              << (uiConfig.enableDebug ? "1" : "0") << std::endl;
}

void ExpertUI::setBrain(std::shared_ptr<class Brain> brain) {
    pImpl->brain = brain;
}

void ExpertUI::setConfig(std::shared_ptr<class Config> config) {
    pImpl->config = config;
}

void ExpertUI::setExperiment(std::shared_ptr<class Experiment> experiment) {
    pImpl->experiment = experiment;
}

void ExpertUI::setPerformanceMonitor(std::shared_ptr<class PerformanceMonitor> monitor) {
    pImpl->performanceMonitor = monitor;
}

void ExpertUI::setDebugTools(std::shared_ptr<class DebugTools> tools) {
    pImpl->debugTools = tools;
}

} // namespace nlm