FROM phcerdan/xenial_deps:latest
#CMAKE
RUN wget -c "https://cmake.org/files/v3.9/cmake-3.9.0-Linux-x86_64.sh" && \
    sh ./cmake-3.9.0-Linux-x86_64.sh --prefix=/usr/local --skip-license && \
    rm cmake-3.9.0-Linux-x86_64.sh

#ITK4: From source
RUN apt-get -y install \
      libfftw3-dev


RUN wget -c "https://sourceforge.net/projects/itk/files/itk/4.12/InsightToolkit-4.12.0.tar.gz/download" && \
    tar -xf download && \
    rm download && \
    cd Insight* && \
    cd Modules/Remote && \
    wget -c "https://raw.githubusercontent.com/InsightSoftwareConsortium/ITK/master/Modules/Remote/IsotropicWavelets.remote.cmake" && \
    cd ../.. && \
    mkdir build && cd build && \
    cmake \
      -DITK_USE_FFTW:BOOL=ON \
      -DITK_USE_FFTWD:BOOL=ON \
      -DITK_USE_FFTWF:BOOL=ON \
      -DModule_IsotropicWavelets:BOOL=ON \
      -DModule_ITKVtkGlue:BOOL=ON \
      -DVTK_DIR:PATH=/opt/VTK-${VTK_VERSION}/lib/cmake/vtk-${VTK_VERSION} \
      -DBUILD_TESTING:BOOL=OFF \
      -DCMAKE_BUILD_TYPE:STRING=Release \
      -DCMAKE_INSTALL_PREFIX:PATH=/opt \
      .. && \
    make -j$(nproc) && \
    make install && \
    cd ../.. && rm -r Insight*

ENV ITK_VERSION=4.12
