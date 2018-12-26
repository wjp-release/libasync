#!/bin/bash
cd local_build
rm -rf *
cmake -GNinja -DCMAKE_PREFIX_PATH=/usr/local/tbb/ -DCMAKE_BUILD_TYPE=Release ..
ninja