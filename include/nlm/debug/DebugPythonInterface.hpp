// ==============================================================================
// Debug Python Interface
// ==============================================================================
#ifndef DEBUGPYTHONINTERFACE_HPP
#define DEBUGPYTHONINTERFACE_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "../src/brain/Brain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../config/OptimizedConfig.hpp"
#include "../performance/PerformanceProfiler.hpp"

namespace nlm {
namespace debug {

/**
 * @class DebugPythonInterface
 * @brief Python-based debugging interface for NLM
 * 
 * Provides Python-based debugging interface with interactive debugging sessions,
 * Python-based analysis tools, and Jupyter notebook integration.
 */
class DebugPythonInterface {
public:
    /**
     * @enum PythonInterfaceMode
     * @brief Python interface mode
     */
    enum class PythonInterfaceMode {
        INTERACTIVE,
        BATCH,
        NOTEBOOK,
        SCRIPT
    };

    /**
     * @struct PythonAnalysisResult
     * @brief Python analysis result
     */
    struct PythonAnalysisResult {
        std::string analysisType;
        std::map<std::string, double> metrics;
        std::vector<std::string> insights;
        std::string visualizationCode;
        std::string interpretation;
        std::chrono::steady_clock::time_point timestamp;
    };

    DebugPythonInterface();
    ~DebugPythonInterface();

    /**
     * @brief Start Python debugging interface
     * @param mode Interface mode
     * @param host Host address
     * @param port Port number
     */
    void startPythonInterface(PythonInterfaceMode mode = PythonInterfaceMode::INTERACTIVE, const std::string& host = "localhost", int port = 8888);

    /**
     * @brief Stop Python debugging interface
     */
    void stopPythonInterface();

    /**
     * @brief Connect Python session
     * @return true if connection was successful
     */
    bool connectPythonSession();

    /**
     * @brief Disconnect Python session
     */
    void disconnectPythonSession();

    /**
     * @brief Is Python session connected?
     * @return true if Python session is connected
     */
    bool isPythonSessionConnected() const;

    /**
     * @brief Execute Python command
     * @param command Python command
     * @return Python execution result
     */
    std::string executePythonCommand(const std::string& command);

    /**
     * @brief Execute Python analysis
     * @param analysisType Analysis type
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @return Python analysis result
     */
    PythonAnalysisResult executePythonAnalysis(const std::string& analysisType, const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Get Python analysis results
     * @param analysisType Analysis type
     * @return Vector of analysis results
     */
    std::vector<PythonAnalysisResult> getPythonAnalysisResults(const std::string& analysisType) const;

    /**
     * @brief Save analysis results
     * @param result Analysis result
     * @param filename Output file path
     */
    void saveAnalysisResults(const PythonAnalysisResult& result, const std::string& filename);

    /**
     * @brief Load analysis results
     * @param filename Input file path
     * @return Loaded analysis result
     */
    PythonAnalysisResult loadAnalysisResults(const std::string& filename);

    /**
     * @brief Get Python debugging scripts
     * @return Vector of script names
     */
    std::vector<std::string> getPythonDebuggingScripts() const;

    /**
     * @brief Execute Python debugging script
     * @param scriptName Script name
     * @param parameters Script parameters
     * @return Python execution result
     */
    std::string executePythonDebuggingScript(const std::string& scriptName, const std::map<std::string, std::string>& parameters);

    /**
     * @brief Get available Python analysis types
     * @return Vector of analysis types
     */
    std::vector<std::string> getAvailablePythonAnalysisTypes() const;

    /**
     * @brief Get Python interface status
     * @return Interface status
     */
    struct InterfaceStatus {
        bool isConnected;
        std::string mode;
        std::string host;
        int port;
        std::chrono::steady_clock::time_point connectionTime;
        uint64_t commandsExecuted;
        double averageExecutionTime;
    };

    InterfaceStatus getInterfaceStatus() const;

    /**
     * @brief Generate Python interface report
     * @return Interface report
     */
    struct InterfaceReport {
        std::chrono::steady_clock::time_point reportTime;
        bool connectionSuccess;
        std::vector<std::string> executedCommands;
        std::vector<PythonAnalysisResult> analysisResults;
        std::string interfaceMode;
        std::string host;
        int port;
    };

    InterfaceReport generateInterfaceReport();

    /**
     * @brief Start Jupyter notebook integration
     * @param notebookPath Notebook file path
     */
    void startJupyterIntegration(const std::string& notebookPath);

    /**
     * @brief Stop Jupyter notebook integration
     */
    void stopJupyterIntegration();

    /**
     * @brief Generate Python interface visualization
     * @param filename Output file path
     */
    void generatePythonInterfaceVisualization(const std::string& filename);

    /**
     * @brief Generate Python analysis visualization
     * @param result Analysis result
     * @param filename Output file path
     */
    void generatePythonAnalysisVisualization(const PythonAnalysisResult& result, const std::string& filename);

    /**
     * @brief Get Python session statistics
     * @return Session statistics
     */
    struct SessionStatistics {
        uint64_t commandsExecuted;
        uint64_t analysisRequests;
        double averageExecutionTime;
        double totalExecutionTime;
        std::chrono::steady_clock::time_point firstConnection;
        std::chrono::steady_clock::time_point lastConnection;
        uint64_t errorsEncountered;
    };

    SessionStatistics getSessionStatistics() const;

protected:
    /**
     * @brief Update interface status
     */
    void updateInterfaceStatus();

    /**
     * @brief Execute Python command
     * @param command Python command
     * @return Python execution result
     */
    std::string executePythonCommandInternal(const std::string& command);

    /**
     * @brief Execute Python analysis
     * @param analysisType Analysis type
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @return Python analysis result
     */
    PythonAnalysisResult executePythonAnalysisInternal(const std::string& analysisType, const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Generate interface report
     */
    void generateInterfaceReportInternal();

    /**
     * @brief Create visualization
     * @param filename Output file path
     */
    void createVisualization(const std::string& filename);

    /**
     * @brief Create analysis visualization
     * @param result Analysis result
     * @param filename Output file path
     */
    void createAnalysisVisualization(const PythonAnalysisResult& result, const std::string& filename);

    /**
     * @brief Generate Python code
     * @param analysisType Analysis type
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @return Python code
     */
    std::string generatePythonCode(const std::string& analysisType, const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Execute Python code
     * @param pythonCode Python code
     * @return Python execution result
     */
    std::string executePythonCode(const std::string& pythonCode);

    /**
     * @brief Parse analysis result
     * @param result Analysis result
     * @return Parsed result
     */
    std::map<std::string, double> parseAnalysisResult(const std::string& result);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // DEBUGPYTHONINTERFACE_HPP
