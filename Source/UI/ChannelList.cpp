/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2014 Open Ephys

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

#include <string>

#include "ChannelList.h"

#include "ChannelComponent.h"

#include "../DeviceThread.h"
#include "../DeviceEditor.h"
#include "../Headstage.h"

using namespace RhythmNode;


ChannelList::ChannelList(DeviceThread* board_, DeviceEditor* editor_) :
    board(board_), editor(editor_), maxChannels(0)
{

    channelComponents.clear();

    //load shank image
    //if 4 shank, 2, or 1 picture depends.
    shankMap = ImageFileFormat::loadFrom(File::getCurrentWorkingDirectory().getChildFile("FourShank.png"));
    imagecomponent.setImage(shankMap);
    imagecomponent->setBounds()
    addAndMakeVisible(&imagecomponent);


    //numbering Scheme
    numberingSchemeLabel = new Label("Global Enable:","Global Enable:");
    numberingSchemeLabel->setEditable(false);
    numberingSchemeLabel->setBounds(10,10,150, 25);
    numberingSchemeLabel->setColour(Label::textColourId,juce::Colours::white);
    addAndMakeVisible(numberingSchemeLabel);

    //enable/disable
    globalEnable = new ComboBox("Global Enable");
    globalEnable->addItem("Enable All",1);
    globalEnable->addItem("Disable All",2);
    globalEnable->setBounds(125,10,140,25);
    globalEnable->addListener(this);
    globalEnable->setSelectedId(1, dontSendNotification);
    addAndMakeVisible(globalEnable);

    //upload stim parameters
    uploadStimParamButton = new UtilityButton("Upload Stim Parameters", Font("Default", 13, Font::plain));
    uploadStimParamButton->setRadius(3);
    uploadStimParamButton->setBounds(280,10,140,25);
    uploadStimParamButton->addListener(this);
    addAndMakeVisible(uploadStimParamButton);

    //star stim
    startStimButton = new UtilityButton("Start Stimulation", Font("Default", 13, Font::plain));
    startStimButton->setRadius(3);
    startStimButton->setBounds(430,10,150,25);
    startStimButton->addListener(this);
    addAndMakeVisible(startStimButton);
    
    //stop stim
    stopStimButton = new UtilityButton("Stop Stimulation", Font("Default", 13, Font::plain));
    stopStimButton->setRadius(3);
    stopStimButton->setBounds(580, 10, 150, 25);
    stopStimButton->addListener(this);
    addAndMakeVisible(stopStimButton);

    //waveform input labels
    waveformInputLabel = new Label("Waveform Labels", "Amplitude(uA)    Period (ms)    Pulse Width(ms)    Rise/Fall Time    #Pulses");
    waveformInputLabel->setEditable(false);
    waveformInputLabel->setBounds(10, 50, 800, 25);
    waveformInputLabel->setColour(Label::textColourId, juce::Colours::white);
    addAndMakeVisible(waveformInputLabel);

    //amplitude
    amplitudeInput = new TextEditor("Amplitude");
    amplitudeInput->setText("0");
    amplitudeInput->setBounds(30, 75, 50, 25);
    amplitudeInput->addListener(this);
    addAndMakeVisible(amplitudeInput);

    //period
    periodInput = new TextEditor("Period");
    periodInput->setText("0");
    periodInput->setBounds(150, 75, 50, 25);
    periodInput->addListener(this);
    addAndMakeVisible(periodInput);

    //pulse width
    pulseWidthInput = new TextEditor("Pulse Width");
    pulseWidthInput->setText("0");
    pulseWidthInput->setBounds(280, 75, 50, 25);
    pulseWidthInput->addListener(this);
    addAndMakeVisible(pulseWidthInput);

    //edgeSelect
    edgeSelectBox = new ComboBox("Edge Select");
    edgeSelectBox->addItem("0 ms", 1);
    edgeSelectBox->addItem("0.1 ms", 2);
    edgeSelectBox->addItem("0.5 ms", 3);
    edgeSelectBox->addItem("1 ms", 4);
    edgeSelectBox->addItem("2 ms", 5);
    edgeSelectBox->setBounds(400, 75, 80, 25);
    edgeSelectBox->addListener(this);
    edgeSelectBox->setSelectedId(1, dontSendNotification);
    addAndMakeVisible(edgeSelectBox);
    
    //pulse count
    pulseCountInput = new TextEditor("Pulse Count");
    pulseCountInput->setText("0");
    pulseCountInput->setBounds(530, 75, 50, 25);
    pulseCountInput->addListener(this);
    addAndMakeVisible(pulseCountInput);
   
    update();
}


void ChannelList::buttonClicked(Button* btn)
{

    if (btn == uploadStimParamButton)
    {
        //upload the stimulation parameters saved in the 
        //This structure should be updated upon       //any change to the UI elements. 
       LOGD("UPLOAD STIM PARAMS PRESSED")

        //get all text entries for waveform at stim
        amplitude = amplitudeInput->getTextValue().toString().getIntValue();
        period = periodInput->getTextValue().toString().getIntValue();
        pulse_width = pulseWidthInput->getTextValue().toString().getIntValue();
        edge_sel = edgeSelectBox->getComponentID().getIntValue();
        pulse_count = pulseCountInput->getTextValue().toString().getIntValue();

        board->uploadStimParameters(amplitude, period, pulse_width, edge_sel, pulse_count);

    }
    else if (btn == startStimButton)
    {
        //simply call the startStim() function  
        LOGD("START STIM")
        board->startStim();
    }
    else if (btn == stopStimButton)
    {
        //simply call the stopStim() function
        LOGD("STOP STIM")
        board->stopStim();
    }



}

void ChannelList::update()
{

    if (!board->foundInputSource())
    {
        disableAll();
        return;
    }

    staticLabels.clear();
    channelComponents.clear();
    uploadStimParamButton->setEnabled(true);//g3 change here

    const int columnWidth = 250;

    Array<const Headstage*> headstages = board->getConnectedHeadstages();

    int column = -1;

    maxChannels = 0;

    globalEnable->setSelectedId(board->getNamingScheme(), dontSendNotification);

    for (int i = 0; i < 64; i++)
    {
        //G3 HERE IS WHERE WE BUILD THE INPUT FOR EACH INDIVIDUAL LED
        /*
        column++;

        maxChannels = hs->getNumActiveChannels() > maxChannels ? hs->getNumActiveChannels() : maxChannels;

        Label* lbl = new Label(hs->getStreamPrefix(), hs->getStreamPrefix());
        lbl->setEditable(false);
        lbl->setBounds(10 + column * columnWidth, 40, columnWidth, 25);
        lbl->setJustificationType(juce::Justification::centred);
        lbl->setColour(Label::textColourId, juce::Colours::white);
        staticLabels.add(lbl);
        addAndMakeVisible(lbl);

        for (int ch = 0; ch < hs->getNumActiveChannels(); ch++)
        {
            ChannelComponent* comp =
                new ChannelComponent(
                    this,
                    ch,
                    0,
                    hs->getChannelName(ch),
                    gains,
                    ContinuousChannel::ELECTRODE);

            comp->setBounds(10 + column * columnWidth, 70 + ch * 22, columnWidth, 22);

            if (hs->hasImpedanceData())
            {
                comp->setImpedanceValues(
                    hs->getImpedanceMagnitude(ch),
                    hs->getImpedancePhase(ch));
            }
            //comp->setUserDefinedData(k);
            channelComponents.add(comp);
            addAndMakeVisible(comp);
        }

    }
    */
    }

    
}

void ChannelList::disableAll()
{
    for (auto channelComponent: channelComponents)
    {
        channelComponent->disableEdit();
    }

    uploadStimParamButton->setEnabled(false);
    startStimButton->setEnabled(false);
    stopStimButton->setEnabled(false);
    globalEnable->setEnabled(false);
}

void ChannelList::enableAll()
{
    for (int k=0; k<channelComponents.size(); k++)
    {
        channelComponents[k]->enableEdit();
    }
    uploadStimParamButton->setEnabled(true);
    startStimButton->setEnabled(true);
    stopStimButton->setEnabled(true);
    globalEnable->setEnabled(true);
}

void ChannelList::setNewGain(int channel, float gain)
{
    //RHD2000Thread* thread = (RHD2000Thread*)proc->getThread();
    //thread->modifyChannelGain(channel, gain);
    //if (chainUpdate)
    //    proc->requestChainUpdate();
}

void ChannelList::setNewName(int channel, String newName)
{
    //RHD2000Thread* thread = (RHD2000Thread*)proc->getThread();
    //thread->modifyChannelName(channel, newName);
    //if (chainUpdate)
    //    proc->requestChainUpdate();
}

void ChannelList::updateButtons()
{
}

void ChannelList::comboBoxChanged(ComboBox* b)
{
    if (b == globalEnable)
    {
       board->setNamingScheme((ChannelNamingScheme) b->getSelectedId());

       CoreServices::updateSignalChain(editor);
    }
}


void ChannelList::updateImpedance(Array<int> streams, Array<int> channels, Array<float> magnitude, Array<float> phase)
{
    int i = 0;
    for (int k = 0; k < streams.size(); k++)
    {
        if (i >= channelComponents.size())
            break; //little safety

        if (channelComponents[i]->type != ContinuousChannel::ELECTRODE)
        {
            k--;
        }
        else
        {
            channelComponents[i]->setImpedanceValues(magnitude[k], phase[k]);
        }
        i++;
    }

}
