#ifndef SCIOSENSE_ENS190_C_H
#define SCIOSENSE_ENS190_C_H

#include "ScioSense_Ens190_defines.h"

#include <stdbool.h>
#include <inttypes.h>

typedef struct ScioSense_Ens190_IO
{
    Result  (*read)     (void* config, const uint16_t address, uint8_t* data, const size_t size);
    Result  (*write)    (void* config, const uint16_t address, uint8_t* data, const size_t size);
    Result  (*clear)    (void* config);
    void    (*wait)     (const uint32_t ms);
    void* config;
} ScioSense_Ens190_IO;

typedef struct ScioSense_Ens190
{
    ScioSense_Ens190_IO     io;
    uint8_t                 dataBuffer      [ENS190_COMMAND_RESPONSE_CO2_VALUE_PAYLOAD];
    uint8_t                 fwVersion       [ENS190_COMMAND_RESPONSE_FIRMWARE_VERSION_NUMBER_PAYLOAD];
    uint16_t                serialNumber;
    uint32_t                productionDate;
} ScioSense_Ens190;

static inline Result    Ens190_Update                       (ScioSense_Ens190* ens190);                                             // Reads measurement data;
static inline Result    Ens190_Reset                        (ScioSense_Ens190* ens190);                                             // Clear variables and read the sensor serial number and firmware version
static inline Result    Ens190_InvokeReadSerialNumber       (ScioSense_Ens190* ens190);                                             // Reads the serial number and production date
static inline Result    Ens190_InvokeReadFirmwareVersion    (ScioSense_Ens190* ens190);                                             // Reads the firmware version
static inline bool      Ens190_IsConnected                  (ScioSense_Ens190* ens190);                                             // Checks if the read firmware version is plausible
static inline uint16_t  Ens190_GetCo2                       (ScioSense_Ens190* ens190);                                             // Returns CO2 concentration in ppm
static inline uint8_t*  Ens190_GetFirmwareVersion           (ScioSense_Ens190* ens190);                                             // Returns a pointer to the firmware version
static inline uint16_t  Ens190_GetSerialNumber              (ScioSense_Ens190* ens190);                                             // Returns serial number
static inline uint32_t  Ens190_GetProductionDate            (ScioSense_Ens190* ens190);                                             // Returns production date

static inline Result    Ens190_CheckData                    (const uint8_t* data, const Ens190_CommandResponse size);               // calculates the checksum of the data and compares it with the last 2 byte; returns RESULT_CHECKSUM_ERROR on failure
static inline Result    Ens190_CheckCommandResponse         (const Ens190_Command command, const uint8_t* data, const Ens190_CommandResponse size); // checks if the data corresponds to the command result protocol and calculates the checksum thereafter; returns RESULT_INVALID if the protocol does not match

#include "ScioSense_Ens190.inl.h"
#endif // SCIOSENSE_ENS190_C_H