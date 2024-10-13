#/bin/bash

set -eu
script_path="$(realpath $0)"
root="$(dirname $script_path)"

pushd "$(dirname $0)"

compiler=${CC:-g++}

src_files=(
	src/main.cxx
	src/util/random.cxx
	src/layout/layout_tree.cxx
	src/rendering.cxx
	src/rendering_context.cxx
)

rapidxml_version="1.13"
rapidxml_flags="-I$root/vendor/rapidxml-$rapidxml_version"

skia_include="$root/vendor/skia"
skia_link="-L$root/vendor/skia/out/Static -lskia"
skia_flags="$skia_link -I$skia_include -DSK_GL -DSK_GANESH"

glfw_flags=$(pkg-config --cflags --libs glfw3)
gl_flags=$(pkg-config --cflags --libs gl)

misc_lib_flags=$(pkg-config --cflags --libs libwebp libwebpmux libwebpdemux libpng libjpeg fontconfig)

common_opts="$rapidxml_flags $skia_flags $glfw_flags $gl_flags -I$root/src -Wall --std=c++20 -lz $misc_lib_flags"
debug_opts="--debug -g --optimize -DDEBUG $common_opts"

mkdir -p build
pushd build >> /dev/null

all_src=""
for p in "${src_files[@]}"; do
	all_src+=" ../${p}"
done

compile="$compiler $all_src -o clef $debug_opts"

echo $compile
$compile

popd >> /dev/null
popd >> /dev/null

