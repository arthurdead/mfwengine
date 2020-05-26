import os, sys, argparse

parser = argparse.ArgumentParser()
parser.add_argument("--debug", action="store_true", required=False, default=False, dest="debug")
parser.add_argument("--wgl", action="store_true", required=False, default=False, dest="wgl")
parser.add_argument("--vulkan", action="store_true", required=False, default=False, dest="vulkan")
parser.add_argument("--gladdir", action="store", required=True, dest="gladdir")
parser.add_argument("--outdir", action="store", required=True, dest="outdir")
args = parser.parse_args()

extensions = []

if args.wgl:
	extensions += [
		"WGL_ARB_create_context",
		"WGL_ARB_create_context_profile",

		"WGL_ARB_pixel_format",
		"WGL_EXT_pixel_format",

		"WGL_ARB_extensions_string",
		"WGL_EXT_extensions_string",
	]

	if not args.debug:
		extensions += [
			"WGL_ARB_create_context_no_error",
		]
elif args.vulkan:
	pass
else:
	extensions += [
		"GL_ARB_program_interface_query",

		"GL_ARB_get_program_binary",
		"GL_MESA_program_binary_formats",
		"GL_EXT_compiled_vertex_array",

		"GL_ARB_shader_precision",
		"GL_OES_single_precision",

		"GL_ARB_explicit_attrib_location",
		"GL_ARB_explicit_uniform_location",

		"GL_SGIX_async",
		"GL_ARB_parallel_shader_compile",
		"GL_KHR_parallel_shader_compile",
		"GL_INTEL_parallel_arrays",

		"GL_ARB_shading_language_include",
		"GL_ARB_gl_spirv",
		"GL_ARB_spirv_extensions",
	]

	if args.debug:
		extensions += [
			"GL_KHR_debug",

			"GL_ARB_debug_output",
			"GL_AMD_debug_output",

			"GL_EXT_debug_label",
			"GL_EXT_debug_marker",
		]
	else:
		extensions += [
			"GL_KHR_no_error",
		]

extensions_string = ""
for ext in extensions:
	extensions_string += ext + ","
extensions_string = extensions_string[:-1]

glad2 = True

out_path = args.outdir + "\\"+ ("debug" if (args.debug == True) else "release")
os.makedirs(out_path, exist_ok=True)
os.chdir(args.gladdir)
command ="\"" + sys.executable + "\" -m glad --quiet --out-path=\"" + out_path + "\" "

if not glad2:
	profile = ""
	spec = ""
	if args.wgl:
		spec += "wgl"
	elif args.vulkan:
		spec += "vulkan"
	else:
		profile += "core"
		spec += "gl"

	if profile:
		command += "--profile=\"" + profile + "\""

	api = spec + "="

	command += "--extensions=\""+extensions_string+"\" --spec=\""+spec+"\" --api=\""+api+"=\" --generator=\""+ ("c-debug" if ((args.debug == True) and (args.wgl == False)) else "c") + "\""
else:
	ext_file = out_path + "\\extensions.txt"
	with open(ext_file, "w+") as file:
		file.write(extensions_string)

	api = ""
	if args.wgl:
		api += "wgl="
	elif args.vulkan:
		api += "vulkan="
	else:
		api += "gl:core="

	options = "--loader "
	if args.debug:
		options += "--debug "
	else:
		options += "--mx --mx-global "

	command += "--extensions=\"" + ext_file + "\" --api=\"" + api + "\" c " + options

#print(command)
os.system("\"" + command + "\"")