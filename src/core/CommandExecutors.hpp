#pragma once

#include "Command.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../environment/World.hpp"
#include "../agent/AgentBrain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

namespace nlm {

// Brain State Commands
class BrainSaveCommand : public Command {
public:
    BrainSaveCommand(Brain* brain) : brain_(brain) {}
    
    CommandResult execute() override {
        if (!brain_) return CommandResult(false, "Brain not initialized");
        std::string filepath = "brain_checkpoint_" + 
            std::to_string(context.commandId) + ".bin";
        bool success = brain_->save(filepath);
        if (success) {
            return CommandResult(true, "Brain state saved to " + filepath);
        } else {
            return CommandResult(false, "Failed to save brain state");
        }
    }
    
    std::string getName() const override { return "save"; }
    std::string getDescription() const override { return "Save brain state to file"; }
    std::string getHelp() const override { return "Saves the current brain state to a checkpoint file.\nUsage: save [filepath]"; }
    bool isUndoable() const override { return false; }
    std::string getCategory() const override { return "Brain State"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<BrainSaveCommand>(*this);
    }
    
private:
    Brain* brain_;
    mutable CommandContext context;
};

class BrainLoadCommand : public Command {
public:
    BrainLoadCommand(Brain* brain) : brain_(brain) {}
    
    CommandResult execute() override {
        if (!brain_) return CommandResult(false, "Brain not initialized");
        std::string filepath = parameters_;
        if (filepath.empty()) {
            filepath = "brain_checkpoint_" + 
                std::to_string(context.commandId - 1) + ".bin";
        }
        bool success = brain_->load(filepath);
        if (success) {
            return CommandResult(true, "Brain state loaded from " + filepath);
        } else {
            return CommandResult(false, "Failed to load brain state from " + filepath);
        }
    }
    
    std::string getName() const override { return "load"; }
    std::string getDescription() const override { return "Load brain state from file"; }
    std::string getHelp() const override { return "Loads a brain state from a checkpoint file.\nUsage: load [filepath]"; }
    bool isUndoable() const override { return false; }
    std::string getCategory() const override { return "Brain State"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<BrainLoadCommand>(*this);
    }
    bool setParameters(const std::string& params) override {
        parameters_ = params;
        return true;
    }
    
private:
    Brain* brain_;
    mutable CommandContext context;
    std::string parameters_;
};

class BrainResetCommand : public Command {
public:
    BrainResetCommand(Brain* brain) : brain_(brain) {}
    
    CommandResult execute() override {
        if (!brain_) return CommandResult(false, "Brain not initialized");
        brain_->reset();
        return CommandResult(true, "Brain state reset to initial state");
    }
    
    std::string getName() const override { return "reset"; }
    std::string getDescription() const override { return "Reset brain state to initial conditions"; }
    std::string getHelp() const override { return "Resets the brain to its initial state, clearing all memories and weights.\nUsage: reset"; }
    bool isUndoable() const override { return false; }
    std::string getCategory() const override { return "Brain State"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<BrainResetCommand>(*this);
    }
    
private:
    Brain* brain_;
    mutable CommandContext context;
};

class BrainStepCommand : public Command {
public:
    BrainStepCommand(Brain* brain, SimulationStep steps = 1) : brain_(brain), steps_(steps) {}
    
    CommandResult execute() override {
        if (!brain_) return CommandResult(false, "Brain not initialized");
        for (SimulationStep i = 0; i < steps_; ++i) {
            brain_->step(i);
        }
        return CommandResult(true, "Brain stepped " + std::to_string(steps_) + " time units");
    }
    
    std::string getName() const override { return "step"; }
    std::string getDescription() const override { return "Advance brain simulation by specified steps"; }
    std::string getHelp() const override { return "Advances the brain simulation forward in time.\nUsage: step [num_steps] (default: 1)"; }
    std::string getParameters() const override { return "[num_steps]"; }
    bool setParameters(const std::string& params) override {
        // Parse parameters (simplified)
        if (!params.empty()) {
            try {
                steps_ = std::stoull(params);
            } catch (...) {
                return false;
            }
        }
        return true;
    }
    std::string getCategory() const override { return "Brain State"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<BrainStepCommand>(*this);
    }
    
private:
    Brain* brain_;
    mutable CommandContext context;
    SimulationStep steps_;
};

class BrainStatusCommand : public Command {
public:
    BrainStatusCommand(Brain* brain) : brain_(brain) {}
    
    CommandResult execute() override {
        if (!brain_) return CommandResult(false, "Brain not initialized");
        
        std::string status = "=== NLM Brain Status ===\n";
        status += "Regions: " + std::to_string(brain_->getRegionCount()) + "\n";
        status += "Total neurons: " + std::to_string(brain_->getTotalNeuronCount()) + "\n";
        status += "Total synapses: " + std::to_string(brain_->getTotalSynapseCount()) + "\n";
        status += "Firing neurons: " + std::to_string(brain_->getFiringNeuronCount()) + "\n";
        status += "Excitatory/Inhibitory ratio: " + std::to_string(brain_->getExcitationInhibitionRatio()) + "\n";
        status += "Total spike count: " + std::to_string(brain_->getTotalSpikeCount()) + "\n";
        status += "Pending spikes: " + std::to_string(brain_->getPendingSpikeEventCount()) + "\n";
        
        return CommandResult(true, status);
    }
    
    std::string getName() const override { return "status"; }
    std::string getDescription() const override { return "Get current brain status and statistics"; }
    std::string getHelp() const override { return "Displays current brain status, statistics, and system information.\nUsage: status"; }
    bool isUndoable() const override { return false; }
    std::string getCategory() const override { return "Brain State"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<BrainStatusCommand>(*this);
    }
    
private:
    Brain* brain_;
    mutable CommandContext context;
};

// Memory System Commands
class WorkingMemoryClearCommand : public Command {
public:
    WorkingMemoryClearCommand(NeuralWorkingMemory* workingMemory) : workingMemory_(workingMemory) {}
    
    CommandResult execute() override {
        if (!workingMemory_) return CommandResult(false, "Working memory not initialized");
        workingMemory_->clear();
        return CommandResult(true, "Working memory cleared");
    }
    
    std::string getName() const override { return "working_memory_clear"; }
    std::string getDescription() const override { return "Clear working memory";
    std::string getHelp() const override { return "Clears all active items from working memory.\nUsage: working_memory_clear"; }
    bool isUndoable() const override { return false; }
    std::string getCategory() const override { return "Memory System"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<WorkingMemoryClearCommand>(*this);
    }
    
private:
    NeuralWorkingMemory* workingMemory_;
    mutable CommandContext context;
};

class WorkingMemorySetCapacityCommand : public Command {
public:
    WorkingMemorySetCapacityCommand(NeuralWorkingMemory* workingMemory, size_t capacity) 
        : workingMemory_(workingMemory), capacity_(capacity) {}
    
    CommandResult execute() override {
        if (!workingMemory_) return CommandResult(false, "Working memory not initialized");
        workingMemory_->setCapacity(capacity_);
        return CommandResult(true, "Working memory capacity set to " + std::to_string(capacity_));
    }
    
    std::string getName() const override { return "working_memory_set_capacity"; }
    std::string getDescription() const override { return "Set working memory capacity"; }
    std::string getHelp() const override { return "Sets the capacity of working memory.\nUsage: working_memory_set_capacity <capacity>"; }
    std::string getParameters() const override { return "<capacity>"; }
    bool setParameters(const std::string& params) override {
        try {
            capacity_ = std::stoull(params);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Memory System"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<WorkingMemorySetCapacityCommand>(*this);
    }
    
private:
    NeuralWorkingMemory* workingMemory_;
    mutable CommandContext context;
    size_t capacity_;
};

class EpisodicMemoryClearCommand : public Command {
public:
    EpisodicMemoryClearCommand(NeuralEpisodicMemory* episodicMemory) : episodicMemory_(episodicMemory) {}
    
    CommandResult execute() override {
        if (!episodicMemory_) return CommandResult(false, "Episodic memory not initialized");
        episodicMemory_->clear();
        return CommandResult(true, "Episodic memory cleared");
    }
    
    std::string getName() const override { return "episodic_memory_clear"; }
    std::string getDescription() const override { return "Clear episodic memory";
    std::string getHelp() const override { return "Clears all stored episodes from episodic memory.\nUsage: episodic_memory_clear"; }
    bool isUndoable() const override { return false; }
    std::string getCategory() const override { return "Memory System"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<EpisodicMemoryClearCommand>(*this);
    }
    
private:
    NeuralEpisodicMemory* episodicMemory_;
    mutable CommandContext context;
};

class AssociativeMemoryClearCommand : public Command {
public:
    AssociativeMemoryClearCommand(NeuralAssociativeMemory* associativeMemory) : associativeMemory_(associativeMemory) {}
    
    CommandResult execute() override {
        if (!associativeMemory_) return CommandResult(false, "Associative memory not initialized");
        associativeMemory_->clear();
        return CommandResult(true, "Associative memory cleared");
    }
    
    std::string getName() const override { return "associative_memory_clear"; }
    std::string getDescription() const override { return "Clear associative memory";
    std::string getHelp() const override { return "Clears all associations from associative memory.\nUsage: associative_memory_clear"; }
    bool isUndoable() const override { return false; }
    std::string getCategory() const override { return "Memory System"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<AssociativeMemoryClearCommand>(*this);
    }
    
private:
    NeuralAssociativeMemory* associativeMemory_;
    mutable CommandContext context;
};

// Neuromodulation Commands
class DopamineSetLevelCommand : public Command {
public:
    DopamineSetLevelCommand(Dopamine* dopamine, float level) : dopamine_(dopamine), level_(level) {}
    
    CommandResult execute() override {
        if (!dopamine_) return CommandResult(false, "Dopamine system not initialized");
        dopamine_->setLevel(level_);
        return CommandResult(true, "Dopamine level set to " + std::to_string(level_));
    }
    
    std::string getName() const override { return "dopamine_set_level"; }
    std::string getDescription() const override { return "Set dopamine level";
    std::string getHelp() const override { return "Sets the dopamine level (0.0-1.0).\nUsage: dopamine_set_level <level>"; }
    std::string getParameters() const override { return "<level>"; }
    bool setParameters(const std::string& params) override {
        try {
            level_ = std::stof(params);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Neuromodulation"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<DopamineSetLevelCommand>(*this);
    }
    
private:
    Dopamine* dopamine_;
    mutable CommandContext context;
    float level_;
};

class CuriositySetLevelCommand : public Command {
public:
    CuriositySetLevelCommand(Curiosity* curiosity, float level) : curiosity_(curiosity), level_(level) {}
    
    CommandResult execute() override {
        if (!curiosity_) return CommandResult(false, "Curiosity system not initialized");
        curiosity_->setLevel(level_);
        return CommandResult(true, "Curiosity level set to " + std::to_string(level_));
    }
    
    std::string getName() const override { return "curiosity_set_level"; }
    std::string getDescription() const override { return "Set curiosity level";
    std::string getHelp() const override { return "Sets the curiosity level (0.0-1.0).\nUsage: curiosity_set_level <level>"; }
    std::string getParameters() const override { return "<level>"; }
    bool setParameters(const std::string& params) override {
        try {
            level_ = std::stof(params);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Neuromodulation"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<CuriositySetLevelCommand>(*this);
    }
    
private:
    Curiosity* curiosity_;
    mutable CommandContext context;
    float level_;
};

class NoveltySetLevelCommand : public Command {
public:
    NoveltySetLevelCommand(Novelty* novelty, float level) : novelty_(novelty), level_(level) {}
    
    CommandResult execute() override {
        if (!novelty_) return CommandResult(false, "Novelty system not initialized");
        novelty_->setLevel(level_);
        return CommandResult(true, "Novelty level set to " + std::to_string(level_));
    }
    
    std::string getName() const override { return "novelty_set_level"; }
    std::string getDescription() const override { return "Set novelty level";
    std::string getHelp() const override { return "Sets the novelty level (0.0-1.0).\nUsage: novelty_set_level <level>"; }
    std::string getParameters() const override { return "<level>"; }
    bool setParameters(const std::string& params) override {
        try {
            level_ = std::stof(params);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Neuromodulation"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<NoveltySetLevelCommand>(*this);
    }
    
private:
    Novelty* novelty_;
    mutable CommandContext context;
    float level_;
};

class AcetylcholineSetLevelCommand : public Command {
public:
    AcetylcholineSetLevelCommand(Acetylcholine* acetylcholine, float level) : acetylcholine_(acetylcholine), level_(level) {}
    
    CommandResult execute() override {
        if (!acetylcholine_) return CommandResult(false, "Acetylcholine system not initialized");
        acetylcholine_->setLevel(level_);
        return CommandResult(true, "Acetylcholine level set to " + std::to_string(level_));
    }
    
    std::string getName() const override { return "acetylcholine_set_level"; }
    std::string getDescription() const override { return "Set acetylcholine level";
    std::string getHelp() const override { return "Sets the acetylcholine level (0.0-1.0).\nUsage: acetylcholine_set_level <level>"; }
    std::string getParameters() const override { return "<level>"; }
    bool setParameters(const std::string& params) override {
        try {
            level_ = std::stof(params);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Neuromodulation"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<AcetylcholineSetLevelCommand>(*this);
    }
    
private:
    Acetylcholine* acetylcholine_;
    mutable CommandContext context;
    float level_;
};

class NorepinephrineSetLevelCommand : public Command {
public:
    NorepinephrineSetLevelCommand(Norepinephrine* norepinephrine, float level) : norepinephrine_(norepinephrine), level_(level) {}
    
    CommandResult execute() override {
        if (!norepinephrine_) return CommandResult(false, "Norepinephrine system not initialized");
        norepinephrine_->setLevel(level_);
        return CommandResult(true, "Norepinephrine level set to " + std::to_string(level_));
    }
    
    std::string getName() const override { return "norepinephrine_set_level"; }
    std::string getDescription() const override { return "Set norepinephrine level";
    std::string getHelp() const override { return "Sets the norepinephrine level (0.0-1.0).\nUsage: norepinephrine_set_level <level>"; }
    std::string getParameters() const override { return "<level>"; }
    bool setParameters(const std::string& params) override {
        try {
            level_ = std::stof(params);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Neuromodulation"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<NorepinephrineSetLevelCommand>(*this);
    }
    
private:
    Norepinephrine* norepinephrine_;
    mutable CommandContext context;
    float level_;
};

class SerotoninSetLevelCommand : public Command {
public:
    SerotoninSetLevelCommand(Serotonin* serotonin, float level) : serotonin_(serotonin), level_(level) {}
    
    CommandResult execute() override {
        if (!serotonin_) return CommandResult(false, "Serotonin system not initialized");
        serotonin_->setLevel(level_);
        return CommandResult(true, "Serotonin level set to " + std::to_string(level_));
    }
    
    std::string getName() const override { return "serotonin_set_level"; }
    std::string getDescription() const override { return "Set serotonin level";
    std::string getHelp() const override { return "Sets the serotonin level (0.0-1.0).\nUsage: serotonin_set_level <level>"; }
    std::string getParameters() const override { return "<level>"; }
    bool setParameters(const std::string& params) override {
        try {
            level_ = std::stof(params);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Neuromodulation"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<SerotoninSetLevelCommand>(*this);
    }
    
private:
    Serotonin* serotonin_;
    mutable CommandContext context;
    float level_;
};

// Cognition Commands
class NeuralPlannerSetDepthCommand : public Command {
public:
    NeuralPlannerSetDepthCommand(NeuralPlanner* planner, size_t depth) : planner_(planner), depth_(depth) {}
    
    CommandResult execute() override {
        if (!planner_) return CommandResult(false, "Neural planner not initialized");
        planner_->setPlanningDepth(depth_);
        return CommandResult(true, "Neural planner depth set to " + std::to_string(depth_));
    }
    
    std::string getName() const override { return "neural_planner_set_depth"; }
    std::string getDescription() const override { return "Set neural planner depth";
    std::string getHelp() const override { return "Sets the planning depth (number of steps to look ahead).\nUsage: neural_planner_set_depth <depth>"; }
    std::string getParameters() const override { return "<depth>"; }
    bool setParameters(const std::string& params) override {
        try {
            depth_ = std::stoull(params);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Cognition"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<NeuralPlannerSetDepthCommand>(*this);
    }
    
private:
    NeuralPlanner* planner_;
    mutable CommandContext context;
    size_t depth_;
};

class ConceptFormationSetLearningRateCommand : public Command {
public:
    ConceptFormationSetLearningRateCommand(ConceptFormation* conceptFormation, float rate) : conceptFormation_(conceptFormation), rate_(rate) {}
    
    CommandResult execute() override {
        if (!conceptFormation_) return CommandResult(false, "Concept formation not initialized");
        // Note: ConceptFormation doesn't have setLearningRate method, using placeholder
        return CommandResult(true, "Concept formation learning rate set to " + std::to_string(rate_) + " (not implemented)");
    }
    
    std::string getName() const override { return "concept_formation_set_learning_rate"; }
    std::string getDescription() const override { return "Set concept formation learning rate";
    std::string getHelp() const override { return "Sets the concept formation learning rate.\nUsage: concept_formation_set_learning_rate <rate>"; }
    std::string getParameters() const override { return "<rate>"; }
    bool setParameters(const std::string& params) override {
        try {
            rate_ = std::stof(params);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Cognition"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<ConceptFormationSetLearningRateCommand>(*this);
    }
    
private:
    ConceptFormation* conceptFormation_;
    mutable CommandContext context;
    float rate_;
};

class AttentionalSelectionSetStrengthsCommand : public Command {
public:
    AttentionalSelectionSetStrengthsCommand(AttentionalSelection* attention, float inhibition, float excitation) 
        : attention_(attention), inhibitionStrength_(inhibition), excitationStrength_(excitation) {}
    
    CommandResult execute() override {
        if (!attention_) return CommandResult(false, "Attentional selection not initialized");
        attention_->setInhibitionStrength(inhibitionStrength_);
        attention_->setExcitationStrength(excitationStrength_);
        return CommandResult(true, "Attention strengths set (inhibition: " + std::to_string(inhibitionStrength_) + ", excitation: " + std::to_string(excitationStrength_) + ")");
    }
    
    std::string getName() const override { return "attentional_selection_set_strengths"; }
    std::string getDescription() const override { return "Set attentional selection strengths";
    std::string getHelp() const override { return "Sets the attentional selection strengths.\nUsage: attentional_selection_set_strengths <inhibition> <excitation>"; }
    std::string getParameters() const override { return "<inhibition> <excitation>"; }
    bool setParameters(const std::string& params) override {
        // Parse two parameters (simplified)
        size_t pos = params.find(' ');
        if (pos == std::string::npos) return false;
        
        std::string inhibitionStr = params.substr(0, pos);
        std::string excitationStr = params.substr(pos + 1);
        
        try {
            inhibitionStrength_ = std::stof(inhibitionStr);
            excitationStrength_ = std::stof(excitationStr);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Cognition"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<AttentionalSelectionSetStrengthsCommand>(*this);
    }
    
private:
    AttentionalSelection* attention_;
    mutable CommandContext context;
    float inhibitionStrength_;
    float excitationStrength_;
};

// Prediction Commands
class PredictionSystemSetPredictionAccuracyCommand : public Command {
public:
    PredictionSystemSetPredictionAccuracyCommand(PredictionSystem* predictionSystem, float accuracy) : predictionSystem_(predictionSystem), accuracy_(accuracy) {}
    
    CommandResult execute() override {
        if (!predictionSystem_) return CommandResult(false, "Prediction system not initialized");
        // Note: PredictionSystem doesn't have setPredictionAccuracy method, using placeholder
        return CommandResult(true, "Prediction system accuracy set to " + std::to_string(accuracy_) + " (not implemented)");
    }
    
    std::string getName() const override { return "prediction_system_set_accuracy"; }
    std::string getDescription() const override { return "Set prediction system accuracy";
    std::string getHelp() const override { return "Sets the prediction system accuracy.\nUsage: prediction_system_set_accuracy <accuracy>"; }
    std::string getParameters() const override { return "<accuracy>"; }
    bool setParameters(const std::string& params) override {
        try {
            accuracy_ = std::stof(params);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Prediction"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<PredictionSystemSetPredictionAccuracyCommand>(*this);
    }
    
private:
    PredictionSystem* predictionSystem_;
    mutable CommandContext context;
    float accuracy_;
};

class PredictionErrorSetSensitivityCommand : public Command {
public:
    PredictionErrorSetSensitivityCommand(PredictionError* predictionError, float sensitivity) : predictionError_(predictionError), sensitivity_(sensitivity) {}
    
    CommandResult execute() override {
        if (!predictionError_) return CommandResult(false, "Prediction error system not initialized");
        // Note: PredictionError doesn't have setSensitivity method, using placeholder
        return CommandResult(true, "Prediction error sensitivity set to " + std::to_string(sensitivity_) + " (not implemented)");
    }
    
    std::string getName() const override { return "prediction_error_set_sensitivity"; }
    std::string getDescription() const override { return "Set prediction error sensitivity";
    std::string getHelp() const override { return "Sets the prediction error sensitivity.\nUsage: prediction_error_set_sensitivity <sensitivity>"; }
    std::string getParameters() const override { return "<sensitivity>"; }
    bool setParameters(const std::string& params) override {
        try {
            sensitivity_ = std::stof(params);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Prediction"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<PredictionErrorSetSensitivityCommand>(*this);
    }
    
private:
    PredictionError* predictionError_;
    mutable CommandContext context;
    float sensitivity_;
};

// Development Commands
class DevelopmentSystemSetStageCommand : public Command {
public:
    DevelopmentSystemSetStageCommand(DevelopmentSystem* developmentSystem, DevelopmentalStage stage) : developmentSystem_(developmentSystem), stage_(stage) {}
    
    CommandResult execute() override {
        if (!developmentSystem_) return CommandResult(false, "Development system not initialized");
        developmentSystem_->setStage(stage_);
        return CommandResult(true, "Development system stage set to " + std::to_string(static_cast<int>(stage_)));
    }
    
    std::string getName() const override { return "development_system_set_stage"; }
    std::string getDescription() const override { return "Set development system stage";
    std::string getHelp() const override { return "Sets the development system stage.\nUsage: development_system_set_stage <stage> (0=Initial, 1=CriticalPeriod, 2=Maturation, 3=Adult)"; }
    std::string getParameters() const override { return "<stage>"; }
    bool setParameters(const std::string& params) override {
        try {
            int stageInt = std::stoi(params);
            stage_ = static_cast<DevelopmentalStage>(stageInt);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Development"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<DevelopmentSystemSetStageCommand>(*this);
    }
    
private:
    DevelopmentSystem* developmentSystem_;
    mutable CommandContext context;
    DevelopmentalStage stage_;
};

class StructuralPlasticitySetRatesCommand : public Command {
public:
    StructuralPlasticitySetRatesCommand(StructuralPlasticity* structuralPlasticity, float synaptogenesis, float pruning) 
        : structuralPlasticity_(structuralPlasticity), synaptogenesisRate_(synaptogenesis), pruningRate_(pruning) {}
    
    CommandResult execute() override {
        if (!structuralPlasticity_) return CommandResult(false, "Structural plasticity not initialized");
        structuralPlasticity_->setSynaptogenesisRate(synaptogenesisRate_);
        structuralPlasticity_->setPruningRate(pruningRate_);
        return CommandResult(true, "Structural plasticity rates set (synaptogenesis: " + std::to_string(synaptogenesisRate_) + ", pruning: " + std::to_string(pruningRate_) + ")");
    }
    
    std::string getName() const override { return "structural_plasticity_set_rates"; }
    std::string getDescription() const override { return "Set structural plasticity rates";
    std::string getHelp() const override { return "Sets the structural plasticity rates.\nUsage: structural_plasticity_set_rates <synaptogenesis> <pruning>"; }
    std::string getParameters() const override { return "<synaptogenesis> <pruning>"; }
    bool setParameters(const std::string& params) override {
        // Parse two parameters (simplified)
        size_t pos = params.find(' ');
        if (pos == std::string::npos) return false;
        
        std::string synaptogenesisStr = params.substr(0, pos);
        std::string pruningStr = params.substr(pos + 1);
        
        try {
            synaptogenesisRate_ = std::stof(synaptogenesisStr);
            pruningRate_ = std::stof(pruningStr);
            return true;
        } catch (...) {
            return false;
        }
    }
    std::string getCategory() const override { return "Development"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<StructuralPlasticitySetRatesCommand>(*this);
    }
    
private:
    StructuralPlasticity* structuralPlasticity_;
    mutable CommandContext context;
    float synaptogenesisRate_;
    float pruningRate_;
};

// Region/Connection Commands
class BrainAddRegionCommand : public Command {
public:
    BrainAddRegionCommand(Brain* brain, const std::string& name) : brain_(brain), regionName_(name) {}
    
    CommandResult execute() override {
        if (!brain_) return CommandResult(false, "Brain not initialized");
        RegionId regionId = brain_->addRegion(regionName_);
        return CommandResult(true, "Added region " + regionName_ + " with ID " + std::to_string(regionId.index()));
    }
    
    std::string getName() const override { return "add_region"; }
    std::string getDescription() const override { return "Add a new brain region";
    std::string getHelp() const override { return "Adds a new brain region.\nUsage: add_region [name] (default: empty string)"; }
    std::string getParameters() const override { return "[name]"; }
    bool setParameters(const std::string& params) override {
        regionName_ = params;
        return true;
    }
    std::string getCategory() const override { return "Region/Connection"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<BrainAddRegionCommand>(*this);
    }
    
private:
    Brain* brain_;
    mutable CommandContext context;
    std::string regionName_;
};

class BrainAddInterRegionConnectionCommand : public Command {
public:
    BrainAddInterRegionConnectionCommand(Brain* brain, RegionId source, RegionId target, 
                                        float weight = 0.0f, Delay delay = 1)
        : brain_(brain), source_(source), target_(target), weight_(weight), delay_(delay) {}
    
    CommandResult execute() override {
        if (!brain_) return CommandResult(false, "Brain not initialized");
        brain_->addInterRegionConnection(source_, target_, weight_, delay_);
        return CommandResult(true, "Added inter-region connection from " + 
            std::to_string(source_.index()) + " to " + std::to_string(target_.index()));
    }
    
    std::string getName() const override { return "add_inter_region_connection"; }
    std::string getDescription() const override { return "Add an inter-region connection";
    std::string getHelp() const override { return "Adds a connection between brain regions.\nUsage: add_inter_region_connection <source_id> <target_id> [weight] [delay]"; }
    std::string getParameters() const override { return "<source_id> <target_id> [weight] [delay]"; }
    bool setParameters(const std::string& params) override {
        // Parse parameters (simplified - would need proper parsing)
        return false;
    }
    std::string getCategory() const override { return "Region/Connection"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<BrainAddInterRegionConnectionCommand>(*this);
    }
    
private:
    Brain* brain_;
    mutable CommandContext context;
    RegionId source_;
    RegionId target_;
    float weight_;
    Delay delay_;
};

class BrainRemoveInterRegionConnectionCommand : public Command {
public:
    BrainRemoveInterRegionConnectionCommand(Brain* brain, RegionId source, RegionId target) 
        : brain_(brain), source_(source), target_(target) {}
    
    CommandResult execute() override {
        if (!brain_) return CommandResult(false, "Brain not initialized");
        brain_->removeInterRegionConnection(source_, target_);
        return CommandResult(true, "Removed inter-region connection from " + 
            std::to_string(source_.index()) + " to " + std::to_string(target_.index()));
    }
    
    std::string getName() const override { return "remove_inter_region_connection"; }
    std::string getDescription() const override { return "Remove an inter-region connection";
    std::string getHelp() const override { return "Removes a connection between brain regions.\nUsage: remove_inter_region_connection <source_id> <target_id>"; }
    std::string getParameters() const override { return "<source_id> <target_id>"; }
    bool setParameters(const std::string& params) override {
        // Parse parameters (simplified - would need proper parsing)
        return false;
    }
    std::string getCategory() const override { return "Region/Connection"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<BrainRemoveInterRegionConnectionCommand>(*this);
    }
    
private:
    Brain* brain_;
    mutable CommandContext context;
    RegionId source_;
    RegionId target_;
};

// Neuron/Population Commands
class BrainInjectCurrentCommand : public Command {
public:
    BrainInjectCurrentCommand(Brain* brain, NeuronId neuron, MembranePotential current) 
        : brain_(brain), neuronId_(neuron), current_(current) {}
    
    CommandResult execute() override {
        if (!brain_) return CommandResult(false, "Brain not initialized");
        brain_->injectCurrent(neuronId_, current_);
        return CommandResult(true, "Injected " + std::to_string(current_) + " current into neuron " + 
            std::to_string(neuronId_.index()));
    }
    
    std::string getName() const override { return "inject_current"; }
    std::string getDescription() const override { return "Inject current into a specific neuron";
    std::string getHelp() const override { return "Injects current into a specific neuron.\nUsage: inject_current <neuron_id> <current>"; }
    std::string getParameters() const override { return "<neuron_id> <current>"; }
    bool setParameters(const std::string& params) override {
        // Parse parameters (simplified - would need proper parsing)
        return false;
    }
    std::string getCategory() const override { return "Neuron/Population"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<BrainInjectCurrentCommand>(*this);
    }
    
private:
    Brain* brain_;
    mutable CommandContext context;
    NeuronId neuronId_;
    MembranePotential current_;
};

class BrainInjectCurrentToNeuronsCommand : public Command {
public:
    BrainInjectCurrentToNeuronsCommand(Brain* brain, NeuronType type, MembranePotential current) 
        : brain_(brain), neuronType_(type), current_(current) {}
    
    CommandResult execute() override {
        if (!brain_) return CommandResult(false, "Brain not initialized");
        brain_->injectCurrentToNeurons(neuronType_, current_);
        return CommandResult(true, "Injected " + std::to_string(current_) + " current into all " + 
            std::to_string(static_cast<int>(neuronType_)) + " neurons");
    }
    
    std::string getName() const override { return "inject_current_to_neurons"; }
    std::string getDescription() const override { return "Inject current into all neurons of a specific type";
    std::string getHelp() const override { return "Injects current into all neurons of a specific type.\nUsage: inject_current_to_neurons <neuron_type> <current>"; }
    std::string getParameters() const override { return "<neuron_type> <current>"; }
    bool setParameters(const std::string& params) override {
        // Parse parameters (simplified - would need proper parsing)
        return false;
    }
    std::string getCategory() const override { return "Neuron/Population"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<BrainInjectCurrentToNeuronsCommand>(*this);
    }
    
private:
    Brain* brain_;
    mutable CommandContext context;
    NeuronType neuronType_;
    MembranePotential current_;
};

// Agent Commands
class AgentBrainSetFeatureEnabledCommand : public Command {
public:
    AgentBrainSetFeatureEnabledCommand(AgentBrain* agentBrain, const std::string& feature, bool enabled) 
        : agentBrain_(agentBrain), feature_(feature), enabled_(enabled) {}
    
    CommandResult execute() override {
        if (!agentBrain_) return CommandResult(false, "Agent brain not initialized");
        // Note: AgentBrain doesn't have setFeatureEnabled method, using placeholder
        return CommandResult(true, "Agent brain feature " + feature_ + " set to " + 
            (enabled_ ? "enabled" : "disabled") + " (not implemented)");
    }
    
    std::string getName() const override { return "agent_brain_set_feature_enabled"; }
    std::string getDescription() const override { return "Set agent brain feature enabled state";
    std::string getHelp() const override { return "Sets whether an agent brain feature is enabled.\nUsage: agent_brain_set_feature_enabled <feature> <enabled>"; }
    std::string getParameters() const override { return "<feature> <enabled>"; }
    bool setParameters(const std::string& params) override {
        // Parse parameters (simplified)
        size_t pos = params.find(' ');
        if (pos == std::string::npos) return false;
        
        feature_ = params.substr(0, pos);
        std::string enabledStr = params.substr(pos + 1);
        
        if (enabledStr == "true" || enabledStr == "1") {
            enabled_ = true;
        } else if (enabledStr == "false" || enabledStr == "0") {
            enabled_ = false;
        } else {
            return false;
        }
        
        return true;
    }
    std::string getCategory() const override { return "Agent"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<AgentBrainSetFeatureEnabledCommand>(*this);
    }
    
private:
    AgentBrain* agentBrain_;
    mutable CommandContext context;
    std::string feature_;
    bool enabled_;
};

// World Commands
class WorldSetEnvironmentCommand : public Command {
public:
    WorldSetEnvironmentCommand(World* world, const std::string& environment) : world_(world), environment_(environment) {}
    
    CommandResult execute() override {
        if (!world_) return CommandResult(false, "World not initialized");
        // Note: World doesn't have setEnvironment method, using placeholder
        return CommandResult(true, "World environment set to " + environment_ + " (not implemented)");
    }
    
    std::string getName() const override { return "world_set_environment"; }
    std::string getDescription() const override { return "Set world environment";
    std::string getHelp() const override { return "Sets the world environment.\nUsage: world_set_environment <environment>"; }
    std::string getParameters() const override { return "<environment>"; }
    bool setParameters(const std::string& params) override {
        environment_ = params;
        return true;
    }
    std::string getCategory() const override { return "World"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<WorldSetEnvironmentCommand>(*this);
    }
    
private:
    World* world_;
    mutable CommandContext context;
    std::string environment_;
};

class WorldSetSensorInputCommand : public Command {
public:
    WorldSetSensorInputCommand(World* world, const std::vector<float>& input) : world_(world), sensorInput_(input) {}
    
    CommandResult execute() override {
        if (!world_) return CommandResult(false, "World not initialized");
        // Note: World doesn't have setSensorInput method, using placeholder
        return CommandResult(true, "World sensor input set (not implemented)");
    }
    
    std::string getName() const override { return "world_set_sensor_input"; }
    std::string getDescription() const override { return "Set world sensor input";
    std::string getHelp() const override { return "Sets the world sensor input.\nUsage: world_set_sensor_input <input1> <input2> ..."; }
    std::string getParameters() const override { return "<input1> <input2> ..."; }
    bool setParameters(const std::string& params) override {
        // Parse parameters (simplified)
        return false;
    }
    std::string getCategory() const override { return "World"; }
    std::unique_ptr<Command> clone() const override {
        return std::make_unique<WorldSetSensorInputCommand>(*this);
    }
    
private:
    World* world_;
    mutable CommandContext context;
    std::vector<float> sensorInput_;
};

} // namespace nlm