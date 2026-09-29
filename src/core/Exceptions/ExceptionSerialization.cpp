// This file contains the implementation of the exception serialization methods
// For each exception type in NLMBaseException.hpp

#include "NLMBaseException.hpp"

namespace nlm {

// Helper function for exception serialization
namespace ExceptionSerialization {

json serializeException(const NLMBaseException& exception) {
    return exception.toJson();
}

std::unique_ptr<NLMBaseException> deserializeException(const json& data) {
    return NLMBaseException::fromJson(data);
}

std::string generateErrorReport(const NLMBaseException& exception) {
    std::stringstream ss;
    ss << "=== ERROR REPORT ===" << std::endl;
    ss << "Type: " << getExceptionTypeName(exception) << std::endl;
    ss << "Message: " << exception.what() << std::endl;
    ss << "Code: " << static_cast<int>(exception.getCode()) << std::endl;
    ss << "Location: " << exception.getFile() << ":" << exception.getLine() 
       << " in " << exception.getFunction() << std::endl;
    
    const auto& context = exception.getContext();
    if (!context.empty()) {
        ss << "Context:" << std::endl;
        for (const auto& [key, value] : context) {
            ss << "  " << key << ": " << value << std::endl;
        }
    }
    
    const auto& causes = exception.getCauses();
    if (!causes.empty()) {
        ss << "Causes:" << std::endl;
        for (const auto& cause : causes) {
            ss << "  - " << cause << std::endl;
        }
    }
    
    ss << "=====================" << std::endl;
    return ss.str();
}

std::string getExceptionTypeName(const NLMBaseException& exception) {
    const auto& type = exception.toJson().value("type", "");
    if (type == "NLMValidationException") return "Validation Exception";
    if (type == "NLMBrainException") return "Brain Exception";
    if (type == "NLMSimulationException") return "Simulation Exception";
    if (type == "NLMSystemException") return "System Exception";
    if (type == "NLMFileException") return "File Exception";
    return "Base Exception";
}

} // namespace ExceptionSerialization

} // namespace nlm
