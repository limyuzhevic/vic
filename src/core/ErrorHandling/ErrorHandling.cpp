#include "ErrorHandling.hpp"
#include <iostream>

namespace nlm {

void safeExecute(const std::string& operation, std::function<void()> func,
                 IErrorHandler& handler) {
    try {
        func();
    } catch (const NLMError& e) {
        handler.handleError(e);
        if (handler.shouldAbortOnError(e)) {
            throw;
        }
    } catch (const std::exception& e) {
        handler.handleError(std::string("Unexpected error in " + operation + ": " + std::string(e.what())));
        if (handler.shouldAbortOnError(NLMError("Unexpected error"))) {
            throw;
        }
    }
}

} // namespace nlm
