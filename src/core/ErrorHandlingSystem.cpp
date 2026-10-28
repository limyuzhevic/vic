#include "ErrorHandlingSystem.hpp"

namespace nlm {

void ErrorManager::initialize() {
    addErrorHandler(std::make_shared<ConsoleErrorHandler>());
}

} // namespace nlm
