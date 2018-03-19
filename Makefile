include makepath

C = g++
INC = -I. -I$(INC_QT) $(_I_INC_QT)
LIB_PATH = -L$(LIB_QT)
CFLAGS = -g -w -Wall $(INC) -ggdb -O1
LIBS = -lgcc -fopenmp -lQtCore$(SUFFIX_QT)
NAME = ./warden

OBJ = w_process.o w_data.o w_data.moc.o w.o w.moc.o main.o

.SUFFIXES: .cpp .o

.cpp.o:
	$(C) $(CFLAGS) -c -o $@ $<

all: $(NAME)

w.moc.cpp: w.o.h
	$(MOC) w.h -o w.moc.cpp 

w_data.moc.cpp: w_data.h
	$(MOC) w_data.h -o w_data.moc.cpp 
	
$(NAME): $(OBJ)
	$(C) $(CFLAGS) $(LIB_PATH) -o $(NAME) $(OBJ) $(LIBS)

clean:
	rm -rf $(OBJ) $(NAME)

# EOF
