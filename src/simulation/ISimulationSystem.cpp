#include "ISimulationSystem.hpp"
#include "SimulationSystemProxy.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/AttentionalSelection.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <memory>

namespace nlm {

// NeuralWorkingMemorySystem implementation
class NeuralWorkingMemorySystem : public ISimulationSystem {
public:
    NeuralWorkingMemorySystem() : enabled_(true) {}
    
    void Initialize(Brain* brain) override {
        NLM_LOG_INFO("NeuralWorkingMemorySystem: Initializing NeuralWorkingMemory");
        workingMemory_ = std::make_unique<NeuralWorkingMemory>();
        workingMemory_->initialize(brain);
        workingMemory_->setCapacity(100);
        NLM_LOG_INFO("NeuralWorkingMemorySystem: NeuralWorkingMemory initialized successfully");
    }
    
    void Update(double timestep) override {
        if (!enabled_) return;
        workingMemory_->update(static_cast<TimestepDuration>(timestep));
    }
    
    void Reset() override {
        NLM_LOG_INFO("NeuralWorkingMemorySystem: Resetting NeuralWorkingMemory");
        workingMemory_->clear();
    }
    
    bool IsEnabled() const override {
        return enabled_;
    }
    
    void Enable() override {
        if (!enabled_) {
            bool oldStatus = enabled_;
            enabled_ = true;
            NLM_LOG_INFO("NeuralWorkingMemorySystem: System enabled");
            LogStateChange("NeuralWorkingMemory", oldStatus, enabled_);
        }
    }
    
    void Disable() override {
        if (enabled_) {
            bool oldStatus = enabled_;
            enabled_ = false;
            NLM_LOG_INFO("NeuralWorkingMemorySystem: System disabled");
            LogStateChange("NeuralWorkingMemory", oldStatus, enabled_);
        }
    }
    
    const char* GetName() const override {
        return "NeuralWorkingMemorySystem";
    }
    
    std::string GetStatus() const override {
        std::string status = "NeuralWorkingMemorySystem: " + std::string(enabled_ ? "ENABLED" : "DISABLED");
        if (workingMemory_) {
            status += ", Active traces: " + std::to_string(workingMemory_->getActiveTraces());
        }
        return status;
    }
    
private:
    void LogStateChange(const std::string& systemName, bool oldStatus, bool newStatus) {
        NLM_LOG_INFO("NeuralWorkingMemorySystem: State change - " + systemName + ": " + 
                    std::string(oldStatus ? "ENABLED" : "DISABLED") + " -> " + 
                    std::string(newStatus ? "ENABLED" : "DISABLED"));
    }
    
    bool enabled_;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
};

// NeuralEpisodicMemorySystem implementation
class NeuralEpisodicMemorySystem : public ISimulationSystem {
public:
    NeuralEpisodicMemorySystem() : enabled_(true) {}
    
    void Initialize(Brain* brain) override {
        NLM_LOG_INFO("NeuralEpisodicMemorySystem: Initializing NeuralEpisodicMemory");
        episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
        episodicMemory_->initialize(brain);
        NLM_LOG_INFO("NeuralEpisodicMemorySystem: NeuralEpisodicMemory initialized successfully");
    }
    
    void Update(double timestep) override {
        if (!enabled_) return;
        // NeuralEpisodicMemory doesn't have an update method, but we can check for consolidation
        if (episodicMemory_->getEpisodeCount() > 0) {
            episodicMemory_->consolidate(0.3f);
        }
    }
    
    void Reset() override {
        NLM_LOG_INFO("NeuralEpisodicMemorySystem: Resetting NeuralEpisodicMemory");
        episodicMemory_->clear();
    }
    
    bool IsEnabled() const override {
        return enabled_;
    }
    
    void Enable() override {
        if (!enabled_) {
            bool oldStatus = enabled_;
            enabled_ = true;
            NLM_LOG_INFO("NeuralEpisodicMemorySystem: System enabled");
            LogStateChange("NeuralEpisodicMemory", oldStatus, enabled_);
        }
    }
    
    void Disable() override {
        if (enabled_) {
            bool oldStatus = enabled_;
            enabled_ = false;
            NLM_LOG_INFO("NeuralEpisodicMemorySystem: System disabled");
            LogStateChange("NeuralEpisodicMemory", oldStatus, enabled_);
        }
    }
    
    const char* GetName() const override {
        return "NeuralEpisodicMemorySystem";
    }
    
    std::string GetStatus() const override {
        std::string status = "NeuralEpisodicMemorySystem: " + std::string(enabled_ ? "ENABLED" : "DISABLED");
        if (episodicMemory_) {
            status += ", Episodes: " + std::to_string(episodicMemory_->getEpisodeCount());
        }
        return status;
    }
    
private:
    void LogStateChange(const std::string& systemName, bool oldStatus, bool newStatus) {
        NLM_LOG_INFO("NeuralEpisodicMemorySystem: State change - " + systemName + ": " + 
                    std::string(oldStatus ? "ENABLED" : "DISABLED") + " -> " + 
                    std::string(newStatus ? "ENABLED" : "DISABLED"));
    }
    
    bool enabled_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
};

// DopamineNeuromodulationSystem implementation
class DopamineNeuromodulationSystem : public ISimulationSystem {
public:
    DopamineNeuromodulationSystem() : enabled_(true) {}
    
    void Initialize(Brain* brain) override {
        NLM_LOG_INFO("DopamineNeuromodulationSystem: Initializing Dopamine");
        dopamine_ = std::make_unique<Dopamine>();
        NLM_LOG_INFO("DopamineNeuromodulationSystem: Dopamine initialized successfully");
    }
    
    void Update(double timestep) override {
        if (!enabled_) return;
        dopamine_->update(static_cast<TimestepDuration>(timestep));
    }
    
    void Reset() override {
        NLM_LOG_INFO("DopamineNeuromodulationSystem: Resetting Dopamine");
        dopamine_->setLevel(0.0f);
    }
    
    bool IsEnabled() const override {
        return enabled_;
    }
    
    void Enable() override {
        if (!enabled_) {
            bool oldStatus = enabled_;
            enabled_ = true;
            NLM_LOG_INFO("DopamineNeuromodulationSystem: System enabled");
            LogStateChange("Dopamine", oldStatus, enabled_);
        }
    }
    
    void Disable() override {
        if (enabled_) {
            bool oldStatus = enabled_;
            enabled_ = false;
            NLM_LOG_INFO("DopamineNeuromodulationSystem: System disabled");
            LogStateChange("Dopamine", oldStatus, enabled_);
        }
    }
    
    const char* GetName() const override {
        return "DopamineNeuromodulationSystem";
    }
    
    std::string GetStatus() const override {
        std::string status = "DopamineNeuromodulationSystem: " + std::string(enabled_ ? "ENABLED" : "DISABLED");
        if (dopamine_) {
            status += ", Level: " + std::to_string(dopamine_->getLevel());
        }
        return status;
    }
    
private:
    void LogStateChange(const std::string& systemName, bool oldStatus, bool newStatus) {
        NLM_LOG_INFO("DopamineNeuromodulationSystem: State change - " + systemName + ": " + 
                    std::string(oldStatus ? "ENABLED" : "DISABLED") + " -> " + 
                    std::string(newStatus ? "ENABLED" : "DISABLED"));
    }
    
    bool enabled_;
    std::unique_ptr<Dopamine> dopamine_;
};

// CuriosityNeuromodulationSystem implementation
class CuriosityNeuromodulationSystem : public ISimulationSystem {
public:
    CuriosityNeuromodulationSystem() : enabled_(true) {}
    
    void Initialize(Brain* brain) override {
        NLM_LOG_INFO("CuriosityNeuromodulationSystem: Initializing Curiosity");
        curiosity_ = std::make_unique<Curiosity>();
        curiosity_->initialize(brain);
        NLM_LOG_INFO("CuriosityNeuromodulationSystem: Curiosity initialized successfully");
    }
    
    void Update(double timestep) override {
        if (!enabled_) return;
        // Curiosity uses novelty and prediction error for update
        curiosity_->update(0.5f, 0.3f, static_cast<TimestepDuration>(timestep));
    }
    
    void Reset() override {
        NLM_LOG_INFO("CuriosityNeuromodulationSystem: Resetting Curiosity");
        // Curiosity doesn't have a reset method, just reinitialize
        curiosity_.reset();
    }
    
    bool IsEnabled() const override {
        return enabled_;
    }
    
    void Enable() override {
        if (!enabled_) {
            bool oldStatus = enabled_;
            enabled_ = true;
            NLM_LOG_INFO("CuriosityNeuromodulationSystem: System enabled");
            LogStateChange("Curiosity", oldStatus, enabled_);
        }
    }
    
    void Disable() override {
        if (enabled_) {
            bool oldStatus = enabled_;
            enabled_ = false;
            NLM_LOG_INFO("CuriosityNeuromodulationSystem: System disabled");
            LogStateChange("Curiosity", oldStatus, enabled_);
        }
    }
    
    const char* GetName() const override {
        return "CuriosityNeuromodulationSystem";
    }
    
    std::string GetStatus() const override {
        std::string status = "CuriosityNeuromodulationSystem: " + std::string(enabled_ ? "ENABLED" : "DISABLED");
        if (curiosity_) {
            status += ", Level: " + std::to_string(curiosity_->getLevel());
        }
        return status;
    }
    
private:
    void LogStateChange(const std::string& systemName, bool oldStatus, bool newStatus) {
        NLM_LOG_INFO("CuriosityNeuromodulationSystem: State change - " + systemName + ": " + 
                    std::string(oldStatus ? "ENABLED" : "DISABLED") + " -> " + 
                    std::string(newStatus ? "ENABLED" : "DISABLED"));
    }
    
    bool enabled_;
    std::unique_ptr<Curiosity> curiosity_;
};

// AttentionSystem implementation
class AttentionSystem : public ISimulationSystem {
public:
    AttentionSystem() : enabled_(true) {}
    
    void Initialize(Brain* brain) override {
        NLM_LOG_INFO("AttentionSystem: Initializing Attention");
        attention_ = std::make_unique<AttentionalSelection>();
        attention_->initialize(brain);
        attention_->setInhibitionStrength(0.5f);
        attention_->setExcitationStrength(1.5f);
        NLM_LOG_INFO("AttentionSystem: Attention initialized successfully");
    }
    
    void Update(double timestep) override {
        if (!enabled_) return;
        attention_->update(static_cast<TimestepDuration>(timestep));
    }
    
    void Reset() override {
        NLM_LOG_INFO("AttentionSystem: Resetting Attention");
        attention_->reset();
    }
    
    bool IsEnabled() const override {
        return enabled_;
    }
    
    void Enable() override {
        if (!enabled_) {
            bool oldStatus = enabled_;
            enabled_ = true;
            NLM_LOG_INFO("AttentionSystem: System enabled");
            LogStateChange("Attention", oldStatus, enabled_);
        }
    }
    
    void Disable() override {
        if (enabled_) {
            bool oldStatus = enabled_;
            enabled_ = false;
            NLM_LOG_INFO("AttentionSystem: System disabled");
            LogStateChange("Attention", oldStatus, enabled_);
        }
    }
    
    const char* GetName() const override {
        return "AttentionSystem";
    }
    
    std::string GetStatus() const override {
        std::string status = "AttentionSystem: " + std::string(enabled_ ? "ENABLED" : "DISABLED");
        if (attention_) {
            status += ", Inhibition: " + std::to_string(attention_->getInhibitionFor(NeuronId(0)));
            status += ", Excitation: " + std::to_string(attention_->getExcitationFor(NeuronId(0)));
        }
        return status;
    }
    
private:
    void LogStateChange(const std::string& systemName, bool oldStatus, bool newStatus) {
        NLM_LOG_INFO("AttentionSystem: State change - " + systemName + ": " + 
                    std::string(oldStatus ? "ENABLED" : "DISABLED") + " -> " + 
                    std::string(newStatus ? "ENABLED" : "DISABLED"));
    }
    
    bool enabled_;
    std::unique_ptr<AttentionalSelection> attention_;
};

// PredictionSystem implementation
class PredictionSystem : public ISimulationSystem {
public:
    PredictionSystem() : enabled_(true) {}
    
    void Initialize(Brain* brain) override {
        NLM_LOG_INFO("PredictionSystem: Initializing PredictionSystem");
        prediction_ = std::make_unique<PredictionSystem>();
        NLM_LOG_INFO("PredictionSystem: PredictionSystem initialized successfully");
    }
    
    void Update(double timestep) override {
        if (!enabled_) return;
        // PredictionSystem doesn't have an update method, just track prediction error
        float error = prediction_->getPredictionError();
        // Keep error history
    }
    
    void Reset() override {
        NLM_LOG_INFO("PredictionSystem: Resetting PredictionSystem");
        prediction_->clearHistory();
    }
    
    bool IsEnabled() const override {
        return enabled_;
    }
    
    void Enable() override {
        if (!enabled_) {
            bool oldStatus = enabled_;
            enabled_ = true;
            NLM_LOG_INFO("PredictionSystem: System enabled");
            LogStateChange("PredictionSystem", oldStatus, enabled_);
        }
    }
    
    void Disable() override {
        if (enabled_) {
            bool oldStatus = enabled_;
            enabled_ = false;
            NLM_LOG_INFO("PredictionSystem: System disabled");
            LogStateChange("PredictionSystem", oldStatus, enabled_);
        }
    }
    
    const char* GetName() const override {
        return "PredictionSystem";
    }
    
    std::string GetStatus() const override {
        std::string status = "PredictionSystem: " + std::string(enabled_ ? "ENABLED" : "DISABLED");
        if (prediction_) {
            status += ", Error: " + std::to_string(prediction_->getPredictionError());
            status += ", Confidence: " + std::to_string(prediction_->getConfidence());
        }
        return status;
    }
    
private:
    void LogStateChange(const std::string& systemName, bool oldStatus, bool newStatus) {
        NLM_LOG_INFO("PredictionSystem: State change - " + systemName + ": " + 
                    std::string(oldStatus ? "ENABLED" : "DISABLED") + " -> " + 
                    std::string(newStatus ? "ENABLED" : "DISABLED"));
    }
    
    bool enabled_;
    std::unique_ptr<PredictionSystem> prediction_;
};

// Function to create and register all default systems
void InitializeDefaultSystems(SimulationSystemProxy& proxy, Brain* brain) {
    NLM_LOG_INFO("ISimulationSystem: Initializing default simulation systems");
    
    // Register all systems
    proxy.AddSystem(std::make_unique<NeuralWorkingMemorySystem>());
    proxy.AddSystem(std::make_unique<NeuralEpisodicMemorySystem>());
    proxy.AddSystem(std::make_unique<DopamineNeuromodulationSystem>());
    proxy.AddSystem(std::make_unique<CuriosityNeuromodulationSystem>());
    proxy.AddSystem(std::make_unique<AttentionSystem>());
    proxy.AddSystem(std::make_unique<PredictionSystem>());
    
    // Initialize all systems
    proxy.InitializeAll(brain);
    
    NLM_LOG_INFO("ISimulationSystem: All default systems initialized successfully");
}

} // namespace nlm
