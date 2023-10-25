#ifndef __EXTENSIONSYSTEM_GLOBAL_H__
#define __EXTENSIONSYSTEM_GLOBAL_H__

#include <QObject>
#include <QtGlobal>
#include <QLoggingCategory>

#if defined(EXTENSIONSYSTEM_LIBRARY)
#  define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
#elif defined(EXTENSIONSYSTEM_STATIC_LIBRARY)
#  define EXTENSIONSYSTEM_EXPORT
#else
#  define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#endif
#if (QT_VERSION < 0x050500)
#undef qCInfo
#define qCInfo qCCritical
#endif
Q_DECLARE_LOGGING_CATEGORY(logger_extension)
Q_DECLARE_LOGGING_CATEGORY(logger_event)
#endif
