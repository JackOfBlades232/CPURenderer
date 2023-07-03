mathd.o: src/mathd.c src/mathd.h src/geom.h
camera.o: src/camera.c src/camera.h src/geom.h src/image.h \
 src/render_options.h src/mathd.h
geom.o: src/geom.c src/mathd.h src/geom.h
file_reader.o: src/file_reader.c src/file_reader.h src/scene.h src/geom.h \
 src/camera.h src/image.h src/render_options.h src/bvh_funcs.h \
 src/debug.h
raytracer.o: src/raytracer.c src/raytracer.h src/geom.h src/scene.h \
 src/camera.h src/image.h src/render_options.h src/mathd.h
scene.o: src/scene.c src/scene.h src/geom.h src/camera.h src/image.h \
 src/render_options.h src/raytracer.h src/debug.h
debug.o: src/debug.c src/debug.h src/geom.h src/scene.h src/camera.h \
 src/image.h src/render_options.h
png_save.o: src/png_save.c src/png_save.h src/image.h src/geom.h \
 src/render_options.h
image.o: src/image.c src/image.h src/geom.h src/render_options.h \
 src/mathd.h
bvh_funcs.o: src/bvh_funcs.c src/bvh_funcs.h src/scene.h src/geom.h \
 src/camera.h src/image.h src/render_options.h src/debug.h
