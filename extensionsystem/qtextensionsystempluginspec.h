#ifndef __QT_EXTENSIONSYSTEM_PLUGIN_SPEC_H__
#define __QT_EXTENSIONSYSTEM_PLUGIN_SPEC_H__

#include "extensionsystem_global.h"

namespace QtExtensionSystem {
    namespace Plugin {

        class QtExtensionSystemPlugin;
        class QtExtensionSystemPluginSpecPrivate;
        class QtExtensionSystemPluginManagerPrivate;

        struct EXTENSIONSYSTEM_EXPORT QtExtensionSystemPluginDependency
        {
            enum Type{Required, Optional, Test};

            QtExtensionSystemPluginDependency():_type(Required){}
            bool operator==(const QtExtensionSystemPluginDependency &other) const{return _name == other._name;}

            QString toString()const;
            static Type fromString(const QString& str);

            QString                 _name;
            Type                    _type;
        };


        class EXTENSIONSYSTEM_EXPORT QtExtensionSystemPluginSpec
        {
            Q_DECLARE_PRIVATE(QtExtensionSystemPluginSpec)

        public:

            enum State { Invalid, Read, Resolved, Loaded, Initialized, Running, Stopped, Deleted};

            virtual~QtExtensionSystemPluginSpec();

            bool hasError()const;
            QString errorString()const;
            QString displayName()const;
            QString displayIcon()const;
            QString version()const;
            QStringList translators()const;
            QString location()const;
            State state()const;
            QVector<QtExtensionSystemPluginDependency> dependencys()const;
            QtExtensionSystemPlugin* plugin()const;

        private:

            QtExtensionSystemPluginSpec();
            const QScopedPointer<QtExtensionSystemPluginSpecPrivate>        d_ptr;
            friend class QtExtensionSystemPluginManagerPrivate;

            static QtExtensionSystemPluginSpec* read(const QString& path);

        };


    }
}




#endif
