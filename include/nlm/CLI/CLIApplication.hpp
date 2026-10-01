// NLM (熙然) - Neural Learning Machine
// CLI Application Base Class
//
// Abstract base class for NLM command-line applications that provides
// common functionality and integration with the CLI framework.

#pragma once

#include "CLI/CLI.hpp"
#include <memory>
#include <string>

namespace nlm {

class CLIApplication {
public:
    CLIApplication(const std::string& programName, const std::string& description);
    virtual ~CLIApplication() = default;

    // Main application entry point
    int run(int argc, char** argv);

    // Get the CLI parser
    CLIArgumentParser& getParser() { return parser; }
    const CLIArgumentParser& getParser() const { return parser; }

    // Virtual methods to be overridden by subclasses
    virtual bool initialize() { return true; }
    virtual int execute() = 0;
    virtual void cleanup() {}

    // Helper methods
    std::string getVersion() const { return version; }
    void setVersion(const std::string& version) { this->version = version; }

protected:
    // Register standard application arguments
    void registerStandardArguments();

    // Helper for logging to CLI
    void logInfo(const std::string& message) const;
    void logError(const std::string& message) const;
    void logWarning(const std::string& message) const;

private:
    CLIArgumentParser parser;
    std::string version;
};

} // namespace nlm
