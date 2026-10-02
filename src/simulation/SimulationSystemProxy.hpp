#pragma once

#include "ISimulationSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

namespace nlm {

// SimulationSystemProxy: Manages simulation systems dynamically at runtime
// Provides centralized control for system lifecycle, dependency injection, and state management
class SimulationSystemProxy {
public:
    SimulationSystemProxy();
    ~SimulationSystemProxy();
    
    // Disable copying and moving
    SimulationSystemProxy(const SimulationSystemProxy&) = delete;
    SimulationSystemProxy& operator=(const SimulationSystemProxy&) = delete;
    SimulationSystemProxy(SimulationSystemProxy&&) = delete;
    SimulationSystemProxy& operator=(SimulationSystemProxy&&) = delete;
    
    // Add a system instance to the proxy
    // Parameter: system - Unique pointer to the simulation system to manage
    // Returns: true if system was added successfully, false if duplicate
    bool AddSystem(std::unique_ptr<ISimulationSystem> system);
    
    // Remove a system by name
    // Parameter: name - Name of the system to remove
    // Returns: true if system was found and removed, false otherwise
    bool RemoveSystem(const std::string& name);
    
    // Get system by name
    // Parameter: name - Name of the system to retrieve
    // Returns: Pointer to the system if found, nullptr if not found
    ISimulationSystem* GetSystem(const std::string& name);
    
    const ISimulationSystem* GetSystem(const std::string& name) const;
    
    // Get all managed systems
    // Returns: Vector of pointers to all systems
    std::vector<ISimulationSystem*> GetAllSystems() const;
    
    // Initialize all systems with brain reference
    // Parameter: brain - Pointer to the brain for dependency injection
    void InitializeAll(Brain* brain);
    
    // Update all enabled systems
    // Parameter: timestep - Time elapsed since last update (seconds)
    void UpdateAll(double timestep);
    
    // Reset all systems
    void ResetAll();
    
    // Enable/disable a system by name
    // Parameter: name - Name of the system to toggle
    // Parameter: enabled - true to enable, false to disable
    // Returns: true if system was found, false otherwise
    bool SetEnabled(const std::string& name, bool enabled);
    
    // Toggle system enabled state
    // Parameter: name - Name of the system to toggle
    // Returns: New enabled state if system was found, false otherwise
    bool ToggleEnabled(const std::string& name);
    
    // Check if system is enabled
    // Parameter: name - Name of the system to check
    // Returns: true if system exists and is enabled, false otherwise
    bool IsEnabled(const std::string& name) const;
    
    // Check if system exists
    // Parameter: name - Name of the system to check
    // Returns: true if system exists, false otherwise
    bool HasSystem(const std::string& name) const;
    
    // Get system status information
    // Parameter: name - Name of the system to get status for
    // Returns: Status string if system exists, empty string otherwise
    std::string GetSystemStatus(const std::string& name) const;
    
    // Get combined status of all systems
    // Returns: String containing status information for all systems
    std::string GetCombinedStatus() const;
    
    // Log system state changes
    // Parameter: systemName - Name of the system that changed
    // Parameter: oldStatus - Previous enabled status
    // Parameter: newStatus - New enabled status
    void LogStateChange(const std::string& systemName, bool oldStatus, bool newStatus);
    
    // Get count of managed systems
    // Returns: Number of systems currently managed
    size_t GetSystemCount() const;
    
    // Clear all managed systems
    void ClearAllSystems();
    
    // Get global proxy instance (Singleton pattern)
    // Returns: Reference to the global SimulationSystemProxy instance
    static SimulationSystemProxy& GetInstance();
    
    // Set global proxy instance
    // Parameter: proxy - Shared pointer to the proxy instance to set
    static void SetInstance(std::shared_ptr<SimulationSystemProxy> proxy);
    
    // Initialize proxy with brain reference
    // Parameter: brain - Pointer to the brain for dependency injection
    void Initialize(Brain* brain);
    
    // Update all systems
    // Parameter: timestep - Time elapsed since last update (seconds)
    void Update(double timestep);
    
    // Reset all systems
    void Reset();
    
    // Get proxy status
    // Returns: String containing proxy status information
    std::string GetStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
