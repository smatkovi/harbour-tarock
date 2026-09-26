#include "LanTable.h"

namespace {

// Wird erhöht, wenn sich die Nachrichten so ändern, dass eine ältere App
// nicht mehr mitkommt.
const int kProtocolVersion = 1;

QString key(const char* name)
{
    return QString::fromLatin1(name);
}

} // namespace

LanTable::LanTable(QObject* parent)
    : QObject(parent)
    , m_role(None)
    , m_players(4)
    , m_mySeat(-1)
    , m_open(false)
    , m_started(false)
    , m_sequence(0)
{
    connect(&m_session, SIGNAL(peerJoined(int)), this, SLOT(onPeerJoined(int)));
    connect(&m_session, SIGNAL(peerLost(int)), this, SLOT(onPeerLost(int)));
    connect(&m_session, SIGNAL(messageReceived(int,QVariantMap)),
            this, SLOT(onMessage(int,QVariantMap)));
    connect(&m_session, SIGNAL(connectionFailed(QString)),
            this, SLOT(onConnectionFailed(QString)));
}

void LanTable::setStatus(const QString& text)
{
    m_status = text;
    emit changed();
}

QVariantList LanTable::lobby() const
{
    QVariantList result;
    if (m_role == None)
        return result;
    for (int seat = 0; seat < m_players; ++seat) {
        QVariantMap entry;
        entry.insert(key("seat"), seat);
        entry.insert(key("name"), seatName(seat));
        entry.insert(key("human"), seat == 0 || seatIsGuest(seat)
                     || (m_role == Guest && seat == m_mySeat));
        entry.insert(key("isMe"), m_role == Host ? seat == 0 : seat == m_mySeat);
        result.append(entry);
    }
    return result;
}

QString LanTable::seatName(int seat) const
{
    if (seat >= 0 && seat < m_seatNames.size() && !m_seatNames.at(seat).isEmpty())
        return m_seatNames.at(seat);
    return tr("Computer");
}

bool LanTable::seatIsGuest(int seat) const
{
    return seat >= 0 && seat < m_seatPeer.size() && m_seatPeer.at(seat) >= 0;
}

int LanTable::seatOfPeer(int peer) const
{
    for (int seat = 0; seat < m_seatPeer.size(); ++seat) {
        if (m_seatPeer.at(seat) == peer)
            return seat;
    }
    return -1;
}

int LanTable::freeSeat() const
{
    for (int seat = 1; seat < m_players; ++seat) {
        if (!seatIsGuest(seat))
            return seat;
    }
    return -1;
}

bool LanTable::startHosting(const QString& name, const QString& profileKey, int players,
                            QString* error)
{
    leave(QString());
    m_name = name.trimmed().isEmpty() ? tr("Gastgeber") : name.trimmed();
    m_profileKey = profileKey;
    m_players = players;
    m_seatPeer.clear();
    m_seatNames.clear();
    for (int seat = 0; seat < players; ++seat) {
        m_seatPeer.append(-1);
        m_seatNames.append(seat == 0 ? m_name : QString());
    }
    if (!m_session.startHosting(m_name, players, players - 1, error))
        return false;
    m_role = Host;
    m_mySeat = 0;
    m_open = true;
    m_started = false;
    m_sequence = 0;
    setStatus(tr("Warte auf Mitspieler …"));
    emit seatsChanged();
    return true;
}

void LanTable::closeTable()
{
    m_open = false;
    m_started = true;
    m_session.setAcceptingGuests(false);
    sendStart();
    setStatus(tr("Die Partie läuft"));
}

void LanTable::sendStart()
{
    if (m_role != Host)
        return;
    QVariantMap start;
    start.insert(key("t"), key("start"));
    start.insert(key("players"), m_players);
    start.insert(key("profile"), m_profileKey);
    m_session.send(start);
}

void LanTable::announceNames()
{
    if (m_role != Host)
        return;
    QVariantMap names;
    names.insert(key("t"), key("names"));
    names.insert(key("names"), QVariant(m_seatNames));
    m_session.send(names);
    emit changed();
}

void LanTable::sendView(int seat, const QString& state, const QVariantList& schrift,
                        const QVariantList& geld)
{
    if (m_role != Host || !seatIsGuest(seat))
        return;
    QVariantMap view;
    view.insert(key("t"), key("view"));
    view.insert(key("s"), ++m_sequence);
    view.insert(key("state"), state);
    view.insert(key("schrift"), schrift);
    view.insert(key("geld"), geld);
    m_session.sendTo(m_seatPeer.at(seat), view);
}

void LanTable::sendNack(int seat, const QString& reason)
{
    if (m_role != Host || !seatIsGuest(seat))
        return;
    QVariantMap nack;
    nack.insert(key("t"), key("nack"));
    nack.insert(key("reason"), reason);
    m_session.sendTo(m_seatPeer.at(seat), nack);
}

void LanTable::join(const QString& address, const QString& name)
{
    leave(QString());
    m_name = name.trimmed().isEmpty() ? tr("Gast") : name.trimmed();
    m_role = Guest;
    m_mySeat = -1;
    setStatus(tr("Verbinde mit %1 …").arg(address));
    m_session.joinHost(address);
}

void LanTable::joinBluetooth(const QString& address, const QString& name)
{
    leave(QString());
    m_name = name.trimmed().isEmpty() ? tr("Gast") : name.trimmed();
    m_role = Guest;
    m_mySeat = -1;
    setStatus(tr("Verbinde über Bluetooth …"));
    m_session.joinBluetooth(address);
}

void LanTable::sendRequest(const QString& type, int a, int b)
{
    if (m_role != Guest)
        return;
    QVariantMap request;
    request.insert(key("t"), key("req"));
    request.insert(key("type"), type);
    request.insert(key("a"), a);
    request.insert(key("b"), b);
    m_session.send(request);
}

void LanTable::leave(const QString& reason)
{
    if (m_session.peerConnected()) {
        QVariantMap bye;
        bye.insert(key("t"), key("bye"));
        bye.insert(key("reason"), reason);
        m_session.send(bye);
    }
    m_session.stop();
    m_role = None;
    m_mySeat = -1;
    m_open = false;
    m_started = false;
    m_seatPeer.clear();
    m_seatNames.clear();
    setStatus(reason);
    emit seatsChanged();
}

void LanTable::onPeerJoined(int peer)
{
    if (m_role != Guest)
        return;
    QVariantMap hello;
    hello.insert(key("t"), key("hello"));
    hello.insert(key("kind"), key("tarock"));
    hello.insert(key("v"), kProtocolVersion);
    hello.insert(key("name"), m_name);
    m_session.sendTo(peer, hello);
    setStatus(tr("Verbunden, warte auf einen Platz …"));
}

void LanTable::onPeerLost(int peer)
{
    if (m_role == Host) {
        const int seat = seatOfPeer(peer);
        if (seat >= 0) {
            m_seatPeer[seat] = -1;
            m_seatNames[seat] = QString();
            // Der Platz spielt ab jetzt vom Computer weiter (§8.4).
            setStatus(tr("Ein Mitspieler ist weg, der Computer übernimmt seinen Platz"));
            emit seatsChanged();
            announceNames();
        }
        return;
    }
    const bool wasPlaying = m_started;
    m_role = None;
    m_started = false;
    m_session.stop();
    setStatus(tr("Die Verbindung zum Gastgeber ist weg"));
    emit tableClosed(m_status);
    Q_UNUSED(wasPlaying);
}

void LanTable::onConnectionFailed(const QString& reason)
{
    m_role = None;
    setStatus(reason);
    emit refused(reason);
}

void LanTable::onMessage(int peer, const QVariantMap& message)
{
    const QString type = message.value(key("t")).toString();

    if (m_role == Host && type == QLatin1String("hello")) {
        if (message.value(key("kind")).toString() != QLatin1String("tarock")
                || message.value(key("v")).toInt() != kProtocolVersion) {
            QVariantMap bad;
            bad.insert(key("t"), key("version"));
            m_session.sendTo(peer, bad);
            m_session.dropPeer(peer);
            return;
        }
        const int seat = m_open ? freeSeat() : -1;
        if (seat < 0) {
            QVariantMap busy;
            busy.insert(key("t"), key("busy"));
            m_session.sendTo(peer, busy);
            m_session.dropPeer(peer);
            return;
        }
        m_seatPeer[seat] = peer;
        const QString name = message.value(key("name")).toString();
        m_seatNames[seat] = name.isEmpty() ? tr("Gast %1").arg(seat) : name;

        QVariantMap welcome;
        welcome.insert(key("t"), key("welcome"));
        welcome.insert(key("v"), kProtocolVersion);
        welcome.insert(key("seat"), seat);
        welcome.insert(key("players"), m_players);
        welcome.insert(key("profile"), m_profileKey);
        welcome.insert(key("names"), QVariant(m_seatNames));
        m_session.sendTo(peer, welcome);
        setStatus(tr("%1 sitzt auf Platz %2").arg(m_seatNames.at(seat)).arg(seat + 1));
        emit seatsChanged();
        announceNames();
        return;
    }

    if (m_role == Host && type == QLatin1String("req")) {
        const int seat = seatOfPeer(peer);
        if (seat < 0)
            return;
        emit requestArrived(seat, message.value(key("type")).toString(),
                            message.value(key("a")).toInt(), message.value(key("b")).toInt());
        return;
    }

    if (m_role == Guest && type == QLatin1String("welcome")) {
        m_mySeat = message.value(key("seat")).toInt();
        m_players = message.value(key("players")).toInt();
        m_profileKey = message.value(key("profile")).toString();
        m_seatNames = message.value(key("names")).toStringList();
        m_seatPeer.clear();
        for (int seat = 0; seat < m_players; ++seat)
            m_seatPeer.append(-1);
        setStatus(tr("Platz %1 am Tisch, warte auf den Anfang …").arg(m_mySeat + 1));
        emit seatsChanged();
        return;
    }

    if (m_role == Guest && type == QLatin1String("names")) {
        m_seatNames = message.value(key("names")).toStringList();
        emit changed();
        return;
    }

    if (m_role == Guest && type == QLatin1String("start")) {
        m_started = true;
        m_players = message.value(key("players")).toInt();
        setStatus(tr("Die Partie läuft"));
        emit matchStarted(message.value(key("profile")).toString(), m_players, m_mySeat);
        return;
    }

    if (m_role == Guest && type == QLatin1String("view")) {
        emit viewArrived(message.value(key("state")).toString(),
                         message.value(key("schrift")).toList(),
                         message.value(key("geld")).toList());
        return;
    }

    if (m_role == Guest && type == QLatin1String("nack")) {
        emit refused(message.value(key("reason")).toString());
        return;
    }

    if (type == QLatin1String("busy")) {
        m_session.stop();
        m_role = None;
        setStatus(tr("Der Tisch ist voll oder die Partie läuft schon"));
        emit refused(m_status);
        return;
    }

    if (type == QLatin1String("version")) {
        m_session.stop();
        m_role = None;
        setStatus(tr("Das andere Gerät hat eine andere Fassung der App"));
        emit refused(m_status);
        return;
    }

    if (type == QLatin1String("bye")) {
        if (m_role == Host) {
            onPeerLost(peer);
            return;
        }
        const QString reason = message.value(key("reason")).toString();
        m_session.stop();
        m_role = None;
        m_started = false;
        setStatus(reason.isEmpty() ? tr("Der Gastgeber hat den Tisch geschlossen") : reason);
        emit tableClosed(m_status);
        return;
    }
}
