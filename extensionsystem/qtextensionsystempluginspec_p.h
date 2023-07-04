#ifndef __QT_EXTENSIONSYSTEM_PLUGIN_SPEC_P_H__
#define __QT_EXTENSIONSYSTEM_PLUGIN_SPEC_P_H__

#include "qtextensionsystempluginspec.h"
#include "qtextensionsystemplugin.h"

#include <QJsonObject>
#include <QPluginLoader>
#include <QMutex>
#include <QVector>

class QTranslator;

namespace QtExtensionSystem {
    namespace Plugin {

        class QtExtensionSystemPlugin;

        class QtExtensionSystemPluginSpecPrivate
        {
            Q_DECLARE_PUBLIC(QtExtensionSystemPluginSpec)

        public:

            QtExtensionSystemPluginSpecPrivate(QtExtensionSystemPluginSpec* q);
            virtual~QtExtensionSystemPluginSpecPrivate();

            bool read(const QString& path);
            bool readMetaData(const QJsonObject &pluginMetaData);

            bool loadLibrary();
            bool initializePlugin();
            bool initializeExtensions();
            bool delayedInitialize();
            QtExtensionSystemPlugin::ShutdownFlag stop();
            void kill();

            QtExtensionSystemPluginSpec*const               q_ptr;
            QMutex                                          _mutex;
            bool                                            _hasError;
            QString                                         _errorString;
            QString                                         _pluginName;
            QString                                         _displayName;
            QString                                         _displayIcon;
            QString                                         _version;
            QTranslator*                                    _translator;
            QStringList                                     _translators;
            QString                                         _location;
            QVector<QtExtensionSystemPluginDependency>      _dependencys;
            QtExtensionSystemPluginSpec::State              _state;
            QJsonObject                                     _metaData;
            QtExtensionSystemPlugin*                        _plugin;
            QPluginLoader                                   _loader;


        };



    }
}




#endif
