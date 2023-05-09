#ifndef __QT_EXTENSIONSYSTEM_CONSTANTS_H__
#define __QT_EXTENSIONSYSTEM_CONSTANTS_H__


#include <QtGlobal>

#include <QString>

namespace QtExtensionSystem {
    namespace Constants {
        namespace Event {




        }

        namespace Utils {

            inline void sleep(int msec)
            {
                QEventLoop loop;
                QTimer::singleShot(msec,&loop,SLOT(quit()));
                loop.exec();
            }

            template<class T>
            typedef struct
            {
                static bool bit(T _v, int _p){ return _v & (static_cast<T>(1) << _p); }
                static int  clear(T _v, int _p) { return _v &~(static_cast<T>(1) << _p); }
                static int  set(T _v, int _p) { return _v | (static_cast<T>(1) << _p); }
            }QtEsBit;

            typedef struct
            {
                static bool isChildId(const QString& id, const QString& base){return id.startsWith(base + ".");}
                static QString suffixAfter(const QString& id, const QString& base)
                {
                    if(isChildId(id,base))
                        return id.mid(base.length() + 1);
                    return "";
                }
            }QtEsID;


        }
    }
}





#endif
