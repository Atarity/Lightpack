/*
* This file is part of QSerialDevice, an open-source cross-platform library
* Copyright (C) 2009  Denis Shienkov
*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact Denis Shienkov:
*          e-mail: <scapig2@yandex.ru>
*             ICQ: 321789831
*/

#ifndef TTYLOCKER_H
#define TTYLOCKER_H

#include <QtCore/QStringList>

class TTYLocker
{
public:
    TTYLocker();
    ~TTYLocker();

    void setDeviceName(const QString &fullName) { this->m_name = fullName; }
    bool lock() const;
    bool unlock() const;
    bool locked(bool *lockedByCurrentPid) const;

private:
    QString m_name;
    int m_descriptor;

    QString shortName() const;

#if !(defined (HAVE_BAUDBOY_H) || defined (HAVE_LOCKDEV_H))

    QString getLockFileInNumericForm() const;
    QString getLockFileInNamedForm() const;
    //QString getLockFileInPidForm() const;
    QString getFirstSharedLockDir() const;

    int checkPid(int pid) const;
    bool m_locked(bool *lockedByCurrentPid) const;
    bool m_unlock() const;
    bool m_lock() const;

    QStringList lockDirList;
#endif

};

#endif
