#pragma once

#include <string>
#include <vector>
#include "AdvancedCLI.hpp"
#include "CommandOutputFormatter.hpp"

namespace nlm {

class SimulationCommands {
public:
    SimulationCommands(std::shared_ptr<CommandOutputFormatter> formatter,
                       std::shared_ptr<Logger> logger);
    
    // Simulation Control
    bool simulate(int steps);
    bool simulateContinue(int steps);
    bool simulatePause();
    bool simulateResume();
    bool simulateStep();
    bool simulateRewind(int steps);
    bool simulateFastforward(float factor);
    bool simulateLoop(int steps);
    bool simulateProfile();
    bool simulateBenchmark();
    
    // Simulation state management
    bool isSimulationPaused() const { return simulationPaused; }
    bool isSimulationRunning() const { return simulationRunning; }
    int getSimulationStep() const { return currentStep; }
    
private:
    std::shared_ptr<CommandOutputFormatter> formatter;
    std::shared_ptr<Logger> logger;
    
    // Simulation state
    bool simulationRunning = false;
    bool simulationPaused = false;
    int currentStep = 0;
    float simulationSpeed = 1.0f;
    
    // Performance tracking
    double startTime = 0.0;
    double elapsedTime = 0.0;
    double stepsPerSecond = 0.0;
    
    void updateSimulationSpeed(float factor);
    double calculatePerformance(double timeElapsed, int steps);
};
