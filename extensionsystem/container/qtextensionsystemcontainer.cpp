#include "qtextensionsystemcontainer.h"

namespace Container {

    QtExtensionSystemContainer::QtExtensionSystemContainer(const QString &id, QObject *parent)
        : QObject(parent)
        , _id(id)
    {

    }

    QtExtensionSystemContainer::~QtExtensionSystemContainer()
    {

    }

    void QtExtensionSystemContainer::setContainerID(const QString &id)
    {
        _id = id;
    }

    QString QtExtensionSystemContainer::containerID() const
    {
        return _id;
    }


}
