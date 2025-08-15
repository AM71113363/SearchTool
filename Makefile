# Project: SearchTool

CC   = gcc.exe -s
WINDRES = windres.exe
RES  = obj/main.res
OBJ  = obj/main.o obj/statusbar.o obj/CountAndSet.o obj/LoadDataFile.o obj/search.o $(RES)
LINKOBJ  = obj/main.o obj/statusbar.o obj/CountAndSet.o obj/LoadDataFile.o obj/search.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -mwindows -lcomctl32  
INCS =  -I"C:/Dev-Cpp/include" 
BIN  = SearchTool.exe
CFLAGS = $(INCS)  
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before SearchTool.exe all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o "SearchTool.exe" $(LIBS)

obj/main.o: main.c
	$(CC) -c main.c -o obj/main.o $(CFLAGS)

obj/statusbar.o: statusbar.c
	$(CC) -c statusbar.c -o obj/statusbar.o $(CFLAGS)

obj/CountAndSet.o: CountAndSet.c
	$(CC) -c CountAndSet.c -o obj/CountAndSet.o $(CFLAGS)

obj/LoadDataFile.o: LoadDataFile.c
	$(CC) -c LoadDataFile.c -o obj/LoadDataFile.o $(CFLAGS)

obj/search.o: search.c
	$(CC) -c search.c -o obj/search.o $(CFLAGS)

obj/main.res: main.rc 
	$(WINDRES) -i main.rc --input-format=rc -o obj/main.res -O coff 
