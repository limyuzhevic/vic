#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * @class BrainInitializer
 * @brief Standardizes brain initialization with proper error handling and resource management
 * 
 * This class provides a centralized initialization pattern for the NLM brain,
 * ensuring proper setup of all integrated systems and consistent configuration handling.
 */
class BrainInitializer {
public:
    BrainInitializer(std::shared_ptr<Config> config = nullptr);
    ~BrainInitializer();
    
    // Disable copying and moving
    BrainInitializer(const BrainInitializer&) = delete;
    BrainInitializer& operator=(const BrainInitializer&) = delete;
    BrainInitializer(BrainInitializer&&) noexcept = default;
    BrainInitializer& operator=(BrainInitializer&&) noexcept = default;
    
    /**
     * @brief Initialize the brain with configuration
     * 
     * @param config Configuration object (creates default if nullptr)
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize(std::shared_ptr<Config> config = nullptr);
    
    /**
     * @brief Create a configured brain instance
     * 
     * @param config Configuration object
     * @return std::shared_ptr<Brain> Configured brain instance, or nullptr on failure
     */
    static std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
    
    /**
     * @brief Configure default Phase 6 settings
     * 
     * @param config Configuration object to update
     */
    static void applyPhase6Defaults(std::shared_ptr<Config> config);
    
    /**
     * @brief Validate configuration parameters
     * 
     * @param config Configuration to validate
     * @return true if configuration is valid, false otherwise
     */
    static bool validateConfiguration(const std::shared_ptr<Config>& config);
    
    /**
     * @brief Get initialization error message
     * 
     * @return std::string Error message if initialization failed
     */
    const std::string& getErrorMessage() const;
    
    /**
     * @brief Check if brain was successfully initialized
     * 
     * @return true if brain is in a valid initialized state
     */
    bool isInitialized() const;
    
    /**
     * @brief Reset initialization state
     */
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm