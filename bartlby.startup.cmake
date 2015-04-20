#! /bin/bash

### BEGIN INIT INFO
# Provides:          bartlby
# Required-Start:    $local_fs $remote_fs $network
# Required-Stop:     $local_fs $remote_fs $network
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Start Bartlby Monitoring Core
# Description:       This Script starts the Bartlby-Core monitoring Process
#                    
### END INIT INFO

ulimit -c 1000
export MALLOC_CHECK_=2



# Source function library
. /lib/lsb/init-functions


BARTLBY_HOME=@prefix@
ETC_DIR="etc/"
BIN_DIR="bin/"

NAME=bartlby
DESC="Bartlby-Core"
PATH=/sbin:/bin:/usr/sbin:/usr/bin:$BARTLBY_HOME/bin/:$BARTLBY_HOME/sbin/
DAEMON=$BARTLBY_HOME/bin/bartlby


#PIDDIR=`getConfigValue "pidfile_dir"|tr -d [:cntrl:]`;
PIDDIR=$BARTLBY_HOME/var/
PIDFILE=$PIDDIR/bartlby.pid

test -x $DAEMON || exit 0

# Include varnish defaults if available
if [ -f /etc/default/bartlby ] ; then
        . /etc/default/bartlby
fi

function getConfigValue {
    if [ ! -f $BARTLBY_HOME/${ETC_DIR}bartlby.cfg ];
    then
        log_failure_msg "Config $BARTLBY_HOME/${ETC_DIR}bartlby.cfg doesnt look like a file"  
        log_end_msg 1
        exit;
    fi;
    
    r=`cat $BARTLBY_HOME/${ETC_DIR}bartlby.cfg |grep "^${1}"|awk -F"=" '{print \$2}'`
    echo $r;

}

function bartlby_status {
    OUTP=$(${BARTLBY_HOME}/${BIN_DIR}bartlby_shmt $BARTLBY_HOME/${ETC_DIR}bartlby.cfg status)
    STATUS_EX=$?;
    if [ $STATUS_EX = 2 ];
    then
        log_failure_msg "Cannot find SHM segment - bartlby does not seem to run";
    else
        status_of_proc -p "${PIDFILE}" "${DAEMON}" "${NAME}"
        PROC_FOUND=$?; #0 == OK RUNNING
        
    fi;    
    
    
}



DAEMON_OPTS=${DAEMON_OPTS:-$BARTLBY_HOME/${ETC_DIR}bartlby.cfg}
case "$1" in
    start)
        RUSER=`getConfigValue "user"|tr -d [:cntrl:]`;

        output=$(/bin/tempfile -s.bartlby)
        log_daemon_msg "Starting $DESC" 
            
        if start-stop-daemon \
           --start --quiet --pidfile ${PIDFILE} -a ${DAEMON} -u $RUSER -- \
                   ${DAEMON_OPTS} > ${output} 2>&1; then

            log_end_msg 0
        else
            if [ $? = 1 ];
            then
                status="0"
                pidofproc -p $PIDFILE  >/dev/null || status="$?"
                
                if [ $status = 1 ];
                then
                    #log_failure_msg " already running";
                    log_failure_msg  "not Running but SHM Segment already exists run 'remove' or 'restart' "
                else
                    log_failure_msg " already running"
                fi;
            
            fi;
            
        fi
        rm $output
        ;;
    stop)
        log_daemon_msg "Stopping $DESC"
        
        log_progress_msg $NAME
        if start-stop-daemon \
           --stop  --quiet --pidfile $PIDFILE --retry 10; then
            log_end_msg 0
        else
            log_end_msg 1
        fi
            ;;
    status)
        bartlby_status;
        ;;
    remove)
        log_daemon_msg "Removing Shared Memory Segment."


        OUTP=$(${BARTLBY_HOME}/${BIN_DIR}bartlby_shmt $BARTLBY_HOME/${ETC_DIR}bartlby.cfg status)
        SHMID=$(echo $OUTP|awk '{print $1}');
            ipcrm shm $SHMID 2>&1 >> /dev/null;
            rm -f $1 2>&1 >> /dev/null;


        log_progress_msg "removed"
        log_end_msg 0
        
        
        ;;
    reload)
        PIDT=$(cat $PIDFILE 2>/dev/null);
        log_daemon_msg "Scheduling Reload"
        echo kill -USR2 $PIDT 2>&1 >> /dev/null;
        log_progress_msg "done"
        log_end_msg 0

    ;;
    restart)
        $0 stop
        $0 remove
        $0 start
            ;;
    *)
            log_success_msg "Usage: $0 {start|stop|restart}"
            exit 1
        ;;
esac

exit 0;















