#
# Parameters
#

# Name of the docker executable
DOCKER = docker

# Docker organization to pull the images from
ORG = phcerdan

# # Directory where to generate the dockcross script for each images (e.g bin/dockcross-manylinux-x64)
# BIN = ./bin

# This list all available images
IMAGES = 0_base 1_qt5 3_vtk 4_itk

# On CircleCI, do not attempt to delete container
# See https://circleci.com/docs/docker-btrfs-error/
RM = --rm
ifeq ("$(CIRCLECI)", "true")
	RM =
endif

#
# images: This target builds all IMAGES (because it is the first one, it is built by default)
#
images: $(IMAGES)

0_base:
	$(DOCKER) build -t $(ORG)/base \
		--build-arg IMAGE=$(ORG)/base \
		--build-arg VCS_URL=`git config --get remote.origin.url` \
		./0_base

1_qt5:
	$(DOCKER) build -t $(ORG)/qt5 \
		--build-arg IMAGE=$(ORG)/qt5 \
		--build-arg VCS_URL=`git config --get remote.origin.url` \
		./$@

3_vtk:
	$(DOCKER) build -t $(ORG)/vtk \
		--build-arg IMAGE=$(ORG)/vtk \
		--build-arg VCS_URL=`git config --get remote.origin.url` \
		./$@

4_itk:
	$(DOCKER) build -t $(ORG)/itk \
		--build-arg IMAGE=$(ORG)/itk \
		--build-arg VCS_URL=`git config --get remote.origin.url` \
		./$@

.PHONY: $(IMAGES)
