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
)

rapidxml_version="1.13"
rapidxml_flags="-I$root/vendor/rapidxml-$rapidxml_version"

skia_include="$root/vendor/skia"
skia_link="-L$root/vendor/skia/out/Static/libaudioplayer.a\
	-L$root/vendor/skia/out/Static/libbentleyottmann.a\
	-L$root/vendor/skia/out/Static/libcompression_utils_portable.a\
	-L$root/vendor/skia/out/Static/libperfetto.a\
	-L$root/vendor/skia/out/Static/libdng_sdk.a\
	-L$root/vendor/skia/out/Static/libpathkit.a\
	-L$root/vendor/skia/out/Static/libpiex.a\
	-L$root/vendor/skia/out/Static/libskcms.a\
	-L$root/vendor/skia/out/Static/libskottie.a\
	-L$root/vendor/skia/out/Static/libskparagraph.a\
	-L$root/vendor/skia/out/Static/libskresources.a\
	-L$root/vendor/skia/out/Static/libsksg.a\
	-L$root/vendor/skia/out/Static/libskshaper.a\
	-L$root/vendor/skia/out/Static/libskunicode_core.a\
	-L$root/vendor/skia/out/Static/libskunicode_icu.a\
	-L$root/vendor/skia/out/Static/libsvg.a\
	-L$root/vendor/skia/out/Static/libwuffs.a\
	-L$root/vendor/skia/out/Static/libskia.a"
skia_flags="-I$skia_include -DSK_GL -DSK_GANESH"

glfw_flags=$(pkg-config --cflags --libs glfw3)
gl_flags=$(pkg-config --cflags --libs gl)

misc_lib_flags=$(pkg-config --cflags --libs libwebp libwebpmux libwebpdemux libpng libjpeg fontconfig)

common_opts="$rapidxml_flags $skia_flags $glfw_flags $gl_flags -I$root/src -Wall --std=c++20 -v -L$root/vendor/skia/out/Static -lskia -lz $misc_lib_flags"
debug_opts="--debug --optimize -DDEBUG $common_opts"

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

