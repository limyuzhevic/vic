#include "NLMBaseException.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

NLMBaseException::NLMBaseException(const std::string& message,
                                   ErrorCode code,
                                   const std::string& file,
                                   int line,
                                   const std::string& function)
    : std::runtime_error(message), code_(code), file_(file), line_(line), function_(function) {
}

void NLMBaseException::setContext(const std::string& key, const std::string& value) {
    context_[key] = value;
}

std::string NLMBaseException::getContextAsJson() const {
    json j = context_;
    return j.dump();
}

void NLMBaseException::addCause(const NLMBaseException& cause) {
    causes_.push_back(std::string(cause.what()) + " (Code: " + 
                     std::to_string(static_cast<int>(cause.getCode())) + ")");
}

json NLMBaseException::toJson() const {
    json j;
    j["type"] = "NLMBaseException";
    j["message"] = what();
    j["code"] = static_cast<int>(code_);
    j["file"] = file_;
    j["line"] = line_;
    j["function"] = function_;
    j["context"] = context_;
    j["causes"] = causes_;
    return j;
}

std::unique_ptr<NLMBaseException> NLMBaseException::fromJson(const json& j) {
    if (j.value("type", "") == "NLMValidationException") {
        return NLMValidationException::fromJson(j);
    } else if (j.value("type", "") == "NLMBrainException") {
        return NLMBrainException::fromJson(j);
    } else if (j.value("type", "") == "NLMSimulationException") {
        return NLMSimulationException::fromJson(j);
    } else if (j.value("type", "") == "NLMSystemException") {
        return NLMSystemException::fromJson(j);
    } else if (j.value("type", "") == "NLMFileException") {
        return NLMFileException::fromJson(j);
    } else {
        auto exception = std::make_unique<NLMBaseException>(
            j.value("message", ""),
            static_cast<ErrorCode>(j.value("code", 9999)),
            j.value("file", ""),
            j.value("line", 0),
            j.value("function", "")
        );
        
        if (j.contains("context")) {
            for (const auto& [key, value] : j["context"].items()) {
                exception->setContext(key, value);
            }
        }
        
        if (j.contains("causes")) {
            for (const auto& cause : j["causes"]) {
                exception->addCause(NLMBaseException(cause.get<std::string>()));
            }
        }
        
        return exception;
    }
}

const char* NLMBaseException::what() const noexcept {
    if (messageWithContext_.empty()) {
        std::stringstream ss;
        ss << std::runtime_error::what();
        if (!context_.empty()) {
            ss << " (Context: ";
            bool first = true;
            for (const auto& [key, value] : context_) {
                if (!first) ss << ", ";
                ss << key << "=" << value;
                first = false;
            }
            ss << ")";
        }
        messageWithContext_ = ss.str();
    }
    return messageWithContext_.c_str();
}

// NLMValidationException implementation
NLMValidationException::NLMValidationException(const std::string& message,
                                             const std::string& parameterName,
                                             const std::string& file,
                                             int line,
                                             const std::string& function)
    : NLMBaseException(message, ErrorCode::ValidationError, file, line, function),
      parameterName_(parameterName) {
}

json NLMValidationException::toJson() const {
    json j = NLMBaseException::toJson();
    j["type"] = "NLMValidationException";
    j["parameterName"] = parameterName_;
    return j;
}

std::unique_ptr<NLMValidationException> NLMValidationException::fromJson(const json& j) {
    auto exception = std::make_unique<NLMValidationException>(
        j.value("message", ""),
        j.value("parameterName", ""),
        j.value("file", ""),
        j.value("line", 0),
        j.value("function", "")
    );
    
    if (j.contains("context")) {
        for (const auto& [key, value] : j["context"].items()) {
            exception->setContext(key, value);
        }
    }
    
    if (j.contains("causes")) {
        for (const auto& cause : j["causes"]) {
            exception->addCause(NLMBaseException(cause.get<std::string>()));
        }
    }
    
    return exception;
}

// NLMBrainException implementation
NLMBrainException::NLMBrainException(const std::string& message,
                                   const std::string& networkName,
                                   const std::string& file,
                                   int line,
                                   const std::string& function)
    : NLMBaseException(message, ErrorCode::NeuralNetworkError, file, line, function),
      networkName_(networkName) {
}

json NLMBrainException::toJson() const {
    json j = NLMBaseException::toJson();
    j["type"] = "NLMBrainException";
    j["networkName"] = networkName_;
    return j;
}

std::unique_ptr<NLMBrainException> NLMBrainException::fromJson(const json& j) {
    auto exception = std::make_unique<NLMBrainException>(
        j.value("message", ""),
        j.value("networkName", ""),
        j.value("file", ""),
        j.value("line", 0),
        j.value("function", "")
    );
    
    if (j.contains("context")) {
        for (const auto& [key, value] : j["context"].items()) {
            exception->setContext(key, value);
        }
    }
    
    if (j.contains("causes")) {
        for (const auto& cause : j["causes"]) {
            exception->addCause(NLMBaseException(cause.get<std::string>()));
        }
    }
    
    return exception;
}

// NLMSimulationException implementation
NLMSimulationException::NLMSimulationException(const std::string& message,
                                             const std::string& simulationName,
                                             const std::string& file,
                                             int line,
                                             const std::string& function)
    : NLMBaseException(message, ErrorCode::SimulationError, file, line, function),
      simulationName_(simulationName) {
}

json NLMSimulationException::toJson() const {
    json j = NLMBaseException::toJson();
    j["type"] = "NLMSimulationException";
    j["simulationName"] = simulationName_;
    return j;
}

std::unique_ptr<NLMSimulationException> NLMSimulationException::fromJson(const json& j) {
    auto exception = std::make_unique<NLMSimulationException>(
        j.value("message", ""),
        j.value("simulationName", ""),
        j.value("file", ""),
        j.value("line", 0),
        j.value("function", "")
    );
    
    if (j.contains("context")) {
        for (const auto& [key, value] : j["context"].items()) {
            exception->setContext(key, value);
        }
    }
    
    if (j.contains("causes")) {
        for (const auto& cause : j["causes"]) {
            exception->addCause(NLMBaseException(cause.get<std::string>()));
        }
    }
    
    return exception;
}

// NLMSystemException implementation
NLMSystemException::NLMSystemException(const std::string& message,
                                     const std::string& systemComponent,
                                     const std::string& file,
                                     int line,
                                     const std::string& function)
    : NLMBaseException(message, ErrorCode::SystemError, file, line, function),
      systemComponent_(systemComponent) {
}

json NLMSystemException::toJson() const {
    json j = NLMBaseException::toJson();
    j["type"] = "NLMSystemException";
    j["systemComponent"] = systemComponent_;
    return j;
}

std::unique_ptr<NLMSystemException> NLMSystemException::fromJson(const json& j) {
    auto exception = std::make_unique<NLMSystemException>(
        j.value("message", ""),
        j.value("systemComponent", ""),
        j.value("file", ""),
        j.value("line", 0),
        j.value("function", "")
    );
    
    if (j.contains("context")) {
        for (const auto& [key, value] : j["context"].items()) {
            exception->setContext(key, value);
        }
    }
    
    if (j.contains("causes")) {
        for (const auto& cause : j["causes"]) {
            exception->addCause(NLMBaseException(cause.get<std::string>()));
        }
    }
    
    return exception;
}

// NLMFileException implementation
NLMFileException::NLMFileException(const std::string& message,
                                 const std::string& filepath,
                                 const std::string& file,
                                 int line,
                                 const std::string& function)
    : NLMBaseException(message, ErrorCode::FileReadError, file, line, function),
      filepath_(filepath) {
}

json NLMFileException::toJson() const {
    json j = NLMBaseException::toJson();
    j["type"] = "NLMFileException";
    j["filepath"] = filepath_;
    return j;
}

std::unique_ptr<NLMFileException> NLMFileException::fromJson(const json& j) {
    auto exception = std::make_unique<NLMFileException>(
        j.value("message", ""),
        j.value("filepath", ""),
        j.value("file", ""),
        j.value("line", 0),
        j.value("function", "")
    );
    
    if (j.contains("context")) {
        for (const auto& [key, value] : j["context"].items()) {
            exception->setContext(key, value);
        }
    }
    
    if (j.contains("causes")) {
        for (const auto& cause : j["causes"]) {
            exception->addCause(NLMBaseException(cause.get<std::string>()));
        }
    }
    
    return exception;
}

} // namespace nlm
