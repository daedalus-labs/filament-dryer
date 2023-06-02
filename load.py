#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2023 Joe Porembski

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:

# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.

# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.

# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

__author__ = 'Joe Porembski'
__copyright__ = 'Copyright (C) 2023 Joe Porembski'
__license__ = 'BSD-3-Clause'

import argparse
import subprocess
import os

PICO_TOTAL_MEMORY_BYTES : int = (256+2)*1024*1024
CONFIGURATION_DIRECTORY : str = 'build'
CONFIGURATION_FILE : str = 'build/picocfg.bin'
PICOTOOL_COMMAND : str = 'picotool'
LOAD_OPTION : str = 'load'
OFFSET_OPTION : str = '--offset'
FORCE_OPTION : str = '-f'

def writeConfigurationItem(value : str, first_write : bool = False):
    option = 'ab'
    if first_write:
        option = 'wb'

    with open(CONFIGURATION_FILE, option) as configuration_file:
        valueBytes = bytearray(value, encoding='utf-8')
        valueLength = len(value)
        configuration_file.write(valueLength.to_bytes(4, 'little'))
        configuration_file.write(valueBytes)
        print('Wrote {} ({} bytes)'.format(value, valueLength))
        
        # Adding 4 to account for the 4 byte length
        return valueLength + 4

if __name__ == '__main__':
    # First step here is to parse the arguments.
    # All arguments are setup with a default to ensure they can be naively referenced later.
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--ssid', default='', help='The Wireless SSID the device should connect to')
    parser.add_argument('-p', '--passphrase', default='', help='The passphrase of the specified Wireless SSID')
    args = parser.parse_args()
    
    if not os.path.exists(CONFIGURATION_DIRECTORY):
        os.makedirs(CONFIGURATION_DIRECTORY)

    # Second step is to create the configuration file.
    # Configuration file will be defined as a binary encoded file with the following values written in order:
    #   SSID Length (Fixed 4-byte length)
    #   SSID
    #   Passphrase Length (Fixed 4-byte length)
    #   Passphrase
    #   Total Length (Fixed 4-byte length)
    #
    # This will allow the pico code to pull the configuration length from a fixed location (last 4 bytes in memory).
    totalLength = writeConfigurationItem(args.ssid, True)
    totalLength += writeConfigurationItem(args.passphrase)
    with open(CONFIGURATION_FILE, 'ab') as configuration_file:
        configuration_file.write(totalLength.to_bytes(4, 'little'))
        totalLength += 4
    print('Created {} ({} bytes)'.format(CONFIGURATION_FILE, totalLength))

    # Final step is to use picotool to load the configuration file
    # at the very end of the flash memory of the Pico.
    offset = PICO_TOTAL_MEMORY_BYTES - totalLength
    offsetStr = ('0x{0:8x}').format(offset)
    print('Writing {} to pico @ {}'.format(CONFIGURATION_FILE, offsetStr))
    picoload_command = [
        PICOTOOL_COMMAND, LOAD_OPTION, FORCE_OPTION, CONFIGURATION_FILE, OFFSET_OPTION, offsetStr
    ]
    subprocess.call(picoload_command)
