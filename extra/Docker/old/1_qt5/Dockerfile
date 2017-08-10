FROM phcerdan/base:latest

ARG DEBIAN_FRONTEND=noninteractive

# Qt5.9 ~359MB
RUN add-apt-repository ppa:beineri/opt-qt59-xenial -y && \
    apt-get update && \
    apt-get -y install \
    qt59base \
    qt59svg \
    qt59x11extras \
    qt59tools \
    qt59charts-no-lgpl

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
