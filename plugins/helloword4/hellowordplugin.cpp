#include "hellowordplugin.h"

#include <QWidget>

#include <event/qtextensionsystemeventdispatch.h>

namespace HelloWord {

    HelloWordPlugin::HelloWordPlugin(QObject *parent)
        : QtExtensionSystemPlugin(parent)
        , _widget(Q_NULLPTR)
    {
        REGISTER_EXTENSIONSYSTEM_OBJ_EVENT(100,this);
        PUBLISH_EXTENSIONSYSTEM_EVENT_VALUE(100,QString(""));
    }

    HelloWordPlugin::~HelloWordPlugin()
    {
        if(_widget)
            delete _widget;
    }

    bool HelloWordPlugin::initialize()
    {
        _widget = new QWidget;
        _widget->resize(400,600);
        return true;
    }

    void HelloWordPlugin::extensionsInitialized()
    {
        _widget->showNormal();
    }





}
