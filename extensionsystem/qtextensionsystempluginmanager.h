#ifndef __QT_EXTENSIONSYSTEM_PLUGIN_MANAGER_H__
#define __QT_EXTENSIONSYSTEM_PLUGIN_MANAGER_H__

#include "extensionsystem_global.h"

namespace QtExtensionSystem {
    namespace Plugin {

        class QtExtensionSystemPlugin;
        class QtExtensionSystemPluginSpec;
        class QtExtensionSystemPluginManagerPrivate;

        class EXTENSIONSYSTEM_EXPORT QtExtensionSystemPluginManager : public QObject
        {
            Q_OBJECT
            Q_DISABLE_COPY(QtExtensionSystemPluginManager)
            Q_DECLARE_PRIVATE(QtExtensionSystemPluginManager)

        public:

            QtExtensionSystemPluginManager(QObject* parent = Q_NULLPTR);
            virtual ~QtExtensionSystemPluginManager();

            static QtExtensionSystemPluginManager* inst();

            void addObject(QObject *obj);
            void removeObject(QObject *obj);
            QList<QObject *> allObjects();

            void setInterfaceIdentifier(const QString& iid);
            QString identifier()const;

            void setPluginPaths(const QStringList& paths);
            QStringList pluginPaths()const;

            QVector<QtExtensionSystemPluginSpec *> specs()const;
            QVector<QtExtensionSystemPlugin*> plugins()const;
            QtExtensionSystemPlugin* plugin(const QString& name)const;

            void loadPlugins();
            void loadPlugin(QtExtensionSystemPlugin* plugin);
            void unLoadPlugin(QtExtensionSystemPlugin* plugin);

        Q_SIGNALS:

            void pluginsChanged();
            void initializationDone();

        protected:

            const QScopedPointer<QtExtensionSystemPluginManagerPrivate>     d_ptr;

        };

    }
}




#endif
