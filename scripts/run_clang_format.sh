#!/bin/sh
find opentracing/ \( -name '*.h' -or -name '*.cpp' \) -exec clang-format-9 -i {} \;
