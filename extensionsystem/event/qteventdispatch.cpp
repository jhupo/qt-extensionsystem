#include "qteventdispatch.h"

#include <QMap>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#include <QReadWriteLock>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(logger_event,"logger.event")

namespace Event {

    class QtEventDispatchPrivate
    {
        Q_DISABLE_COPY(QtEventDispatchPrivate)
        Q_DECLARE_PUBLIC(QtEventDispatch)

    public:

        QtEventDispatchPrivate(QtEventDispatch* q);
        virtual~QtEventDispatchPrivate();

        void init();

        void sendPostedEvents(QObject* obj, const int event, const QVariant &val, Qt::ConnectionType type);

        void listenerCollects(const int event, QList<QObject*>& objs);

        QtEventDispatch*const                    q_ptr;
        QReadWriteLock                                          _lock;
        QMap<int,QMap<Qt::EventPriority, QSet<QObject*> > >     _listeners;
    };

    QtEventDispatchPrivate::QtEventDispatchPrivate(QtEventDispatch *q)
        : q_ptr(q)
    {
        Q_ASSERT(q);
    }

    QtEventDispatchPrivate::~QtEventDispatchPrivate()
    {

    }

    void QtEventDispatchPrivate::init()
    {

    }

    void QtEventDispatchPrivate::sendPostedEvents(QObject *obj, const int event, const QVariant &val, Qt::ConnectionType type)
    {
        qCDebug(logger_event)<<obj<<" recv "<<event<<" connect type "<<type;
        switch (type)
        {
        case Qt::AutoConnection:
        {
            if(obj->thread() != QThread::currentThread())
            {
                QCoreApplication::postEvent(obj,new QtEvent(event,val));
            }
            else
            {
                QCoreApplication::sendEvent(obj,&QtEvent(event,val));
                qCDebug(logger_event)<<obj<<" The same thread";
            }
            break;
        }
        case Qt::DirectConnection:
        {
            QCoreApplication::sendEvent(obj,&QtEvent(event,val));
            break;
        }
        case Qt::BlockingQueuedConnection:
        {
            if(!QMetaObject::invokeMethod(obj,"invokeCustomEvent",type,Q_ARG(QEvent*, &QtEvent(event,val)))){
                QCoreApplication::postEvent(obj,new QtEvent(event,val));
            }
            break;
        }
        case Qt::UniqueConnection:
        case Qt::QueuedConnection:
        {
            QCoreApplication::postEvent(obj,new QtEvent(event,val));
            break;
        }
        default:break;
        }
    }

    void QtEventDispatchPrivate::listenerCollects(const int event, QList<QObject *> &objs)
    {
        objs = _listeners[event][Qt::HighEventPriority].values();
        objs.append(_listeners[event][Qt::NormalEventPriority].values());
        objs.append(_listeners[event][Qt::LowEventPriority].values());
    }

    QtEventDispatch::QtEventDispatch(QObject *parent)
        : QObject(parent)
        , d_ptr(new QtEventDispatchPrivate(this))
    {
        d_func()->init();
    }

    QtEventDispatch::~QtEventDispatch()
    {

    }

    Q_GLOBAL_STATIC(QtEventDispatch,_event_dispatch_inst_)
    QtEventDispatch *QtEventDispatch::inst()
    {
        return _event_dispatch_inst_;
    }

    void QtEventDispatch::publishEvent(const int event, const QVariant &val, const Qt::ConnectionType conn)
    {
        Q_D(QtEventDispatch);
        QReadLocker lock(&d->_lock);
        QList<QObject*> listeners;
        d->listenerCollects(event,listeners);
        foreach(QObject* obj, listeners)
        {
            d->sendPostedEvents(obj,event,val,conn);
        }
    }

    void QtEventDispatch::sendPostedEvents(QObject *obj, const int event, const QVariant &var, const bool block)
    {
        if(block){
            if(QMetaObject::invokeMethod(obj,"invokeCustomEvent",Qt::BlockingQueuedConnection,Q_ARG(QEvent*, &QtEvent(event,var)))){
                qCInfo(logger_event)<<obj<<" exec invokeCustomEvent";
            }else{
                qCWarning(logger_event)<<obj<<"exec invokeCustomEvent error";
            }
        }else{
            if(obj->thread() != QThread::currentThread()){
                QCoreApplication::postEvent(obj,new QtEvent(event,var));
            }else{
                QCoreApplication::sendEvent(obj,&QtEvent(event,var));
            }
            qCInfo(logger_event)<<obj<<" "<<event<<" event";
        }
    }

    bool QtEventDispatch::registerEvent(const int event, QObject *listener, const Qt::EventPriority priority)
    {
        Q_D(QtEventDispatch);
        QWriteLocker lock(&d->_lock);
        d->_listeners[event][priority].insert(listener);
        qCInfo(logger_event)<< listener <<" register "<<event<<" event "<<priority<<" level";
        return true;
    }

    bool QtEventDispatch::unRegisterEvent(QObject *listener, const int event)
    {
        Q_D(QtEventDispatch);
        QWriteLocker lock(&d->_lock);
        if(-1 == event){
            QList<int> keys = d->_listeners.keys();
            foreach(int key, keys){
                d->_listeners[key][Qt::HighEventPriority].remove(listener);
                d->_listeners[key][Qt::NormalEventPriority].remove(listener);
                d->_listeners[key][Qt::LowEventPriority].remove(listener);
            }
            qCInfo(logger_event)<<listener << " unregister all event";
        }
        else{
            d->_listeners[event][Qt::HighEventPriority].remove(listener);
            d->_listeners[event][Qt::NormalEventPriority].remove(listener);
            d->_listeners[event][Qt::LowEventPriority].remove(listener);
            qCInfo(logger_event)<<listener<<" unregister "<<event<<" event";
        }
        return true;
    }

}

