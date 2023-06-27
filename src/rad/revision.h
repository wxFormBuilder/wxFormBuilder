#ifndef RAD_REVISION_H
#define RAD_REVISION_H

#include <string>


const char* getRevision();

const char* getBranch();

const char* getDescribe();

const char* getTagName();

unsigned int getTagDistance();

bool isDirty();


std::string getPostfixRevision(const char* version);

std::string getReleaseRevision(const char* version);

std::string getReleaseBranch(const char* branch);

#endif  // RAD_REVISION_H
