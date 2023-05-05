#include "hellowordplugin.h"

#include <QWidget>

namespace HelloWord {

    HelloWordPlugin::HelloWordPlugin(QObject *parent)
        : QtExtensionSystemPlugin(parent)
        , _widget(Q_NULLPTR)
    {

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
