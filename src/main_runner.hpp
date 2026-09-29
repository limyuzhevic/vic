#pragma once

#include <memory>
#include <string>
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "main_config.hpp"
#include "main_brain.hpp"
#include "main_tests.hpp"

namespace nlm {

class MainRunner {
public:
    static int run(int argc, char** argv);
    static void runAllTests(std::shared_ptr<Brain> brain);
};

} // namespace nlm