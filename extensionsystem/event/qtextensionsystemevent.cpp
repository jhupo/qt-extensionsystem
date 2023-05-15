#include "qtextensionsystemevent.h"


namespace QtExtensionSystem {

    namespace Event {

        QEvent::Type QtExtensionSystemEvent::_Custom_Event_Type_ = static_cast<QEvent::Type>(QEvent::registerEventType());

        QtExtensionSystemEvent::QtExtensionSystemEvent(QEvent::Type type)
            : QEvent(type)
        {

        }

        QtExtensionSystemEvent::QtExtensionSystemEvent(const int event, const QVariant &data, QEvent::Type type)
            : QEvent(type)
            , _event(event)
            , _variant(data)
        {

        }

        QtExtensionSystemEvent::~QtExtensionSystemEvent()
        {

        }

        void QtExtensionSystemEvent::setCustomData(const QVariant &data)
        {
            _variant = data;
        }

        QVariant QtExtensionSystemEvent::customData() const
        {
            return _variant;
        }

        void QtExtensionSystemEvent::setCustomEvent(const int event)
        {
            _event = event;
        }

        int QtExtensionSystemEvent::customEvent() const
        {
            return _event;
        }


    }

}
