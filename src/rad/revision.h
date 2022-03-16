#ifndef RAD_REVISION_H
#define RAD_REVISION_H

#include <string>


const char* getRevision();

const char* getDescribe();

const char* getTagName();

unsigned int getTagDistance();

bool isDirty();


std::string getPostfixRevision(const char* version);

std::string getReleaseRevision(const char* version);

#endif  // RAD_REVISION_H
