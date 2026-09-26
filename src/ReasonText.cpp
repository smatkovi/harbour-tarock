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
#include "ReasonText.h"

#include "core/Card.h"

#include <QByteArray>
#include <QStringList>

using tarock::BonusId;
using tarock::Card;
using tarock::CardList;
using tarock::CardSet;
using tarock::ContractId;
using tarock::ProfileId;
using tarock::Reason;
using tarock::ReasonCode;
using tarock::Severity;
using tarock::Suit;

namespace {

// Marks a code that has no sentence yet. It is deliberately loud: the rule
// book is the specification, and a code the book forgot must be visible in
// the app and must fail tests/test_learn.cpp, never show up as "".
const char* const kMissingPrefix = "[[KEIN TEXT: ";

// One row per reason code: the key docs/koenigrufen.md §11.4 uses, the rule
// chapter "Mehr dazu" jumps to, and the two words the toast shows. A code
// that is not in this table gets the placeholder above.
struct Entry {
    ReasonCode code;
    const char* specKey;     // "" when §11.4 has no key of its own for it
    const char* anchor;      // chapter of the rule reference
    const char* label;       // QT_TR_NOOP, the short form
};

const Entry kEntries[] = {
    {ReasonCode::Ok, "", "6.7", QT_TR_NOOP("Erlaubt")},

    {ReasonCode::NotYourTurn, "E_NICHT_AM_ZUG", "6.1", QT_TR_NOOP("Nicht am Zug")},
    {ReasonCode::NotYourCard, "", "6.7", QT_TR_NOOP("Nicht dein Blatt")},
    {ReasonCode::WrongPhase, "", "3.5", QT_TR_NOOP("Falscher Zeitpunkt")},
    {ReasonCode::SeatSittingOut, "", "2.7", QT_TR_NOOP("Du setzt aus")},
    {ReasonCode::UnknownAction, "", "3.5", QT_TR_NOOP("Unbekannte Aktion")},

    {ReasonCode::BidTooLow, "E_GEBOT_ZU_NIEDRIG", "3.1", QT_TR_NOOP("Gebot zu niedrig")},
    {ReasonCode::ForehandGameOnlyLast, "E_VORHANDSPIEL_NUR_HINTENNACH", "3.3",
     QT_TR_NOOP("Vorhandspiel")},
    {ReasonCode::SechserOnlyFirst, "E_SECHSER_NUR_VORNEWEG", "3.3", QT_TR_NOOP("Zu spät")},
    {ReasonCode::AlreadyPassed, "E_SCHON_GEPASST", "3.4", QT_TR_NOOP("Schon gepasst")},
    {ReasonCode::ForehandMustSpeak, "E_VORHAND_MUSS_SPRECHEN", "3.3",
     QT_TR_NOOP("Vorhand muss sprechen")},
    {ReasonCode::HoldOnlyForehand, "E_HALTEN_NUR_VORHAND", "3.3", QT_TR_NOOP("Halten")},
    {ReasonCode::HoldNotAllowed, "", "3.3", QT_TR_NOOP("Halten nicht möglich")},
    {ReasonCode::CallerHasFourKings, "E_RUFER_VIER_KOENIGE", "4.1", QT_TR_NOOP("Vier Könige")},
    {ReasonCode::BesserruferWithoutBird, "E_BESSERRUFER_OHNE_VOGEL", "3.7",
     QT_TR_NOOP("Kein Vogel")},
    {ReasonCode::NoHonour, "", "3.7", QT_TR_NOOP("Kein Honneur")},
    {ReasonCode::InviterLocked, "", "3.5", QT_TR_NOOP("Gebot gebunden")},
    {ReasonCode::JumpWithoutCard, "", "3.5", QT_TR_NOOP("Sprung ohne Karte")},
    {ReasonCode::PassIsEngedes, "", "3.5", QT_TR_NOOP("Passen gilt als Aufgabe")},

    {ReasonCode::CallOwnKing, "E_EIGENER_KOENIG", "4.1", QT_TR_NOOP("Eigener König")},
    {ReasonCode::FourthKingNeedsThree, "E_VIERTER_KOENIG_NUR_MIT_DREI", "4.1",
     QT_TR_NOOP("Vierter König")},
    {ReasonCode::CallMustBeXX, "", "4.1", QT_TR_NOOP("Falsches Tarock gerufen")},
    {ReasonCode::CallOwnSkart, "", "4.1", QT_TR_NOOP("Eigene Karte gerufen")},
    {ReasonCode::CallHonourForbidden, "", "4.1", QT_TR_NOOP("Honneur nicht rufbar")},

    {ReasonCode::TalonHalfTaken, "", "4.3", QT_TR_NOOP("Hälfte schon genommen")},
    {ReasonCode::DiscardKing, "E_ABLAGE_KOENIG", "4.6", QT_TR_NOOP("Kein König")},
    {ReasonCode::DiscardTrull, "E_ABLAGE_TRULL", "4.6", QT_TR_NOOP("Kein Trullstück")},
    {ReasonCode::DiscardTarockWhileSuit, "E_ABLAGE_TAROCK_NOCH_FARBE", "4.6",
     QT_TR_NOOP("Tarock zuletzt")},
    {ReasonCode::DiscardSuitWhileTarock, "E_ABLAGE_FARBE_NOCH_TAROCK", "4.6",
     QT_TR_NOOP("Tarock zuerst")},
    {ReasonCode::DiscardCount, "E_ABLAGE_ANZAHL", "4.6", QT_TR_NOOP("Falsche Anzahl")},
    {ReasonCode::DiscardXX, "", "4.6", QT_TR_NOOP("XX bleibt")},
    {ReasonCode::DiscardInvitCard, "", "4.6", QT_TR_NOOP("Gerufene Karte bleibt")},
    {ReasonCode::DiscardNotHeld, "", "4.6", QT_TR_NOOP("Karte nicht im Blatt")},

    {ReasonCode::BonusCardNotHeld, "E_VOGEL_NICHT_IM_BLATT", "5.3", QT_TR_NOOP("Karte fehlt")},
    {ReasonCode::KingUltimoWithoutKing, "E_KU_OHNE_KOENIG", "5.3", QT_TR_NOOP("König fehlt")},
    {ReasonCode::AnnounceOnlyOnce, "E_ANSAGE_NUR_EINMAL", "5.3", QT_TR_NOOP("Nur einmal")},
    {ReasonCode::BesserruferBirdMandatory, "E_BESSERRUFER_VOGEL_PFLICHT", "5.3",
     QT_TR_NOOP("Vogel ist Pflicht")},
    {ReasonCode::NoBonusInNegative, "E_NEGATIV_KEINE_ANSAGE", "5.1", QT_TR_NOOP("Keine Prämien")},
    {ReasonCode::SuitGameBonusLimited, "E_FARBENSPIEL_ANSAGE", "5.1",
     QT_TR_NOOP("Farbenspiel-Ansage")},
    {ReasonCode::SolodreierAnnounceTooLate, "E_SOLODREIER_ANSAGE_ZU_SPAET", "5.3",
     QT_TR_NOOP("Zu spät angesagt")},
    {ReasonCode::KontraBeforeReady, "E_KONTRA_VOR_ICH_LIEGE", "5.6", QT_TR_NOOP("Noch kein Kontra")},
    {ReasonCode::KontraOwnAnnouncement, "E_KONTRA_EIGENE_ANSAGE", "5.6",
     QT_TR_NOOP("Eigene Ansage")},
    {ReasonCode::KontraLevel, "E_KONTRA_STUFE", "5.6", QT_TR_NOOP("Kontrastufe")},
    {ReasonCode::KontraTooLate, "E_KONTRA_ZU_SPAET", "5.6", QT_TR_NOOP("Kontra zu spät")},
    {ReasonCode::TrischakenNoKontra, "E_TRISCHAKEN_KEIN_KONTRA", "5.6",
     QT_TR_NOOP("Kein Kontra")},
    {ReasonCode::SuitGameKontraOrValat, "E_FARBENSPIEL_KONTRA_ODER_VALAT", "5.6",
     QT_TR_NOOP("Kontra oder Valat")},
    {ReasonCode::TrullNeedsHonour, "", "5.3", QT_TR_NOOP("Trull ohne Honneur")},
    {ReasonCode::AnnounceAfterVolat, "", "5.3", QT_TR_NOOP("Nach dem Valat")},
    {ReasonCode::AnnounceRepeat, "", "5.3", QT_TR_NOOP("Schon angesagt")},
    {ReasonCode::SameTrickTwice, "", "5.3", QT_TR_NOOP("Stich doppelt belegt")},

    {ReasonCode::FollowSuit, "E_FARBZWANG", "6.2", QT_TR_NOOP("Farbzwang")},
    {ReasonCode::FollowTarock, "E_FARBZWANG_TAROCK", "6.2", QT_TR_NOOP("Tarock bedienen")},
    {ReasonCode::MustTarock, "E_TAROCKZWANG", "6.2", QT_TR_NOOP("Tarockzwang")},
    {ReasonCode::MustOvertrumpSuit, "E_STICHZWANG_FARBE", "6.4", QT_TR_NOOP("Stichzwang")},
    {ReasonCode::MustOvertrumpTarock, "E_STICHZWANG_TAROCK", "6.4",
     QT_TR_NOOP("Stichzwang im Tarock")},
    {ReasonCode::MustTrumpNegative, "E_STICHZWANG_TRUMPFEN", "6.4", QT_TR_NOOP("Stechen Pflicht")},
    {ReasonCode::PagatNotLastTarock, "E_PAGAT_NEGATIV", "6.4", QT_TR_NOOP("Pagat zuletzt")},
    {ReasonCode::SuitGameNoTarockLead, "E_FARBENSPIEL_TAROCK_AUSSPIEL", "6.3",
     QT_TR_NOOP("Kein Tarockausspiel")},
    {ReasonCode::BoundMustPlay, "E_VOGEL_FAELLIG", "6.5", QT_TR_NOOP("Vogel fällig")},
    {ReasonCode::BoundOrder, "E_HOEHERER_VOGEL", "6.5", QT_TR_NOOP("Höherer Vogel zuerst")},
    {ReasonCode::BoundWinsTooEarly, "E_VOGEL_STICHT_ZU_FRUEH", "6.5",
     QT_TR_NOOP("Vogel sticht zu früh")},

    {ReasonCode::W_BirdReserved, "W_VOGEL_RESERVIERT", "6.5", QT_TR_NOOP("Vogel reserviert")},
    {ReasonCode::W_KingUltimoReserved, "W_KOENIG_ULTIMO_RESERVIERT", "6.5",
     QT_TR_NOOP("König reserviert")},
    {ReasonCode::W_GivesUpSilentBonus, "", "5.5", QT_TR_NOOP("Stille Prämie weg")},

    {ReasonCode::I_DiscardShownOpen, "I_ABLAGE_OFFEN", "4.6", QT_TR_NOOP("Offen verlegt")},
    {ReasonCode::I_TalonHalfGoesToOpponents, "", "4.2", QT_TR_NOOP("Hälfte den Gegnern")},
    {ReasonCode::I_SilentFigure, "", "5.5", QT_TR_NOOP("Stille Prämie")},
    {ReasonCode::I_VolatSilentRule, "", "7.4", QT_TR_NOOP("Valat still")},
};

const Entry* entryFor(ReasonCode code)
{
    for (const Entry& entry : kEntries) {
        if (entry.code == code)
            return &entry;
    }
    return nullptr;
}

// The stem the rule browser files are named after; "rules:at-kr-ooe#6.2".
const char* profileStem(ProfileId profile)
{
    return profile == ProfileId::HuIlluItvb2019 ? "hu-illu" : "at-kr-ooe";
}

} // namespace

// --- naming -------------------------------------------------------------------------

QString ReasonText::suitName(int suit)
{
    switch (suit) {
    case 0: return tr("Herz");
    case 1: return tr("Karo");
    case 2: return tr("Pik");
    case 3: return tr("Treff");
    case 4: return tr("Tarock");
    default: break;
    }
    return tr("Farbe");
}

QString ReasonText::cardName(Card card)
{
    if (!card.valid())
        return tr("diese Karte");
    if (card.tarock()) {
        // The four birds and the two big ones have names of their own; players
        // say "Pagat", not "I" (§11.6).
        switch (card.id) {
        case 0: return tr("Pagat (I)");
        case 1: return tr("Uhu (II)");
        case 2: return tr("Kakadu (III)");
        case 3: return tr("Quapil (IIII)");
        case 20: return tr("Mond (XXI)");
        case 21: return tr("Sküs");
        default: break;
        }
        return QString::fromStdString(tarock::cardKey(card));
    }
    const QString suit = suitName(static_cast<int>(card.suit()));
    switch (card.slot()) {
    case 0: return tr("%1-König").arg(suit);
    case 1: return tr("%1-Dame").arg(suit);
    case 2: return tr("%1-Reiter").arg(suit);
    case 3: return tr("%1-Bube").arg(suit);
    default: break;
    }
    static const char* const kRed[4] = {"As", "2", "3", "4"};
    static const char* const kBlack[4] = {"10", "9", "8", "7"};
    const int low = card.slot() - 4;
    return tr("%1-%2").arg(suit, QString::fromLatin1(card.red() ? kRed[low] : kBlack[low]));
}

QString ReasonText::cardNames(const CardList& list)
{
    QStringList names;
    for (Card card : list)
        names.append(cardName(card));
    return names.isEmpty() ? QString::fromUtf8("–") : names.join(QLatin1String(", "));
}

QString ReasonText::cardNames(const CardSet& set)
{
    return cardNames(tarock::toList(set));
}

QString ReasonText::contractName(int contract)
{
    switch (static_cast<ContractId>(contract)) {
    case ContractId::Trischaken: return tr("Trischaken");
    case ContractId::Rufer: return tr("Rufer");
    case ContractId::Piccolo: return tr("Piccolo");
    case ContractId::Bettel: return tr("Bettel");
    case ContractId::Solorufer: return tr("Solorufer");
    case ContractId::Besserrufer: return tr("Besserrufer");
    case ContractId::Farbendreier: return tr("Farbendreier");
    case ContractId::Sechserdreier: return tr("Sechserdreier");
    case ContractId::Dreier: return tr("Dreier");
    case ContractId::Farbensolo: return tr("Farbensolo");
    case ContractId::PiccoloOuvert: return tr("Piccolo ouvert");
    case ContractId::BettelOuvert: return tr("Bettel ouvert");
    case ContractId::Solodreier: return tr("Solodreier");
    case ContractId::Harom: return tr("Három");
    case ContractId::Ketto: return tr("Kettő");
    case ContractId::Egy: return tr("Egy");
    case ContractId::Szolo: return tr("Szóló");
    case ContractId::TappDreier: return tr("Dreier");
    case ContractId::TappUnterer: return tr("Unterer");
    case ContractId::TappOberer: return tr("Oberer");
    case ContractId::TappSolo: return tr("Solo");
    case ContractId::StrohEinfach: return tr("einfaches Spiel");
    case ContractId::StrohAufgenommen: return tr("aufgenommenes Spiel");
    case ContractId::None: break;
    }
    return tr("dieses Spiel");
}

QString ReasonText::bonusName(int bonus)
{
    switch (static_cast<BonusId>(bonus)) {
    case BonusId::Trull: return tr("Trull");
    case BonusId::AllKings: return tr("Alle Könige");
    case BonusId::KingUltimo: return tr("König ultimo");
    case BonusId::Pagat: return tr("Pagat ultimo");
    case BonusId::Uhu: return tr("Uhu");
    case BonusId::Kakadu: return tr("Kakadu");
    case BonusId::Quapil: return tr("Quapil");
    case BonusId::Valat: return tr("Valat");
    case BonusId::Duplajatek: return tr("Duplajáték");
    case BonusId::SasUltimo: return tr("Sas ultimó");
    case BonusId::XXIFogas: return tr("XXI-fogás");
    case BonusId::Centrum: return tr("Centrum");
    case BonusId::Kismadar: return tr("Kismadár");
    case BonusId::Nagymadar: return tr("Nagymadár");
    case BonusId::PagatUhu: return tr("Pagát-uhu");
    case BonusId::SasUhu: return tr("Sas-uhu");
    case BonusId::KiralyUhu: return tr("Király-uhu");
    case BonusId::Tarokk8: return tr("Tarokk 8");
    case BonusId::Tarokk9: return tr("Tarokk 9");
    case BonusId::Pagatfogas: return tr("Pagát-fogás");
    case BonusId::Grammel: return tr("Grammel-Punkt");
    case BonusId::None: break;
    }
    return tr("diese Prämie");
}

QString ReasonText::seatName(int seat)
{
    if (seat < 0)
        return tr("der nächste Spieler");
    return seat == 0 ? tr("Du") : tr("Spieler %1").arg(seat + 1);
}

QString ReasonText::ordinalLastNominative(int n)
{
    switch (n) {
    case 1: return tr("letzte");
    case 2: return tr("vorletzte");
    case 3: return tr("drittletzte");
    case 4: return tr("viertletzte");
    default: break;
    }
    return tr("%1.-letzte").arg(n);
}

QString ReasonText::ordinalLast(int n)
{
    switch (n) {
    case 1: return tr("letzten");
    case 2: return tr("vorletzten");
    case 3: return tr("drittletzten");
    case 4: return tr("viertletzten");
    default: break;
    }
    return tr("%1.-letzten").arg(n);
}

// --- the sentences of koenigrufen.md §11.4 ------------------------------------------

QString ReasonText::textFor(const Reason& reason)
{
    // Everything a text may want to fill in, prepared once. The core fills
    // what it knows; LearnEngine::enrich() adds the allowed cards and the
    // named suit before the Reason gets here.
    const QString cards = cardNames(reason.cards);
    const QString card = cardName(reason.card);
    const QString suit = suitName(reason.suit);
    const QString game = contractName(reason.contract);
    const QString bonus = bonusName(reason.bonus);
    const QString seat = seatName(reason.seat);
    const int count = reason.count;
    // Bonus tricks are counted from the back: trick 12 is the last one.
    const int fromEnd = reason.trick > 0 ? 13 - reason.trick : 1;

    switch (reason.code) {
    case ReasonCode::Ok:
        return tr("Das ist erlaubt.");

    // --- general --------------------------------------------------------
    case ReasonCode::NotYourTurn:
        return tr("Du bist noch nicht dran. Zuerst spielt %1.").arg(seat);
    case ReasonCode::NotYourCard:
        return tr("Die %1 liegt nicht in deinem Blatt. Du kannst nur Karten spielen, "
                  "die du selbst auf der Hand hast.").arg(card);
    case ReasonCode::WrongPhase:
        return tr("Das geht jetzt nicht: In dieser Phase des Spiels ist diese Entscheidung "
                  "nicht vorgesehen. Warte, bis du an der Reihe bist.");
    case ReasonCode::SeatSittingOut:
        return tr("Du setzt dieses Spiel aus. Am Fünfertisch gibt der Geber nur den anderen "
                  "vier und ist reiner Zuschauer: Er darf nichts sagen und den Talon nicht "
                  "anschauen.");
    case ReasonCode::UnknownAction:
        return tr("Diese Aktion kennt das Spiel nicht.");

    // --- bidding --------------------------------------------------------
    case ReasonCode::BidTooLow:
        return tr("%1 steht in der Reihenfolge nicht über dem schon angesagten Spiel. "
                  "Du musst höher bieten oder „weiter“ sagen.").arg(game);
    case ReasonCode::ForehandGameOnlyLast:
        return tr("Rufer und Trischaken sind Vorhandspiele: Sie gehen nur, wenn du Vorhand "
                  "bist und alle anderen „weiter“ gesagt haben.");
    case ReasonCode::SechserOnlyFirst:
        return tr("Den Sechserdreier kann die Vorhand nur ganz zu Beginn ansagen, also statt "
                  "„Vorhand“. Jetzt ist er nicht mehr möglich.");
    case ReasonCode::AlreadyPassed:
        return tr("Du hast schon „weiter“ gesagt. Wer einmal gepasst hat, ist aus der "
                  "Lizitation draußen.");
    case ReasonCode::ForehandMustSpeak:
        return tr("Als Vorhand kannst du nicht passen. Sag „Vorhand“, wenn du dir alles "
                  "offenhalten willst, oder sag gleich ein Spiel an.");
    case ReasonCode::HoldOnlyForehand:
        return tr("Halten darf nur die Vorhand. Du musst höher bieten oder „weiter“ sagen.");
    case ReasonCode::HoldNotAllowed:
        return tr("Dieses Spiel kannst du nicht halten. Du musst höher bieten oder "
                  "„weiter“ sagen.");
    case ReasonCode::CallerHasFourKings:
        return tr("Du hast alle vier Könige. Dann kannst du keinen König rufen und darfst "
                  "kein Ruferspiel ansagen. Möglich sind Alleinspiele oder Negativspiele.");
    case ReasonCode::BesserruferWithoutBird:
        return tr("Für den Besserrufer brauchst du schon jetzt einen Vogel im Blatt: "
                  "Pagat (I), Uhu (II), Kakadu (III) oder Quapil (IIII). Du hast keinen davon.");
    case ReasonCode::NoHonour:
        return tr("Für dieses Gebot brauchst du ein Honneur (Sküs, XXI oder I) im Blatt.");
    case ReasonCode::InviterLocked:
        return tr("Dein Gebot ist gebunden: Du hast dich schon festgelegt und kannst es "
                  "jetzt nicht mehr ändern.");
    case ReasonCode::JumpWithoutCard:
        return tr("So hoch darfst du nur springen, wenn du die dafür verlangte Karte "
                  "wirklich im Blatt hast.");
    case ReasonCode::PassIsEngedes:
        return tr("Passen bedeutet hier, dass du das Spiel abgibst. Wenn du weiterspielen "
                  "willst, musst du ein Gebot abgeben.");

    // --- calling a partner ----------------------------------------------
    case ReasonCode::CallOwnKing:
        return tr("Du darfst keinen König rufen, den du selbst hast. Ruf eine Farbe, in der "
                  "dir der König fehlt – am besten eine, in der du die Dame hast.");
    case ReasonCode::FourthKingNeedsThree:
        return tr("„Den vierten König“ darfst du nur rufen, wenn du genau drei Könige in "
                  "der Hand hast.");
    case ReasonCode::CallMustBeXX:
        return tr("Gerufen wird das XX; nur wenn du es selbst hast, geht das nächsttiefere "
                  "Tarock.");
    case ReasonCode::CallOwnSkart:
        return tr("Du darfst keine Karte rufen, die du selbst hast oder gerade weggelegt hast.");
    case ReasonCode::CallHonourForbidden:
        return tr("Ein Honneur (Sküs, XXI, I) darf nicht gerufen werden.");

    // --- talon and discarding -------------------------------------------
    case ReasonCode::TalonHalfTaken:
        return tr("Diese Talonhälfte ist schon vergeben. Die andere Hälfte gehört am Ende "
                  "den Gegnern.");
    case ReasonCode::DiscardKing:
        return tr("Könige darf man nie verlegen. Der %1 bleibt in der Hand. Verlege "
                  "stattdessen kleine Farbkarten – am besten aus einer Farbe, die du dann "
                  "ganz loswirst.").arg(card);
    case ReasonCode::DiscardTrull:
        return tr("Trullstücke (Sküs, Mond, Pagat) darf man nie verlegen. Sie zählen je "
                  "5 Punkte und gehören zur Trull.");
    case ReasonCode::DiscardTarockWhileSuit:
        return tr("Tarock darfst du nur verlegen, wenn du keine Farbkarten mehr verlegen "
                  "kannst. Du hast noch %1. Und wenn es einmal nötig ist, musst du das "
                  "Tarock offen hinlegen.").arg(cards);
    case ReasonCode::DiscardSuitWhileTarock:
        return tr("Im Farbendreier ist es umgekehrt: Zuerst müssen Tarock weg (verdeckt), "
                  "Farbkarten erst, wenn du keine verlegbaren Tarock mehr hast – und dann "
                  "offen. Du hast noch %1.").arg(cards);
    case ReasonCode::DiscardCount:
        if (reason.trick >= 0)
            return tr("Du musst genau %1 Karten verlegen, im Moment sind es %2.")
                .arg(count).arg(static_cast<int>(reason.trick));
        return tr("Du musst genau %1 Karten verlegen.").arg(count);
    case ReasonCode::DiscardXX:
        return tr("Das XX darf nicht verlegt werden.");
    case ReasonCode::DiscardInvitCard:
        return tr("Die gerufene Karte darf nicht verlegt werden.");
    case ReasonCode::DiscardNotHeld:
        return tr("Die %1 liegt nicht in deinem Blatt; verlegen kannst du nur eigene Karten.")
            .arg(card);

    // --- announcements and kontra ---------------------------------------
    case ReasonCode::BonusCardNotHeld:
        return tr("%1 darfst du nur ansagen, wenn du die Karte selbst hast. Für den Partner "
                  "darf man Vögel und König ultimo nicht ansagen.").arg(bonus);
    case ReasonCode::KingUltimoWithoutKing:
        return tr("König ultimo darf nur ansagen, wer den gerufenen %1 selbst in der Hand "
                  "hat.").arg(card);
    case ReasonCode::AnnounceOnlyOnce:
        return tr("Jeder Spieler darf nur einmal Prämien ansagen. Deine Ansage ist schon "
                  "gemacht.");
    case ReasonCode::BesserruferBirdMandatory:
        return tr("Beim Besserrufer musst du deinen Vogel ansagen – und zwar den, den du "
                  "schon vor dem Talon hattest (%1). Ein Vogel aus dem Talon darf nur "
                  "zusätzlich angesagt werden.").arg(cards);
    case ReasonCode::NoBonusInNegative:
        return tr("Im %1 gibt es keine Prämien. Es geht nur um das Stichziel.").arg(game);
    case ReasonCode::SuitGameBonusLimited:
        return tr("Im Farbenspiel sind nur Alle Könige und Valat möglich. Trull und Vögel "
                  "werden hier nicht gewertet.");
    case ReasonCode::SolodreierAnnounceTooLate:
        return tr("Beim Solodreier müssen alle Ansagen sofort mit dem Gebot kommen, weil "
                  "niemand mehr überbieten kann. Nach „Ich liege“ geht nichts mehr.");
    case ReasonCode::KontraBeforeReady:
        return tr("Kontrieren kannst du erst, wenn der Spieler „Ich liege“ gesagt hat.");
    case ReasonCode::KontraOwnAnnouncement:
        return tr("Die eigene Ansage kann man nicht kontrieren. Gegen ein Kontra der Gegner "
                  "kannst du „Rekontra“ sagen.");
    case ReasonCode::KontraLevel:
        return tr("Höher als Subkontra geht es nicht: Kontra (×2), Rekontra (×4), "
                  "Subkontra (×8).");
    case ReasonCode::KontraTooLate:
        return tr("Diese Ansage hättest du sofort kontrieren müssen. Jetzt kannst du nur "
                  "noch Ansagen kontrieren, die nach dir gemacht wurden.");
    case ReasonCode::TrischakenNoKontra:
        return tr("Beim Trischaken gibt es kein Kontra – hier spielt jeder für sich.");
    case ReasonCode::SuitGameKontraOrValat:
        return tr("Im Farbenspiel darfst du als Gegenspieler entweder kontrieren oder den "
                  "Valat ansagen, nicht beides.");
    case ReasonCode::TrullNeedsHonour:
        return tr("Die Trull kannst du nur ansagen, wenn du wenigstens ein Trullstück "
                  "(Sküs, Mond, Pagat) selbst im Blatt hast.");
    case ReasonCode::AnnounceAfterVolat:
        return tr("Nach einem angesagten Valat sind keine weiteren Prämien mehr möglich: "
                  "Wer alle Stiche macht, hat ohnehin alles.");
    case ReasonCode::AnnounceRepeat:
        return tr("%1 ist schon angesagt. Zweimal dieselbe Prämie geht nicht.").arg(bonus);
    case ReasonCode::SameTrickTwice:
        return tr("Für denselben Stich ist schon eine Prämie angesagt. Jeder Stich kann "
                  "nur einmal belegt werden.");

    // --- playing cards --------------------------------------------------
    case ReasonCode::FollowSuit:
        // %1, %2 and %5 are the same suit; QString::arg() numbers each
        // argument, so the suit is passed three times.
        return tr("Es gilt Farbzwang: %1 wurde ausgespielt, und du hast noch %2 auf der Hand "
                  "(%3). Du musst eine davon spielen. Die Karte %4 darfst du nur spielen, wenn "
                  "du keine %5 mehr hast.").arg(suit, suit, cards, card, suit);
    case ReasonCode::FollowTarock:
        return tr("Es wurde Tarock ausgespielt. Tarock ist wie eine eigene Farbe: Solange du "
                  "Tarock hast, musst du Tarock zugeben. Erlaubt sind: %1.").arg(cards);
    case ReasonCode::MustTarock:
        return tr("Du hast kein %1 mehr – dann gilt der Tarockzwang: Du musst ein Tarock "
                  "zugeben, auch wenn du damit nichts gewinnst. Erlaubt sind: %2. Erst wenn "
                  "du auch kein Tarock mehr hast, darfst du eine beliebige Karte abwerfen.")
            .arg(suit, cards);
    case ReasonCode::MustOvertrumpSuit:
        return tr("Im %1 gilt Stichzwang: Du musst die höchste Karte im Stich überstechen, "
                  "wenn du es in der ausgespielten Farbe kannst. Möglich wäre: %2.")
            .arg(game, cards);
    case ReasonCode::MustOvertrumpTarock:
        return tr("Im %1 gilt Stichzwang auch beim Tarock: Du musst das höchste Tarock im "
                  "Stich überstechen, wenn du kannst. Möglich wäre: %2.").arg(game, cards);
    case ReasonCode::MustTrumpNegative:
        return tr("Du kannst %1 nicht bedienen. Im %2 musst du dann mit Tarock stechen, "
                  "solange du eines hast. Erlaubt sind: %3.").arg(suit, game, cards);
    case ReasonCode::PagatNotLastTarock:
        return tr("Der Pagat darf im %1 erst als letztes Tarock gespielt werden. Du hast noch "
                  "andere Tarock (%2). Spiel eines davon – der Pagat kommt erst, wenn er dein "
                  "einziges Tarock ist.").arg(game, cards);
    case ReasonCode::SuitGameNoTarockLead:
        return tr("Im Farbenspiel sticht Tarock keine Farbe, und du darfst Tarock erst "
                  "ausspielen, wenn du keine Farbkarte mehr hast. Du hast noch %1.").arg(cards);
    case ReasonCode::BoundMustPlay:
        return tr("Jetzt ist der %1 Stich – der Stich für deinen angesagten %2. Du darfst ihn "
                  "regelkonform spielen, also musst du ihn jetzt auch spielen.")
            .arg(ordinalLastNominative(fromEnd), bonus);
    case ReasonCode::BoundOrder:
        return tr("Du musst jetzt einen deiner angesagten Vögel vorzeitig hergeben. Die Regel "
                  "lautet: zuerst der höhere Vogel. Spiel also %1.").arg(cards);
    case ReasonCode::BoundWinsTooEarly:
        return tr("Mit dem angesagten %1 darfst du keinen früheren Stich gewinnen – das ist "
                  "der einzige Fall, den die Regeln ausdrücklich verbieten („Pagat vorzeitig "
                  "heimstechen, um Trull und Spiel zu retten“). Spiel eine andere Karte: %2.")
            .arg(bonus, cards);

    // --- warnings: allowed, but it costs a bonus ------------------------
    case ReasonCode::W_BirdReserved:
        return tr("Du hast %1 angesagt. Wenn du ihn jetzt weggibst, ist die Prämie verloren – "
                  "erlaubt ist es aber. Behalte ihn bis zum %2 Stich, oder spiel eine andere "
                  "Karte: %3.").arg(bonus, ordinalLast(fromEnd), cards);
    case ReasonCode::W_KingUltimoReserved:
        return tr("Du hast König ultimo angesagt. Gibst du den %1 jetzt her, ist die Prämie "
                  "weg – verboten ist es nicht.").arg(card);
    case ReasonCode::W_GivesUpSilentBonus:
        return tr("Damit gibst du die stille Prämie %1 auf. Erlaubt ist der Zug, er kostet "
                  "aber die halbe Prämie.").arg(bonus);

    // --- hints ----------------------------------------------------------
    case ReasonCode::I_DiscardShownOpen:
        return tr("Dieses Tarock wird beim Verlegen offen hingelegt – alle sehen es. Das ist "
                  "Vorschrift.");
    case ReasonCode::I_TalonHalfGoesToOpponents:
        return tr("Denk daran: Die liegen gelassene Talonhälfte zählt am Ende zu den Stichen "
                  "der Gegner. Sie entscheidet oft das Spiel.");
    case ReasonCode::I_SilentFigure:
        return tr("Diese Prämie kannst du auch still erzielen – ohne Ansage zählt sie die "
                  "Hälfte, kann dafür aber nicht verloren gehen.");
    case ReasonCode::I_VolatSilentRule:
        return tr("Ein stiller Valat wird auch ohne Ansage gewertet; angesagt zählt er "
                  "doppelt, misslungen kostet er aber genauso viel.");
    }

    // A code the rule book has not spelt out yet: loud, never empty.
    return QString::fromLatin1(kMissingPrefix)
        + QString::fromLatin1(tarock::reasonKey(reason.code)) + QLatin1String("]]");
}

// --- the rest of the table ----------------------------------------------------------

bool ReasonText::isPlaceholder(const QString& text)
{
    return text.isEmpty() || text.startsWith(QLatin1String(kMissingPrefix));
}

QString ReasonText::shortLabel(ReasonCode code)
{
    const Entry* entry = entryFor(code);
    if (!entry)
        return QString::fromLatin1(kMissingPrefix)
            + QString::fromLatin1(tarock::reasonKey(code)) + QLatin1String("]]");
    return tr(entry->label);
}

int ReasonText::severity(ReasonCode code)
{
    return static_cast<int>(tarock::severityOf(code));
}

QString ReasonText::anchor(ReasonCode code, ProfileId profile)
{
    const Entry* entry = entryFor(code);
    return QLatin1String("rules:") + QLatin1String(profileStem(profile)) + QLatin1Char('#')
        + QLatin1String(entry ? entry->anchor : "1");
}

QString ReasonText::specKey(ReasonCode code, ProfileId profile)
{
    const Entry* entry = entryFor(code);
    if (!entry || entry->specKey[0] == '\0')
        return QString();
    // Only the Königrufen keys of §11.4 are written down so far; the Hungarian
    // profile fills its own column of the table in design.md §7.3 here.
    return profile == ProfileId::AtKrOoe2023 ? QString::fromLatin1(entry->specKey) : QString();
}

tarock::ReasonCode ReasonText::codeForSpecKey(const QString& key)
{
    if (key.isEmpty())
        return ReasonCode::Ok;
    const QByteArray latin = key.trimmed().toLatin1();
    for (const Entry& entry : kEntries) {
        if (latin == entry.specKey || latin == tarock::reasonKey(entry.code))
            return entry.code;
    }
    return ReasonCode::Ok;
}

QVariantMap ReasonText::toMap(const Reason& reason, ProfileId profile)
{
    QVariantMap map;
    map.insert(QStringLiteral("code"), static_cast<int>(reason.code));
    map.insert(QStringLiteral("key"), QString::fromLatin1(tarock::reasonKey(reason.code)));
    map.insert(QStringLiteral("specKey"), specKey(reason.code, profile));
    map.insert(QStringLiteral("severity"), severity(reason.code));
    map.insert(QStringLiteral("short"), shortLabel(reason.code));
    map.insert(QStringLiteral("text"), textFor(reason));
    map.insert(QStringLiteral("anchor"), anchor(reason.code, profile));
    QVariantList cards;
    for (Card card : tarock::toList(reason.cards))
        cards.append(static_cast<int>(card.id));
    map.insert(QStringLiteral("cards"), cards);
    map.insert(QStringLiteral("card"), reason.card.valid() ? static_cast<int>(reason.card.id) : -1);
    map.insert(QStringLiteral("seat"), static_cast<int>(reason.seat));
    map.insert(QStringLiteral("suit"), static_cast<int>(reason.suit));
    map.insert(QStringLiteral("bonus"), static_cast<int>(reason.bonus));
    map.insert(QStringLiteral("contract"), static_cast<int>(reason.contract));
    map.insert(QStringLiteral("trick"), static_cast<int>(reason.trick));
    map.insert(QStringLiteral("count"), static_cast<int>(reason.count));
    return map;
}

QVector<ReasonCode> ReasonText::allCodes()
{
    // Walked off tarock::reasonKey(), not off the table above: a code that is
    // missing from the table has to show up here, or the test could not find
    // the hole it is looking for.
    QVector<ReasonCode> codes;
    for (int i = 0; i < 1024; ++i) {
        const ReasonCode code = static_cast<ReasonCode>(i);
        if (qstrcmp(tarock::reasonKey(code), "E_UNKNOWN") == 0)
            break;
        codes.append(code);
    }
    return codes;
}
