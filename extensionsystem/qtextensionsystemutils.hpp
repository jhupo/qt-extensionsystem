#ifndef __QTEXTENSIONSYSTEMUTILS_HPP__
#define __QTEXTENSIONSYSTEMUTILS_HPP__

#include <QEventLoop>
#include <QString>
#include <QTimer>
#include <QElapsedTimer>
#include <QCoreApplication>

class QWidget;

namespace Utils {

    inline void sleep(int msec)
    {
        QEventLoop loop;
        QTimer::singleShot(msec,&loop,SLOT(quit()));
        loop.exec();
    }

    inline void sleep_v2(int msec)
    {
        QElapsedTimer timer;
        timer.start();
        while(timer.elapsed() < msec){
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }

    namespace ID {

        static bool isChildId(const QString& id, const QString& base){return id.startsWith(base + ".");}
        static QString suffixAfter(const QString& id, const QString& base)
        {
            if(isChildId(id,base))
                return id.mid(base.length() + 1);
            return "";
        }

    }

    namespace Bit {

        template<class T>
        static bool bit(T _v, int _p){ return _v & (static_cast<T>(1) << _p); }

        template<class T>
        static int  clear(T _v, int _p) { return _v &~(static_cast<T>(1) << _p); }

        template<class T>
        static int  set(T _v, int _p) { return _v | (static_cast<T>(1) << _p); }

    }

    namespace Str {

        static void str2char(const QString& _s, char* _d, const int _l){
            memset(_d,0,_l);
            memcpy(_d,_s.toUtf8().data(),qMin(_s.toUtf8().size(),_l));
        }
        static QString char2str(const char* _s){return QString(_s);}
        static void copy2char(char* _d, const char* _s, const int _l){memcpy(_d,_s,_l);}
        static std::string char2stdstr(const char* _s){return std::string(_s);}
        static std::string qstr2stdstr(const QString& _s){return _s.toUtf8().data();}
        static QString stdstr2qstr(const std::string& _s){return _s.c_str();}

    }

}


#endif
