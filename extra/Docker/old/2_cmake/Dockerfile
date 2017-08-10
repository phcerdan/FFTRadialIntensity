FROM phcerdan/qt5:latest

# CMAKE
RUN wget -c "https://cmake.org/files/v3.9/cmake-3.9.0-Linux-x86_64.sh" && \
    sh ./cmake-3.9.0-Linux-x86_64.sh --prefix=/usr/local --skip-license && \
    rm cmake-3.9.0-Linux-x86_64.sh

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
