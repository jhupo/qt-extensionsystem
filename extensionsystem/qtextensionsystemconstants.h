#ifndef __QT_EXTENSIONSYSTEM_CONSTANTS_H__
#define __QT_EXTENSIONSYSTEM_CONSTANTS_H__


#include <QtGlobal>
#include <QEventLoop>
#include <QString>
#include <QTimer>

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

            class QtEsStr
            {
            public:
                static void str2char(const QString& _s, char* _d, const int _l){
                    memset(_d,0,_l);
                    memcpy(_d,_s.toUtf8().data(),qMin(_s.toUtf8().size(),_l));
                }
                static QString char2str(const char* _s){return QString(_s);}
                static void copy2char(char* _d, const char* _s, const int _l){memcpy(_d,_s,_l);}
                static std::string char2stdstr(const char* _s){return std::string(_s);}
                static std::string qstr2stdstr(const QString& _s){return _s.toUtf8().data();}
                static QString stdstr2qstr(const std::string& _s){return _s.c_str();}
            };


        }
    }
}





#endif
