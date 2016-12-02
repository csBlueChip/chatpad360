#!/usr/bin/perl
# http://www.comptechdoc.org/os/linux/howlinuxworks/linux_hlkeycodes.html

$TIOCSTI = 0x5412;
$tty = "/dev/pts/2";

#$char = "a";
#$char = $ARGV[0];
#$char = pack "H*","42";
$char = pack "H*",$ARGV[0];

open($fh, ">", $tty);
ioctl($fh, $TIOCSTI, $char)