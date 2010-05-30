namespace eval tas {

set_help_text toggle_frame_counter\
{Toggles display of a frame counter in the lower right corner.

The frame counter will be wrong if at any time there is a change of VDP
register 9 bit 1 during the run time of the machine! (So switching between 50/60Hz.)
}

proc toggle_frame_counter {} {

	if {![catch {osd info framecount}]} {
		osd destroy framecount
		return ""
	}

	osd create rectangle framecount -x 280 -y 235 -h 6 -w 50 -rgba 0x0077FF80 -scaled true
	osd create text framecount.text -text "" -size 5 -rgba 0xffffffff

	framecount_update
	return ""
}

proc framecount_current {} {
	set freq [expr {(6 * 3579545) / (1368 * (([vdpreg 9] & 1) ? 313 : 262))}]
	return [expr int([machine_info time] * $freq)]
}

proc framecount_update {} {
	if {[catch {osd info framecount}]} {return ""}
	osd configure framecount.text -text "Frame: [framecount_current]"
	after frame [namespace code framecount_update]
	return ""
}

# TODO: this doesn't work properly, why?
# The best way is to enable some kind of global TAS mode (a boolean setting),
# which if enabled sets up some useful key bindings for TAS
#
#set_help_text frame_advance_mode \
#{frame_advance_mode let's you go through frames by pushing f7}
#
#proc frame_advance_mode {} {
#	bind_default f7 "set pause off;after frame \"set pause on\""
#}

# more TODO:
# real time OSD memory watch
# TAS mode indicators

namespace export toggle_frame_counter
#namespace export frame_advance_mode
}

namespace import tas::*