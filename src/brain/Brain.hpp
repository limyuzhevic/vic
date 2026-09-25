// Get current configuration
    /**
     * @brief Get the configuration object used to initialize the brain.
     * 
     * @return Shared pointer to the configuration object
     * @note Configuration contains simulation parameters and settings
     */
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    /**
     * @brief Get access to the random number generator.
     * 
     * @return Pointer to the RandomGenerator instance for stochastic operations
     * @note Used for initializing random neural connectivity and stochastic processes
     */
    RandomGenerator* getRandomGenerator();
    
    // Logging
    /**
     * @brief Log the current status of the brain to the global logger.
     * 
     * @note Logs key statistics including neuron counts, spike activity, and system status
     */
    void logStatus() const;