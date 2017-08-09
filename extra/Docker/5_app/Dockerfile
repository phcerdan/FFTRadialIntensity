FROM phcerdan/itk:latest

#  Boost
RUN apt-get -y install \
    libboost-system-dev \
    libboost-program-options-dev \
    libboost-filesystem-dev

ARG DEPS_DIR=/opt/deps

RUN wget -c "https://sourceforge.net/projects/itk/files/itk/4.12/InsightToolkit-4.12.0.tar.gz/download" && \
    tar -xf download && \
    cd Insight* && \
    mkdir build && cd build && \
    cmake \
    -DITK_USE_FFTW:BOOL=ON \
    -DITK_USE_FFTWD:BOOL=ON \
    -DITK_USE_FFTWF:BOOL=ON \
    -DITK_MODULE_IsotropicWavelets:BOOL=ON \
    -DVTK_DIR:PATH=${DEPS_DIR}/lib/cmake/vtk-${VTK_VERSION} \
    -DITK_MODULE_ITKVtkGlue:BOOL=ON \
    -DBUILD_TESTING:BOOL=OFF \
    -DCMAKE_BUILD_TYPE:STRING=Release \
    -DCMAKE_INSTALL_PREFIX:PATH=${DEPS_DIR} \
    .. && \
    make -j$(nproc) && \
    make install
RUN rm ../download

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
