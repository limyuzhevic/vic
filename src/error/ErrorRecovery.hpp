// Error Recovery Header
#pragma once

#include <string>
#include "NLMErrors.hpp"

namespace nlm {

/**
 * Error recovery base class
 */
class ErrorRecovery : public IErrorRecovery {
public:
    ErrorRecovery(ErrorCode code, const std::string& description);
    virtual ~ErrorRecovery() = default;
    
    ErrorCode getCode() const;
    std::string getDescription() const;
    
    virtual void execute(const NLMError& error) override;
    
private:
    ErrorCode _code;
    std::string _description;
};

} // namespace nlm