#!/bin/bash

set -e
cpu_cores=$(grep -c ^processor /proc/cpuinfo)
job_count=$(expr $cpu_cores - 2)
sdk_path=$(echo ${PICO_SDK_PATH})

if [[ $job_count -lt 1 ]]; then
    job_count=1
fi

case "$1" in
    "--analyze")
        cmake -B build -S .
        cmake --build build --parallel $job_count
        run-clang-tidy -p build -quiet -export-fixes build/clang-tidy-fixes.yaml
        ;;
    "--build")
        cmake -B build  -S . -DPICO_SDK_PATH=$sdk_path "${@:2}"
        cmake --build build --parallel $job_count
        ;;
    "--clean")
        rm -rf build
        ;;
    *)
        echo "Usage: build.bash [OPTION]"
        echo "  --analyze       Analyzes the C/C++ code in the project"
        echo "  --build         Builds the project without testing"
        echo "  --clean         Cleans all project files"
        exit 1
        ;;
esac
