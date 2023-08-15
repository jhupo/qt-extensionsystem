#ifndef __QT_EXTENSIONSYSTEM_SPEC_H__
#define __QT_EXTENSIONSYSTEM_SPEC_H__

#include "extensionsystem_global.h"

namespace System {

    class QtExtensionSystemSpecPrivate;

    class EXTENSIONSYSTEM_EXPORT QtExtensionSystemSpec : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(QtExtensionSystemSpec)
        Q_DECLARE_PRIVATE(QtExtensionSystemSpec)

    public:

        QtExtensionSystemSpec(QObject* parent = Q_NULLPTR);
        virtual~QtExtensionSystemSpec();

        static QtExtensionSystemSpec* inst();

        QStringList names()const;
        QString defaultName()const;
        void setDefaultNameIndex(const int index);
        int defaultNameIndex()const;
        QString icon()const;
        QString language()const;
        QString company()const;
        QString copyright()const;

    Q_SIGNALS:

        void defaultNameChanged(const QString&);

    protected:

        const QScopedPointer<QtExtensionSystemSpecPrivate>      d_ptr;

    };

}




#endif
