#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Config/Config.hpp"

namespace nlm {

class AdvancedNeuroscientificAnalysis {
public:
    AdvancedNeuroscientificAnalysis(const std::string& id, const Config& config);
    ~AdvancedNeuroscientificAnalysis();
    
    std::string analyzeEEG(const std::string& subjectId, const std::vector<double>& eegData);
    
    std::string analyzeFMRI(const std::string& subjectId, const std::vector<std::vector<double>>& voxels);
    
    std::string performStatisticalAnalysis(const std::string& metricType);
    
    std::string getAdvancedNeuroscientificAnalysisStatus() const;
    
    void saveAnalysisResults(const Config& config) const;
    
    void loadAnalysisResults(const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
