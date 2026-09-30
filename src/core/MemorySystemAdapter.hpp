// MemorySystemAdapter.hpp
#pragma once

#include <memory>

namespace nlm {

class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Forward declarations for development system
class DevelopmentalStage;

// Unified memory system interface
class MemorySystemInterface {
public:
    virtual ~MemorySystemInterface() = default;
    
    // Memory systems
    virtual NeuralWorkingMemory* getWorkingMemory() = 0;
    virtual NeuralEpisodicMemory* getEpisodicMemory() = 0;
    virtual NeuralAssociativeMemory* getAssociativeMemory() = 0;
    
    // Prediction system
    virtual PredictionSystem* getPredictionSystem() = 0;
    
    // Cognition systems
    virtual NeuralPlanner* getPlanner() = 0;
    virtual ConceptFormation* getConceptFormation() = 0;
    virtual AttentionalSelection* getAttention() = 0;
    
    // Development system
    virtual DevelopmentSystem* getDevelopmentSystem() = 0;
    virtual DevelopmentalStage getDevelopmentalStage() const = 0;
    virtual void setDevelopmentalStage(DevelopmentalStage stage) = 0;
    
    // Neuromodulation systems
    virtual Dopamine* getDopamine() = 0;
    virtual Curiosity* getCuriosity() = 0;
    virtual Novelty* getNovelty() = 0;
    virtual PredictionError* getPredictionErrorSignal() = 0;
};

// Implementation adapter
class MemorySystemAdapter : public MemorySystemInterface {
public:
    explicit MemorySystemAdapter(
        std::unique_ptr<NeuralWorkingMemory> workingMemory,
        std::unique_ptr<NeuralEpisodicMemory> episodicMemory,
        std::unique_ptr<NeuralAssociativeMemory> associativeMemory,
        std::unique_ptr<PredictionSystem> predictionSystem,
        std::unique_ptr<NeuralPlanner> planner,
        std::unique_ptr<ConceptFormation> conceptFormation,
        std::unique_ptr<AttentionalSelection> attention,
        std::unique_ptr<DevelopmentSystem> developmentSystem,
        std::unique_ptr<Dopamine> dopamine,
        std::unique_ptr<Curiosity> curiosity,
        std::unique_ptr<Novelty> novelty,
        std::unique_ptr<PredictionError> predictionError);
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory() override { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() override { return episodicMemory_.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() override { return associativeMemory_.get(); }
    
    // Prediction system
    PredictionSystem* getPredictionSystem() override { return predictionSystem_.get(); }
    
    // Cognition systems
    NeuralPlanner* getPlanner() override { return planner_.get(); }
    ConceptFormation* getConceptFormation() override { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() override { return attention_.get(); }
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem() override { return developmentSystem_.get(); }
    DevelopmentalStage getDevelopmentalStage() const override { return developmentalStage_; }
    void setDevelopmentalStage(DevelopmentalStage stage) override { developmentalStage_ = stage; }
    
    // Neuromodulation systems
    Dopamine* getDopamine() override { return dopamine_.get(); }
    Curiosity* getCuriosity() override { return curiosity_.get(); }
    Novelty* getNovelty() override { return novelty_.get(); }
    PredictionError* getPredictionErrorSignal() override { return predictionError_.get(); }
    
private:
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    std::unique_ptr<PredictionSystem> predictionSystem_;
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
    DevelopmentalStage developmentalStage_;
};

} // namespace nlm