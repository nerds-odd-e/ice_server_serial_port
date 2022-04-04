socat -v -d -d pty,raw,echo=0 tcp-listen:4641,reuseaddr,fork &
socat PTY,raw,echo=0 tcp:host.docker.internal:4641 &
/opt/ice_server/build/ice_server_serial_port
