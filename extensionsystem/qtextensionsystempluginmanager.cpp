#include "qtextensionsystempluginmanager.h"
#include "qtextensionsystempluginspec.h"
#include "qtextensionsystempluginspec_p.h"

#include <QDebug>
#include <QMutex>
#include <QDir>

namespace QtExtensionSystem {
    namespace Plugin {

        class QtExtensionSystemPluginSpec;

        class QtExtensionSystemPluginManagerPrivate
        {
            Q_DECLARE_PUBLIC(QtExtensionSystemPluginManager)

        public:

            QtExtensionSystemPluginManagerPrivate(QtExtensionSystemPluginManager* q);
            virtual~QtExtensionSystemPluginManagerPrivate();

            void loadPlugin(QtExtensionSystemPluginSpec *spec, QtExtensionSystemPluginSpec::State destState);

            void loadQueue(QtExtensionSystemPluginSpec *spec,
                           QVector<QtExtensionSystemPlugin*>& plugins,
                           QSet<QtExtensionSystemPlugin*>& visited = QSet<QtExtensionSystemPlugin*>());

            void unLoadQueue(QtExtensionSystemPluginSpec *spec,
                             QVector<QtExtensionSystemPlugin*>& plugins);

            void readPluginPaths(const QStringList& paths);
            QStringList pluginFiles(const QStringList& pluginPaths);

            QtExtensionSystemPluginManager*const                q_ptr;
            QMutex                                              _mutex;
            QList<QObject *>                                    _allObjects;
            QString                                             _interfaceIdentifier;
            QStringList                                         _pluginPaths;
            QVector<QtExtensionSystemPluginSpec *>              _pluginsSpec;
        };


        QtExtensionSystemPluginManager::QtExtensionSystemPluginManager(QObject *parent)
            : QObject(parent)
            , d_ptr(new QtExtensionSystemPluginManagerPrivate(this))
        {

        }

        QtExtensionSystemPluginManager::~QtExtensionSystemPluginManager()
        {

        }

        Q_GLOBAL_STATIC(QtExtensionSystemPluginManager,_plugin_manager_inst_)
        QtExtensionSystemPluginManager *QtExtensionSystemPluginManager::inst()
        {
            return _plugin_manager_inst_;
        }

        void QtExtensionSystemPluginManager::addObject(QObject *obj)
        {
            Q_D(QtExtensionSystemPluginManager);
            if(!obj)
            {
                qCritical()<<"add object for empty";return;
            }
            QMutexLocker lock(&d->_mutex);
            if(d->_allObjects.contains(obj))
            {
                qCritical()<<"object Repeat Add "<< obj;
                return;
            }
            d->_allObjects.push_back(obj);
        }

        void QtExtensionSystemPluginManager::removeObject(QObject *obj)
        {
            Q_D(QtExtensionSystemPluginManager);
            if(!obj)
            {
                qCritical()<<"remove object for empty";
                return;
            }
            QMutexLocker lock(&d->_mutex);
            if(!d->_allObjects.contains(obj))
            {
                qCritical()<<"object Not added "<< obj;
                return;
            }
            d->_allObjects.removeOne(obj);
        }

        QList<QObject *> QtExtensionSystemPluginManager::allObjects()
        {
            Q_D(QtExtensionSystemPluginManager);
            QMutexLocker lock(&d->_mutex);
            return d->_allObjects;
        }

        void QtExtensionSystemPluginManager::setInterfaceIdentifier(const QString &iid)
        {
            Q_D(QtExtensionSystemPluginManager);
            d->_interfaceIdentifier = iid;
        }

        QString QtExtensionSystemPluginManager::identifier() const
        {
            Q_D(const QtExtensionSystemPluginManager);
            return d->_interfaceIdentifier;
        }

        void QtExtensionSystemPluginManager::setPluginPaths(const QStringList &paths)
        {
            Q_D(QtExtensionSystemPluginManager);
            d->readPluginPaths(paths);
        }

        QStringList QtExtensionSystemPluginManager::pluginPaths() const
        {
            Q_D(const QtExtensionSystemPluginManager);
            return d->_pluginPaths;
        }

        QVector<QtExtensionSystemPluginSpec *> QtExtensionSystemPluginManager::specs() const
        {
            Q_D(const QtExtensionSystemPluginManager);
            return d->_pluginsSpec;
        }

        QtExtensionSystemPlugin *QtExtensionSystemPluginManager::plugin(const QString &name) const
        {
            Q_D(const QtExtensionSystemPluginManager);
            QtExtensionSystemPlugin* plugin = Q_NULLPTR;
            foreach(QtExtensionSystemPluginSpec* spec, d->_pluginsSpec)
            {
                if(spec->displayName() == name)
                {
                    plugin = spec->plugin();
                    break;
                }
            }
            return plugin;
        }

        void QtExtensionSystemPluginManager::loadPlugins()
        {

        }

        void QtExtensionSystemPluginManager::loadPlugin(QtExtensionSystemPlugin *plugin)
        {
            Q_D(QtExtensionSystemPluginManager);
            QVector<QtExtensionSystemPlugin*> queue;
            d->loadQueue(plugin->pluginSpec(),queue);
            foreach(QtExtensionSystemPlugin* pl, queue)
            {
                QtExtensionSystemPluginSpec* spec = pl->pluginSpec();
                if (spec->state() == QtExtensionSystemPluginSpec::Resolved)
                {
                    d_ptr->loadPlugin(spec, QtExtensionSystemPluginSpec::Loaded);
                    d_ptr->loadPlugin(spec, QtExtensionSystemPluginSpec::Initialized);
                    d_ptr->loadPlugin(spec, QtExtensionSystemPluginSpec::Running);
                    emit pluginsChanged();
                }
            }
        }

        void QtExtensionSystemPluginManager::unLoadPlugin(QtExtensionSystemPlugin *plugin)
        {
            Q_D(QtExtensionSystemPluginManager);
            QVector<QtExtensionSystemPlugin*> queue;
            d->unLoadQueue(plugin->pluginSpec(),queue);
            foreach(QtExtensionSystemPlugin* pl, queue)
            {
                QtExtensionSystemPluginSpec* spec = pl->pluginSpec();
                if(spec->state() == QtExtensionSystemPluginSpec::Running)
                {
                    d->loadPlugin(spec,QtExtensionSystemPluginSpec::Stopped);
                    d->loadPlugin(spec,QtExtensionSystemPluginSpec::Deleted);
                    emit pluginsChanged();
                }
            }
        }

        QVector<QtExtensionSystemPlugin *> QtExtensionSystemPluginManager::plugins() const
        {
            Q_D(const QtExtensionSystemPluginManager);
            QVector<QtExtensionSystemPlugin*> vecs;
            foreach(QtExtensionSystemPluginSpec* spec, d->_pluginsSpec)
            {
                vecs.push_back(spec->plugin());
            }
            return vecs;
        }

        QtExtensionSystemPluginManagerPrivate::QtExtensionSystemPluginManagerPrivate(QtExtensionSystemPluginManager *q)
            : q_ptr(q)
        {
            Q_ASSERT(q);
        }

        QtExtensionSystemPluginManagerPrivate::~QtExtensionSystemPluginManagerPrivate()
        {

        }

        void QtExtensionSystemPluginManagerPrivate::loadPlugin(QtExtensionSystemPluginSpec *spec, QtExtensionSystemPluginSpec::State destState)
        {
            QMutexLocker locker(&_mutex);
            if (spec->hasError() || spec->state() != destState - 1)
                return;
            switch (destState)
            {
            case QtExtensionSystemPluginSpec::Loaded:
            {
                spec->d_ptr->loadLibrary();
                break;
            }
            case QtExtensionSystemPluginSpec::Initialized:
            {
                spec->d_ptr->initializePlugin();
                break;
            }
            case QtExtensionSystemPluginSpec::Stopped:
            {
                spec->d_ptr->stop();
                break;
            }
            case QtExtensionSystemPluginSpec::Running:
            {
                spec->d_ptr->initializeExtensions();
                break;
            }
            case QtExtensionSystemPluginSpec::Deleted:
            {
                spec->d_ptr->kill();
                spec->d_ptr->_state = QtExtensionSystemPluginSpec::Resolved;
                break;
            }
            default:break;
            }
        }

        void QtExtensionSystemPluginManagerPrivate::loadQueue(
                QtExtensionSystemPluginSpec *spec,
                QVector<QtExtensionSystemPlugin*>& plugins,
                QSet<QtExtensionSystemPlugin*>& visited)
        {
            if (visited.contains(spec->plugin()))
                return;
            visited.insert(spec->plugin());
            QVector<QtExtensionSystemPluginDependency> des = spec->dependencys();
            foreach(QtExtensionSystemPluginDependency iter, des) {
                QtExtensionSystemPlugin * plugin = q_ptr->plugin(iter._name);
                loadQueue(plugin->pluginSpec(), plugins, visited);
            }
            plugins.push_back(spec->plugin());
        }

        void QtExtensionSystemPluginManagerPrivate::unLoadQueue(QtExtensionSystemPluginSpec *spec, QVector<QtExtensionSystemPlugin *> &plugins)
        {
            foreach(QtExtensionSystemPluginSpec* plugin, _pluginsSpec)
            {
                QVector<QtExtensionSystemPluginDependency> deps = plugin->dependencys();
                for(int i = 0; i < deps.size(); ++i)
                {
                    QtExtensionSystemPluginDependency dep = deps.at(i);
                    if(dep._name == spec->displayName())
                        unLoadQueue(plugin,plugins);
                }
            }
            plugins.append(spec->plugin());
        }

        void QtExtensionSystemPluginManagerPrivate::readPluginPaths(const QStringList &paths)
        {
            qDeleteAll(_pluginsSpec);
            _pluginsSpec.clear();
            foreach(const QString &pluginFile, pluginFiles(paths))
            {
                QtExtensionSystemPluginSpec* spec = QtExtensionSystemPluginSpec::read(pluginFile);
                if(!spec)
                    continue;
                _pluginsSpec.append(spec);
            }
        }

        QStringList QtExtensionSystemPluginManagerPrivate::pluginFiles(const QStringList &pluginPaths)
        {
            QStringList pluginFiles;
            _pluginPaths = pluginPaths;
            while (!_pluginPaths.isEmpty()) {
                const QDir dir(_pluginPaths.takeFirst());
                const QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
                QStringList absoluteFilePaths;
                foreach(const QFileInfo& info, files)
                {
                    absoluteFilePaths += info.absoluteFilePath();
                }
                foreach(const QString& path, absoluteFilePaths)
                {
                    if(QLibrary::isLibrary(path))
                        pluginFiles += path;
                }
            }
            return pluginFiles;
        }

    }
}
