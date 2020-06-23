#!/bin/bash

set -e
install_dir="$(git rev-parse --show-toplevel)/deps"

pushd "${BUILD_DIR}"
if [[ ! -d "dd-opentracing-cpp" ]]; then
	git clone https://github.com/DataDog/dd-opentracing-cpp.git
fi
cd dd-opentracing-cpp
git checkout "v1.1.4"
if [[ ! -d .build ]]; then
	mkdir .build
fi
cd .build
cmake -DCMAKE_PREFIX_PATH="$install_dir" \
      -DCMAKE_INSTALL_PREFIX="$install_dir" \
      -DDEPS_PATH="$install_dir" \
      -DCMAKE_CXX_FLAGS="-fPIC" \
      -DBUILD_STATIC=ON \
      -DBUILD_SHARED=OFF \
      -DBUILD_TESTING=OFF \
      ..
make && make install
popd
