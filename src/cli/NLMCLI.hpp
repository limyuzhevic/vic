#pragma once

#include <string>
#include <memory>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/ExperimentRunner.hpp"

namespace nlm {

class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();

    // Parse command line arguments
    bool parseCommandLine(int argc, char** argv);

    // Handle specific commands
    bool handleRunCommand(int argc, char** argv);
    bool handleConfigCommand(int argc, char** argv);
    bool handleMonitorCommand(int argc, char** argv);
    bool handleProfileCommand(int argc, char** argv);
    bool handleCheckpointCommand(int argc, char** argv);
    bool handleDebugCommand(int argc, char** argv);

    // Print help messages
    void printHelp();
    void printRunHelp();
    void printConfigHelp();
    void printMonitorHelp();
    void printProfileHelp();
    void printCheckpointHelp();
    void printDebugHelp();

    // Run a simulation
    bool runSimulation(const std::string& configFile, bool enablePerformance,
                      bool verbose, bool enableMonitoring,
                      bool runPhase1, bool runPhase2, bool runPhase3,
                      bool runPhase4, bool runPhase5, bool runPhase6,
                      bool useOptimizedBrain, bool enableMultithreading,
                      bool enableSIMD, bool enableEventDriven,
                      bool enableSparseConnectivity, int maxSteps);

    // Initialize performance optimizations
    void initializePerformanceOptimizations(bool enableMultithreading,
                                            bool enableSIMD,
                                            bool enableEventDriven,
                                            bool enableSparseConnectivity);

private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<ExperimentRunner> experimentRunner_;
    bool performanceInitialized_;
};

} // namespace nlm