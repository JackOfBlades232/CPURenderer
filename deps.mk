camera.o: src/camera.c src/camera.h src/geom.h src/image.h
debug.o: src/debug.c src/debug.h src/geom.h
geom.o: src/geom.c src/mathd.h src/geom.h
image.o: src/image.c src/image.h src/geom.h
raytracer.o: src/raytracer.c src/raytracer.h src/geom.h src/scene.h \
  src/camera.h src/image.h src/mathd.h
mathd.o: src/mathd.c src/mathd.h src/geom.h
scene.o: src/scene.c src/scene.h src/geom.h src/camera.h src/image.h \
<<<<<<< HEAD
 src/raytracer.h src/debug.h
file_reader.o: src/file_reader.c src/file_reader.h src/scene.h src/geom.h \
 src/camera.h src/image.h
=======
  src/raytracer.h src/debug.h
>>>>>>> 084bf20e7328227c2ffd581d2c2decfb74081cfa
