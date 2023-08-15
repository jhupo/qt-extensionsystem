#include <QApplication>
#include <QDir>
#include <qtextensionsystempluginmanager.h>

int main(int argc, char** argv)
{
    QApplication a(argc,argv);

    Plugin::QtExtensionSystemPluginManager::inst()->setInterfaceIdentifier("cn.jhupo.QESP.QtExtensionSystemPlugin");
    QStringList rc(QDir::cleanPath(QApplication::applicationDirPath() + "/plugins"));
    Plugin::QtExtensionSystemPluginManager::inst()->setPluginPaths(rc);
    Plugin::QtExtensionSystemPluginManager::inst()->loadPlugin(Plugin::QtExtensionSystemPluginManager::inst()->spec("HelloWord4"));
    Plugin::QtExtensionSystemPluginManager::inst()->unLoadPlugin(Plugin::QtExtensionSystemPluginManager::inst()->spec("HelloWord4"));
    return a.exec();
}
