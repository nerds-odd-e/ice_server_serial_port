FROM lionelman45/rhel6.4_gcc

RUN sed -i 's/1/0/g' /etc/yum/pluginconf.d/subscription-manager.conf
COPY ./centos-base.repo /etc/yum.repos.d/yum.repo
RUN yum -y install wget tar gcc-c++.x86_64

WORKDIR /opt
RUN wget http://download.oracle.com/berkeley-db/db-4.3.29.tar.gz \
      && tar -zxvf db-4.3.29.tar.gz \
      && mv db-4.3.29 db \
      && cd db \
      && cd build_unix \
      && ../dist/configure --prefix=/usr/local/berkeleydb --enable-cxx \
      && make \
      && make install \
      && echo '/usr/local/berkeleydb/lib/' >> /etc/ld.so.conf.d/db.conf \
      && cp /usr/local/berkeleydb/lib/libdb_cxx.so /usr/lib \
      && ldconfig \
      && rm -f /opt/db-4.3.29.tar.gz

ARG CMAKE_VERSION=3.22.3
RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
      -q -O /tmp/cmake-install.sh \
      && chmod u+x /tmp/cmake-install.sh \
      && mkdir /usr/bin/cmake \
      && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
      && rm /tmp/cmake-install.sh
ENV PATH="/usr/bin/cmake/bin:${PATH}"

RUN yum -y install bzip2-devel.x86_64 expat-devel.x86_64 openssl-devel.x86_64

COPY ./Ice-3.1.1 /opt/Ice-3.1.1_src
WORKDIR /opt/Ice-3.1.1_src
RUN make
RUN make install
ENV PATH="/opt/Ice-3.1/bin:${PATH}"
ENV LD_LIBRARY_PATH="/opt/Ice-3.1/lib64:${LD_LIBRARY_PATH}"

COPY ./socat-1.7.2.4-1.el6.rf.x86_64.rpm /opt/socat/socat.rpm
RUN yum -y install tcp_wrappers minicom
RUN rpm -ivh /opt/socat/socat.rpm

COPY ./src /opt/ice_server
WORKDIR /opt/ice_server
RUN slice2cpp Server.ice

WORKDIR /opt/ice_server/build
RUN cmake ..
RUN make

COPY ./start.sh /opt/scripts/start.sh
ENTRYPOINT /opt/scripts/start.sh

