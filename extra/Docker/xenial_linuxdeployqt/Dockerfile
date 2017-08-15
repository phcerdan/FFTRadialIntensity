FROM phcerdan/radial_intensity_app:latest

ARG APP_DIR=/opt/app

# linuxdeployqt deps (NOT FOR DOCKER)
# RUN apt-get -y install \
#         fuse

RUN cd /opt
RUN wget -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" && \
    chmod a+x linuxdeployqt*.AppImage
RUN unset QTDIR; unset QT_PLUGIN_PATH; unset LD_LIBRARY_PATH
# qmake is simlinked to qtchooser!
ENV QT_SELECT=5
# For lconvert
RUN apt-get install -y \
    qttools5-dev-tools \
    patchelf

RUN ./linuxdeployqt*.AppImage --appimage-extract
# RUN echo "extern int is_selinux_enabled(void){return 0;}" >> selinux-mock.c && \
#     gcc -s -shared -o libselinux.so.1 -Wl,-soname,libselinux.so.1 selinux-mock.c && \
#     rm selinux-mock.c && \
#     mv libselinux.so.1 \
#     ${APP_DIR}/usr/lib/
RUN cp /lib/x86_64-linux-gnu/libselinux.so.1 ${APP_DIR}/usr/lib/ && \
    patchelf --set-rpath '$ORIGIN' ${APP_DIR}/usr/lib/libselinux.so.1
RUN ./squashfs-root/AppRun .${APP_DIR}/usr/share/applications/*.desktop -bundle-non-qt-libs
RUN ./squashfs-root/AppRun .${APP_DIR}/usr/share/applications/*.desktop -appimage
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


