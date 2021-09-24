#pragma once

#ifdef WXFB_VERSION_GENERATE
const char* getRevision();

const char* getStrippedRevision(const char* version);

const char* getReleaseRevision(const char* version);
#else
inline const char* getRevision() { return ""; }

inline const char* getStrippedRevision(const char* version) { return ""; }

inline const char* getReleaseRevision(const char* version) { return ""; }
#endif
