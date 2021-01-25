run: build
	./main_crcw; ./main_crew

build: crew crcw

crew: temalab1_SM_CREW.c
	gcc temalab1_SM_CREW.c -o main_crew -pthread

crcw: temalab1_SM_CREW.c
	gcc temalab1_SM_CRCW.c -o main_crcw -pthread -Wno-discarded-qualifiers

clean:
	rm main_crcw main_crew