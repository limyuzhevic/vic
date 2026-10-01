#pragma once

#include <string>

namespace nlm {
namespace tests {

// Config system tests
int runConfigTests();

// Test result counters
int getConfigPassedCount();
int getConfigFailedCount();

} // namespace tests
} // namespace nlm