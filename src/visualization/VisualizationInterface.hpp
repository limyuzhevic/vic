#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;
class Config;

/**
 * Visualization interface for NLM brain monitoring and analysis
 */
class VisualizationInterface {
public:
    VisualizationInterface() = default;
    virtual ~VisualizationInterface() = default;
    
    /**
     * Initialize the visualization interface
     */
    virtual bool initialize() = 0;
    
    /**
     * Update visualization data
     */
    virtual void update(const Brain* brain, double timestep) = 0;
    
    /**
     * Render the visualization
     */
    virtual void render() = 0;
    
    /**
     * Start visualization in background thread
     */
    virtual void start() = 0;
    
    /**
     * Stop visualization
     */
    virtual void stop() = 0;
    
    /**
     * Check if visualization is running
     */
    virtual bool isRunning() const = 0;
    
    /**
     * Set visualization update frequency
     */
    virtual void setUpdateFrequency(double frequency) = 0;
    
    /**
     * Get current update frequency
     */
    virtual double getUpdateFrequency() const = 0;
};

/**
 * Web-based visualization dashboard
 */
class WebDashboard : public VisualizationInterface {
public:
    WebDashboard(int port = 8080, const std::string& host = "localhost");
    virtual ~WebDashboard() override;
    
    bool initialize() override;
    void update(const Brain* brain, double timestep) override;
    void render() override;
    void start() override;
    void stop() override;
    bool isRunning() const override;
    void setUpdateFrequency(double frequency) override;
    double getUpdateFrequency() const override;
    
    /**
     * Set WebSocket handler for real-time updates
     */
    void setWebSocketHandler(std::function<void(const std::string&)> handler);
    
    /**
     * Serve static files
     */
    void serveStaticFiles(const std::string& path);
    
    /**
     * Add metric to dashboard
     */
    void addMetric(const std::string& name, double value);
    
    /**
     * Add brain data to dashboard
     */
    void addBrainData(const std::string& key, const std::vector<float>& data);
    
    /**
     * Get dashboard status
     */
    std::string getStatus() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Real-time monitoring system
 */
class MonitorSystem {
public:
    MonitorSystem();
    ~MonitorSystem();
    
    /**
     * Initialize monitoring system
     */
    bool initialize();
    
    /**
     * Start monitoring
     */
    void start();
    
    /**
     * Stop monitoring
     */
    void stop();
    
    /**
     * Check if monitoring is running
     */
    bool isRunning() const;
    
    /**
     * Add metric to monitoring
     */
    void addMetric(const std::string& name, const std::function<double()>& getter);
    
    /**
     * Update all metrics
     */
    void update();
    
    /**
     * Get metric value
     */
    double getMetric(const std::string& name) const;
    
    /**
     * Get all metrics
     */
    std::unordered_map<std::string, double> getAllMetrics() const;
    
    /**
     * Set sampling interval
     */
    void setSamplingInterval(double interval);
    
    /**
     * Get sampling interval
     */
    double getSamplingInterval() const;
    
    /**
     * Start monitoring for a specific brain
     */
    void monitorBrain(const Brain* brain);
    
    /**
     * Stop monitoring brain
     */
    void stopMonitoringBrain();
    
    /**
     * Export metrics to file
     */
    bool exportMetrics(const std::string& filepath) const;
    
    /**
     * Get performance statistics
     */
    std::string getPerformanceStats() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Simple text-based visualization for terminal environments
 */
class TerminalVisualization : public VisualizationInterface {
public:
    TerminalVisualization();
    ~TerminalVisualization() override;
    
    bool initialize() override;
    void update(const Brain* brain, double timestep) override;
    void render() override;
    void start() override;
    void stop() override;
    bool isRunning() const override;
    void setUpdateFrequency(double frequency) override;
    double getUpdateFrequency() const override;
    
    /**
     * Set output stream for visualization
     */
    void setOutputStream(std::ostream& output);
    
    /**
     * Enable color output
     */
    void enableColors(bool enable);
    
    /**
     * Configure display layout
     */
    void configureLayout(const std::string& layout);
    
    /**
     * Get current visualization layout
     */
    std::string getLayout() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Visualization factory for creating appropriate visualization interfaces
 */
class VisualizationFactory {
public:
    static std::unique_ptr<VisualizationInterface> createVisualization(
        const std::string& type,
        const std::unordered_map<std::string, std::string>& options = {});
    
    static std::unique_ptr<MonitorSystem> createMonitorSystem(
        const std::unordered_map<std::string, std::string>& options = {});
    
    static std::vector<std::string> getAvailableVisualizationTypes();
    static std::vector<std::string> getAvailableMonitorTypes();
};

} // namespace nlm

