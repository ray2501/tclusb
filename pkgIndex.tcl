# -*- tcl -*-
# Tcl package index file, version 1.1
#
if {[package vsatisfies [package provide Tcl] 9.0-]} {
    package ifneeded tclusb 0.5 \
	    [list load [file join $dir libtcl9tclusb0.5.so] [string totitle tclusb]]
} else {
    package ifneeded tclusb 0.5 \
	    [list load [file join $dir libtclusb0.5.so] [string totitle tclusb]]
}
