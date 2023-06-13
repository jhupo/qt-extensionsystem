#ifndef __QT_EXTENSIONSYSTEM_LOGGER_H__
#define __QT_EXTENSIONSYSTEM_LOGGER_H__

#include "extensionsystem_global.h"

#include <QDebug>
#include <QFile>

class QMutex;

namespace QtExtensionSystem {
    namespace Logger {

        class QtExtensionSystemLoggerPrivate;

        class EXTENSIONSYSTEM_EXPORT QtExtensionSystemLogger : public QObject
        {
            Q_OBJECT
            Q_DISABLE_COPY(QtExtensionSystemLogger)
            Q_DECLARE_PRIVATE(QtExtensionSystemLogger)

        public:

            QtExtensionSystemLogger(QObject* parent = Q_NULLPTR);
            virtual~QtExtensionSystemLogger();

            static QtExtensionSystemLogger* inst();

            void installingMessageFilters();

            void unInstallingMessageFilters();

            bool isMsgFilters()const;

            struct LoggerFormater{
#pragma pack(1)
                struct FormaterInformationen
                {
                    QtMsgType               filter;
                    QString                 formater;
                    bool                    autoFlush;
                    int                     fileSize;
                    QString                 fileName;
                    QString                 target;
                    int                     maxFileNumber;
                    static QtMsgType fromMsgType(const QString& type)
                    {
#define MSG_TYPE(type,msg,level)\
            if(type == msg)\
                return level;
                        MSG_TYPE(type,"Debug",QtDebugMsg);
                        MSG_TYPE(type,"Info",QtInfoMsg);
                        MSG_TYPE(type,"Warning",QtWarningMsg);
                        MSG_TYPE(type,"Critical",QtCriticalMsg);
                        MSG_TYPE(type,"Fatal",QtFatalMsg);
                        return QtDebugMsg;
                    }
                };
#pragma pack()
                LoggerFormater(const QString& configure){analysisConfigureInformation(configure);}
                virtual ~LoggerFormater(){}
                virtual QString formater(QtMsgType type, const QMessageLogContext& context, const QString& msg)const;
                virtual void write(const QString& msg);
                FormaterInformationen                                   _information;
            protected:
                void analysisConfigureInformation(const QString& configure);
                QFile                                                     _file;
                int                                                       _number;
                QMutex                                                    _mutex;
            };

            void setLoggerFormater(LoggerFormater* format);
            LoggerFormater* loggerFormater();

        Q_SIGNALS:

            void logger_format(QtMsgType,const QString&);

        protected:

            const QScopedPointer<QtExtensionSystemLoggerPrivate>            d_ptr;
            friend struct LoggerFormater;
        };

    }
}
Q_DECLARE_METATYPE(QtMsgType)


#endif
