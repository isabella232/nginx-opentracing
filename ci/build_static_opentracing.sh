#!/bin/bash

set -e
install_dir="$(git rev-parse --show-toplevel)/deps"

pushd "${BUILD_DIR}"
if [[ ! -d "opentracing-cpp" ]]; then
	git clone https://github.com/opentracing/opentracing-cpp.git
fi
cd opentracing-cpp
git checkout "v1.5.1"
if [[ ! -d .build ]]; then
	mkdir .build
fi
cd .build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX="$install_dir" \
      -DCMAKE_CXX_FLAGS="-fPIC" \
      -DBUILD_SHARED_LIBS=OFF \
      -DBUILD_TESTING=OFF \
      -DBUILD_MOCKTRACER=OFF \
      ..
make && make install
popd
