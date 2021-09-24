#pragma once

#ifdef WXFB_VERSION_GENERATE
const char* getVersion();
#else
inline const char* getVersion() { return "3.9.0"; }
#endif
