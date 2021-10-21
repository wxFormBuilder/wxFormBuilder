#!/bin/bash
set -eux

FLATHUB=https://flathub.org/repo/flathub.flatpakrepo

# needed in github-ci
if [ `id -u` == 0 ]; then
  command -v dnf && { dnf upgrade -y ; dnf install -y flatpak-builder git wget; }
fi

NOTFOUND=""
command -v flatpak         >/dev/null 2>&1 || NOTFOUND="$NOTFOUND flatpak"
command -v flatpak-builder >/dev/null 2>&1 || NOTFOUND="$NOTFOUND flatpak-builder"
command -v git             >/dev/null 2>&1 || NOTFOUND="$NOTFOUND git"
command -v wget            >/dev/null 2>&1 || NOTFOUND="$NOTFOUND wget"
test -z "$NOTFOUND" || { echo "$NOTFOUND: not found, but needed. please install. Aborting. "; exit 1; }

flatpak --user remote-add --if-not-exists flathub $FLATHUB
flatpak --user install -y flathub org.freedesktop.Platform 21.08
flatpak --user install -y flathub org.freedesktop.Sdk 21.08
flatpak-builder --force-clean --repo=repo ./build org.wxformbuilder.wxFormBuilder.json
flatpak build-bundle repo wxFormBuilder.flatpak org.wxformbuilder.wxFormBuilder --runtime-repo=$FLATHUB
