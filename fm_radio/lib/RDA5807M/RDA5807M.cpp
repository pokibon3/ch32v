/* Arduino RDA5807M Library
 * See the README file for author and licensing information. In case it's
 * missing from your distribution, use the one here as the authoritative
 * version: https://github.com/csdexter/RDA5807M/blob/master/README
 *
 * This library is for interfacing with a RDA Microelectronics RDA5807M
 * single-chip FM broadcast radio receiver.
 * See the example sketches to learn how to use the library in your code.
 *
 * This is the main code file for the library.
 * See the header file for better function documentation.
 */
#include "i2c_master.h"
#include "RDA5807M.h"
#include "RDA5807M-private.h"
#include <stdio.h>

//#include <Wire.h>
#include "i2c_master.h"

void RDA5807M::begin(byte band) {
    init_i2c_master();
//    Wire.begin();
    setRegister(RDA5807M_REG_CONFIG, RDA5807M_FLG_DHIZ | RDA5807M_FLG_DMUTE | 
//                RDA5807M_FLG_BASS | RDA5807M_FLG_SEEKUP | RDA5807M_FLG_RDS | 
                RDA5807M_FLG_BASS | RDA5807M_FLG_SEEKUP |
                RDA5807M_FLG_NEW | RDA5807M_FLG_ENABLE);
    updateRegister(RDA5807M_REG_TUNING, RDA5807M_BAND_MASK, band);
};

void RDA5807M::end(void) {
    setRegister(RDA5807M_REG_CONFIG, 0x00);
};

void RDA5807M::setRegister(byte reg, const word value) {
    uint8_t data[3];

    data[0] = reg;
    data[1] = value >> 8;
    data[2] = value & 0x00ff;
    i2c_send(RDA5807M_I2C_ADDR_RANDOM, data, 3);

//    Wire.beginTransmission(RDA5807M_I2C_ADDR_RANDOM);
//    Wire.write(reg);
//    Wire.write(highByte(value));
//    Wire.write(lowByte(value));
//    Wire.endTransmission(true);
};

word RDA5807M::getRegister(byte reg) {
    uint8_t data[2];
    word result;

    i2c_send(RDA5807M_I2C_ADDR_RANDOM, &reg, 1);
    i2c_slave_available(RDA5807M_I2C_ADDR_RANDOM);
    i2c_receive(RDA5807M_I2C_ADDR_RANDOM, data, 2);
    result = data[0] << 8;
    result |= data[1];

//    Wire.beginTransmission(RDA5807M_I2C_ADDR_RANDOM);
//    Wire.write(reg);
//    Wire.endTransmission(false);
//    Wire.requestFrom(RDA5807M_I2C_ADDR_RANDOM, (size_t)2, true);
    //Don't let gcc play games on us, enforce order of execution.
//    result = (word)Wire.read() << 8;
//    result |= Wire.read();

    return result;
};

/*
void RDA5807M::setRegisterBulk(byte count, const word regs[]) {
    uint8_t data[32];

    if ( count > 16) return;                // error
    for(byte i = 0; i < count; i++) {
        data[i * 2] = regs[i] > 8;
        data[i * 2 + 1] = regs[i] &0xff;
    };
    i2c_send(RDA5807M_I2C_ADDR_SEQRDA, data, count * 2);
//    Wire.beginTransmission(RDA5807M_I2C_ADDR_SEQRDA);
//    for(byte i=0; i < count; i++) {
//        Wire.write(highByte(regs[i]));
//        Wire.write(lowByte(regs[i]));
//    };

//    Wire.endTransmission(true);
};

void RDA5807M::getRegisterBulk(byte count, word regs[]) {
    uint8_t data[32];

    if ( count > 16) return;                // error
    i2c_slave_available(RDA5807M_I2C_ADDR_SEQRDA);
    i2c_receive(RDA5807M_I2C_ADDR_SEQRDA, data, count * 2);
    for(byte i = 0; i < count; i++) {
        regs[i] =  data[i * 2] << 8;
        regs[i] |= data[i * 2 + 1];
    };


//    Wire.requestFrom(RDA5807M_I2C_ADDR_SEQRDA, (size_t)(count * 2), true);
//
//    for(byte i=0; i < count; i++) {
//        //Don't let gcc play games on us, enforce order of execution.
//        regs[count] = (word)Wire.read() << 8;
//        regs[count] |= Wire.read();
//    };
};

void RDA5807M::setRegisterBulk(const TRDA5807MRegisterFileWrite *regs) {
    const uint8_t * const ptr = (uint8_t *)regs;

    Wire.beginTransmission(RDA5807M_I2C_ADDR_SEQRDA);

    for(byte i=0; i < sizeof(TRDA5807MRegisterFileWrite); i++)
        Wire.write(ptr[i]);

    Wire.endTransmission(true);
};

void RDA5807M::getRegisterBulk(TRDA5807MRegisterFileRead *regs) {
    uint8_t * const ptr = (uint8_t *)regs;

    Wire.requestFrom(RDA5807M_I2C_ADDR_SEQRDA,
                     sizeof(TRDA5807MRegisterFileRead), true);

    for(byte i=0; i < sizeof(TRDA5807MRegisterFileRead); i++)
        ptr[i] = Wire.read();

};
*/


bool RDA5807M::volumeUp(void) {
    const byte volume = getRegister(RDA5807M_REG_VOLUME) & RDA5807M_VOLUME_MASK;

    if (volume == RDA5807M_VOLUME_MASK)
        return false;
    else {
        updateRegister(RDA5807M_REG_VOLUME, RDA5807M_VOLUME_MASK, volume + 1);
        return true;
    };
};

bool RDA5807M::volumeDown(bool alsoMute) {
    const byte volume = getRegister(RDA5807M_REG_VOLUME) & RDA5807M_VOLUME_MASK;

    if (volume) {
        updateRegister(RDA5807M_REG_VOLUME, RDA5807M_VOLUME_MASK, volume - 1);
        if(!(volume - 1) && alsoMute)
            //If we are to trust the datasheet, this is superfluous as a volume
            //of zero triggers mute & HiZ on its own.
            mute();
        return true;
    } else 
        return false;
};

void RDA5807M::seekUp(bool wrap) {
    updateRegister(RDA5807M_REG_CONFIG,
                   (RDA5807M_FLG_SEEKUP | RDA5807M_FLG_SEEK |
                    RDA5807M_FLG_SKMODE), 
                   (RDA5807M_FLG_SEEKUP | RDA5807M_FLG_SEEK |
                    (wrap ? 0x00 : RDA5807M_FLG_SKMODE)));
};

void RDA5807M::seekDown(bool wrap) {
    updateRegister(RDA5807M_REG_CONFIG,
                   (RDA5807M_FLG_SEEKUP | RDA5807M_FLG_SEEK |
                    RDA5807M_FLG_SKMODE), 
                   (0x00 | RDA5807M_FLG_SEEK |
                    (wrap ? 0x00 : RDA5807M_FLG_SKMODE)));
};

void RDA5807M::mute(void) {
    updateRegister(RDA5807M_REG_CONFIG, RDA5807M_FLG_DMUTE, 0x00);
};

void RDA5807M::unMute(bool minVolume) {
    if (minVolume)
        updateRegister(RDA5807M_REG_VOLUME, RDA5807M_VOLUME_MASK, 0x1);
    updateRegister(RDA5807M_REG_CONFIG, RDA5807M_FLG_DMUTE, RDA5807M_FLG_DMUTE);
};

//const word RDA5807M_BandLowerLimits[5] PROGMEM = { 8700, 7600, 7600, 6500, 5000 };
//const word RDA5807M_BandHigherLimits[5] PROGMEM = { 10800, 9100, 10800, 7600, 6500 };
//const byte RDA5807M_ChannelSpacings[4] PROGMEM = { 100, 200, 50, 25 };

const word RDA5807M_BandLowerLimits[5]  = { 8700, 7600, 7600, 6500, 5000 };
const word RDA5807M_BandHigherLimits[5] = { 10800, 9100, 10800, 7600, 6500 };
const byte RDA5807M_ChannelSpacings[4]  = { 100, 200, 50, 25 };

word RDA5807M::getBandAndSpacing(void) {
    byte band = getRegister(RDA5807M_REG_TUNING) & (RDA5807M_BAND_MASK |
                                                    RDA5807M_SPACE_MASK);
    //Separate channel spacing
    const byte space = band & RDA5807M_SPACE_MASK;

    if (band & RDA5807M_BAND_MASK == RDA5807M_BAND_EAST && 
        (getRegister(RDA5807M_REG_BLEND) & RDA5807M_FLG_EASTBAND65M))
        //Lower band limit is 50MHz
        band = (band >> RDA5807M_BAND_SHIFT) + 1;
    else
        band >>= RDA5807M_BAND_SHIFT;

    return word(space << 8 | band);
};

word RDA5807M::getFrequency(void) {
    const word spaceandband = getBandAndSpacing();
    const byte band = spaceandband & 0xff;
//    if (band & RDA5807M_BAND_MASK == RDA5807M_BAND_EAST && 
//        !(getRegister(RDA5807M_REG_BLEND) & RDA5807M_FLG_EASTBAND65M)) {
//        band = (spaceandband & 0x00ff);
//    } else {
//        //Lower band limit is 50MHz
//        band = (spaceandband & 0x00ff) + 1;
//    }

//    return pgm_read_word(&RDA5807M_BandLowerLimits[lowByte(spaceandband)]) +
//        (getRegister(RDA5807M_REG_STATUS) & RDA5807M_READCHAN_MASK) *
//        pgm_read_byte(&RDA5807M_ChannelSpacings[highByte(spaceandband)]) / 10;

    return RDA5807M_BandLowerLimits[band] +
        (getRegister(RDA5807M_REG_STATUS) & RDA5807M_READCHAN_MASK) *
        RDA5807M_ChannelSpacings[spaceandband >> 8] / 10;
};

bool RDA5807M::setFrequency(word frequency) {
    const word spaceandband = getBandAndSpacing();
    const byte band = spaceandband & 0xff;
//    const word origin = RDA5807M_BandLowerLimits[spaceandband & 0x00ff];

//    if (band & RDA5807M_BAND_MASK == RDA5807M_BAND_EAST && 
//        !(getRegister(RDA5807M_REG_BLEND) & RDA5807M_FLG_EASTBAND65M)) {
//        band = (spaceandband & 0x00ff);
//    } else {
//        //Lower band limit is 50MHz
//        band = (spaceandband & 0x00ff) + 1;
//    }
    const word origin = RDA5807M_BandLowerLimits[band];

    //Check that specified frequency falls within our current band limits
    if (frequency < origin ||
        frequency > RDA5807M_BandHigherLimits[band])
        return false;

    //Adjust start offset
    frequency -= origin;

    const byte spacing = RDA5807M_ChannelSpacings[spaceandband >> 8];

    //Check that the given frequency can be tuned given current channel spacing
    if (frequency * 10 % spacing)
        return false;

    //Attempt to tune to the requested frequency
    updateRegister(RDA5807M_REG_TUNING, RDA5807M_CHAN_MASK | RDA5807M_FLG_TUNE,
                   ((frequency * 10 / spacing) << RDA5807M_CHAN_SHIFT) |
                   RDA5807M_FLG_TUNE);

    return true;
};

byte RDA5807M::getRSSI(void) {
    return (getRegister(RDA5807M_REG_RSSI) & RDA5807M_RSSI_MASK) >> RDA5807M_RSSI_SHIFT;
};

bool RDA5807M::setBand(word band) {
    // set or reset RDA5807M_FLG_EASTBAND65M
    updateRegister(RDA5807M_REG_BLEND, RDA5807M_FLG_EASTBAND65M, (band & 0x001) << 9);
    // set band;
    updateRegister(RDA5807M_REG_TUNING, RDA5807M_BAND_MASK, band & RDA5807M_BAND_MASK);

    return true;
}

bool RDA5807M::setSpacing(word space) {
    // set spacing;
    updateRegister(RDA5807M_REG_TUNING, RDA5807M_SPACE_MASK, space & RDA5807M_SPACE_MASK);

    return true;
}
bool RDA5807M::dumpRegister(void)
{
    printf("REG_CHIPID  :%04x\n", getRegister(RDA5807M_REG_CHIPID));
    printf("REG_CONFIG  :%04x\n", getRegister(RDA5807M_REG_CONFIG));
    printf("REG_TUNING  :%04x\n", getRegister(RDA5807M_REG_TUNING));
    printf("REG_GPIO    :%04x\n", getRegister(RDA5807M_REG_GPIO));
    printf("REG_VOLUME  :%04x\n", getRegister(RDA5807M_REG_VOLUME));
    printf("REG_I2S     :%04x\n", getRegister(RDA5807M_REG_I2S)); 
    printf("REG_BLEND   :%04x\n", getRegister(RDA5807M_REG_BLEND));
    printf("REG_FREQ    :%04x\n", getRegister(RDA5807M_REG_FREQ));
    printf("REG_STATUS  :%04x\n", getRegister(RDA5807M_REG_STATUS));
    printf("REG_RSSI    :%04x\n", getRegister(RDA5807M_REG_RSSI));

    return true;
}