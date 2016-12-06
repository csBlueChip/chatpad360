#! /bin/sh

## TOTALLY STUMPED
## NO MATTER WHAT I DO IT COMES UP BEFORE THE GETTY
## AND DOESN'T WORK DURING BOOK
## I THINK THE GETTY IS STEALING THE TTY fd & fh
## HOW DO I GET THIS START AFTER THE GETTY??

### BEGIN INIT INFO
# Provides:          chatpad360
# Required-Start:    $all
# Required-Stop:     $remote_fs $syslog $time
# Should-Start:      
# Should-Stop:       
# Default-Start:     3 4 5
# Default-Stop:      0 6
# Short-Description: Chatpad360 Serial Keybaord Daemon v0.5.1
### END INIT INFO

NAME=chatpad360
DESC="Chatpad360 Serial Keyboard daemon v0.5.1"
DAEMON=/usr/sbin/chatpad360
PIDFILE=/var/run/$NAME.pid
LOGFILE=/var/log/chatpad360.log
CONFIG=/etc/chatpad360.conf
ARGS="-n $CONFIG"
RUNAS=root

. /lib/lsb/init-functions

PATH=/bin:/usr/bin:/sbin:/usr/sbin

[ -x $DAEMON ] || exit 0

ulimit -n 65535

start() {
	if [ -f /var/run/$PIDNAME ] && kill -0 $(cat /var/run/$PIDNAME); then
		log_failure_msg 'Service already running'
		return 1
	fi
	local CMD="$SCRIPT &> \"$LOGFILE\" & echo \$!"
	start-stop-daemon --quiet --start \
		--pidfile $PIDFILE \
		--exec $DAEMON -- $ARGS
	return $?
}

stop() {
	if [ ! -f "$PIDFILE" ] || ! kill -0 $(cat "$PIDFILE"); then
		log_failure_msg 'Service not running'
		return 1
	fi
	start-stop-daemon --quiet --stop \
		--pidfile $PIDFILE \
		--exec $DAEMON
	return $?
}

case "$1" in
	start)
		log_daemon_msg "Starting $DESC" "$NAME"
		if start ; then
			log_end_msg $?
		else
			log_end_msg $?
		fi
	;;

	stop)
		log_daemon_msg "Stopping $DESC" "$NAME"
		if stop ; then
			log_end_msg $?
		else
			log_end_msg $?
		fi
	;;

	reload|force-reload)
		echo "not implemented"
		exit 1
	;;

	restart)
		echo "not implemented"
		exit 1
	;;

	status)
		status_of_proc -p $PIDFILE $DAEMON $NAME && exit 0 || exit 3
	;;

	*)
		echo "Usage: /etc/init.d/$NAME {start|stop|reload|force-reload|restart|status}"
		exit 3
	;;
esac

exit 0
