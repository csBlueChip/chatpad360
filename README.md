# chatpad360
An open source keyboard driver for the Cbox 360 Chatpad with standard firmware

There are still a few minor issue with this:
- You require People+5 to start the keyboard working at the login prompt
- Bear in mind it's (currently) a keyboard stuffing daemon, not a "keyboard driver"
- There's no user definable functionality yet
- It desperately needs to handle packet fragmentation gracefully
- There quite a lot of code tidying still to be done
- Theres way too much documentation - every problem is going to finish with "tl;dr"

...However, you can [well, *I* can] type:- 
`make exe`, `make install`, 
reboot, wait for the login prompt; 
press and hold [People], and tap [5] on the chatpad (to rebind it to the terminal)

Read the NOTES file
