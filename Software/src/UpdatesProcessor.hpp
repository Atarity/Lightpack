/*
 * UpdatesProcessor.hpp
 *
 *  Created on: 4/11/2014
 *     Project: Prismatik
 *
 *  Copyright (c) 2014 tim
 *
 *  Lightpack is an open-source, USB content-driving ambient lighting
 *  hardware.
 *
 *  Prismatik is a free, open-source software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Prismatik and Lightpack files is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NEWSPROCESSOR_HPP
#define NEWSPROCESSOR_HPP

#include <QString>
#include <QObject>
#include <QNetworkAccessManager>

class QNetworkReply;
class QXmlStreamReader;

class AppVersion
{
public:
    AppVersion()
        : _major(0)
        , _minor(0)
        , _rc(0)
        , _build(0)
    {}

    AppVersion(const QString &version)
    {
       initWith(version);
    }

    void initWith(const QString &version)
    {
        QStringList splittedVer = version.split(".");
        if (splittedVer.size() > 4 || splittedVer.size() < 1)
        {
            return;
        }

        _major = splittedVer[0].toInt();
        _minor = splittedVer.size() > 1 ? splittedVer[1].toInt() : 0;
        _rc    = splittedVer.size() > 2 ? splittedVer[2].toInt() : 0;
        _build = splittedVer.size() > 3 ? splittedVer[3].toInt() : 0;
    }

    bool isValid() const {
        return _major != 0 || _minor != 0 || _rc != 0 || _build != 0;
    }

    int compare(const AppVersion &other) const {
        int dmj = this->_major - other._major;
        if (dmj != 0)
            return dmj;
        int dmn = this->_minor - other._minor;
        if (dmn != 0)
            return dmn;
        int drc = this->_rc - other._rc;
        if (drc != 0)
            return drc;
        int dbuild = this->_build - other._build;
        return dbuild;
    }

    bool operator== (const AppVersion &other) const {
        return this->compare(other) == 0;
    }

    bool operator!= (const AppVersion &other) const {
        return this->compare(other) != 0;
    }

    bool operator< (const AppVersion &other) const {
        return this->compare(other) < 0;
    }

    bool operator> (const AppVersion &other) const {
        return this->compare(other) > 0;
    }

    bool operator>= (const AppVersion &other) const {
        return *this > other || *this == other ;
    }

    bool operator<= (const AppVersion &other) const {
        return *this < other || *this == other ;
    }

private:
    uint _major;
    uint _minor;
    uint _rc;
    uint _build;
};

struct UpdateInfo
{
    UpdateInfo()
        : id(0)
//        , url(NULL)
//        , text(NULL)
//        , softwareVersion(NULL)
//        , firmwareVersion(NULL)
    {}
    unsigned int id;
    QString      url;
    QString      title;
    QString      text;
    QString      softwareVersion;
    QString      firmwareVersion;

    bool operator== (const UpdateInfo &other) const {
        return this->id == other.id;
    }

    bool operator!= (const UpdateInfo &other) const {
        return !(*this == other);
    }
};

class UpdatesProcessor: public QObject
{
    Q_OBJECT
public:
    UpdatesProcessor(QObject * parent = NULL);
    void requestUpdates();
    QList<UpdateInfo> readUpdates(uint lastReadId);

signals:
    void readyRead();

private:
    QList<UpdateInfo> * readUpdates(QList<UpdateInfo> * readUpdates, QXmlStreamReader * xmlReader);
    bool isVersionMatches(const QString &predicate, const AppVersion &version);

    QNetworkAccessManager _networkMan;
    QNetworkReply * _reply;
};

#endif // NEWSPROCESSOR_HPP
