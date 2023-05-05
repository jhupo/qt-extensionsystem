#include "qtextensionsystemspec.h"

#include <QResource>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace QtExtensionSystem {

    namespace{
        const char _QT_EXTENSION_SYSTEM_SPEC_[]        =   ":/ExtensionSystem.json";
    }

    class QtExtensionSystemSpecPrivate
    {
        Q_DISABLE_COPY(QtExtensionSystemSpecPrivate)
        Q_DECLARE_PUBLIC(QtExtensionSystemSpec)

    public:

        QtExtensionSystemSpecPrivate(QtExtensionSystemSpec* q);
        virtual~QtExtensionSystemSpecPrivate();

        void init();

        void read();

        QtExtensionSystemSpec*const             q_ptr;
        int                                     _index;
        QStringList                             _names;
        QString                                 _icon;
        QString                                 _language;
        QString                                 _company;
        QString                                 _copyright;

    };

    QtExtensionSystemSpecPrivate::QtExtensionSystemSpecPrivate(QtExtensionSystemSpec *q)
        : q_ptr(q)
    {
        Q_ASSERT(q);
    }

    QtExtensionSystemSpecPrivate::~QtExtensionSystemSpecPrivate()
    {

    }

    void QtExtensionSystemSpecPrivate::init()
    {
        _index = 0;
        read();
    }

    void QtExtensionSystemSpecPrivate::read()
    {
        QResource resource(_QT_EXTENSION_SYSTEM_SPEC_);
        if(!resource.isValid())
        {
            qCritical()<<"Failed to load "<<_QT_EXTENSION_SYSTEM_SPEC_<< " resource";
            return;
        }
        QJsonParseError error;
        QString jsonString(resource.isCompressed() ? qUncompress(resource.data(), resource.size()) : QString::fromUtf8(reinterpret_cast<const char*>(resource.data()), resource.size()));
        qDebug()<<jsonString;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
        if (QJsonParseError::NoError != error.error)
        {
            qCritical() << error.errorString();
            return;
        }
        QJsonObject json = jsonDoc.object();
        QJsonValue value = json.value(QLatin1String("Names"));
        if(value.isUndefined() || !value.isArray())
        {
            qCritical() << "system spec names not found or undefined";
            return;
        }
        QJsonArray array = value.toArray();
        foreach(const QJsonValue &name, array)
        {
            if(!name.isString())
            {
                qCritical() << "system spec namesarray values not string";
                return;
            }
            _names.push_back(name.toString());
        }

        value = json.value(QLatin1String("Icon"));
        if(value.isUndefined() || !value.isString())
        {
            qCritical() << "system spec icon not found or undefined";
            return;
        }
        _icon = value.toString();

        value = json.value(QLatin1String("Language"));
        if(value.isUndefined() || !value.isString())
        {
            qCritical() << "system spec language not found or undefined";
            return;
        }
        _language = value.toString();

        value = json.value(QLatin1String("Company"));
        if(value.isUndefined() || !value.isString())
        {
            qCritical() << "system spec company not found or undefined";
            return;
        }
        _company = value.toString();

        value = json.value(QLatin1String("Copyright"));
        if(value.isUndefined() || !value.isString())
        {
            qCritical() << "system spec copyright not found or undefined";
            return;
        }
        _copyright = value.toString();
    }

    QtExtensionSystemSpec::QtExtensionSystemSpec(QObject *parent)
        : QObject(parent)
        , d_ptr(new QtExtensionSystemSpecPrivate(this))
    {
        d_func()->init();
    }

    QtExtensionSystemSpec::~QtExtensionSystemSpec()
    {

    }

    Q_GLOBAL_STATIC(QtExtensionSystemSpec,_spec_inst_)
    QtExtensionSystemSpec *QtExtensionSystemSpec::inst()
    {
        return _spec_inst_;
    }

    QStringList QtExtensionSystemSpec::names() const
    {
        Q_D(const QtExtensionSystemSpec);
        return d->_names;
    }

    QString QtExtensionSystemSpec::defaultName() const
    {
        Q_D(const QtExtensionSystemSpec);
        if(d->_index < d->_names.size())
            return d->_names.at(d->_index);
        return d->_names.first();
    }

    void QtExtensionSystemSpec::setDefaultNameIndex(const int index)
    {
        Q_D(QtExtensionSystemSpec);
        d->_index = index;
        if(d->_index >= d->_names.size())
            d->_index = 0;
        emit defaultNameChanged(d->_names.at(index));
    }

    int QtExtensionSystemSpec::defaultNameIndex() const
    {
        Q_D(const QtExtensionSystemSpec);
        return d->_index;
    }

    QString QtExtensionSystemSpec::icon() const
    {
        Q_D(const QtExtensionSystemSpec);
        return d->_icon;
    }

    QString QtExtensionSystemSpec::language() const
    {
        Q_D(const QtExtensionSystemSpec);
        return d->_language;
    }

    QString QtExtensionSystemSpec::company() const
    {
        Q_D(const QtExtensionSystemSpec);
        return d->_company;
    }

    QString QtExtensionSystemSpec::copyright() const
    {
        Q_D(const QtExtensionSystemSpec);
        return d->_copyright;
    }
}
