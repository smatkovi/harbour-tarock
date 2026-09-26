#pragma once

// Der Tisch über Netz oder Bluetooth (docs/design.md §8): ein Gerät führt die
// Partie, die anderen sitzen mit. Diese Klasse kennt die Regeln nicht -- sie
// verteilt Plätze, trägt Nachrichten und sagt der Engine, was angekommen ist.
//
// Abweichung vom Entwurf §8.2: statt `act` samt gefiltertem Vorzustand (`pre`)
// schickt der Gastgeber nach jeder Änderung den **ganzen** sichtgefilterten
// Zustand (TarockCore::serializeFor). Ein Zustand ist knapp ein Kilobyte, bei
// höchstens einer Aktion pro Sekunde also auch über RFCOMM reichlich schnell,
// und der Gast braucht keinen Kern, der fremde Aktionen auf verdeckte Hände
// anwenden könnte. Was ein Gast nicht sehen darf, verlässt das Gerät des
// Gastgebers damit nie -- auch nicht in einer manipulierten App.

#include "LanSession.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

class LanTable : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int role READ roleValue NOTIFY changed)
    Q_PROPERTY(bool connected READ connected NOTIFY changed)
    Q_PROPERTY(bool hosting READ hosting NOTIFY changed)
    Q_PROPERTY(bool guest READ isGuest NOTIFY changed)
    Q_PROPERTY(QString status READ status NOTIFY changed)
    Q_PROPERTY(int mySeat READ mySeat NOTIFY changed)
    Q_PROPERTY(int seatCount READ seatCount NOTIFY changed)
    Q_PROPERTY(QVariantList lobby READ lobby NOTIFY changed)
    Q_PROPERTY(QObject* browser READ browserObject CONSTANT)
    Q_PROPERTY(QObject* bluetooth READ bluetoothObject CONSTANT)
    Q_PROPERTY(bool bluetoothOpen READ bluetoothOpen NOTIFY changed)

public:
    enum Role { None = 0, Host = 1, Guest = 2 };

    explicit LanTable(QObject* parent = 0);

    Role role() const { return m_role; }
    int roleValue() const { return int(m_role); }
    bool connected() const { return m_session.peerConnected(); }
    bool hosting() const { return m_role == Host; }
    bool isGuest() const { return m_role == Guest; }
    QString status() const { return m_status; }
    int mySeat() const { return m_mySeat; }
    int seatCount() const { return m_players; }
    // Die Regeln, unter denen dieser Tisch aufgemacht wurde -- nicht die aus
    // den Einstellungen: am Tisch wird eigens gewählt.
    QString tableProfileKey() const { return m_profileKey; }
    // Ein Eintrag je Platz: {seat, name, human}.
    QVariantList lobby() const;
    QObject* browserObject() { return &m_browser; }
    QObject* bluetoothObject() { return &m_bluetooth; }
    bool bluetoothOpen() const { return m_session.bluetoothHosting(); }

    // --- Gastgeber ---------------------------------------------------------
    bool startHosting(const QString& name, const QString& profileKey, int players, QString* error);
    // Ab hier kommt niemand mehr dazu; freie Plätze spielt der Computer.
    void closeTable();
    // Der Zustand, wie dieser Platz ihn sehen darf, plus die laufende Nummer
    // und der Stand der Partie (Schrift und Geld, für alle Plätze gedreht).
    void sendView(int seat, const QString& state, const QVariantList& schrift,
                  const QVariantList& geld);
    void sendStart();
    void sendNack(int seat, const QString& reason);
    void announceNames();
    // Welcher Platz ist von einem Gast besetzt?
    bool seatIsGuest(int seat) const;
    QString seatName(int seat) const;

    // --- Gast --------------------------------------------------------------
    void join(const QString& address, const QString& name);
    void joinBluetooth(const QString& address, const QString& name);
    void sendRequest(const QString& type, int a, int b);

    void leave(const QString& reason);

signals:
    void changed();
    // Gastgeber: ein Gast wünscht sich eine Aktion auf seinem Platz.
    void requestArrived(int seat, const QString& type, int a, int b);
    void seatsChanged();
    // Gast: ein neuer Sichtzustand ist da, oder die Partie beginnt/endet.
    void viewArrived(const QString& state, const QVariantList& schrift,
                     const QVariantList& geld);
    void matchStarted(const QString& profileKey, int players, int seat);
    void tableClosed(const QString& reason);
    void refused(const QString& reason);

private slots:
    void onPeerJoined(int peer);
    void onPeerLost(int peer);
    void onMessage(int peer, const QVariantMap& message);
    void onConnectionFailed(const QString& reason);

private:
    void setStatus(const QString& text);
    int seatOfPeer(int peer) const;
    int freeSeat() const;

    LanSession m_session;
    LanBrowser m_browser;
    BtDevices m_bluetooth;
    Role m_role;
    QString m_status;
    QString m_name;
    QString m_profileKey;
    int m_players;
    int m_mySeat;          // beim Gast: der eigene Platz am Tisch des Gastgebers
    bool m_open;           // nimmt der Tisch noch Gäste?
    bool m_started;
    // Platz -> Gegenstelle (-1 = frei/Computer), und der Name dazu.
    QList<int> m_seatPeer;
    QStringList m_seatNames;
    int m_sequence;
};
