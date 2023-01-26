bitmap.o: src/bitmap.c src/bitmap.h
camera.o: src/camera.c src/camera.h src/geom.h src/image.h src/bitmap.h
debug.o: src/debug.c src/debug.h src/geom.h
geom.o: src/geom.c src/mathd.h src/geom.h
image.o: src/image.c src/image.h src/bitmap.h src/geom.h
lighting.o: src/lighting.c src/lighting.h src/geom.h src/scene.h \
 src/camera.h src/image.h src/bitmap.h src/mathd.h
mathd.o: src/mathd.c src/mathd.h src/geom.h
png_save.o: src/png_save.c src/png_save.h src/bitmap.h
scene.o: src/scene.c src/scene.h src/geom.h src/camera.h src/image.h \
 src/bitmap.h src/lighting.h src/debug.h
