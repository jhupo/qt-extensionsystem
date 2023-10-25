#include "qtlogformater.h"

#include <iostream>

#include <QLoggingCategory>
#include <QReadWriteLock>
#include <QSettings>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QRegularExpression>

namespace LogFormater {


    class QtLogFormaterPrivate
    {
        Q_DISABLE_COPY(QtLogFormaterPrivate)
        Q_DECLARE_PUBLIC(QtLogFormater)

    public:

        typedef struct tagConfigure
        {
            typedef enum{
                TextFile,
                Console,
            }_Destination;
            _Destination destination;
            QString      rules;
            QString      pattern;
            bool         flush;
            qint64       size;
            QString      taget;
            quint32      maxFiles;
        }Configure;

        QtLogFormaterPrivate(QtLogFormater* q);
        virtual~QtLogFormaterPrivate();

        static void call_MessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

        QtLogFormater*const         q_ptr;
        static QMutex               _mutex;
        Configure                   _configure;
        quint16                     _localFileCount;
        QFile                       _file;
    };

    QMutex QtLogFormaterPrivate::_mutex;

    QtLogFormaterPrivate::QtLogFormaterPrivate(QtLogFormater *q)
        : q_ptr(q)
        , _localFileCount(0)
    {

    }

    QtLogFormaterPrivate::~QtLogFormaterPrivate()
    {

    }

    void QtLogFormaterPrivate::call_MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        QMutexLocker locker(&QtLogFormaterPrivate::_mutex);
#ifdef Q_OS_WIN
        static QRegularExpression regex("\\b[A-Za-z]:\\\\[^\\s:]+");
#else
        static QRegularExpression regex("\\b/[^\\s:]+");
#endif
        QString format = qFormatLogMessage(type,context,msg);
        QRegularExpressionMatch match = regex.match(format);
        if(match.hasMatch()){
            QString filePath = match.captured();
            QString tem = match.captured().remove(0,filePath.lastIndexOf(QDir::separator(),filePath.lastIndexOf(QDir::separator()) - 1) +  1);
            format = format.replace(filePath,tem);
        }
        emit QtLogFormater::inst()->logger_formater(type,format);
#ifdef QT_DEBUG
        std::cout<<format.toUtf8().data()<<std::endl;
#endif
        QtLogFormater::inst()->write(format);
    }

    QtLogFormater::QtLogFormater(QObject *parent)
        : QObject(parent)
        , d_ptr(new QtLogFormaterPrivate(this))
    {

    }

    QtLogFormater::~QtLogFormater()
    {

    }

    Q_GLOBAL_STATIC(QtLogFormater,_logger_formater_inst_)
    QtLogFormater *QtLogFormater::inst()
    {
        return _logger_formater_inst_;
    }

    bool QtLogFormater::setMessagePattern(const QString &pattern)
    {
        Q_D(QtLogFormater);
        QMutexLocker locker(&QtLogFormaterPrivate::_mutex);
        d->_configure.pattern = pattern;
        qSetMessagePattern(pattern);
        return true;
    }

    bool QtLogFormater::setFilterRules(const QString &rules)
    {
        Q_D(QtLogFormater);
        QMutexLocker locker(&QtLogFormaterPrivate::_mutex);
        d->_configure.rules = rules;
        QLoggingCategory::setFilterRules(rules);
        return true;
    }

    QtLogFormaterPrivate::Configure::_Destination string2Destination(const QString& str)
    {
        if("TextFile" == str)
            return QtLogFormaterPrivate::Configure::TextFile;
        return QtLogFormaterPrivate::Configure::Console;
    }

    bool QtLogFormater::setConfigFile(const QString &file)
    {
        Q_D(QtLogFormater);

        QMutexLocker locker(&QtLogFormaterPrivate::_mutex);

        QSettings settings(file,QSettings::IniFormat);
        if(QSettings::NoError != settings.status()){
            return false;
        }

        settings.beginGroup("Logger");
        d->_configure.destination = string2Destination(settings.value("Destination").toString());
        d->_configure.rules = settings.value("Rules").toString();
        d->_configure.pattern = settings.value("Pattern").toString();
        d->_configure.flush = settings.value("AutoFlush").toBool();
        d->_configure.size = settings.value("MaxFileSize").toULongLong();
        d->_configure.taget = settings.value("Target").toString();
        d->_configure.maxFiles = settings.value("MaxFiles").toUInt();
        settings.endGroup();

        if(d->_configure.taget.isEmpty())
            d->_configure.taget = \
                    QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

        if(QtLogFormaterPrivate::Configure::Console != d->_configure.destination)
            qInstallMessageHandler(QtLogFormaterPrivate::call_MessageHandler);
        QLoggingCategory::setFilterRules(d->_configure.rules);
        qSetMessagePattern(d->_configure.pattern);
        return true;
    }

    void QtLogFormater::write(const QString &msg)
    {
        Q_D(QtLogFormater);

        if(d->_localFileCount > d->_configure.maxFiles){
            d->_localFileCount = 0;
        }

        QString file_path = d->_configure.taget
                + QDir::separator()
                + QDate::currentDate().toString("yyyy.MM.dd")
                + QDir::separator()
                + QCoreApplication::applicationName()
                + "_"
                + QString::number(d->_localFileCount)
                + ".log";

        QDir dir(QFileInfo(file_path).absolutePath());
        if (!dir.exists())
            dir.mkpath(dir.absolutePath());

        if(!d->_file.isOpen()){
            d->_file.setFileName(file_path);
             if(!d->_file.open((QFileInfo::exists(file_path) &&
                                d->_file.size() <  d->_configure.size)
                               ? QFile::Append | QFile::ReadOnly
                               : QFile::WriteOnly | QFile::Text)){
                std::cerr << d->_file.fileName().toUtf8().data() <<" open error" << std::endl;
                return;
             }
             d->_localFileCount++;
        }

        if(d->_file.size() >= d->_configure.size && 0 != d->_configure.size){
            d->_file.flush();
            d->_file.close();
            write(msg);
            return;
        }

        d->_file.write(msg.toLocal8Bit() + "\n");

        if(!d->_configure.flush)
            d->_file.flush();
    }

}
