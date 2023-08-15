#include "qtextensionsystemplugin.h"
#include "qtextensionsystemplugin_p.h"
#include "qtextensionsystempluginspec.h"
#include "qtextensionsystempluginmanager.h"


namespace Plugin {

    QtExtensionSystemPlugin::QtExtensionSystemPlugin(QObject *parent)
        : QObject(parent)
        , d(new QtExtensionSystemPluginPrivate())
    {

    }

    QtExtensionSystemPlugin::~QtExtensionSystemPlugin()
    {
        foreach (QObject *obj, d->_addedObjectsInReverseOrder)
            QtExtensionSystemPluginManager::inst()->removeObject(obj);
        qDeleteAll(d->_addedObjectsInReverseOrder);
        d->_addedObjectsInReverseOrder.clear();
        delete d;
        d = 0;
    }

    void QtExtensionSystemPlugin::addObject(QObject *obj)
    {
        QtExtensionSystemPluginManager::inst()->addObject(obj);
    }

    void QtExtensionSystemPlugin::addAutoReleasedObject(QObject *obj)
    {
        d->_addedObjectsInReverseOrder.prepend(obj);
        QtExtensionSystemPluginManager::inst()->addObject(obj);
    }

    void QtExtensionSystemPlugin::removeObject(QObject *obj)
    {
        QtExtensionSystemPluginManager::inst()->removeObject(obj);
    }

    QtExtensionSystemPluginSpec *QtExtensionSystemPlugin::pluginSpec() const
    {
        return d->_pluginSpec;
    }


}


