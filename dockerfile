FROM alpine:latest as development

RUN apk --no-cache --no-check-certificate update && apk --no-cache --no-check-certificate upgrade
RUN apk add --no-cache --no-check-certificate ca-certificates openssl
RUN update-ca-certificates

RUN apk add --no-check-certificate --no-cache \
        build-base \
        cmake \
        gcc \
        g++ \
        make \
        gdb \
        git \
        curl \
        wget \
        libc-dev \
        c-ares-dev \
        re2-dev
RUN apk add --no-check-certificate --no-cache \
        protobuf \
        protobuf-dev \
        grpc \
        grpc-dev 
RUN apk add --no-check-certificate --no-cache \
        valgrind

WORKDIR /app

RUN wget --no-check-certificate https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.zip
RUN unzip v3.11.2.zip

## COMPILING APP
COPY proto ./proto
WORKDIR /app/proto
RUN mkdir -p cpp
RUN protoc --proto_path=. --cpp_out=cpp --grpc_out=cpp --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` service.proto
RUN mkdir python
RUN protoc --python_out=python --grpc_out=python --plugin=protoc-gen-grpc=`which grpc_python_plugin` service.proto
WORKDIR /app
COPY CMakeLists.txt .
COPY test ./test
COPY catalog ./catalog
COPY include ./include
COPY src ./src
WORKDIR /app/build
RUN cmake ..
RUN make -j 4

## PRODUCTION
FROM alpine:latest as production

RUN apk --no-cache --no-check-certificate update && apk --no-cache --no-check-certificate upgrade
RUN apk add --no-cache --no-check-certificate ca-certificates openssl
RUN update-ca-certificates

RUN apk add --no-check-certificate --no-cache grpc-dev 
WORKDIR /app
COPY --from=development /app/catalog /catalog
COPY --from=development /app/build/openformat /app/openformat
COPY --from=development /app/build/client /app/client
CMD ["./openformat"]
