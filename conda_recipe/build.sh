#!/bin/sh
rm -rf build
mkdir -p build
cd build

# gmsl must be on
cmake ${CMAKE_ARGS} -GNinja $SRC_DIR \
      -DOB_BUILD_EXAMPLES=OFF \
      -DOB_BUILD_DOCS=OFF \
      -DOB_BUILD_NET_PAL=OFF \
      -DOB_BUILD_GMSL_PAL=ON \
      -DOB_INSTALL_EXAMPLES_SOURCE=OFF \
      -DCMAKE_CXX_FLAGS=-std=c++11 \
      -DCMAKE_INSTALL_PREFIX=install

cmake --build . --config Release
cmake --build . --config Release --target install
cp -ar install/include/* $PREFIX/include/
rm -rf install/lib/extensions/depthengine
mkdir -p $PREFIX/lib/cmake
mv -f install/lib/*.cmake $PREFIX/lib/cmake/
cp -ar install/lib/* $PREFIX/lib/
