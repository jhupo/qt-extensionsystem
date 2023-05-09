#include "qtextensionsystemeventdispatch.h"
#include "qtextensionsystemevent.h"

#include <QMap>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#include <QReadWriteLock>

namespace QtExtensionSystem {

    namespace Event {

        class QtExtensionSystemEventDispatchPrivate
        {
            Q_DISABLE_COPY(QtExtensionSystemEventDispatchPrivate)
            Q_DECLARE_PUBLIC(QtExtensionSystemEventDispatch)

        public:

            QtExtensionSystemEventDispatchPrivate(QtExtensionSystemEventDispatch* q);
            virtual~QtExtensionSystemEventDispatchPrivate();

            void init();

            void sendPostedEvents(QObject* obj, QtExtensionSystemEvent* event, Qt::ConnectionType type);

            void listenerCollects(const int event, QList<QObject*>& objs);

            QtExtensionSystemEventDispatch*const                    q_ptr;
            QReadWriteLock                                          _lock;
            QMap<int,QMap<Qt::EventPriority, QSet<QObject*> > >     _listeners;
        };

        QtExtensionSystemEventDispatchPrivate::QtExtensionSystemEventDispatchPrivate(QtExtensionSystemEventDispatch *q)
            : q_ptr(q)
        {
            Q_ASSERT(q);
        }

        QtExtensionSystemEventDispatchPrivate::~QtExtensionSystemEventDispatchPrivate()
        {

        }

        void QtExtensionSystemEventDispatchPrivate::init()
        {

        }

        void QtExtensionSystemEventDispatchPrivate::sendPostedEvents(QObject *obj, QtExtensionSystemEvent *event, Qt::ConnectionType type)
        {
            bool ispost = obj->thread() != QThread::currentThread();

            switch (type)
            {
            case Qt::AutoConnection:
            {
                if(ispost)
                    QCoreApplication::postEvent(obj,event);
                else
                    QCoreApplication::sendEvent(obj,event);
                break;
            }
            case Qt::DirectConnection:
            {
                QCoreApplication::sendEvent(obj,event);
                break;
            }
            case Qt::BlockingQueuedConnection:
            case Qt::UniqueConnection:
            case Qt::QueuedConnection:
            {
                QCoreApplication::postEvent(obj,event);
                break;
            }
            default:break;
            }
            if(!ispost)
                delete event;
        }

        void QtExtensionSystemEventDispatchPrivate::listenerCollects(const int event, QList<QObject *> &objs)
        {
            objs = _listeners[event][Qt::HighEventPriority].values();
            objs.append(_listeners[event][Qt::NormalEventPriority].values());
            objs.append(_listeners[event][Qt::LowEventPriority].values());
        }

        QtExtensionSystemEventDispatch::QtExtensionSystemEventDispatch(QObject *parent)
            : QObject(parent)
            , d_ptr(new QtExtensionSystemEventDispatchPrivate(this))
        {
            d_func()->init();
        }

        QtExtensionSystemEventDispatch::~QtExtensionSystemEventDispatch()
        {

        }

        Q_GLOBAL_STATIC(QtExtensionSystemEventDispatch,_event_dispatch_inst_)
        QtExtensionSystemEventDispatch *QtExtensionSystemEventDispatch::inst()
        {
            return _event_dispatch_inst_;
        }

        void QtExtensionSystemEventDispatch::publishEvent(const int event, const QVariant &val, const Qt::ConnectionType conn)
        {
            Q_D(QtExtensionSystemEventDispatch);
            QReadLocker lock(&d->_lock);
            QList<QObject*> listeners;
            d->listenerCollects(event,listeners);
            foreach(QObject* obj, listeners)
            {
                d->sendPostedEvents(obj,new QtExtensionSystemEvent(event,val),conn);
            }
        }

        bool QtExtensionSystemEventDispatch::registerEvent(const int event, QObject *listener, const Qt::EventPriority priority)
        {
            Q_D(QtExtensionSystemEventDispatch);
            QWriteLocker lock(&d->_lock);
            d->_listeners[event][priority].insert(listener);
            return true;
        }

        bool QtExtensionSystemEventDispatch::unRegisterEvent(QObject *listener, const int event)
        {
            Q_D(QtExtensionSystemEventDispatch);
            QWriteLocker lock(&d->_lock);
            if(-1 == event)
            {
                QList<int> keys = d->_listeners.keys();
                foreach(int key, keys)
                {
                    d->_listeners[key][Qt::HighEventPriority].remove(listener);
                    d->_listeners[key][Qt::NormalEventPriority].remove(listener);
                    d->_listeners[key][Qt::LowEventPriority].remove(listener);
                }
            }
            else
            {
                d->_listeners[event][Qt::HighEventPriority].remove(listener);
                d->_listeners[event][Qt::NormalEventPriority].remove(listener);
                d->_listeners[event][Qt::LowEventPriority].remove(listener);
            }
            return true;
        }

    }

}
