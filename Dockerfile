#FROM rikorose/gcc-cmake:gcc-8
FROM lionelman45/rhel6.4_gcc

RUN sed -i 's/1/0/g' /etc/yum/pluginconf.d/subscription-manager.conf
ADD ./centos-base.repo /etc/yum.repos.d/yum.repo
RUN yum -y install wget tar

ARG CMAKE_VERSION=3.22.3
RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
      -q -O /tmp/cmake-install.sh \
      && chmod u+x /tmp/cmake-install.sh \
      && mkdir /usr/bin/cmake \
      && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
      && rm /tmp/cmake-install.sh
ENV PATH="/usr/bin/cmake/bin:${PATH}"

RUN yum -y install gcc-c++.x86_64 bzip2-devel.x86_64 expat-devel.x86_64 openssl-devel.x86_64

ADD . /opt/ice_server
WORKDIR /opt/ice_server/Ice-3.1.1
RUN make

WORKDIR /opt/ice_server/build
RUN cmake ..
RUN make

ENTRYPOINT ./ice_server_serial_port
#ENTRYPOINT tail -f /dev/null
