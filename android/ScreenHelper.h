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
#pragma once

#include <QObject>

#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QJniObject>
#endif

// While a LAN game is hosted, joined or played: keeps the display on and runs
// a foreground service (LanService.java). Without it Android drops incoming
// packets for the app as soon as it is in the background or the screen is off.
// LanTable switches this on in M7; the class is already here so the Android
// package, its permissions and its service declaration are complete from M0.
class ScreenHelper : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    void setLanActive(bool active)
    {
        if (active == m_lanActive)
            return;
        m_lanActive = active;
#ifdef Q_OS_ANDROID
        QJniObject context(QNativeInterface::QAndroidApplication::context().object());
        QJniObject::callStaticMethod<void>("org/smatkovi/tarock/LanService", active ? "start" : "stop",
                                           "(Landroid/content/Context;)V", context.object());
        QNativeInterface::QAndroidApplication::runOnAndroidMainThread([active]() {
            QJniObject activity(QNativeInterface::QAndroidApplication::context().object());
            QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
            if (!window.isValid())
                return;
            const jint flagKeepScreenOn = 128; // WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
            if (active)
                window.callMethod<void>("addFlags", "(I)V", flagKeepScreenOn);
            else
                window.callMethod<void>("clearFlags", "(I)V", flagKeepScreenOn);
        });
#endif
    }

private:
    bool m_lanActive = false;
};
