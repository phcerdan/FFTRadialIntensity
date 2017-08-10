FROM phcerdan/radial_intensity_app:latest

ARG APP_DIR=/opt/app

# linuxdeployqt deps (NOT FOR DOCKER)
# RUN apt-get -y install \
#         fuse

RUN wget -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" && \
    chmod a+x linuxdeployqt*.AppImage
RUN unset QTDIR; unset QT_PLUGIN_PATH; unset LD_LIBRARY_PATH
RUN ./linuxdeployqt*.AppImage --appimage-extract
RUN ./squashfs-root/AppRun .${APP_DIR}/bin/* -bundle-non-qt-libs
RUN ./squashfs-root/AppRun .${APP_DIR}/bin/* -appimage
RUN rm -rf ./squashfs-root/

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


