project ('sprint', 'cpp',
    version : '1.0.0',
    default_options : [
        'buildtype=debug',
        'optimization=0',
        'cpp_std=c++23'
    ],
    meson_version: '>= 0.60.0',
)

srcs = [
    'main.cpp',
    'option.cpp',
    'printer.cpp',
    'profilestore.cpp'
]

cups_inc = include_directories ('/home/steve/OpenSource/libcups/cups')
#cups_lib = static_library ('libcups3.a')

cpp = meson.get_compiler ('cpp')

deps = [
#    dependency ('cups'),
    declare_dependency (include_directories: cups_inc),
    dependency ('yaml-cpp'),
    cpp.find_library ('cups3', dirs: ['/home/steve/OpenSource/libcups/cups']),
    cpp.find_library ('xdgbasedir')   ### don't have package for this, self-compiled
]

executable ('sprint',
    sources: srcs,
    dependencies: deps
)

