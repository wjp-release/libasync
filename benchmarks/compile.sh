#!/bin/bash
cd build
rm -rf *
cmake -GNinja -DCMAKE_PREFIX_PATH=/usr/local/tbb/ ..
ninja