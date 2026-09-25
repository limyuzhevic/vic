#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error(message) {}
};

class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) 
        : NLMException("Config error: " + message) {}
};

class ConfigLoadException : public ConfigException {
public:
    explicit ConfigLoadException(const std::string& file, const std::string& reason) 
        : ConfigException("Failed to load config file \"" + file + "\": " + reason) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& file, const std::string& line, const std::string& reason) 
        : ConfigException("Failed to parse config file \"" + file + "\" at line \"" + line + "\": " + reason) {}
};

class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message) 
        : NLMException("Brain error: " + message) {}
};

class BrainInitializationException : public BrainException {
public:
    explicit BrainInitializationException(const std::string& reason) 
        : BrainException("Brain initialization failed: " + reason) {}
};

class PlasticityRuleException : public NLMException {
public:
    explicit PlasticityRuleException(const std::string& message) 
        : NLMException("Plasticity rule error: " + message) {}
};

class RewardModulatedSTDPException : public PlasticityRuleException {
public:
    explicit RewardModulatedSTDPException(const std::string& reason) 
        : PlasticityRuleException("Reward-modulated STDP error: " + reason) {}
};

class ValidationException : public NLMException {
public:
    explicit ValidationException(const std::string& message) 
        : NLMException("Validation error: " + message) {}
};

class CommandLineException : public NLMException {
public:
    explicit CommandLineException(const std::string& message) 
        : NLMException("Command line error: " + message) {}
};

} // namespace nlm