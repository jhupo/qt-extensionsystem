#include "qtextensionsystemcontainermanager.h"
#include "qtextensionsystemconstants.h"

namespace Container {

    QtExtensionSystemContainerManager::QtExtensionSystemContainerManager(QObject *parent)
        : QObject(parent)
    {

    }

    QtExtensionSystemContainerManager::~QtExtensionSystemContainerManager()
    {

    }

    Q_GLOBAL_STATIC(QtExtensionSystemContainerManager,_container_inst_)
    QtExtensionSystemContainerManager *QtExtensionSystemContainerManager::inst()
    {
        return _container_inst_;
    }

    QList<QtExtensionSystemContainer *> QtExtensionSystemContainerManager::containers(const QString &id)
    {
        QMutexLocker lock(&_lock);
        QList<QString> keys = _containers.keys();
        QList<QtExtensionSystemContainer*> cts;
        foreach(const QString& key, keys)
        {
            if(QtExtensionSystem::Constants::Utils::QtEsID::isChildId(key,id))
            {
                if(QtExtensionSystemContainer* val = qobject_cast<QtExtensionSystemContainer*>(_containers.value(key)))
                {
                    cts.push_back(val);
                }
            }
        }
        return cts;
    }

    void QtExtensionSystemContainerManager::unRegisterContainer(const QString &id)
    {
        QMutexLocker lock(&_lock);
        if(_containers.contains(id))
        {
            emit containerChanged();
            _containers.remove(id);
        }
    }

    uint qHash(const QtExtensionSystemContainer *value)
    {
        return value->containerID();
    }




}
