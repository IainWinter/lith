headers = [
	'src/printfLogger.h',
	'src/msdfgenFontGenerator.h',
	'src/Project.h',
	'src/SDLMixerAudioBackend.h',
	'src/SDLWindow.h',
	'src/SketchPlugin.h',
	'src/SketchRenderBackend.h',
]

sources = [
	'src/main.cpp',
	'src/printfLogger.cpp',
	'src/msdfgenFontGenerator.cpp',
	'src/Project.cpp',
	'src/SDLMixerAudioBackend.cpp',
	'src/SDLWindow.cpp',
	'src/SketchPlugin.cpp',
	'src/SketchRenderBackend.cpp'
]

deps = [
	framework_dep,
	msdf_atlas_gen_dep,
	msdfgen_dep,
	dependency('glm'),
	dependency('fmt'),
	dependency('sdl2', default_options: { 'test': false }),
	dependency('sdl2_mixer')
]

args = ['-DSDL_MAIN_HANDLED=1'] + msdf_atlas_args

include = include_directories(['src'])

executable('lith', headers + sources, include_directories: include, dependencies: deps, cpp_args: args, install: true)