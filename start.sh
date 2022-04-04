socat -v -d -d pty,raw,echo=0 tcp-listen:4641,reuseaddr,fork &
sleep 3
socat -x -d -d PTY,raw,echo=0 tcp:host.docker.internal:4641 &
/opt/ice_server/build/ice_server_serial_port
