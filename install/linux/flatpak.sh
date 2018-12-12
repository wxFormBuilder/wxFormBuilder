#!/bin/bash
set -eux

FLATHUB=https://flathub.org/repo/flathub.flatpakrepo

dnf upgrade -y
dnf install -y flatpak-builder git wget
flatpak remote-add --if-not-exists flathub $FLATHUB
flatpak install -y flathub org.freedesktop.Platform 18.08
flatpak install -y flathub org.freedesktop.Sdk 18.08
flatpak-builder --repo=repo ./build org.wxformbuilder.wxFormBuilder.json
flatpak build-bundle repo wxformbuilder.flatpak org.wxformbuilder.wxFormBuilder --runtime-repo=$FLATHUB
