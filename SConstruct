#******************************************************************
#
# Copyright 2106 Intel Corporation All Rights Reserved.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

vars = Variables('BuildOptions.txt')
vars.Add(EnumVariable('BUILD_TYPE', 'Specify release or debug build', 'debug', ['debug','release']))
vars.Add(PathVariable('IOTIVITY_BASE', 'Location of the iotivity project', None, PathVariable.PathAccept))
vars.Add(EnumVariable('IOTIVITY_LIB_TYPE', 'Specify release or debug build', 'release', ['debug','release']))
vars.Add(EnumVariable('TARGET_ARCH', 'Target architecture', 'x86_64', ['x86_64']))
vars.Add(BoolVariable('VERBOSE', 'Show compilation', False))
#vars.Add(EnumVariable('TEST', 'Run unit tests', '0', allowed_values=('0', '1')))

env = Environment(variables = vars);
Help('''
Build options can be specified using the command line, BuildOptions.txt or using
the SCONS_FLAGS environment variable 

Example of command line release build:

   $ scons BUILD_TYPE=release IOTIVITY_BASE=/path/to/IOTIVITY_BASE

The current options are:
''')
Help(vars.GenerateHelpText(env))

if env.get('VERBOSE') == False:
    env['CCCOMSTR'] = "Compiling $TARGET"
    env['SHCCCOMSTR'] = "Compiling $TARGET"
    env['CXXCOMSTR'] = "Compiling $TARGET"
    env['SHCXXCOMSTR'] = "Compiling $TARGET"
    env['LINKCOMSTR'] = "Linking $TARGET"
    env['SHLINKCOMSTR'] = "Linking $TARGET"
    env['ARCOMSTR'] = "Archiving $TARGET"
    env['RANLIBCOMSTR'] = "Indexing Archive $TARGET"

env['CPPFLAGS'] = ['-std=c++11', '-fvisibility=hidden', '-Wall', '-fPIC']

# If the gcc version is 4.9 or newer add the diagnostics-color flag
# the adding diagnostics colors helps discover error quicker.
gccVer = env['CCVERSION'].split('.')
if int(gccVer[0]) > 4:
    env['CPPFLAGS'].append('-fdiagnostics-color');
elif int(gccVer[0]) == 4 and int(gccVer[1]) >= 9:
    env['CPPFLAGS'].append('-fdiagnostics-color');
if env['BUILD_TYPE'] == 'debug':
    env.AppendUnique(CPPFLAGS = ['-g'])
else:
    env.AppendUnique(CCFLAGS = ['-Os'])
    env.AppendUnique(CPPDEFINES = ['NDEBUG'])

iotivity_service_inc_paths = ['${IOTIVITY_BASE}/service/resource-container/include',
                  '${IOTIVITY_BASE}/service/resource-container/bundle-api/include',
                  '${IOTIVITY_BASE}/service/resource-container/src',
                  '${IOTIVITY_BASE}/service/resource-encapsulation/include']
iotivity_resource_inc_paths = ['${IOTIVITY_BASE}/resource/oc_logger/include',
                              '${IOTIVITY_BASE}/resource/csdk/stack/include',
                              '${IOTIVITY_BASE}/resource/c_common',
                              '${IOTIVITY_BASE}/resource/include',
                              '${IOTIVITY_BASE}/resource/c_common']

env['CPPPATH'] = ['${IOTIVITY_BASE}/extlibs']
env.AppendUnique(CPPPATH = iotivity_service_inc_paths)
env.AppendUnique(CPPPATH = iotivity_resource_inc_paths)
env.AppendUnique(CPPPATH = ['/usr/include/glib-2.0',
                            '/usr/lib/glib-2.0/include',
                            '/usr/lib64/glib-2.0/include',
                            '/usr/lib/x86_64-linux-gnu/glib-2.0/include/',
                            '/usr/include/gssdp-1.0',
                            '/usr/include/gssdp-1.0/libgssdp',
                            '/usr/include/gupnp-1.0',
                            '/usr/include/gupnp-1.0/libgupnp',
                            '/usr/include/libsoup-2.4',
                            '/usr/include/libsoup-2.4/libsoup',
                            '/usr/include/libxml2'])

env.AppendUnique(CPPPATH = ['#/include/'])

# currently this build script only supports linux build as we add other target OSs
# target_os will need to be replaced buy actual OSs
target_os = 'linux'

env.Replace(BUILD_DIR = 'out/' + target_os + '/${TARGET_ARCH}/${BUILD_TYPE}' )

# UPnP libraries
# oic_libs = ['oc', 'octbstack', 'oc_logger', 'rcs_container', 'rcs_client', 'rcs_server', 'rcs_common']
env['LIBS'] = ['glib-2.0', 'gobject-2.0', 'gssdp-1.0', 'gupnp-1.0', 'soup-2.4']
env['LIBPATH'] =  ['/usr/local/lib', '${IOTIVITY_BASE}/out/' + target_os + '/${TARGET_ARCH}/${IOTIVITY_LIB_TYPE}']

# Boost library
env.AppendUnique(LIBS = ['boost_regex'])
env.AppendUnique(LIBPATH = ['/usr/local/lib'])

upnpbundle_lib = env.SConscript('src/SConscript', variant_dir = env['BUILD_DIR'] + '/obj/bridge', exports= 'env', duplicate=0)
env.Install('${BUILD_DIR}/libs', upnpbundle_lib)
# currenlty we install the library in libs and bin since the xml file assumes
# the library is in the same location as the bin file.
env.Install('${BUILD_DIR}/bin', upnpbundle_lib)

# build examples
env.SConscript('examples/SConscript', variant_dir = env['BUILD_DIR'] + '/obj/examples', exports= ['env', 'iotivity_resource_inc_paths'], duplicate=0)

#build unit tests
env.SConscript('unittests/SConscript', variant_dir = env['BUILD_DIR'] + '/obj/unittests', exports=['env', 'upnpbundle_lib'], duplicate=0)
