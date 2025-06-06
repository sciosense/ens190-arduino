#ifndef SCIOSENSE_ENS190_H
#define SCIOSENSE_ENS190_H

#include <stdint.h>

#include <Arduino.h>
#include <Stream.h>

#include "lib/ens190/ScioSense_Ens190.h"
#include "lib/io/ScioSense_IOInterface_Arduino_Serial.h"

class ENS190 : public ScioSense_Ens190
{
public:
    ENS190();
    virtual ~ENS190();

public:
    inline void begin(Stream* serial);              // Connnects to ENS190 using the given Stream(Serial) object
    inline bool init();                             // Reads PartID, production date, and FirmwareVersion
    bool isConnected();                             // Checks if the read firmware version is plausible; returns true, if so.

public:
    void enableDebugging(Stream& debugStream);      // Enables the debug log. The output is written to the given debugStream
    void disableDebugging();                        // Stops the debug log if enabled. Does nothing otherwise.

public:
    inline void clear();                            // Clears IO buffers of the Stream device

public:
    inline Result       update                  ();                                             // Reads measurement data;
    inline uint16_t     getCo2                  ();                                             // Returns CO2 concentration in ppm
    inline uint8_t*     getFirmwareVersion      ();                                             // Returns a pointer to the firmware version
    inline uint16_t     getSerialNumber         ();                                             // Returns serial number
    inline uint8_t*     getProductionDate       ();                                             // Returns production date
    inline uint16_t     setAbcParameters        (uint16_t periodHrs);                           // Sets the ABC parameters
    inline uint16_t     setAlarmThresholds      (uint16_t highAlarmThresholdPpm, uint16_t lowAlarmThresholdPpm);    // Sets the alarm thresholds
    inline uint16_t     getAlarmThresholdHigh   ();                                             // Returns the value of the higher alarm threshold
    inline uint16_t     getAlarmThresholdLow    ();                                             // Returns the value of the lower alarm threshold
    inline uint16_t     setCo2Baseline          (uint16_t co2BaselinePpm);                      // Sets the CO2 Baseline
    inline uint16_t     setBaudrate             (Ens190_Baudrate baudrate);                     // Sets the Baudrate

protected:
    ScioSense_Arduino_Serial_Config     serialConfig;

private:
    Stream* debugStream;
};

#include "ens190.inl.h"

#endif //SCIOSENSE_ENS190_H