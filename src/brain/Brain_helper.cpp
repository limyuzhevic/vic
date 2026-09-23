#include "../core/Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

// Helper function for safe null pointer access
template<typename T>
T* assertNotNull(T* ptr, const std::string& operation) {
    if (!ptr) {
        NLM_LOG_ERROR("Null pointer detected during " + operation);
        throw std::runtime_error("Null pointer in Brain operation: " + operation);
    }
    return ptr;
}

void Brain::assertValid() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Brain is not properly initialized");
        throw std::runtime_error("Brain not initialized");
    }
}

} // namespace nlm