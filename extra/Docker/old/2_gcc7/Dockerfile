FROM phcerdan/qt5:latest

# Modern Compiler (C++14)
RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
    apt-get update && \
    apt-get -y install \
    g++-7 \
    unzip

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
