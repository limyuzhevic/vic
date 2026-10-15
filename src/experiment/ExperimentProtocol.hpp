#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "../core/Config/Config.hpp"

namespace nlm {

// Forward declarations
class ExperimentProtocol;
class ExperimentContext;
class ExperimentResults;
class ExperimentReport;
class Config;
class ExperimentConfig;

class ExperimentProtocol {
public:
    virtual ~ExperimentProtocol() = default;
    
    // Experiment lifecycle
    virtual bool initialize(const ExperimentConfig& config) = 0;
    virtual bool execute(ExperimentContext& context) = 0;
    virtual bool analyze(const ExperimentResults& results) = 0;
    virtual bool report(const ExperimentReport& report) = 0;
    
    // Protocol features
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual bool validate(const ExperimentConfig& config) const = 0;
    
    // Protocol composition
    virtual bool addSubProtocol(std::shared_ptr<ExperimentProtocol> subProtocol);
    virtual bool removeSubProtocol(const std::string& name);
    virtual std::vector<std::shared_ptr<ExperimentProtocol>> getSubProtocols() const;
};

} // namespace nlm