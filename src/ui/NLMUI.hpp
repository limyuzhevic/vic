// UI components for NLM advanced interface
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>

namespace nlm {

// Rich text-based interface
class RichTextInterface {
public:
    virtual ~RichTextInterface() = default;
    
    // Set text color
    virtual void setColor(const std::string& color) = 0;
    
    // Set text style
    virtual void setStyle(const std::string& style) = 0;
    
    // Clear screen
    virtual void clear() = 0;
    
    // Print text with formatting
    virtual void print(const std::string& text) = 0;
    
    // Print formatted text with arguments
    virtual void printf(const std::string& format, 
                       const std::vector<std::string>& args) = 0;
    
    // Get current cursor position
    virtual std::pair<int, int> getCursorPosition() const = 0;
    
    // Set cursor position
    virtual void setCursorPosition(int row, int col) = 0;
};

// Interactive brain state visualization
class BrainVisualizer {
public:
    virtual ~BrainVisualizer() = default;
    
    // Update visualization with current brain state
    virtual void update(const std::shared_ptr<Brain>& brain, double time) = 0;
    
    // Set visualization mode
    virtual void setMode(const std::string& mode) = 0;
    
    // Add annotation to visualization
    virtual void addAnnotation(const std::string& text, 
                               const std::pair<int, int>& position) = 0;
    
    // Export visualization to file
    virtual bool exportToFile(const std::string& filepath, 
                             const std::string& format) = 0;
    
    // Get visualization data
    virtual std::string getDataJSON() const = 0;
};

// Real-time monitoring dashboard
class MonitoringDashboard {
public:
    virtual ~MonitoringDashboard() = default;
    
    // Update dashboard metrics
    virtual void updateMetrics(const std::map<std::string, double>& metrics) = 0;
    
    // Add metric display
    virtual void addMetric(const std::string& name, 
                          const std::string& unit, 
                          const std::string& format) = 0;
    
    // Set alert threshold
    virtual void setAlertThreshold(const std::string& metric, 
                                  double threshold) = 0;
    
    // Get dashboard HTML
    virtual std::string getHTML() const = 0;
    
    // Get dashboard data
    virtual std::string getDataCSV() const = 0;
};

// Neural network topology visualization
class NetworkTopologyView {
public:
    virtual ~NetworkTopologyView() = default;
    
    // Update topology with current network structure
    virtual void update(const std::shared_ptr<Brain>& brain) = 0;
    
    // Set layout algorithm
    virtual void setLayoutAlgorithm(const std::string& algorithm) = 0;
    
    // Highlight specific neurons
    virtual void highlightNeurons(const std::vector<NeuronId>& neuronIds, 
                                  const std::string& color) = 0;
    
    // Highlight connections
    virtual void highlightConnections(const std::vector<std::pair<NeuronId, NeuronId>>& connections, 
                                     const std::string& color) = 0;
    
    // Get network statistics
    virtual std::string getStatistics() const = 0;
};

// Activity heat maps and spike raster plots
class ActivityVisualizer {
public:
    virtual ~ActivityVisualizer() = default;
    
    // Update with spike data
    virtual void updateSpikes(const std::vector<std::pair<NeuronId, Timestamp>>& spikes, 
                             double timeWindow) = 0;
    
    // Set heat map parameters
    virtual void setHeatmapParameters(const std::string& metric, 
                                     const std::string& colorScale, 
                                     double minValue, 
                                     double maxValue) = 0;
    
    // Generate spike raster plot
    virtual std::string generateRasterPlot(const std::string& format, 
                                           int width, int height) = 0;
    
    // Get spike statistics
    virtual std::string getSpikeStatistics() const = 0;
};

// Learning statistics and progress tracking
class LearningTracker {
public:
    virtual ~LearningTracker() = default;
    
    // Record learning event
    virtual void recordLearningEvent(const std::string& eventType, 
                                    const std::map<std::string, double>& metrics) = 0;
    
    // Update progress
    virtual void updateProgress(double progress) = 0;
    
    // Get learning history
    virtual std::string getLearningHistory(int maxHistory) const = 0;
    
    // Generate learning report
    virtual std::string generateReport(const std::string& format) const = 0;
};

// Main UI interface that combines all components
class NLMUI {
private:
    std::shared_ptr<RichTextInterface> textInterface_;
    std::shared_ptr<BrainVisualizer> brainVisualizer_;
    std::shared_ptr<MonitoringDashboard> monitoringDashboard_;
    std::shared_ptr<NetworkTopologyView> topologyView_;
    std::shared_ptr<ActivityVisualizer> activityVisualizer_;
    std::shared_ptr<LearningTracker> learningTracker_;
    
    std::shared_ptr<CommandSystem> commandSystem_;
    std::shared_ptr<Logger> logger_;
    
    std::atomic<bool> running_ = false;
    std::thread uiThread_;
    
    std::mutex updateMutex_;
    std::condition_variable updateCV_;
    std::queue<std::function<void()>> updateQueue_;
    
public:
    NLMUI() {
        initializeComponents();
        logger_ = Logger::getGlobal();
    }
    
    ~NLMUI() {
        stop();
    }
    
    // Initialize UI components
    void initializeComponents() {
        // Initialize text interface
        textInterface_ = createRichTextInterface();
        
        // Initialize brain visualizer
        brainVisualizer_ = createBrainVisualizer();
        
        // Initialize monitoring dashboard
        monitoringDashboard_ = createMonitoringDashboard();
        
        // Initialize topology view
        topologyView_ = createNetworkTopologyView();
        
        // Initialize activity visualizer
        activityVisualizer_ = createActivityVisualizer();
        
        // Initialize learning tracker
        learningTracker_ = createLearningTracker();
        
        // Initialize command system
        commandSystem_ = std::make_shared<CommandSystem>();
        
        // Set up callbacks
        commandSystem_->setOutputCallback([this](const std::string& output) {
            onCommandOutput(output);
        });
        
        commandSystem_->setErrorCallback([this](const std::string& error) {
            onCommandError(error);
        });
    }
    
    // Start UI
    void start() {
        running_ = true;
        
        // Start command system
        commandSystem_->start();
        
        // Start UI thread
        uiThread_ = std::thread(&NLMUI::uiThreadFunc, this);
        
        if (logger_) {
            logger_->info("NLM UI started");
        }
    }
    
    // Stop UI
    void stop() {
        running_ = false;
        
        if (uiThread_.joinable()) {
            updateCV_.notify_all();
            uiThread_.join();
        }
        
        if (commandSystem_) {
            commandSystem_->stop();
        }
        
        if (logger_) {
            logger_->info("NLM UI stopped");
        }
    }
    
    // Update all components with brain state
    void update(std::shared_ptr<Brain> brain, double time) {
        std::lock_guard<std::mutex> lock(updateMutex_);
        
        // Update all components
        brainVisualizer_->update(brain, time);
        topologyView_->update(brain);
        activityVisualizer_->updateSpikes(brain->getSpikeEvents(), time);
        
        // Collect metrics for dashboard
        auto metrics = collectMetrics(brain, time);
        monitoringDashboard_->updateMetrics(metrics);
        
        // Queue UI update
        {
            std::lock_guard<std::mutex> lock(updateMutex_);
            updateQueue_.push([this, metrics]() {
                renderUI(metrics);
            });
        }
        
        updateCV_.notify_one();
    }
    
    // Execute command
    std::string executeCommand(const std::string& commandLine) {
        return std::visit([](const auto& result) -> std::string {
            if constexpr (std::is_same_v<decltype(result), std::string>) {
                return result;
            } else {
                return "command_result_type_not_supported";
            }
        }, commandSystem_->executeCommand(commandLine));
    }
    
    // Set brain reference
    void setBrain(std::shared_ptr<Brain> brain) {
        currentBrain_ = brain;
        if (brainVisualizer_) {
            brainVisualizer_->update(brain, 0.0);
        }
    }
    
    // Get UI HTML for web interface
    std::string getHTML() const {
        std::stringstream html;
        
        html << "<!DOCTYPE html>\n";
        html << "<html>\n";
        html << "<head>\n";
        html << "<title>NLM Brain Emulation Interface</title>\n";
        html << "<style>\n";
        html << "body { font-family: monospace; background-color: #000; color: #0f0; }\n";
        html << ".neuron { background-color: #0f0; color: #000; }\n";
        html << ".synapse { color: #0ff; }\n";
        html << ".active { background-color: #ff0; }\n";
        html << "</style>\n";
        html << "</head>\n";
        html << "<body>\n";
        html << "<h1>NLM Brain Emulation Interface</h1>\n";
        html << "<div id=\"dashboard\">" << monitoringDashboard_->getHTML() << "</div>\n";
        html << "<div id=\"brain\">" << brainVisualizer_->getDataJSON() << "</div>\n";
        html << "<script>\n";
        html << "// Real-time updates\n";
        html << "setInterval(updateData, 1000);\n";
        html << "function updateData() {\n";
        html << "  // Fetch latest data from server\n";
        html << "  fetch('/api/metrics').then(response => response.json()).then(data => {\n";
        html << "    updateDashboard(data);\n";
        html << "  });\n";
        html << "}\n";
        html << "</script>\n";
        html << "</body>\n";
        html << "</html>\n";
        
        return html.str();
    }
    
private:
    std::shared_ptr<Brain> currentBrain_;
    
    // Create rich text interface
    std::shared_ptr<RichTextInterface> createRichTextInterface() {
        // TODO: Implement actual terminal interface
        return nullptr;
    }
    
    // Create brain visualizer
    std::shared_ptr<BrainVisualizer> createBrainVisualizer() {
        // TODO: Implement brain visualizer
        return nullptr;
    }
    
    // Create monitoring dashboard
    std::shared_ptr<MonitoringDashboard> createMonitoringDashboard() {
        // TODO: Implement monitoring dashboard
        return nullptr;
    }
    
    // Create network topology view
    std::shared_ptr<NetworkTopologyView> createNetworkTopologyView() {
        // TODO: Implement topology view
        return nullptr;
    }
    
    // Create activity visualizer
    std::shared_ptr<ActivityVisualizer> createActivityVisualizer() {
        // TODO: Implement activity visualizer
        return nullptr;
    }
    
    // Create learning tracker
    std::shared_ptr<LearningTracker> createLearningTracker() {
        // TODO: Implement learning tracker
        return nullptr;
    }
    
    // Collect metrics from brain
    std::map<std::string, double> collectMetrics(std::shared_ptr<Brain> brain, double time) {
        std::map<std::string, double> metrics;
        
        if (brain) {
            metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
            metrics["firing_rate"] = static_cast<double>(brain->getAverageFiringRate());
            metrics["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
            metrics["active_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
            metrics["excitatory_inhibitory_ratio"] = brain->getExcitationInhibitionRatio();
        }
        
        return metrics;
    }
    
    // Render UI
    void renderUI(const std::map<std::string, double>& metrics) {
        if (!textInterface_) return;
        
        textInterface_->clear();
        
        // Print header
        textInterface_->setColor("green");
        textInterface_->print("=== NLM Brain Emulation Interface ===\n\n");
        
        // Print metrics
        textInterface_->setColor("yellow");
        textInterface_->print("Metrics:\n");
        textInterface_->setColor("white");
        textInterface_->printf("  Total Spikes: %zu\n", {std::to_string(static_cast<size_t>(metrics.at("total_spikes"))))});
        textInterface_->printf("  Firing Rate: %.2f Hz\n", {std::to_string(metrics.at("firing_rate"))});
        textInterface_->printf("  Neurons: %zu (%.1f%% active)\n", {
            std::to_string(static_cast<size_t>(metrics.at("neuron_count"))),
            std::to_string(100.0 * metrics.at("active_neurons") / metrics.at("neuron_count"))
        });
        
        // Print status
        textInterface_->setColor("cyan");
        textInterface_->print("\nStatus: Running\n");
        
        // Print prompt
        textInterface_->setColor("green");
        textInterface_->print("> ");
    }
    
    // Handle command output
    void onCommandOutput(const std::string& output) {
        if (textInterface_) {
            textInterface_->print(output);
        }
    }
    
    // Handle command error
    void onCommandError(const std::string& error) {
        if (textInterface_) {
            textInterface_->setColor("red");
            textInterface_->print("Error: " + error + "\n");
            textInterface_->setColor("white");
        }
    }
    
    // UI thread function
    void uiThreadFunc() {
        while (running_) {
            std::unique_lock<std::mutex> lock(updateMutex_);
            
            if (updateQueue_.empty()) {
                updateCV_.wait_for(lock, std::chrono::milliseconds(100));
                continue;
            }
            
            auto updateFunc = updateQueue_.front();
            updateQueue_.pop();
            
            lock.unlock();
            
            updateFunc();
        }
    }
};

} // namespace nlm
