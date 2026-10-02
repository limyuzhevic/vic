#pragma once

#include <string>
#include <chrono>

namespace nlm {

// Integration state structure
struct IntegrationState {
    size_t systemsIntegrated;        // Number of integrated systems
    size_t totalSystems;             // Total number of systems expected
    float integrationScore;          // Integration score (0.0 - 1.0)
    double lastActivityTime;         // Last time integration was updated
};

// Integration manager for coordinating system interactions
class IntegrationManager {
public:
    IntegrationManager();
    ~IntegrationManager();
    
    // Disable copying
    IntegrationManager(const IntegrationManager&) = delete;
    IntegrationManager& operator=(const IntegrationManager&) = delete;
    
    /**
     * Initialize integration manager with brain reference
     * @param brain Pointer to brain instance
     * @return true if successful
     */
    bool initialize(class Brain* brain);
    
    /**
     * Update integration state (should be called regularly)
     */
    void update();
    
    /**
     * Enable or disable integration checking
     * @param enable True to enable, false to disable
     */
    void enableIntegration(bool enable);
    
    /**
     * Check if integration is enabled
     * @return true if enabled
     */
    bool isIntegrationEnabled() const;
    
    /**
     * Get current integration state
     * @return Reference to integration state
     */
    const IntegrationState& getIntegrationState() const;
    
    /**
     * Attempt to reconnect a specific system
     * @param systemName Name of system to reconnect
     * @return true if reconnection attempted
     */
    bool reconnectSystem(const std::string& systemName);
    
private:
    /**
     * Update integration state
     */
    void updateIntegration();
    
    /**
     * Handle integration issues
     */
    void handleIntegrationIssues();
    
    class Brain* brain_;                    // Brain instance
    bool integrationEnabled_;              // Whether integration is enabled
    IntegrationState integrationState_;    // Current integration state
    double lastIntegrationCheck_;          // Last integration check time
};

} // namespace nlm
