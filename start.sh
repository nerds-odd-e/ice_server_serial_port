socat -v -d -d pty,raw,echo=0 tcp-listen:4641,reuseaddr,fork &
/opt/ice_server/build/ice_server_serial_port
