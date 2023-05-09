#ifndef __QTEXTENSIONSYSTEM_EVENT_DISPATCH_H__
#define __QTEXTENSIONSYSTEM_EVENT_DISPATCH_H__


#include "extensionsystem_global.h"

#include "qtextensionsystemevent.h"

#include <QVariant>

namespace QtExtensionSystem {

    namespace Event {

        class QtExtensionSystemEventDispatchPrivate;

        class EXTENSIONSYSTEM_EXPORT QtExtensionSystemEventDispatch : public QObject
        {
            Q_OBJECT
            Q_DISABLE_COPY(QtExtensionSystemEventDispatch)
            Q_DECLARE_PRIVATE(QtExtensionSystemEventDispatch)

        public:

            QtExtensionSystemEventDispatch(QObject* parent = Q_NULLPTR);
            virtual~QtExtensionSystemEventDispatch();

            static QtExtensionSystemEventDispatch* inst();

            void publishEvent(const int event,
                              const QVariant& val = QVariant(),
                              const Qt::ConnectionType conn = Qt::AutoConnection);

            bool registerEvent(const int event,
                               QObject* listener,
                               const Qt::EventPriority priority = Qt::NormalEventPriority);

            bool unRegisterEvent(QObject* listener, const int event = -1);

        protected:

            const QScopedPointer<QtExtensionSystemEventDispatchPrivate>     d_ptr;

        };

#define HANDLE_EXTENSIONSYSTEM_EVENT_BEGIN(name)                                                                        \
            if (event->type() == QtExtensionSystem::Event::QtExtensionSystemEvent::_Custom_Event_Type_)                 \
            {                                                                                                           \
                QtExtensionSystem::Event::QtExtensionSystemEvent* name =                                                \
                    dynamic_cast<QtExtensionSystem::Event::QtExtensionSystemEvent*>(event);
#define HANDLE_EXTENSIONSYSTEM_EVENT_END                                                                                \
            }
#define REGISTER_INTERNAL_EVENT(event,listener,priority)                                                                \
            QtExtensionSystem::Event::QtExtensionSystemEventDispatch::inst()->registerEvent(event,listener);
#define REGISTER_EVENT(event,listener)                                                                                  \
            REGISTER_INTERNAL_EVENT(event,listener,Qt::NormalEventPriority)
#define REGISTER_EXTENSIONSYSTEM_EVENT(event)                                                                           \
            REGISTER_EVENT(event,this)
#define REGISTER_EXTENSIONSYSTEM_OBJ_EVENT(event,obj)                                                                   \
            REGISTER_EVENT(event,obj)
#define REGISTER_EXTENSIONSYSTEM_EVELT_LEVEL(event,level)                                                               \
            REGISTER_EVENT(event,this,level)
#define UNREGISTER_EXTENSIONSYSTEM_INTERNAL_EVENT(obj,event)                                                            \
            QtExtensionSystem::Event::QtExtensionSystemEventDispatch::inst()->unRegisterEvent(obj,event);
#define UNREGISTER_EXTENSIONSYSTEM_EVENT_ALL                                                                            \
            UNREGISTER_EXTENSIONSYSTEM_INTERNAL_EVENT(this,-1)
#define UNREGISTER_EXTENSIONSYSTEM_EVENT_OBJ_ALL(obj)                                                                   \
            UNREGISTER_EXTENSIONSYSTEM_INTERNAL_EVENT(obj,-1)
#define UNREGISTER_EXTENSIONSYSTEM_EVENT(event)                                                                         \
            UNREGISTER_EXTENSIONSYSTEM_INTERNAL_EVENT(this,event)
#define UNREGISTER_EXTENSIONSYSTEM_EVENT_OBJ(obj,event)                                                                 \
            UNREGISTER_EXTENSIONSYSTEM_INTERNAL_EVENT(obj,event)
#define PUBLISH_EXTENSIONSYSTEM_INTERNAL_EVENT(event,variant,type)                                                      \
            QtExtensionSystem::Event::QtExtensionSystemEventDispatch::inst()->publishEvent(event,variant,type);
#define PUBLISH_EXTENSIONSYSTEM_EVENT(event)                                                                            \
            QtExtensionSystem::Event::QtExtensionSystemEventDispatch::inst()->publishEvent(event);
#define VOID_FOREM_VARIANT(value)                                                                                       \
            QVariant var = QVariant::fromValue(value);
#define PUBLISH_EXTENSIONSYSTEM_EVENT_VALUE(event,variant)                                                              \
        {                                                                                                               \
            VOID_FOREM_VARIANT(variant)                                                                                 \
            PUBLISH_EXTENSIONSYSTEM_INTERNAL_EVENT(event,var,Qt::AutoConnection)                                        \
        }

    }

}



#endif
