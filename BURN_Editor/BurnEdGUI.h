///////////////////////////////////////////////////////////////////////////
// GSi BURN Editor - Version 1.0 - October 2013
// Uses wxWidgets and RtMidi for cross platform (Win and Mac) compatibility
//
// www.GenuineSoundware.com
// 
///////////////////////////////////////////////////////////////////////////

#ifndef __BURNEDGUI_H__
#define __BURNEDGUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/statbmp.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/slider.h>
#include <wx/bmpcbox.h>
#include <wx/listbox.h>
#include <wx/frame.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/timer.h>
#include <wx/stdpaths.h>
#include <wx/gauge.h>
#include <wx/splash.h>
#include <wx/hyperlink.h>
#include <wx/sysopt.h>

///////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "embedded_images/BurnEd_cover.bmp.h"
#include "embedded_images/BurnEd_about.bmp.h"

///////////////////////////////////////////////////////////////////////////////
/// Class MainWindow
///////////////////////////////////////////////////////////////////////////////
class MainWindow : public wxFrame 
{
public:
	// Construct MainWindow
	MainWindow( wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxString& title = wxT(APPLICATION_NAME), const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxSize( MAIN_WIN_SIZE ), 
		long style = wxCAPTION|wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL, 
		const wxString& name = wxT(APPLICATION_NAME) );

	// Destroy MainWindow
	~MainWindow();

	// Other wx stuff
	void OnQuit(wxCommandEvent& event);
	bool FileOpen(bool, bool);

	// The configuration file
	wxString configFile;
	bool loadConfig();
	bool saveConfig();

	// MainWindow widgets
	wxStaticBitmap *bmpCover;
	wxStatusBar *statusBar;
	wxMenuBar *menuBar;
	wxMenu *menuFile, *menuImport, *menuExport, *menuMIDI, *menuInputPort, *menuOutputPort, *menuHelp;
	wxStaticText *stInfoDevice, *stInfoVersion, *lblParam1, *lblParam2, *lblParam3;
	wxButton *btnConnect, *btnDisconnect, *btnWritePreset, *btnInitPreset, *btnDownloadBank, *btnDownloadPreset, *btnUploadBank, *btnMoveUp, *btnMoveDown;
	wxTextCtrl *txtPresetName;
	wxListBox *listPresets;
	wxCheckBox *checkBypass, *checkOverdrive;
	wxSlider *sliderGain, *sliderLevel, *sliderParam1, *sliderParam2, *sliderParam3;
	wxComboBox *cmbEffects, *cmbNRparam[NUM_NR_PARAMETERS];
	wxGauge *progressBar;
	wxTimer	*timer1;

	// RtMIDI stuff
	RtMidiOut	*MidiOut;
	RtMidiIn	*MidiIn;
	int MidiOutPorts, MidiOutPort;
	int MidiInPorts, MidiInPort;
	bool RtMidiOutPortsAreOpen, RtMidiInPortsAreOpen;
	std::vector<string> MidiOutPortName;
	std::vector<string> MidiInPortName;

	// Flags
	bool sendingMIDI;
	bool BURNisConnected;
	int presetMemoryOK;
	
	// Local preset memory
	unsigned char PresetData[NUMBEROFPRESETS][PRESETCHUNKSIZE];

	// File stuff
	size_t InFileSize;
	char InFileName[256];
	char InFileInfo[256];
	unsigned char *fileBuffer;

	// Other...
	int timeOut;

	bool confirm(char *msg)
	{
		return wxMessageBox(msg, _T(APPLICATION_NAME), wxYES_NO | wxICON_QUESTION | wxYES_DEFAULT, this) == wxYES ? true : false;
	}

	bool info(char *msg)
	{
		wxMessageBox(msg, _T(APPLICATION_NAME), wxOK | wxICON_INFORMATION, this);
		return false;
	}

	bool alert(char *msg)
	{
		wxMessageBox(msg, _T(APPLICATION_NAME), wxOK | wxICON_ERROR, this);
		return false;
	}

	// Send a complete 3-byte MIDI event
	void RtMidiSendEvent(unsigned char Byte1, unsigned char Byte2, unsigned char Byte3)
	{
		if (!RtMidiOutPortsAreOpen || !RtMidiInPortsAreOpen) return;

		std::vector<unsigned char> MidiMessage(3, 0); // 3 unsigned chars with value 0
		MidiMessage[0] = Byte1;
		MidiMessage[1] = Byte2;
		MidiMessage[2] = Byte3;
		MidiOut->sendMessage(&MidiMessage);
	}

	// Send a single byte
	void RtMidiSendByte(unsigned char Byte)
	{
		if (!RtMidiOutPortsAreOpen || !RtMidiInPortsAreOpen) return;

		std::vector<unsigned char> MidiMessage(1, 0); // 1 unsigned char with value 0
		MidiMessage[0] = Byte;
		MidiOut->sendMessage(&MidiMessage);
	}

	// Send a complete SysEx chunk
	void RtMidiSendSysexChunk(unsigned char *buffer, int buf_len)
	{
		if (!RtMidiOutPortsAreOpen || !RtMidiInPortsAreOpen) return;

		std::vector<unsigned char> MidiMessage(buf_len, 0);
		for (int i=0; i<buf_len; i++) MidiMessage[i] = buffer[i];
		MidiOut->sendMessage(&MidiMessage);
	}


	// This is the callback function for receiving the MIDI INPUT stream.
	// This function must be static, otherwise it can't be referenced, but we use the *userData pointer to point to a non-static function
	static void ProcessMidiInput(double deltatime, std::vector<unsigned char> *message, void *userData)
	{
		vector<unsigned char>&midiData = *message;	// Deference

		// If end of a sysex string
		if (!midiData.empty() && midiData.back() == 0xF7) ((MainWindow*)userData)->ProcessSysexChunk(midiData);
	}
	void ProcessSysexChunk(std::vector<unsigned char> midiData)
	{
		///////////////////////////////////////////////////////////////////////////////////////////////
		// Not a sysex from a BURN?
		if (midiData.at(1) != 0x08 || midiData.at(2) != 0x45)
		{
			alert("Invalid data");
			BURNisConnected = false;
			DisconnectMIDI();
			return;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		// Connection has been broken?
		if (midiData.at(3) == 0x1F && midiData.at(4) == 0)
		{
			alert("MIDI Connection has been broken");
			DisconnectMIDI();
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		// Received version string?
		if (midiData.at(3) == 0x2A)
		{
			// Stop timeout timer
			timer1->Stop();

			// Clear list
			if (listPresets->GetCount() > 0) listPresets->Clear();

			// Reset preset memory
			memset(PresetData, 0, PRESETCHUNKSIZE * NUMBEROFPRESETS);
			presetMemoryOK = 0;
#if WIN32
			// Clear status bar
			statusBar->SetStatusText(wxT(""));
#endif
			// Print info string
			wxString versString;
			for (int c=4; c<20; c++) versString += midiData.at(c);		//	hex data of ascii characters
			stInfoVersion->SetLabel("Connected to BURN - Version: " + versString);

			// Set buttons on the GUI
			btnDisconnect->Enable();
			btnDownloadPreset->Enable();
			btnDownloadBank->Enable();

			// Mark as connected
			BURNisConnected = true;

			// Now request the whole bank dump...
			unsigned char syxReqBankDump[5] = {0xF0, 0x08, 0x45, 0x2C, 0xF7};
			RtMidiSendSysexChunk(syxReqBankDump, 5);
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// Received a preset?
		if (midiData.at(3) == 0x1A)
		{
			// Get preset number and name
			int presetNum = midiData.at(4); 
			wxString presetName;
			for (int c=5; c<17; c++) presetName += midiData.at(c);

			// This will tell if all 32 presets have been loaded
			presetMemoryOK = listPresets->GetCount();

			// Add preset to the list
			if (presetMemoryOK > presetNum)
				listPresets->SetString(presetNum, presetName);
			else
			{
				listPresets->Append(presetName);
				presetNum = presetMemoryOK++;	// get presetNum and increment presetMemoryOK
			}

/*
			// Add preset to the list
			if (listPresets->GetCount() > presetNum)
				listPresets->SetString(presetNum, presetName);
			else
			{
				listPresets->Append(presetName);
				presetNum = listPresets->GetCount() - 1;
			}

			// This will tell if all 32 presets have been loaded
			presetMemoryOK = listPresets->GetCount();
*/
			// Copy preset data bytes into array
			for (int b=0; b<PRESETCHUNKSIZE; b++) PresetData[presetNum][b] = midiData.at(b+5);

			// Select the current preset
			SelectPreset(presetNum, false);	// don't recall preset from MIDI
#if WIN32
			// Notify on status bar
			char txt[256]; sprintf(txt, "Received preset n. %d: %s", presetNum, presetName.c_str());
			statusBar->SetStatusText(wxT(txt));
#endif
			// Enable this button if entire bank has been loaded and BURN is connected
			if (!btnUploadBank->IsEnabled() && presetMemoryOK == 32 && BURNisConnected) btnUploadBank->Enable();
		}
	}

	// MIDI OUT PORT
	bool OpenMidiOutPort()
	{
		MidiOut->closePort();

		char ermsg[256];
		try { MidiOut->openPort(MidiOutPort); }
		catch ( RtError &rtErr ) {
			sprintf(ermsg, "Unable to open MIDI Output port: %s", MidiOutPortName[MidiOutPort].c_str());
			alert(ermsg);
			return false;
		}

		return true;
	}
	void CloseAllMidiOutPorts()
	{
		MidiOut->closePort();
		RtMidiOutPortsAreOpen = false;
	}
	// MIDI IN PORT
	bool OpenMidiInPort()
	{
		MidiIn->closePort();

		char ermsg[256];
		try { MidiIn->openPort(MidiInPort); }
		catch ( RtError &rtErr ) {
			sprintf(ermsg, "Unable to open MIDI Input port: %s", MidiInPortName[MidiInPort].c_str());
			alert(ermsg);
			return false;
		}

		// Set CallBack function
		MidiIn->ignoreTypes( false, false, false );	// must be set for accepting SysEx
		MidiIn->setCallback(ProcessMidiInput, this);

		return true;
	}
	void CloseAllMidiInPorts()
	{
		MidiIn->closePort();
		RtMidiInPortsAreOpen = false;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TIMER //////////////////////////////////////////////////////////////////////////////////////////////////
	void OnTimerTimeout(wxTimerEvent& WXUNUSED(event))	// This is called every 100 ms
	{
		if (++timeOut > 10) // if 1 second is last...
		{
			timer1->Stop();
			DisconnectMIDI();
			alert("MIDI connection failed!\n\nPlease make sure that the BURN is turned on and connected.\nCheck your cables and select the proper MIDI ports.");
			btnConnect->Enable();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MIDI MENU AND BUTTON FUNCTIONS /////////////////////////////////////////////////////////////////////////
	void OnBtnConnect(wxCommandEvent& WXUNUSED(event))
	{
		// Get MIDI Input port from menu
		for (unsigned int i=0; i<menuInputPort->GetMenuItemCount(); i++) 
			if (menuInputPort->IsChecked(ID_MENU_MIDI_INPUT + i)) 
				MidiInPort = i;

		// Get MIDI Output port from menu
		for (unsigned int i=0; i<menuOutputPort->GetMenuItemCount(); i++) 
			if (menuOutputPort->IsChecked(ID_MENU_MIDI_OUTPUT + i)) 
				MidiOutPort = i;

		if (MidiInPorts < 1 || MidiOutPorts < 1)
		{
			alert("No MIDI ports available. Impossible to connect to the BURN.");
			return;
		}
		
		// Try and save the configuration file
		if (!saveConfig()) alert("Warning! Couldn't save the configuration file.");

		// Disable connect button
		btnConnect->Disable();

		// Close all MIDIports
		CloseAllMidiInPorts();	
		CloseAllMidiOutPorts();	

		// Try to open the requested MIDI ports
		RtMidiInPortsAreOpen = OpenMidiInPort();
		RtMidiOutPortsAreOpen = OpenMidiOutPort();

		// Request activation of MIDI OUT port on RA0
		unsigned char syxStart[6] = { 0xF0, 0x08, 0x45, 0x1F, 0x01, 0xF7 };
		RtMidiSendSysexChunk(syxStart, 6);

		// Must wait a few milliseconds because if the EXT button is on, it must be turned OFF and stored into EEPROM, 
		// which is an interrupt routine that blocks the code for some millisecond
		wxMilliSleep(20);

		// Request version string
		unsigned char syxReqVersString[5] = { 0xF0, 0x08, 0x45, 0x2A, 0xF7 };
		RtMidiSendSysexChunk(syxReqVersString, 5);

		// Start timeout timer
		timeOut = 0;
		timer1->Start(100); // start timer with 100 ms
	}
	void DisconnectMIDI()
	{
		if (BURNisConnected)
		{
			// Request deactivation of MIDI OUT port on RA0
			unsigned char syxStop[6] = { 0xF0, 0x08, 0x45, 0x1F, 0x00, 0xF7 };
			RtMidiSendSysexChunk(syxStop, 6);
		}

		BURNisConnected = false;
		CloseAllMidiInPorts();
		CloseAllMidiOutPorts();
		btnConnect->Enable();
		btnDisconnect->Disable();
		//btnWritePreset->Disable();
		btnDownloadPreset->Disable();
		btnDownloadBank->Disable();
		btnUploadBank->Disable();
		
		stInfoVersion->SetLabel("");
		statusBar->SetStatusText("Disconnected");
	}
	void OnBtnDisconnect(wxCommandEvent& WXUNUSED(event)) { DisconnectMIDI(); }
	void OnBtnMovePreset(wxCommandEvent& event)
	{
		if (!presetMemoryOK) return;

		// Get the preset number
		unsigned char sourcePreset = listPresets->GetSelection();
		unsigned char destPreset(0);
		unsigned char tmpPresetData[PRESETCHUNKSIZE] = { 0 };

		switch (event.GetId())
		{
		case ID_BTN_MOVEUP:
			if (sourcePreset == 0) return; // first preset? can't go beyond
			destPreset = sourcePreset - 1;
			break;
		case ID_BTN_MOVEDOWN:
			if (sourcePreset == NUMBEROFPRESETS-1) return; // last preset? can't go beyond
			destPreset = sourcePreset + 1;
			break;
		}

		// Save the source preset into a temporary buffer
		memcpy(tmpPresetData, PresetData[sourcePreset], PRESETCHUNKSIZE);

		// Init a buffer of zeroes
		int chunkLen = 5 + PRESETCHUNKSIZE + 1;
		unsigned char *syxWritePreset;
		syxWritePreset = new unsigned char[chunkLen];
		memset(syxWritePreset, 0, chunkLen);

		// Header
		syxWritePreset[0] = 0xF0;
		syxWritePreset[1] = 0x08;
		syxWritePreset[2] = 0x45;
		syxWritePreset[3] = 0x1A;
		
		// Footer
		syxWritePreset[chunkLen-1] = 0xF7;

		// Write destination preset into source
		syxWritePreset[4] = sourcePreset;
		for (unsigned char b=0; b<PRESETCHUNKSIZE; b++) syxWritePreset[b+5] = PresetData[destPreset][b];

		// Store the preset in the local memory
		std::vector<unsigned char> localPreset1(syxWritePreset, syxWritePreset + chunkLen);
		ProcessSysexChunk(localPreset1);

		// Send SysEx to the BURN
		if (BURNisConnected) RtMidiSendSysexChunk(syxWritePreset, chunkLen);

		// Now write the temporary buffer into destination
		syxWritePreset[4] = destPreset;
		for (unsigned char b=0; b<PRESETCHUNKSIZE; b++) syxWritePreset[b+5] = tmpPresetData[b];

		// Store the preset in the local memory
		std::vector<unsigned char> localPreset2(syxWritePreset, syxWritePreset + chunkLen);
		ProcessSysexChunk(localPreset2);

		// Send SysEx to the BURN
		if (BURNisConnected) { wxMilliSleep(100); RtMidiSendSysexChunk(syxWritePreset, chunkLen); }

		delete [] syxWritePreset;

		// Select and activate the destination preset
		SelectPreset(destPreset);
	}
	void OnBtnWritePreset(wxCommandEvent& WXUNUSED(event))
	{
		//if (!BURNisConnected) return;
		//if (!confirm("Write this preset into the BURN?")) return;

		// Get the preset number
		unsigned char selectedPreset = listPresets->GetSelection();

		// Init a buffer of zeroes
		int chunkLen = 5 + PRESETCHUNKSIZE + 1;
		unsigned char *syxWritePreset;
		syxWritePreset = new unsigned char[chunkLen];
		memset(syxWritePreset, 0, chunkLen);

		// Header
		syxWritePreset[0] = 0xF0;
		syxWritePreset[1] = 0x08;
		syxWritePreset[2] = 0x45;
		syxWritePreset[3] = 0x1A;
		syxWritePreset[4] = selectedPreset;

		// Get the name
		wxString modifiedPresetName = txtPresetName->GetValue();
		for (int n=modifiedPresetName.length(); n<12; n++) modifiedPresetName.append(" ");	// Append blank spaces up to 12 characters
		for (int n=0; n<12; n++) syxWritePreset[5 + n] = modifiedPresetName.at(n);

		// Get all RT params
		syxWritePreset[5 + PRESET_LOC_FX]			= (unsigned char)cmbEffects->GetSelection();
		syxWritePreset[5 + PRESET_LOC_TUBE_SW]		= (unsigned char)checkOverdrive->GetValue() ? 1 : 0;
		syxWritePreset[5 + PRESET_LOC_TUBE_GAIN]	= (unsigned char)sliderGain->GetValue();
		syxWritePreset[5 + PRESET_LOC_TUBE_LEVEL]	= (unsigned char)sliderLevel->GetValue();
		syxWritePreset[5 + PRESET_LOC_PARAM1]		= (unsigned char)sliderParam1->GetValue();
		syxWritePreset[5 + PRESET_LOC_PARAM2]		= (unsigned char)sliderParam2->GetValue();
		syxWritePreset[5 + PRESET_LOC_PARAM3]		= (unsigned char)sliderParam3->GetValue();
		syxWritePreset[5 + PRESET_LOC_BYPASS]		= (unsigned char)checkBypass->GetValue() ? 1 : 0;

		// Get all NRT params
		for (int n=0; n<NUM_NR_PARAMETERS; n++)
			syxWritePreset[5 + PRESET_LOC_NRPARAM0 + n] = (unsigned char)cmbNRparam[n]->GetSelection();

		// Footer
		syxWritePreset[chunkLen-1] = 0xF7;

		// Store the preset in the local memory
		std::vector<unsigned char> localPreset(syxWritePreset, syxWritePreset + chunkLen);
		ProcessSysexChunk(localPreset);

		// Send SysEx to the BURN
		if (BURNisConnected)
			if (confirm("Write this preset into the BURN?"))
				RtMidiSendSysexChunk(syxWritePreset, chunkLen);

		delete [] syxWritePreset;
	}

	void OnBtnInitPreset(wxCommandEvent& WXUNUSED(event))
	{
		if (!confirm("Initialize selected preset?")) return;

		// Get the preset number
		unsigned char selectedPreset = listPresets->GetSelection();

		// Init a buffer of zeroes
		int chunkLen = 5 + PRESETCHUNKSIZE + 1;
		unsigned char *syxWritePreset;
		syxWritePreset = new unsigned char[chunkLen];
		memset(syxWritePreset, 0, chunkLen);

		// Header
		syxWritePreset[0] = 0xF0;
		syxWritePreset[1] = 0x08;
		syxWritePreset[2] = 0x45;
		syxWritePreset[3] = 0x1A;
		syxWritePreset[4] = selectedPreset;

		// Factory data
		for (int i=0; i<ACTUALPRESETSIZE; i++) syxWritePreset[i+5] = InitPresetData[i];

		// Footer
		syxWritePreset[chunkLen-1] = 0xF7;

		// Store the preset in the local memory
		std::vector<unsigned char> localPreset(syxWritePreset, syxWritePreset + chunkLen);
		ProcessSysexChunk(localPreset);

		// Send SysEx to the BURN
		if (BURNisConnected)
			RtMidiSendSysexChunk(syxWritePreset, chunkLen);

		delete [] syxWritePreset;
	}

	void OnBtnDownload(wxCommandEvent& event)
	{
		if (!BURNisConnected) 
		{
			alert("Please connect to the BURN");
			return;
		}

		// Get Preset Number
		int selectedPreset = listPresets->GetSelection();
		statusBar->SetStatusText("");

		// Download single preset
		if (event.GetId() == ID_BTN_DWNPRESET)
		{
			if (selectedPreset < 0) { alert("Please select a preset from the list."); return; }
			if (!confirm("Overwrite selected preset?")) return;

			// Now request the desired preset...
			unsigned char syxReqBankDump[6] = {0xF0, 0x08, 0x45, 0x2B, selectedPreset, 0xF7};
			RtMidiSendSysexChunk(syxReqBankDump, 6);
		}

		// Download whole bank
		else
		if (event.GetId() == ID_BTN_DWNBANK)
		{
			if (!confirm("Replace whole bank?")) return;

			// Clear list
			listPresets->Clear();

			// Reset preset memory
			memset(PresetData, 0, PRESETCHUNKSIZE * NUMBEROFPRESETS);
			presetMemoryOK = 0;

			// Now request the whole bank dump...
			unsigned char syxReqBankDump[5] = {0xF0, 0x08, 0x45, 0x2C, 0xF7};
			RtMidiSendSysexChunk(syxReqBankDump, 5);
		}
	}

	void OnBtnUploadBank(wxCommandEvent& WXUNUSED(event))
	{
		if (!BURNisConnected) return;
		if (presetMemoryOK != 32) { alert("Preset memory is empty or not fully loaded.\nPlease connect to the BURN or load a bank."); return; }
		if (!confirm("Upload all 32 presets and overwrite the existing ones?")) return;

		// Temporarily disable button
		btnUploadBank->Disable();

		// Prepare the progress bar
		progressBar->SetValue( 0 ); 
		progressBar->Show();
		progressBar->SetRange(NUMBEROFPRESETS);

		// Init a buffer of zeroes
		int chunkLen = 5 + PRESETCHUNKSIZE + 1;
		unsigned char *syxWritePreset;
		syxWritePreset = new unsigned char[chunkLen];
		memset(syxWritePreset, 0, chunkLen);

		char txt[256];

		// Header
		syxWritePreset[0] = 0xF0;
		syxWritePreset[1] = 0x08;
		syxWritePreset[2] = 0x45;
		syxWritePreset[3] = 0x1A;

		for (unsigned char p=0; p<NUMBEROFPRESETS; p++)
		{
			// Preset number
			syxWritePreset[4] = p;

			// Preset data
			for (int b=0; b<PRESETCHUNKSIZE; b++) syxWritePreset[b+5] = PresetData[p][b];

			// Footer
			syxWritePreset[chunkLen-1] = 0xF7;

			// Send
			RtMidiSendSysexChunk(syxWritePreset, chunkLen);

			// Notify
			sprintf(txt, "Uploading preset n. %d...", p);
			statusBar->SetStatusText(txt);

			// Progress bar
			progressBar->SetValue(p);
			
			// Pause
			wxMilliSleep(150);
		}

		// Enable button
		btnUploadBank->Enable();

		// Hide progress bar
		progressBar->Hide();

		// Notify
		statusBar->SetStatusText("Upload Bank done.");
		
		delete [] syxWritePreset;
	}

	void SelectPreset(int selectedPreset, bool MIDI_Recall = true)
	{
		// Enable buttons
		if (!btnWritePreset->IsEnabled())	btnWritePreset->Enable();
		if (!btnInitPreset->IsEnabled())	btnInitPreset->Enable();
		if (!btnMoveUp->IsEnabled())		btnMoveUp->Enable();
		if (!btnMoveDown->IsEnabled())		btnMoveDown->Enable();

		// Set selected the item in the listBox
		if (listPresets->GetSelection() != selectedPreset) listPresets->SetSelection(selectedPreset);

		// Set labels
		unsigned char FX_Number = PresetData[selectedPreset][PRESET_LOC_FX];
		lblParam1->SetLabel(EffectData[FX_Number].prm1);
		lblParam2->SetLabel(EffectData[FX_Number].prm2);
		lblParam3->SetLabel(EffectData[FX_Number].prm3);

		// Set effect
		cmbEffects->SetSelection(PresetData[selectedPreset][PRESET_LOC_FX]);
		SelectFX(PresetData[selectedPreset][PRESET_LOC_FX], false);
		
		// Set parameters
		checkOverdrive->SetValue(PresetData[selectedPreset][PRESET_LOC_TUBE_SW] == 1 ? true : false);
		sliderGain->SetValue(PresetData[selectedPreset][PRESET_LOC_TUBE_GAIN]);
		sliderLevel->SetValue(PresetData[selectedPreset][PRESET_LOC_TUBE_LEVEL]);
		sliderParam1->SetValue(PresetData[selectedPreset][PRESET_LOC_PARAM1]);
		sliderParam2->SetValue(PresetData[selectedPreset][PRESET_LOC_PARAM2]);
		sliderParam3->SetValue(PresetData[selectedPreset][PRESET_LOC_PARAM3]);
		checkBypass->SetValue(PresetData[selectedPreset][PRESET_LOC_BYPASS] == 1 ? true : false);
		for (int n=0; n<NUM_NR_PARAMETERS; n++) cmbNRparam[n]->SetSelection(PresetData[selectedPreset][PRESET_LOC_NRPARAM0 + n]);

		// Load preset on BURN: F0 08 45 2F nn F7
		if (MIDI_Recall) 
		{
			// Set name box
			//wxString thisPresetName = listPresets->GetString(selectedPreset);
			wxString thisPresetName; for (int c=0; c<12; c++) thisPresetName += PresetData[selectedPreset][c];
			thisPresetName.erase(thisPresetName.find_last_not_of(" \n\r\t")+1);	// erase blank spaces at the end of the string
			txtPresetName->SetValue(thisPresetName);

			unsigned char syxSetPreset[6] = { 0xF0, 0x08, 0x45, 0x2F, selectedPreset, 0xF7 };
			if (BURNisConnected) RtMidiSendSysexChunk(syxSetPreset, 6);

			// Status Bar
			char txt[256]; 
			sprintf(txt, "Selected preset n. %d: %s", selectedPreset, listPresets->GetString(selectedPreset).c_str());
			statusBar->SetStatusText(txt);
		}
	}
	void OnSelectPreset(wxCommandEvent& WXUNUSED(event)) { SelectPreset(listPresets->GetSelection()); }
	void OnNRTparam(wxCommandEvent& event)
	{
		if (!BURNisConnected) return;

		unsigned char NRT_Param_Num = event.GetId() - ID_CMB_NRTP0;
		unsigned char NRT_Param_Value = cmbNRparam[NRT_Param_Num]->GetSelection();

		// Set Non Real Time parameter: F0 08 45 1E nn vv F7
		unsigned char syxSetNRTP[7] = { 0xF0, 0x08, 0x45, 0x1E, NRT_Param_Num, NRT_Param_Value, 0xF7 };
		RtMidiSendSysexChunk(syxSetNRTP, 7);
	}

	void OnRTparam(wxCommandEvent& event)
	{
		if (!BURNisConnected) return;

		int RT_Param_Num = event.GetId() - ID_CHK_BYPASS;
		unsigned char value(0);
		char txt[256]; 

		switch(RT_Param_Num)
		{
		case 0:
			value = checkBypass->GetValue();
			sprintf(txt, "Bypass: %s", value ? "ON" : "OFF");
			break;
		case 1:
			value = checkOverdrive->GetValue();
			sprintf(txt, "Overdrive: %s", value ? "ON" : "OFF");
			break;
		case 2:
			value = sliderGain->GetValue();
			sprintf(txt, "Slider: Gain, value: %d", value);
			break;
		case 3:
			value = sliderLevel->GetValue();
			sprintf(txt, "Slider: Level, value: %d", value);
			break;
		case 4:
			value = sliderParam1->GetValue();
			sprintf(txt, "Slider: Parameter 1, value: %d", value);
			break;
		case 5:
			value = sliderParam2->GetValue();
			sprintf(txt, "Slider: Parameter 2, value: %d", value);
			break;
		case 6:
			value = sliderParam3->GetValue();

			// Set Param 3 range
			if (cmbEffects->GetSelection() <= LAST_ROTARY_FX) 
			{
				string Speed; 
				if (value == 0) 
					Speed = "SLOW"; 
				else if (value == 2) 
					Speed = "FAST"; 
				else 
					Speed = "STOP";
				
				sprintf(txt, "Slider: Parameter 3, value: %s", Speed.c_str());
				value *= 63;
			}
			else
				sprintf(txt, "Slider: Parameter 3, value: %d", value);
			break;
		}

		statusBar->SetStatusText(txt);

		// Send parameter value F0 08 45 1D nn vv F7
		unsigned char syxSetParameter[7] = {0xF0, 0x08, 0x45, 0x1D, RT_Param_Num, value, 0xF7};
		RtMidiSendSysexChunk(syxSetParameter, 7);
	}

	void SelectFX(unsigned char FX_Number, bool recall = true)
	{
		// Set Param 3 range
		if (FX_Number <= LAST_ROTARY_FX) 
			sliderParam3->SetMax(2); 
		else 
			sliderParam3->SetMax(127);
		
		// Set labels
		lblParam1->SetLabel(EffectData[FX_Number].prm1);
		lblParam2->SetLabel(EffectData[FX_Number].prm2);
		lblParam3->SetLabel(EffectData[FX_Number].prm3);

		// Set default values (there are only set visually, the actual values are set by the BURN itself)
		sliderParam1->SetValue(EffectData[FX_Number].def_val_1);
		sliderParam2->SetValue(EffectData[FX_Number].def_val_2);
		sliderParam3->SetValue(EffectData[FX_Number].def_val_3);

		// Enable/Disable NRT parameters
		if (FX_Number <= LAST_ROTARY4ORGAN)
			for (int n=0; n<NUM_NR_PARAMETERS; n++) cmbNRparam[n]->Enable();		// Enable all NRT parameters if this is an organ rotary
		else
			for (int n=0; n<(NUM_NR_PARAMETERS-3); n++) cmbNRparam[n]->Disable();	// Disable all NRT parameters except the last 3 if this is NOT an organ rotary

		if (BURNisConnected && recall)
		{
			// Set FX: F0 08 45 1C nn F7
			unsigned char syxSetFX[6] = { 0xF0, 0x08, 0x45, 0x1C, FX_Number, 0xF7 };
			RtMidiSendSysexChunk(syxSetFX, 6);
		}
	}

	void OnCmbFxSelect(wxCommandEvent& WXUNUSED(event)) { SelectFX(cmbEffects->GetSelection()); }
	void OnTxtPresetName(wxCommandEvent& WXUNUSED(event))
	{
		if (!presetMemoryOK) return;

		alert("Error: preset name must be max 12 characters.");

		// Trim name (delete blank spaces at the end)
		wxString modifiedPresetName = txtPresetName->GetValue();
		txtPresetName->ChangeValue(modifiedPresetName.erase(modifiedPresetName.find_last_not_of(" \n\r\t")+1));
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// FILE FUNCTIONS /////////////////////////////////////////////////////////////////////////////////////////
	void OnMenuImport(wxCommandEvent& event)
	{
		if (!FileOpen(event.GetId() == ID_MENU_IMPORT ? false : true, false)) return;
		
		unsigned char loadError = 0;
		const unsigned int SinglePresetFileSize = 5 + PRESETCHUNKSIZE + 1; 
		const unsigned int WholeBankFileSize = SinglePresetFileSize * NUMBEROFPRESETS; 

		// Try to open file
		FILE *fp = fopen(InFileName, "rb");
		if (!fp) { loadError = 5; goto label_load_error; }

		// Get file size
		fseek (fp, 0 , SEEK_END);
		unsigned int FileSize = ftell(fp);
		rewind (fp);

		// Load file content into buffer
		fileBuffer = new unsigned char[FileSize];
		if (fread(fileBuffer, 1, FileSize, fp) != FileSize) { loadError = 1; goto label_load_error; }

		// Check if it's a Sysex file
		if (fileBuffer[0] != 0xF0) { loadError = 2; goto label_load_error; }

		// File contains a single preset?
		if (FileSize == SinglePresetFileSize)
		{
			// Is this really a file for me?
			if (fileBuffer[1] != 0x08 || fileBuffer[2] != 0x45 || fileBuffer[3] != 0x1A) { loadError = 3; goto label_load_error; }

			int selectedPreset = listPresets->GetSelection();
			if (selectedPreset > -1) 
				if (!confirm("File contains a single preset.\nOverwrite the selected preset with this one?"))
					goto label_exit_load;
			
			// Replace original preset number
			if (selectedPreset < 0) selectedPreset = 0;
			fileBuffer[4] = (unsigned char)selectedPreset;
			
			// Process sysex
			std::vector<unsigned char> localPreset(fileBuffer, fileBuffer + FileSize);
			ProcessSysexChunk(localPreset);
		}

		// File contains a whole bank?
		else if (FileSize == WholeBankFileSize)
		{
			if (presetMemoryOK > 0) 
				if (!confirm("This file appears to contain a whole bank.\nDo you wish to replace all presets?"))
					goto label_exit_load;
			
			for (int i=0; i<NUMBEROFPRESETS; i++)
			{
				//if (fileBuffer[1] != 0x08 || fileBuffer[2] != 0x45 || fileBuffer[3] != 0x1A)

				unsigned char *thisPresetBuffer;
				thisPresetBuffer = new unsigned char[SinglePresetFileSize];

				int thisPresetStart = i * SinglePresetFileSize;
				for (int b = 0; b < SinglePresetFileSize; b++)
					thisPresetBuffer[b] = fileBuffer[thisPresetStart + b];

				std::vector<unsigned char> localPreset(thisPresetBuffer, thisPresetBuffer + SinglePresetFileSize);
				ProcessSysexChunk(localPreset);

				delete [] thisPresetBuffer;
			}
		}

		// file contains neither a bank nor a preset for the BURN
		else loadError = 4;

label_load_error:
		switch (loadError)
		{
		case 1:	alert("Error while loading file!"); break;
		case 2:	alert("Not a MIDI System Exclusive file!"); break;
		case 3:	alert("Wrong file format!"); break;
		case 4:	alert("Not a valid file!"); break;
		case 5:	alert("Unable to open file!"); break;
		}

label_exit_load:
		if (fp) fclose(fp);
		delete [] fileBuffer;			
	}
	void OnMenuExport(wxCommandEvent& event)
	{
		bool presetORbank = event.GetId() == ID_MENU_EXPORT_PRESET ? false : true;	// true = bank, false = preset

		if (presetORbank && presetMemoryOK != 32) 
		{
			alert("Preset memory is empty or not fully loaded.\nPlease connect to the BURN or load a bank.");
			return;
		}

		unsigned char selectedPreset = listPresets->GetSelection();
		if (presetMemoryOK < 1) { alert("Nothing to save."); return; }

		// Invoke file explorer
		if (!FileOpen(presetORbank, true)) return;

		FILE *fp = fopen(InFileName, "wb");
		if (fp)
		{
			if (presetORbank) 
			{
				// save whole bank
				//fwrite(PresetData, NUMBEROFPRESETS * PRESETCHUNKSIZE, 1, fp);
				for (unsigned char i=0; i<NUMBEROFPRESETS; i++)
				{
					fputc (0xF0, fp);
					fputc (0x08, fp);
					fputc (0x45, fp);
					fputc (0x1A, fp);
					fputc (i, fp);
					fwrite(PresetData[i], PRESETCHUNKSIZE, 1, fp);
					fputc (0xF7, fp);
				}
			} else {
				// save selected preset
				fputc (0xF0, fp);
				fputc (0x08, fp);
				fputc (0x45, fp);
				fputc (0x1A, fp);
				fputc (selectedPreset, fp);
				fwrite(PresetData[selectedPreset], PRESETCHUNKSIZE, 1, fp);
				fputc (0xF7, fp);
			}

			fclose(fp);
		} else
			alert("Couldn't write file to disk!\n\nPlease check disk permissions or make sure that\nthe file is not locked by another application.");
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ABOUT //////////////////////////////////////////////////////////////////////////////////////////////////
	void OnMenuAbout(wxCommandEvent& WXUNUSED(event))
	{
/*
#if defined(__WXMSW__)
		wxBitmap about_bmp( wxT("ABOUTBMP") );
#else	// MAC OSX
		wxBitmap about_bmp( wxStandardPaths::Get().GetResourcesDir() + "/BurnEd_about.bmp" );
#endif
*/

		wxMemoryInputStream memIStream( BurnEd_about_bmp, sizeof( BurnEd_about_bmp ) );
		wxImage image( memIStream, wxBITMAP_TYPE_BMP );
		wxBitmap bmp( image );

		// No need to point it to a variable
		new wxSplashScreen( bmp,// wxBitmap(wxImage(wxMemoryInputStream(BurnEd_about_bmp, sizeof(BurnEd_about_bmp)), wxBITMAP_TYPE_BMP)),
			wxSPLASH_CENTRE_ON_PARENT | wxSPLASH_TIMEOUT, 5000, this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxSIMPLE_BORDER|wxSTAY_ON_TOP|wxFRAME_TOOL_WINDOW | wxFRAME_NO_TASKBAR);
	}


	// Event table
	DECLARE_EVENT_TABLE()
};

#endif //__BURNEDGUI_H__
