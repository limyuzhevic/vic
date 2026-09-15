#pragma once

namespace nlm {

// Export format enumeration
enum ExportFormat {
    Text,
    JSON,
    CSV,
    PNG,
    SVG,
    PDF,
    Binary,
    HDF5
};

// Visualization engine base class
class VisualizationEngine {
public:
    VisualizationEngine() {}
    virtual ~VisualizationEngine() {}
    
    // Initialize the engine
    virtual bool initialize() = 0;
    
    // Update engine with new data
    virtual void update(double timestep) = 0;
    
    // Render current frame
    virtual void render() = 0;
    
    // Close and cleanup
    virtual void close() = 0;
    
    // Check if engine is active
    virtual bool isActive() const = 0;
    
    // Export data
    virtual bool exportData(const std::string& filepath, ExportFormat format) = 0;
    
    // Get engine-specific configuration
    virtual void* getEngineSpecificConfig() = 0;
    
    // Set engine-specific configuration
    virtual void setEngineSpecificConfig(const void* config, size_t size) = 0;
};

// Console backend for headless systems
class ConsoleBackend : public VisualizationEngine {
public:
    ConsoleBackend() {}
    ~ConsoleBackend() override {}
    
    bool initialize() override;
    void update(double timestep) override;
    void render() override;
    void close() override;
    bool isActive() const override;
    bool exportData(const std::string& filepath, ExportFormat format) override;
    void* getEngineSpecificConfig() override;
    void setEngineSpecificConfig(const void* config, size_t size) override;
};

// OpenGL backend for 3D rendering
class OpenGLBackend : public VisualizationEngine {
public:
    OpenGLBackend() {}
    ~OpenGLBackend() override {}
    
    bool initialize() override;
    void update(double timestep) override;
    void render() override;
    void close() override;
    bool isActive() const override;
    bool exportData(const std::string& filepath, ExportFormat format) override;
    void* getEngineSpecificConfig() override;
    void setEngineSpecificConfig(const void* config, size_t size) override;
};

// Web backend for web-based visualization
class WebBackend : public VisualizationEngine {
public:
    WebBackend() {}
    ~WebBackend() override {}
    
    bool initialize() override;
    void update(double timestep) override;
    void render() override;
    void close() override;
    bool isActive() const override;
    bool exportData(const std::string& filepath, ExportFormat format) override;
    void* getEngineSpecificConfig() override;
    void setEngineSpecificConfig(const void* config, size_t size) override;
};

// File backend for data export
class FileBackend : public VisualizationEngine {
public:
    FileBackend() {}
    ~FileBackend() override {}
    
    bool initialize() override;
    void update(double timestep) override;
    void render() override;
    void close() override;
    bool isActive() const override;
    bool exportData(const std::string& filepath, ExportFormat format) override;
    void* getEngineSpecificConfig() override;
    void setEngineSpecificConfig(const void* config, size_t size) override;
};

} // namespace nlm
