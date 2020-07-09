/*
 *  Copyright (C) 2014 Kyle Manna <kyle@kylemanna.com>
 *  Copyright (C) 2017 KeePassXC Team <team@keepassxc.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KEEPASSX_FIDOKEY_H
#define KEEPASSX_FIDOKEY_H

#include <QMutex>
#include <QObject>

/**
 * Singleton class to manage the interface to the hardware
 */
class FIDOKey : public QObject
{
    Q_OBJECT

public:
    enum ChallengeResult
    {
        ERROR = -1,
        SUCCESS = 0,
        WOULDBLOCK,
        ALREADY_RUNNING
    };

    /**
     * @brief FIDOKey::instance - get instance of singleton
     * @return instance
     */
    static FIDOKey* instance();

    /**
     * @brief FIDOKey::init - initialize yubikey library and hardware
     * @return true on success
     */
    bool init();

    /**
     * @brief FIDOKey::deinit - cleanup after init
     * @return true on success
     */
    bool deinit();

    /**
     * @brief FIDOKey::challenge - issue a challenge
     *
     * This operation could block if the FIDOKey requires a touch to trigger.
     *
     * TODO: Signal to the UI that the system is waiting for challenge response
     *       touch.
     *
     * @param slot FIDOKey configuration slot
     * @param mayBlock operation is allowed to block
     * @param challenge challenge input to FIDOKey
     * @param response response output from FIDOKey
     * @return challenge result
     */
    ChallengeResult challenge(int slot, bool mayBlock, const QByteArray& challenge, QByteArray& response);

    /**
     * @brief FIDOKey::getSerial - serial number of FIDOKey
     * @param serial serial number
     * @return true on success
     */
    bool getSerial(unsigned int& serial);

    /**
     * @brief FIDOKey::getVendorName - vendor name of token
     * @return vendor name
     */
    QString getVendorName();

    /**
     * @brief FIDOKey::detect - probe for attached FIDOKeys
     */
    void detect();

    /**
     * @param slot the yubikey slot.
     * @param errorMessage populated if an error occured.
     *
     * @return whether the key is blocking (touch needed) or not.
     */
    bool checkSlotIsBlocking(int slot, QString& errorMessage);
signals:
    /** Emitted in response to detect() when a device is found
     *
     * @slot is the slot number detected
     * @blocking signifies if the YK is setup in passive mode or if requires
     *           the user to touch it for a response
     */
    void detected(int slot, bool blocking);

    /**
     * Emitted when detection is complete
     */
    void detectComplete();

    /**
     * Emitted when no Yubikey could be found.
     */
    void notFound();

private:
    explicit FIDOKey();
    static FIDOKey* m_instance;

    // Create void ptr here to avoid ifdef header include mess
    struct fido_dev * m_dev;

    QMutex m_mutex;

    Q_DISABLE_COPY(FIDOKey)
};

#endif // KEEPASSX_FIDOKEY_H