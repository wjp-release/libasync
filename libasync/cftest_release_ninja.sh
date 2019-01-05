#!/bin/bash
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -Dcftest=ON ..
ninja