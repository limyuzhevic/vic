#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;
class SpikeEvent;

// Visualization output formats
enum class VisualizationFormat {
    ASCII,        // Text-based console output
    JSON,         // JSON file output
    CSV,          // CSV file output
    HTML,         // HTML web page output
    PPM,          // Portable Pixmap image (simple 2D)
    SVG           // Scalable Vector Graphics
};

// Visualization interface for debugging and analysis
// Provides multiple output formats for brain activity visualization
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Disable copying, enable moving
    VisualizationInterface(const VisualizationInterface&) = delete;
    VisualizationInterface& operator=(const VisualizationInterface&) = delete;
    VisualizationInterface(VisualizationInterface&&) noexcept;
    VisualizationInterface& operator=(VisualizationInterface&&) noexcept;
    
    // Initialize visualization
    bool initialize();
    
    // Set output configuration
    void setOutputFormat(VisualizationFormat format);
    void setOutputDirectory(const std::string& dir);
    void setFrameRate(double fps);
    
    // Get configuration
    VisualizationFormat getOutputFormat() const;
    std::string getOutputDirectory() const;
    double getFrameRate() const;
    
    // Update visualization data
    void update();
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Network visualization
    void visualizeNetwork(const Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);
    
    // Generate comprehensive visualization bundle
    void generateReport(const Brain& brain, const std::string& reportName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Factory function for creating visualization backends
class VisualizationBackend {
public:
    virtual ~VisualizationBackend() = default;
    virtual bool initialize() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void close() = 0;
    virtual bool isActive() const = 0;
};

// ASCII visualization backend (console output)
class ASCIIVisualizationBackend : public VisualizationBackend {
public:
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
};

// JSON visualization backend (file output)
class JSONVisualizationBackend : public VisualizationBackend {
public:
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
};

// CSV visualization backend (time series data)
class CSVVisualizationBackend : public VisualizationBackend {
public:
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
};

// PPM visualization backend (simple 2D images)
class PPMVisualizationBackend : public VisualizationBackend {
public:
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
};

} // namespace nlm
