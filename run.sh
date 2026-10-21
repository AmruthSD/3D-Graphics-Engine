#!/usr/bin/env bash
set -e

# Project root directory
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
BUILD_TYPE="Release"
BUILD_ONLY=false
CLEAN_BUILD=false
EXTRA_ARGS=()

# Parse arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --build-only|-b)
            BUILD_ONLY=true
            shift
            ;;
        --help|-h)
            echo "Usage: ./run.sh [OPTIONS] [-- [APP_ARGS...]]"
            echo ""
            echo "Options:"
            echo "  --release       Build in Release mode (default)"
            echo "  --debug         Build in Debug mode"
            echo "  --clean         Remove existing build directory before building"
            echo "  --build-only, -b Compile the project without running"
            echo "  --help, -h      Show this help message"
            echo ""
            echo "Any additional arguments after '--' are forwarded to the executable."
            exit 0
            ;;
        --)
            shift
            EXTRA_ARGS+=("$@")
            break
            ;;
        *)
            EXTRA_ARGS+=("$1")
            shift
            ;;
    esac
done

cd "$PROJECT_ROOT"

if [ "$CLEAN_BUILD" = true ] && [ -d "$BUILD_DIR" ]; then
    echo "==> Cleaning build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

# Configure if build directory doesn't exist or CMakeCache.txt is missing
if [ ! -f "${BUILD_DIR}/CMakeCache.txt" ]; then
    echo "==> Configuring CMake ($BUILD_TYPE)..."
    cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
fi

# Determine number of CPU threads for build
NUM_JOBS=$(nproc 2>/dev/null || echo 4)

echo "==> Building target (using $NUM_JOBS threads)..."
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" -j "$NUM_JOBS"

if [ "$BUILD_ONLY" = true ]; then
    echo "==> Build complete."
    exit 0
fi

EXECUTABLE="${BUILD_DIR}/3d_graphics_engine"
if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Executable not found at $EXECUTABLE" >&2
    exit 1
fi

echo "==> Running 3d_graphics_engine..."
# Run from the build directory so relative paths (../assets, ../shaders) resolve correctly
cd "$BUILD_DIR"
exec ./3d_graphics_engine "${EXTRA_ARGS[@]}"

