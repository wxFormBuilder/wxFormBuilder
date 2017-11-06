#!/bin/bash
set -eux

rpm --import https://getfedora.org/static/9DB62FB1.txt
dnf install -y flatpak-builder git wget
wget https://kojipkgs.fedoraproject.org//packages/flatpak-builder/0.10.4/1.fc28/x86_64/flatpak-builder-0.10.4-1.fc28.x86_64.rpm
dnf install -y ./flatpak-builder*.rpm
flatpak remote-add --from gnome https://sdk.gnome.org/gnome.flatpakrepo
flatpak install gnome org.freedesktop.Platform 1.6
flatpak install gnome org.freedesktop.Sdk 1.6
flatpak-builder --repo=repo ./build org.wxformbuilder.wxFormBuilder.json
flatpak build-bundle repo wxformbuilder.flatpak org.wxformbuilder.wxFormBuilder
