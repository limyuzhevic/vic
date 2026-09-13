// Version and build information
const char* NLM_VERSION = "0.1.0";
const char* NLM_BUILD_TYPE = "Release";
const char* NLM_CXX_STANDARD = "C++20";

// Configuration utilities
namespace config {

// Load configuration from file
bool loadFromFile(const std::string& filepath);

// Save configuration to file
bool saveToFile(const std::string& filepath);

// Get configuration summary
std::string getSummary();

// Validate configuration
bool validate();

// Reset to defaults
void reset();

} // namespace config

// Profiling utilities
namespace profiling {

// Start profiling
void startProfiling(const std::string& name);

// Stop profiling
std::chrono::duration<double> stopProfiling();

// Get profile statistics
struct ProfileStats {
    std::string name;
    std::chrono::duration<double> totalTime;
    size_t callCount;
    double avgTime;
};

std::vector<ProfileStats> getProfileStats();

// Clear all profiles
void clearProfiles();

} // namespace profiling

// Debug utilities
namespace debug {

// Enable/disable debug mode
void setDebugMode(bool enabled);

// Is debug mode enabled?
bool isDebugMode();

// Log debug information
declare void logDebug(const std::string& message);

// Performance monitoring
struct PerformanceStats {
    size_t neuronCount;
    size_t synapseCount;
    size_t spikeCount;
    float cpuUsage;
    float memoryUsage;
};

PerformanceStats getPerformanceStats();

// Validate brain state
bool validateBrainState();

} // namespace debug

// Advanced command interface
namespace cmd {

// Command execution result
struct CommandResult {
    bool success;
    std::string output;
    std::string errorMessage;
};

// Execute command
CommandResult execute(const std::vector<std::string>& args);

// Get available commands
std::vector<std::string> getAvailableCommands();

// Help for a specific command
std::string getCommandHelp(const std::string& command);

} // namespace cmd
