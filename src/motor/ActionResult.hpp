#pragma once

#include <string>

namespace nlm {

// Action result from the world's perspective
// Represents the outcome of applying a motor command to the environment
struct ActionResult {
    float reward;
    bool success;
    std::string message;
    
    ActionResult() : reward(0.0f), success(false), message("") {}
    ActionResult(float r, bool s, const std::string& m = "") 
        : reward(r), success(s), message(m) {}
};

} // namespace nlm