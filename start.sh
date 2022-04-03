socat -v -d -d pty,link=/dev/ttyS0,raw,echo=0,b9600 tcp-listen:4641,reuseaddr,fork &
socat PTY,raw,echo=0,link=/dev/ttyS0 tcp:host.docker.internal:4641 &
/opt/ice_server/build/ice_server_serial_port
