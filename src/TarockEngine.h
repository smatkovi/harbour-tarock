/*
    Copyright (C) 2026 smatkovi

    This file is part of harbour-tarock.

    harbour-tarock is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    harbour-tarock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with harbour-tarock. If not, see <https://www.gnu.org/licenses/>.

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef TAROCKENGINE_H
#define TAROCKENGINE_H

#include <QObject>
#include <QString>

// Milestone M0 placeholder.
//
// The full QML contract is specified in docs/design.md §5; it arrives with the
// rule core in M2/M3. Until then this class carries exactly the three
// properties the empty table binds to, all of them a subset of §5 and all
// notified by the same stateChanged() signal the final engine uses, so the
// QML bindings written now keep working when the real engine replaces this
// one. Nothing else belongs here: rules, AI, LAN and the learning mode are
// later milestones.
class TarockEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active NOTIFY stateChanged)
    Q_PROPERTY(QString profileName READ profileName NOTIFY stateChanged)
    Q_PROPERTY(QString status READ status NOTIFY stateChanged)

public:
    explicit TarockEngine(QObject *parent = nullptr);

    bool active() const { return m_active; }
    QString profileName() const;
    QString status() const;

signals:
    void stateChanged();

private:
    bool m_active = false;
};

#endif // TAROCKENGINE_H
