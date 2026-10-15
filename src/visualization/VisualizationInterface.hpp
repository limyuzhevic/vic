#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace nlm {

// Visualization formats
enum class VisualizationFormat {
    TEXT,
    ASCII,
    JSON,
    CSV,
    HTML,
    SVG
};

// Visualization export options
struct ExportOptions {
    VisualizationFormat format = VisualizationFormat::TEXT;
    std::string filename;
    bool includeMetadata = true;
    std::string customTemplate;
};

// Base visualization interface
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    // Initialize visualization
    virtual bool initialize() = 0;
    
    // Update visualization data
    virtual void update(double deltaTime) = 0;
    
    // Render current state
    virtual void render() = 0;
    
    // Close visualization
    virtual void close() = 0;
    
    // Check if visualization is active
    virtual bool isActive() const = 0;
    
    // Set update rate
    virtual void setUpdateRate(double hz) = 0;
    virtual double getUpdateRate() const = 0;
    
    // Export visualization data
    virtual bool exportData(const ExportOptions& options) = 0;
    
    // Import visualization data
    virtual bool importData(const std::string& filepath) = 0;
    
    // Get current frame as string (for text-based formats)
    virtual std::string getCurrentFrame() const = 0;
    
    // Get performance statistics
    virtual std::string getPerformanceStats() const = 0;
};

} // namespace nlm
