#ifndef __QTLOGFORMATER_H__
#define __QTLOGFORMATER_H__

#include "extensionsystem_global.h"

namespace LogFormater{

    class QtLogFormaterPrivate;

    class EXTENSIONSYSTEM_EXPORT QtLogFormater : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(QtLogFormater)
        Q_DECLARE_PRIVATE(QtLogFormater)

    public:

        QtLogFormater(QObject* parent = Q_NULLPTR);
        virtual~QtLogFormater();

        static QtLogFormater* inst();

        bool setMessagePattern(const QString& pattern);

        bool setFilterRules(const QString& rules);

        bool setConfigFile(const QString& file);

        virtual void write(const QString& msg);

    Q_SIGNALS:

        void logger_formater(QtMsgType,const QString&);

    protected:

        const QScopedPointer<QtLogFormaterPrivate>      d_ptr;

    };

}
Q_DECLARE_METATYPE(QtMsgType)


#endif
