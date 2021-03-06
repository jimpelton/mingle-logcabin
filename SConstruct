import sys
import os

opts = Variables('Local.sc')

opts.AddVariables(
    ("CC", "C Compiler"),
    ("CPPPATH", "The list of directories that the C preprocessor "
                "will search for include directories", []),
    ("CXX", "C++ Compiler"),
    ("CXXFLAGS", "Options that are passed to the C++ compiler", []),
    ("AS", "Assembler"),
    ("LINK", "Linker"),
    ("LIBEVENT2PATH", "libevent-2.0 library path (if necessary).", ""),
    ("BUILDTYPE", "Build type (RELEASE or DEBUG)", "DEBUG"),
    ("VERBOSE", "Show full build information (0 or 1)", "0"),
    ("NUMCPUS", "Number of CPUs to use for build (0 means auto).", "0"),
)

env = Environment(options = opts,
                  tools = ['default', 'protoc'],
                  ENV = os.environ)
Help(opts.GenerateHelpText(env))

env.Prepend(CXXFLAGS = [
    "-std=c++0x",
    "-Wall",
    "-Wextra",
    "-Wcast-align",
    "-Wcast-qual",
    "-Wconversion",
    "-Weffc++",
    "-Wformat=2",
    "-Wmissing-format-attribute",
    "-Wno-non-template-friend",
    "-Wno-unused-parameter",
    "-Woverloaded-virtual",
    "-Wwrite-strings",
    ])

if env["BUILDTYPE"] == "DEBUG":
    env.Append(CPPFLAGS = [ "-g", "-DDEBUG" ])
elif env["BUILDTYPE"] == "RELEASE":
    env.Append(CPPFLAGS = "-DNDEBUG")
else:
    print "Error BUILDTYPE must be RELEASE or DEBUG"
    sys.exit(-1)

if env["VERBOSE"] == "0":
    env["CCCOMSTR"] = "Compiling $SOURCE"
    env["CXXCOMSTR"] = "Compiling $SOURCE"
    env["SHCCCOMSTR"] = "Compiling $SOURCE"
    env["SHCXXCOMSTR"] = "Compiling $SOURCE"
    env["ARCOMSTR"] = "Creating library $TARGET"
    env["LINKCOMSTR"] = "Linking $TARGET"

if env["LIBEVENT2PATH"] != "":
    env.Append(LIBPATH = [env["LIBEVENT2PATH"]])

env.Append(CPPPATH = '#')

# Define protocol buffers builder to simplify SConstruct files
def Protobuf(env, source):
    # First build the proto file
    cc = env.Protoc(os.path.splitext(source)[0] + '.pb.cc',
                    source,
                    PROTOCPROTOPATH = ["."],
                    PROTOCPYTHONOUTDIR = None,
                    PROTOCOUTDIR = ".")[1]
    # Then build the resulting C++ file with no warnings
    return env.StaticObject(cc,
                            CXXFLAGS = "-std=c++0x")
env.AddMethod(Protobuf)

def GetNumCPUs():
    if env["NUMCPUS"] != "0":
        return int(env["NUMCPUS"])
    if os.sysconf_names.has_key("SC_NPROCESSORS_ONLN"):
        cpus = os.sysconf("SC_NPROCESSORS_ONLN")
        if isinstance(cpus, int) and cpus > 0:
            return 2*cpus
        else:
            return 2
    return 2*int(os.popen2("sysctl -n hw.ncpu")[1].read())

env.SetOption('num_jobs', GetNumCPUs())

object_files = {}
Export('object_files')

Export('env')
SConscript('Core/SConscript', variant_dir='build/Core')
SConscript('Event/SConscript', variant_dir='build/Event')
SConscript('RPC/SConscript', variant_dir='build/RPC')
SConscript('Protocol/SConscript', variant_dir='build/Protocol')
SConscript('Client/SConscript', variant_dir='build/Client')
SConscript('Storage/SConscript', variant_dir='build/Storage')
SConscript('Server/SConscript', variant_dir='build/Server')
SConscript('test/SConscript', variant_dir='build/test')
SConscript('Examples/SConscript', variant_dir='build/Examples')

# This function is taken from http://www.scons.org/wiki/PhonyTargets
def PhonyTargets(env = None, **kw):
    if not env: env = DefaultEnvironment()
    for target,action in kw.items():
        env.AlwaysBuild(env.Alias(target, [], action))

PhonyTargets(check = "scripts/cpplint.py")
PhonyTargets(lint = "scripts/cpplint.py")
PhonyTargets(doc = "doxygen docs/Doxyfile")
PhonyTargets(docs = "doxygen docs/Doxyfile")
PhonyTargets(tags = "ctags -R --exclude=build --exclude=docs .")

env.StaticLibrary("build/logcabin",
                  (object_files['Client'] +
                   object_files['Protocol'] +
                   object_files['RPC'] +
                   object_files['Event'] +
                   object_files['Core']))

env.Program("build/LogCabin",
            (["build/Server/Main.cc"] +
             object_files['Server'] +
             object_files['Storage'] +
             object_files['Protocol'] +
             object_files['RPC'] +
             object_files['Event'] +
             object_files['Core']),
            LIBS = [ "pthread", "protobuf", "rt", "cryptopp",
                     "event_core", "event_pthreads" ])
