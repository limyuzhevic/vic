#include "BrainAccessor.hpp"
#include "LoggerHelper.hpp"
#include <iostream>

namespace nlm {

// NeuralSystemAccessor implementation

template<typename T>
void NeuralSystemAccessor::registerNeuralSystem(const std::string& systemName,
                                                std::shared_ptr<T> systemPtr) {
    systems_[systemName] = std::static_pointer_cast<void>(systemPtr);
}

// BrainAccessor implementation

template<typename T>
T BrainAccessor::getProperty(const std::string& key, const T& defaultValue) {
    // In a real implementation, this would access configuration
    LoggerHelper::warning("Property access not implemented for key: " + key);
    return defaultValue;
}

// Singleton instance for BrainAccessor
template<typename T>
T getBrainProperty(const std::string& key, const T& defaultValue) {
    return BrainAccessor::getProperty<T>(key, defaultValue);
}

} // namespace nlm
