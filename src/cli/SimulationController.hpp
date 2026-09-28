// src/cli/SimulationController.hpp
#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class Brain;

/**
 * Advanced simulation controller for NLM
 * 
 * Provides comprehensive simulation control including pause/resume,
 * step-through mode, speed control, and recording capabilities.
 */
class SimulationController {
public:
    SimulationController();
    ~SimulationController();
    
    /**
     * Start simulation with given timestep
     * 
     * @param timestep Simulation timestep
     * @param brain Brain to simulate
     * @return true if simulation started successfully
     */
    bool startSimulation(double timestep, std::shared_ptr<Brain> brain);
    
    /**
     * Pause simulation
     * 
     * @return true if simulation paused successfully
     */
    bool pauseSimulation();
    
    /**
     * Resume simulation
     * 
     * @return true if simulation resumed successfully
     */
    bool resumeSimulation();
    
    /**
     * Step simulation forward by one timestep
     * 
     * @return true if simulation stepped successfully
     */
    bool stepSimulation();
    
    /**
     * Set simulation speed multiplier
     * 
     * @param speed Speed multiplier (1.0 = normal, 2.0 = 2x, 0.5 = 0.5x, etc.)
     * @return true if speed set successfully
     */
    bool setSimulationSpeed(double speed);
    
    /**
     * Get current simulation speed
     * 
     * @return Current speed multiplier
     */
    double getSimulationSpeed() const;
    
    /**
     * Check if simulation is running
     * 
     * @return true if simulation is running
     */
    bool isSimulationRunning() const;
    
    /**
     * Check if simulation is paused
     * 
     * @return true if simulation is paused
     */
    bool isSimulationPaused() const;
    
    /**
     * Get current simulation step
     * 
     * @return Current simulation step
     */
    SimulationStep getCurrentStep() const;
    
    /**
     * Get current simulation time
     * 
     * @return Current simulation time in seconds
     */
    Timestamp getCurrentTime() const;
    
    /**
     * Start recording simulation
     * 
     * @param outputPath Path to output file for recording
     * @param format Recording format ("text", "json", "csv")
     * @return true if recording started successfully
     */
    bool startRecording(const std::string& outputPath, const std::string& format = "text");
    
    /**
     * Stop recording simulation
     * 
     * @return true if recording stopped successfully
     */
    bool stopRecording();
    
    /**
     * Get recorded simulation data
     * 
     * @return Vector of simulation data frames
     */
    std::vector<std::string> getRecordedData() const;
    
    /**
     * Clear recorded simulation data
     */
    void clearRecordedData();
    
    /**
     * Set callback for simulation events
     * 
     * @param eventType Type of event ("step", "pause", "resume", "complete")
     * @param callback Callback function
     */
    void setEventCallback(const std::string& eventType, 
                         const std::function<void()>& callback);
    
    /**
     * Get simulation statistics
     * 
     * @return Human-readable simulation statistics
     */
    std::string getSimulationStatistics() const;
    
    /**
     * Reset simulation to initial state
     */
    void resetSimulation();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

/**
 * Advanced simulation controller for NLM
 * 
 * Provides comprehensive simulation control including pause/resume,
 * step-through mode, speed control, and recording capabilities.
 */
class SimulationController {
public:
    SimulationController();
    ~SimulationController();
    
    /**
     * Start simulation with given timestep
     * 
     * @param timestep Simulation timestep
     * @param brain Brain to simulate
     * @return true if simulation started successfully
     */
    bool startSimulation(double timestep, std::shared_ptr<Brain> brain);
    
    /**
     * Pause simulation
     * 
     * @return true if simulation paused successfully
     */
    bool pauseSimulation();
    
    /**
     * Resume simulation
     * 
     * @return true if simulation resumed successfully
     */
    bool resumeSimulation();
    
    /**
     * Step simulation forward by one timestep
     * 
     * @return true if simulation stepped successfully
     */
    bool stepSimulation();
    
    /**
     * Set simulation speed multiplier
     * 
     * @param speed Speed multiplier (1.0 = normal, 2.0 = 2x, 0.5 = 0.5x, etc.)
     * @return true if speed set successfully
     */
    bool setSimulationSpeed(double speed);
    
    /**
     * Get current simulation speed
     * 
     * @return Current speed multiplier
     */
    double getSimulationSpeed() const;
    
    /**
     * Check if simulation is running
     * 
     * @return true if simulation is running
     */
    bool isSimulationRunning() const;
    
    /**
     * Check if simulation is paused
     * 
     * @return true if simulation is paused
     */
    bool isSimulationPaused() const;
    
    /**
     * Get current simulation step
     * 
     * @return Current simulation step
     */
    SimulationStep getCurrentStep() const;
    
    /**
     * Get current simulation time
     * 
     * @return Current simulation time in seconds
     */
    Timestamp getCurrentTime() const;
    
    /**
     * Start recording simulation
     * 
     * @param outputPath Path to output file for recording
     * @param format Recording format ("text", "json", "csv")
     * @return true if recording started successfully
     */
    bool startRecording(const std::string& outputPath, const std::string& format = "text");
    
    /**
     * Stop recording simulation
     * 
     * @return true if recording stopped successfully
     */
    bool stopRecording();
    
    /**
     * Get recorded simulation data
     * 
     * @return Vector of simulation data frames
     */
    std::vector<std::string> getRecordedData() const;
    
    /**
     * Clear recorded simulation data
     */
    void clearRecordedData();
    
    /**
     * Set callback for simulation events
     * 
     * @param eventType Type of event ("step", "pause", "resume", "complete")
     * @param callback Callback function
     */
    void setEventCallback(const std::string& eventType, 
                         const std::function<void()>& callback);
    
    /**
     * Get simulation statistics
     * 
     * @return Human-readable simulation statistics
     */
    std::string getSimulationStatistics() const;
    
    /**
     * Reset simulation to initial state
     */
    void resetSimulation();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm