#! /usr/bin/env bash
# Cleanup script to remove the duplicate python/CMakeLists.txt
# This file was creating conflicting targets with the root CMakeLists.txt

rm -f python/CMakeLists.txt

echo "Cleaned up duplicate python/CMakeLists.txt"
