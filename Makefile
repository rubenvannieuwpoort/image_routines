example: example.c image.c
	gcc example.c image.c -lm -o example

.PHONY: clean
clean:
	rm -f example
