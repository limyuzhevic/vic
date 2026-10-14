#pragma once

/**
 * @file VisualizationFramework.hpp
 * @brief Enhanced visualization framework for NLM
 * @author NLM Project
 * @date Phase 4
 * 
 * This file defines the enhanced visualization framework for the NLM
 * computational neuroscience platform, providing comprehensive
 * visualization capabilities for neural network debugging and analysis.
 * 
 * Key features:
 * - Multiple visualization backends (console, JSON, CSV, WebGL)
 * - Real-time visualization update
 * - Network structure visualization
 * - Activity and spike visualization
 * - Weight matrix visualization
 * - Progress tracking and progress bars
 * - Configuration-driven visualization
 * - Progress tracking and monitoring
 * - Data export and archiving
 * - Performance optimization
 * - Error handling and recovery
 * 
 * The visualization framework provides:
 * - Real-time neural network visualization
 * - Spike raster plots and activity maps
 * - Network topology and connectivity visualization
 * - Synaptic weight visualization with heat maps
 * - Progress monitoring and reporting
 * - Data export for external analysis
 * - Integration with debugging tools
 * 
 * @note This framework builds upon the placeholder visualization interface
 *       to provide comprehensive visualization capabilities for Phase 2 and beyond.
 */

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "core/EnhancedConfig.hpp"
#include "core/ErrorHandling.hpp"
#include "brain/Brain.hpp"

namespace nlm {

/**
 * @brief Visualization output format
 */
enum class VisualizationFormat {
    Console,      ///< Console-based visualization
    JSON,         ///< JSON data output
    CSV,          ///< CSV data export
    YAML,         ///< YAML data export
    HTML,         ///< HTML web visualization
    WebGL,        ///< WebGL 3D visualization
    PNG,          ///< PNG image output
    SVG,          ///< SVG vector graphics
    Custom       ///< Custom format
};

/**
 * @brief Visualization update mode
 */
enum class VisualizationMode {
    Static,       ///< Static visualization (snapshot)
    RealTime,     ///< Real-time visualization
    Buffered,     ///< Buffered visualization with updates
    Triggered,    ///< Visualization triggered by events
    Scheduled     ///< Visualization on schedule
};

/**
 * @brief Visualization data structure
 */
struct VisualizationData {
    std::chrono::system_clock::time_point timestamp;
    std::string format;
    std::string content;
    std::unordered_map<std::string, std::any> metadata;
    std::vector<std::string> dependencies;
    
    VisualizationData() : timestamp(std::chrono::system_clock::now()) {}
    VisualizationData(const std::string& f, const std::string& c)
        : timestamp(std::chrono::system_clock::now()), format(f), content(c) {}
};

/**
 * @brief Progress tracking for long-running visualizations
 */
struct ProgressTracker {
    std::string operation;
    size_t totalSteps;
    size_t currentStep;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point lastUpdateTime;
    std::string currentMessage;
    double progressPercentage;
    
    ProgressTracker(const std::string& op, size_t total)
        : operation(op), totalSteps(total), currentStep(0),
          startTime(std::chrono::system_clock::now()),
          lastUpdateTime(startTime), currentMessage(""),
          progressPercentage(0.0) {}
    
    void update(size_t step, const std::string& message = "");
    double getEstimatedTimeRemaining() const;
    std::string getProgressString() const;
    std::string getEstimatedTimeString() const;
};

/**
 * @brief Visualization backend interface
 */
class IVisualizationBackend {
public:
    virtual ~IVisualizationBackend() = default;
    
    /**
     * @brief Initialize backend
     * 
     * @param config Configuration for backend
     * @return true if initialization succeeded
     */
    virtual bool initialize(const std::unordered_map<std::string, std::any>& config) = 0;
    
    /**
     * @brief Shutdown backend
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Check if backend is active
     * 
     * @return true if backend is active
     */
    virtual bool isActive() const = 0;
    
    /**
     * @brief Update visualization data
     * 
     * @param data Visualization data
     */
    virtual void update(const VisualizationData& data) = 0;
    
    /**
     * @brief Render visualization
     * 
     * @param output Output stream or file path
     * @return true if rendering succeeded
     */
    virtual bool render(const std::string& output = "") = 0;
    
    /**
     * @brief Get backend name
     * 
     * @return Backend name
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get supported formats
     * 
     * @return Vector of supported formats
     */
    virtual std::vector<VisualizationFormat> getSupportedFormats() const = 0;
};

/**
 * @brief Console visualization backend
 */
class ConsoleVisualizationBackend : public IVisualizationBackend {
public:
    ConsoleVisualizationBackend();
    ~ConsoleVisualizationBackend() override;
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isActive() const override;
    void update(const VisualizationData& data) override;
    bool render(const std::string& output = "") override;
    const char* getName() const override;
    std::vector<VisualizationFormat> getSupportedFormats() const override;
    
    /**
     * @brief Set console visualization options
     * 
     * @param width Console width
     * @param height Console height
     */
    void setConsoleSize(size_t width, size_t height);
    
    /**
     * @brief Set visualization style
     * 
     * @param style Visualization style
     */
    void setStyle(const std::string& style);
    
    /**
     * @brief Enable/disable colors
     * 
     * @param enable Color enable flag
     */
    void setColors(bool enable);
    
    /**
     * @brief Set update interval
     * 
     * @param interval Update interval in seconds
     */
    void setUpdateInterval(double interval);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief JSON visualization backend
 */
class JSONVisualizationBackend : public IVisualizationBackend {
public:
    JSONVisualizationBackend();
    ~JSONVisualizationBackend() override;
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isActive() const override;
    void update(const VisualizationData& data) override;
    bool render(const std::string& output = "") override;
    const char* getName() const override;
    std::vector<VisualizationFormat> getSupportedFormats() const override;
    
    /**
     * @brief Add metadata to JSON output
     * 
     * @param key Metadata key
     * @param value Metadata value
     */
    void addMetadata(const std::string& key, const std::any& value);
    
    /**
     * @brief Set pretty print option
     * 
     * @param enable Pretty print enable flag
     */
    void setPrettyPrint(bool enable);
    
    /**
     * @brief Set indentation level
     * 
     * @param level Indentation level
     */
    void setIndentation(int level);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief CSV visualization backend
 */
class CSVVisualizationBackend : public IVisualizationBackend {
public:
    CSVVisualizationBackend();
    ~CSVVisualizationBackend() override;
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isActive() const override;
    void update(const VisualizationData& data) override;
    bool render(const std::string& output = "") override;
    const char* getName() const override;
    std::vector<VisualizationFormat> getSupportedFormats() const override;
    
    /**
     * @brief Add column to CSV output
     * 
     * @param name Column name
     * @param data Column data
     */
    template<typename T>
    void addColumn(const std::string& name, const std::vector<T>& data);
    
    /**
     * @brief Set separator character
     * 
     * @param sep Separator character
     */
    void setSeparator(char sep);
    
    /**
     * @brief Set quote character
     * 
     * @param quote Quote character
     */
    void setQuote(char quote);
    
    /**
     * @brief Include headers
     * 
     * @param include Header include flag
     */
    void includeHeaders(bool include);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Web visualization backend
 */
class WebVisualizationBackend : public IVisualizationBackend {
public:
    WebVisualizationBackend();
    ~WebVisualizationBackend() override;
    
    bool initialize(const std::unordered_map<std::string, std::any>& config) override;
    void shutdown() override;
    bool isActive() const override;
    void update(const VisualizationData& data) override;
    bool render(const std::string& output = "") override;
    const char* getName() const override;
    std::vector<VisualizationFormat> getSupportedFormats() const override;
    
    /**
     * @brief Set web server port
     * 
     * @param port Web server port
     */
    void setPort(int port);
    
    /**
     * @brief Set web server host
     * 
     * @param host Web server host
     */
    void setHost(const std::string& host);
    
    /**
     * @brief Set auto-open browser
     * 
     * @param enable Auto-open enable flag
     */
    void setAutoOpen(bool enable);
    
    /**
     * @brief Add JavaScript to HTML
     * 
     * @param js JavaScript code
     */
    void addJavaScript(const std::string& js);
    
    /**
     * @brief Add CSS to HTML
     * 
     * @param css CSS code
     */
    void addCSS(const std::string& css);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Main visualization framework
 */
class VisualizationFramework {
public:
    VisualizationFramework();
    ~VisualizationFramework() = default;
    
    /**
     * @brief Initialize visualization framework
     * 
     * @param config Configuration
     * @return true if initialization succeeded
     */
    bool initialize(const EnhancedConfig& config);
    
    /**
     * @brief Shutdown visualization framework
     */
    void shutdown();
    
    /**
     * @brief Add visualization backend
     * 
     * @param backend Backend instance
     */
    void addBackend(std::unique_ptr<IVisualizationBackend> backend);
    
    /**
     * @brief Set active backend
     * 
     * @param name Backend name
     * @return true if backend was activated
     */
    bool setActiveBackend(const std::string& name);
    
    /**
     * @brief Get active backend
     * 
     * @return Active backend name (empty if none)
     */
    std::string getActiveBackend() const;
    
    /**
     * @brief Update visualization with brain data
     * 
     * @param brain Brain reference
     * @param timestamp Timestamp
     */
    void update(const Brain& brain, std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now());
    
    /**
     * @brief Render visualization
     * 
     * @param output Output file or stream
     * @return true if rendering succeeded
     */
    bool render(const std::string& output = "");
    
    /**
     * @brief Check if framework is active
     * 
     * @return true if framework is active
     */
    bool isActive() const;
    
    /**
     * @brief Set visualization mode
     * 
     * @param mode Visualization mode
     */
    void setMode(VisualizationMode mode);
    
    /**
     * @brief Get current visualization mode
     * 
     * @return Current visualization mode
     */
    VisualizationMode getMode() const;
    
    /**
     * @brief Set update interval
     * 
     * @param interval Update interval in seconds
     */
    void setUpdateInterval(double interval);
    
    /**
     * @brief Get update interval
     * 
     * @return Update interval in seconds
     */
    double getUpdateInterval() const;
    
    /**
     * @brief Add visualization event listener
     * 
     * @param event Event name
     * @param callback Callback function
     */
    void addEventListener(const std::string& event,
                         std::function<void(const VisualizationData&)> callback);
    
    /**
     * @brief Remove visualization event listener
     * 
     * @param event Event name
     */
    void removeEventListener(const std::string& event);
    
    /**
     * @brief Trigger visualization event
     * 
     * @param event Event name
     * @param data Event data
     */
    void triggerEvent(const std::string& event, const VisualizationData& data);
    
    /**
     * @brief Generate comprehensive visualization report
     * 
     * @param brain Brain reference
     * @param outputDir Output directory
     * @return true if report was generated
     */
    bool generateReport(const Brain& brain, const std::string& outputDir);
    
    /**
     * @brief Generate spike raster visualization
     * 
     * @param brain Brain reference
     * @param output Output file
     * @return true if visualization was generated
     */
    bool generateSpikeRaster(const Brain& brain, const std::string& output = "");
    
    /**
     * @brief Generate network topology visualization
     * 
     * @param brain Brain reference
     * @param output Output file
     * @return true if visualization was generated
     */
    bool generateNetworkTopology(const Brain& brain, const std::string& output = "");
    
    /**
     * @brief Generate activity visualization
     * 
     * @param brain Brain reference
     * @param output Output file
     * @return true if visualization was generated
     */
    bool generateActivityVisualization(const Brain& brain, const std::string& output = "");
    
    /**
     * @brief Generate weight matrix visualization
     * 
     * @param brain Brain reference
     * @param output Output file
     * @return true if visualization was generated
     */
    bool generateWeightMatrix(const Brain& brain, const std::string& output = "");
    
    /**
     * @brief Get visualization statistics
     * 
     * @return Visualization statistics string
     */
    std::string getStatistics() const;
    
    /**
     * @brief Get visualization data
     * 
     * @return Vector of visualization data
     */
    std::vector<VisualizationData> getData() const;
    
    /**
     * @brief Clear all visualization data
     */
    void clearData();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Visualization utility functions
 */
namespace VisualizationUtils {

/**
 * @brief Format spike data for visualization
 * 
 * @param spikes Vector of spike events
 * @param brain Brain reference
 * @return Formatted spike data
 */
std::string formatSpikeData(const std::vector<SpikeEvent>& spikes,
                           const Brain& brain);

/**
 * @brief Format network topology data
 * 
 * @param brain Brain reference
 * @return Formatted network topology data
 */
std::string formatNetworkTopology(const Brain& brain);

/**
 * @brief Format activity data
 * 
 * @param brain Brain reference
 * @param timeStep Time step
 * @return Formatted activity data
 */
std::string formatActivityData(const Brain& brain, size_t timeStep);

/**
 * @brief Format weight matrix data
 * 
 * @param brain Brain reference
 * @return Formatted weight matrix data
 */
std::string formatWeightMatrix(const Brain& brain);

/**
 * @brief Generate HTML visualization
 * 
 * @param title Plot title
 * @param data Visualization data
 * @return HTML visualization string
 */
std::string generateHTMLVisualization(const std::string& title,
                                      const std::string& data);

/**
 * @brief Generate CSV visualization
 * 
 * @param title Plot title
 * @param data Visualization data
 * @return CSV visualization string
 */
std::string generateCSVVisualization(const std::string& title,
                                      const std::string& data);

/**
 * @brief Generate console visualization
 * 
 * @param title Plot title
 * @param data Visualization data
 * @return Console visualization string
 */
std::string generateConsoleVisualization(const std::string& title,
                                          const std::string& data);

/**
 * @brief Save visualization to file
 * 
 * @param data Visualization data
 * @param filepath File path
 * @return true if saving succeeded
 */
bool saveVisualization(const VisualizationData& data, const std::string& filepath);

/**
 * @brief Load visualization from file
 * 
 * @param filepath File path
 * @return Visualization data
 */
VisualizationData loadVisualization(const std::string& filepath);

/**
 * @brief Generate visualization progress report
 * 
 * @param progress Progress tracker
 * @return Progress report string
 */
std::string generateProgressReport(const ProgressTracker& progress);

/**
 * @brief Create color scheme for visualization
 * 
 * @param scheme Color scheme name
 * @return Color scheme string
 */
std::string createColorScheme(const std::string& scheme);

/**
 * @brief Apply style to visualization
 * 
 * @param style Style name
 * @param data Visualization data
 * @return Styled visualization
 */
std::string applyStyle(const std::string& style, const std::string& data);

/**
 * @brief Validate visualization data
 * 
 * @param data Visualization data
 * @return true if data is valid
 */
bool validateVisualizationData(const VisualizationData& data);

/**
 * @brief Generate visualization metadata
 * 
 * @param data Visualization data
 * @param brain Brain reference
 * @return Metadata string
 */
std::string generateMetadata(const VisualizationData& data, const Brain& brain);

} // namespace VisualizationUtils

} // namespace nlm

// Explicit instantiations
namespace nlm {
    template void CSVVisualizationBackend::addColumn<int>(const std::string&, const std::vector<int>&);
    template void CSVVisualizationBackend::addColumn<double>(const std::string&, const std::vector<double>&);
    template void CSVVisualizationBackend::addColumn<std::string>(const std::string&, const std::vector<std::string>&);
    template std::string VisualizationUtils::convertValue<std::string>(const ConfigValue&);
}