headers = [
	'include/lith/assets.h',
	'include/lith/audio.h',
	'include/lith/buffer.h',
	'include/lith/bytes.h',
	'include/lith/capsule.h',
	'include/lith/clock.h',
	'include/lith/color.h',
	'include/lith/context.h',
	'include/lith/event.h',
	'include/lith/font.h',
	'include/lith/icosphere.h',
	'include/lith/index.h',
	'include/lith/input.h',
	'include/lith/interpolation.h',
	'include/lith/io.h',
	'include/lith/job.h',
	'include/lith/lens.h',
	'include/lith/line.h',
	'include/lith/log.h',
	'include/lith/math.h',
	'include/lith/mesh.h',
	'include/lith/plane.h',
	'include/lith/plugin.h',
	'include/lith/quad.h',
	'include/lith/random.h',
	'include/lith/rect.h',
	'include/lith/render.h',
	'include/lith/shader.h',
	'include/lith/sketch.h',
	'include/lith/sketchapi.h',
	'include/lith/sprite.h',
	'include/lith/string.h',
	'include/lith/target.h',
	'include/lith/text.h',
	'include/lith/texture.h',
	'include/lith/timer.h',
	'include/lith/typedef.h',
	'include/lith/ui.h',
	'include/lith/uvsphere.h',
	'include/lith/window.h',
	'include/lith/zip.h',

	# external files

	'include/lith/cr.h'
]

sources = [
	'src/assets.cpp',
	'src/audio.cpp',
	'src/buffer.cpp',
	'src/bytes.cpp',
	'src/capsule.cpp',
	'src/clock.cpp',
	'src/font.cpp',
	'src/icosphere.cpp',
	'src/index.cpp',
	'src/input.cpp',
	'src/interpolation.cpp',
	'src/io.cpp',
	'src/job.cpp',
	'src/lens.cpp',
	'src/line.cpp',
	'src/log.cpp',
	'src/math.cpp',
	'src/mesh.cpp',
	'src/plane.cpp',
	'src/quad.cpp',
	'src/random.cpp',
	'src/rect.cpp',
	'src/render.cpp',
	'src/shader.cpp',
	'src/sketchapi.cpp',
	'src/sprite.cpp',
	'src/string.cpp',
	'src/target.cpp',
	'src/text.cpp',
	'src/texture.cpp',
	'src/timer.cpp',
	'src/ui.cpp',
	'src/uvsphere.cpp',

	# external files

	'src/glad.c'
]

deps = [
	dependency('glm'),
	dependency('fmt')
]

include = include_directories(['include', 'src'])

target = static_library('lith', headers + sources, include_directories: include, dependencies: deps, install: true)

framework_dep = declare_dependency(include_directories: include, link_with: target)

install_headers(headers, subdir: 'lith')

pkg = import('pkgconfig')
pkg.generate(target)