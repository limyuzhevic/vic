#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Internal signals class
class InternalSignals : public SensoryInput {
public:
    InternalSignals();
    ~InternalSignals() override;
    
    // Disable copying, enable moving
    InternalSignals(const InternalSignals&) = delete;
    InternalSignals& operator=(const InternalSignals&) = delete;
    InternalSignals(InternalSignals&& other) noexcept;
    InternalSignals& operator=(InternalSignals&& other) noexcept;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Add signal
    void addSignal(float value);
    void clearSignals();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
