# Name of the docker executable
DOCKER = docker

# Docker organization to pull the images from
ORG = phcerdan

IMAGES = xenial_linuxdeployqt

images: $(IMAGES)

xenial_linuxdeployqt:
	$(DOCKER) build -t $(ORG)/radial_intensity_linuxdeployqt \
		--build-arg IMAGE=$(ORG)/radial_intensity_linuxdeployqt \
		--build-arg VCS_URL=`git config --get remote.origin.url` \
		.

.PHONY: $(IMAGES)
