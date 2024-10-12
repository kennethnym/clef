#/bin/bash

set -eu

script_path="$(realpath $0)"
root="$(dirname $script_path)"
pushd "$(dirname "$0")" > /dev/null

glfw_include=$(pkg-config --cflags glfw3)
gl_include=$(pkg-config --cflags gl)

cat > .clangd <<- EOF
CompileFlags:
  Add:
    - -DSK_VULKAN
    - -I$root/vendor/rapidxml-1.13
    - -I$root/vendor/skia
    - -I$root/src
    - $glfw_include
    - $gl_include
EOF
