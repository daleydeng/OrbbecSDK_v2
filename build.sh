# GMSL must be on or error
cmake .. \
        -DCMAKE_INSTALL_PREFIX=install \
	-DOB_BUILD_EXAMPLES=OFF \
        -DOB_BUILD_DOCS=OFF \
        -DOB_BUILD_NET_PAL=OFF \
	-DOB_BUILD_GMSL_PAL=ON \
        -DOB_INSTALL_EXAMPLES_SOURCE=OFF \
        -DCMAKE_CXX_FLAGS=-std=c++11
