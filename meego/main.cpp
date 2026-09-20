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
// Entry point of the MeeGo Harmattan (Nokia N9 / N950) edition: Qt 4.7 with
// QtQuick 1.1 and com.nokia.meego, against the same engine as the Sailfish
// and Android builds. src/ is never touched; everything that differs lives
// under meego/.
#include <QApplication>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeError>
#include <QDeclarativeView>
#include <QDir>
#include <QImage>
#include <QLocale>
#include <QPainter>
#include <QTextCodec>
#include <QTimer>
#include <QTranslator>
#include <QUrl>
#include <QWidget>
#include <cstdio>

#include "TarockEngine.h"

// Debug aid: with TAROCK_SHOT_DIR set, a PNG of the view is written there
// every 2.5 seconds. The N9 has no other way to look at the UI over ssh.
class ScreenshotTimer : public QObject
{
    Q_OBJECT
public:
    ScreenshotTimer(QWidget* view, const QString& dir)
        : QObject(view), m_view(view), m_dir(dir), m_count(0)
    {
        m_timer.setInterval(2500);
        connect(&m_timer, SIGNAL(timeout()), this, SLOT(shoot()));
        m_timer.start();
    }
private slots:
    void shoot()
    {
        const QString file = QString::fromLatin1("%1/shot-%2.png").arg(m_dir).arg(++m_count);
        QImage image(m_view->size(), QImage::Format_ARGB32);
        image.fill(0xff000000);
        QPainter painter(&image);
        m_view->render(&painter);
        painter.end();
        if (!image.save(file))
            qWarning("screenshot: cannot write %s", qPrintable(file));
    }
private:
    QWidget* m_view;
    QString m_dir;
    QTimer m_timer;
    int m_count;
};

// QtQuick 1.1 has neither "pragma Singleton" nor singletons in a qmldir, but
// the shared QML of qml-common/ refers to Style, Prefs and Theme by those
// bare names several hundred times. Instantiating each one here and putting
// it in the root context gives exactly the same spelling at the QML side,
// which is why none of those references had to change.
static QObject* instantiate(QDeclarativeEngine* engine, const QString& file)
{
    QDeclarativeComponent component(engine, QUrl::fromLocalFile(file));
    if (component.isError()) {
        const QList<QDeclarativeError> errors = component.errors();
        for (int i = 0; i < errors.size(); ++i)
            std::fprintf(stderr, "%s: %s\n", qPrintable(file), qPrintable(errors[i].toString()));
        return 0;
    }
    return component.create(engine->rootContext());
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    // Qt 4 passes untranslated tr()/qsTr() sources through Latin-1, and the
    // German strings are full of umlauts ("Königrufen").
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    // Same pair as the Sailfish and Android builds, so a phone that has had
    // both keeps one settings file.
    QCoreApplication::setOrganizationName(QString::fromLatin1("harbour-tarock"));
    QCoreApplication::setApplicationName(QString::fromLatin1("harbour-tarock"));

    // Installed as /opt/harbour-tarock/{bin,qml,assets,translations}.
    QString root = QString::fromLocal8Bit(qgetenv("TAROCK_ROOT"));
    if (root.isEmpty())
        root = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QString::fromLatin1(".."));

    QTranslator translator;
    if (translator.load(QString::fromLatin1("harbour-tarock-") + QLocale::system().name(),
                        root + QString::fromLatin1("/translations")))
        app.installTranslator(&translator);

    // Declared before the view so it outlives every binding to it.
    TarockEngine engine;

    QDeclarativeView view;
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);

    QDeclarativeContext* ctx = view.rootContext();
    ctx->setContextProperty(QString::fromLatin1("tarockEngine"), &engine);
    const QString qml = root + QString::fromLatin1("/qml/");
    // Theme first: Style reads it, and Prefs reads neither.
    ctx->setContextProperty(QString::fromLatin1("Theme"), instantiate(view.engine(), qml + QString::fromLatin1("Theme.qml")));
    ctx->setContextProperty(QString::fromLatin1("Style"), instantiate(view.engine(), qml + QString::fromLatin1("Style.qml")));
    ctx->setContextProperty(QString::fromLatin1("Prefs"), instantiate(view.engine(), qml + QString::fromLatin1("Prefs.qml")));

    view.setSource(QUrl::fromLocalFile(qml + QString::fromLatin1("harbour-tarock.qml")));
    if (view.status() == QDeclarativeView::Error) {
        const QList<QDeclarativeError> errors = view.errors();
        for (int i = 0; i < errors.size(); ++i)
            std::fprintf(stderr, "%s\n", qPrintable(errors[i].toString()));
        return 1;
    }

    const QByteArray shotDir = qgetenv("TAROCK_SHOT_DIR");
    if (!shotDir.isEmpty())
        new ScreenshotTimer(&view, QString::fromLocal8Bit(shotDir));

    if (qgetenv("TAROCK_WINDOWED").isEmpty()) {
        view.showFullScreen();
    } else {
        view.resize(480, 854);
        view.setVisible(true);
    }
    return app.exec();
}

#include "main.moc"
