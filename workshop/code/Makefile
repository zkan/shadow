CC = g++

EXPENDABLES = get_data_under_mask shadow_ml shadow_ncc shadow_hsv

CPPFLAGS = -gstabs+

INC = `pkg-config opencv --cflags`
LIB = `pkg-config opencv --libs`

all: $(EXPENDABLES)

shadow_utils.o: shadow_utils.cc shadow_utils.h
	$(CC) $(CPPFLAGS) $(INC) -c shadow_utils.cc
	
get_data_under_mask: get_data_under_mask.cc shadow_utils.o
	$(CC) $(CPPFLAGS) $(INC) get_data_under_mask.cc shadow_utils.o $(LIB) -o get_data_under_mask

shadow_ncc: shadow_ncc.cc shadow_utils.o
	$(CC) $(CPPFLAGS) $(INC) shadow_ncc.cc shadow_utils.o $(LIB) -o shadow_ncc

shadow_hsv: shadow_hsv.cc shadow_utils.o
	$(CC) $(CPPFLAGS) $(INC) shadow_hsv.cc shadow_utils.o $(LIB) -o shadow_hsv

shadow_ml: shadow_ml.cc shadow_utils.o
	$(CC) $(CPPFLAGS) $(INC) shadow_ml.cc shadow_utils.o $(LIB) -o shadow_ml

clean:
	rm -f *.o $(EXPENDABLES)




