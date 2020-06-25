#!/bin/bash

set -e

export OPENTRACING_VERSION=1.5.1
NGINX_VERSIONS=(1.19.0 1.18.0 1.17.{10..4} 1.16.1 1.15.12)

# Compile for a portable cpu architecture
export CFLAGS="-march=x86-64 -fPIC"
export CXXFLAGS="-march=x86-64 -fPIC"
export LDFLAGS="-fPIC"

./ci/build_static_opentracing.sh

for NGINX_VERSION in ${NGINX_VERSIONS[*]}
do
  ./ci/build_module_binary.sh $NGINX_VERSION
done
