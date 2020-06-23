#!/bin/bash

set -e
install_dir="$(git rev-parse --show-toplevel)/deps"

[ -z "${OPENTRACING_VERSION}" ] && export OPENTRACING_VERSION="v1.5.1"

# Build OpenTracing
if [[ ! -d "opentracing-cpp" ]]; then
	git clone https://github.com/opentracing/opentracing-cpp.git
fi
cd opentracing-cpp
git checkout "v$OPENTRACING_VERSION"
if [[ ! -d .build ]]; then
	mkdir .build
fi
cd .build
cmake \
  -DCMAKE_INSTALL_PREFIX="$install_dir" \
  -DBUILD_MOCKTRACER=OFF \
  -DBUILD_TESTING=OFF \
  ..
make && make install
