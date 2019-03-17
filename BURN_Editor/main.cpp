///////////////////////////////////////////////////////////////////////////
// GSi BURN Editor - Version 1.0 - October 2013
// Uses wxWidgets and RtMidi for cross platform (Win and Mac) compatibility
//
// www.GenuineSoundware.com
// 
///////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "BurnEdGUI.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
 
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


// Define a new application type, each program should derive a class from wxApp
class BurnEd : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_TIMER(TIMER_ID,					MainWindow::OnTimerTimeout)
	EVT_BUTTON(ID_BTN_CONNECT,			MainWindow::OnBtnConnect)
	EVT_BUTTON(ID_BTN_DISCONNECT,		MainWindow::OnBtnDisconnect)
	EVT_BUTTON(ID_BTN_WRITEPRESET,		MainWindow::OnBtnWritePreset)
	EVT_BUTTON(ID_BTN_INITPRESET,		MainWindow::OnBtnInitPreset)
	EVT_BUTTON(ID_BTN_UPLOADBANK,		MainWindow::OnBtnUploadBank)
	EVT_BUTTON(ID_BTN_DWNBANK,			MainWindow::OnBtnDownload)
	EVT_BUTTON(ID_BTN_DWNPRESET,		MainWindow::OnBtnDownload)
	EVT_BUTTON(ID_BTN_MOVEUP,			MainWindow::OnBtnMovePreset)
	EVT_BUTTON(ID_BTN_MOVEDOWN,			MainWindow::OnBtnMovePreset)

	EVT_MENU(ID_MENU_IMPORT,			MainWindow::OnMenuImport)
	EVT_MENU(ID_MENU_EXPORT_PRESET,		MainWindow::OnMenuExport)
	EVT_MENU(ID_MENU_EXPORT_BANK,		MainWindow::OnMenuExport)

	EVT_MENU(ID_MENU_HELP_ABOUT,		MainWindow::OnMenuAbout)
	EVT_MENU(ID_MENU_QUIT,				MainWindow::OnQuit)
	
	EVT_LISTBOX(ID_PRESETLIST,			MainWindow::OnSelectPreset)

	EVT_CHECKBOX(ID_CHK_BYPASS,			MainWindow::OnRTparam)
	EVT_CHECKBOX(ID_CHK_OVERDRIVE,		MainWindow::OnRTparam)

	EVT_SLIDER(ID_SLIDER_GAIN,			MainWindow::OnRTparam)
	EVT_SLIDER(ID_SLIDER_LEVEL,			MainWindow::OnRTparam)
	EVT_SLIDER(ID_SLIDER_PARAM1,		MainWindow::OnRTparam)
	EVT_SLIDER(ID_SLIDER_PARAM2,		MainWindow::OnRTparam)
	EVT_SLIDER(ID_SLIDER_PARAM3,		MainWindow::OnRTparam)

	EVT_COMBOBOX(ID_CMB_NRTP0,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP1,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP2,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP3,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP4,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP5,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP6,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP7,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP8,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP9,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP10,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP11,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP12,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP13,			MainWindow::OnNRTparam)
	EVT_COMBOBOX(ID_CMB_NRTP14,			MainWindow::OnNRTparam)

	EVT_COMBOBOX(ID_FX_SELECT,			MainWindow::OnCmbFxSelect)
	EVT_TEXT_MAXLEN(ID_TXT_PRESETNAME,	MainWindow::OnTxtPresetName)

END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(BurnEd)

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool BurnEd::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

	// create the main application window
    MainWindow *frame = new MainWindow(NULL);

	// and show it (the frames, unlike simple controls, are not shown when created initially)
    frame->Show(true);

	// Alert if no MIDI ports have been found.
	if (frame->MidiOutPorts == 0 || frame->MidiInPorts == 0) 
		frame->alert("It appears that this system lacks the required MIDI connections.\nPlease connect a MIDI interface with at least\none input and one output, then restart the application.");


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Configuration file
	//wxString AppName = wxApp::GetAppName(); //
	//frame->configFile = wxStandardPaths::Get().GetUserConfigDir() + "/" + wxApp::GetAppName() + ".ini";
	wxString configDir = wxStandardPaths::Get().GetUserConfigDir() + "/GSi BURN Editor";
	frame->configFile = configDir + "/Config.ini";
	//frame->info((char*)configFile.c_str());
	
	// Create config dir if doesn't exist
	if (!wxDirExists(configDir)) wxMkdir(configDir);
	
	// Load config
	frame->loadConfig();
	
	// success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}
