#ifndef NLM_VISUALIZATION_HPP
#define NLM_VISUALIZATION_HPP

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/AgentBody.hpp"
#include "../world/SimpleWorld.hpp"

namespace nlm {

// Forward declarations for visualization components
class VisualizationInterface;
class ConsoleVisualizer;
class HeadlessVisualizer;
class WebVisualizer;
class DataExporter;

// Visualization modes
enum class VisualizationMode {
    Terminal,      // Text-based console visualization
    Headless,      // No visualization, just data export
    Web,           // Web-based visualization (HTML/JS)
    Binary,        // Binary format for programmatic use
    CSV,           // CSV format for data analysis
    JSON,          // JSON format for interoperability
    Performance    // Performance-focused visualization
};

// Visualization data structures
struct VisualizationData {
    double timestamp;
    size_t step;
    
    // Neural activity
    std::vector<float> neuronActivity;
    std::vector<float> neuronMembranePotentials;
    std::vector<float> neuronFiringRates;
    
    // Synapses
    std::vector<float> synapticWeights;
    
    // Memory systems
    float workingMemoryLoad;
    size_t episodicMemoryCount;
    std::vector<float> conceptActivations;
    
    // Neuromodulation
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    
    // Attention and planning
    std::vector<float> attentionWeights;
    std::vector<float> planningScores;
    
    // Performance metrics
    size_t totalSpikeCount;
    size_t firingNeuronCount;
    float excitationInhibitionRatio;
    double simulationTime;
    
    // Agent state
    float agentEnergy;
    float agentHealth;
    std::vector<float> agentPosition;
    float agentOrientation;
    
    // World state
    std::vector<float> worldObjects;
    std::vector<float> environmentalFactors;
};

// Base visualization interface
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    // Initialize visualization with brain and agent
    virtual bool initialize(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<AgentBrain> agent = nullptr,
                           std::shared_ptr<SimpleWorld> world = nullptr) = 0;
    
    // Update visualization with new data
    virtual void update(const VisualizationData& data) = 0;
    
    // Render the current visualization
    virtual void render() = 0;
    
    // Export data to file
    virtual bool exportData(const std::string& filepath, 
                          const std::string& format = "auto") = 0;
    
    // Set visualization options
    virtual void setOption(const std::string& key, const std::string& value) = 0;
    
    // Get current visualization options
    virtual std::string getOption(const std::string& key) const = 0;
    
    // Check if visualization is ready
    virtual bool isReady() const = 0;
    
    // Clean up resources
    virtual void shutdown() = 0;
};

// Console terminal visualization
class ConsoleVisualizer : public VisualizationInterface {
public:
    ConsoleVisualizer();
    ~ConsoleVisualizer() override = default;
    
    bool initialize(std::shared_ptr<Brain> brain,
                   std::shared_ptr<AgentBrain> agent = nullptr,
                   std::shared_ptr<SimpleWorld> world = nullptr) override;
    
    void update(const VisualizationData& data) override;
    
    void render() override;
    
    bool exportData(const std::string& filepath,
                   const std::string& format = "auto") override;
    
    void setOption(const std::string& key, const std::string& value) override;
    
    std::string getOption(const std::string& key) const override;
    
    bool isReady() const override;
    
    void shutdown() override;
    
    // Console-specific methods
    void setRefreshRate(int fps);
    void setMaxHistory(size_t maxPoints);
    void toggleNeuronDetails(bool enable);
    void toggleSynapseDetails(bool enable);
    void toggleMemoryDetails(bool enable);
    void toggleNeuromodulationDetails(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Headless visualization (no display, just data export)
class HeadlessVisualizer : public VisualizationInterface {
public:
    HeadlessVisualizer();
    ~HeadlessVisualizer() override = default;
    
    bool initialize(std::shared_ptr<Brain> brain,
                   std::shared_ptr<AgentBrain> agent = nullptr,
                   std::shared_ptr<SimpleWorld> world = nullptr) override;
    
    void update(const VisualizationData& data) override;
    
    void render() override;
    
    bool exportData(const std::string& filepath,
                   const std::string& format = "auto") override;
    
    void setOption(const std::string& key, const std::string& value) override;
    
    std::string getOption(const std::string& key) const override;
    
    bool isReady() const override;
    
    void shutdown() override;
    
    // Headless-specific methods
    void setOutputDirectory(const std::string& directory);
    void enableDataCompression(bool enable);
    void setExportFrequency(size_t steps);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Web-based visualization
class WebVisualizer : public VisualizationInterface {
public:
    WebVisualizer();
    ~WebVisualizer() override = default;
    
    bool initialize(std::shared_ptr<Brain> brain,
                   std::shared_ptr<AgentBrain> agent = nullptr,
                   std::shared_ptr<SimpleWorld> world = nullptr) override;
    
    void update(const VisualizationData& data) override;
    
    void render() override;
    
    bool exportData(const std::string& filepath,
                   const std::string& format = "auto") override;
    
    void setOption(const std::string& key, const std::string& value) override;
    
    std::string getOption(const std::string& key) const override;
    
    bool isReady() const override;
    
    void shutdown() override;
    
    // Web-specific methods
    void startServer(int port = 8080);
    void stopServer();
    bool isServerRunning() const;
    void setWebRoot(const std::string& root);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main visualization manager
class VisualizationManager {
public:
    VisualizationManager();
    ~VisualizationManager();
    
    // Create a new visualization interface
    std::shared_ptr<VisualizationInterface> createVisualizer(VisualizationMode mode);
    
    // Initialize all visualizations
    bool initializeAll(std::shared_ptr<Brain> brain,
                      std::shared_ptr<AgentBrain> agent = nullptr,
                      std::shared_ptr<SimpleWorld> world = nullptr);
    
    // Update all visualizations with new data
    void updateAll(const VisualizationData& data);
    
    // Render all visualizations
    void renderAll();
    
    // Export data from all visualizations
    bool exportAll(const std::string& directory, const std::string& format = "auto");
    
    // Set global visualization options
    void setGlobalOption(const std::string& key, const std::string& value);
    
    // Get visualization status
    bool isInitialized() const;
    size_t getActiveVisualizerCount() const;
    
    // Clean up
    void shutdownAll();
    
    // Performance monitoring
    double getAverageRenderTime() const;
    double getAverageUpdateTime() const;
    size_t getTotalFramesRendered() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Data exporter for different formats
class DataExporter {
public:
    DataExporter();
    ~DataExporter();
    
    // Export visualization data to different formats
    bool exportToCSV(const VisualizationData& data, const std::string& filepath);
    bool exportToJSON(const VisualizationData& data, const std::string& filepath);
    bool exportToBinary(const VisualizationData& data, const std::string& filepath);
    bool exportToXML(const VisualizationData& data, const std::string& filepath);
    bool exportToHDF5(const VisualizationData& data, const std::string& filepath);
    
    // Batch export
    bool exportBatch(const std::vector<VisualizationData>& data, 
                    const std::string& directory,
                    const std::string& format = "auto");
    
    // Data transformation
    static std::vector<VisualizationData> normalizeData(const std::vector<VisualizationData>& data);
    static std::vector<VisualizationData> filterData(const std::vector<VisualizationData>& data,
                                                    const std::string& filterExpression);
    static std::vector<VisualizationData> resampleData(const std::vector<VisualizationData>& data,
                                                       double targetInterval);
    
    // Metadata handling
    bool saveMetadata(const std::string& filepath, const std::map<std::string, std::string>& metadata);
    std::map<std::string, std::string> loadMetadata(const std::string& filepath);
    
    // Statistics
    static double calculateCorrelation(const std::vector<VisualizationData>& data,
                                      const std::string& field1, const std::string& field2);
    static double calculateMean(const std::vector<VisualizationData>& data,
                               const std::string& field);
    static double calculateStdDev(const std::vector<VisualizationData>& data,
                                 const std::string& field);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_VISUALIZATION_HPP
