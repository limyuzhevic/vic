// Status reporting system for NLM simulations
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <unordered_map>

namespace nlm {

class SystemStatus {
public:
    struct SystemStatusReport {
        // Neural system status
        bool neuralSystemHealthy;
        size_t neuronCount;
        float averageFiringRate;
        size_t activeNeuronCount;
        float excitationInhibitionRatio;
        
        // Memory system status
        bool workingMemoryIntegrated;
        size_t workingMemoryTraces;
        bool episodicMemoryIntegrated;
        size_t episodicMemoryEpisodes;
        bool associativeMemoryIntegrated;
        
        // Plasticity system status
        bool stdpEnabled;
        bool hebbianEnabled;
        bool structuralPlasticityEnabled;
        float plasticityModulationFactor;
        
        // Neuromodulation system status
        bool dopamineIntegrated;
        float dopamineLevel;
        bool curiosityIntegrated;
        float curiosityLevel;
        bool noveltyIntegrated;
        float noveltyLevel;
        bool predictionErrorIntegrated;
        float predictionErrorLevel;
        
        // Cognitive system status
        bool plannerIntegrated;
        bool conceptFormationIntegrated;
        bool attentionIntegrated;
        
        // Development system status
        bool developmentIntegrated;
        std::string developmentalStage;
        float developmentProgress;
        
        // Environmental integration
        bool worldIntegration;
        bool sensoryIntegration;
        bool motorIntegration;
        
        // Simulation metrics
        uint64_t totalSteps;
        double elapsedTime;
        float averageReward;
        size_t spikeEventsCount;
        
        // Health indicators
        bool isHealthy() const;
        std::string getHealthSummary() const;
        std::vector<std::string> getIssues() const;
        
        // Timestamp and metadata
        time_t timestamp;
        std::string simulationId;
        std::string configurationHash;
    };
    
    SystemStatus();
    
    // Neural system status
    void setNeuralStatus(bool healthy, size_t neuronCount, float avgFiringRate,
                        size_t activeNeurons, float eiratio);
    
    // Memory system status
    void setWorkingMemoryStatus(bool integrated, size_t traces);
    void setEpisodicMemoryStatus(bool integrated, size_t episodes);
    void setAssociativeMemoryStatus(bool integrated);
    
    // Plasticity system status
    void setPlasticityStatus(bool stdp, bool hebbian, bool structural,
                           float modulationFactor);
    
    // Neuromodulation system status
    void setDopamineStatus(bool integrated, float level);
    void setCuriosityStatus(bool integrated, float level);
    void setNoveltyStatus(bool integrated, float level);
    void setPredictionErrorStatus(bool integrated, float level);
    
    // Cognitive system status
    void setPlannerStatus(bool integrated);
    void setConceptFormationStatus(bool integrated);
    void setAttentionStatus(bool integrated);
    
    // Development system status
    void setDevelopmentStatus(bool integrated, const std::string& stage,
                             float progress);
    
    // Environmental integration status
    void setWorldIntegrationStatus(bool integrated);
    void setSensoryIntegrationStatus(bool integrated);
    void setMotorIntegrationStatus(bool integrated);
    
    // Simulation metrics
    void setSimulationMetrics(uint64_t steps, double elapsedTime,
                             float averageReward, size_t spikeEvents);
    
    // Configuration and metadata
    void setConfiguration(const std::string& id, const std::string& hash);
    
    // Report generation
    SystemStatusReport generateReport() const;
    std::string generateJsonReport() const;
    std::string generateTextReport() const;
    std::string generateDetailedReport() const;
    
    // Issue tracking
    void addIssue(const std::string& category, const std::string& description,
                  const std::string& severity = "WARNING");
    
    // Health checking
    bool isSystemHealthy() const;
    
private:
    SystemStatusReport status_;
    std::vector<SystemStatusReport> statusHistory_;
    std::unordered_map<std::string, std::vector<std::string>> issues_;
    
    void validateHealth() const;
    bool checkNeuralHealth() const;
    bool checkMemoryHealth() const;
    bool checkPlasticityHealth() const;
    bool checkNeuromodulationHealth() const;
    bool checkCognitiveHealth() const;
    bool checkDevelopmentHealth() const;
    bool checkEnvironmentalHealth() const;
    
    // Helper functions
    std::string generateHash(const std::string& input) const;
    std::string formatTimestamp(time_t timestamp) const;
};

} // namespace nlm
