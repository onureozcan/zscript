#!/bin/bash
# build
mkdir -p /build \
    && cd /build \
    && cmake /Zscript \
    && make \
    && echo "testing..." \
    && echo "file list:" \
    && ls -ll ../test_scripts/ \
    && echo "running tester" \
    && ./tester
cp /build/zero /Zscript/cmake-build-debug
