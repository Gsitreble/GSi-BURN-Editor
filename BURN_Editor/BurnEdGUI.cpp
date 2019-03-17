///////////////////////////////////////////////////////////////////////////
// GSi BURN Editor - Version 1.0 - October 2013
// Uses wxWidgets and RtMidi for cross platform (Win and Mac) compatibility
//
// www.GenuineSoundware.com
// 
///////////////////////////////////////////////////////////////////////////

#include "BurnEdGUI.h"

///////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxFrame( parent, id, title, pos, size, style, name )
{

	// Init variables
	MidiOutPort = MidiInPort = -1;
	RtMidiOutPortsAreOpen = RtMidiInPortsAreOpen = false;
	sendingMIDI = false;
	BURNisConnected = false;
	presetMemoryOK = 0;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MAIN WINDOW
	this->SetSizeHints( wxSize( MAIN_WIN_SIZE ), wxSize( MAIN_WIN_SIZE ) );
	this->SetBackgroundColour( wxColour(255, 255, 255, 255) /*wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW )*/ );

// Compiling for Mac? Default fonts are too big, make them smaller! (Damn OSX = Shit!)
#if defined (__WXMAC__) || defined (__WXOSX__)
	SetWindowVariant(wxWINDOW_VARIANT_SMALL);

	wxFont font(8, //wxSMALL_FONT->GetPointSize(),
			wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
			wxFONTWEIGHT_NORMAL);
	this->SetFont(font);

	// Use generic List Control
	wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), 1);
	
	// Link the OSX about menu
	wxApp::s_macAboutMenuItemId = ID_MENU_HELP_ABOUT;
#endif

#if WIN32
	SetIcon(wxICON(ICON_GSi));
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Status Bar
	statusBar = this->CreateStatusBar();// 1, wxST_SIZEGRIP, wxID_ANY );
	statusBar->SetStatusText(APPLICATION_NAME);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MENU
	menuBar = new wxMenuBar( 0 );
	menuFile = new wxMenu();
	menuImport = new wxMenu();
	menuExport = new wxMenu();
	menuMIDI = new wxMenu();
	menuInputPort = new wxMenu(); 
	menuOutputPort = new wxMenu();
	menuHelp = new wxMenu();

	menuFile->Append( ID_MENU_IMPORT, wxT("Import SysEx") );
	
	wxMenuItem* m_menuItem11;
	m_menuItem11 = new wxMenuItem( menuExport, ID_MENU_EXPORT_PRESET, wxString( wxT("Selected Preset") ) , wxEmptyString, wxITEM_NORMAL );
	menuExport->Append( m_menuItem11 );
	
	wxMenuItem* m_menuItem12;
	m_menuItem12 = new wxMenuItem( menuExport, ID_MENU_EXPORT_BANK, wxString( wxT("Whole Bank") ) , wxEmptyString, wxITEM_NORMAL );
	menuExport->Append( m_menuItem12 );
	
	menuFile->Append( -1, wxT("Export"), menuExport );
	
	menuFile->AppendSeparator();
	
	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( menuFile, ID_MENU_QUIT, wxString( wxT("Quit") ) , wxEmptyString, wxITEM_NORMAL );
	menuFile->Append( m_menuItem3 );
	
	menuBar->Append( menuFile, wxT("File") ); 
	
	menuMIDI->Append( ID_MENU_MIDI_INPUT + 200, wxT("Input port"), menuInputPort );
	menuMIDI->Append( ID_MENU_MIDI_OUTPUT + 200, wxT("Output port"), menuOutputPort );
	
	menuBar->Append( menuMIDI, wxT("MIDI") ); 
	
	wxMenuItem* m_menuItem15;
	m_menuItem15 = new wxMenuItem( menuHelp, ID_MENU_HELP_ABOUT, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
	menuHelp->Append( m_menuItem15 );
	
	menuBar->Append( menuHelp, wxT("?") ); 
	
	this->SetMenuBar( menuBar );
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MAIN SECTION
	wxBoxSizer* spMainContainer;
	spMainContainer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* spTop;
	spTop = new wxBoxSizer( wxHORIZONTAL );
/*	
#if defined(__WXMSW__)
	wxBitmap cover_bmp( wxT("COVERBMP") );
#else	// MAC OSX
	wxBitmap cover_bmp( wxStandardPaths::Get().GetResourcesDir() + "/BurnEd_cover.pct", wxBITMAP_TYPE_PICT_RESOURCE  );
#endif
	bmpCover = new wxStaticBitmap( this, wxID_ANY, cover_bmp, wxDefaultPosition, wxDefaultSize, 0 );
*/
	// wxBitmap(wxImage(wxMemoryInputStream(BurnEd_cover_bmp, sizeof(BurnEd_cover_bmp)), wxBITMAP_TYPE_BMP))

	wxMemoryInputStream memIStream( BurnEd_cover_bmp, sizeof( BurnEd_cover_bmp ) );
	wxImage image( memIStream, wxBITMAP_TYPE_BMP );
	wxBitmap bmp( image );
	bmpCover = new wxStaticBitmap( this, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, 0 );
	spTop->Add( bmpCover, 0, wxALL, 5 );
	
	wxBoxSizer* spTopControls;
	spTopControls = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* spConnectionButtons;
	spConnectionButtons = new wxBoxSizer( wxVERTICAL );
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TOP BUTTONS
	btnConnect = new wxButton( this, ID_BTN_CONNECT, wxT("CONNECT"), wxPoint( -1,-1 ), wxSize( 110, 30 ), 0 );
	spConnectionButtons->Add( btnConnect, 0, wxALL, 5 );
	
	btnDisconnect = new wxButton( this, ID_BTN_DISCONNECT, wxT("DISCONNECT"), wxDefaultPosition, wxSize( 110, 30 ), 0 );
	btnDisconnect->Disable();
	spConnectionButtons->Add( btnDisconnect, 0, wxALL, 5 );
		
	spTopControls->Add( spConnectionButtons, 1, wxALIGN_RIGHT, 5 );
	
	wxBoxSizer* spInfoStrings;
	spInfoStrings = new wxBoxSizer( wxHORIZONTAL );
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TOP INFO STRINGS
	stInfoVersion = new wxStaticText( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	stInfoVersion->Wrap( -1 );
	spInfoStrings->Add( stInfoVersion, 0, wxALL, 5 );
		
	spTopControls->Add( spInfoStrings, 0, 0, 0 );
		
	spTop->Add( spTopControls, 1, 0, 0 );
		
	spMainContainer->Add( spTop, 0, wxEXPAND, 0 );
	
	wxBoxSizer* spSeparator;
	spSeparator = new wxBoxSizer( wxVERTICAL );
	
	spSeparator->SetMinSize( wxSize( -1,20 ) ); 
	spSeparator->Add( new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL ), 0, wxEXPAND | wxALL, 5 );
	
	spMainContainer->Add( spSeparator, 0, wxEXPAND, 5 );
	
	wxBoxSizer* spAll_Controls;
	spAll_Controls = new wxBoxSizer( wxHORIZONTAL );

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PRESET LIST
	listPresets = new wxListBox(this, ID_PRESETLIST, wxDefaultPosition, wxSize( 150,LISTHEIGHT ), 0, NULL, wxLB_SINGLE|wxLB_NEEDED_SB );
	spAll_Controls->Add( listPresets, 0, 0, 0 );


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PRESET PARAMETERS
	wxBoxSizer* spPreset_Parameters;
	spPreset_Parameters = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	checkBypass = new wxCheckBox( this, ID_CHK_BYPASS, wxT("Bypass"), wxDefaultPosition, wxSize( 70,-1 ), 0 );
	bSizer8->Add( checkBypass, 0, wxALL, 5 );
	
	checkOverdrive = new wxCheckBox( this, ID_CHK_OVERDRIVE, wxT("Overdrive"), wxDefaultPosition, wxSize( 70,-1 ), 0 );
	bSizer8->Add( checkOverdrive, 0, wxALL, 5 );

	bSizer8->Add( new wxStaticText( this, wxID_ANY, wxT("Effect Select: "), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL, 5 );
	
	cmbEffects = new wxComboBox( this, ID_FX_SELECT, wxEmptyString, wxDefaultPosition, wxSize( 160,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer8->Add( cmbEffects, 0, wxALL, 5 );

	bSizer8->Add( new wxStaticText( this, wxID_ANY, wxT("Preset Name:"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL, 5 );

	txtPresetName = new wxTextCtrl( this, ID_TXT_PRESETNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 /*wxTE_PROCESS_ENTER*/  );
	txtPresetName->SetMaxLength(12);
	bSizer8->Add( txtPresetName, 0, wxALL, 5 );

		
	spPreset_Parameters->Add( bSizer8, 0, 0, 0 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer9->Add( new wxStaticText( this, wxID_ANY, wxT("GAIN"), wxDefaultPosition, wxSize(35, -1), 0 ), 0, wxALL, 5 );
	sliderGain = new wxSlider( this, ID_SLIDER_GAIN, 50, 0, 127, wxDefaultPosition, wxSize(SLIDERSIZE, -1), wxSL_HORIZONTAL );
	bSizer9->Add( sliderGain, 0, wxALL, 5 );
	
	bSizer9->Add( new wxStaticText( this, wxID_ANY, wxT("LEVEL"), wxDefaultPosition, wxSize(35, -1), 0 ), 0, wxALL, 5 );
	sliderLevel = new wxSlider( this, ID_SLIDER_LEVEL, 50, 0, 127, wxDefaultPosition, wxSize(SLIDERSIZE, -1), wxSL_HORIZONTAL );
	bSizer9->Add( sliderLevel, 0, wxALL, 5 );
	
	lblParam1 = new wxStaticText( this, wxID_ANY, wxT("PARAM 1"), wxDefaultPosition, wxSize(LABELSIZE, -1), 0 );
	bSizer9->Add( lblParam1, 0, wxALL, 5 );
	sliderParam1 = new wxSlider( this, ID_SLIDER_PARAM1, 50, 0, 127, wxDefaultPosition, wxSize(SLIDERSIZE, -1), wxSL_HORIZONTAL );
	bSizer9->Add( sliderParam1, 0, wxALL, 5 );
	
	lblParam2 = new wxStaticText( this, wxID_ANY, wxT("PARAM 2"), wxDefaultPosition, wxSize(LABELSIZE, -1), 0 );
	bSizer9->Add( lblParam2, 0, wxALL, 5 );
	sliderParam2 = new wxSlider( this, ID_SLIDER_PARAM2, 50, 0, 127, wxDefaultPosition, wxSize(SLIDERSIZE, -1), wxSL_HORIZONTAL );
	bSizer9->Add( sliderParam2, 0, wxALL, 5 );
	
	lblParam3 = new wxStaticText( this, wxID_ANY, wxT("PARAM 3"), wxDefaultPosition, wxSize(LABELSIZE, -1), 0 );
	bSizer9->Add( lblParam3, 0, wxALL, 5 );
	sliderParam3 = new wxSlider( this, ID_SLIDER_PARAM3, 50, 0, 127, wxDefaultPosition, wxSize(SLIDERSIZE, -1), wxSL_HORIZONTAL );
	bSizer9->Add( sliderParam3, 0, wxALL, 5 );
	
	spPreset_Parameters->Add( bSizer9, 0, 0, 0 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer10->Add( new wxStaticText( this, wxID_ANY, wxT("Horn Slow Speed:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[0] = new wxComboBox( this, ID_CMB_NRTP0, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer10->Add( cmbNRparam[0], 0, wxALL, 5 );
	
	bSizer10->Add( new wxStaticText( this, wxID_ANY, wxT("Horn Fast Speed:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[1] = new wxComboBox( this, ID_CMB_NRTP1, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer10->Add( cmbNRparam[1], 0, wxALL, 5 );
	
	bSizer10->Add( new wxStaticText( this, wxID_ANY, wxT("Bass Slow Speed:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[2] = new wxComboBox( this, ID_CMB_NRTP2, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer10->Add( cmbNRparam[2], 0, wxALL, 5 );
	
	bSizer10->Add( new wxStaticText( this, wxID_ANY, wxT("Bass Fast Speed:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[3] = new wxComboBox( this, ID_CMB_NRTP3, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer10->Add( cmbNRparam[3], 0, wxALL, 5 );
		
	spPreset_Parameters->Add( bSizer10, 0, 0, 0 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer11->Add( new wxStaticText( this, wxID_ANY, wxT("Horn Ramp Up:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[4] = new wxComboBox( this, ID_CMB_NRTP4, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer11->Add( cmbNRparam[4], 0, wxALL, 5 );
	
	bSizer11->Add( new wxStaticText( this, wxID_ANY, wxT("Horn Ramp Down:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[5] = new wxComboBox( this, ID_CMB_NRTP5, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer11->Add( cmbNRparam[5], 0, wxALL, 5 );
	
	bSizer11->Add( new wxStaticText( this, wxID_ANY, wxT("Bass Ramp Up:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[6] = new wxComboBox( this, ID_CMB_NRTP6, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer11->Add( cmbNRparam[6], 0, wxALL, 5 );
	
	bSizer11->Add( new wxStaticText( this, wxID_ANY, wxT("Bass Ramp Down:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[7] = new wxComboBox( this, ID_CMB_NRTP7, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer11->Add( cmbNRparam[7], 0, wxALL, 5 );
		
	spPreset_Parameters->Add( bSizer11, 0, 0, 0 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer12->Add( new wxStaticText( this, wxID_ANY, wxT("Freq. Mod. Amount:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[8] = new wxComboBox( this, ID_CMB_NRTP8, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer12->Add( cmbNRparam[8], 0, wxALL, 5 );
	
	bSizer12->Add( new wxStaticText( this, wxID_ANY, wxT("Crossover Cutoff:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[9] = new wxComboBox( this, ID_CMB_NRTP9, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer12->Add( cmbNRparam[9], 0, wxALL, 5 );
	
	bSizer12->Add( new wxStaticText( this, wxID_ANY, wxT("Horn Resonance:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[10] = new wxComboBox( this, ID_CMB_NRTP10, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer12->Add( cmbNRparam[10], 0, wxALL, 5 );
	
	bSizer12->Add( new wxStaticText( this, wxID_ANY, wxT("Horn Timbre:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[11] = new wxComboBox( this, ID_CMB_NRTP11, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer12->Add( cmbNRparam[11], 0, wxALL, 5 );
		
	spPreset_Parameters->Add( bSizer12, 0, 0, 0 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer13->Add( new wxStaticText( this, wxID_ANY, wxT("Reverb Length:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[12] = new wxComboBox( this, ID_CMB_NRTP12, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer13->Add( cmbNRparam[12], 0, wxALL, 5 );
	
	bSizer13->Add( new wxStaticText( this, wxID_ANY, wxT("Reverb Tone:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[13] = new wxComboBox( this, ID_CMB_NRTP13, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer13->Add( cmbNRparam[13], 0, wxALL, 5 );
	
	bSizer13->Add( new wxStaticText( this, wxID_ANY, wxT("Reverb Gain:"), wxDefaultPosition, wxSize( 100,-1 ), 0 ), 0, wxALL, 5 );
	
	cmbNRparam[14] = new wxComboBox( this, ID_CMB_NRTP14, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	bSizer13->Add( cmbNRparam[14], 0, wxALL, 5 );
		
	spPreset_Parameters->Add( bSizer13, 0, 0, 0 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	bSizer15->Add( new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL ), 0, wxEXPAND | wxALL, 5 );
		
	spPreset_Parameters->Add( bSizer15, 0, wxEXPAND, 0 );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );

	// Buttons: Move UP & Move DOWN /////////////////////////////////////////////////////////////////////////////////////////////////////
	wxBoxSizer *bSizerUpDown = new wxBoxSizer( wxVERTICAL );
	
	btnMoveUp = new wxButton( this, ID_BTN_MOVEUP, wxT("Move UP"), wxDefaultPosition, wxSize( 80, 22 ), 0 ); btnMoveUp->Disable();
	bSizerUpDown->Add( btnMoveUp, 0, wxALL, 5 );

	btnMoveDown = new wxButton( this, ID_BTN_MOVEDOWN, wxT("Move Down"), wxDefaultPosition, wxSize( 80, 22 ), 0 ); btnMoveDown->Disable();
	bSizerUpDown->Add( btnMoveDown, 0, wxALL, 5 );

	bSizer16->Add( bSizerUpDown, 0, 0, 0 );
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	btnInitPreset = new wxButton( this, ID_BTN_INITPRESET, wxT("Init Preset"), wxDefaultPosition, wxSize( 110, 40 ), 0 ); btnInitPreset->Disable();
	bSizer16->Add( btnInitPreset, 0, wxALL, 5 );

	btnDownloadPreset = new wxButton( this, ID_BTN_DWNPRESET, wxT("Download Preset"), wxDefaultPosition, wxSize( 110, 40 ), 0 ); btnDownloadPreset->Disable();
	bSizer16->Add( btnDownloadPreset, 0, wxALL, 5 );

	btnDownloadBank = new wxButton( this, ID_BTN_DWNBANK, wxT("Download Bank"), wxDefaultPosition, wxSize( 110, 40 ), 0 ); btnDownloadBank->Disable();
	bSizer16->Add( btnDownloadBank, 0, wxALL, 5 );

	btnWritePreset = new wxButton( this, ID_BTN_WRITEPRESET, wxT("Write Preset"), wxDefaultPosition, wxSize( 110, 40 ), 0 ); btnWritePreset->Disable();
	bSizer16->Add( btnWritePreset, 0, wxALL, 5 );

	btnUploadBank = new wxButton( this, ID_BTN_UPLOADBANK, wxT("Upload Bank"), wxDefaultPosition, wxSize( 110, 40 ), 0 );	btnUploadBank->Disable();
	bSizer16->Add( btnUploadBank, 0, wxALL, 5 );

	//bSizer16->Add( new wxButton( this, -1, wxT("fake button"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL, 5 );

	spPreset_Parameters->Add( bSizer16, 0, 0, 0 );
		
	spAll_Controls->Add( spPreset_Parameters, 0, 0, 0 );
		
	spMainContainer->Add( spAll_Controls, 0, 0, 0 );

	// This uses an absolute position, is not in any sizer
	progressBar = new wxGauge( this, wxID_ANY, 32, wxPoint(160, 405), wxSize( 790,-1 ), wxGA_HORIZONTAL|wxGA_SMOOTH ); 
	progressBar->SetValue( 0 ); progressBar->Hide();

	// Hyperlink to GSi Website :-) // No need to point it to a variable
    new wxHyperlinkCtrl(this, wxID_ANY, wxT("GSi Website"), wxT("www.genuinesoundware.com"), wxPoint(887, 325));

	this->SetSizer( spMainContainer );
	this->Layout();
	this->Centre( wxBOTH );


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Fill combos and preset list
	int i;

	//for (i=0; i<NUMBEROFPRESETS; i++) listPresets->Append("Empty");

	for (i=0; i<TOTEFFECTS+1; i++)
		cmbEffects->Append(EffectData[i].name);

	for (i=0; i<NUM_NR_PARAMETERS; i++)
	{
		for (int p=0; p<nr_parameters[i].max_idx+1; p++)
			cmbNRparam[i]->Append(nr_parameters[i].value_str[p]);
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setup Timer
	timer1 = new wxTimer(this, TIMER_ID); // timer needs an ID, can't use wxID_ANY


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// RTMIDI
	MidiOut = new RtMidiOut();		MidiOutPorts = MidiOut->getPortCount();
	MidiIn = new RtMidiIn();		MidiInPorts = MidiIn->getPortCount();
	
	char txt[256]; 
	//sprintf(txt, "Found %d MIDI OUT and %d MIDI IN ports", MidiOutPorts, MidiInPorts); info(txt);
	
	// Get MIDI Out port names
	for (int i=0; i<MidiOutPorts; i++)
	{
		MidiOutPortName.push_back(MidiOut->getPortName(i));

		sprintf(txt, "%d) %s", i+1, MidiOutPortName[i].c_str());
		menuOutputPort->AppendRadioItem(ID_MENU_MIDI_OUTPUT + i, txt);	// every radio item must have a unique ID
	}

	// Get MIDI In port names
	for (int i=0; i<MidiInPorts; i++)
	{
#if WIN32
		// I don't know why but MIDI IN ports add the index number at the end of the string. I have to remove it, I only need the literal name.
		string portName = MidiIn->getPortName(i);
		stringstream portNum; portNum << i; int pos = portName.rfind(" " + portNum.str()); // find the string " i" where i is the value of variable i and delete it from the string
		MidiInPortName.push_back(portName.erase(pos));
#else
		MidiInPortName.push_back(MidiIn->getPortName(i));
#endif
		sprintf(txt, "%d) %s", i+1, MidiInPortName[i].c_str());
		menuInputPort->AppendRadioItem(ID_MENU_MIDI_INPUT + i, txt); // every radio item must have a unique ID
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

// DESTROY
MainWindow::~MainWindow()
{
	DisconnectMIDI();
	CloseAllMidiOutPorts();
	CloseAllMidiInPorts();
	delete MidiOut; 
	delete MidiIn;
}

void MainWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

bool MainWindow::FileOpen(bool presetORbank = false, bool loadORsave = false)
{
	char winTitle[256]; 
	//sprintf(winTitle, "Select the %s file to %s", presetORbank ? "bank" : "preset", loadORsave ? "save" : "load");

	if (loadORsave)
		sprintf(winTitle, "Select the %s file to save", presetORbank ? "bank" : "preset");
	else
		sprintf(winTitle, "Select the file to load");

	// Invoce the file selector
	static wxString s_extDef;
	wxString path = wxFileSelector(
		winTitle,
		wxEmptyString, wxEmptyString,
		s_extDef,
		wxString::Format
		(
			_T("System Exclusive (*.SYX)|*.syx|All files (%s)|%s"), wxFileSelectorDefaultWildcardStr, wxFileSelectorDefaultWildcardStr
		),
		loadORsave ? wxFD_SAVE|wxFD_OVERWRITE_PROMPT : wxFD_OPEN|wxFD_CHANGE_DIR|wxFD_PREVIEW,
		this
	);

	if (!path) return false;

	// The MAC doesn't add the extension! Mac = Shit
#if defined (__WXMAC__) || defined (__WXOSX__)
	if (loadORsave) path += ".syx";
#endif

	// it is just a sample, would use wxSplitPath in real program
	s_extDef = path.AfterLast(_T('.'));

	//wxLogMessage(_T("You selected the file '%s', remembered extension '%s'"), (const wxChar*) path, (const wxChar*) s_extDef);
	strcpy(InFileName, (const wxChar*) path);

	return true;
}

bool MainWindow::loadConfig()
{
	string line;
	ifstream fh (configFile);
	if (!fh) return false;

	if (!fh.eof())
	{
		// Skip header
		getline (fh, line);

		// Read variables...
		getline (fh, line);
		if (line.substr(0, 7)	== "OUTPUT=")
			for (int i=0; i<MidiOutPorts; i++)
				if (line.substr(7, line.length()) == MidiOutPortName[i])
					MidiOutPort = i;

		getline (fh, line);
		if (line.substr(0, 6)	== "INPUT=")
			for (int i=0; i<MidiInPorts; i++)
				if (line.substr(6, line.length()) == MidiInPortName[i])
					MidiInPort = i;
	}
	fh.close();

	// Set MIDI OUT
	if (MidiOutPort > -1 && MidiOutPort <= (int)menuOutputPort->GetMenuItemCount()) 
		menuOutputPort->FindItemByPosition(MidiOutPort)->Check();
	else
		if (MidiOutPorts > 0) alert("Please set MIDI OUT port.");

	// Set MIDI IN
	if (MidiInPort > -1 && MidiInPort <= (int)menuInputPort->GetMenuItemCount())
		menuInputPort->FindItemByPosition(MidiInPort)->Check();
	else
		if (MidiInPorts > 0) alert("Please set MIDI IN port.");

	return true;
}

bool MainWindow::saveConfig()
{
	ofstream file(configFile);
	if (!file) return false; 
	file << "[MIDI PORTS]"	<< "\n";
	file << "OUTPUT="	<< MidiOutPortName[MidiOutPort].c_str()	<< "\n";
	file << "INPUT="	<< MidiInPortName[MidiInPort].c_str()	<< "\n";
	file.close();

	return true;
}

