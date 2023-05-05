#ifndef __HELLOWORDPLUGIN_H__
#define __HELLOWORDPLUGIN_H__

#include <qtextensionsystemplugin.h>

#include "helloword_global.h"

namespace HelloWord {

    class HELLOWORD_EXPORT HelloWordPlugin : public QtExtensionSystem::Plugin::QtExtensionSystemPlugin
    {
        Q_OBJECT
        Q_DISABLE_COPY(HelloWordPlugin)
        Q_PLUGIN_METADATA(IID "cn.jhupo.QESP.QtExtensionSystemPlugin" FILE "Helloword.json")

    public:

        HelloWordPlugin(QObject* parent = Q_NULLPTR);
        virtual~HelloWordPlugin();

        virtual bool initialize();
        virtual void extensionsInitialized();


    private:

        QWidget*            _widget;

    };




}


#endif
