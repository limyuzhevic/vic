// Integration of Advanced Actions System with existing Brain class
// This file provides backward compatibility and integration hooks

#pragma once

#include "AdvancedActions.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

// Advanced integration layer for Brain class
class BrainAdvancedIntegration {
public:
    BrainAdvancedIntegration();
    ~BrainAdvancedIntegration();
    
    // Initialize with existing Brain instance
    bool initialize(Brain* brain);
    
    // Enhanced action production with advanced features
    std::unique_ptr<Action> produceAdvancedAction(const Config& config = Config());
    
    // Action planning and coordination
    std::vector<std::unique_ptr<Action>> planActionSequence(const NeuralRegion& motorRegion,
                                                          const Config& config = Config());
    
    // Action validation for neural commands
    bool validateActionCommand(const Action& action, const NeuralRegion& sourceRegion);
    
    // Real-time action execution
    ActionResult executeActionInContext(std::unique_ptr<Action> action,
                                       const std::unordered_map<std::string, float>& context);
    
    // Action result processing and feedback
    void processActionResult(const ActionResult& result, SimulationStep step);
    
    // Action statistics and metrics
    struct AdvancedActionStats {
        size_t totalActionsProduced = 0;
        size_t successfulActions = 0;
        double averageExecutionTime = 0.0;
        std::unordered_map<ActionType, size_t> actionsByType;
        float noveltyScore = 0.0f;
        float explorationRate = 0.0f;
    };
    
    AdvancedActionStats getAdvancedActionStats() const;
    
    // Configuration management
    void loadAdvancedConfiguration(const Config& config);
    Config exportAdvancedConfiguration() const;
    
    // Performance optimization
    void enableOptimizedActionProcessing(bool enable);
    void setActionProcessingQueueSize(size_t queueSize);
    
    // Plugin support
    template<typename PluginType>
    bool registerPlugin(std::unique_ptr<PluginType> plugin);
    
    template<typename PluginType>
    PluginType* getPlugin();
    
    // Event hooks for advanced features
    virtual void onActionSequenceStarted(const std::vector<std::unique_ptr<Action>>& sequence);
    virtual void onActionSequenceCompleted(const std::vector<ActionResult>& results);
    virtual void onActionPlanned(const Action& action, const NeuralRegion& region);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Brain extension class for advanced features
class BrainAdvancedFeatures {
public:
    BrainAdvancedFeatures();
    ~BrainAdvancedFeatures();
    
    // Enhanced motor system integration
    void integrateWithMotorSystem(MotorSystem* motorSystem);
    
    // Advanced action validation
    class AdvancedActionValidator {
    public:
        bool validateMotorConstraints(const Action& action, const NeuralRegion& motorRegion) const;
        bool validateEnergyConstraints(const Action& action, float availableEnergy) const;
        bool validateTimeConstraints(const Action& action, float maxDuration) const;
        
        // Custom validation rules
        void addCustomRule(const std::string& name, 
                          std::function<bool(const Action&)> rule,
                          const std::string& errorMessage);
        
        std::vector<std::string> validateAll(const Action& action,
                                           float availableEnergy = 1.0f,
                                           float maxDuration = 1.0f) const;
    
    private:
        std::vector<std::function<bool(const Action&)>> customRules_;
        std::vector<std::string> errorMessages_;
    };
    
    // Advanced action execution with neural feedback
    class NeuralActionExecutor {
    public:
        ActionResult executeWithNeuralFeedback(std::unique_ptr<Action> action,
                                              const std::vector<float>& neuralState);
        
        // Neuromodulation integration
        void setNeuromodulator(Dopamine* dopamine);
        void setCuriosity(Curiosity* curiosity);
        void setNovelty(Novelty* novelty);
        void setPredictionError(PredictionError* predictionError);
        
        // Learning integration
        void setPredictionSystem(PredictionSystem* predictionSystem);
        void setLearningSystem(const std::string& learningType,
                             std::unique_ptr<class LearningAlgorithm> algorithm);
    
    private:
        Dopamine* dopamine_ = nullptr;
        Curiosity* curiosity_ = nullptr;
        Novelty* novelty_ = nullptr;
        PredictionError* predictionError_ = nullptr;
        PredictionSystem* predictionSystem_ = nullptr;
        std::unique_ptr<class LearningAlgorithm> learningAlgorithm_;
    };
    
    // Advanced statistics and monitoring
    class ActionMonitor {
    public:
        struct PerformanceMetrics {
            double throughput = 0.0;      // actions per second
            double latency = 0.0;         // average execution time
            float successRate = 0.0f;     // percentage successful
            float explorationRate = 0.0f; // percentage of novel actions
        };
        
        void recordActionExecution(const ActionResult& result);
        PerformanceMetrics getPerformanceMetrics() const;
        
        // Real-time monitoring
        void startMonitoring(float samplingInterval = 1.0f);
        void stopMonitoring();
        bool isMonitoring() const;
        
        // Event callbacks
        std::function<void(const PerformanceMetrics&)> onMetricsUpdated;
    };
    
    // Action template manager for advanced features
    class ActionTemplateManager {
    public:
        void registerTemplate(const ActionTemplate& template_);
        std::vector<ActionTemplate> getTemplates() const;
        std::unique_ptr<Action> createFromTemplate(const std::string& name,
                                                  const std::vector<float>& parameters);
        
        // Template composition
        void composeTemplate(const std::string& baseName,
                           const std::string& derivedName,
                           const std::map<std::string, float>& overrides);
        
        // Template validation
        bool validateTemplate(const std::string& name) const;
        std::string getTemplateError(const std::string& name) const;
    };
    
    // Integration helper methods
    template<typename T>
    T* getExtension() {
        // Type-safe extension retrieval
        return static_cast<T*>(extensions_[typeid(T).name()].get());
    }
    
    template<typename T, typename... Args>
    T* addExtension(Args&&... args) {
        auto extension = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = extension.get();
        extensions_[typeid(T).name()] = std::move(extension);
        return ptr;
    }
    
    // Configuration management for advanced features
    void loadAdvancedFeaturesConfig(const Config& config);
    Config exportAdvancedFeaturesConfig() const;
    
    // Performance optimization
    void enableParallelActionProcessing(bool enable);
    void setActionProcessorCount(int count);
    
    // Event-driven architecture
    void registerActionEventHandler(const std::string& eventType,
                                   std::function<void(const ActionEvent&)> handler);
    
    // Debugging and analysis
    void enableDebugLogging(bool enable);
    void enablePerformanceProfiling(bool enable);
    
    // State management
    void saveActionState(std::ostream& stream) const;
    void loadActionState(std::istream& stream);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global integration helper functions
namespace AdvancedIntegration {
    // Create advanced integration for existing brain
    std::unique_ptr<BrainAdvancedIntegration> createIntegration(Brain& brain);
    
    // Initialize advanced features for brain
    bool initializeAdvancedFeatures(Brain& brain, const Config& config);
    
    // Get advanced features instance
    BrainAdvancedFeatures* getAdvancedFeatures(Brain& brain);
    
    // Check if advanced features are available
    bool hasAdvancedFeatures(Brain& brain);
}

} // namespace nlm
