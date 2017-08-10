FROM phcerdan/qt5:latest

ENV VTK_VERSION=8.0
ARG DEPS_DIR=/opt/deps

# For nvidia you will require others
RUN apt-get update && apt-get -y install \
    libgl1-mesa-dev \
    libxt-dev

RUN wget -c http://www.vtk.org/files/release/8.0/VTK-8.0.0.zip && \
    unzip -qq VTK-8.0.0.zip && \
    rm VTK-8.0.0.zip && \
    cd VTK* && \
    mkdir build && cd build && \
    cmake \
     -DVTK_QT_VERSION:STRING=5 \
     -DVTK_Group_Qt:BOOL=ON \
     -DCMAKE_BUILD_TYPE:STRING=Release \
     -DCMAKE_INSTALL_PREFIX:PATH=${DEPS_DIR} \
     .. && \
    make -j$(nproc) && \
    make install && \
    cd ../.. && rm -r VTK*

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
