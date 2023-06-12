SrcFiles= main.cpp
ObjectFiles=$(patsubst %.c,%.o,$(SrcFiles))

app: $(ObjectFiles)
	g++ -o app -I ./include $(ObjectFiles) -lmpg123 -lopus -lao

%.o:%.c
	g++ -c $< -I ./include


clean:
	rm -f *.o
	rm -f app
