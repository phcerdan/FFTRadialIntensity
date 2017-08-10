FROM phcerdan/itk:latest

#  Boost
RUN apt-get -y install \
    libboost-system-dev \
    libboost-program-options-dev \
    libboost-filesystem-dev

ARG DEPS_DIR=/opt/deps
ARG APP_DIR=/opt/app
COPY . repo
RUN cd repo && mkdir build && cd build && \
    echo ${DEPS_DIR} && \
    cmake \
     -DITK_DIR=${DEPS_DIR}/lib/cmake/ITK-${ITK_VERSION} \
     -DCMAKE_INSTALL_PREFIX={APP_DIR} \
     .. && \
     make -j$(nproc) && \
     make install

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

