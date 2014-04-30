# Prepare to create the Cara Emotion (Server)

# Imports
import os

# Define the Environment Variables
if GetOption('DEBUG'):
    print 'Debug mode'
    cc_flags = '-Wall -fexceptions -g -std=c++11'
else:
    cc_flags = '-Wall -fexceptions -O2 -std=c++11'

# Define Sources and Includes
include_path = Split("""
            include
            /usr/include
            /usr/local/include 
            """)

src_files = Split("""
                src/INotifyEvent.cpp
                src/INotifyEventPoller.cpp
                src/INotifyWatch.cpp            
                src/main.cpp
                """)                

source_files = src_files

print '\nSource Listing'
print source_files

# Define Library Paths
lib_path = Split("""
            /usr/local/lib         
            """)
            
# Define Library Dependencies
libs = Split("""
        boost_system.so
        boost_thread.so
        boost_program_options.so
        boost_random.so 
        boost_atomic.so  
        boost_chrono.so
        boost_iostreams.so
        boost_filesystem.so
        """)
            
# Construct the Construction Environment
env = Environment()

path = os.environ['PATH']

# If user defined a gcc directory
if GetOption('gcc_dir') :
    gcc_dir = GetOption('gcc_dir')
    print 'GCC directory: %s' % gcc_dir
    env.Replace(CXX=gcc_dir+'/g++')
    env.Replace(CC=gcc_dir+'/gcc')
    path = gcc_dir+':'+os.environ['PATH'] #append infront

env.Append(CCFLAGS=cc_flags) 
env.Append(CPPPATH=include_path)
env.Append(LIBPATH=lib_path)
env.Append(LIBS=libs)
env.Append(PATH=path)

# Execution Environment settings, none right now

print 'Scons Construction Environment'
print 'OS:', env['PLATFORM']
print 'Compiler: ', env['CXX']
print 'Compile Flags: ', env['CCFLAGS']
print 'Include Path: ', env['CPPPATH']
print 'Library Path: ', env['LIBPATH']
print 'Libraries: ', env['LIBS']
print 'Path: ', env['PATH']

# Build Program
env.Program('cara-emotion', source_files)
