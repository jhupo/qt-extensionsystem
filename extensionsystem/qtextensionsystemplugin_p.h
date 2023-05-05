#ifndef __QT_EXTENSIONSYSTEM_PLUGIN_P_H__
#define __QT_EXTENSIONSYSTEM_PLUGIN_P_H__

#include <QObject>

namespace QtExtensionSystem {
    namespace Plugin {

        class QtExtensionSystemPluginSpec;

        class QtExtensionSystemPluginPrivate
        {
        public:
            QtExtensionSystemPluginSpec*        _pluginSpec;
            QList<QObject *>                    _addedObjectsInReverseOrder;
        };


    }
}




#endif
