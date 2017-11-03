#!/bin/bash
set -eux

dnf install -y flatpak-builder git
flatpak remote-add --from gnome https://sdk.gnome.org/gnome.flatpakrepo
flatpak install gnome org.freedesktop.Platform 1.6
flatpak install gnome org.freedesktop.Sdk 1.6

export FLATPAK_OSTREE_REPO_MODE=user-only # See https://github.com/flatpak/flatpak-builder/issues/57
flatpak-builder --repo=repo ./build org.wxformbuilder.wxFormBuilder.json
flatpak build-bundle repo wxformbuilder.flatpak org.wxformbuilder.wxFormBuilder
