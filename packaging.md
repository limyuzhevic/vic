# NLM Packaging Documentation

## Overview

This document provides comprehensive information about NLM packaging, distribution, and deployment strategies. It covers CPack configuration, package formats, component installation, and deployment best practices.

## Table of Contents

1. [CPack Configuration](packaging.md#cpack-configuration)
2. [Package Formats](packaging.md#package-formats)
3. [Package Components](packaging.md#package-components)
4. [Cross-Platform Packaging](packaging.md#cross-platform-packaging)
5. [Package Metadata](packaging.md#package-metadata)
6. [Package Dependencies](packaging.md#package-dependencies)
7. [Package Installation](packaging.md#package-installation)
8. [Package Verification](packaging.md#package-verification)
9. [Package Maintenance](packaging.md#package-maintenance)
10. [Package Examples](packaging.md#package-examples)

## CPack Configuration

### Introduction

CPack (CMake Package Manager) is a cross-platform packaging tool integrated into CMake. It provides a consistent interface for creating platform-specific package formats from CMake projects.

### CPack Integration

NLM uses CPack with comprehensive configuration for multiple package formats:

```cmake
include(CPack)
```

### Basic CPack Settings

```cmake
# Package metadata
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "Neural Learning Machine - An Experimental Artificial Developmental Brain")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NLM Neural Simulation Framework")
set(CPACK_PACKAGE_CONTACT "contact@nlm.org")
set(CPACK_PACKAGE_VENDOR "NLM Project")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/nlm-project/nlm")
```

### Component-Based Packaging

NLM uses component-based packaging for better control over package contents:

```cmake
include(CPackComponent)

# Core library component
cpack_component(core
    DISPLAY_NAME "Core Library"
    DESCRIPTION "Core neural simulation library"
    REQUIRED
    GROUP "Runtime"
)

# Python bindings component
cpack_component(python_bindings
    DISPLAY_NAME "Python Bindings"
    DESCRIPTION "Python bindings for NLM"
    OPTIONAL
    GROUP "Runtime"
)

# Documentation component
cpack_component(documentation
    DISPLAY_NAME "Documentation"
    DESCRIPTION "Documentation and examples"
    OPTIONAL
    GROUP "Runtime"
)
```

### Setting Component Dependencies

```cmake
# Set component dependencies
cp_pack_component(comp_core)
cp_pack_component(comp_python_bindings)
cp_pack_component(comp_documentation)
```

### Adding Targets to Components

```cmake
# Add core library targets to component
cp_pack_add_target(comp_core nlm_core nlm_agent nlm_world)
cp_pack_add_target(comp_core nlm nlm_phase3_demo nlm_phase4_demo)

# Add Python bindings targets to component
cp_pack_add_target(comp_python_bindings pynlm)
```

### Installation Directories

```cmake
# Install library files
install(TARGETS nlm_core nlm_agent nlm_world
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install executables
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ${NLM_SOURCE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)
```

## Package Formats

### DEB Packages (Debian/Ubuntu)

#### Configuration

```cmake
if(CPACK_GENERATOR MATCHES "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Developers <contact@nlm.org>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "science")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libgomp1")
    set(CPACK_DEBIAN_PACKAGE_REPLACES "nlm (< 0.1.0)")
    set(CPACK_DEBIAN_PACKAGE_PROVIDES "nlm")
endif()
```

#### Building DEB Packages

```bash
# Create build directory
mkdir build && cd build

# Configure with DEB generator
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"

# Build packages
cpack --config Release/Deb

# Alternative: Use CMake's built-in DEB support
cmake -DCPACK_GENERATOR=DEB ..
cmake --build . --target package
```

#### Package Contents

A typical DEB package structure:

```
/usr/lib/libnlm_core.so.0.1.0
/usr/lib/libnlm_agent.so.0.1.0
/usr/lib/libnlm_world.so.0.1.0
/usr/bin/nlm
/usr/bin/nlm_phase3_demo
/usr/bin/nlm_phase4_demo
/usr/include/nlm/
    Types.hpp
    Config.hpp
    ...
/usr/lib/pkgconfig/nlm.pc
/usr/share/doc/nlm/
    copyright
    changelog.Debian.gz
    README.Debian
/usr/share/man/man1/nlm.1.gz
```

### RPM Packages (Fedora/CentOS/RHEL)

#### Configuration

```cmake
if(CPACK_GENERATOR MATCHES "RPM")
    set(CPACK_RPM_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
    set(CPACK_RPM_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")
    set(CPACK_RPM_PACKAGE_RELEASE "1%{?dist}")
    set(CPACK_RPM_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_VENDOR "${CPACK_PACKAGE_VENDOR}")
    set(CPACK_RPM_PACKAGE_CONTACT "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_RPM_PACKAGE_URL "${CPACK_PACKAGE_HOMEPAGE_URL}")
    
    set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
    set(CPACK_RPM_PACKAGE_REQUIRES "glibc, libstdc++, libgomp")
    set(CPACK_RPM_PACKAGE_PROVIDES "nlm")
    set(CPACK_RPM_PACKAGE_OBSOLETES "nlm < 0.1.0")
    
    set(CPACK_RPM_SPEC_INSTALL "")
    set(CPACK_RPM_SPEC_POST_INSTALL "")
    set(CPACK_RPM_SPEC_POST_UNINSTALL "")
endif()
```

#### Building RPM Packages

```bash
# Create build directory
mkdir build && cd build

# Configure with RPM generator
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"

# Build packages
cpack --config Release/RPM

# Alternative: Use CMake's built-in RPM support
cmake -DCPACK_GENERATOR=RPM ..
cmake --build . --target package
```

#### Package Contents

A typical RPM package structure:

```
%{_libdir}/libnlm_core.so.0.1.0
%{_libdir}/libnlm_agent.so.0.1.0
%{_libdir}/libnlm_world.so.0.1.0
%{_bindir}/nlm
%{_bindir}/nlm_phase3_demo
%{_bindir}/nlm_phase4_demo
%{_includedir}/nlm/
    Types.hpp
    Config.hpp
    ...
%{_libdir}/pkgconfig/nlm.pc
%{_docdir}/nlm/
    copyright
    changelog
    README
```

### TGZ Archives (Tarballs)

#### Configuration

```cmake
if(CPACK_GENERATOR MATCHES "TGZ")
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_COMPRESSION_TYPE "BZIP2")
    set(CPACK_TGZ_FORMAT "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-%{BUILD_NUMBER}-%{OS}.tar.gz")
endif()
```

#### Building TGZ Archives

```bash
# Create build directory
mkdir build && cd build

# Configure with TGZ generator
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"

# Build packages
cpack --config Release/TGZ

# Alternative: Use CMake's built-in TGZ support
cmake -DCPACK_GENERATOR=TGZ ..
cmake --build . --target package
```

#### Package Contents

A typical TGZ archive structure:

```
nlm-${version}/
    README.md
    LICENSE
    CHANGELOG.md
    docs/
    lib/
        libnlm_core.so.0.1.0
        libnlm_agent.so.0.1.0
        libnlm_world.so.0.1.0
    bin/
        nlm
        nlm_phase3_demo
        nlm_phase4_demo
    include/nlm/
        Types.hpp
        Config.hpp
        ...
    share/doc/nlm/
        copyright
        changelog
    python/
        pynlm*
        setup.py
    cmake/
        NLMConfig.cmake
        NLMConfigVersion.cmake
```

### ZIP Archives (Windows/MSYS2)

#### Configuration

```cmake
if(CPACK_GENERATOR MATCHES "ZIP")
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_ARCHIVE_FORMAT "ZIP")
    set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
endif()
```

#### Building ZIP Archives

```bash
# Create build directory
mkdir build && cd build

# Configure with ZIP generator
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"

# Build packages
cpack --config Release/ZIP

# Alternative: Use CMake's built-in ZIP support
cmake -DCPACK_GENERATOR=ZIP ..
cmake --build . --target package
```

### NSIS Installers (Windows)

#### Configuration

```cmake
if(CPACK_GENERATOR MATCHES "NSIS")
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_DESCRIPTION}")
    set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/docs/favicon.ico")
    set(CPACK_NSIS_MUI_UNIWFCREATE_DEFAULTINSTRUCTIONSDISABLED ON)
    set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_DESCRIPTION}")
    set(CPACK_NSIS_HELP_LINK "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_NSIS_URL_INFO_ABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")
    
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "")
    set(CPACK_NSIS_DELETE_ICONS_EXTRA "")
    set(CPACK_NSIS_MENU_GROUP "NLM")
    set(CPACK_NSIS_INSTALLED_ICON_PATH "\\${NSIS_EXECUTABLES\\}nlm.exe")
    set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_NSIS_LOGO "\\${CMAKE_CURRENT_SOURCE_DIR}\\docs\\logo.bmp")
endif()
```

#### Building NSIS Installers

```bash
# Create build directory
mkdir build && cd build

# Configure with NSIS generator (Windows)
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" -A x64

# Build packages
cpack --config Release/NSIS

# Alternative: Use CMake's built-in NSIS support
cmake -DCPACK_GENERATOR=NSIS ..
cmake --build . --target package
```

#### Package Contents

A typical NSIS installer structure:

```
Program Files\\NLM\\
    bin\\
        nlm.exe
        nlm_phase3_demo.exe
        nlm_phase4_demo.exe
    lib\\
        libnlm_core.dll
        libnlm_agent.dll
        libnlm_world.dll
    include\\nlm\\
        Types.hpp
        Config.hpp
        ...
    share\\doc\\nlm\\
        README.txt
        LICENSE.txt
    Python\\
        pynlm*.pyd
        pynlm\\
            __init__.py
            ...
    CMake\\
        NLMConfig.cmake
        NLMConfigVersion.cmake
```

## Package Components

### Component Structure

NLM uses component-based packaging to allow users to install only the features they need:

#### Core Component

The core component includes:

- **Libraries:** Static and shared libraries
- **Executables:** Main NLM programs
- **Headers:** Development headers
- **Documentation:** Basic documentation

#### Python Bindings Component

The Python bindings component includes:

- **Python Module:** pynlm Python bindings
- **Python Packages:** Python package structure
- **Python Documentation:** Python-specific documentation
- **Python Dependencies:** Required Python packages

#### Documentation Component

The documentation component includes:

- **Build Documentation:** Build system documentation
- **API Documentation:** Complete API reference
- **Tutorial Documentation:** Step-by-step tutorials
- **Examples:** Sample code and examples

### Component Installation

#### Component Installation Commands

```bash
# Install all components (default)
cmake --build . --target install

# Install only core component
cmake --build . --target install COMPONENT=core

# Install core and Python bindings
cmake --build . --target install COMPONENT=core,python_bindings

# Install all components
cmake --build . --target install COMPONENT=core,python_bindings,documentation
```

#### Component Configuration

```cmake
# Core component configuration
cp_pack_add_target(comp_core nlm_core nlm_agent nlm_world)
install(TARGETS nlm nlm_core nlm_agent nlm_world
    COMPONENT core
)

# Python bindings component configuration
cp_pack_add_target(comp_python_bindings pynlm)
install(TARGETS pynlm
    COMPONENT python_bindings
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${Python_SITEARCH}/pynlm
    COMPONENT python_bindings
)

# Documentation component configuration
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs/
    DESTINATION share/doc/nlm
    COMPONENT documentation
)
```

## Cross-Platform Packaging

### Platform Detection

NLM automatically detects the target platform and configures CPack accordingly:

```cmake
# Detect platform
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
    include(CPackDEB)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
    include(CPackTGZ)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_PACKAGING_INSTALL_PREFIX "C:/Program Files/${CPACK_PACKAGE_NAME}")
    include(CPackNSIS)
endif()
```

### Architecture Detection

```cmake
# Detect architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
else()
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "i686")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
endif()
```

### Distribution-Specific Configuration

#### Debian/Ubuntu

```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT CMAKE_CROSSCOMPILING)
    # Check if we're building for Debian/Ubuntu
    execute_process(
        COMMAND lsb_release -si
        OUTPUT_VARIABLE LSB_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    if(LSB_ID STREQUAL "Ubuntu" OR LSB_ID STREQUAL "Debian")
        include(CPackDEB)
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Developers <contact@nlm.org>")
        set(CPACK_DEBIAN_PACKAGE_SECTION "science")
    endif()
endif()
```

#### Fedora/CentOS/RHEL

```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT CMAKE_CROSSCOMPILING)
    # Check if we're building for Fedora/CentOS/RHEL
    execute_process(
        COMMAND cat /etc/redhat-release
        OUTPUT_VARIABLE REDHAT_RELEASE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    if(REDHAT_RELEASE MATCHES "Fedora" OR REDHAT_RELEASE MATCHES "CentOS" OR REDHAT_RELEASE MATCHES "RHEL")
        include(CPackRPM)
        set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
    endif()
endif()
```

#### macOS

```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    include(CPackTGZ)
    set(CPACK_DMG_FORMAT "Apple_HFS")
    set(CPACK_GENERATOR "DragNDropTGZ")
endif()
```

#### Windows

```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    include(CPackNSIS)
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_DESCRIPTION}")
    set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
endif()
```

## Package Metadata

### Versioning

NLM uses semantic versioning:

```cmake
set(PROJECT_VERSION "0.1.0")
```

**Version components:**

- **Major version:** Breaking changes
- **Minor version:** New features
- **Patch version:** Bug fixes

### Package Description

```cmake
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "Neural Learning Machine - An Experimental Artificial Developmental Brain")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NLM Neural Simulation Framework")
set(CPACK_PACKAGE_CONTACT "contact@nlm.org")
set(CPACK_PACKAGE_VENDOR "NLM Project")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/nlm-project/nlm")
```

### License Information

```cmake
set(CPACK_PACKAGE_LICENSE "MIT")
set(CPACK_PACKAGE_COPYRIGHT "Copyright (c) 2026 NLM Project")
```

### Dependencies

#### Runtime Dependencies

```cmake
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libgomp1")
set(CPACK_RPM_PACKAGE_REQUIRES "glibc, libstdc++, libgomp")
```

#### Optional Dependencies

```cmake
# Python dependencies
find_package(Python REQUIRED COMPONENTS Interpreter Development)
if(Python_FOUND)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, python3")
    set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES}, python3")
endif()
```

### Package URLs

```cmake
# Package URLs for distribution
set(CPACK_PACKAGE_URL "${CPACK_PACKAGE_HOMEPAGE_URL}/releases/download/v${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_ARCHITECTURE}")
```

## Package Dependencies

### Runtime Dependencies

NLM has the following runtime dependencies:

#### Core Dependencies

- **libc6** (Linux) / **MSVCRT** (Windows) - C standard library
- **libstdc++6** (Linux) / **MSVCRT** (Windows) - C++ standard library
- **libgomp1** (Linux) - OpenMP runtime

#### Optional Dependencies

- **Python 3.8+** - For Python bindings
- **libpython3-dev** (Linux) - Python development headers
- **Python3** (macOS/Windows) - Python interpreter

### Development Dependencies

NLM development dependencies include:

- **CMake 3.19+** - Build system
- **C++ compiler** - C++20 support
- **Python development headers** - Python bindings
- **pybind11** - Python bindings framework
- **Build tools** - Ninja, Make, etc.

## Package Installation

### Installation Methods

#### Package Manager Installation

```bash
# Ubuntu/Debian
cd /tmp
curl -LO https://github.com/nlm-project/nlm/releases/download/v0.1.0/nlm-0.1.0-amd64.deb
dpkg -i nlm-0.1.0-amd64.deb

# Fedora/CentOS
cd /tmp
curl -LO https://github.com/nlm-project/nlm/releases/download/v0.1.0/nlm-0.1.0.x86_64.rpm
rpm -i nlm-0.1.0.x86_64.rpm

# macOS (Homebrew)
brew tap nlm-project/nlm
brew install nlm

# Windows (Chocolatey)
choco install nlm
```

#### Manual Installation

```bash
# From TGZ archive
cd /tmp
curl -LO https://github.com/nlm-project/nlm/releases/download/v0.1.0/nlm-0.1.0.tar.gz
tar -xzf nlm-0.1.0.tar.gz
cd nlm-0.1.0
cmake --build . --target install

# From ZIP archive
cd /tmp
curl -LO https://github.com/nlm-project/nlm/releases/download/v0.1.0/nlm-0.1.0.zip
unzip nlm-0.1.0.zip
cd nlm-0.1.0
cmake --build . --target install
```

#### System Paths

After installation, files will be installed to:

- **Linux/Debian:** `/usr/local/lib`, `/usr/local/include`, `/usr/local/bin`
- **Fedora/CentOS:** `%{_libdir}`, `%{_includedir}`, `%{_bindir}`
- **macOS:** `/usr/local/lib`, `/usr/local/include`, `/usr/local/bin`
- **Windows:** `C:/Program Files/NLM/`, `C:/Program Files/NLM/include`

### Environment Variables

#### Linux/macOS

```bash
# Add NLM to PATH
export PATH="$PATH:/usr/local/bin"

# Add NLM libraries to library path
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"

# Add NLM includes to include path
export CPATH="$CPATH:/usr/local/include"
```

#### Windows

```bash
# Add NLM to PATH
set PATH=%PATH%;C:\Program Files\NLM\bin

# Add NLM libraries to library path
set LIB=%LIB%;C:\Program Files\NLM\lib

# Add NLM includes to include path
set INCLUDE=%INCLUDE%;C:\Program Files\NLM\include
```

## Package Verification

### Installation Verification

#### Check Installation

```bash
# Check if NLM is installed
nlm --version
nlm_phase3_demo --help
nlm_phase4_demo --help

# Check Python bindings
python3 -c "import pynlm; print(pynlm.__version__)"
```

#### Check Libraries

```bash
# Check library versions
ldd /usr/local/bin/nlm

# Check system dependencies
lsof -p $(pgrep nlm)
```

### Package Contents Verification

#### Verify Package Structure

```bash
# Verify Linux package structure
dpkg -L nlm-0.1.0-amd64.deb | head -20

# Verify RPM package structure
rpm -ql nlm-0.1.0.x86_64.rpm | head -20

# Verify TGZ package structure
tar -tzf nlm-0.1.0.tar.gz | head -20
```

#### Verify Component Installation

```bash
# Verify core component installation
find /usr/local -name "*nlm*" -type f | head -20

# Verify Python bindings installation
find /usr/local/lib/python* -name "*pynlm*" -type f | head -20
```

## Package Maintenance

### Package Updates

#### Updating from Package Manager

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get upgrade nlm

# Fedora/CentOS
sudo dnf update nlm

# macOS
brew update && brew upgrade nlm

# Windows
choco upgrade nlm
```

#### Manual Updates

```bash
# Download and install new version
cd /tmp
curl -LO https://github.com/nlm-project/nlm/releases/download/v0.1.1/nlm-0.1.1.tar.gz
tar -xzf nlm-0.1.1.tar.gz
cd nlm-0.1.1
cmake --build . --target install
rm -rf /tmp/nlm-0.1.0
```

### Package Configuration

#### Modify Package Configuration

```bash
# Reconfigure package generation
cd build
cmake .. -DCPACK_PACKAGE_NAME="CustomNLM" -DCPACK_PACKAGE_VERSION="1.0.0"
cmake --build . --target package
```

#### Custom Package Scripts

```cmake
# Add custom pre-build script
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pre-build.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/pre-build.sh"
    COPYONLY
)

# Add custom post-build script
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/post-build.sh.in"
    "${CMAKE_CURRENT_BINARY_DIR}/post-build.sh"
    COPYONLY
)
```

### Package Testing

#### Package Content Testing

```bash
# Create test package content
cd build
cpack --config Release/TGZ

# Extract and test
cd extracted
cat README.md
ls -la lib/
ls -la bin/
ls -la include/
```

#### Package Installation Testing

```bash
# Test package installation
cd /tmp
mkdir test-install
cd test-install

# Install package
cp ../nlm-0.1.0.tar.gz .
tar -xzf nlm-0.1.0.tar.gz

cd nlm-0.1.0
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=./install
cmake --build . --target install

# Verify installation
./install/bin/nlm --version
```

## Package Examples

### Example Package Scripts

#### build-packages.sh

```bash
#!/bin/bash
# Build all package formats for NLM

set -e

PROJECT_NAME="nlm"
PROJECT_VERSION="0.1.0"
BUILD_DIR="build-package"

# Create build directory
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with all package generators
cmake ..

# Build all packages
for generator in DEB RPM TGZ ZIP NSIS; do
    echo "Building $generator package..."
    cpack --config $generator
    echo "$generator package built successfully"
    echo "---"
done

echo "All packages built successfully!"
```

#### package-test.sh

```bash
#!/bin/bash
# Test package installation

set -e

PACKAGE_DIR="packages"
TEST_INSTALL_DIR="test-install"

# Create test installation directory
mkdir -p $TEST_INSTALL_DIR

# Test each package format
for package in $PACKAGE_DIR/*.deb; do
    echo "Testing DEB package: $package"
    sudo dpkg -i "$package"
    nlm --version
    sudo apt-get remove --purge -y nlm
    echo "DEB package test passed"
done

for package in $PACKAGE_DIR/*.rpm; do
    echo "Testing RPM package: $package"
    sudo rpm -i "$package"
    nlm --version
    sudo rpm -e nlm
    echo "RPM package test passed"
done

for package in $PACKAGE_DIR/*.tar.gz; do
    echo "Testing TGZ package: $package"
    cd $TEST_INSTALL_DIR
    tar -xzf "$package"
    cd */
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=.
    cmake --build . --target install
    cd ..
    ./install/bin/nlm --version
    cd ..
    rm -rf */
    echo "TGZ package test passed"
done

echo "All package tests passed!"
```

#### package-upload.sh

```bash
#!/bin/bash
# Upload packages to distribution repository

set -e

PACKAGE_DIR="packages"
REPO_URL="https://github.com/nlm-project/nlm/releases"

# Create GitHub release
GH_RELEASE_URL="$REPO_URL/tag/v$PROJECT_VERSION"

# Upload packages
git tag -a v$PROJECT_VERSION -m "NLM Version $PROJECT_VERSION"
git push origin v$PROJECT_VERSION

curl -X POST \
    -H "Authorization: token $GITHUB_TOKEN" \
    -H "Accept: application/vnd.github.v3+json" \
    "$REPO_URL/releases" \
    -d "{
        \"tag_name\": \"v$PROJECT_VERSION\",
        "name": "NLM v$PROJECT_VERSION",
        "body": "NLM Neural Learning Machine version $PROJECT_VERSION release.",
        "draft": false,
        "prerelease": false
    }"

# Upload each package
for package in $PACKAGE_DIR/*; do
    if [ -f "$package" ]; then
        echo "Uploading $package to $GH_RELEASE_URL..."
        curl -X POST \
            -H "Authorization: token $GITHUB_TOKEN" \
            -H "Accept: application/vnd.github.v3+json" \
            "$GH_RELEASE_URL/assets" \
            -F "file=@$package"
    fi
done

echo "All packages uploaded successfully!"
```

### Example Package Repository Structure

```
releases/
├── v0.1.0/
│   ├── nlm-0.1.0-amd64.deb
│   ├── nlm-0.1.0.x86_64.rpm
│   ├── nlm-0.1.0.tar.gz
│   ├── nlm-0.1.0.zip
│   └── nlm-0.1.0.exe
├── v0.1.1/
│   ├── nlm-0.1.1-amd64.deb
│   ├── nlm-0.1.1.x86_64.rpm
│   ├── nlm-0.1.1.tar.gz
│   ├── nlm-0.1.1.zip
│   └── nlm-0.1.1.exe
└── latest/
    └── symref → v0.1.1/
```

## Conclusion

NLM's packaging system provides comprehensive support for multiple package formats, component-based installation, and cross-platform distribution. By leveraging CPack's powerful features and following the best practices outlined in this guide, developers can efficiently create and distribute NLM packages across various platforms and distributions.

Key packaging improvements:

- **Multi-format support:** DEB, RPM, TGZ, ZIP, and NSIS formats
- **Component-based installation:** Install only needed components
- **Cross-platform compatibility:** Automatic platform detection and configuration
- **Comprehensive metadata:** Versioning, licensing, and dependency management
- **Package testing:** Automated testing and verification
- **Repository management:** GitHub release management and package distribution

These improvements make the NLM packaging system robust, user-friendly, and suitable for production deployment across various platforms and distributions.
