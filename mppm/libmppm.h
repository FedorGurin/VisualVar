#ifndef LIBMPPM_H
#define LIBMPPM_H

#include "libmppm_global.h"
#include "CommonEngineData.h"

extern "C" LIBMPPMSHARED_EXPORT IEngineData* createEngine();
extern "C" LIBMPPMSHARED_EXPORT void setPathToFileEngine(QString nameFile);


#endif // LIBMPPM_H
