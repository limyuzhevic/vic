#ifndef NLM_PLUGIN_EXAMPLES_WORLDPLUGIN_HPP
#define NLM_PLUGIN_EXAMPLES_WORLDPLUGIN_HPP

#include "AbstractIntegrationPlugin.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {
namespace plugin {

class WorldPlugin : public AbstractIntegrationPlugin {
public:
    WorldPlugin();
    ~WorldPlugin() override;
    
    // IPlugin interface implementation
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    PluginType getType() const override;
    PluginMetadata getMetadata() const override;
    
    bool initialize(const PluginMetadata& metadata) override;
    void shutdown() override;
    bool activate() override;
    bool deactivate() override;
    bool loadDependencies() override;
    bool checkConflicts() override;
    
    // AbstractIntegrationPlugin interface implementation
    std::string getIntegrationType() const override;
    std::vector<std::string> getIntegratedSystems() const override;
    
    bool configureIntegration(const std::unordered_map<std::string, std::string>& config) override;
    std::unordered_map<std::string, std::string> getIntegrationConfig() const override;
    
    bool preIntegrate() override;
    bool postIntegrate() override;
    bool preDeintegrate() override;
    bool postDeintegrate() override;
    
    bool integrateWithSystem(const std::string& systemName, const std::unordered_map<std::string, std::string>& config) override;
    bool deintegrateFromSystem(const std::string& systemName) override;
    bool isIntegratedWithSystem(const std::string& systemName) const override;
    std::vector<std::string> getIntegratedSystemsList() const override;
    
    bool handleEvent(const IntegrationEvent& event) override;
    bool addEventHandler(const std::string& eventType, const std::function<bool(const IntegrationEvent&)>& handler) override;
    bool removeEventHandler(const std::string& eventType, const std::function<bool(const IntegrationEvent&)>& handler) override;
    std::vector<std::string> getEventTypes() const override;
    
    uint64_t getIntegrationCount() const override;
    double getAverageIntegrationTime() const override;
    uint64_t getIntegrationErrors() const override;
    std::string getLastIntegrationError() const override;
    
    void* getIntegrationExtension(const std::string& extensionName) override;
    bool registerIntegrationExtension(const std::string& extensionName, void* extension) override;
    
    bool optimizeIntegration() override;
    bool tuneIntegrationParameters(const std::unordered_map<std::string, double>& parameters) override;
    
    bool validateIntegrationConfiguration() const override;
    bool validateIntegrationDependencies() const override;
    
    // World system functionality
    class WorldSystem {
    public:
        WorldSystem(const std::string& name, WorldPlugin* plugin);
        ~WorldSystem();
        
        std::string getName() const { return name_; }
        std::string getType() const { return type_; }
        std::string getDescription() const { return description_; }
        
        void setType(const std::string& type) { type_ = type; }
        void setDescription(const std::string& desc) { description_ = desc; }
        
        virtual bool update(double dt) = 0;
        virtual bool reset() = 0;
        virtual std::string getState() const = 0;
        
    protected:
        std::string name_;
        std::string type_;
        std::string description_;
        WorldPlugin* plugin_;
    };
    
    class SimpleWorld : public WorldSystem {
    public:
        SimpleWorld(const std::string& name, WorldPlugin* plugin);
        ~SimpleWorld() override;
        
        bool update(double dt) override;
        bool reset() override;
        std::string getState() const override;
        
        // Sensor input
        std::vector<double> getSensorInput(const std::string& sensorType) const;
        bool setSensorInput(const std::string& sensorType, const std::vector<double>& values);
        
        // Motor output
        std::vector<double> getMotorOutput(const std::string& motorType) const;
        bool setMotorOutput(const std::string& motorType, const std::vector<double>& values);
        
        // Environment state
        std::string getEnvironmentState() const;
        bool setEnvironmentState(const std::string& state);
        
    private:
        std::unordered_map<std::string, std::vector<double>> sensorInputs_;
        std::unordered_map<std::string, std::vector<double>> motorOutputs_;
        std::string environmentState_;
    };
    
    WorldSystem* createWorldSystem(const std::string& name, const std::string& type);
    bool destroyWorldSystem(WorldSystem* world);
    std::vector<WorldSystem*> getWorldSystems() const;
    WorldSystem* getWorldSystem(const std::string& name) const;
    
    // World management
    bool addWorldSystem(WorldSystem* world);
    bool removeWorldSystem(const std::string& name);
    bool updateAllWorldSystems(double dt);
    bool resetAllWorldSystems();
    
    // World plugin configuration
    struct WorldConfig {
        double timeStep = 0.01;
        bool enableRendering = false;
        std::string renderMode = "text";
        bool enableLogging = true;
        std::string logLevel = "info";
        double sensorNoiseLevel = 0.01;
        double motorNoiseLevel = 0.01;
        bool enableEventDrivenUpdates = true;
        double updateThreshold = 0.001;
        bool enableCollisionDetection = true;
        bool enablePhysicsSimulation = false;
    };
    
    WorldConfig getWorldConfig() const { return config_; }
    void setWorldConfig(const WorldConfig& config) { config_ = config; }
    
private:
    PluginMetadata metadata_;
    PluginState state_;
    std::string error_;
    WorldConfig config_;
    std::vector<WorldSystem*> worldSystems_;
    std::unordered_map<std::string, void*> integrationExtensions_;
    std::unordered_map<std::string, std::function<bool(const IntegrationEvent&)>> eventHandlers_;
    std::unordered_map<std::string, std::string> config_;
    uint64_t integrationCount_ = 0;
    uint64_t integrationErrors_ = 0;
    std::chrono::duration<double> totalIntegrationTime_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_EXAMPLES_WORLDPLUGIN_HPP