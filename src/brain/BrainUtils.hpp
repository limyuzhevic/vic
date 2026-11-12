// BrainUtils.hpp - Helper functions and status reporting
#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>

namespace nlm {

class BrainUtils {
public:
    BrainUtils(Brain* brain);
    ~BrainUtils();
    
    void logStatus() const;
    
private:
    Brain* brain_;
};

} // namespace nlm
