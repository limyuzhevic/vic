#pragma once

#include <string>
#include <vector>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

class AdvancedMultiObjectiveOptimization {
public:
    AdvancedMultiObjectiveOptimization(const Config& config);
    ~AdvancedMultiObjectiveOptimization();
    
    void runOptimization(const Brain& brain, const Config& config);
    
    std::vector<std::vector<double>> getParetoOptimalSolutions() const;
    
    std::string getOptimizationSummary() const;
    
    void saveResults(const Config& config) const;
    
    void loadResults(const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
