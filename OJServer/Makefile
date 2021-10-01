TGT := ekoj
SRC := OJServer.cc main.cc OJSql.cc OJ.cc
OBJ :=

CFLAGS := -I$(HEAD_DIR) -I/usr/local/lib/ \
						-I/usr/include/mysql \
						-I/usr/local/include/mysql++

LD :=   -L/usr/local/lib \
		-L/usr/lib64/mysql/ \
		-levent \
		-lpthread
		
# 动态库
LDLIBS := -lmysqlpp \
		  -lmysqlclient

CC := g++ $(CFLAGS) $(LD) $(LDLIBS) --std=c++17

$(TGT): $(SRC)
	$(CC) $^ -o $@ -ljsoncpp

clean:
	rm -rf $(TGT) $(OBJ)

.PHONY: clean
