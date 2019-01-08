/**
 * Battery Buddy, a Sailfish application to prolong battery lifetime
 *
 * Copyright (C) 2019 Matti Viljanen
 *
 * Battery Buddy is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Battery Buddy is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with CarBudget. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Matti Viljanen
 */
#include "battery.h"

Battery::Battery(QObject* parent) : QObject(parent)
{
    // Number: meaning percentage, e.g. 42
    chargeFile   = new QFile("/run/state/namespaces/Battery/ChargePercentage");
    // Number: 0 or 1
    chargingFile = new QFile("/run/state/namespaces/Battery/IsCharging");
    // String: charging, discharging, (empty), unknown (others?)
    stateFile   = new QFile("/run/state/namespaces/Battery/ChargingState");

    // TODO
    // What if the files can't be opened?
    // What if the device doesn't have a battery?

    updateData();
}

Battery::~Battery() { }

void Battery::updateData()
{
    if(chargeFile->open(QIODevice::ReadOnly)) {
        nextCharge = chargeFile->readAll().toInt();
        if(nextCharge != charge) {
            charge = nextCharge;
            emit chargeChanged();
        }
        chargeFile->close();
    }
    if(chargingFile->open(QIODevice::ReadOnly)) {
        nextCharging = (chargingFile->readAll().toInt() == 0 ? false : true);
        if(nextCharging != charging) {
            charging = nextCharging;
            emit chargingChanged();
        }
        chargingFile->close();
    }
    if(stateFile->open(QIODevice::ReadOnly)) {
        nextStatus = (QString(stateFile->readAll()));
        if(nextStatus != state) {
            state = nextStatus;

            // Update translated text accordingly
            if(state == "")
                stateTr = tr("idle", "Charger plugged in, not using nor charging battery");
            else if(state == "discharging")
                stateTr = tr("discharging", "Charger not plugged in, battery discharging");
            else if(state == "charging")
                stateTr = tr("charging", "Charger plugged in and battery charging");
            else if(state == "unknown")
                stateTr = tr("unknown", "Battery not detected, or faulty, or something");

            // Finally, emit the signal
            emit stateChanged();
        }
        stateFile->close();
    }
}

int Battery::getCharge(){ return charge; }

bool Battery::getCharging() { return charging; }

QString Battery::getState() { return stateTr; }
