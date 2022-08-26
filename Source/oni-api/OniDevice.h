/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2022 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#ifndef OniDevice_hpp
#define OniDevice_hpp

#include <stdio.h>

#include "oni.h"

#include "../rhx-api/Abstract/abstractrhxcontroller.h"

/**

    Class for communicating with an ONI Device
 
 */

class OniDevice : public AbstractRHXController
{

public:
    OniDevice();
    ~OniDevice();

    bool isSynthetic() const override { return false; }
    bool isPlayback() const override { return false; }
    AcquisitionMode acquisitionMode() const override { return LiveMode; }
    int open(const std::string& boardSerialNumber) override;
    bool uploadFPGABitfile(const std::string& filename) override;
    void resetBoard() override;

    void run() override;
    bool isRunning() override;
    void flush() override;
    void resetFpga() override;

    bool readDataBlock(RHXDataBlock* dataBlock) override;
    bool readDataBlocks(int numBlocks, std::deque<RHXDataBlock*>& dataQueue) override;
    long readDataBlocksRaw(int numBlocks, uint8_t* buffer) override;

    void setContinuousRunMode(bool continuousMode) override;
    void setMaxTimeStep(unsigned int maxTimeStep) override;
    void setCableDelay(BoardPort port, int delay) override;
    void setDspSettle(bool enabled) override;
    void setDataSource(int stream, BoardDataSource dataSource) override;  // used only with ControllerRecordUSB2
    void setTtlOut(const int* ttlOutArray) override;  // not used with ControllerStimRecordUSB2
    void setDacManual(int value) override;
    void setClockDivider(int divide_factor) override;
    void enableLeds(bool ledsOn) override;
    void setLedDisplay(const int* ledArray) override;
    void setSpiLedDisplay(const int* ledArray) override;  // not used with ControllerRecordUSB2
    void setDacGain(int gain) override;
    void setAudioNoiseSuppress(int noiseSuppress) override;
    void setExternalFastSettleChannel(int channel) override;             // not used with ControllerStimRecordUSB2
    void setExternalDigOutChannel(BoardPort port, int channel) override; // not used with ControllerStimRecordUSB2
    void setDacHighpassFilter(double cutoff) override;
    void setDacThreshold(int dacChannel, int threshold, bool trigPolarity) override;
    void setTtlMode(int mode) override;      // not used with ControllerStimRecordUSB2
    void setDacRerefSource(int stream, int channel) override;  // not used with ControllerRecordUSB2
    void setExtraStates(unsigned int extraStates) override;
    void setStimCmdMode(bool enabled) override;
    void setAnalogInTriggerThreshold(double voltageThreshold) override;
    void setManualStimTrigger(int trigger, bool triggerOn) override;
    void setGlobalSettlePolicy(bool settleWholeHeadstageA, bool settleWholeHeadstageB, bool settleWholeHeadstageC, bool settleWholeHeadstageD, bool settleAllHeadstages) override;
    void setTtlOutMode(bool mode1, bool mode2, bool mode3, bool mode4, bool mode5, bool mode6, bool mode7, bool mode8) override;
    void setAmpSettleMode(bool useFastSettle) override;
    void setChargeRecoveryMode(bool useSwitch) override;
    bool setSampleRate(AmplifierSampleRate newSampleRate) override;

    void enableDataStream(int stream, bool enabled) override;
    void enableDac(int dacChannel, bool enabled) override;
    void enableExternalFastSettle(bool enable) override;                 // not used with ControllerStimRecordUSB2
    void enableExternalDigOut(BoardPort port, bool enable) override;     // not used with ControllerStimRecordUSB2
    void enableDacHighpassFilter(bool enable) override;
    void enableDacReref(bool enabled) override;  // not used with ControllerRecordUSB2
    void enableDcAmpConvert(bool enable) override;
    void enableAuxCommandsOnAllStreams() override;
    void enableAuxCommandsOnOneStream(int stream) override;

    void selectDacDataStream(int dacChannel, int stream) override;
    void selectDacDataChannel(int dacChannel, int dataChannel) override;
    void selectAuxCommandLength(AuxCmdSlot auxCommandSlot, int loopIndex, int endIndex) override;
    void selectAuxCommandBank(BoardPort port, AuxCmdSlot auxCommandSlot, int bank) override; // not used with ControllerStimRecordUSB2

    int getBoardMode() override;
    int getNumSPIPorts(bool& expanderBoardDetected) override;

    void clearTtlOut() override;                 // not used with ControllerStimRecordUSB2
    void resetSequencers() override;
    void programStimReg(int stream, int channel, StimRegister reg, int value) override;
    void uploadCommandList(const std::vector<unsigned int>& commandList, AuxCmdSlot auxCommandSlot, int bank = 0) override;

    int findConnectedChips(std::vector<ChipType>& chipType, std::vector<int>& portIndex, std::vector<int>& commandStream,
        std::vector<int>& numChannelsOnPort) override;

private:
    // Objects of this class should not be copied.  Disable copy and assignment operators.
    OniDevice(const OniDevice&);            // declaration only
    OniDevice& operator=(const OniDevice&); // declaration only

    unsigned int numWordsInFifo() override;
    bool isDcmProgDone() const override;
    bool isDataClockLocked() const override;
    void forceAllDataStreamsOff() override;
    
    oni_ctx* ctx;
   
};

#endif /* OniDevice_hpp */