// Performance Profiling Integration for Advanced CLI
// Extends the existing AdvancedCLI with comprehensive performance profiling and benchmarking commands

#pragma once

#include <memory>
#include "AdvancedCLI.hpp"
#include "PerformanceProfilerCLI.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

namespace nlm {

/**
 * Advanced CLI with Performance Profiling Integration
 * Combines the existing AdvancedCLI functionality with comprehensive performance profiling tools
 */
class AdvancedCLIWithPerformance : public AdvancedCLI {
private:
    std::shared_ptr<PerformanceProfilerCLI> performance_cli;
    
    // Additional performance-specific commands
    struct PerformanceCommand {
        std::string name;
        std::string description;
        std::function<void()> handler;
        std::string usage;
    };
    
    std::vector<PerformanceCommand> performance_commands;
    
public:
    AdvancedCLIWithPerformance(int argc, char** argv);
    ~AdvancedCLIWithPerformance() = default;
    
    bool execute() override;
    void printHelp() const override;
    void printVersion() const override;
    
    // Performance profiling integration
    void initializePerformanceTools();
    void registerPerformanceCommands();
    
    // Command execution
    bool executePerformanceCommand(const std::string& command_name, const std::vector<std::string>& args);
    
    // Performance-specific command handlers
    void handlePerformanceProfileStart() const;
    void handlePerformanceProfileStop() const;
    void handlePerformanceProfileStatus() const;
    void handlePerformanceLiveMonitor() const;
    void handlePerformanceNeuronProfile() const;
    void handlePerformanceSynapseProfile() const;
    void handlePerformancePlasticityProfile() const;
    void handlePerformanceMemoryProfile() const;
    void handlePerformanceNetworkSizeBenchmark() const;
    void handlePerformanceSimulationSpeedBenchmark() const;
    void handlePerformancePlasticityRuleBenchmark() const;
    void handlePerformanceMemoryEfficiencyBenchmark() const;
    void handlePerformanceGenerateReport() const;
    void handlePerformanceCompareReports() const;
    void handlePerformanceAnalysis() const;
    void handlePerformanceReset() const;
    void handlePerformanceSave() const;
    void handlePerformanceLoad() const;
    
    // Configuration
    void setPerformanceOutputDirectory(const std::string& directory);
    void setPerformanceSamplingInterval(double interval);
    void setPerformanceEnabled(bool enabled);
    
    // Data access
    std::shared_ptr<PerformanceProfilerCLI> getPerformanceCLI() const { return performance_cli; }
    std::shared_ptr<PerformanceProfiler> getProfiler() const {
        return performance_cli ? performance_cli->getProfiler() : nullptr;
    }
};

} // namespace nlm