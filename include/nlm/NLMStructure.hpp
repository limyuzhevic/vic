// NLM (熙然) - Neural Learning Machine
// Improved hierarchical namespace structure
// 
// This file demonstrates the new hierarchical namespace organization
// that reduces complexity and improves code organization.

#pragma once

// Top-level NLM namespace
namespace nlm {

// Core computational namespace - contains fundamental neural components
namespace core {
    // Neural processing components
    namespace brain {
        class Neuron;
        class Synapse;
        class NeuralRegion;
        class NeuralPopulation;
        class Brain;
    }
    
    // Dynamics and computation
    namespace dynamics {
        class SpikeSystem;
        class NeuralDynamics;
    }
    
    // Learning and adaptation
    namespace plasticity {
        class PlasticityRule;
        class STDP;
        class Hebbian;
        class StructuralPlasticity;
    }
    
    // Development
    namespace development {
        class DevelopmentSystem;
        class Synaptogenesis;
        class Pruning;
        class Maturation;
    }
    
    // Memory systems
    namespace memory {
        class Memory;
        class WorkingMemory;
        class EpisodicMemory;
        class SemanticMemory;
        class ProceduralMemory;
        class AssociativeMemory;
        class NeuralWorkingMemory;
        class NeuralEpisodicMemory;
    }
    
    // Neuromodulation
    namespace neuromodulation {
        class Neuromodulator;
        class Dopamine;
        class Curiosity;
        class Novelty;
        class PredictionError;
    }
    
    // Cognition
    namespace cognition {
        class NeuralPlanner;
        class ConceptFormation;
        class AttentionalSelection;
    }
    
    // Prediction
    namespace prediction {
        class PredictionSystem;
        class NeuralPrediction;
    }
    
    // Sensory and motor
    namespace sensory {
        class SensoryInput;
        class Vision;
        class Audio;
        class InternalSignals;
    }
    
    namespace motor {
        class Action;
        class MotorSystem;
    }
    
    // Environment
    namespace environment {
        class Environment;
        class Observation;
        class Action;
    }
    
    // Core utilities
    namespace core {
        class Config;
        class RandomGenerator;
        class SimulationClock;
        class Logger;
        class Constants;
        class ErrorHandling;
    }
}

// Integration namespace - higher-level systems
namespace integration {
    // Neural agent systems
    namespace agent {
        class AgentBrain;
        class SensoryPercept;
        class AgentBody;
    }
    
    // World systems
    namespace world {
        class SimpleWorld;
        class Observation;
    }
    
    // Performance and optimization
    namespace performance {
        class MemoryPool;
        class EventQueue;
        class CheckpointSystem;
        class SIMDVectorization;
        class SparseConnectivity;
        class OptimizedBrain;
        class ParallelProcessing;
    }
    
    // Experiment systems
    namespace experiments {
        class Experiment;
        class ExperimentRunner;
        class Metrics;
        class AblationSystem;
        class Phase3Experiment;
        class Phase4Experiment;
        class Phase5Experiment;
        class Phase6IntegratedExperiment;
    }
    
    // Visualization
    namespace visualization {
        class VisualizationInterface;
    }
}

// Convenience namespace for common operations
namespace api {
    // Simplified API for common use cases
    namespace basic {
        // Basic neural network operations
        class BasicNetwork;
        class BasicLearning;
    }
    
    // Research API for advanced use cases
    namespace research {
        // Research-grade functionality
        class ResearchBrain;
        class ResearchExperiment;
    }
}

} // namespace nlm
