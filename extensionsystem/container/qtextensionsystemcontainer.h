#ifndef __QTEXTENSIONSYSTEMCONTAINER_H__
#define __QTEXTENSIONSYSTEMCONTAINER_H__


#include "extensionsystem_global.h"


namespace Container {

    class QtExtensionSystemContainerManager;

    class EXTENSIONSYSTEM_EXPORT QtExtensionSystemContainer : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(QtExtensionSystemContainer)

    protected:

        QtExtensionSystemContainer(const QString& id, QObject* parent = Q_NULLPTR);
        virtual ~QtExtensionSystemContainer();

        void setContainerID(const QString& id);
        QString containerID()const;

    private:

        QString             _id;
        friend class QtExtensionSystemContainerManager;
    };


}

#endif
