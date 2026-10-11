#pragma once

#include <string>
#include <vector>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class AdvancedCommandLineParser {
public:
    AdvancedCommandLineParser(std::shared_ptr<Config> config,
                             std::shared_ptr<Brain> brain,
                             std::shared_ptr<Logger> logger);
    
    // Basic command parsing
    bool parseArguments(int argc, char* argv[]);
    
    // Command groups
    bool parseBrainCommands();
    bool parseSimulationCommands();
    bool parseInspectionCommands();
    bool parseAnalysisCommands();
    bool parseDebuggingCommands();
    bool parseExportImportCommands();
    bool parseAdvancedCommands();
    
    // Command execution
    void executeBrainCommands();
    void executeSimulationCommands();
    void executeInspectionCommands();
    void executeAnalysisCommands();
    void executeDebuggingCommands();
    void executeExportImportCommands();
    void executeAdvancedCommands();
    
    // Command definitions
    struct BrainCommands {
        std::string config;
        std::string save;
        std::string load;
        bool info = false;
        bool reset = false;
        std::string dump;
        std::string diff1, diff2;
        bool status = false;
        bool validate = false;
    };
    
    struct SimulationCommands {
        int simulate = 0;
        int simulateContinue = 0;
        bool simulatePause = false;
        bool simulateResume = false;
        bool simulateStep = false;
        int simulateRewind = 0;
        float simulateFastforward = 1.0f;
        int simulateLoop = 0;
        bool simulateProfile = false;
        bool simulateBenchmark = false;
    };
    
    struct InspectionCommands {
        int inspectNeurons = 0;
        uint64_t inspectNeuron = 0;
        int inspectSynapses = 0;
        uint64_t inspectSynapse = 0;
        int inspectRegions = 0;
        uint64_t inspectRegion = 0;
        int inspectPopulations = 0;
        uint64_t inspectPopulation = 0;
        bool inspectMemory = false;
        bool inspectEpisodic = false;
        bool inspectAssociative = false;
        bool inspectNeuromodulation = false;
        bool inspectPrediction = false;
        bool inspectPlanning = false;
        bool inspectAttention = false;
        bool inspectConcept = false;
    };
    
    struct AnalysisCommands {
        bool analyzeNeuralActivity = false;
        bool analyzePlasticity = false;
        bool analyzeMemoryUsage = false;
        bool analyzePerformance = false;
        bool analyzeSTDPTiming = false;
        bool analyzeHebbianCoactivity = false;
        bool analyzeMemoryConsolidation = false;
        bool analyzeNeuromodulation = false;
        bool analyzePredictionErrors = false;
        bool analyzeExplorationExploitation = false;
        bool analyzeDevelopmentalStages = false;
    };
    
    struct DebuggingCommands {
        int debugSpikeEvents = 0;
        uint64_t debugSpikeHistory = 0;
        uint64_t debugPlasticityHistory = 0;
        int debugMemoryTraces = 0;
        uint64_t debugPlasticChange = 0;
        bool debugNeuromodulation = false;
        bool debugSimulationClock = false;
        bool debugCheckpointStatus = false;
        bool debugMemoryConsistency = false;
        bool debugPlasticityConsistency = false;
        bool debugNeuralConsistency = false;
    };
    
    struct ExportImportCommands {
        std::string exportNeuralGraph;
        std::string exportSynapticMatrix;
        std::string exportMemoryTraces;
        std::string exportPlasticityData;
        std::string exportBehaviorLog;
        std::string importNeuralGraph;
        std::string importSynapticMatrix;
        std::string importBehaviorLog;
    };
    
    struct AdvancedCommands {
        std::string experimentCreate;
        std::string experimentRun;
        bool experimentList = false;
        std::string experimentStatus;
        std::string experimentResults;
        std::string experimentExport;
        std::string experimentImport;
        std::string parameterSweep;
        int monteCarlo = 0;
        std::string sensitivityAnalysis;
        bool onlineLearning = false;
        bool batchLearning = false;
        bool curriculumLearning = false;
    };
    
    // Getters
    BrainCommands getBrainCommands() const { return brainCommands; }
    SimulationCommands getSimulationCommands() const { return simulationCommands; }
    InspectionCommands getInspectionCommands() const { return inspectionCommands; }
    AnalysisCommands getAnalysisCommands() const { return analysisCommands; }
    DebuggingCommands getDebuggingCommands() const { return debuggingCommands; }
    ExportImportCommands getExportImportCommands() const { return exportImportCommands; }
    AdvancedCommands getAdvancedCommands() const { return advancedCommands; }
    
private:
    BrainCommands brainCommands;
    SimulationCommands simulationCommands;
    InspectionCommands inspectionCommands;
    AnalysisCommands analysisCommands;
    DebuggingCommands debuggingCommands;
    ExportImportCommands exportImportCommands;
    AdvancedCommands advancedCommands;
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Logger> logger;
};
