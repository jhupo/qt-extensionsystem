#ifndef __QT_EXTENSIONSYSTEM_PLUGIN_H__
#define __QT_EXTENSIONSYSTEM_PLUGIN_H__

#include "extensionsystem_global.h"

namespace QtExtensionSystem {
    namespace Plugin {

        class QtExtensionSystemPluginSpec;
        class QtExtensionSystemPluginPrivate;
        class QtExtensionSystemPluginSpecPrivate;

        class EXTENSIONSYSTEM_EXPORT QtExtensionSystemPlugin : public QObject
        {
            Q_OBJECT

        public:

            enum ShutdownFlag {SynchronousShutdown, AsynchronousShutdown};

            QtExtensionSystemPlugin(QObject* parent = Q_NULLPTR);
            virtual ~QtExtensionSystemPlugin();

            virtual bool initialize() = 0;
            virtual void extensionsInitialized() = 0;
            virtual bool delayedInitialize() { return false; }
            virtual ShutdownFlag aboutToShutdown() { return SynchronousShutdown; }

            void addObject(QObject *obj);
            void addAutoReleasedObject(QObject *obj);
            void removeObject(QObject *obj);
            QtExtensionSystemPluginSpec *pluginSpec() const;

        Q_SIGNALS:

            void asynchronousShutdownFinished();

        private:

            QtExtensionSystemPluginPrivate*      d;
            friend class QtExtensionSystemPluginSpecPrivate;
        };

    }
}




#endif
