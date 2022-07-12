#!/usr/bin/env wish8.6
puts [tk scaling]
# tk scaling 4.0
# puts [tk scaling]
# puts [info tclversion]
puts [info patchlevel]
set img [image create photo add2schem -file add2schematic-2x.png]
button .hello -text "Hello, World!" -image $img -command { exit }
set btn_icon [.hello cget -image]
puts before
$btn_icon configure -width 16 -height 16
puts after
pack .hello
