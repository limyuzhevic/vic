#pragma once

#include <string>
#include <vector>
#include "../core/Config/Config.hpp"

namespace nlm {

struct DistributedTask {
    std::string taskId;
    std::string taskType;
    std::vector<double> parameters;
    std::vector<double> data;
    std::string sourceNode;
    std::vector<std::string> targetNodes;
    double priority;
    double complexity;
    double estimatedTime;
};

class AdvancedDistributedComputing {
public:
    AdvancedDistributedComputing(const std::string& id, const Config& config);
    ~AdvancedDistributedComputing();
    
    bool submitTask(const DistributedTask& task);
    
    std::vector<DistributedTask> runComputation(const std::vector<DistributedTask>& tasks);
    
    double getThroughput() const;
    
    double getReliability() const;
    
    double getScalability() const;
    
    std::string getComputingStatus() const;
    
    void saveState(const Config& config) const;
    
    void loadState(const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
