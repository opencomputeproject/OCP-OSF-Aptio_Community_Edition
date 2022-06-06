#  @file
#  Build Script for JadePlatform 
#  Copyright (c) 2022, American Megatrends International LLC.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

import subprocess
import os
import sys
import argparse
import yaml
import shutil

"""
 This function executes a given command in shell enviroinment
"""
def ExecuteCommand (Command, Build_Env={}):
                                                          
  with subprocess.Popen(Command, shell=True, env=Build_Env) as process:
    try:
        stdout, stderr = process.communicate()
    except: 
        print('An error occured while executing command :'+ Command )
        raise
                
    finally:
        if process.poll() is None:
            process.kill()
            stdout, stderr = process.communicate()


"""
 This function retrives the build settings from the YML file
"""
def Get_confgs():
  configs = {}
  current_work_space_path = os.getcwd()
  
  
  Yamlfilepath = current_work_space_path+'/Jade/Config.yml'
  with open(Yamlfilepath) as file:
    configs = yaml.full_load(file)
  
  configs['WorkSpace'] = current_work_space_path
  #print(configs)
  return configs 
  
"""
 This function creates the enviroinment variables
"""
def Set_Configs (configs):
  Build_env = os.environ.copy()
  Build_env["CROSS_COMPILE"] = configs["CROSS_COMPILE"]
  return Build_env

"""
 This function validates the input parameters to this python program
"""
def ValidateArgs(args):
    args.buildmode = args.buildmode.upper()
    if not args.buildmode in {'RELEASE','DEBUG'}:
      print(" ..Invalid build mode")
      return 0

"""
 Prepares source 
"""
     
def PrepareSource():
  current_work_space_path = os.getcwd()
  
  print("  Copying AtfTools..")
  sourcepath = current_work_space_path +'/Jade/atf-tools/'
  Destpath = current_work_space_path +'/edk2-ampere-tools/toolchain/atf-tools'
  shutil.copytree(sourcepath, Destpath, dirs_exist_ok=True)

  ChangeToolMode = 'chmod 777 ' + Destpath +'/*'
  ExecuteCommand (ChangeToolMode)
  print("  Done.") 
  
  print("  Copying Platforms Override..")
  sourcepath = current_work_space_path +'/Jade/Overrides/edk2-platforms/Platform/'
  Destpath = current_work_space_path +'/edk2-platforms/Platform/'
  shutil.copytree(sourcepath, Destpath, dirs_exist_ok=True)
  print("  Done.") 


"""
 This functions invokes the build scripts and builds the image 
"""

def BuildImage (Configs, Build_env, Args):

  Full_Image_Params = ''
  
  #if UEFI_BUILD_ONLY is not set then pass -atf-image  param to build command
  if Configs['UEFI_BUILD_ONLY'] == False :
  	Full_Image_Params = Full_Image_Params + '--atf-image {ATF_PATH}'.format ( ATF_PATH=Configs['ATF_PATH'])
  
  #if GEN_SCP_CAPSULES is set then pass --scp-image param to build command. Else, although still capsules are generated, but it is not valid capsules.
  if Configs['GEN_SCP_CAPSULES'] == True :
  	Full_Image_Params = Full_Image_Params + ' --scp-image {SCP_PATH}'.format ( SCP_PATH=Configs['SCP_PATH'])
  	
  #if UEFI_BUILD_ONLY is set then do not pass --atf-image and --scp-image  param to build command
  if Configs['UEFI_BUILD_ONLY'] == True :
    Full_Image_Params = ''

  buildcmd = '{WORKSPACE}/edk2-ampere-tools/edk2-build.sh -v -b {BUILD_MODE} Jade {FULL_IMAGE_BUILD_PARAMS} '.format (
                                                            WORKSPACE=Configs['WorkSpace'],
                                                            BUILD_MODE= Args.buildmode.upper(), 
                                                            FULL_IMAGE_BUILD_PARAMS= Full_Image_Params,
                                                            )
  #print(buildcmd)
  ExecuteCommand(buildcmd, Build_env)

"""
 Removes the builds files and resets the edk2-platforms directory back to orginal setting 
"""

def CleanSource():
  current_work_space_path = os.getcwd()
  
  print("  Removing build directories..")
  BuildPath = current_work_space_path +'/Build'
  if os.path.exists(BuildPath):
    shutil.rmtree(BuildPath)
  
  BuildPath = current_work_space_path +'/BUILDS'
  if os.path.exists(BuildPath):
    shutil.rmtree(BuildPath)
    
  GitCleanCommand = 'cd '+ current_work_space_path+'/edk2-platforms && git reset --hard && git clean -fd'
  ExecuteCommand (GitCleanCommand)

  BaseToolsCleanCommand = 'cd '+current_work_space_path+'/edk2 && make -C BaseTools clean'
  ExecuteCommand (BaseToolsCleanCommand)

  print(" Done.")

def main():
  parser = argparse.ArgumentParser(description='A Scirpt to build Aptio OE source')
  parser.add_argument('-c','--clean', help='clean the source', action='store_true')
  parser.add_argument('-b','--buildmode', help='Build Mode : DEBUG,RELEASE', type=str, default='RELEASE')
  args=parser.parse_args ()

  retvalue = ValidateArgs(args)
  if (retvalue == 0):
   print ("...Invalid arguments. Exiting Program")
   sys.exit(1)

  if args.clean  == True :
   CleanSource();
   sys.exit(1)

  PrepareSource ();
  BuildConfigs = Get_confgs ()
  BuildEnv = Set_Configs (BuildConfigs)  
  BuildImage (BuildConfigs, BuildEnv, args)

if __name__ == '__main__':
    main()

