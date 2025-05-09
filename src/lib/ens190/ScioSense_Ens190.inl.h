#ifndef SCIOSENSE_ENS190_C_INL
#define SCIOSENSE_ENS190_C_INL

#include "ScioSense_ENS190.h"

#include <math.h>

#define clear()             if (ens190->io.clear) { ens190->io.clear(ens190->io.config); }
#define wait(ms)            ens190->io.wait(ms)

#define hasAnyFlag(a, b)    (((a) & (b)) != 0)
#define hasFlag(a, b)       (((a) & (b)) == (b))

#define memset(a, b, s)     for(size_t i = 0; i < s; i++) {a[i] = b;}
#define memcpy(a, b, s)     for(size_t i = 0; i < s; i++) {a[i] = b[i];}

static inline uint16_t Ens190_GetValueOf16(const uint8_t* data, const uint16_t resultAddress)
{
    return ((uint16_t)data[resultAddress] << 8) + (uint16_t)data[resultAddress + 1];
}

static inline uint32_t Ens190_GetValueOf32(uint8_t* data, const uint16_t resultAddress)
{
    return  ((uint32_t)data[resultAddress + 0] << 24)
          + ((uint32_t)data[resultAddress + 1] << 16)
          + ((uint32_t)data[resultAddress + 2] <<  8)
          +  (uint32_t)data[resultAddress + 3];
}

static inline uint64_t Ens190_GetValueOf64(uint8_t* data, const uint16_t resultAddress)
{
    return  ((uint64_t)data[resultAddress + 0] << 56)
          + ((uint64_t)data[resultAddress + 1] << 48)
          + ((uint64_t)data[resultAddress + 2] << 40)
          + ((uint64_t)data[resultAddress + 3] << 32)
          + ((uint64_t)data[resultAddress + 4] << 24)
          + ((uint64_t)data[resultAddress + 5] << 16)
          + ((uint64_t)data[resultAddress + 6] << 8)
          +  (uint64_t)data[resultAddress + 7];
}

static inline Result Ens190_Read(ScioSense_Ens190* ens190, uint8_t* data, const size_t size)
{
    return ens190->io.read(ens190->io.config, 0, data, size);
}

static inline Result Ens190_Write(ScioSense_Ens190* ens190, uint8_t* data, const size_t size)
{
    return ens190->io.write(ens190->io.config, 0, data, size);
}

static inline Result Ens190_Invoke(ScioSense_Ens190* ens190, Ens190_Command command, const size_t sizeCommand, uint8_t* resultBuf, const size_t sizeResponse)
{
    Result result;

    result = Ens190_Write(ens190, (uint8_t*)command, sizeCommand);

    if (result == RESULT_OK)
    {
        if (resultBuf != NULL)
        {
            result = Ens190_Read(ens190, resultBuf, sizeResponse);
            if (result == RESULT_OK)
            {
                result = Ens190_CheckCommandResponse(command, resultBuf, (Ens190_CommandResponse)sizeResponse);
            }
        }
    }

    return result;
}

static inline Result Ens190_Reset(ScioSense_Ens190* ens190)
{
    Result result;

    memset(ens190->fwVersion ,      0, ENS190_COMMAND_RESPONSE_FIRMWARE_VERSION_NUMBER_PAYLOAD);
    memset(ens190->dataBuffer,      0, ENS190_COMMAND_RESPONSE_CO2_VALUE_PAYLOAD);

    ens190->serialNumber    = 0;
    ens190->productionDate  = 0;

    clear();

    result = Ens190_InvokeReadSerialNumber(ens190);
    if ( result == RESULT_OK )
    {
        result = Ens190_InvokeReadFirmwareVersion(ens190);        
    }

    return result;

}

static inline Result Ens190_InvokeReadSerialNumber(ScioSense_Ens190* ens190)
{
    Result result;
    static const Ens190_Command command = ENS190_COMMAND_GET_SERIAL_NUMBER;
    uint8_t buff[ENS190_COMMAND_RESPONSE_SERIAL_NUMBER_LENGTH];

    result = Ens190_Invoke(ens190, command, ENS190_COMMAND_GET_SERIAL_NUMBER_LENGTH, buff, ENS190_COMMAND_RESPONSE_SERIAL_NUMBER_LENGTH);
    if (
            buff[ENS190_COMMAND_RESPONSE_START_BYTE_ADDRESS]    ==  ENS190_COMMAND_ADDRESS_START_BYTE
        &&  buff[ENS190_COMMAND_RESPONSE_LENGTH_BYTE_ADDRESS]   ==  ( ENS190_COMMAND_RESPONSE_SERIAL_NUMBER_PAYLOAD + 1 )
        &&  buff[ENS190_COMMAND_RESPONSE_COMMAND_BYTE_ADDRESS]  ==  ENS190_COMMAND_ADDRESS_GET_SERIAL_NUMBER
    )
    {
        result = Ens190_CheckData(buff, ENS190_COMMAND_RESPONSE_SERIAL_NUMBER_LENGTH);
        if (result == RESULT_OK)
        {            
            ens190->productionDate  = Ens190_GetValueOf32(buff, ENS190_COMMAND_RESPONSE_PRODUCTION_DATE_BYTE_ADDRESS);
            ens190->serialNumber    = Ens190_GetValueOf16(buff, ENS190_COMMAND_RESPONSE_SERIAL_NUMBER_BYTE_ADDRESS);
        }
    }

    return result;
}

static inline Result Ens190_InvokeReadFirmwareVersion(ScioSense_Ens190* ens190)
{
    Result result;

    static const Ens190_Command command = ENS190_COMMAND_GET_FIRMWARE_VERSION_NUMBER;
    uint8_t buff[ENS190_COMMAND_RESPONSE_FIRMWARE_VERSION_NUMBER_LENGTH];

    result = Ens190_Invoke(ens190, command, ENS190_COMMAND_GET_FIRMWARE_VERSION_NUMBER_LENGTH, buff, ENS190_COMMAND_RESPONSE_FIRMWARE_VERSION_NUMBER_LENGTH);
    if (
            buff[ENS190_COMMAND_RESPONSE_START_BYTE_ADDRESS]    ==  ENS190_COMMAND_ADDRESS_START_BYTE
        &&  buff[ENS190_COMMAND_RESPONSE_LENGTH_BYTE_ADDRESS]   ==  ( ENS190_COMMAND_RESPONSE_FIRMWARE_VERSION_NUMBER_PAYLOAD + 1 )
        &&  buff[ENS190_COMMAND_RESPONSE_COMMAND_BYTE_ADDRESS]  ==  ENS190_COMMAND_ADDRESS_GET_FIRMWARE_VERSION_NUMBER
)
    {
        result = Ens190_CheckData(buff, ENS190_COMMAND_RESPONSE_FIRMWARE_VERSION_NUMBER_LENGTH);
        if (result == RESULT_OK)
        {
            memcpy(ens190->fwVersion, (buff + ENS190_COMMAND_RESPONSE_DATA_BYTE_ADDRESS), ENS190_COMMAND_RESPONSE_FIRMWARE_VERSION_NUMBER_PAYLOAD);
        }
    }

    return result;
}

static inline Result Ens190_Update(ScioSense_Ens190* ens190)
{
    Result result;

    static const Ens190_Command command = ENS190_COMMAND_GET_CO2_VALUE;
    uint8_t buff[ENS190_COMMAND_RESPONSE_CO2_VALUE_LENGTH];

    result = Ens190_Invoke(ens190, command, ENS190_COMMAND_GET_CO2_VALUE_LENGTH, buff, ENS190_COMMAND_RESPONSE_CO2_VALUE_LENGTH);
    if (
            buff[ENS190_COMMAND_RESPONSE_START_BYTE_ADDRESS]    ==  ENS190_COMMAND_ADDRESS_START_BYTE
        &&  buff[ENS190_COMMAND_RESPONSE_LENGTH_BYTE_ADDRESS]   ==  ( ENS190_COMMAND_RESPONSE_CO2_VALUE_PAYLOAD + 1 )
        &&  buff[ENS190_COMMAND_RESPONSE_COMMAND_BYTE_ADDRESS]  ==  ENS190_COMMAND_ADDRESS_GET_CO2_VALUE
)
    {
        result = Ens190_CheckData(buff, ENS190_COMMAND_RESPONSE_CO2_VALUE_LENGTH);
        if (result == RESULT_OK)
        {
            memcpy(ens190->dataBuffer, (buff + ENS190_COMMAND_RESPONSE_DATA_BYTE_ADDRESS), ENS190_COMMAND_RESPONSE_CO2_VALUE_PAYLOAD);
        }
    }

    return result;
}

static inline bool Ens190_IsConnected(ScioSense_Ens190* ens190)
{
    return ens190->fwVersion[0] != 0;
}

static inline uint16_t Ens190_GetCo2(ScioSense_Ens190* ens190)
{
    return Ens190_GetValueOf16(ens190->dataBuffer, 0);
}

static inline uint8_t* Ens190_GetFirmwareVersion(ScioSense_Ens190* ens190)
{
    return ens190->fwVersion;
}

static inline uint16_t Ens190_GetSerialNumber(ScioSense_Ens190* ens190)
{
    return ens190->serialNumber;
}

static inline uint32_t Ens190_GetProductionDate(ScioSense_Ens190* ens190)
{
    return ens190->productionDate;
}

static inline Result Ens190_CheckData(const uint8_t* data, const Ens190_CommandResponse size)
{
    if (size < 3)
    {
        return RESULT_CHECKSUM_ERROR;
    }

    const uint8_t payloadSize   = ((uint8_t)size) - 1;
    const uint8_t checksum     = data[payloadSize];
    int16_t dataSum = 0;

    for (uint8_t i = 0; i<payloadSize; i++)
    {
        dataSum += data[i];
    }

    uint8_t calculatedChecksum = (uint8_t) ( ((uint16_t)size * 256) - dataSum );

    return (checksum == calculatedChecksum) ? RESULT_OK : RESULT_CHECKSUM_ERROR;
}

static inline Result Ens190_CheckCommandResponse(const Ens190_Command command, const uint8_t* data, const Ens190_CommandResponse size)
{
    Result result = RESULT_INVALID;

    if
    (
        command[ENS190_COMMAND_RESPONSE_START_BYTE_ADDRESS]     == data[ENS190_COMMAND_RESPONSE_START_BYTE_ADDRESS]
     && command[ENS190_COMMAND_RESPONSE_COMMAND_BYTE_ADDRESS]   == data[ENS190_COMMAND_RESPONSE_COMMAND_BYTE_ADDRESS]
    )
    {
        result = Ens190_CheckData(data, size);
    }

    return result;
}


#undef wait
#undef clear
#undef hasAnyFlag
#undef hasFlag
#undef memset
#undef memcpy

#endif // SCIOSENSE_ENS190_C_INL