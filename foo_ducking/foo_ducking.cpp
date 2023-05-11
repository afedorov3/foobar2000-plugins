#include <helpers/foobar2000+atl.h>

// Declaration of your component's version information
// Since foobar2000 v1.0 having at least one of these in your DLL is mandatory to let the troubleshooter tell different versions of your component apart.
// Note that it is possible to declare multiple components within one DLL, but it's strongly recommended to keep only one declaration per DLL.
// As for 1.1, the version numbers are used by the component update finder to find updates; for that to work, you must have ONLY ONE declaration per DLL. If there are multiple declarations, the component is assumed to be outdated and a version number of "0" is assumed, to overwrite the component with whatever is currently on the site assuming that it comes with proper version numbers.
DECLARE_COMPONENT_VERSION("Ducking control","1.0","Controls response to ducking events\n\nOpt in by default.\nTo opt out, enable\nPreferences - Advanced - Tools - Ducking control - Opt out\n");


// This will prevent users from renaming your component around (important for proper troubleshooter behaviors) or loading multiple instances of it.
VALIDATE_COMPONENT_FILENAME("foo_ducking.dll");

static const GUID guid_advconfig_branch = { 0x92d07a22, 0xbd04, 0x4aa9, { 0x81, 0x8a, 0xd6, 0xeb, 0x76, 0x02, 0x06, 0x80 } };
static const GUID guid_cfg_optout = { 0x088ad07d, 0x6a7b, 0x4362, { 0x84, 0x73, 0xb1, 0x5f, 0xe8, 0x33, 0xc1, 0x9b } };

advconfig_branch_factory g_advconfigBranch("Ducking control", guid_advconfig_branch, advconfig_branch::guid_branch_tools, 0);
advconfig_checkbox_factory cfg_optout("Opt out (requires restart)", guid_cfg_optout, guid_advconfig_branch, 0, false);

#include <Mmdeviceapi.h>
#include <Audiopolicy.h>

///////////////////////////////////////////////////////////////////
//https://msdn.microsoft.com/en-us/library/windows/desktop/dd940391(v=vs.85).aspx
////////////////////////////////////////////////////////////////////
//Description: Specifies the ducking options for the application.
//Parameters: 
//    If DuckingOptOutChecked is TRUE system ducking is disabled; 
//    FALSE, system ducking is enabled.
////////////////////////////////////////////////////////////////////

static HRESULT DuckingOptOut(bool DuckingOptOutChecked)
{
    HRESULT hr = S_OK;

    IMMDeviceEnumerator* pDeviceEnumerator = NULL;
    IMMDevice* pEndpoint = NULL;
    IAudioSessionManager2* pSessionManager2 = NULL;
    IAudioSessionControl* pSessionControl = NULL;
    IAudioSessionControl2* pSessionControl2 = NULL;


    //  Start with the default endpoint.

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), 
                          NULL, 
                          CLSCTX_INPROC_SERVER, 
                          IID_PPV_ARGS(&pDeviceEnumerator));
    
    if (SUCCEEDED(hr))
    {
        hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pEndpoint);

        pDeviceEnumerator->Release();
        pDeviceEnumerator = NULL;
    }

    // Activate session manager.
    if (SUCCEEDED(hr))
    {
        hr = pEndpoint->Activate(__uuidof(IAudioSessionManager2), 
                                 CLSCTX_INPROC_SERVER,
                                 NULL, 
                                 reinterpret_cast<void **>(&pSessionManager2));
        pEndpoint->Release();
        pEndpoint = NULL;
    }
    if (SUCCEEDED(hr))
    {
        hr = pSessionManager2->GetAudioSessionControl(NULL, 0, &pSessionControl);
        
        pSessionManager2->Release();
        pSessionManager2 = NULL;
    }

    if (SUCCEEDED(hr))
    {
        hr = pSessionControl->QueryInterface(
                  __uuidof(IAudioSessionControl2),
                  (void**)&pSessionControl2);
                
        pSessionControl->Release();
        pSessionControl = NULL;
    }

    //  Sync the ducking state with the specified preference.

    if (SUCCEEDED(hr))
    {
        if (DuckingOptOutChecked)
        {
            hr = pSessionControl2->SetDuckingPreference(TRUE);
        }
        else
        {
            hr = pSessionControl2->SetDuckingPreference(FALSE);
        }
        pSessionControl2->Release();
        pSessionControl2 = NULL;
    }
    return hr;
}

static void adjustDucking() {
    // don't touch ducking at all if not opting out
    if (cfg_optout.get()) {
        HRESULT ret;
        if (FAILED(ret = DuckingOptOut(true)))
            console::formatter() << "Ducking control: opt out enable failed: "
                                 << ret;
    }
}

class CDucking : public play_callback_static
{
	unsigned int get_flags(void)
    {
        return flag_on_playback_starting;
    }
	void on_playback_new_track(metadb_handle_ptr p_track) {  }
	void on_playback_dynamic_info_track(const file_info & p_info) {  }

	void on_playback_starting(play_control::t_track_command p_command,bool p_paused)
                             { adjustDucking(); }
	void on_playback_stop(play_control::t_stop_reason p_reason) {}
	void on_playback_seek(double p_time) {}
	void on_playback_pause(bool p_state) {}
	void on_playback_edited(metadb_handle_ptr p_track) {}
	void on_playback_dynamic_info(const file_info & p_info) {}
	void on_playback_time(double p_time) {}
	void on_volume_change(float p_new_val) {}
};
static play_callback_static_factory_t<CDucking> Ducking;
