#ifndef __HELLOWORD_GLOBAL_H__
#define __HELLOWORD_GLOBAL_H__

#include <QObject>
#include <QtGlobal>

#if defined(HELLOWORD_LIBRARY)
#  define HELLOWORD_EXPORT Q_DECL_EXPORT
#elif defined(HELLOWORD_STATIC_LIBRARY)
#  define HELLOWORD_EXPORT
#else
#  define HELLOWORD_EXPORT Q_DECL_IMPORT
#endif


#endif
