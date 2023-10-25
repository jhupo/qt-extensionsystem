#ifndef __QTEXTENSIONSYSTEM_EVENT_DISPATCH_H__
#define __QTEXTENSIONSYSTEM_EVENT_DISPATCH_H__


#include "extensionsystem_global.h"

#include "qtevent.h"

#include <QVariant>

namespace Event {

    class QtEventDispatchPrivate;

    class EXTENSIONSYSTEM_EXPORT QtEventDispatch : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(QtEventDispatch)
        Q_DECLARE_PRIVATE(QtEventDispatch)

    public:

        QtEventDispatch(QObject* parent = Q_NULLPTR);
        virtual~QtEventDispatch();

        static QtEventDispatch* inst();

        void publishEvent(const int event,
                          const QVariant& val = QVariant(),
                          const Qt::ConnectionType conn = Qt::AutoConnection);

        static void sendPostedEvents(QObject* obj, const int event, const QVariant& var = QVariant(), const bool block = false);

        bool registerEvent(const int event,
                           QObject* listener,
                           const Qt::EventPriority priority = Qt::NormalEventPriority);

        bool unRegisterEvent(QObject* listener, const int event = -1);

    protected:

        const QScopedPointer<QtEventDispatchPrivate>     d_ptr;

    };

#define QT_DECLARE_INVOKEBLOCK public Q_SLOTS:void invokeCustomEvent(QEvent*event){customEvent(event);}

#define HANDLE_EVENT_BEGIN(name)                                                                        \
        if (event->type() == Event::QtEvent::_Custom_Event_Type_)                                       \
        {                                                                                               \
            Event::QtEvent* name =                                                                      \
                dynamic_cast<Event::QtEvent*>(event);
#define HANDLE_EVENT_END                                                                                \
        }

#define REGISTER_PRIVATE_EVENT(listener,event,priority)                                                \
        Event::QtEventDispatch::inst()->registerEvent(event,listener,priority);

#define REGISTER_EVENT(event)                                                                           \
        REGISTER_PRIVATE_EVENT(this,event,Qt::NormalEventPriority)

#define REGISTER_EVENT_LEVEL(event,level)                                                               \
        REGISTER_PRIVATE_EVENT(this,event,level)

#define REGISTER_OBJ_EVENT(listener,event)                                                              \
        REGISTER_PRIVATE_EVENT(listener,event,Qt::NormalEventPriority)

#define REGISTER_OBJ_EVENT_LEVEL(listener,event,level)                                                  \
        REGISTER_PRIVATE_EVENT(listener,event,level)

#define UNREGISTER_PRIVATE_EVENT(obj,event)                                                            \
        Event::QtEventDispatch::inst()->unRegisterEvent(obj,event);

#define UNREGISTER_EVENT_ALL                                                                            \
        UNREGISTER_PRIVATE_EVENT(this,-1)

#define UNREGISTER_EVENT_OBJ_ALL(obj)                                                                   \
        UNREGISTER_PRIVATE_EVENT(obj,-1)

#define UNREGISTER_EVENT(event)                                                                         \
        UNREGISTER_PRIVATE_EVENT(this,event)

#define UNREGISTER_OBJ_EVENT(obj,event)                                                                 \
        UNREGISTER_PRIVATE_EVENT(obj,event)

#define PUBLISH_PRIVATE_EVENT(event,variant,type)                                                       \
        Event::QtEventDispatch::inst()->publishEvent(event,variant,type);
#define PUBLISH_EVENT(event)                                                                            \
        Event::QtEventDispatch::inst()->publishEvent(event);
#define VOID_FOREM_VARIANT(value)                                                                       \
        QVariant var = QVariant::fromValue(value);
#define PUBLISH_EVENT_VALUE(event,variant)                                                              \
    {                                                                                                   \
        VOID_FOREM_VARIANT(variant)                                                                     \
        PUBLISH_PRIVATE_EVENT(event,var,Qt::AutoConnection)                                             \
    }
#define PUBLISH_INTERNAL_EVENT(obj,event)                                                               \
            Event::QtEventDispatch::sendPostedEvents(obj,event);
#define PUBLISH_INTERNAL_EVENT_VALUE(obj,event,variant)                                                 \
        {                                                                                               \
            VOID_FOREM_VARIANT(variant)                                                                 \
            Event::QtEventDispatch::sendPostedEvents(obj,event,var);                                    \
        }
#define PUBLISH_INTERNAL_PRIVATE(obj,event,var,block)                                                   \
            Event::QtEventDispatch::sendPostedEvents(obj,event,var,block);
}



#endif
