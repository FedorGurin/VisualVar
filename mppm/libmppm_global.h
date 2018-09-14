#ifndef LIBMPPM_GLOBAL_H
#define LIBMPPM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBMPPM_LIBRARY)
#  define LIBMPPMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBMPPMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBMPPM_GLOBAL_H
