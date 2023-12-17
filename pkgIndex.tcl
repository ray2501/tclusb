# -*- tcl -*-
# Tcl package index file, version 1.1
#
if {[package vsatisfies [package provide Tcl] 9.0-]} {
    package ifneeded tclusb 0.3 \
	    [list load [file join $dir libtcl9tclusb0.3.so] [string totitle tclusb]]
} else {
    package ifneeded tclusb 0.3 \
	    [list load [file join $dir libtclusb0.3.so] [string totitle tclusb]]
}
