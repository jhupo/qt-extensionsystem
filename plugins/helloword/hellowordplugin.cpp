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

    }

    bool HelloWordPlugin::initialize()
    {
        _widget = new QWidget;
        _widget->resize(800,400);
        addAutoReleasedObject(_widget);
        return true;
    }

    void HelloWordPlugin::extensionsInitialized()
    {
        _widget->showNormal();
    }





}
