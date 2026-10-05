#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

// Base NLM exception
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message) : std::runtime_error(message) {}
};

// Application-level exceptions
class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message) : NLMException("Configuration error: " + message) {}
};

class BrainInitializationException : public NLMException {
public:
    BrainInitializationException(const std::string& reason) 
        : NLMException("Brain initialization failed: " + reason) {}
};

class SimulationStepException : public NLMException {
public:
    SimulationStepException(size_t step, const std::string& reason)
        : NLMException("Simulation step " + std::to_string(step) + " failed: " + reason) {}
};

class ResourceAllocationException : public NLMException {
public:
    ResourceAllocationException(const std::string& resource, const std::string& reason)
        : NLMException("Resource allocation failed for " + resource + ": " + reason) {}
};

class FileOperationException : public NLMException {
public:
    enum class Operation { Read, Write, Open, Close };
    
    FileOperationException(Operation op, const std::string& filepath, const std::string& reason)
        : NLMException("File operation " + operationToString(op) + " failed for '" + 
                     filepath + "': " + reason) {}
    
private:
    static const char* operationToString(Operation op) {
        switch (op) {
            case Operation::Read: return "read";
            case Operation::Write: return "write";
            case Operation::Open: return "open";
            case Operation::Close: return "close";
            default: return "unknown";
        }
    }
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& reason) 
        : NLMException("Memory allocation failed: " + reason) {}
};

class NetworkException : public NLMException {
public:
    NetworkException(const std::string& reason)
        : NLMException("Network operation failed: " + reason) {}
};

// Test framework exceptions
class TestFrameworkException : public NLMException {
public:
    TestFrameworkException(const std::string& message) : NLMException("Test framework error: " + message) {}
};

class TestNotFoundException : public TestFrameworkException {
public:
    TestNotFoundException(const std::string& testName)
        : TestFrameworkException("Test not found: '" + testName + "'") {}
};

// Agent/system exceptions
class AgentException : public NLMException {
public:
    AgentException(const std::string& agentName, const std::string& reason)
        : NLMException("Agent '" + agentName + "' error: " + reason) {}
};

class EnvironmentException : public NLMException {
public:
    EnvironmentException(const std::string& reason)
        : NLMException("Environment error: " + reason) {}
};

// General exception hierarchy complete
} // namespace nlm
