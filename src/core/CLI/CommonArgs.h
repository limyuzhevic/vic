#ifndef NLM_COMMON_ARGS_H
#define NLM_COMMON_ARGS_H

namespace nlm {
namespace cli {

/**
 * @brief Common argument definitions for NLM programs
 * 
 * Static constants for commonly used command-line arguments across all
 * NLM demo programs, ensuring consistency in the user interface.
 */
class CommonArgs {
public:
    // Basic run parameters
    static const char EPISODES_SHORT = 'e';
    static const char EPISODES_LONG = "--episodes";
    static const char STEPS_SHORT = 's';
    static const char STEPS_LONG = "--steps";
    static const char CONFIG_SHORT = 'c';
    static const char CONFIG_LONG = "--config";
    static const char SEED_SHORT = 'S';
    static const char SEED_LONG = "--seed";
    
    // Output and logging
    static const char LOG_LEVEL_SHORT = 'l';
    static const char LOG_LEVEL_LONG = "--log-level";
    static const char QUIET_SHORT = 'q';
    static const char QUIET_LONG = "--quiet";
    static const char VERBOSE_SHORT = 'v';
    static const char VERBOSE_LONG = "--verbose";
    static const char OUTPUT_SHORT = 'o';
    static const char OUTPUT_LONG = "--output";
    
    // Debug and development
    static const char DEBUG_SHORT = 'd';
    static const char DEBUG_LONG = "--debug";
    static const char VALIDATE_SHORT = 'V';
    static const char VALIDATE_LONG = "--validate";
    static const char PROFILE_SHORT = 'p';
    static const char PROFILE_LONG = "--profile";
    
    // Advanced
    static const char MODE_SHORT = 'm';
    static const char MODE_LONG = "--mode";
    static const char INTERACTIVE_SHORT = 'i';
    static const char INTERACTIVE_LONG = "--interactive";
    
    // File operations
    static const char EXPORT_SHORT = 'x';
    static const char EXPORT_LONG = "--export";
    static const char IMPORT_SHORT = 'I';
    static const char IMPORT_LONG = "--import";
    
    // Performance
    static const char TIMING_SHORT = 't';
    static const char TIMING_LONG = "--timing";
    static const char MEMORY_SHORT = 'm';
    static const char MEMORY_LONG = "--memory";
    
    // Phase-specific configuration
    static const char NEURON_COUNT_SHORT = 'n';
    static const char NEURON_COUNT_LONG = "--neuron-count";
    static const char MAX_STEPS_SHORT = 'x';
    static const char MAX_STEPS_LONG = "--max-steps";
};

} // namespace cli
} // namespace nlm

#endif // NLM_COMMON_ARGS_H
