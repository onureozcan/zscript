FROM gcc:6.5.0
RUN mkdir /antlr && \
    cd /antlr && \
    wget https://github.com/antlr/antlr4/archive/master.zip

RUN curl -sSL https://cmake.org/files/v3.5/cmake-3.5.2-Linux-x86_64.tar.gz | tar -xzC /opt

ENV PATH="/opt/cmake-3.5.2-Linux-x86_64/bin/:${PATH}"

RUN apt-get update && apt-get install uuid-dev

RUN cd /antlr && \
    unzip master.zip &&\
    cd antlr4-master/runtime/Cpp &&\
    mkdir build && mkdir run && cd build &&\
    cmake .. && make && ls

RUN cp /antlr/antlr4-master/runtime/Cpp/dist/libantlr4-runtime.a /usr/lib
# TODO: why not using include directory instead?
RUN cp -R /antlr/antlr4-master/runtime/Cpp/runtime/src/* /usr/local/include/

# install java
RUN apt-get install -y default-jre

# download antlr jar
RUN curl https://www.antlr.org/download/antlr-4.7.1-complete.jar -o /usr/local/lib/antlr-4.7.1-complete.jar

# install valgrind
RUN apt-get install -y valgrind
