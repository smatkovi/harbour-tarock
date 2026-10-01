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
// Das Nachschlagewerk (docs/design.md §7.7): jedes Profil trägt seine
// Kapitel mit Text, und jeder Anker, den die Erklärleiste oder ein Grund
// nennt, führt zu einem Kapitel mit Inhalt. Genau das fehlte bis 0.6.1 --
// die Regelseite zeigte Überschriften, und ein angetipptes Kapitel blieb
// leer, weil assets/rules nicht existierte.
#include "ReasonText.h"
#include "RulesIndex.h"
#include "core/Reason.h"
#include "core/RuleProfile.h"
#include "core/TarockCore.h"

#include <QCoreApplication>
#include <QSet>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

#include <cstdio>

using namespace tarock;

namespace {

int failures = 0;

void report(const char* what, const QString& detail, int line)
{
    std::fprintf(stderr, "FAILED line %d: %s (%s)\n", line, what, detail.toUtf8().constData());
    ++failures;
}
#define CHECK_MSG(x, detail) do { if (!(x)) report(#x, (detail), __LINE__); } while (false)

const ProfileId kProfiles[] = {
    ProfileId::AtKrOoe2023, ProfileId::HuIlluItvb2019,
    ProfileId::AtTappKlassik, ProfileId::AtStrohMsErw
};

QString sectionOf(const QString& anchor)
{
    const int hash = anchor.indexOf(QLatin1Char('#'));
    return hash >= 0 ? anchor.mid(hash + 1) : anchor;
}

} // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    int chapters = 0;

    for (const ProfileId profile : kProfiles) {
        const QString key = QString::fromLatin1(RuleProfile::get(profile).key());
        const QVariantList list = RulesIndex::chapters(profile);
        CHECK_MSG(list.size() >= 30, key + QLatin1String(": only ") + QString::number(list.size())
                  + QLatin1String(" chapters"));

        QSet<QString> sections;
        int withText = 0;
        for (const QVariant& value : list) {
            const QVariantMap entry = value.toMap();
            const QString chapter = entry.value(QStringLiteral("chapter")).toString();
            const QString name = key + QLatin1Char('#') + chapter;
            CHECK_MSG(!chapter.isEmpty(), key);
            CHECK_MSG(!sections.contains(chapter), name + QLatin1String(" twice"));
            sections.insert(chapter);
            CHECK_MSG(!entry.value(QStringLiteral("title")).toString().isEmpty(), name);
            const int level = entry.value(QStringLiteral("level")).toInt();
            CHECK_MSG(level == 1 || level == 2, name);
            CHECK_MSG(level == (chapter.contains(QLatin1Char('.')) ? 2 : 1), name);
            const QString text = entry.value(QStringLiteral("text")).toString();
            CHECK_MSG(!text.isEmpty(), name + QLatin1String(" has no text"));
            if (!text.isEmpty())
                ++withText;
            // Der Anker ist der volle "rules:<stamm>#<kapitel>".
            const QString anchor = entry.value(QStringLiteral("anchor")).toString();
            CHECK_MSG(anchor == RulesIndex::anchorFor(profile, chapter), name);
            CHECK_MSG(RulesIndex::chapterTitle(profile, anchor)
                      == entry.value(QStringLiteral("title")).toString(), name);
            // Wer die Seite liest, bekommt den Text, den die Datei trägt;
            // ein Rest Markdown wäre ein Fehler des Erzeugers.
            CHECK_MSG(!text.contains(QLatin1String("**")), name + QLatin1String(": markdown left"));
            CHECK_MSG(!text.contains(QLatin1String("```")), name + QLatin1String(": code fence left"));
            CHECK_MSG(!text.startsWith(QLatin1Char('|')), name + QLatin1String(": table left"));
        }
        chapters += list.size();
        // Die Kapitel 1 bis 9 sind die Spielregeln; alle neun müssen da sein.
        for (int n = 1; n <= 9; ++n)
            CHECK_MSG(sections.contains(QString::number(n)), key + QLatin1String(": chapter ")
                      + QString::number(n) + QLatin1String(" missing"));

        // Jeder Grund zeigt auf ein Kapitel, das es gibt. Die Anker der
        // Gründe sind in den Nummern des Königrufens geschrieben
        // (ReasonText.cpp kennt kein Profil); bei den anderen Regelwerken
        // sind Lücken deshalb nur gezählt, nicht gezählt als Fehler.
        const QVector<ReasonCode> codes = ReasonText::allCodes();
        int danglingReasons = 0;
        for (const ReasonCode code : codes) {
            const QString section = sectionOf(ReasonText::anchor(code, profile));
            if (profile == ProfileId::AtKrOoe2023)
                CHECK_MSG(sections.contains(section), key + QLatin1String(": reason ")
                          + QString::fromLatin1(reasonKey(code)) + QLatin1String(" -> #") + section);
            else if (!sections.contains(section))
                ++danglingReasons;
        }
        if (danglingReasons > 0)
            std::printf("%s: %d reason anchors point at chapters this rule book does not have\n",
                        qPrintable(key), danglingReasons);
        for (int phase = static_cast<int>(Phase::Deal); phase <= static_cast<int>(Phase::HandOver); ++phase) {
            const QVariantMap explanation =
                RulesIndex::explanation(profile, static_cast<Phase>(phase), ContractId::None);
            const QString section = sectionOf(explanation.value(QStringLiteral("anchor")).toString());
            if (section.isEmpty())
                continue;
            CHECK_MSG(sections.contains(section), key + QLatin1String(": phase ")
                      + QString::number(phase) + QLatin1String(" -> #") + section);
        }
        const QVariantList glossary = RulesIndex::glossary(profile);
        for (const QVariant& value : glossary) {
            const QVariantMap term = value.toMap();
            const QString section = sectionOf(term.value(QStringLiteral("anchor")).toString());
            CHECK_MSG(sections.contains(section), key + QLatin1String(": term ")
                      + term.value(QStringLiteral("term")).toString() + QLatin1String(" -> #") + section);
        }
        std::printf("%s: %d chapters, %d with text\n", qPrintable(key), list.size(), withText);
    }

    if (failures == 0)
        std::printf("test_rules: %d chapters checked\n", chapters);
    else
        std::fprintf(stderr, "test_rules: %d failures\n", failures);
    return failures == 0 ? 0 : 1;
}
