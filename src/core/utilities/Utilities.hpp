#pragma once

#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Motor Command utilities
const char* motorCommandToString(MotorCommand cmd);

// Developmental stage utilities  
const char* developmentalStageToString(DevelopmentalStage stage);

// Configuration utilities
std::shared_ptr<Config> createDefaultConfig(const std::string& experimentType = "general");

// Error handling utilities
bool safeFileOperation(const std::string& path, std::function<void()> operation, const std::string& errorMsg = "");

template<typename T>
bool safeInitialize(T& object, bool (T::*init)(), const std::string& errorMsg = "Initialization failed");

// Configuration validation
bool validateConfig(const std::shared_ptr<Config>& config);

// Performance utilities
void enableDebugOutput(bool enabled);
bool isDebugOutputEnabled();

} // namespace nlm
