FROM ubuntu:14.04

ENV PKG_BUILD_DIR /tmp
ENV CMAKE_VERSION 3.5.2
ENV CMOCKA_VERSION 1.0.1

RUN apt-get update && apt-get install -y \
	autoconf \
	automake \
	build-essential \
	curl \
	gdb \
	libtool \
	pkg-config \
	uuid-dev \
&& rm -rf /var/lib/apt/lists/*

WORKDIR $PKG_BUILD_DIR

RUN curl -sSL https://cmake.org/files/v3.5/cmake-$CMAKE_VERSION.tar.gz | tar xz \
	&& cd cmake-$CMAKE_VERSION \
	&& ./bootstrap && make && make install \
	&& cd $PKG_BUILD_DIR \
	&& rm -rf cmake-$CMAKE_VERSION

RUN curl -sL https://cmocka.org/files/1.0/cmocka-$CMOCKA_VERSION.tar.xz | tar xJf - \
	&& cd cmocka-$CMOCKA_VERSION \
	&& mkdir build && cd build \
	&& cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release .. \
	&& make && make install \
	&& cd $PKG_BUILD_DIR \
	&& rm -rf cmocka-$CMOCKA_VERSION \
	&& rm -rf cmocka-$CMOCKA_VERSION.tar.xz

RUN apt-get purge -y \
	autoconf \
	automake \
	build-essential \
	curl \
	libtool \
	pkg-config \
	uuid-dev

RUN groupadd -r rtree && useradd -r -m -g rtree rtree
WORKDIR /home/rtree/rtree
COPY . .
RUN chown -R rtree:rtree /home/rtree && chmod g+s /home/rtree
USER rtree

CMD ["make"]
