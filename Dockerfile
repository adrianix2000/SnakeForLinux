FROM ubuntu:latest

COPY src /app/src

WORKDIR /app/src

RUN apt-get update && \
    apt-get install -y libncurses5-dev g++ && \
    g++ -o snake snake.cpp -lncurses

CMD ["./snake"]
 


