#pragma once

#include <string>
#include <memory>
#include <unordered_map>

namespace nlm {

/**
 * Configuration for NLM phase selection and system settings
 */
class NLMConfig {
public:
    enum class Phase {
        Phase2,      // Real neural computation
        Phase3,      // Memory systems integration
        Phase4,      // Cognitive systems integration
        Phase5,      // Performance optimizations
        Phase6       // Final integration (default)
    };

    /**
     * Get the current configuration
     */
    static NLMConfig& getInstance() {
        static NLMConfig instance;
        return instance;
    }

    /**
     * Set the phase to run
     */
    void setPhase(Phase phase) {
        currentPhase = phase;
    }

    /**
     * Get the current phase
     */
    Phase getPhase() const {
        return currentPhase;
    }

    /**
     * Set a configuration value
     */
    template<typename T>
    void set(const std::string& key, const T& value) {
        configValues[key] = std::to_string(value);
    }

    /**
     * Get a configuration value
     */
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const {
        auto it = configValues.find(key);
        if (it == configValues.end()) {
            return defaultValue;
        }
        try {
            // Simple conversion for basic types
            return static_cast<T>(std::stod(it->second));
        } catch (...) {
            return defaultValue;
        }
    }

    /**
     * Set debug mode
     */
    void setDebugMode(bool enabled) {
        debugMode = enabled;
    }

    /**
     * Check if debug mode is enabled
     */
    bool isDebugMode() const {
        return debugMode;
    }

    /**
     * Print configuration
     */
    void printConfig() const {
        std::cout << "NLM Configuration:" << std::endl;
        std::cout << "  Phase: " << phaseToString(currentPhase) << std::endl;
        std::cout << "  Debug Mode: " << (debugMode ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Configuration Values:" << std::endl;
        for (const auto& pair : configValues) {
            std::cout << "    " << pair.first << ": " << pair.second << std::endl;
        }
    }

private:
    NLMConfig() : currentPhase(Phase::Phase6), debugMode(false) {}

    /**
     * Convert phase to string
     */
    std::string phaseToString(Phase phase) const {
        switch (phase) {
            case Phase::Phase2: return "Phase 2";
            case Phase::Phase3: return "Phase 3";
            case Phase::Phase4: return "Phase 4";
            case Phase::Phase5: return "Phase 5";
            case Phase::Phase6: return "Phase 6";
            default: return "Unknown";
        }
    }

    Phase currentPhase;
    bool debugMode;
    std::unordered_map<std::string, std::string> configValues;
};

/**
 * Convenience functions for configuration
 */
inline void setPhase(NLMConfig::Phase phase) {
    NLMConfig::getInstance().setPhase(phase);
}

inline NLMConfig::Phase getPhase() {
    return NLMConfig::getInstance().getPhase();
}

inline void setDebugMode(bool enabled) {
    NLMConfig::getInstance().setDebugMode(enabled);
}

inline bool isDebugMode() {
    return NLMConfig::getInstance().isDebugMode();
}

} // namespace nlm
