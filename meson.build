project('lith', 'c', 'cpp', default_options: ['default_library=static', 'cpp_std=c++20'], version: '0.1')

subdir('dep/msdf-atlas-gen')

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
	'include/lith/zip.h'
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

	'src/glad.c',

	# runtime

	'src/runtime/main.cpp',
	'src/runtime/printfLogger.cpp',
	'src/runtime/msdfgenFontGenerator.cpp',
	'src/runtime/Project.cpp',
	'src/runtime/SDLMixerAudioBackend.cpp',
	'src/runtime/SDLWindow.cpp',
	'src/runtime/SketchRenderBackend.cpp',


	# game

	'src/game/sketch.cpp',
	'src/game/BulletRenderer.cpp',
	'src/game/entity.cpp',
	'src/game/global.cpp',
	'src/game/level.cpp',

	'src/game/entity/Bullet.cpp',
	'src/game/entity/DeathDome.cpp',
	'src/game/entity/Door.cpp',
	'src/game/entity/Enemy.cpp',
	'src/game/entity/Player.cpp',
	'src/game/entity/Text.cpp',
	'src/game/entity/Wall.cpp',

	'src/game/content/levels.cpp',
]

deps = [
	msdf_atlas_gen_dep,
	msdfgen_dep,
	dependency('glm'),
	dependency('fmt'),
	dependency('sdl2', default_options: { 'test': false }),
	dependency('sdl2_mixer'),
	
	dependency('box2d')
]

include = include_directories(['include', 'src', 'src/game'])
args = ['-DSDL_MAIN_HANDLED=1'] + msdf_atlas_args

executable('lith', headers + sources, include_directories: include, dependencies: deps, cpp_args: args)