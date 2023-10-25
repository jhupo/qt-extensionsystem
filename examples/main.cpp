#include <QApplication>
#include <QDir>
#include <qtextensionsystempluginmanager.h>
#include <logger/qtlogformater.h>

#include <QThread>


int main(int argc, char** argv)
{
    QApplication a(argc,argv);

    LogFormater::QtLogFormater::inst()->setConfigFile(QDir::cleanPath(QApplication::applicationDirPath() + QDir::separator() + "logger.ini"));

    Plugin::QtExtensionSystemPluginManager::inst()->setInterfaceIdentifier("cn.jhupo.QESP.QtExtensionSystemPlugin");
    QStringList rc(QDir::cleanPath(QApplication::applicationDirPath() + "/plugins"));
    Plugin::QtExtensionSystemPluginManager::inst()->setPluginPaths(rc);
    Plugin::QtExtensionSystemPluginManager::inst()->loadPlugin(Plugin::QtExtensionSystemPluginManager::inst()->spec("HelloWord"));

    Plugin::QtExtensionSystemPluginManager::inst()->unLoadPlugin(Plugin::QtExtensionSystemPluginManager::inst()->spec("HelloWord"));

    return a.exec();
}
