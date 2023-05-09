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

        void QtExtensionSystemEvent::setExtensionSystemData(const QVariant &data)
        {
            _variant = data;
        }

        QVariant QtExtensionSystemEvent::extensionSystemData() const
        {
            return _variant;
        }

        void QtExtensionSystemEvent::setExtensionSystemEvent(const int event)
        {
            _event = event;
        }

        int QtExtensionSystemEvent::extensionSystemEvent() const
        {
            return _event;
        }


    }

}
