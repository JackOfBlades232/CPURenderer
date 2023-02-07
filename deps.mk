camera.o: src/camera.c src/camera.h src/geom.h src/image.h
debug.o: src/debug.c src/debug.h src/geom.h
geom.o: src/geom.c src/mathd.h src/geom.h
image.o: src/image.c src/image.h src/geom.h
raytracer.o: src/raytracer.c src/raytracer.h src/geom.h src/scene.h \
 src/camera.h src/image.h src/mathd.h
mathd.o: src/mathd.c src/mathd.h src/geom.h
png_save.o: src/png_save.c src/png_save.h src/image.h src/geom.h
scene.o: src/scene.c src/scene.h src/geom.h src/camera.h src/image.h \
 src/raytracer.h src/debug.h
scene_reader.o: src/scene_reader.c src/scene_reader.h src/scene.c \
 src/scene.h src/geom.h src/camera.h src/image.h src/raytracer.h \
 src/debug.h src/reader_utils.h
reader_utils.o: src/reader_utils.c src/reader_utils.h
