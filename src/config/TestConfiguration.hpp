// NLM (熙然) - Neural Learning Machine
// Implementation: Configuration Management
// Provides configuration presets and validation for NLM tests

#ifndef NLM_CONFIG_TESTCONFIGURATION_HPP
#define NLM_CONFIG_TESTCONFIGURATION_HPP

#include <memory>
#include <string>
#include <map>

namespace nlm::config {

class TestConfiguration {
public:
    struct Parameters {
        uint64_t randomSeed;
        double simulationTimestep;
        int64_t neuronCount;
        int64_t regionCount;
        float connectionProbability;
        float stdpLtpWeight;
        float stdpLtdWeight;
        float stdpTau;
        float synaptogenesisRate;
        float pruningRate;
    };
    
    static Parameters loadDefaults();
    static Parameters loadFromConfig(const std::shared_ptr<Config>& config);
    static Parameters loadFromCommandLine(const std::shared_ptr<Config>& config, int argc, char** argv);
};

} // namespace nlm::config

#endif // NLM_CONFIG_TESTCONFIGURATION_HPP
