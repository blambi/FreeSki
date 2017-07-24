TARGET = freeski
SRCS = main.c graphics.c area.c lua_api.c
OBJS = $(SRCS:.c=.o)

CC = gcc
LD = gcc

CFLAGS = `pkg-config --cflags lua5.3`
LDFLAGS = -lSDL2 -lSDL2_image -lm `pkg-config --libs lua5.3`

all:$(TARGET)

# $(TARGET):$(SRCS)
# $(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(TARGET): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)
clean:
	rm $(TARGET) $(OBJS)
