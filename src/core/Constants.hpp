#pragma once

namespace nlm {

// ================================================
// NAMED CONSTANTS (REMOVED MAGIC NUMBERS)
// ================================================

// Simulation constants
namespace SimConstants {
    constexpr double DefaultTimestep = 0.001;           // seconds
    constexpr size_t DefaultNeuronCount = 500;          // neurons
    constexpr size_t DefaultRegionCount = 1;            // brain regions
    constexpr float DefaultConnectionProbability = 0.15f;
    constexpr size_t DefaultMaxSteps = 1000;            // steps per experiment
    constexpr size_t DefaultConnectivityTestSteps = 50; // steps for connectivity test
    constexpr double DefaultExperimentDuration = 60.0;  // seconds
}

// Plasticity constants
namespace PlasticityConstants {
    constexpr float DefaultSTDP_LTPWeight = 0.02f;       // Long-term potentiation
    constexpr float DefaultSTDP_LTDWeight = 0.015f;      // Long-term depression
    constexpr float DefaultSTDPTau = 20.0f;              // STDP time constant
    constexpr float DefaultSynaptogenesisRate = 0.0001f;
    constexpr float DefaultPruningRate = 0.00001f;
    constexpr float DefaultWeightChangeThreshold = 0.01f;
    constexpr float DefaultLearningThreshold = 0.001f;
}

// Neuromodulation constants
namespace NeuromodulationConstants {
    constexpr float DefaultDopamineRange = 1.0f;         // +/- 1.0
    constexpr float DefaultCuriosityThreshold = 0.3f;
    constexpr float DefaultExplorationChance = 0.3f;    // Max exploration probability
    constexpr float DefaultPredictionErrorThreshold = 0.001f;
}

// Development constants
namespace DevelopmentConstants {
    constexpr double InitialStageDuration = 60.0;        // seconds
    constexpr double CriticalPeriodDuration = 240.0;     // seconds
    constexpr double MaturationStageDuration = 540.0;    // seconds
    constexpr float InitialPlasticityModifier = 1.0f;
    constexpr float AdultPlasticityModifier = 0.2f;
    constexpr float CriticalPeriodPlasticityModifier = 0.8f;
    constexpr float MaturationPlasticityModifier = 0.5f;
}

// Sensory/Motor constants
namespace SensoryMotorConstants {
    // Vision: 16x16 grid
    constexpr size_t VisionWidth = 16;
    constexpr size_t VisionHeight = 16;
    constexpr size_t VisionNeuronCount = VisionWidth * VisionHeight;
    
    // Touch input
    constexpr size_t TouchNeuronCount = 8;
    
    // Internal state
    constexpr size_t InternalNeuronCount = 4;
    
    // Proprioception
    constexpr size_t ProprioceptionNeuronCount = 6;
    
    // Motor actions
    constexpr size_t MotorActionCount = 6;
    
    // Scaling factors
    constexpr float VisionCurrentScale = 5.0f;
    constexpr float TouchCurrentScale = 8.0f;
    constexpr float InternalCurrentScale = 5.0f;
    constexpr float ProprioceptionCurrentScale = 3.0f;
    
    // Novelty detection
    constexpr float NoveltyDecayRate = 0.99f;
}

// AgentBrain constants
namespace AgentBrainConstants {
    constexpr size_t MaxMostActiveNeurons = 10;
    constexpr float MotorActivityThreshold = 0.5f;
    constexpr float CuriosityLevelThreshold = 0.3f;
    constexpr float HighCuriosityLevel = 0.5f;
    constexpr size_t RandomMotorCommandChoices = 7;
    constexpr float ExpectedRewardSmoothing = 0.95f;
    constexpr float EligibilityTraceDecay = 0.1f;
}

// Error handling constants
namespace ErrorConstants {
    constexpr int SuccessCode = 0;
    constexpr int ErrorCode = 1;
    constexpr int ConfigLoadErrorCode = 2;
    constexpr int BrainInitErrorCode = 3;
}

// Logging constants
namespace LoggingConstants {
    constexpr const char* DefaultLogFile = "nlm_simulation.log";
    constexpr size_t MaxLogHistory = 1000;
    constexpr const char* DefaultLogFormat = "%Y-%m-%d %H:%M:%S.%F [%l] %m";
}

// Command-line constants
namespace CLIConstants {
    constexpr const char* ProgramName = "nlm_simulation";
    constexpr const char* Version = "Phase 2.0";
    constexpr const char* Description = "NLM - Neural Learning Machine Phase 2: Real Neural Computation";
    constexpr const char* HelpFlag = "--help";
    constexpr const char* VersionFlag = "--version";
    constexpr const char* ConfigFlag = "--config";
    constexpr const char* NeuronCountFlag = "--neurons";
    constexpr const char* TimestepFlag = "--timestep";
    constexpr const char* SeedFlag = "--seed";
    constexpr const char* ExperimentalModeFlag = "--experimental";
    constexpr const char* NoTestsFlag = "--no-tests";
    constexpr const char* ConfigFileDefault = "configs/default.cfg";
}

// Experimental modes
namespace ExperimentalModes {
    constexpr const char* ConnectivityTest = "connectivity";
    constexpr const char* PlasticityExperiment = "plasticity";
    constexpr const char* STDPVerification = "stdp";
    constexpr const char* AllTests = "all";
    constexpr const char* Quick = "quick";
}

} // namespace nlm