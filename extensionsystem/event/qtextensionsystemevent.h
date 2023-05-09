#ifndef __QTEXTENSIONSYSTEM_EVENT_H__
#define __QTEXTENSIONSYSTEM_EVENT_H__

#include "extensionsystem_global.h"

#include <QEvent>
#include <QVariant>

namespace QtExtensionSystem {

    namespace Event {

        class EXTENSIONSYSTEM_EXPORT QtExtensionSystemEvent : public QEvent
        {
        public:

            QtExtensionSystemEvent(QEvent::Type type = _Custom_Event_Type_);
            QtExtensionSystemEvent(const int event, const QVariant& data, Type type = _Custom_Event_Type_);
            virtual~QtExtensionSystemEvent();

            void setExtensionSystemData(const QVariant& data);
            QVariant extensionSystemData()const;

            void setExtensionSystemEvent(const int event);
            int extensionSystemEvent()const;


            static QEvent::Type _Custom_Event_Type_;

        private:
            int                             _event;
            QVariant                        _variant;
        };

    }

}



#endif
