#ifndef NLM_BASE_DEMO_HPP
#define NLM_BASE_DEMO_HPP

#include "src/experiments/NLMExperiments.hpp"
#include <string>
#include <memory>

namespace nlm {

/**
 * Base class for neural demos with common functionality
 * Extends NeuralDemo with additional utilities
 */
class BaseDemo : public NeuralDemo {
public:
    virtual ~BaseDemo() = default;
    
    virtual void run() override = 0;
    virtual void setupConfiguration(std::shared_ptr<Config> config) override = 0;
    virtual void printHeader() const override = 0;
    virtual void printFooter() const override = 0;
    
    // Common utility functions
    void initializeLogging(const std::string& logLevel, const std::string& logFile = "");
    void saveConfiguration(const std::string& filename);
    bool loadConfiguration(const std::string& filename);
    virtual void printDemoInfo() const;
    
protected:
    NLMCommandLine::Options options;
};

} // namespace nlm

#endif // NLM_BASE_DEMO_HPP