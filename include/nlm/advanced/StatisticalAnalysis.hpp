#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Config/Config.hpp"

namespace nlm {

class StatisticalAnalysis {
public:
    StatisticalAnalysis(const Config& config);
    ~StatisticalAnalysis();
    
    void addDataset(const std::string& id, const std::string& description,
                   const std::vector<double>& values);
    
    std::string performTTest(const std::string& dataset1Id, const std::string& dataset2Id);
    
    std::string calculateCorrelation(const std::string& dataset1Id, const std::string& dataset2Id);
    
    std::string calculateCovariance(const std::string& dataset1Id, const std::string& dataset2Id);
    
    std::string analyzeTimeSeries(const std::string& datasetId, int windowSize = 10);
    
    std::string getStatisticalAnalysisStatus() const;
    
    void saveResults(const Config& config) const;
    
    void loadResults(const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
