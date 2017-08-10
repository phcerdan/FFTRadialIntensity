FROM phcerdan/xenial_itk:latest

#  Boost
RUN apt-get -y install \
    libboost-system-dev \
    libboost-program-options-dev \
    libboost-filesystem-dev

RUN apt-get -y install \
    libqt5svg5-dev

ARG APP_DIR=/opt/app
COPY . repo
RUN cd repo && mkdir build && cd build && \
    echo ${DEPS_DIR} && \
    cmake \
     -DENABLE_QT:BOOL=ON \
     -DITK_DIR=/opt/lib/cmake/ITK-${ITK_VERSION} \
     -DCMAKE_INSTALL_PREFIX=/usr \
     .. && \
     make -j$(nproc) && \
     make DESTDIR=/opt/app install ; find /opt/app

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

