// Lädt die portierten QML-Dateien und schreibt hin, was die Maschine dazu
// sagt. Das beweist nicht, dass der Tisch richtig aussieht -- es beweist, dass
// er sich überhaupt bauen lässt: dass jede Datei sich lesen lässt, dass keine
// Eigenschaft zugewiesen wird, die es nicht gibt, und dass die Typen und
// Kontexteigenschaften zusammenpassen. Genau diese Fehler macht eine
// mechanische Portierung, und genau einer davon (ein doppeltes "visible" in
// Card.qml) hat in 0.4.0 bis 0.4.2 den ganzen Tisch unsichtbar gemacht.
//
// com.nokia.meego lässt sich hier nicht laden -- sein Erweiterungsteil ist für
// das Qt 4.7.4 des SDK gebaut, das ohne Bildschirm abbricht, und Qt 4.8.1
// lehnt einen Teil ab, der für eine andere Fassung gebaut wurde. Darum liegen
// unter meego/tests/stubs Stellvertreter, die meego/tests/make-stubs.sh aus
// den echten Quellen erzeugt: jede Eigenschaft, jedes Signal, jede Funktion,
// die das echte Bauteil hat, und nichts darüber hinaus.
//
//   meego/tests/check-qml.sh
#include <QApplication>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeError>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QUrl>

#include <cstdio>

#include "TarockEngine.h"
#include "LearnEngine.h"

namespace {

// Wie meego/main.cpp: ein aus einer QML-Datei erzeugtes Objekt, das die
// Maschine überlebt und als Kontexteigenschaft dient.
QObject* instantiate(QDeclarativeEngine* engine, const QString& file)
{
    QDeclarativeComponent* component =
        new QDeclarativeComponent(engine, QUrl::fromLocalFile(file), engine);
    if (component->isError()) {
        const QList<QDeclarativeError> errors = component->errors();
        for (int i = 0; i < errors.size(); ++i)
            std::fprintf(stderr, "%s\n", qPrintable(errors[i].toString()));
        return 0;
    }
    QObject* object = component->create(engine->rootContext());
    if (object) {
        object->setParent(engine);
        QDeclarativeEngine::setObjectOwnership(object, QDeclarativeEngine::CppOwnership);
    }
    return object;
}

} // namespace

int main(int argc, char* argv[])
{
    // Ohne Oberfläche: der Prüfer zeigt kein Fenster, und die Baumaschine hat
    // keinen Bildschirm. QDeclarativeItem ist ein QGraphicsObject und braucht
    // keinen.
    QApplication app(argc, argv, false);

    const QString qmlDir = argc > 1 ? QString::fromLocal8Bit(argv[1])
                                    : QString::fromLatin1("meego/qml");
    const QString stubs = argc > 2 ? QString::fromLocal8Bit(argv[2])
                                   : QString::fromLatin1("meego/tests/stubs");

    qmlRegisterUncreatableType<LearnEngine>("harbour.tarock", 1, 0, "LearnEngine",
                                            QString::fromLatin1("über tarockEngine.learn"));

    TarockEngine engine;
    QDeclarativeEngine qml;
    qml.addImportPath(stubs);
    qml.addImportPath(qmlDir);

    QDeclarativeContext* ctx = qml.rootContext();
    ctx->setContextProperty(QString::fromLatin1("tarockEngine"), &engine);
    const QString ctxQml = qmlDir + QString::fromLatin1("/context/");
    // Dieselbe Reihenfolge wie in meego/main.cpp: AppTheme zuerst, Style liest
    // es, Prefs liest keines von beiden.
    ctx->setContextProperty(QString::fromLatin1("AppTheme"),
                            instantiate(&qml, ctxQml + QString::fromLatin1("Theme.qml")));
    ctx->setContextProperty(QString::fromLatin1("Style"),
                            instantiate(&qml, ctxQml + QString::fromLatin1("Style.qml")));
    ctx->setContextProperty(QString::fromLatin1("Prefs"),
                            instantiate(&qml, ctxQml + QString::fromLatin1("Prefs.qml")));

    QDir dir(qmlDir);
    const QStringList names = dir.entryList(QStringList() << QString::fromLatin1("*.qml"),
                                            QDir::Files, QDir::Name);
    int failed = 0;
    for (int i = 0; i < names.size(); ++i) {
        const QString file = dir.absoluteFilePath(names.at(i));
        QDeclarativeComponent component(&qml, QUrl::fromLocalFile(file));
        if (component.isError()) {
            ++failed;
            std::printf("FEHLT %s\n", qPrintable(names.at(i)));
            const QList<QDeclarativeError> errors = component.errors();
            for (int e = 0; e < errors.size(); ++e)
                std::printf("      %s\n", qPrintable(errors.at(e).toString()));
            continue;
        }
        std::printf("ok    %s\n", qPrintable(names.at(i)));
    }

    std::printf("\n%d von %d Dateien abgelehnt\n", failed, names.size());
    return failed == 0 ? 0 : 1;
}
