# Scons construction file for Job Dispatcher #

# Help Options
Help("""
Type: 'scons' to build the production job dispatcher.
      'scons --DEBUG' to build the debug version.  
""")

# Command Line Options
AddOption('--DEBUG')
AddOption('--gcc_dir')

# Imports
import os
import atexit
import SCons.Errors

# Setup Build Status Reporting on failure
def bf_to_str(bf):
    """Convert an element of GetBuildFailures() to a string
    in a useful way."""
    if bf is None: # unknown targets product None in list
        return '(unknown tgt)'
    elif isinstance(bf, SCons.Errors.StopError):
        return str(bf)
    elif bf.node:
        return str(bf.node) + ': ' + bf.errstr
    elif bf.filename:
        return bf.filename + ': ' + bf.errstr
    return 'unknown failure: ' + bf.errstr

def build_status():
    """Convert the build status to a 2-tuple, (status, msg)."""
    from SCons.Script import GetBuildFailures
    bf = GetBuildFailures()
    if bf:
        # bf is normally a list of build failures; if an element is None,
        # it's because of a target that scons doesn't know anything about.
        status = 'failed'
        failures_message = "\n".join(["Failed building %s" % bf_to_str(x)
                        for x in bf if x is not None])
    else:
        # if bf is None, the build completed successfully.
        status = 'ok'
        failures_message = ''
    return (status, failures_message)
    
def display_build_status():
    """Display the build status. Called by atexit.
    Here you could do all kinds of complicated things."""
    status, failures_message = build_status()
    if status == 'failed':
        print "FAILED!!!!" # could display alert, ring bell, etc.
    elif status == 'ok':
        print "Build succeeded."
    print failures_message

# Register the display build status
atexit.register(display_build_status)

# Read Command Line Options and create the correct environments for the
# SConscript to use by exporting it

# Call SConscripts to create the library
if not GetOption('help'):
    if not GetOption('DEBUG'):
        SConscript('SConscript', variant_dir='build/scons/release', src_dir='.', duplicate=0)
    else:
        SConscript('SConscript', variant_dir='build/scons/debug', src_dir='.', duplicate=0)
