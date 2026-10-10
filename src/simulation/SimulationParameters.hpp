#pragma once

/**
 * Simulation Parameters for NLM
 * 
 * Configuration parameters for NLM simulation controls
 * 
 * Features:
 * - Comprehensive parameter management
 * - Parameter validation
 * - Parameter serialization
 * - Default parameter values
 * - Parameter categories
 * 
 * Usage:
 * ```cpp
 * #include <nlm/simulation/SimulationParameters.hpp>
 * 
 * int main() {
 *     // Create default parameters
 *     auto params = nlm::createSimulationParameters();
 *     
 *     // Modify parameters
 *     params->speed = 2.0;
 *     params->timeScale = 1.5;
 *     params->createWorld = true;
 *     params->worldWidth = 50;
 *     params->worldHeight = 50;
 *     
 *     // Save parameters to file
 *     params->saveToFile("sim_params.json");
 * }
 * ```
 */

#include <string>
#include <map>
#include <nlohmann/json.hpp>

namespace nlm {

/**
 * Simulation parameter categories
 */
enum class ParameterCategory {
    BASIC,
    WORLD,
    BRAIN,
    AGENT,
    ANALYSIS,
    VISUALIZATION,
    CONTROL,
    ADVANCED,
    CUSTOM
};

/**
 * Parameter metadata
 */
struct ParameterInfo {
    std::string name;                 // Parameter name
    std::string description;          // Parameter description
    ParameterCategory category;       // Parameter category
    std::string type;                 // Parameter type ("int", "float", "bool", "string", "double")
    double defaultValue;              // Default value
    double minValue;                  // Minimum value
    double maxValue;                  // Maximum value
    bool required;                    // Whether parameter is required
    std::string unit;                 // Unit (e.g., "s", "Hz", "dimension")
    std::map<std::string, std::string> extraInfo; // Extra information
    
    ParameterInfo() : category(ParameterCategory::BASIC), defaultValue(0.0),
                     minValue(0.0), maxValue(0.0), required(false) {}
    
    ParameterInfo(const std::string& name, const std::string& description,
                  ParameterCategory category, const std::string& type,
                  double defaultValue, double minValue = 0.0,
                  double maxValue = 0.0, bool required = false,
                  const std::string& unit = "")
        : name(name), description(description), category(category), type(type),
          defaultValue(defaultValue), minValue(minValue), maxValue(maxValue),
          required(required), unit(unit) {}
};

/**
 * Simulation parameters for NLM
 */
class SimulationParameters {
public:
    SimulationParameters();
    explicit SimulationParameters(const std::string& filePath);
    
    // Basic simulation parameters
    double speed;                      // Simulation speed (1.0 = real-time)
    double timeScale;                   // Time scaling factor
    int maxFramesToSkip;                // Maximum frames to skip for fast-forward
    bool conditionalExecutionEnabled;   // Enable conditional execution
    bool eventDrivenSimulation;          // Enable event-driven simulation
    bool createWorld;                   // Create world simulation
    bool createAgent;                   // Create agent
    bool createAgentBrain;              // Create agent brain
    
    // World parameters
    int worldWidth;                     // World width
    int worldHeight;                    // World height
    int visionWidth;                    // Vision width
    int visionHeight;                   // Vision height
    double agentStartX;                 // Agent starting position X
    double agentStartY;                 // Agent starting position Y
    
    // Brain parameters
    int neuronCount;                    // Number of neurons
    int regionCount;                    // Number of brain regions
    float connectionProbability;        // Connection probability
    float simulationTimestep;           // Simulation timestep
    int randomSeed;                      // Random seed
    
    // Analysis parameters
    bool enablePerformanceAnalysis;     // Enable performance analysis
    bool enableStateAnalysis;           // Enable state analysis
    bool enableBehavioralAnalysis;     // Enable behavioral analysis
    bool enableVisualization;           // Enable visualization
    
    // Control parameters
    bool enableBranchSimulation;       // Enable branch simulation
    bool enableParallelSimulation;      // Enable parallel simulation
    
    // Advanced parameters
    int maxStepCount;                   // Maximum number of steps
    double maxSimulationTime;           // Maximum simulation time
    double checkpointInterval;          // Checkpoint interval
    std::string checkpointPath;        // Checkpoint file path
    bool autoSave;                       // Auto-save state
    
    // Configuration file
    std::string configFilePath;        // Configuration file path
    
    // Custom parameters
    std::map<std::string, double> customParameters; // Custom parameters
    
    // Parameter information
    static std::map<std::string, ParameterInfo> getParameterInfo();
    
    // Parameter validation
    bool validate() const;
    std::string validateAndGetErrors() const;
    
    // Parameter modification
    void setParameter(const std::string& name, double value);
    double getParameter(const std::string& name) const;
    bool hasParameter(const std::string& name) const;
    
    // Parameter serialization
    bool saveToFile(const std::string& filePath) const;
    bool loadFromFile(const std::string& filePath);
    std::string toJSON() const;
    bool fromJSON(const std::string& jsonStr);
    
    // Parameter categories
    std::vector<std::string> getParameterNamesByCategory(ParameterCategory category) const;
    ParameterCategory getParameterCategory(const std::string& name) const;
    
    // Parameter descriptions
    std::string getParameterDescription(const std::string& name) const;
    std::string getParameterType(const std::string& name) const;
    double getParameterMin(const std::string& name) const;
    double getParameterMax(const std::string& name) const;
    std::string getParameterUnit(const std::string& name) const;
    
    // Parameter ranges
    bool isParameterInRange(const std::string& name, double value) const;
    double clampParameter(const std::string& name, double value) const;
    
    // Parameter defaults
    double getDefaultValue(const std::string& name) const;
    bool isRequiredParameter(const std::string& name) const;
    
    // Parameter enumeration
    std::vector<std::string> getAllParameterNames() const;
    
    // Parameter modification with validation
    bool setParameterValidated(const std::string& name, double value);
    bool setParameterIfChanged(const std::string& name, double value);
    
    // Parameter comparison
    bool equals(const SimulationParameters& other) const;
    bool isSubsetOf(const SimulationParameters& other) const;
    
    // Parameter merging
    void merge(const SimulationParameters& other);
    void mergeWithDefault(const SimulationParameters& defaultParams);
    
    // Parameter reset
    void resetToDefault();
    void resetToMinimal();
    void resetToPerformance();
    
    // Parameter validation helpers
    static bool isValidSpeed(double speed);
    static bool isValidTimeScale(double timeScale);
    static bool isValidTimestep(double timestep);
    static bool isValidNeuronCount(int count);
    static bool isValidConnectionProbability(double prob);
    static bool isValidWorldSize(int width, int height);
    
private:
    // Load default parameter info
    void loadDefaultParameterInfo();
    
    // Validate individual parameters
    bool validateBasicParameters() const;
    bool validateWorldParameters() const;
    bool validateBrainParameters() const;
    bool validateAnalysisParameters() const;
    bool validateControlParameters() const;
    bool validateAdvancedParameters() const;
    bool validateCustomParameters() const;
    
    // Get parameter info by name
    const ParameterInfo* getParameterInfo(const std::string& name) const;
    
    // Parameter info storage
    static std::map<std::string, ParameterInfo> parameterInfo_;
    
    // Parameter validation errors
    std::vector<std::string> validationErrors_;
};

// Factory functions
std::unique_ptr<SimulationParameters> createSimulationParameters();
std::unique_ptr<SimulationParameters> createMinimalSimulationParameters();
std::unique_ptr<SimulationParameters> createPerformanceSimulationParameters();

} // namespace nlm
