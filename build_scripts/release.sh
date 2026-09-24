#!/bin/bash

# release.sh - NLM Release Script
# Create and manage releases for NLM project

set -euo pipefail

# Default configuration
VERSION_FILE="pyproject.toml"
CHANGELOG_FILE="CHANGELOG.md"
GIT_REMOTE="origin"
GITHUB_RELEASE=true
DOCKER_HUB_RELEASE=false
NPM_RELEASE=false
VERSION_BUMP_LEVEL=""
PACKAGE_MANAGER="pip"
DIST_DIR="dist"
ARTIFACTS_DIR="artifacts"
CREATE_DOCKER=true
CREATE_PYTHON_PACKAGE=true
CREATE_JAVA_PACKAGE=false
CREATE_NODEJS_PACKAGE=false

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Print usage
usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Create and manage releases for NLM project.

Options:
  --version VERSION             Version to release (default: auto-detect from pyproject.toml)
  --bump-level LEVEL           Version bump level (major, minor, patch)
  --no-docker                  Skip Docker image creation
  --no-python                  Skip Python package creation
  --no-java                    Skip Java package creation
  --no-nodejs                  Skip Node.js package creation
  --github-token TOKEN         GitHub API token (for GitHub releases)
  --docker-username USER       Docker Hub username (for Docker releases)
  --docker-password PASS       Docker Hub password (for Docker releases)
  --npm-token TOKEN            npm API token (for Node.js releases)
  --dist-dir DIR               Distribution directory (default: dist)
  --artifacts-dir DIR          Artifacts directory (default: artifacts)
  --confirm                    Skip confirmation prompts
  -h, --help                   Show this help message

Examples:
  $0                                          # Create release from current version
  $0 --bump-level minor                        # Bump minor version and create release
  $0 --github-token $GITHUB_TOKEN             # Create GitHub release with token
  $0 --docker-username $DOCKER_USER --docker-password $DOCKER_PASS  # Create Docker release
  $0 --confirm                                 # Skip confirmation and proceed
EOF
}

# Get current version
get_current_version() {
    if [[ -f "$VERSION_FILE" ]]; then
        local version=$(grep -E "^version\s*=" "$VERSION_FILE" | head -1 | cut -d'"' -f2)
        if [[ -n "$version" ]]; then
            echo "$version"
            return
        fi
    fi
    
    if [[ -f "Cargo.toml" ]]; then
        local version=$(grep -E "^version\s*=" "Cargo.toml" | head -1 | cut -d'"' -f2)
        if [[ -n "$version" ]]; then
            echo "$version"
            return
        fi
    fi
    
    if [[ -f "package.json" ]]; then
        local version=$(grep -E "^version\s*:" "package.json" | head -1 | sed 's/[^0-9.]//g')
        if [[ -n "$version" ]]; then
            echo "$version"
            return
        fi
    fi
    
    log_error "Could not determine current version from version files"
    exit 1
}

# Bump version
bump_version() {
    local level="$1"
    local current_version="$(get_current_version)"
    
    log_info "Bumping version from $current_version (level: $level)"
    
    # Parse current version
    local major=$(echo "$current_version" | cut -d'.' -f1)
    local minor=$(echo "$current_version" | cut -d'.' -f2)
    local patch=$(echo "$current_version" | cut -d'.' -f3)
    
    case "$level" in
        "major")
            major=$((major + 1))
            minor=0
            patch=0
            ;;
        "minor")
            minor=$((minor + 1))
            patch=0
            ;;
        "patch")
            patch=$((patch + 1))
            ;;
        *)
            log_error "Invalid version bump level: $level. Use major, minor, or patch."
            exit 1
            ;;
    esac
    
    local new_version="${major}.${minor}.${patch}"
    
    # Update version in files
    if [[ -f "$VERSION_FILE" && "$VERSION_FILE" == *.toml ]]; then
        sed -i "s/^version\s*=.*$/version = \"$new_version\"/" "$VERSION_FILE"
        log_info "Updated $VERSION_FILE to version $new_version"
    elif [[ -f "pyproject.toml" ]]; then
        sed -i "s/^version\s*=.*$/version = \"$new_version\"/" pyproject.toml
        log_info "Updated pyproject.toml to version $new_version"
    elif [[ -f "Cargo.toml" ]]; then
        sed -i "s/^version\s*=.*$/version = \"$new_version\"/" Cargo.toml
        log_info "Updated Cargo.toml to version $new_version"
    elif [[ -f "package.json" ]]; then
        sed -i "s/^  \"version\": .*/  \"version\": \"$new_version\",/" package.json
        log_info "Updated package.json to version $new_version"
    else
        log_error "No version file found to update"
        exit 1
    fi
    
    # Update CHANGELOG if available
    if [[ -f "$CHANGELOG_FILE" ]]; then
        local today=$(date +%Y-%m-%d)
        sed -i "0i ## [$new_version] - $today\n\n### Added\n\n### Changed\n\n### Fixed\n" "$CHANGELOG_FILE"
        log_info "Updated $CHANGELOG_FILE with new release notes"
    fi
    
    echo "$new_version"
}

# Update version
update_version() {
    local version="$1"
    
    log_info "Setting version to $version"
    
    # Update version in files
    if [[ -f "pyproject.toml" ]]; then
        sed -i "s/^version\s*=.*$/version = \"$version\"/" pyproject.toml
        log_info "Updated pyproject.toml to version $version"
    elif [[ -f "Cargo.toml" ]]; then
        sed -i "s/^version\s*=.*$/version = \"$version\"/" Cargo.toml
        log_info "Updated Cargo.toml to version $version"
    elif [[ -f "package.json" ]]; then
        sed -i "s/^  \"version\": .*/  \"version\": \"$version\",/" package.json
        log_info "Updated package.json to version $version"
    else
        log_error "No version file found to update"
        exit 1
    fi
}

# Build distributions
build_distributions() {
    local version="$1"
    
    log_info "Building distributions for version $version..."
    
    # Build Python package
    if [[ "$CREATE_PYTHON_PACKAGE" == true ]]; then
        log_info "Building Python package..."
        pip install --no-cache-dir build
        python -m build
        log_success "Python package built"
    fi
    
    # Build Docker image
    if [[ "$CREATE_DOCKER" == true ]]; then
        log_info "Building Docker image..."
        docker build -t nlm:$version .
        docker tag nlm:$version nlm:latest
        log_success "Docker image built"
    fi
}

# Create GitHub release
create_github_release() {
    local version="$1"
    local token="$2"
    
    if [[ -z "$token" ]]; then
        log_warning "GitHub token not provided. GitHub releases will not be created."
        return 0
    fi
    
    log_info "Creating GitHub release for version $version..."
    
    # Create release notes
    local release_notes="NLM $version Release"
    if [[ -f "$CHANGELOG_FILE" ]]; then
        release_notes="$(head -n 50 "$CHANGELOG_FILE" | sed 's/^[[:space:]]*//;s/^# \*\*\*//')"
    fi
    
    # Create GitHub release
    curl -X POST \
        -H "Authorization: token $token" \
        -H "Accept: application/vnd.github.v3+json" \
        "https://api.github.com/repos/$(git remote get-url $GIT_REMOTE | sed 's/.*github.com:\/\/\|.*/')/releases" \
        -d "{
            \"tag_name\": \"v$version\",
            \"target_committish\": "$(git rev-parse HEAD)",
            \"name\": \"NLM v$version\",
            \"body\": "$release_notes",
            \"draft\": false,
            \"prerelease\": false
        }" > /tmp/github_release_response.json
    
    local response_code=$(cat /tmp/github_release_response.json | grep -o '"code": [0-9]*' | cut -d' ' -f2 || echo "0")
    
    if [[ $response_code -eq 201 ]]; then
        log_success "GitHub release created successfully"
        local upload_url=$(cat /tmp/github_release_response.json | grep -o '"upload_url": "[^"]*"' | cut -d'"' -f4)
        echo "$upload_url"
    else
        log_error "Failed to create GitHub release"
        return 1
    fi
}

# Upload artifacts to GitHub release
upload_artifacts_to_github() {
    local upload_url="$1"
    local version="$2"
    
    log_info "Uploading artifacts to GitHub release..."
    
    # Upload built distributions
    if [[ -d "$DIST_DIR" ]]; then
        for artifact in "$DIST_DIR"/*; do
            if [[ -f "$artifact" ]]; then
                local filename=$(basename "$artifact")
                curl -X POST \
                    -H "Authorization: token $GITHUB_TOKEN" \
                    -H "Accept: application/vnd.github.v3+json" \
                    "$upload_url?name=$filename" \
                    -F "file=@$artifact"
                
                if [[ $? -eq 0 ]]; then
                    log_success "Uploaded $filename to GitHub release"
                else
                    log_warning "Failed to upload $filename to GitHub release"
                fi
            fi
        done
    fi
    
    # Upload artifacts directory
    if [[ -d "$ARTIFACTS_DIR" ]]; then
        for artifact in "$ARTIFACTS_DIR"/*; do
            if [[ -f "$artifact" ]]; then
                local filename=$(basename "$artifact")
                curl -X POST \
                    -H "Authorization: token $GITHUB_TOKEN" \
                    -H "Accept: application/vnd.github.v3+json" \
                    "$upload_url?name=$filename" \
                    -F "file=@$artifact"
                
                if [[ $? -eq 0 ]]; then
                    log_success "Uploaded $filename to GitHub release"
                else
                    log_warning "Failed to upload $filename to GitHub release"
                fi
            fi
        done
    fi
}

# Create Docker Hub release
deploy_docker_hub_release() {
    local version="$1"
    local username="$2"
    local password="$3"
    
    if [[ -z "$username" || -z "$password" ]]; then
        log_warning "Docker Hub credentials not provided. Docker Hub releases will not be created."
        return 0
    fi
    
    log_info "Deploying Docker image to Docker Hub..."
    
    # Login to Docker Hub
    echo "$password" | docker login "$username" --username "$username" --password-stdin
    
    # Push Docker image
    docker push "$username/nlm:$version"
    docker push "$username/nlm:latest"
    
    log_success "Docker image pushed to Docker Hub"
}

# Create npm package release
deploy_npm_release() {
    local version="$1"
    local token="$2"
    
    if [[ -z "$token" ]]; then
        log_warning "npm token not provided. npm releases will not be created."
        return 0
    fi
    
    log_info "Creating npm package release for version $version..."
    
    # Login to npm
    echo "$token" | npm login --scope=https://registry.npmjs.org/ --password -
    
    # Publish package
    npm publish
    
    log_success "Package published to npm"
}

# Create checksums
create_checksums() {
    log_info "Creating checksums..."
    
    # Create checksums for distribution files
    if [[ -d "$DIST_DIR" ]]; then
        cd "$DIST_DIR"
        sha256sum * > checksums.sha256
        md5sum * > checksums.md5
        cd ..
        
        log_success "Checksums created in $DIST_DIR/checksums.sha256 and $DIST_DIR/checksums.md5"
    fi
    
    # Create checksums for artifacts
    if [[ -d "$ARTIFACTS_DIR" ]]; then
        cd "$ARTIFACTS_DIR"
        sha256sum * > checksums.sha256
        md5sum * > checksums.md5
        cd ..
        
        log_success "Checksums created in $ARTIFACTS_DIR/checksums.sha256 and $ARTIFACTS_DIR/checksums.md5"
    fi
}

# Generate release notes
generate_release_notes() {
    local version="$1"
    
    log_info "Generating release notes..."
    
    # Create release notes template
    cat > "RELEASE_NOTES.md" << EOF
# NLM Release Notes

## Version $version

Release Date: $(date +%Y-%m-%d)

### Changes

<!-- Add release notes here -->

### Dependencies

- scikit-build-core >=0.5.0
- pybind11 >=2.11.0
- numpy >=1.20
- pytest >=7.0

### Build

- CMake 3.16 or higher
- C++20 compatible compiler
- Python 3.8 or higher

### Testing

- Unit tests
- Integration tests
- Performance benchmarks
- Code coverage

### Known Issues

<!-- List known issues for this release -->

### Upgrading

To upgrade to this version:

```bash
# Python bindings
pip install pynlm==$version

# From source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Installation

See the documentation for detailed installation instructions.
EOF
    
    log_success "Release notes generated in RELEASE_NOTES.md"
}

# Main release function
main() {
    log_info "Starting NLM release process..."
    
    # Determine version
    local version="$1"
    if [[ -z "$version" ]]; then
        if [[ -n "$VERSION_BUMP_LEVEL" ]]; then
            version="$(bump_version "$VERSION_BUMP_LEVEL")"
        else
            version="$(get_current_version)"
        fi
    fi
    
    log_info "Release version: $version"
    
    # Confirm release
    if [[ "$SKIP_CONFIRM" != "true" ]]; then
        echo "This will create a release for version $version"
        echo "Are you sure you want to continue? (y/N)"
        read -r answer
        if [[ "$answer" != "y" && "$answer" != "Y" ]]; then
            log_info "Release cancelled by user"
            exit 0
        fi
    fi
    
    # Update version if needed
    update_version "$version"
    
    # Build distributions
    build_distributions "$version"
    
    # Create checksums
    create_checksums
    
    # Generate release notes
    generate_release_notes "$version"
    
    # Create GitHub release
    if [[ "$GITHUB_RELEASE" == true ]]; then
        local upload_url="$(create_github_release "$version" "$GITHUB_TOKEN")"
        if [[ -n "$upload_url" ]]; then
            upload_artifacts_to_github "$upload_url" "$version"
        fi
    fi
    
    # Create Docker Hub release
    if [[ "$CREATE_DOCKER" == true && -n "$DOCKER_USERNAME" && -n "$DOCKER_PASSWORD" ]]; then
        deploy_docker_hub_release "$version" "$DOCKER_USERNAME" "$DOCKER_PASSWORD"
    fi
    
    # Create npm package release
    if [[ "$NPM_RELEASE" == true && -n "$NPM_TOKEN" ]]; then
        deploy_npm_release "$version" "$NPM_TOKEN"
    fi
    
    log_success "Release process completed successfully!"
    log_info "Release version: $version"
    log_info "Distribution files: $DIST_DIR/"
    log_info "Artifacts: $ARTIFACTS_DIR/"
    log_info "Release notes: RELEASE_NOTES.md"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --version)
            VERSION="$2"
            shift 2
            ;;
        --bump-level)
            VERSION_BUMP_LEVEL="$2"
            shift 2
            ;;
        --no-docker)
            CREATE_DOCKER=false
            shift
            ;;
        --no-python)
            CREATE_PYTHON_PACKAGE=false
            shift
            ;;
        --no-java)
            CREATE_JAVA_PACKAGE=false
            shift
            ;;
        --no-nodejs)
            CREATE_NODEJS_PACKAGE=false
            shift
            ;;
        --github-token)
            GITHUB_TOKEN="$2"
            shift 2
            ;;
        --docker-username)
            DOCKER_USERNAME="$2"
            shift 2
            ;;
        --docker-password)
            DOCKER_PASSWORD="$2"
            shift 2
            ;;
        --npm-token)
            NPM_TOKEN="$2"
            shift 2
            ;;
        --dist-dir)
            DIST_DIR="$2"
            shift 2
            ;;
        --artifacts-dir)
            ARTIFACTS_DIR="$2"
            shift 2
            ;;
        --confirm)
            SKIP_CONFIRM=true
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Run main function
main "$@"