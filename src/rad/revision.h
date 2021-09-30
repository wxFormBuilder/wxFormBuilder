#pragma once

#include <string>

#ifdef WXFB_VERSION_GENERATE
const char* getRevision();

const char* getDescribe();

const char* getTagName();

unsigned int getTagDistance();

bool isDirty();


std::string getPostfixRevision(const char* version);

std::string getReleaseRevision(const char* version);
#else
inline const char* getRevision() { return ""; }

inline const char* getDescribe() { return ""; }

inline const char* getTagName() { return  ""; }

inline unsigned int getTagDistance() { return 0; }

inline bool isDirty() { return false; }


inline std::string getPostfixRevision(const char* version) { return ""; }

inline std::string getReleaseRevision(const char* version) { return ""; }
#endif
