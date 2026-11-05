#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <map>
#include <atomic>
#include <functional>
#include <optional>
#include <variant>
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"

namespace nlm {

// Visualization Interface
class VisualizationInterface {
public:
    enum class VisualizationType {
        NEURON_ACTIVITY,
        SYNAPTIC_WEIGHTS,
        BRAIN_REGIONS,
        MOTOR_OUTPUTS,
        SENSOR_INPUTS,
        MEMORY_STATE,
        NEUROMODULATION,
        DEVELOPMENT_STAGES,
        CONCEPT_FORMATION,
        FULL_DASHBOARD
    };

    struct VisualizationConfig {
        VisualizationType type;
        std::string title;
        double updateInterval; // Hz
        std::map<std::string, std::string> parameters;
        bool enabled;
        bool autoRefresh;
        int width;
        int height;
        std::string backend; // "console", "web", "qt", "imgui"
        std::string filepath; // For saving visualizations
    };

    struct NeuronData {
        size_t id;
        std::string region;
        NeuronType neuronType;
        float firingRate;
        float membranePotential;
        float restingPotential;
        float inputCurrent;
        float outputCurrent;
        double lastSpikeTime;
        bool isActive;
        std::map<std::string, float> metrics;
    };

    struct SynapseData {
        size_t id;
        size_t preNeuronId;
        size_t postNeuronId;
        float weight;
        float weightChange;
        double lastUpdateTime;
        float eligibilityTrace;
        std::string connectionType;
        float delay;
    };

    struct RegionData {
        std::string name;
        size_t neuronCount;
        float averageFiringRate;
        float totalActivity;
        std::vector<size_t> neuronIds;
        std::map<std::string, float> metrics;
    };

    struct MotorOutputData {
        MotorCommand command;
        float activity;
        float firingRate;
        float confidence;
        float noveltyLevel;
        float curiosityLevel;
        double lastExecutedTime;
    };

    struct SensorData {
        std::string sensorName;
        float intensity;
        float normalizedValue;
        double timestamp;
        std::vector<float> rawValues;
    };

    struct MemoryStateData {
        std::string memoryType; // "working", "episodic", "semantic"
        size_t traceCount;
        size_t episodeCount;
        float averageStrength;
        std::vector<float> activationPattern;
        double lastUpdateTime;
    };

    struct NeuromodulationData {
        std::string neuromodulator; // "dopamine", "curiosity", "novelty"
        float level;
        float baseline;
        float changeRate;
        double lastUpdateTime;
        std::map<std::string, float> effects;
    };

    struct DevelopmentData {
        DevelopmentalStage stage;
        double age;
        float plasticityModifier;
        float synaptogenesisRate;
        float pruningRate;
        double timeToNextStage;
    };

    struct ConceptData {
        size_t conceptId;
        std::vector<float> pattern;
        float strength;
        float avgReward;
        size_t activationCount;
        double lastActivatedTime;
        std::string category;
    };

    virtual ~VisualizationInterface() = default;

    // Core visualization methods
    virtual void update(const Brain* brain, const AgentBrain* agent, double timestep) = 0;
    virtual void render() = 0;
    virtual void save(const std::string& filepath) = 0;
    
    // Configuration methods
    virtual void setConfig(const VisualizationConfig& config) = 0;
    virtual VisualizationConfig getConfig() const = 0;
    
    // Data access methods
    virtual std::vector<NeuronData> getNeuronData() const = 0;
    virtual std::vector<SynapseData> getSynapseData() const = 0;
    virtual std::vector<RegionData> getRegionData() const = 0;
    virtual std::vector<MotorOutputData> getMotorOutputData() const = 0;
    virtual std::vector<SensorData> getSensorData() const = 0;
    virtual std::vector<MemoryStateData> getMemoryStateData() const = 0;
    virtual std::vector<NeuromodulationData> getNeuromodulationData() const = 0;
    virtual DevelopmentData getDevelopmentData() const = 0;
    virtual std::vector<ConceptData> getConceptData() const = 0;
    
    // Interaction methods
    virtual void setNeuronActivity(size_t neuronId, float activity) = 0;
    virtual void injectCurrent(size_t neuronId, float current) = 0;
    virtual void selectRegion(const std::string& regionName) = 0;
    virtual void zoom(double factor) = 0;
    virtual void pan(double dx, double dy) = 0;
    
    // Callback methods
    virtual void setUpdateCallback(std::function<void(const VisualizationData&)> callback) = 0;
    virtual void setInteractionCallback(std::function<void(const std::string&, const std::map<std::string, std::string>&)> callback) = 0;
    
    // Utility methods
    virtual std::string getVisualizationData() const = 0;
    virtual bool isDataAvailable() const = 0;
    virtual void clear() = 0;
    virtual void reset() = 0;

protected:
    mutable std::mutex dataMutex_;
    std::atomic<bool> dataDirty_;
    std::chrono::system_clock::time_point lastUpdate_;
};

// Console-based Visualization (default backend)
class ConsoleVisualization : public VisualizationInterface {
public:
    ConsoleVisualization();
    ~ConsoleVisualization() override = default;
    
    void update(const Brain* brain, const AgentBrain* agent, double timestep) override;
    void render() override;
    void save(const std::string& filepath) override;
    
    void setConfig(const VisualizationConfig& config) override;
    VisualizationConfig getConfig() const override;
    
    std::vector<NeuronData> getNeuronData() const override;
    std::vector<SynapseData> getSynapseData() const override;
    std::vector<RegionData> getRegionData() const override;
    std::vector<MotorOutputData> getMotorOutputData() const override;
    std::vector<SensorData> getSensorData() const override;
    std::vector<MemoryStateData> getMemoryStateData() const override;
    std::vector<NeuromodulationData> getNeuromodulationData() const override;
    DevelopmentData getDevelopmentData() const override;
    std::vector<ConceptData> getConceptData() const override;
    
    void setNeuronActivity(size_t neuronId, float activity) override;
    void injectCurrent(size_t neuronId, float current) override;
    void selectRegion(const std::string& regionName) override;
    void zoom(double factor) override;
    void pan(double dx, double dy) override;
    
    void setUpdateCallback(std::function<void(const VisualizationData&)> callback) override;
    void setInteractionCallback(std::function<void(const std::string&, const std::map<std::string, std::string>&)> callback) override;
    
    std::string getVisualizationData() const override;
    bool isDataAvailable() const override;
    void clear() override;
    void reset() override;

private:
    VisualizationConfig config_;
    std::vector<NeuronData> neuronData_;
    std::vector<SynapseData> synapseData_;
    std::vector<RegionData> regionData_;
    std::vector<MotorOutputData> motorOutputData_;
    std::vector<SensorData> sensorData_;
    std::vector<MemoryStateData> memoryStateData_;
    std::vector<NeuromodulationData> neuromodulationData_;
    DevelopmentData developmentData_;
    std::vector<ConceptData> conceptData_;
    
    std::string selectedRegion_;
    double zoomLevel_;
    double panX_, panY_;
    
    std::function<void(const VisualizationData&)> updateCallback_;
    std::function<void(const std::string&, const std::map<std::string, std::string>&)> interactionCallback_;
    
    // Rendering methods
    void renderNeuronActivity() const;
    void renderSynapticWeights() const;
    void renderBrainRegions() const;
    void renderMotorOutputs() const;
    void renderSensorInputs() const;
    void renderMemoryStates() const;
    void renderNeuromodulation() const;
    void renderDevelopmentStages() const;
    void renderConceptFormation() const;
    void renderFullDashboard() const;
    
    // Data processing methods
    void processNeuronData(const Brain* brain);
    void processSynapseData(const Brain* brain);
    void processRegionData(const Brain* brain);
    void processMotorOutputData(const AgentBrain* agent);
    void processSensorData(const AgentBrain* agent);
    void processMemoryStateData(const Brain* brain);
    void processNeuromodulationData(const Brain* brain);
    void processDevelopmentData(const AgentBrain* agent);
    void processConceptData(const Brain* brain);
    
    // Data filtering and aggregation
    std::vector<NeuronData> filterNeuronsByRegion(const std::string& regionName) const;
    std::vector<NeuronData> filterNeuronsByType(NeuronType type) const;
    float calculateRegionActivity(const RegionData& region) const;
    std::string formatActivityDisplay(float activity) const;
    std::string formatMetricDisplay(float value, const std::string& unit) const;
};

// Web-based Visualization Backend
class WebVisualization : public VisualizationInterface {
public:
    WebVisualization();
    ~WebVisualization() override;
    
    void update(const Brain* brain, const AgentBrain* agent, double timestep) override;
    void render() override;
    void save(const std::string& filepath) override;
    
    void setConfig(const VisualizationConfig& config) override;
    VisualizationConfig getConfig() const override;
    
    std::vector<NeuronData> getNeuronData() const override;
    std::vector<SynapseData> getSynapseData() const override;
    std::vector<RegionData> getRegionData() const override;
    std::vector<MotorOutputData> getMotorOutputData() const override;
    std::vector<SensorData> getSensorData() const override;
    std::vector<MemoryStateData> getMemoryStateData() const override;
    std::vector<NeuromodulationData> getNeuromodulationData() const override;
    DevelopmentData getDevelopmentData() const override;
    std::vector<ConceptData> getConceptData() const override;
    
    void setNeuronActivity(size_t neuronId, float activity) override;
    void injectCurrent(size_t neuronId, float current) override;
    void selectRegion(const std::string& regionName) override;
    void zoom(double factor) override;
    void pan(double dx, double dy) override;
    
    void setUpdateCallback(std::function<void(const VisualizationData&)> callback) override;
    void setInteractionCallback(std::function<void(const std::string&, const std::map<std::string, std::string>&)> callback) override;
    
    std::string getVisualizationData() const override;
    bool isDataAvailable() const override;
    void clear() override;
    void reset() override;

private:
    VisualizationConfig config_;
    std::string webServerUrl_;
    std::string websocketUrl_;
    std::thread websocketThread_;
    std::atomic<bool> running_;
    
    // WebSocket server for real-time visualization updates
    class WebSocketServer {
    public:
        WebSocketServer(const std::string& host, int port);
        ~WebSocketServer();
        
        void start();
        void stop();
        bool isRunning() const;
        
        void broadcastData(const std::string& data);
        void setDataCallback(std::function<void(const std::string&, const std::string&)> callback);
    
    private:
        std::string host_;
        int port_;
        std::thread serverThread_;
        std::atomic<bool> running_;
        std::function<void(const std::string&, const std::string&)> dataCallback_;
    };
    
    WebSocketServer webSocketServer_;
    
    // Data serialization for web transmission
    std::string serializeNeuronData(const NeuronData& neuron) const;
    std::string serializeSynapseData(const SynapseData& synapse) const;
    std::string serializeRegionData(const RegionData& region) const;
    std::string serializeFullState() const;
};

// Visualization Manager - coordinates multiple visualization instances
class VisualizationManager {
public:
    VisualizationManager();
    ~VisualizationManager();
    
    // Singleton pattern
    static VisualizationManager& getInstance();
    static void resetInstance();
    
    // Visualization management
    bool addVisualization(const std::string& name, std::unique_ptr<VisualizationInterface> visualization);
    bool removeVisualization(const std::string& name);
    bool activateVisualization(const std::string& name);
    bool deactivateVisualization(const std::string& name);
    
    std::vector<std::string> getVisualizationNames() const;
    VisualizationInterface* getVisualization(const std::string& name);
    VisualizationInterface* getActiveVisualization();
    
    // Configuration management
    void setGlobalConfig(const VisualizationConfig& config);
    VisualizationConfig getGlobalConfig() const;
    
    // Update and rendering
    void updateAll(const Brain* brain, const AgentBrain* agent, double timestep);
    void renderAll();
    void saveAll(const std::string& basePath);
    
    // Data access
    std::map<std::string, std::vector<VisualizationInterface::NeuronData>> getAllNeuronData() const;
    std::map<std::string, std::vector<VisualizationInterface::SynapseData>> getAllSynapseData() const;
    std::map<std::string, std::vector<VisualizationInterface::RegionData>> getAllRegionData() const;
    
    // Control methods
    void startAll();
    void stopAll();
    bool isRunning() const;
    
    void setBrainReference(const std::shared_ptr<Brain>& brain);
    void setAgentReference(const std::shared_ptr<AgentBrain>& agent);
    
    // Configuration files
    bool loadVisualizationConfig(const std::string& filepath);
    bool saveVisualizationConfig(const std::string& filepath) const;
    
    // Utility methods
    std::string getVisualizationStatus() const;
    void logPerformanceMetrics();
    void resetPerformanceMetrics();
    
    // Visualization types available
    std::vector<VisualizationInterface::VisualizationType> getAvailableVisualizationTypes() const;
    VisualizationInterface::VisualizationType getVisualizationTypeFromString(const std::string& typeName);
    std::string getVisualizationTypeName(VisualizationInterface::VisualizationType type);

private:
    std::map<std::string, std::unique_ptr<VisualizationInterface>> visualizations_;
    std::string activeVisualizationName_;
    VisualizationConfig globalConfig_;
    std::shared_ptr<Brain> brainReference_;
    std::shared_ptr<AgentBrain> agentReference_;
    
    mutable std::mutex visualizationsMutex_;
    
    // Performance monitoring
    struct PerformanceMetrics {
        double updateTime; // ms
        double renderTime; // ms
        size_t neuronCount;
        size_t synapseCount;
        double memoryUsage; // MB
        double cpuUsage; // %
        std::chrono::system_clock::time_point lastUpdate;
    };
    
    std::map<std::string, PerformanceMetrics> performanceMetrics_;
};

// Convenience functions for quick visualization setup
inline VisualizationManager& Viz() { return VisualizationManager::getInstance(); }
inline void initializeVisualization(const std::string& configFile = "configs/visualization.cfg") {
    VisualizationManager::getInstance().loadVisualizationConfig(configFile);
}

// Visualization data structure for callbacks
struct VisualizationData {
    std::chrono::system_clock::time_point timestamp;
    VisualizationInterface::VisualizationType type;
    std::string source;
    std::map<std::string, std::string> metrics;
    std::map<std::string, double> values;
    std::string formattedData;
    
    VisualizationData() : timestamp(std::chrono::system_clock::now()), type(VisualizationInterface::VisualizationType::FULL_DASHBOARD) {}
    VisualizationData(VisualizationInterface::VisualizationType t, const std::string& src) 
        : timestamp(std::chrono::system_clock::now()), type(t), source(src) {}
};

// Visualization backend factory functions
namespace VisualizationBackend {
    std::unique_ptr<VisualizationInterface> createVisualization(
        VisualizationInterface::VisualizationType type,
        const VisualizationInterface::VisualizationConfig& config);
    
    std::unique_ptr<VisualizationInterface> createConsoleVisualization(
        const VisualizationInterface::VisualizationConfig& config);
    
    std::unique_ptr<VisualizationInterface> createWebVisualization(
        const VisualizationInterface::VisualizationConfig& config);
    
    bool isBackendAvailable(const std::string& backendName);
    std::vector<std::string> getAvailableBackends();
}

// Visualization configuration utilities
namespace VisualizationConfig {
    VisualizationInterface::VisualizationConfig createDefaultConfig(
        VisualizationInterface::VisualizationType type,
        int width = 800,
        int height = 600);
    
    VisualizationInterface::VisualizationConfig createConsoleConfig(
        VisualizationInterface::VisualizationType type,
        int width = 80,
        int height = 24);
    
    VisualizationInterface::VisualizationConfig createWebConfig(
        VisualizationInterface::VisualizationType type,
        int port = 8080,
        const std::string& host = "localhost");
    
    bool saveConfig(const VisualizationInterface::VisualizationConfig& config, const std::string& filepath);
    bool loadConfig(VisualizationInterface::VisualizationConfig& config, const std::string& filepath);
    std::string configToString(const VisualizationInterface::VisualizationConfig& config);
    bool configFromString(VisualizationInterface::VisualizationConfig& config, const std::string& str);
}

// Visualization event system
class VisualizationEvent {
public:
    enum class EventType {
        DATA_UPDATED,
        INTERACTION,
        CONFIGURATION_CHANGED,
        RENDER_COMPLETE,
        ERROR_OCCURRED
    };

    struct EventData {
        EventType type;
        std::string source;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> data;
        std::string errorMessage;
    };

    VisualizationEvent();
    ~VisualizationEvent();
    
    void emitEvent(const EventData& event);
    void addListener(std::function<void(const EventData&)> listener);
    void removeListener(std::function<void(const EventData&)> listener);
    void clearListeners();
    
    std::vector<EventData> getRecentEvents(size_t count = 100) const;
    void clearEvents();

private:
    std::vector<std::function<void(const EventData&)>> listeners_;
    std::vector<EventData> events_;
    mutable std::mutex eventsMutex_;
};

// Global event instance
inline VisualizationEvent& VizEvents() { static VisualizationEvent instance; return instance; }

} // namespace nlm

// Include implementation files
#include "VisualizationInterface.cpp"
#include "ConsoleVisualization.cpp"
#include "WebVisualization.cpp"
