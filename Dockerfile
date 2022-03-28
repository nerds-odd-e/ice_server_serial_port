FROM rikorose/gcc-cmake:gcc-8

ADD . /opt/ice_server
WORKDIR /opt/ice_server/build

RUN cmake ..
RUN make

ENTRYPOINT ./ice_server_serial_port
