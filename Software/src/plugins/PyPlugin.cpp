/**
 * This file is part of the raptor project.
 *
 * Copyright (C) 2011 Christophe Duvernois <christophe.duvernois@gmail.com>
 *
 * Raptor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Raptor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <PythonQt.h>
#include <QFileInfo>
#include <QIcon>
#include <QUuid>
#include "PyPlugin.h"

#include "Settings.hpp"

using namespace SettingsScope;


PyPlugin::PyPlugin(PythonQtObjectPtr plugin_,  QObject* parent_) :
	QObject(parent_), _plugin(plugin_) {
    _sessionKey = QUuid::createUuid().toString();
    _name = _plugin.call("name", QVariantList()).toString();
	_description = _plugin.call("description", QVariantList()).toString();
	_author = _plugin.call("author", QVariantList()).toString();
	_version = _plugin.call("version", QVariantList()).toString();
    _icon = QString(Settings::getApplicationDirPath()+ "Plugins/"+_name +"/icon.png");
    runing = false;
}

PyPlugin::~PyPlugin(){
    if (this->isEnabled())
    {
        if ( runing ) this->stop();
        _plugin.call("dispose", QVariantList()).toString();
    }
}

void PyPlugin::init(){
  if (this->isEnabled())
    _plugin.call("init", QVariantList()).toString();
}

void PyPlugin::execute(){

	emit aboutToExecute();
   QVariant ret = _plugin.call("run", QVariantList());
   runing = true;
    //@todo use ret ???
	emit executed();
}

void PyPlugin::stop(){

    emit aboutToExecute();
   QVariant ret = _plugin.call("stop", QVariantList());
   runing = false;
    //@todo use ret ???
    emit executed();
}

void  PyPlugin::getSettings(QObject* parent)
{
    emit aboutToExecute();
    QVariantList list;
    list.append((QVariant::fromValue(parent)));
    QVariant ret;
    ret =  _plugin.call("settings", list);
    emit executed();
}


QString PyPlugin::getSessionKey() const {
    return _sessionKey;
}

QString PyPlugin::getName() const {
	return _name;
}

QString PyPlugin::getAuthor() const {
	return _author;
}

QString PyPlugin::getDescription() const {
	return _description;
}

QString PyPlugin::getVersion() const {
	return _version;
}


QIcon PyPlugin::getIcon() const {
    // TODO path to image
   QFileInfo f(_icon);
   if (f.exists())
       return QIcon(_icon);
   return QIcon(":/plugin/Plugin.png");
}


int PyPlugin::getPriority() {
    QString key = this->_name+"/Priority";
    return Settings::valueMain(key).toInt();
}

void PyPlugin::setPriority(int priority) {
    QString key = this->_name+"/Priority";
    Settings::setValueMain(key,priority);
}

bool PyPlugin::isEnabled() {
    QString key = this->_name+"/Enable";
    return Settings::valueMain(key).toBool();
}

void PyPlugin::setEnabled(bool enable){
    DEBUG_LOW_LEVEL << Q_FUNC_INFO << enable;
    QString key = this->_name+"/Enable";
    Settings::setValueMain(key,enable);
    if (!enable && runing ) this->stop();
    if (enable && !runing ) {
        this->init();
        this->execute();
    }
}


