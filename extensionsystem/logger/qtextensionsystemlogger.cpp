#include "qtextensionsystemlogger.h"
#include "qtextensionsystemconstants.h"

#include <QMutex>
#include <QSettings>
#include <QThread>
#include <QDateTime>
#include <QCoreApplication>
#include <QDir>

#include <iostream>

namespace QtExtensionSystem{
    namespace Logger {

        class QtExtensionSystemLoggerPrivate
        {
            Q_DISABLE_COPY(QtExtensionSystemLoggerPrivate)
            Q_DECLARE_PUBLIC(QtExtensionSystemLogger)

        public:

            QtExtensionSystemLoggerPrivate(QtExtensionSystemLogger* q);
            virtual~QtExtensionSystemLoggerPrivate();

            void init();

            static void call_MessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

            QtExtensionSystemLogger*const               q_ptr;
            bool                                        _isMsgFilters;
            QtExtensionSystemLogger::LoggerFormater*    _loggerFormater;
            QMutex                                      _mutex;
        };

        QtExtensionSystemLoggerPrivate::QtExtensionSystemLoggerPrivate(QtExtensionSystemLogger *q)
            : q_ptr(q)
        {
            Q_ASSERT(q);
        }

        QtExtensionSystemLoggerPrivate::~QtExtensionSystemLoggerPrivate()
        {
            if(_loggerFormater)
                delete _loggerFormater;
        }

        void QtExtensionSystemLoggerPrivate::init()
        {
            _isMsgFilters = false;
            _loggerFormater = new QtExtensionSystemLogger::LoggerFormater(QDir::cleanPath(QCoreApplication::applicationDirPath() + "/ESLogCfg.ini"));
        }

        void QtExtensionSystemLoggerPrivate::call_MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
        {
            const QString format = QtExtensionSystemLogger::inst()->loggerFormater()->formater(type,context,msg);
            emit QtExtensionSystemLogger::inst()->logger_format(type,format);
#ifdef LOGGER_WRITE
            QtExtensionSystemLogger::inst()->loggerFormater()->write(format);
#else
            std::cout<<format.toUtf8().data()<<std::endl;
#endif
            if(QtFatalMsg == type)
                abort();
        }

        QtExtensionSystemLogger::QtExtensionSystemLogger(QObject *parent)
            : QObject(parent)
            , d_ptr(new QtExtensionSystemLoggerPrivate(this))
        {
            d_func()->init();
        }

        QtExtensionSystemLogger::~QtExtensionSystemLogger()
        {

        }

        Q_GLOBAL_STATIC(QtExtensionSystemLogger,_logger_inst_)
        QtExtensionSystemLogger *QtExtensionSystemLogger::inst()
        {
            return _logger_inst_;
        }

        void QtExtensionSystemLogger::installingMessageFilters()
        {
            Q_D(QtExtensionSystemLogger);
            d->_isMsgFilters = true;
            qInstallMessageHandler(QtExtensionSystemLoggerPrivate::call_MessageHandler);
        }

        void QtExtensionSystemLogger::unInstallingMessageFilters()
        {
            Q_D(QtExtensionSystemLogger);
            d->_isMsgFilters = false;
            qInstallMessageHandler(Q_NULLPTR);
        }

        bool QtExtensionSystemLogger::isMsgFilters() const
        {
            Q_D(const QtExtensionSystemLogger);
            return d->_isMsgFilters;
        }

        void QtExtensionSystemLogger::setLoggerFormater(QtExtensionSystemLogger::LoggerFormater *format)
        {
            Q_D(QtExtensionSystemLogger);
            QMutexLocker lock(&d->_mutex);
            d->_loggerFormater = format;
        }

        QtExtensionSystemLogger::LoggerFormater *QtExtensionSystemLogger::loggerFormater()
        {
            Q_D(QtExtensionSystemLogger);
            QMutexLocker lock(&d->_mutex);
            return d->_loggerFormater;
        }

        namespace {

        static const char* _logger_Timer="%Timer%";
        static const char* _logger_Level="%Level%";
        static const char* _logger_Thread="%Thread%";
        static const char* _logger_Func="%Func%";
        static const char* _logger_File="%File%";
        static const char* _logger_Line="%Line%";
        static const char* _logger_Msg="%Message%";
        static const char* _logger_Number="%Number%";

        }

#define FORMATER_FORMATER(f,rep)\
         if(format.contains(f))\
            format = format.replace(f,rep)

        QString QtExtensionSystemLogger::LoggerFormater::formater(QtMsgType type, const QMessageLogContext &context, const QString &msg) const
        {
            if(type < _information.filter)
                return "";

            QString format = _information.formater;

            FORMATER_FORMATER(_logger_Timer,QString(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz"))));

            if(format.contains(_logger_Level))
            {
                switch (type) {
                case QtDebugMsg:
                    format = format.replace(_logger_Level,QStringLiteral("Debug"));
                    break;
                case QtWarningMsg:
                    format = format.replace(_logger_Level,QStringLiteral("Warning"));
                    break;
                case QtInfoMsg:
                    format = format.replace(_logger_Level,QStringLiteral("Info"));
                    break;
                case QtCriticalMsg:
                    format = format.replace(_logger_Level,QStringLiteral("Critical"));
                    break;
                case QtFatalMsg:
                    format = format.replace(_logger_Level,QStringLiteral("Fatal"));
                    break;
                default:break;
                }
            }

            FORMATER_FORMATER(_logger_Thread,QString("0x%1").arg(quintptr(QThread::currentThreadId()), 8, 16, QLatin1Char('0')));

            QString str = context.function;
            str.remove(0,str.indexOf("_cdecl ") + 7);
            str.remove(str.lastIndexOf("("),str.size() - str.lastIndexOf("(") + 1);

            FORMATER_FORMATER(_logger_Func,str);

            str = context.file;
            str.remove(0,str.lastIndexOf(QDir::separator(),str.lastIndexOf(QDir::separator()) - 1) +  1);

            FORMATER_FORMATER(_logger_File,str);

            FORMATER_FORMATER(_logger_Line,QString::number(context.line));

            FORMATER_FORMATER(_logger_Msg,msg);


            return format;
        }

        void QtExtensionSystemLogger::LoggerFormater::write(const QString &msg)
        {
            static QTextStream os;

            if(msg.isEmpty())
                return;

            QMutexLocker locker(&_mutex);

            if(_number > _information.maxFileNumber)
                _number = 0;

            if(!_file.isOpen())
            {
                QString format = _information.fileName;
                FORMATER_FORMATER(_logger_Timer,QDateTime::currentDateTime().toString(QStringLiteral("yyyy_MM_dd_")));
                FORMATER_FORMATER(_logger_Number,QString::number(_number++));
                _file.setFileName(QDir::cleanPath(QCoreApplication::applicationDirPath() + _information.target + format));
                if(_file.open(QFile::WriteOnly | QFile::Text))
                    os.setDevice(&_file);
            }

            if(_file.size() >= _information.fileSize)
            {
                os.flush();
                _file.close();
                locker.unlock();
                write(msg);
                return;
            }

            os<<msg<<endl;

            if(!_information.autoFlush)
                os.flush();
        }

        void QtExtensionSystemLogger::LoggerFormater::analysisConfigureInformation(const QString &configure)
        {
            QSettings settings(configure,QSettings::IniFormat);
            settings.beginGroup(QStringLiteral("Logger"));
            _information.filter = QtExtensionSystemLogger::LoggerFormater::FormaterInformationen::fromMsgType(settings.value(QStringLiteral("Filter")).toString());
            _information.formater = settings.value(QStringLiteral("Formater")).toString();
            _information.autoFlush = settings.value(QStringLiteral("AutoFlush")).toBool();
            _information.fileSize = settings.value(QStringLiteral("FileSize")).toInt();
            _information.fileName = settings.value(QStringLiteral("FileName")).toString();
            _information.target = settings.value(QStringLiteral("Target")).toString();
            _information.maxFileNumber = settings.value(QStringLiteral("MaxFileNumber")).toInt();
            settings.endGroup();

            _number = 0;
#ifdef LOGGER_WRITE
            QDir logger(QDir::cleanPath(QCoreApplication::applicationDirPath() + _information.target));
            if(!logger.exists())
                logger.mkpath(logger.absolutePath());
#endif
        }

    }
}
