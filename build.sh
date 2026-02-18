#!/bin/bash

    if [ ! -d "build" ]; then
    mkdir build
    cd build
    cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release -DBUILD_WITH_TCMALLOC=OFF -DBUILD_WITH_JEMALLOC=ON
    cd ..
fi

cmake --build build --config Release
