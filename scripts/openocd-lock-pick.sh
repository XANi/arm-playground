#!/bin/bash
CONFDIR='/usr/share/openocd/scripts'

openocd -f openocd.cfg -f "$CONFDIR/interface/jtagkey.cfg" -f "$CONFDIR/target/at91sam7x256.cfg"  $@