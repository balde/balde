# Development only docker, run with `docker run -it -v $(pwd):/balde balde make check`
# in example.
FROM balde/balde
RUN apt-get update && apt-get install valgrind libglib2.0-0-dbg -y && apt-get clean
WORKDIR /balde
