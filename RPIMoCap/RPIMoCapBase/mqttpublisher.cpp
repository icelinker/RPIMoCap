/*
 * This file is part of the RPIMoCap (https://github.com/kaajo/RPIMoCap).
 * Copyright (c) 2019 Miroslav Krajicek.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "mqttpublisher.h"

#include <mosquitto.h>

RPIMoCap::MQTTPublisher::MQTTPublisher(std::string clientName, std::string topic, MQTTSettings settings, QObject *parent)
    : QObject(parent)
    , mosqpp::mosquittopp(clientName.c_str())
    , m_clientName(std::move(clientName))
    , m_topic(std::move(topic))
    , m_settings(std::move(settings))
{
    auto conRes = connect_async(m_settings.IPAddress.c_str(),m_settings.port);
    switch(conRes) {
    case MOSQ_ERR_INVAL:
        qDebug() << QString("the input parameters were invalid");
        return;
    case MOSQ_ERR_SUCCESS:
        loop_start();
        return;
    default: {
        QString errorMessage = mosquitto_strerror(conRes);
        qDebug() << errorMessage;
    }
    }
}

void RPIMoCap::MQTTPublisher::publishData(const QByteArray &data)
{
    mosqpp::mosquittopp::publish(nullptr, m_topic.c_str(), data.size(), data.data(),
                                 static_cast<std::underlying_type_t<MQTTQoS>>(m_settings.QoS), false);
}

void RPIMoCap::MQTTPublisher::on_connect(int rc) {
    switch(rc) {
    case MOSQ_ERR_INVAL:
        qDebug() << QString("the input parameters were invalid");
        return;
    case MOSQ_ERR_SUCCESS:
        loop_start();
        return;
    default: {
        QString errorMessage = mosquitto_strerror(rc);
        qDebug() << errorMessage;
    }
    }
}

void RPIMoCap::MQTTPublisher::on_disconnect(int rc) {
    QString errorMessage = mosquitto_strerror(rc);
    qDebug() << errorMessage;
}

void RPIMoCap::MQTTPublisher::on_log(int, const char *str) {
    qDebug() << str;
}

void RPIMoCap::MQTTPublisher::on_error() {
    qDebug() << "error";
}
