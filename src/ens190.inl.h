#include "ens190.h"


ENS190::~ENS190() { }

ENS190::ENS190()
{
    io              = { 0 };
    fwVersion[0]    = 0;
    productionDate  = 0;
    serialNumber    = 0;
}

void ENS190::begin(Stream* serial)
{
    serialConfig        = { 0 };
    serialConfig.serial = serial;

    io.read             = ScioSense_Arduino_Serial_Read;
    io.write            = ScioSense_Arduino_Serial_Write;
    io.wait             = ScioSense_Arduino_Serial_Wait;
    io.clear            = ScioSense_Arduino_Serial_Clear;
    io.config           = &serialConfig;
}

inline bool ENS190::init()
{
    return Ens190_Reset(this) == RESULT_OK;
}

void ENS190::clear()
{
    this->io.clear(this->io.config);
}

bool ENS190::isConnected()
{
    return Ens190_IsConnected(this);
}

Result ENS190::update()
{
    return Ens190_Update(this);
}

uint16_t ENS190::getCo2()
{
    return Ens190_GetCo2(this);
}

uint16_t ENS190::getSerialNumber()
{
    return Ens190_GetSerialNumber(this);
}

uint32_t ENS190::getProductionDate()
{
    return Ens190_GetProductionDate(this);
}

uint8_t* ENS190::getFirmwareVersion()
{
    return Ens190_GetFirmwareVersion(this);
}

