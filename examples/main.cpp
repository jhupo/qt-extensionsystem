#include <QApplication>
#include <QDir>
#include <qtextensionsystempluginmanager.h>

int main(int argc, char** argv)
{
    QApplication a(argc,argv);

    QtExtensionSystem::Plugin::QtExtensionSystemPluginManager::inst()->setInterfaceIdentifier("cn.jhupo.QESP.QtExtensionSystemPlugin");
    QStringList rc(QDir::cleanPath(QApplication::applicationDirPath() + "/plugins"));
    QtExtensionSystem::Plugin::QtExtensionSystemPluginManager::inst()->setPluginPaths(rc);
    QtExtensionSystem::Plugin::QtExtensionSystemPluginManager::inst()->loadPlugin(QtExtensionSystem::Plugin::QtExtensionSystemPluginManager::inst()->spec("HelloWord4"));
    QtExtensionSystem::Plugin::QtExtensionSystemPluginManager::inst()->unLoadPlugin(QtExtensionSystem::Plugin::QtExtensionSystemPluginManager::inst()->spec("HelloWord4"));
    return a.exec();
}
