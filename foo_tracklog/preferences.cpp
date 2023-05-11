#include <helpers/foobar2000+atl.h>

#include "preferences.h"

static const GUID guid_advconfig_branch = { 0x83822d1c, 0x8498, 0x4f91, { 0x83, 0x04, 0x40, 0x49, 0x76, 0x29, 0x29, 0xfb } };
static const GUID guid_cfg_showtime = { 0xf715a73f, 0x566f, 0x4b8f, { 0x8c, 0xd0, 0x25, 0x29, 0x3f, 0xd8, 0xb9, 0xb7 } };
static const GUID guid_cfg_tracknum = { 0x1909874e, 0x0321, 0x4e59, { 0x95, 0x0e, 0x07, 0x2d, 0x23, 0xc1, 0x21, 0x38 } };
static const GUID guid_cfg_logtofile = { 0x4284eba4, 0x8990, 0x4c46, { 0x9f, 0x7a, 0xad, 0xea, 0x0d, 0x3a, 0xec, 0x9a } };
static const GUID guid_cfg_logappend = { 0xd2a5402a, 0xd9f2, 0x4d0d, { 0x88, 0x1e, 0x48, 0xcd, 0x91, 0x69, 0xb9, 0x8a } };
static const GUID guid_cfg_streamtext = { 0xb20d6e7c, 0x31bb, 0x4901, { 0x8f, 0x02, 0x5a, 0xa4, 0x81, 0x7c, 0x07, 0x2c } };
static const GUID guid_cfg_tracktext = { 0x874bdaa2, 0xcf78, 0x4cb4, { 0xbf, 0x93, 0x9e, 0x47, 0xb3, 0xf4, 0x86, 0xdb } };
static const GUID guid_cfg_logfile = { 0x5d0fb341, 0x13ad, 0x4179, { 0xa2, 0x7a, 0xc0, 0xd3, 0xdb, 0x21, 0xd3, 0xb2 } };
static const GUID guid_cfg_timefmt = { 0x33d195e7, 0x09e8, 0x470d,{ 0x95, 0xbb, 0x96, 0x63, 0x30, 0xb9, 0xee, 0x23 } };

advconfig_branch_factory g_advconfigBranch("TrackLog", guid_advconfig_branch, advconfig_branch::guid_branch_tools, 0);
advconfig_checkbox_factory cfg_showtime("Log time", guid_cfg_showtime, guid_advconfig_branch, 0, true);
advconfig_string_factory cfg_timefmt("Time format (strftime[32])", guid_cfg_timefmt, guid_advconfig_branch, 0, "%Y %b %#d %H:%M:%S");
advconfig_checkbox_factory cfg_tracknum("Log track number", guid_cfg_tracknum, guid_advconfig_branch, 0, true);
advconfig_string_factory cfg_streamtext("Stream open text", guid_cfg_streamtext, guid_advconfig_branch, 0, "starting stream \"%title%\"");
advconfig_string_factory cfg_tracktext("New record text", guid_cfg_tracktext, guid_advconfig_branch, 0, "new track \"[%artist% - ][%title%]\"");
advconfig_checkbox_factory cfg_logtofile("Log to a file", guid_cfg_logtofile, guid_advconfig_branch, 0, false);
advconfig_string_factory cfg_logfile("Log file path", guid_cfg_logfile, guid_advconfig_branch, 0, "");
advconfig_checkbox_factory cfg_logappend("Append logfile", guid_cfg_logappend, guid_advconfig_branch, 0, true);
