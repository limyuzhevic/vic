// Experiment CLI Tools
#ifndef NLM_EXPERIMENT_CLI_H
#define NLM_EXPERIMENT_CLI_H

#include <map>
#include <string>

class ExperimentCli {
public:
    static void createExperiment(const std::map<std::string, std::string>& options);
    static void runExperiment(const std::map<std::string, std::string>& options);
    static void manageExperiment(const std::map<std::string, std::string>& options);
    static void analyzeExperiment(const std::map<std::string, std::string>& options);
    static void compareExperiments(const std::map<std::string, std::string>& options);
    static void generateReport(const std::map<std::string, std::string>& options);
    static void exportExperiment(const std::map<std::string, std::string>& options);
    static void importExperiment(const std::map<std::string, std::string>& options);
};

#endif // NLM_EXPERIMENT_CLI_H
