#include "qtevent.h"


namespace Event {

    QEvent::Type QtEvent::_Custom_Event_Type_ = static_cast<QEvent::Type>(QEvent::registerEventType());

    QtEvent::QtEvent(QEvent::Type type)
        : QEvent(type)
    {

    }

    QtEvent::QtEvent(const int event, const QVariant &data, QEvent::Type type)
        : QEvent(type)
        , _event(event)
        , _variant(data)
    {

    }

    QtEvent::~QtEvent()
    {

    }

    void QtEvent::setCustomData(const QVariant &data)
    {
        _variant = data;
    }

    QVariant QtEvent::customData() const
    {
        return _variant;
    }

    void QtEvent::setCustomEvent(const int event)
    {
        _event = event;
    }

    int QtEvent::customEvent() const
    {
        return _event;
    }


}

