CFLAGS := -std=c11

bin/build-monitor: var/cache/build/build-monitor/main.o bin
	$(CC) $< -o $@

var/cache/build/build-monitor/main.o: src/build-monitor/main.c \
                                      var/cache/build/build-monitor
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	mkdir $@

var:
	mkdir $@

var/cache: var
	mkdir $@

var/cache/build: var/cache
	mkdir $@

var/cache/build/build-monitor: var/cache/build
	mkdir $@

.PHONY: clean
clean:
	rm -rf bin
	rm -rf var

