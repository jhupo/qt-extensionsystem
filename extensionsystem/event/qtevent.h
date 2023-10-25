#ifndef __EVENT_H__
#define __EVENT_H__

#include "extensionsystem_global.h"

#include <QEvent>
#include <QVariant>

namespace Event {

    class EXTENSIONSYSTEM_EXPORT QtEvent : public QEvent
    {
    public:

        QtEvent(QEvent::Type type = _Custom_Event_Type_);
        QtEvent(const int event, const QVariant& data, Type type = _Custom_Event_Type_);
        virtual~QtEvent();

        void setCustomData(const QVariant& data);
        QVariant customData()const;

        void setCustomEvent(const int event);
        int customEvent()const;

        static QEvent::Type _Custom_Event_Type_;

    private:
        int                             _event;
        QVariant                        _variant;
    };

}




#endif
