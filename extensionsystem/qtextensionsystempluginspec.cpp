#include "qtextensionsystempluginspec.h"
#include "qtextensionsystempluginspec_p.h"
#include "qtextensionsystempluginmanager.h"
#include "qtextensionsystemplugin_p.h"

#include <QDebug>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>
#include <QCoreApplication>

namespace QtExtensionSystem {
    namespace Plugin{

        namespace  {

                const char _QES_PLUGIN_METADATA[]                    =   "MetaData";
                const char _QES_PLUGIN_DISPLAYNAME[]                 =   "DisplayName";
                const char _QES_PLUGIN_DISPLAYICON[]                 =   "DisplayIcon";
                const char _QES_PLUGIN_TRANSLATOR[]                  =   "Translators";
                const char _QES_PLUGIN_DEPENDENCY[]                  =   "Dependencys";
                const char _QES_PLUGIN_VERSION[]                     =   "Version";


            }

        QtExtensionSystemPluginSpecPrivate::QtExtensionSystemPluginSpecPrivate(QtExtensionSystemPluginSpec *q)
            : q_ptr(q)
        {
            Q_ASSERT(q);
        }

        QtExtensionSystemPluginSpecPrivate::~QtExtensionSystemPluginSpecPrivate()
        {
            if(_translator)
                QCoreApplication::removeTranslator(_translator);
        }

        bool QtExtensionSystemPluginSpecPrivate::read(const QString &path)
        {
            _hasError = false;
            _errorString = _displayName = _displayIcon = _location = _version = QString();
            _translator = Q_NULLPTR;
            _translators.clear();
            _dependencys.clear();
            _state = QtExtensionSystemPluginSpec::Invalid;
            _metaData = QJsonObject();
            _plugin = Q_NULLPTR;
            QFileInfo fileInfo(path);
            _location = fileInfo.absoluteFilePath();
            _loader.setFileName(path);
            if (_loader.fileName().isEmpty()) {
               qCritical() << "Cannot open file";
               return false;
            }
            if (!readMetaData(_loader.metaData()))
                return false;
            _state = QtExtensionSystemPluginSpec::Read;
            return true;
        }

        bool QtExtensionSystemPluginSpecPrivate::readMetaData(const QJsonObject &pluginMetaData)
        {
            qDebug()<<pluginMetaData;
            _hasError = true;
            QJsonValue value;
            QJsonArray array;
            do
            {
                value = pluginMetaData.value(QLatin1String("IID"));
                if (!value.isString()) {
                    _errorString = "Not a plugin (no string IID found)";
                    break;
                }
                if (value.toString() != QtExtensionSystemPluginManager::inst()->identifier()) {
                     _errorString = "Plugin ignored (IID does not match)";
                    break;
                }
                value = pluginMetaData.value(QLatin1String(_QES_PLUGIN_METADATA));
                if (!value.isObject())
                {
                     _errorString = "Plugin meta data not found";
                    break;
                }

                _metaData = value.toObject();
                value = _metaData.value(QLatin1String(_QES_PLUGIN_DISPLAYNAME));
                if(value.isUndefined() || !value.isString())
                {
                    _errorString = "Plugin display name not found or undefined";
                    break;
                }
                _displayName = value.toString();

                value = _metaData.value(QLatin1String(_QES_PLUGIN_DISPLAYICON));
                if(value.isUndefined() || !value.isString())
                {
                    _errorString = "Plugin display icon not found or undefined";
                    break;
                }

                _displayIcon = value.toString();

                value = _metaData.value(QLatin1String(_QES_PLUGIN_TRANSLATOR));
                if(value.isUndefined() || !value.isArray())
                {
                    _errorString = "Plugin translators not found or undefined";
                    break;
                }
                array = value.toArray();
                foreach(const QJsonValue &ts, array)
                {
                    if(!ts.isString())
                    {
                        _errorString = "Plugin translators array values not string";
                        break;
                    }
                    _translators.push_back(ts.toString());
                }

                value = _metaData.value(QLatin1String(_QES_PLUGIN_DEPENDENCY));
                if(value.isUndefined() || !value.isArray())
                {
                    _errorString = "Plugin dependency not found or undefined";
                    break;
                }
                array = value.toArray();
                foreach(const QJsonValue &ds, array)
                {
                    if(!ds.isObject())
                    {
                        _errorString = "Plugin dependency array values not object";
                        break;
                    }
                    QJsonValue depValue;
                    QJsonObject depObject = value.toObject();
                    QStringList keys = depObject.keys();
                    foreach (const QString &key, keys) {
                        depValue = depObject.value(key);
                        if(depValue.isUndefined() || !depValue.isString())
                        {
                            _errorString = "Plugin dependency array values in key&value not string";
                            break;
                        }
                        QtExtensionSystemPluginDependency dependency;
                        dependency._name = key;
                        dependency._type = QtExtensionSystemPluginDependency::fromString(depValue.toString());
                        _dependencys.push_back(dependency);
                    }
                }

                value = _metaData.value(QLatin1String(_QES_PLUGIN_VERSION));
                if(value.isUndefined() || !value.isString())
                {
                    _errorString = "Plugin version not found or undefined";
                    break;
                }

                _version = value.toString();

            }while(_hasError = false);
            return !_hasError;
        }

        bool QtExtensionSystemPluginSpecPrivate::loadLibrary()
        {
            if (_hasError)
                return false;
            if (_state != QtExtensionSystemPluginSpec::Resolved) {
                if (_state == QtExtensionSystemPluginSpec::Loaded)
                    return true;
                _errorString = QCoreApplication::translate("PluginSpec", "Loading the library failed because state != Resolved");
                _hasError = true;
                qCritical() << _errorString;
                return false;
            }
            if (!_loader.load()) {
                _hasError = true;
                _errorString = QDir::toNativeSeparators(_location) + QString::fromLatin1(": ") + _loader.errorString();
                qCritical() << _errorString;
                return false;
            }
            QtExtensionSystemPlugin *pluginObject = qobject_cast<QtExtensionSystemPlugin*>(_loader.instance());
            if (!pluginObject) {
                _hasError = true;
                _errorString = QCoreApplication::translate("PluginSpec", "Plugin is not valid (does not derive from QtComponentsPlugin)");
                _loader.unload();
                qCritical() << _errorString;
                return false;
            }
            _state = QtExtensionSystemPluginSpec::Loaded;
            _plugin = pluginObject;
            _plugin->d->_pluginSpec = q_ptr;
            return true;
        }

        bool QtExtensionSystemPluginSpecPrivate::initializePlugin()
        {
            if (_hasError)
                return false;
            if (_state != QtExtensionSystemPluginSpec::Loaded) {
                if (_state == QtExtensionSystemPluginSpec::Initialized)
                    return true;
                _errorString = QCoreApplication::translate("PluginSpec", "Initializing the plugin failed because state != Loaded");
                _hasError = true;
                qCritical() << _errorString;
                return false;
            }
            if (!_plugin) {
                _errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to initialize");
                _hasError = true;
                qCritical() << _errorString;
                return false;
            }
            if (!_plugin->initialize()) {
                _errorString = QCoreApplication::translate("PluginSpec", "Plugin initialization failed");
                _hasError = true;
                qCritical() << _errorString;
                return false;
            }
            _state = QtExtensionSystemPluginSpec::Initialized;
            return true;
        }

        bool QtExtensionSystemPluginSpecPrivate::initializeExtensions()
        {
            if (_hasError)
                return false;
            if (_state != QtExtensionSystemPluginSpec::Initialized) {
                if (_state == QtExtensionSystemPluginSpec::Running)
                    return true;
                _errorString = QCoreApplication::translate("PluginSpec", "Cannot perform extensionsInitialized because state != Initialized");
                _hasError = true;
                qCritical() << _errorString;
                return false;
            }
            if (!_plugin) {
                _errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to perform extensionsInitialized");
                _hasError = true;
                qCritical() << _errorString;
                return false;
            }
            _plugin->extensionsInitialized();
            _state = QtExtensionSystemPluginSpec::Running;
            return true;
        }

        bool QtExtensionSystemPluginSpecPrivate::delayedInitialize()
        {
            if (_hasError)
                return false;
            if (_state != QtExtensionSystemPluginSpec::Running)
                return false;
            if (!_plugin) {
                _errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to perform delayedInitialize");
                _hasError = true;
                qCritical() << _errorString;
                return false;
            }
            return _plugin->delayedInitialize();
        }

        QtExtensionSystemPlugin::ShutdownFlag QtExtensionSystemPluginSpecPrivate::stop()
        {
            if (!_plugin)
                return QtExtensionSystemPlugin::SynchronousShutdown;
            _state = QtExtensionSystemPluginSpec::Stopped;
            return _plugin->aboutToShutdown();
        }

        void QtExtensionSystemPluginSpecPrivate::kill()
        {
            if (!_plugin)
                return;
            delete _plugin;
            _plugin = Q_NULLPTR;
            if (_loader.isLoaded())
                _loader.unload();
           _state = QtExtensionSystemPluginSpec::Deleted;
        }

        QtExtensionSystemPluginSpec::~QtExtensionSystemPluginSpec()
        {

        }

        bool QtExtensionSystemPluginSpec::hasError() const
        {
            Q_D(const QtExtensionSystemPluginSpec);
            return d->_hasError;
        }

        QString QtExtensionSystemPluginSpec::errorString() const
        {
            Q_D(const QtExtensionSystemPluginSpec);
            return d->_errorString;
        }

        QString QtExtensionSystemPluginSpec::displayName() const
        {
            Q_D(const QtExtensionSystemPluginSpec);
            return d->_displayName;
        }

        QString QtExtensionSystemPluginSpec::displayIcon() const
        {
            Q_D(const QtExtensionSystemPluginSpec);
            return d->_displayIcon;
        }

        QString QtExtensionSystemPluginSpec::version() const
        {
            Q_D(const QtExtensionSystemPluginSpec);
            return d->_version;
        }

        QStringList QtExtensionSystemPluginSpec::translators() const
        {
            Q_D(const QtExtensionSystemPluginSpec);
            return d->_translators;
        }

        QString QtExtensionSystemPluginSpec::location() const
        {
            Q_D(const QtExtensionSystemPluginSpec);
            return d->_location;
        }

        QtExtensionSystemPluginSpec::State QtExtensionSystemPluginSpec::state() const
        {
            Q_D(const QtExtensionSystemPluginSpec);
            return d->_state;
        }

        QVector<QtExtensionSystemPluginDependency> QtExtensionSystemPluginSpec::dependencys() const
        {
            Q_D(const QtExtensionSystemPluginSpec);
            return d->_dependencys;
        }

        QtExtensionSystemPlugin *QtExtensionSystemPluginSpec::plugin() const
        {
            Q_D(const QtExtensionSystemPluginSpec);
            return d->_plugin;
        }

        QtExtensionSystemPluginSpec::QtExtensionSystemPluginSpec()
            : d_ptr(new QtExtensionSystemPluginSpecPrivate(this))
        {

        }

        QtExtensionSystemPluginSpec *QtExtensionSystemPluginSpec::read(const QString &path)
        {
            QtExtensionSystemPluginSpec* spec = new QtExtensionSystemPluginSpec;
           if(!spec->d_ptr->read(path))
           {
               delete spec;
               return Q_NULLPTR;
           }
           return spec;
        }

        QString QtExtensionSystemPluginDependency::toString() const
        {
            switch (_type) {
            case Required:return "Required";
            case Optional:return "Optional";
            case Test:return "Test";
            default:return "";
            }
        }

        QtExtensionSystemPluginDependency::Type QtExtensionSystemPluginDependency::fromString(const QString &str)
        {
            if("Required" == str)
                return Required;
            else if("Optional" == str)
                return Optional;
            return Test;
        }

    }
}
