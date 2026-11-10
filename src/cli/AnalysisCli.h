// Analysis CLI Tools
#ifndef NLM_ANALYSIS_CLI_H
#define NLM_ANALYSIS_CLI_H

#include <map>
#include <string>

class AnalysisCli {
public:
    static void importAnalysis(const std::map<std::string, std::string>& options);
    static void exportAnalysis(const std::map<std::string, std::string>& options);
    static void computeStatistics(const std::map<std::string, std::string>& options);
    static void computeCorrelation(const std::map<std::string, std::string>& options);
    static void performClustering(const std::map<std::string, std::string>& options);
    static void generateVisualization(const std::map<std::string, std::string>& options);
    static void compareAnalyses(const std::map<std::string, std::string>& options);
    static void buildModel(const std::map<std::string, std::string>& options);
    static void makePrediction(const std::map<std::string, std::string>& options);
    static void validateAnalysis(const std::map<std::string, std::string>& options);
};

#endif // NLM_ANALYSIS_CLI_H
