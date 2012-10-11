/*
vNES
Copyright © 2006-2011 Jamie Sanders

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Globals.h"

     MapperDefault::MapperDefault() {
        this->nes = NULL;
        this->cpuMem = NULL;
        this->cpuMemArray = NULL;
        this->ppuMem = NULL;
        this->rom = NULL;
        this->cpu = NULL;
        this->ppu = NULL;
    }

	void MapperDefault::write(int address, short value) {
		base_write(address, value);
	}

     void MapperDefault::base_init(NES* nes) {

        this->nes = nes;
        this->cpuMem = nes->getCpuMemory();
        this->cpuMemArray = cpuMem->mem;
        this->ppuMem = nes->getPpuMemory();
        this->rom = nes->getRom();
        this->cpu = nes->getCpu();
        this->ppu = nes->getPpu();

        cpuMemSize = cpuMem->getMemSize();
        joypadLastWrite = -1;

    }

	void MapperDefault::init(NES* nes) {
		this->base_init(nes);
	}

     void MapperDefault::stateLoad(ByteBuffer* buf) {

        // Check version:
        if (buf->readByte() == 1) {

            // Joypad stuff:
            joy1StrobeState = buf->readInt();
            joy2StrobeState = buf->readInt();
            joypadLastWrite = buf->readInt();

            // Mapper specific stuff:
            mapperInternalStateLoad(buf);

        }

    }

     void MapperDefault::stateSave(ByteBuffer* buf) {

        // Version:
        buf->putByte((short) 1);

        // Joypad stuff:
        buf->putInt(joy1StrobeState);
        buf->putInt(joy2StrobeState);
        buf->putInt(joypadLastWrite);

        // Mapper specific stuff:
        mapperInternalStateSave(buf);

    }

     void MapperDefault::mapperInternalStateLoad(ByteBuffer* buf) {

        buf->putByte((short) joy1StrobeState);
        buf->putByte((short) joy2StrobeState);
        buf->putByte((short) joypadLastWrite);

    }

     void MapperDefault::mapperInternalStateSave(ByteBuffer* buf) {

        joy1StrobeState = buf->readByte();
        joy2StrobeState = buf->readByte();
        joypadLastWrite = buf->readByte();

    }

     void MapperDefault::setGameGenieState(bool enable) {
        gameGenieActive = enable;
    }

     bool MapperDefault::getGameGenieState() {
        return gameGenieActive;
    }

     void MapperDefault::base_write(int address, short value) {

        if (address < 0x2000) {

            // Mirroring of RAM:
            (*cpuMem->mem)[address & 0x7FF] = value;

        } else if (address > 0x4017) {

            (*cpuMem->mem)[address] = value;
            if (address >= 0x6000 && address < 0x8000) {

                // Write to SaveRAM. Store in file:
                if (rom != NULL) {
                    rom->writeBatteryRam(address, value);
                }

            }

        } else if (address > 0x2007 && address < 0x4000) {

            regWrite(0x2000 + (address & 0x7), value);

        } else {

            regWrite(address, value);

        }

    }

     void MapperDefault::writelow(int address, short value) {

        if (address < 0x2000) {
            // Mirroring of RAM:
            (*cpuMem->mem)[address & 0x7FF] = value;

        } else if (address > 0x4017) {
            (*cpuMem->mem)[address] = value;

        } else if (address > 0x2007 && address < 0x4000) {
            regWrite(0x2000 + (address & 0x7), value);

        } else {
            regWrite(address, value);
        }

    }

     short MapperDefault::load(int address) {

        // Wrap around:
        address &= 0xFFFF;

        // Check address range:
        if (address > 0x4017) {

            // ROM:
            return (*cpuMemArray)[address];

        } else if (address >= 0x2000) {

            // I/O Ports.
            return regLoad(address);

        } else {

            // RAM (mirrored)
            return (*cpuMemArray)[address & 0x7FF];

        }

    }

     short MapperDefault::regLoad(int address) {

        switch (address >> 12) { // use fourth nibble (0xF000)

            case 0: {
                break;
            }
            case 1: {
                break;
            }
            case 2: {
                // Fall through to case 3
            }
            case 3: {

                // PPU Registers
                switch (address & 0x7) {
                    case 0x0: {

                        // 0x2000:
                        // PPU Control Register 1.
                        // (the value is stored both
                        // in main memory and in the
                        // PPU as flags):
                        // (not in the real NES)
                        return (*cpuMem->mem)[0x2000];

                    }
                    case 0x1: {

                        // 0x2001:
                        // PPU Control Register 2.
                        // (the value is stored both
                        // in main memory and in the
                        // PPU as flags):
                        // (not in the real NES)
                        return (*cpuMem->mem)[0x2001];

                    }
                    case 0x2: {

                        // 0x2002:
                        // PPU Status Register.
                        // The value is stored in
                        // main memory in addition
                        // to as flags in the PPU.
                        // (not in the real NES)
                        return ppu->readStatusRegister();

                    }
                    case 0x3: {
                        return 0;
                    }
                    case 0x4: {

                        // 0x2004:
                        // Sprite Memory read.
                        return ppu->sramLoad();

                    }
                    case 0x5: {
                        return 0;
                    }
                    case 0x6: {
                        return 0;
                    }
                    case 0x7: {

                        // 0x2007:
                        // VRAM read:
                        return ppu->vramLoad();

                    }
                }
                break;

            }
            case 4: {


                // Sound+Joypad registers

                switch (address - 0x4015) {
                    case 0: {

                        // 0x4015:
                        // Sound channel enable, DMC Status
                        return nes->getPapu()->readReg(address);

                    }
                    case 1: {

                        // 0x4016:
                        // Joystick 1 + Strobe
                        return joy1Read();

                    }
                    case 2: {

                        // 0x4017:
                        // Joystick 2 + Strobe
                        if (mousePressed && nes->ppu != NULL && nes->ppu->get_screen_buffer() != NULL) {

                            // Check for white pixel nearby:

                            int sx, sy, ex, ey, w;
                            sx = max(0, mouseX - 4);
                            ex = min(256, mouseX + 4);
                            sy = max(0, mouseY - 4);
                            ey = min(240, mouseY + 4);
                            w = 0;

                            for (int y = sy; y < ey; y++) {
                                for (int x = sx; x < ex; x++) {
                                    if (((*nes->ppu->get_screen_buffer())[(y << 8) + x] & 0xFFFFFF) == 0xFFFFFF) {
                                        w = 0x1 << 3;
                                        break;
                                    }
                                }
                            }

                            w |= (mousePressed ? (0x1 << 4) : 0);
                            return (short) (joy2Read() | w);

                        } else {
                            return joy2Read();
                        }

                    }
                }

                break;

            }
        }

        return 0;

    }

     void MapperDefault::regWrite(int address, short value) {

        switch (address) {
            case 0x2000: {

                // PPU Control register 1
                cpuMem->write(address, value);
                ppu->updateControlReg1(value);
                break;

            }
            case 0x2001: {

                // PPU Control register 2
                cpuMem->write(address, value);
                ppu->updateControlReg2(value);
                break;

            }
            case 0x2003: {

                // Set Sprite RAM address:
                ppu->writeSRAMAddress(value);
                break;

            }
            case 0x2004: {

                // Write to Sprite RAM:
                ppu->sramWrite(value);
                break;

            }
            case 0x2005: {

                // Screen Scroll offsets:
                ppu->scrollWrite(value);
                break;

            }
            case 0x2006: {

                // Set VRAM address:
                ppu->writeVRAMAddress(value);
                break;

            }
            case 0x2007: {

                // Write to VRAM:
                ppu->vramWrite(value);
                break;

            }
            case 0x4014: {

                // Sprite Memory DMA Access
                ppu->sramDMA(value);
                break;

            }
            case 0x4015: {

                // Sound Channel Switch, DMC Status
                nes->getPapu()->writeReg(address, value);
                break;

            }
            case 0x4016: {

                ////System.out.println("joy strobe write "+value);

                // Joystick 1 + Strobe
                if (value == 0 && joypadLastWrite == 1) {
                    ////System.out.println("Strobes reset.");
                    joy1StrobeState = 0;
                    joy2StrobeState = 0;
                }
                joypadLastWrite = value;
                break;

            }
            case 0x4017: {

                // Sound channel frame sequencer:
                nes->papu->writeReg(address, value);
                break;

            }
            default: {

                // Sound registers
                ////System.out.println("write to sound reg");
                if (address >= 0x4000 && address <= 0x4017) {
                    nes->getPapu()->writeReg(address, value);
                }
                break;

            }
        }

    }

     short MapperDefault::joy1Read() {
        short ret = 0;
        KbInputHandler* in = nes->_joy1;

        switch (joy1StrobeState) {
            case 0:
                ret = in->getKeyState(KbInputHandler::KEY_A);
                break;
            case 1:
                ret = in->getKeyState(KbInputHandler::KEY_B);
                break;
            case 2:
                ret = in->getKeyState(KbInputHandler::KEY_SELECT);
                break;
            case 3:
                ret = in->getKeyState(KbInputHandler::KEY_START);
                break;
            case 4:
                ret = in->getKeyState(KbInputHandler::KEY_UP);
                break;
            case 5:
                ret = in->getKeyState(KbInputHandler::KEY_DOWN);
                break;
            case 6:
                ret = in->getKeyState(KbInputHandler::KEY_LEFT);
                break;
            case 7:
                ret = in->getKeyState(KbInputHandler::KEY_RIGHT);
                break;
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
                ret = (short) 0;
                break;
            case 19:
                ret = (short) 1;
                break;
            default:
                ret = 0;
        }

        joy1StrobeState++;
        if (joy1StrobeState == 24) {
            joy1StrobeState = 0;
        }
        
        return ret;
    }

     short MapperDefault::joy2Read() {
        short ret = 0;
        KbInputHandler* in = nes->_joy2;

        switch (joy2StrobeState) {
            case 0:
                ret = in->getKeyState(KbInputHandler::KEY_A);
                break;
            case 1:
                ret = in->getKeyState(KbInputHandler::KEY_B);
                break;
            case 2:
                ret = in->getKeyState(KbInputHandler::KEY_SELECT);
                break;
            case 3:
                ret = in->getKeyState(KbInputHandler::KEY_START);
                break;
            case 4:
                ret = in->getKeyState(KbInputHandler::KEY_UP);
                break;
            case 5:
                ret = in->getKeyState(KbInputHandler::KEY_DOWN);
                break;
            case 6:
                ret = in->getKeyState(KbInputHandler::KEY_LEFT);
                break;
            case 7:
                ret = in->getKeyState(KbInputHandler::KEY_RIGHT);
                break;
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
                ret = (short) 0;
                break;
            case 19:
                ret = (short) 1;
                break;
            default:
                ret = 0;
        }
        
        joy2StrobeState++;
        if (joy2StrobeState == 24) {
            joy2StrobeState = 0;
        }
        
        return ret;
    }

     void MapperDefault::loadROM(ROM* rom) {

        if (!rom->isValid() || rom->getRomBankCount() < 1) {
            //System.out.println("NoMapper: Invalid ROM! Unable to load.");
            return;
        }

        // Load ROM into memory:
        loadPRGROM();

        // Load CHR-ROM:
        loadCHRROM();

        // Load Battery RAM (if present):
        loadBatteryRam();

        // Reset IRQ:
        //nes->getCpu().doResetInterrupt();
        nes->getCpu()->requestIrq(CPU::IRQ_RESET);

    }

    void MapperDefault::loadPRGROM() {

        if (rom->getRomBankCount() > 1) {
            // Load the two first banks into memory.
            loadRomBank(0, 0x8000);
            loadRomBank(1, 0xC000);
        } else {
            // Load the one bank into both memory locations:
            loadRomBank(0, 0x8000);
            loadRomBank(0, 0xC000);
        }

    }

    void MapperDefault::loadCHRROM() {

        ////System.out.println("Loading CHR ROM..");

        if (rom->getVromBankCount() > 0) {
            if (rom->getVromBankCount() == 1) {
                loadVromBank(0, 0x0000);
                loadVromBank(0, 0x1000);
            } else {
                loadVromBank(0, 0x0000);
                loadVromBank(1, 0x1000);
            }
        } else {
            //System.out.println("There aren't any CHR-ROM banks..");
        }

    }

     void MapperDefault::loadBatteryRam() {

        if (rom->batteryRam) {

            vector<short>* ram = rom->getBatteryRam();
            if (ram != NULL && ram->size() == 0x2000) {
                arraycopy_short(ram, 0, nes->cpuMem->mem, 0x6000, 0x2000);
            }

        }

    }

    void MapperDefault::loadRomBank(int bank, int address) {

        // Loads a ROM bank into the specified address.
        bank %= rom->getRomBankCount();
        //vector<short>* data = rom->getRomBank(bank);
        //cpuMem->write(address,data,data.length);
        arraycopy_short(rom->getRomBank(bank), 0, cpuMem->mem, address, 16384);

    }

    void MapperDefault::loadVromBank(int bank, int address) {

        if (rom->getVromBankCount() == 0) {
            return;
        }
        ppu->triggerRendering();

        arraycopy_short(rom->getVromBank(bank % rom->getVromBankCount()), 0, nes->ppuMem->mem, address, 4096);

        vector<Tile*>* vromTile = rom->getVromBankTiles(bank % rom->getVromBankCount());
        arraycopy_Tile(vromTile, 0, ppu->ptTile, address >> 4, 256);

    }

    void MapperDefault::load32kRomBank(int bank, int address) {

        loadRomBank((bank * 2) % rom->getRomBankCount(), address);
        loadRomBank((bank * 2 + 1) % rom->getRomBankCount(), address + 16384);

    }

    void MapperDefault::load8kVromBank(int bank4kStart, int address) {

        if (rom->getVromBankCount() == 0) {
            return;
        }
        ppu->triggerRendering();

        loadVromBank((bank4kStart) % rom->getVromBankCount(), address);
        loadVromBank((bank4kStart + 1) % rom->getVromBankCount(), address + 4096);

    }

    void MapperDefault::load1kVromBank(int bank1k, int address) {

        if (rom->getVromBankCount() == 0) {
            return;
        }
        ppu->triggerRendering();

        int bank4k = (bank1k / 4) % rom->getVromBankCount();
        int bankoffset = (bank1k % 4) * 1024;
        arraycopy_short(rom->getVromBank(bank4k), 0, nes->ppuMem->mem, bankoffset, 1024);

        // Update tiles:
        vector<Tile*>* vromTile = rom->getVromBankTiles(bank4k);
        int baseIndex = address >> 4;
        for (int i = 0; i < 64; i++) {
            (*ppu->ptTile)[baseIndex + i] = (*vromTile)[((bank1k % 4) << 6) + i];
        }

    }

    void MapperDefault::load2kVromBank(int bank2k, int address) {

        if (rom->getVromBankCount() == 0) {
            return;
        }
        ppu->triggerRendering();

        int bank4k = (bank2k / 2) % rom->getVromBankCount();
        int bankoffset = (bank2k % 2) * 2048;
        arraycopy_short(rom->getVromBank(bank4k), bankoffset, nes->ppuMem->mem, address, 2048);

        // Update tiles:
        vector<Tile*>* vromTile = rom->getVromBankTiles(bank4k);
        int baseIndex = address >> 4;
        for (int i = 0; i < 128; i++) {
            (*ppu->ptTile)[baseIndex + i] = (*vromTile)[((bank2k % 2) << 7) + i];
        }

    }

    void MapperDefault::load8kRomBank(int bank8k, int address) {

        int bank16k = (bank8k / 2) % rom->getRomBankCount();
        int offset = (bank8k % 2) * 8192;

        vector<short>* bank = rom->getRomBank(bank16k);
        cpuMem->write(address, bank, offset, 8192);

    }

     void MapperDefault::clockIrqCounter() {
        // Does nothing. This is used by the MMC3 mapper.
    }

     void MapperDefault::latchAccess(int address) {
        // Does nothing. This is used by MMC2.
    }

     int MapperDefault::syncV() {
        return 0;
    }

     int MapperDefault::syncH(int scanline) {
        return 0;
    }

     void MapperDefault::setMouseState(bool pressed, int x, int y) {

        mousePressed = pressed;
        mouseX = x;
        mouseY = y;

    }

     void MapperDefault::reset() {

        this->joy1StrobeState = 0;
        this->joy2StrobeState = 0;
        this->joypadLastWrite = 0;
        this->mousePressed = false;

    }

     void MapperDefault::destroy() {

        nes = NULL;
        cpuMem = NULL;
        ppuMem = NULL;
        rom = NULL;
        cpu = NULL;
        ppu = NULL;

    }
