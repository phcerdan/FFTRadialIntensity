FROM phcerdan/vtk:latest

ENV ITK_VERSION=4.12
ARG DEPS_DIR=/opt/deps

# For nvidia you will require others
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
      -DVTK_DIR:PATH=${DEPS_DIR}/lib/cmake/vtk-${VTK_VERSION} \
      -DBUILD_TESTING:BOOL=OFF \
      -DCMAKE_BUILD_TYPE:STRING=Release \
      -DCMAKE_INSTALL_PREFIX:PATH=${DEPS_DIR} \
      .. && \
    make -j$(nproc) && \
    make install && \
    cd ../.. && rm -r Insight*

# Build-time metadata as defined at http://label-schema.org
ARG BUILD_DATE
ARG IMAGE
ARG VCS_REF
ARG VCS_URL
LABEL org.label-schema.build-date=$BUILD_DATE \
      org.label-schema.name=$IMAGE \
      org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.vcs-url=$VCS_URL \
      org.label-schema.schema-version="1.0"
