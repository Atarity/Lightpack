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

/*! \~english
    \class TTYLocker

    TTYLocker class designed to lock, unlock, verification of employment
    specific serial device in Unix-like operating systems. \n
    This is an internal (helper) class that is used in such classes as AbstractSerial and SerialDeviceEnumerator.
*/

/* this directive by default will not work because when creating Makefile no checks on the existence of:
1. header: <baudboy.h>
ie using qmake to do this is impossible (difficult).
it is possible to solve the transition to CMake*/
#if defined (HAVE_BAUDBOY_H)
  #include <baudboy.h>
  #include <cstdlib>

/* this directive by default will not work because when creating Makefile no checks on the existence of:
1. header: <lockdev.h>
2. library:-llockdev
ie using qmake to do this is impossible (difficult).
it is possible to solve the transition to CMake*/
#elif defined (HAVE_LOCKDEV_H)
  #include <lockdev.h>
  #include <unistd.h>

#else
  #include <QtCore/QFile>
  #include <QtCore/QDir>

  #include <signal.h>
  #include <errno.h>
  #include <fcntl.h>
  #include <sys/stat.h>

#endif

#include "qcore_unix_p.h"
#include "ttylocker.h"

TTYLocker::TTYLocker()
        : m_descriptor(-1)
{
#if !(defined (HAVE_BAUDBOY_H) || defined (HAVE_LOCKDEV_H))
    this->lockDirList
            << "/var/lock"
            << "/etc/locks"
            << "/var/spool/locks"
            << "/var/spool/uucp"
            << "/tmp";
#endif
}

TTYLocker::~TTYLocker()
{
}

/*! \~english
    \fn void TTYLocker::setDeviceName(const QString &fullName)
    Sets the name \a fullName of the serial device you want to lock, unlock or check the status.
    The name must be complete, for example: /dev/ttyS0 (but not ttyS0).
    \param[in] fullName Serial device name.
*/

/* Truncate the full name of the device to short.
   For example the long name "/dev/ttyS0" will cut up to "ttyS0". */
QString TTYLocker::shortName() const
{
    return QDir::cleanPath(this->m_name).section(QDir::separator() , -1);
}

/*! \~english
    \fn bool TTYLocker::lock() const
    Lock serial device. However, other processes can not access it.
    \return True on success.
*/
bool TTYLocker::lock() const
{
    bool result = false;
#if defined (HAVE_BAUDBOY_H)
    if (::ttylock(this->shortName().toLocal8Bit().constData()))
        ::ttywait(this->shortName().toLocal8Bit().constData());
    result = (::ttylock(this->shortName().toLocal8Bit().constData()) != -1);
#elif defined (HAVE_LOCKDEV_H)
    result = (::dev_lock(this->shortName().toLocal8Bit().constData()) != -1);
#else
    result = this->m_lock();
#endif
    return result;
}

/*! \~english
    \fn bool TTYLocker::unlock() const
    Unlock serial device. However, other processes can access it.
    \return True on success.
*/
bool TTYLocker::unlock() const
{
    bool result = false;
#if defined (HAVE_BAUDBOY_H)
    result = (::ttyunlock(this->shortName().toLocal8Bit().constData()) != -1);
#elif defined (HAVE_LOCKDEV_H)
    result = (::dev_unlock(this->shortName().toLocal8Bit().constData(), ::getpid()) != -1);
#else
    result = this->m_unlock();
#endif
    return result;
}

/*! \~english
    \fn bool TTYLocker::locked(bool *lockedByCurrentPid) const
    Verifies the device is locked or not.
    \param[out] lockedByCurrentPid Also returns an output parameter \a lockedByCurrentPid,
    if lockedByCurrentPid = true - this means that the device is locked the current process.
    \return True if locked.
*/
bool TTYLocker::locked(bool *lockedByCurrentPid) const
{
    bool result = false;
#if defined (HAVE_BAUDBOY_H)
    result = (::ttylocked(this->shortName().toLocal8Bit().constData()) != -1);
    *lockedByCurrentPid = false;
#elif defined (HAVE_LOCKDEV_H)
    result = (::dev_testlock(this->shortName().toLocal8Bit().constData()) != -1);
    *lockedByCurrentPid = false;
#else
    result = this->m_locked(lockedByCurrentPid);
#endif
    return result;
}

#if !(defined (HAVE_BAUDBOY_H) || defined (HAVE_LOCKDEV_H))

/* Returns the name of the lock file, which is tied to the major and minor device number,
    eg "LCK.30.50" etc. */
QString TTYLocker::getLockFileInNumericForm() const
{
    QString result = this->getFirstSharedLockDir();
    if (!result.isEmpty()) {
        struct stat buf;
        if (::stat(this->m_name.toLocal8Bit().constData(), &buf))
            result.clear();
        else {
            result.append("/LCK.%1.%2");
            result = result.arg(major(buf.st_rdev)).arg(minor(buf.st_rdev));
        }
    }
    return result;
}

/* Returns the name of the lock file which is tied to the device name,
    eg "LCK..ttyS0", etc. */
QString TTYLocker::getLockFileInNamedForm() const
{
    QString result = this->getFirstSharedLockDir();
    if (!result.isEmpty()) {
        result.append("/LCK..%1");
        result = result.arg(this->shortName());
    }
    return result;
}

/* Returns the name of the lock file, which is tied to the number of the process which uses a device,
    such as "LCK...999", etc. */
/*
QString TTYLocker::getLockFileInPidForm() const
{
    QString result = this->getFirstSharedLockDir();
    if (!result.isEmpty()) {
        result.append("/LCK...%1");
        result = result.arg(::getpid());
    }
    return result;
}
*/

/* Checks the validity of the process number that was obtained from the Lock file.
    Returns the following values:
        0 - the process does not exist
        1 - process exists and it is "foreign" (ie not current)
        2 - process exists and it is "their" (ie, current)
        3 - another error */
int TTYLocker::checkPid(int pid) const
{
    int ret = ::kill(pid_t(pid), 0);
    switch (ret) {
    case -1:
        switch (errno) {
        case ESRCH: ret = 0; break;
        default: ret = 3;
        }
        break;
    default:
        (::getpid() == pid) ?
                ret = 2 : ret = 1;
    }
    return ret;
}

bool TTYLocker::m_locked(bool *lockedByCurrentPid) const
{
    QFile f; //<- lock file
    bool result = *lockedByCurrentPid = false;

    for (int i = 0; i < 2; ++i) {
        switch (i) {
        case 0: f.setFileName(this->getLockFileInNumericForm()); break;
        case 1: f.setFileName(this->getLockFileInNamedForm()); break;
        //case 2: f.setFileName(this->getLockFileInPidForm()); break;
        default: ;
        }

        if (!f.exists())
            continue;
        if (!f.open(QIODevice::ReadOnly)) {
            result = true;
            break;
        }

        QString content(f.readAll());
        f.close();

        int pid = content.section(' ', 0, 0, QString::SectionSkipEmpty).toInt();

        switch (this->checkPid(pid)) {
        case 0:
            break;
        case 1:
            result = true;
            break;
        case 2:
            result = *lockedByCurrentPid = true;
            break;
        default:
            result = true;
        }

        if (result)
            break;
    }
    return result;
}

bool TTYLocker::m_unlock() const
{
    QFile f;
    f.setFileName(this->getLockFileInNumericForm());
    f.remove();
    f.setFileName(this->getLockFileInNamedForm());
    f.remove();
    //f.setFileName(this->getLockFileInPidForm());
    //f.remove();
    return true;
}

bool TTYLocker::m_lock() const
{
    bool result = true;
    int flags = (O_WRONLY | O_CREAT | O_EXCL);
    int mode = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    for (int i = 0; i < 2; ++i) {
        QString f;
        switch (i) {
        case 0: f = this->getLockFileInNumericForm(); break;
        case 1: f = this->getLockFileInNamedForm(); break;
        //case 2: f = this->getLockFileInPidForm(); break;
        default: ;
        }

        int fd = qt_safe_open(f.toLocal8Bit().constData(), flags, mode);

        if (-1 == fd) {
            result = false;
            break;
        }

        //Make lock file content
        f = "     %1 %2\x0A";
        f = f.arg(::getpid()).arg(::getuid());

        if (-1 == qt_safe_write(fd, (const void *)f.toLocal8Bit().constData(), f.length()))
            result = false;

        qt_safe_close(fd);
        if (!result)
            break;
    }
    if (!result)
        this->m_unlock();
    return result;
}

/* Returns the full path first found in the directory where you can create a lock file
   (ie a directory with access to the read/write).
    Verification of directories is of the order in accordance with the order of records in the variable lockDirList.
*/
QString TTYLocker::getFirstSharedLockDir() const
{
    foreach (QString result, this->lockDirList) {
        if (0 == ::access(result.toLocal8Bit().constData(), (R_OK | W_OK)))
            return result;
    }
    return QString();
}

#endif//HAVE_BAUDBOY_H, HAVE_LOCKDEV_H
