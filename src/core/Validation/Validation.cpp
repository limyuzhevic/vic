#include "Validation.hpp"

namespace nlm {

namespace Validation {
    ErrorLogger& ErrorLogger::GetInstance() {
        static ErrorLogger instance;
        return instance;
    }
    
} // namespace Validation

} // namespace nlm
