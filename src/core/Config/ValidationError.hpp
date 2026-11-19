// Validation Error Definition
#ifndef VALIDATION_ERROR_HPP
#define VALIDATION_ERROR_HPP

#include <string>

namespace nlm {

enum class ValidationErrorType {
    TypeMismatch,
    ValueOutOfRange,
    RequiredParameterMissing,
    PatternMismatch,
    FilePathInvalid,
    CrossParameterInvalid,
    DeprecatedParameter,
    InvalidFormat
};

class ValidationError {
public:
    ValidationError(ValidationErrorType type, const std::string& message, 
                   size_t lineNumber = 0, const std::string& context = "")
        : type(type), message(message), lineNumber(lineNumber), context(context) {}
    
    ValidationErrorType getType() const { return type; }
    const std::string& getMessage() const { return message; }
    size_t getLineNumber() const { return lineNumber; }
    const std::string& getContext() const { return context; }
    
    std::string toString() const {
        std::ostringstream oss;
        if (lineNumber > 0) {
            oss << "Line " << lineNumber << ": ";
        }
        oss << message;
        if (!context.empty()) {
            oss << " (Context: " << context << ")";
        }
        return oss.str();
    }

private:
    ValidationErrorType type;
    std::string message;
    size_t lineNumber;
    std::string context;
};

} // namespace nlm

#endif // VALIDATION_ERROR_HPP
