FROM gcc:latest
RUN mkdir /usr/src/myapp
COPY ./out/zhangshao.out /usr/src/myapp
COPY ./in/1618991563MrZhang_0_2.in /usr/src/myapp
COPY ./dockerfile/OJCore /usr/src/myapp
WORKDIR /usr/src/myapp
CMD ["./OJCore", "./zhangshao.out", "1000", "65535", "./1618991563MrZhang_0_2.in"]