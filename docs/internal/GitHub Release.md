# GitHub Release

The GitHub release process is semi-automated, the GitHub Actions CI produces the artifacts but the release post
is created manually. This document describes the steps necessary to perform a release.

The version number is incremented on release, preferably the commit that changes the version number is the actual
release commit. The release commit must be tagged with the release version number. Since the GitHub workflows that
produce the artifacts only trigger on regular commits it is important to push that commit together with the tag,
otherwise the workflows need to be triggered again manually after the tag got pushed.

1. Verify that the wxFormBuilder Project File version is current. `ApplicationData::ApplicationData(const wxString&)`
   defined in `src/rad/appdata.cpp` should contain a comment if a version number increase is necessary.
   Perform this in a separate commit if necessary.
2. Update the version number in the main `CMakeLists.txt` file in the root directory
3. Update the legacy fallback version number in `src/rad/version.h`
4. Add the version number and release timestamp to the AppStream metadata file `install/linux/data/gnome/usr/share/appdata/org.wxformbuilder.wxformbuilder.appdata.xml`
5. Add the version number and release timestamp to the Debian changelog file `install/linux/deb/changelog`
6. Add the version number and release timestamp together with a link to the future GitHub release post in `output/Changelog.txt`
7. Commit the changes
8. Tag the commit. The tag name must be the version number prefixed with the letter `v`. To make the tag annotated,
   add a simple message like `Release`.
9. Push the commit together with the tag: `git push --follow-tags`
10. Download all created artifacts and extract them. Not the downloaded zip archives but their content gets added to the release post.
    Rename the Flatpak file to contain the architecture tag like its archive, all other filenames are already final.
11. On the GitHub website create a release for the tag. Highlight the most important changes, upload all artifacts.
12. Publish the release
