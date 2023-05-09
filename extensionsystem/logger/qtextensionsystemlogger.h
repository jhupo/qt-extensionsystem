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
                    enum Destination{Console,TextFile,NetWork};
                    enum Permission{Write,Append};
                    bool                    disable;
                    QtMsgType               filter;
                    Destination             destination;
                    QString                 formater;
                    bool                    asynchronous;
                    bool                    autoFlush;
                    int                     flushTime;
                    int                     fileSize;
                    Permission              permisson;
                    QString                 fileName;
                    QString                 target;
                    int                     maxFileNumber;
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

        protected:

            const QScopedPointer<QtExtensionSystemLoggerPrivate>            d_ptr;
            friend class LoggerFormater;
        };

    }
}



#endif
