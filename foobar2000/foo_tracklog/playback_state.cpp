#include <helpers/foobar2000+atl.h>

#include "preferences.h"

typedef struct _log_job
{
	pfc::string8 fname;
	bool append;
	pfc::string_formatter str;
} log_job;

static LPWSTR MBS2WCS(pfc::string8 mbs)
{
	int bufsz;
	LPWSTR ret;

	bufsz = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, mbs.c_str(), -1, NULL, 0);
	if (bufsz == 0)
		return NULL;
	ret = new WCHAR[bufsz];
	if (ret == NULL)
		return NULL;
	MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, mbs.c_str(), -1, ret, bufsz);
	return ret;
}

static DWORD WINAPI fileLogThreadProc(LPVOID lpParameter)
{
	static pfc::string8 pfname = "";

	if (lpParameter == NULL)
		return 0;

	const log_job *job = (log_job*)lpParameter;

	LPCWSTR fname = MBS2WCS(job->fname);
	if (fname == NULL) {
		delete job;
		return 0;
	}

	HANDLE logFile;
	DWORD delay = 10;
	do {
		if ((logFile = CreateFile(fname, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE) {
			DWORD written;

			if ((GetLastError() == ERROR_ALREADY_EXISTS) && (job->append || (job->fname == pfname))) {
				// seek to the end
				LARGE_INTEGER pos = { 0 };
				if (!SetFilePointerEx(logFile, pos, NULL, FILE_END)) {
					CloseHandle(logFile);
					break;
				}
			} else {
				// truncate and add BOM
				char BOM[] = "\xEF\xBB\xBF";

				SetEndOfFile(logFile);
				if (!WriteFile(logFile, BOM, sizeof(BOM) - 1, &written, NULL)) {
					CloseHandle(logFile);
					break;
				}

			}

			char NL[] = "\r\n";
			WriteFile(logFile, job->str, (DWORD)job->str.get_length(), &written, NULL);
			WriteFile(logFile, NL, sizeof(NL) - 1, &written, NULL);
			CloseHandle(logFile);
			pfname = job->fname;
		} else if (GetLastError() == ERROR_SHARING_VIOLATION && delay < 100) { // max approx 150ms
			Sleep(delay);
			delay *= 2;
			continue;
		}
		break;
	} while (1);

	delete[] fname;
	delete job;
	return 0;
}

class CTrackLog : public play_callback_static
{
	unsigned int get_flags(void)
	{
		return flag_on_playback_dynamic_info_track|flag_on_playback_new_track;
	}

	// Playback callback methods.
	void on_playback_new_track(metadb_handle_ptr p_track) { newtrack(p_track); }
	void on_playback_dynamic_info_track(const file_info & p_info) { updateinfo(p_info); }

	void on_playback_starting(play_control::t_track_command p_command,bool p_paused) { }
	void on_playback_stop(play_control::t_stop_reason p_reason) {}
	void on_playback_seek(double p_time) {}
	void on_playback_pause(bool p_state) {}
	void on_playback_edited(metadb_handle_ptr p_track) {}
	void on_playback_dynamic_info(const file_info & p_info) {}
	void on_playback_time(double p_time) {}
	void on_volume_change(float p_new_val) {}

	pfc::string8& ct(pfc::string8 & str, const char *format);
	pfc::string8& getNum(pfc::string8 & str);
	bool fileLog(pfc::string_formatter & str);
	void newtrack(metadb_handle_ptr p_track);
	void updateinfo(const file_info & p_info);

	int tracknum;
};
static play_callback_static_factory_t<CTrackLog> TrackLog;

pfc::string8& CTrackLog::ct(pfc::string8 & str, const char *format)
{
	time_t rawtime;
	struct tm timeinfo;

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);
	strftime(str.lock_buffer(32), 32, format, &timeinfo);
	str.unlock_buffer();
	return str;
}

pfc::string8& CTrackLog::getNum(pfc::string8 & str)
{
	_snprintf_s(str.lock_buffer(16), 16, _TRUNCATE, "#%d ", tracknum);
	str.unlock_buffer();
	return str;
}

bool CTrackLog::fileLog(pfc::string_formatter & str)
{
	log_job *job;

	job = new log_job;
	if (job == NULL)
		return false;
	cfg_logfile.get(job->fname);
	if (job->fname.is_empty()) {
		delete job;
		return false;
	}
	job->append = cfg_logappend.get();
	job->str = str;

	if (CreateThread(NULL, 0, &fileLogThreadProc, (LPVOID)job, 0, NULL) == NULL) {
		delete job;
		return false;
	}

	return true;
}

void CTrackLog::newtrack(metadb_handle_ptr p_track)
{
	if (p_track->get_length() <= 0.0) {
		tracknum = 0;
		pfc::string8 pattern;
		cfg_streamtext.get(pattern);
		service_ptr_t<titleformat_object> streamscript;
		static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(streamscript,	pattern);
		if (streamscript.is_empty())
			return;
		pfc::string_formatter stream;
		if (p_track->format_title(NULL, stream, streamscript, NULL)) {
		} else if (static_api_ptr_t<playback_control>()->is_playing()) {
			stream = "Opening...";
		} else {
			stream = "Stopped.";
		}
		if (cfg_showtime.get()) {
			pfc::string8 str, timefmt;

			stream.insert_chars(0, " ");
			cfg_timefmt.get(timefmt);
			stream.insert_chars(0, ct(str, timefmt));
		}
		console::print(stream);
		if (cfg_logtofile.get())
			fileLog(stream);
	}
}

void CTrackLog::updateinfo(const file_info & p_info)
{
	if (p_info.get_length() <= 0.0) {
		pfc::string8 pattern;
		cfg_tracktext.get(pattern);
		service_ptr_t<titleformat_object> trackscript;
		static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(trackscript, pattern);
		if (trackscript.is_empty())
			return;
		pfc::string_formatter track;
		static_api_ptr_t<playback_control>()->playback_format_title(NULL, track, trackscript,
			NULL, playback_control::display_level_all);
		tracknum++;
		if (cfg_tracknum.get()) {
			pfc::string8 str;

			track.insert_chars(0, getNum(str));
		}
		if (cfg_showtime.get()) {
			pfc::string8 str, timefmt;

			track.insert_chars(0, " ");
			cfg_timefmt.get(timefmt);
			track.insert_chars(0, ct(str, timefmt));
		}
		console::print(track);
		if (cfg_logtofile.get())
			fileLog(track);
	}
}
