#
# Parameters
#

# Name of the docker executable
DOCKER = docker

# Docker organization to pull the images from
ORG = phcerdan

# On CircleCI, do not attempt to delete container
# See https://circleci.com/docs/docker-btrfs-error/
RM = --rm
ifeq ("$(CIRCLECI)", "true")
	RM =
endif

app:
	$(DOCKER) build -t $(ORG)/radial_intensity_app \
		--build-arg IMAGE=$(ORG)/radial_intensity_app \
		--build-arg VCS_URL=`git config --get remote.origin.url` \
		.
.PHONY: app

