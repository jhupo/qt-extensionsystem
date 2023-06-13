#include "qtextensionsystempluginmanager.h"
#include "qtextensionsystempluginspec.h"
#include "qtextensionsystempluginspec_p.h"

#include <QDebug>
#include <QMutex>
#include <QDir>
#include <QTimer>

namespace QtExtensionSystem {
    namespace Plugin {

        const int DELAYED_INITIALIZE_INTERVAL = 20;

        class QtExtensionSystemPluginSpec;

        class QtExtensionSystemPluginManagerPrivate
        {
            Q_DECLARE_PUBLIC(QtExtensionSystemPluginManager)

        public:

            QtExtensionSystemPluginManagerPrivate(QtExtensionSystemPluginManager* q);
            virtual~QtExtensionSystemPluginManagerPrivate();

            void loadPlugin(QtExtensionSystemPluginSpec *spec, QtExtensionSystemPluginSpec::State destState);

            void loadQueue(QtExtensionSystemPluginSpec *spec,
                           QVector<QtExtensionSystemPluginSpec *> &plugins,
                           QSet<QtExtensionSystemPluginSpec *> &visited = QSet<QtExtensionSystemPluginSpec*>());

            void unLoadQueue(QtExtensionSystemPluginSpec *spec,
                             QVector<QtExtensionSystemPluginSpec*>& plugins);

            void readPluginPaths(const QStringList& paths);
            QStringList pluginFiles(const QStringList& pluginPaths);

            void resolveDependencies(QtExtensionSystemPluginSpec* spec);

            void nextDelayedInitialize();

            QtExtensionSystemPluginManager*const                q_ptr;
            QMutex                                              _mutex;
            QList<QObject *>                                    _allObjects;
            QString                                             _interfaceIdentifier;
            QStringList                                         _pluginPaths;
            QVector<QtExtensionSystemPluginSpec *>              _pluginsSpec;
            QVector<QtExtensionSystemPluginSpec *>              _delayedInitializeQueue;
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

        QtExtensionSystemPluginSpec *QtExtensionSystemPluginManager::spec(const QString &name) const
        {
            Q_D(const QtExtensionSystemPluginManager);
            foreach(QtExtensionSystemPluginSpec* spec, d->_pluginsSpec)
            {
                if(spec->displayName() == name)
                    return spec;
            }
            return Q_NULLPTR;
        }

        void QtExtensionSystemPluginManager::loadPlugins()
        {
            Q_D(QtExtensionSystemPluginManager);
            foreach(QtExtensionSystemPluginSpec* spec, d->_pluginsSpec)
            {
                loadPlugin(spec);
            }
        }

        void QtExtensionSystemPluginManager::loadPlugin(QtExtensionSystemPluginSpec *spec)
        {
            Q_D(QtExtensionSystemPluginManager);
            QVector<QtExtensionSystemPluginSpec*> queue;
            d->loadQueue(spec,queue);

            foreach(QtExtensionSystemPluginSpec* pluginSpec, queue)
                d_ptr->loadPlugin(pluginSpec, QtExtensionSystemPluginSpec::Loaded);

            foreach(QtExtensionSystemPluginSpec* pluginSpec, queue)
                d_ptr->loadPlugin(pluginSpec, QtExtensionSystemPluginSpec::Initialized);

            foreach(QtExtensionSystemPluginSpec* pluginSpec, queue)
                d_ptr->loadPlugin(pluginSpec, QtExtensionSystemPluginSpec::Running);

            foreach(QtExtensionSystemPluginSpec* pluginSpec, queue)
                if(QtExtensionSystemPluginSpec::Running == pluginSpec->state())
                    d->_delayedInitializeQueue.push_back(pluginSpec);

            QTimer::singleShot(DELAYED_INITIALIZE_INTERVAL,this,SLOT(nextDelayedInitialize()));


            emit pluginsChanged();
        }

        void QtExtensionSystemPluginManager::unLoadPlugin(QtExtensionSystemPluginSpec *spec)
        {
            Q_D(QtExtensionSystemPluginManager);
            QVector<QtExtensionSystemPluginSpec*> queue;
            d->unLoadQueue(spec,queue);

            foreach(QtExtensionSystemPluginSpec* pluginSpec, queue)
                d->loadPlugin(pluginSpec,QtExtensionSystemPluginSpec::Stopped);

            foreach(QtExtensionSystemPluginSpec* pluginSpec, queue)
                d->loadPlugin(pluginSpec,QtExtensionSystemPluginSpec::Deleted);

            emit pluginsChanged();
        }

        void QtExtensionSystemPluginManager::nextDelayedInitialize()
        {
            Q_D(QtExtensionSystemPluginManager);
            d->nextDelayedInitialize();
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
                resolveDependencies(spec);
                break;
            }
            default:break;
            }
        }

        void QtExtensionSystemPluginManagerPrivate::loadQueue(
                QtExtensionSystemPluginSpec *spec,
                QVector<QtExtensionSystemPluginSpec*>& plugins,
                QSet<QtExtensionSystemPluginSpec*>& visited)
        {
            if (visited.contains(spec))
                return;
            visited.insert(spec);
            QVector<QtExtensionSystemPluginDependency> des = spec->dependencys();
            foreach(QtExtensionSystemPluginDependency iter, des) {
                if(QtExtensionSystemPluginDependency::Required == iter._type)
                    loadQueue(q_ptr->spec(iter._name), plugins, visited);
            }
            plugins.push_back(spec);
            resolveDependencies(spec);
        }

        void QtExtensionSystemPluginManagerPrivate::unLoadQueue(QtExtensionSystemPluginSpec *spec, QVector<QtExtensionSystemPluginSpec *> &plugins)
        {
            foreach(QtExtensionSystemPluginSpec* plugin, _pluginsSpec)
            {
                QVector<QtExtensionSystemPluginDependency> deps = plugin->dependencys();
                for(int i = 0; i < deps.size(); ++i)
                {
                    QtExtensionSystemPluginDependency dep = deps.at(i);
                    if(dep._name == spec->displayName())
                        if(QtExtensionSystemPluginDependency::Required == dep._type)
                            unLoadQueue(plugin,plugins);
                }
            }
            plugins.append(spec);
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

        void QtExtensionSystemPluginManagerPrivate::resolveDependencies(QtExtensionSystemPluginSpec *spec)
        {
            spec->d_ptr->_state = QtExtensionSystemPluginSpec::Resolved;
        }

        void QtExtensionSystemPluginManagerPrivate::nextDelayedInitialize()
        {
            Q_Q(QtExtensionSystemPluginManager);
            while (!_delayedInitializeQueue.isEmpty()) {
                QtExtensionSystemPluginSpec *spec = _delayedInitializeQueue.front();
                _delayedInitializeQueue.pop_front();
                bool delay = spec->d_ptr->delayedInitialize();
                if (delay)
                    break;
            }
            if (_delayedInitializeQueue.isEmpty()) {
                emit q->initializationDone();
            } else {
                QTimer::singleShot(DELAYED_INITIALIZE_INTERVAL,q,SLOT(nextDelayedInitialize()));
            }
        }

    }
}
