#ifndef __QT_EXTENSIONSYSTEM_CONSTANTS_H__
#define __QT_EXTENSIONSYSTEM_CONSTANTS_H__


#include <QtGlobal>
#include <QEventLoop>
#include <QString>
#include <QTimer>

namespace QtExtensionSystem {
    namespace Constants {
        namespace Event {

            typedef enum{
                ES_LOGGER,
            }_Event;
        }

        namespace Utils {

            inline void sleep(int msec)
            {
                QEventLoop loop;
                QTimer::singleShot(msec,&loop,SLOT(quit()));
                loop.exec();
            }

            template<class T>
            class QtEsBit
            {
            public:
                static bool bit(T _v, int _p){ return _v & (static_cast<T>(1) << _p); }
                static int  clear(T _v, int _p) { return _v &~(static_cast<T>(1) << _p); }
                static int  set(T _v, int _p) { return _v | (static_cast<T>(1) << _p); }
            };

            class QtEsID
            {
            public:
                static bool isChildId(const QString& id, const QString& base){return id.startsWith(base + ".");}
                static QString suffixAfter(const QString& id, const QString& base)
                {
                    if(isChildId(id,base))
                        return id.mid(base.length() + 1);
                    return "";
                }
            };


        }
    }
}





#endif
