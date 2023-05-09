#ifndef __QTEXTENSIONSYSTEMCONTAINER_MANAGER_H__
#define __QTEXTENSIONSYSTEMCONTAINER_MANAGER_H__

#include "qtextensionsystemcontainer.h"

#include <QMap>
#include <QMutex>

namespace QtExtensionSystem {

    namespace Container {

        class EXTENSIONSYSTEM_EXPORT QtExtensionSystemContainerManager : public QObject
        {
            Q_OBJECT
            Q_DISABLE_COPY(QtExtensionSystemContainerManager)

        public:

            QtExtensionSystemContainerManager(QObject* parent = Q_NULLPTR);
            virtual~QtExtensionSystemContainerManager();

            static QtExtensionSystemContainerManager* inst();

            template<class T>
            T* createContainer(const QString& id)
            {
                QMutexLocker lock(&_lock);
                if(_containers.contains(id))
                    return qobject_cast<T*>(_containers.value(id));
                return new T(id);
            }

            template<class T>
            T* container(const QString& id)
            {
               QMutexLocker rlock(&_lock);
               if(!_containers.contains(id))
               {
                   qWarning()<<"CoreContainerManager::container is nullptr";
                   return Q_NULLPTR;//createContainer<T>(id);
               }
               return qobject_cast<T*>(_containers.value(id));
            }

            QList<QtExtensionSystemContainer*> containers(const QString& id);

            template<class T>
            bool registerContainer(T* container)
            {
                if(Q_NULLPTR == container)
                {
                    qWarning()<<"CoreContainerManager::registerContainer is nullptr";
                    return false;
                }
                QtExtensionSystemContainer* context = qobject_cast<QtExtensionSystemContainer*>(container);
                if(Q_NULLPTR == context)
                {
                    qWarning()<<"CoreContainerManager::registerContainer Type conversion error";
                    return false;
                }
                QMutexLocker lock(&_lock);
                if(_containers.contains(context->id()))
                {
                    qWarning()<<"CoreContainerManager::registerContainer Already exists id = "<<context->id();
                    return false;
                }
                _containers.insert(context->containerID(),context);
                emit containerChanged();
                emit containerRegister(context->id());
                return true;
            }

            void unRegisterContainer(const QString& id);

        Q_SIGNALS:

               void containerChanged();
               void containerRegister(const QString& id);

        private:
            QMutex                                                  _lock;
            QMap<QString,QtExtensionSystemContainer*>               _containers;
        };

    }

}



#endif
