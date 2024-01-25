 # *****************************************************************************
 # *
 # * Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
 # *
 # ******************************************************************************
import os
import sys
#Module Data Packing
import struct
#Module for arguments parsing
import argparse
#Module for logging,
import logging
#Module for XML parsing
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Element, SubElement, Comment, tostring
from xml.dom import minidom
import types
import pdb
import traceback
import copy

Version = "4.0.4E"

#Static Value
ROMSIG_SIG_OFFSET = 0
ROMSIG_PSPDIR_OFFSET = 5
ROMSIG_BIOSDIR_OFFSET = 6
SECOND_GEN_EFS_OFFSET = 9
ROMSIG_PSPDIR_RECOVERY_OFFSET = 11
PSP_COMBO_SIGNATURE = 0x50535032 #2PSP
PSP_DIR_SIGNATURE = 0x50535024 #$PSP
PSP_LEVEL2_DIR_SIGNATURE = 0x324C5024 #$PL2
BIOS_COMBO_SIGNATURE = 0x44484232 #2BHD
BIOS_DIR_SIGNATURE = 0x44484224 #$BHD
BIOS_LEVEL2_DIR_SIGNATURE = 0x324C4224 #$BL2
#Global Variable
gAlignment= 0x100
DirHeaderSize = 0x400   #Always reserve 0x400 for Dir Header
OutputPath='/'
AddressMode = 0
Writable = 0
ProgramStr = 'None'
PSP_ENTRY_TYPE_LEVEL2_DIR = 0x40
PSP_ENTRY_TYPE_LEVEL2A_DIR = 0x48
PSP_ENTRY_TYPE_LEVEL2B_DIR = 0x4A
BIOS_ENTRY_TYPE_LEVEL2AB_DIR = 0x49
BIOS_ENTRY_TYPE_LEVEL2_DIR = 0x70
gXorBit24 = 0
IsABRecovery = 0
PspDirectoryEntryName = {
#                         Type:   DIR, Entry Type    ShortName    Full description  Modifiable
                          0x00: ['PSP' ,'IMAGE_ENTRY','AmdPubKey','AMD public Key', 0],
                          0x01: ['PSP' ,'IMAGE_ENTRY','PspBootLoader','PSP Boot Loader firmware', 0],
                          0x03: ['PSP' ,'IMAGE_ENTRY','PspRecBL','PSP Recovery Boot Loader', 0],
                          0x08: ['PSP' ,'IMAGE_ENTRY','Smu','SMU offchip firmware', 0],
                          0x09: ['PSP' ,'IMAGE_ENTRY','DbgKey','AMD Secure Debug Key', 0],
                          0x0A: ['PSP' ,'IMAGE_ENTRY','OemAblKey','OEM ABL public key signed with AMD key', 0],
                          0x12: ['PSP' ,'IMAGE_ENTRY','Smu2','SMU offchip firmware 2', 0],
                          0x13: ['PSP' ,'IMAGE_ENTRY','PspEarlyUnlock','PSP early secure unlock debug image', 0],
                          0x21: ['PSP' ,'IMAGE_ENTRY','ikek', 'Wrapped iKEK', 0],
                          0x22: ['PSP' ,'IMAGE_ENTRY','PspTokenUnlockData', 'PSP token unlock data', 1],
                          0x23: ['PSP' ,'IMAGE_ENTRY','PspDiagBL', 'Entry to load PSP Diag BL on non-secure part via fuse', 0],
                          0x24: ['PSP' ,'IMAGE_ENTRY','RegisterAccessPolicy', 'Register Access Policy', 0],
                          0x2A: ['PSP' ,'IMAGE_ENTRY','Mp5Fw', 'Location field pointing to MP5 FW', 0],
                          0x30: ['PSP' ,'IMAGE_ENTRY','PspAgesaBL0', 'PSP AGESA Binary 0', 0],
                          0x39: ['PSP' ,'IMAGE_ENTRY','SevCode', 'Trusted application implementing SEV functionality', 0],
                          0x42: ['PSP' ,'IMAGE_ENTRY','DxioFw', 'DXIO Phy FW binary', 0],
                          0x44: ['PSP' ,'IMAGE_ENTRY','UsbPhyFw', 'USB unified PHY FW', 0],
                          0x47: ['PSP' ,'IMAGE_ENTRY','DrtmTa', 'DRTM TA', 0],
                          0x50: ['PSP' ,'IMAGE_ENTRY','PspBlPubKey', 'Public keys are needed to certify firmware components and data. Public keys are stored in SPI ROM and are loaded into SRAM. This entry point to the Public keys Table for PSP BL', 0],
                          0x55: ['PSP' ,'IMAGE_ENTRY','SPL Table', 'Table of SPL values used by bootloader for FW anti-rollback', 0],
                          0x5D: ['PSP' ,'IMAGE_ENTRY','MPIOOffchipFW', 'MPIO Offchip Firmware, responsible for xGMI, WAFL, PCIe, etc. training', 0],
                          0x76: ['PSP' ,'IMAGE_ENTRY','RIB', 'Register Initialization Binary (RIB)', 0],
                          0x8B: ['PSP','IMAGE_ENTRY','FipsCert', 'FIPS certification dedicated module used for FIPS certification of PSP', 0],
                          0x8C: ['PSP','IMAGE_ENTRY','MpdmaTfFw', 'Firmware for MPDMA TF instances', 0],
                          0x91: ['PSP','IMAGE_ENTRY','Gmi3PhyFw', 'Delivered by MPIO team. Validated with PSP Entry Type 0x43', 0],
                          0x92: ['PSP','IMAGE_ENTRY','MpdmaPmFw', 'Firmware for MPDMA Page Migration instances', 0],
                                 }

BiosDirectoryEntryName = {
#                         Type:   DIR, Entry Type    ShortName    Full description  Modifiable
                          0x05: ['BIOS','IMAGE_ENTRY','OemPubKey','Location field points to the public part of the OEM/IBV BIOS Signing Key Token or BIOS Signing Sub-CA Key Token when OEM uses a BIOS Signing Sub-CA key hierarchy.', 0],
                          0x06: ['BIOS','IMAGE_ENTRY','OemPubKey2','Location filed points to the public part of the OEM/IBV BIOS Signing Key Token when OEM uses a BIOS Signing Sub-CA key hierarchy otherwise left as reserved', 0],
                          0x63: ['BIOS','IMAGE_ENTRY','ApobNv','APOB NV Copy', 1],
                          0x66: ['BIOS','IMAGE_ENTRY','UCodePatch','Microcode Patch', 0],
                          0x68: ['BIOS','IMAGE_ENTRY','ApcbRec','Location field points to the backup copy of APCB data', 0],
                          0x69: ['BIOS','IMAGE_ENTRY','EarlyVgaImage','Location field pointing to the interpreter binary that displays the video image', 0],
                          0x70: ['BIOS','POINT_ENTRY','BIOSDirLv2', 'Point to BIOS level 2 directory', 0],
                                 }

ProgramTable = {
  'Genoa': {"AndMask":0xFFFFFFFF,"PSPID": 0xBC0D0111, "BIOSDIR_OFFSET": 0xA, "SECOND_GEN_EFS": 0, "ISH_STRUCTURE": 0},
}

version_info_dir = {
  # SMU
  ('psp', 0x08): {'function': 'version_smu_from_dir_Entry'},
  ('psp', 0x12): {'redirect': ('psp', 0x08)},
  'smu': {'header_size': 0x100, 'version_offset': 0x60, 'unpack_format': 'BBBB', 'print_format': "{3:d}.{2:d}.{1:d}.{0:d}"},
  # KVM
  ('psp', 0x29): {'header_size': 0x100, 'version_offset': 0x60, 'unpack_format': 'BBH', 'print_format': "{0:d}.{1:d}.{2:d}"},
  # ABL
  ('psp', 0x30): {'function': 'version_abl_from_dir_Entry'},
  ('psp', 0x31): {'redirect': ('psp', 0x30)},
  ('psp', 0x32): {'redirect': ('psp', 0x30)},
  ('psp', 0x33): {'redirect': ('psp', 0x30)},
  ('psp', 0x34): {'redirect': ('psp', 0x30)},
  ('psp', 0x35): {'redirect': ('psp', 0x30)},
  ('psp', 0x36): {'redirect': ('psp', 0x30)},
  ('psp', 0x37): {'redirect': ('psp', 0x30)},
  # ABL version types
  'abl_base': {'header_size': 0x110, 'version_offset': 0x60, 'unpack_format': 'I', 'print_format': "{0:08X}"},
  'abl_version_location': {'header_size': 0x110, 'version_offset': 0x104, 'unpack_format': 'I'},
  # DXIO
  # ('psp', 0x42): {'header_size': 0x100, 'version_offset': 0x60, 'unpack_format': 'BBH', 'print_format': "{1:d}.{0:d}.{2:d}"},
  # Microcode
  ('bios', 0x66): {'header_size': 0x30, 'version_offset': 0x0, 'unpack_format': 'HBBL16xH', 'print_format': "Date={2:X}/{1:X}/{0:X}, PatchId={3:#X}, EquivProcRevId={4:#X}"},
  # Default is standard PSP format
  'Default': {'header_size': 0x100, 'version_offset': 0x60, 'unpack_format': 'BBBB', 'print_format': "{3:X}.{2:X}.{1:X}.{0:X}"}
}

RomSigAddrTable = [
  0x00FA0000, #  --> 512KB base
  0x00F20000, #  --> 1MB base
  0x00E20000, #  --> 2MB base
  0x00C20000, #  --> 4MB base
  0x00820000, #  --> 8MB base
  0x00020000, #  --> 16MB base
  0x00120000, #  --> Special Base
]

#Set log level to ERROR
log = logging.getLogger()
FORMAT = '%(message)s'  #message ONLY
logging.basicConfig(format= FORMAT,stream=sys.stderr,level=logging.ERROR)

#Utility routines
def StrToNum (Str):
  if (Str != None):
    if type(Str) is str:
      if (Str.endswith ('L')):
        Str = Str.rstrip ('L')
      num = int(eval(Str))
      return num
    elif type(Str) is int:
      return Str
    else:
      logging.error("unexpected Data Type {}".format(type(Str)))
      FatalErrorExit ()

def CheckFileExistance (file):
  return os.path.exists(file)

def IsImageEntry (Entry):
  if (Entry['Entry'] == 'IMAGE_ENTRY'):
    return True
  return False

def IsValueEntry (Entry):
  if (Entry['Entry'] == 'VALUE_ENTRY'):
    return True
  return False

def IsPointEntry (Entry):
  if (Entry['Entry'] == 'POINT_ENTRY'):
    return True
  return False

def IsResetImageEntry (Entry):
  if ((Entry['Type'] == 0x62)  and
      (Entry['ResetImage'] == 1)) :
    return True
  return False

def FatalErrorExit ():
  log.error (traceback.extract_stack())
  log.error ("ERROR Exit\n")
  sys.exit (2)

def prettify(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = tostring(elem).decode()
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

def GetOccupiedSize (ActualSize, Alignment):
  """Calculate occupied size considering the alignment"""
  return   (ActualSize + (((Alignment) - ((ActualSize) & ((Alignment) - 1))) & ((Alignment) - 1)))

def AlignAddress (Address, Alignment):
  """Calculate occupied size considering the alignment"""
  return   (Address + (((Alignment) - ((Address) & ((Alignment) - 1))) & ((Alignment) - 1)))

def getEntryType (DirectoryInfo, EntryInfo, isPspEntry):
  global PspDirectoryEntryName
  global BiosDirectoryEntryName
  if(isPspEntry == 1):
    if EntryInfo["Type"] in PspDirectoryEntryName:
      #Check if inside of Current DIR region if not set to "POINT_ENTRY"
      if (DirectoryInfo["BaseOffset"] != 0):
        BaseOffset = DirectoryInfo["BaseOffset"]
      else :
        BaseOffset = DirectoryInfo["DirOffset"]

      if ((EntryInfo["Address"] >= BaseOffset) and (EntryInfo["Address"] < BaseOffset + DirectoryInfo["DirSize"])):
        EntryType = PspDirectoryEntryName[EntryInfo["Type"]][1]
      else:
        EntryType = "POINT_ENTRY"
    else:
      EntryType = "UNKNOWN_ENTRY"
  else:
    if EntryInfo["Type"] in BiosDirectoryEntryName:
      #Check if inside of Current DIR region if not set to "POINT_ENTRY"
      if (DirectoryInfo["BaseOffset"] != 0):
        BaseOffset = DirectoryInfo["BaseOffset"]
      else :
        BaseOffset = DirectoryInfo["DirOffset"]

      if ((EntryInfo["Address"] >= BaseOffset) and (EntryInfo["Address"] < BaseOffset + DirectoryInfo["DirSize"])):
        EntryType = BiosDirectoryEntryName[EntryInfo["Type"]][1]
      else:
        EntryType = "POINT_ENTRY"
    else:
      EntryType = "UNKNOWN_ENTRY"
  return EntryType

def getEntryShortName (Type, isPspEntry):
  global PspDirectoryEntryName
  global BiosDirectoryEntryName
  if(isPspEntry == 1):
    if Type in PspDirectoryEntryName:
      return PspDirectoryEntryName[Type][2]
    else:
      return "unknown"
  else:
    if Type in BiosDirectoryEntryName:
      return BiosDirectoryEntryName[Type][2]
    else:
      return "unknown"

def getEntryName (Type, isPspEntry):
  global PspDirectoryEntryName
  global BiosDirectoryEntryName
  if(isPspEntry == 1):
    if Type in PspDirectoryEntryName:
      return PspDirectoryEntryName[Type][3]
    else:
      return "Unknown Type"
  else:
    if Type in BiosDirectoryEntryName:
      return BiosDirectoryEntryName[Type][3]
    else:
      return "Unknown Type"

def IsEntryModifiable (Type, isPspEntry):
  global PspDirectoryEntryName
  global BiosDirectoryEntryName
  if(isPspEntry == 1):
    if Type in PspDirectoryEntryName:
      return PspDirectoryEntryName[Type][4]
    else:
      return 0
  else:
    if Type in BiosDirectoryEntryName:
      return BiosDirectoryEntryName[Type][4]
    else:
      return 0

def GetOutEntryFileBaseName (entry, DirStr, Level, isPspEntry, LevelType="", LevelIndex=0):
  if (IsResetImageEntry (entry)):
      return "ResetImage"
  elif (IsImageEntry (entry)):
    if ((entry['Type'] == 7) and (Level == 1)):
    #Use constant name for RTM signature
      return "RTMSignature"
    elif ((entry['Type'] == 7) and (Level == 2)):
    #Use constant name for RTM signature
      return "RTMSignatureL1L2"
    else:
      return "%sL%x%s%d_Typex%x_%x_%x_%x_%s" %(DirStr, Level, LevelType, LevelIndex, entry['Type'], entry['RomId'], entry['Instance'], entry['SubProgram'], getEntryShortName(entry['Type'], isPspEntry))
  elif (IsPointEntry (entry)):
    return "%sL%x%s%d_Typex%x_%x_%x_%x_%s" %(DirStr, Level, LevelType, LevelIndex, entry['Type'], entry['RomId'], entry['Instance'], entry['SubProgram'], getEntryShortName(entry['Type'], isPspEntry))
  else:
    return "Unsupported"

def GetOutEntryFileName (entry, DirStr, Level, isPspEntry, LevelType="", LevelIndex=0):
  if (IsResetImageEntry (entry)):
      return "ResetImage.bin"
  elif (IsImageEntry (entry)):
    if ((entry['Type'] == 7) and (Level == 1)):
    #Use constant name for RTM signature
      return "RTMSignature.bin"
    elif ((entry['Type'] == 7) and (Level == 2)):
    #Use constant name for RTM signature
      return "RTMSignatureL1L2.bin"
    else:
      return "%sL%x%s%d_Typex%x_%x_%x_%x_%s.bin" %(DirStr, Level, LevelType, LevelIndex, entry['Type'], entry['RomId'], entry['Instance'], entry['SubProgram'], getEntryShortName(entry['Type'], isPspEntry))
  else:
    return "Unsupported"

def Fletcher32Crc (data, words):
  sum1 = 0xffff
  sum2 = 0xffff
  tlen = 0
  i = 0

  while (words):
    tlen = 359
    if (words < 359):
      tlen = words
    words -=tlen

    while tlen:
      s = struct.Struct('H')
      val = s.unpack(data [i: i+2])[0]
      #print "%x"%(val),
      i +=2
      sum1 += val
      sum2 += sum1
      tlen -=1

    sum1 = (sum1 & 0xffff) + (sum1 >> 16)
    sum2 = (sum2 & 0xffff) + (sum2 >> 16)

  sum1 = (sum1 & 0xffff) + (sum1 >> 16)
  sum2 = (sum2 & 0xffff) + (sum2 >> 16)
  return (sum2 << 16 | sum1)

def WriteBinaryFile (File, buffer):
  global OutputPath
  try:
    FileHandle = open (OutputPath + File,'wb')
  except:
    log.error("Error: Opening {}".format(OutputPath + File))
    FatalErrorExit ()
  FileHandle.write(buffer)
  FileHandle.close()


def ReadBinaryFile (File):

  if (os.path.exists(File) == False):
    log.error("\nError :{} does not exist".format(File))
    return (None)

  try:
    FileHandle = open (File,'rb')
  except:
    log.error("Error: Opening {}".format(File))
    FatalErrorExit ()
  lines = FileHandle.read()
  FileHandle.close()
  return lines

def getOffsetInBin (romsize, offset):
  '''
    romsize equal zero indicate it is a relative address, no need to do the translation
  '''
  if ((gXorBit24 == 1) and (offset & 0xF0000000)):
    log.error ("Incorrect configuration, EFS at top 16M, where offset still using legacy Pysical MMIO address")
    FatalErrorExit ()
  if (romsize):
    if (romsize > 0x1000000):
      # Physical Address can only be used on pre SSP program, force set romsize to 16M
      romsize = 0x1000000
    #Use 0xFFFFFFFF + 1 instead of 0x100000000 to avoid long number which will leading "L" trailing when print out
    return romsize-(0xFFFFFFFF - offset + 1)
  else:
    return offset

def getMmioAddress (romsize, offset):
  #Use 0xFFFFFFFF + 1 instead of 0x100000000 to avoid long number which will leading "L" trailing when print out
  # MMIO address can only be used for legacy program, and only support upper to 16M SPI
  if(offset > 0x1000000):
    log.error("Does not support AddressMode 0 in 32M BIOS!")
    FatalErrorExit()
  if (romsize >0x1000000):
    romsize = 0x1000000
  return 0xFFFFFFFF - romsize + offset + 1

def getRelativeAddress (romsize, mmioAddr):
  #Use 0xFFFFFFFF + 1 instead of 0x100000000 to avoid long number which will leading "L" trailing when print out
  #return 0,
  #if mmioAddr is not in given romsize MMIO region
  if (mmioAddr < getMmioAddress (romsize, 0)):
    return 0
  else:
    return mmioAddr - getMmioAddress (romsize, 0);

def AddrXorBit24 (address, xorbit):
  return (address ^ (xorbit << 24))

def AddrGlobalXorBit24 (address):
  global gXorBit24
  if (address != None):
    return (address ^ (gXorBit24 << 24))
  else:
    return None

def PatchBinary (OrgBuffer, ModBuffer, Offset, Message):
  Offset = AddrGlobalXorBit24 (Offset)
  print("IMAGE: [0x%08x ~ 0x%08x] <%s>" %(Offset, Offset + len(ModBuffer) -1, Message))
  if (Offset + len (ModBuffer) > len (OrgBuffer)):
    log.error ("ERROR: Size error in PatchBinary")
    FatalErrorExit ()
  buffer = OrgBuffer[0:Offset] + ModBuffer + OrgBuffer[Offset+ len(ModBuffer):]
  return buffer

def StringlizeDict (Dict):
  try:
    # Python 2 supports int and long
    NumberTypes = (int, long)
  except NameError:
    # Python 3 only supports int
    NumberTypes = (int)
  for (k,v) in Dict.items():
    if (isinstance(v, NumberTypes)):
      Dict[k]=hex(v)
  return Dict

#Functional routines
def ParseArg ():
  global Version
  global ProgramStr
  global ProgramTable
  """Parse Input arguments, and return the parsed result"""
  Parser = argparse.ArgumentParser (description='Tool used to Build PSP DirTable & Embed/Dump PSP entries')
  Parser.add_argument ('--version', action='version', version='BuildPspDirectory %s' %(Version))
  Parser.add_argument('-v', '--verbosity', type=str, choices=['CRITICAL', 'ERROR', 'WARNING', 'INFO', 'DEBUG'], default='INFO')
  Parser.add_argument ('-o','--outputpath',default="Output")
  SubParser = Parser.add_subparsers(dest='subcommand',help="Type '<subcommand> -h' for help on a specific subcommand")
  #Build Directory table
  BDParser = SubParser.add_parser ('bd',help='Build Directory table header only')
  BDParser.add_argument ("InBiosImage", help="Specify the INPUT BIOS image file name")
  BDParser.add_argument ("CfgFile", help="Specify the configure file for build the PSP Directory")
  #Build Bios image with psp entry file embedded
  BBParser = SubParser.add_parser ('bb',help='Build Bios image with psp entry file embedded')
  BBParser.add_argument ("InBiosImage", help="Specify the INPUT BIOS image file name")
  BBParser.add_argument ("CfgFile", help="Specify the config file for build the PSP Directory")
  BBParser.add_argument ("OutBiosImage", help="Specify the OUTPUT BIOS image file name")
  #DumP psp entry of given BIOS image
  DPParser = SubParser.add_parser ('dp',help='DumP psp entry of given BIOS image')
  DPParser.add_argument ("InBiosImage", help="Specify the INPUT BIOS image file name")
  DPParser.add_argument ("-p", "--program", help="Specify the program name, Valid choices: %s" %list(ProgramTable.keys()), required=True)
  DPParser.add_argument ("-x","--xml", help="Output the information in XML format to PspDirInfo.xml",action="store_true")
  DPParser.add_argument ("-b","--binary", help="Output  psp binaries to outputpath",action="store_true")
  DPParser.add_argument ("-d","--directory", help="Output PspDirectory.xml to outputpath",action="store_true")
  DPParser.add_argument ("-t","--versiontxt", help="Output PspVersion.txt to outputpath",action="store_true")

  args = Parser.parse_args ()

  log.setLevel(args.verbosity)

  #Parameter check
  if (args.subcommand == 'bd' or args.subcommand == 'bb'):
    if (CheckFileExistance(args.CfgFile) == False):
      log.error("[Error] Can't Open CfgFile '%s'", args.CfgFile)
      FatalErrorExit ()
      #To be Done: Check PSP Directory Format
      #if (CheckCfgFileFormat(args.CfgFile) == False):
        # print "[Error] CfgFile '%s' doesn't exists" %args.CfgFile
        # FatalErrorExit ()

  if (args.subcommand == 'dp' or args.subcommand == 'bd' or args.subcommand == 'bb'):
    if (CheckFileExistance(args.InBiosImage) == False):
      log.error ("[Error] Can't Open BiosImage '%s'", args.InBiosImage)
      FatalErrorExit ()
# Validate Program string
  if (args.subcommand == 'dp'):
    ValidProgram = False
    for program in list(ProgramTable.keys()):
      if (args.program.upper() != program):
        ValidProgram = True
        break
    if (ValidProgram == False):
      log.error ("[Error] Not a valid program string, only %s are supported", list(ProgramTable.keys()))
      FatalErrorExit ()
    ProgramStr = args.program.upper ()
  return args

def ValidatePspDirAttrib (PspDirAttrib):
  # The valid attributes for PSP Dir tag can be like below:
  # *Note* the attributes are case sensitive
  # 1) None       |Used when only generate the directory header with "bd" command
  # 2) Base       |Used for fix address format, which all address need explicit defined, include where to embed the Directory Header.
  # 3) Base, Size |Used for dynamic address format, user reserve a region for hold all PSP entries include the header
  if (((PspDirAttrib['Base'] == None) and  (PspDirAttrib['Size'] != None))):
    return False

  if (((PspDirAttrib['Base'] == None) and  (PspDirAttrib['HeaderBase'] != None))):
    log.error ("ERROR: Don't support specify HeaderBase only, Base need be declared")
    return False

  if (PspDirAttrib['HeaderBase'] != None):
    if ((StrToNum(PspDirAttrib['HeaderBase']) % 0x1000) != 0):
      log.error ("ERROR: HeaderBase must be 0x1000 aligned")
      return False
    if (StrToNum(PspDirAttrib['HeaderBase']) > 0x4000000):
      log.error ("ERROR: Exceed Max HeaderBase 0x4000000")
      return False

  if (PspDirAttrib['Size'] != None):
    if ((StrToNum(PspDirAttrib['Size']) % 0x1000) != 0):
      log.error ("ERROR: Dir Size must be 0x1000 aligned")
      return False
    if (StrToNum(PspDirAttrib['Size']) > 0x400000):
      log.error ("ERROR: Exceed Max Dir Size 0x400000")
      return False

  if (PspDirAttrib['SpiBlockSize'] != None):
    if ((StrToNum(PspDirAttrib['SpiBlockSize']) % 0x1000) != 0):
      log.error ("ERROR: SpiBlockSize must be 0x1000 aligned")
      return False
    if (StrToNum(PspDirAttrib['SpiBlockSize']) > 0x10000):
      log.error ("ERROR: Exceed Max SpiBlockSize 0x10000")
      return False
  return True

def ValidateBiosDirAttrib (BiosDirAttrib):
  # The valid attributes for PSP Dir tag can be like below:
  # *Note* the attributes are case sensitive
  # 1) None       |Used when only generate the directory header with "bd" command
  # 2) Address    |Used for fix address format, which all address need explicit defined, include where to embed the Directory Header.
  # 3) Base, Size |Used for dynamic address format, user reserve a region for hold all PSP entries include the header
  if (((BiosDirAttrib['Base'] == None) and  (BiosDirAttrib['Size'] != None))):
    return False

  if (((BiosDirAttrib['Base'] == None) and  (BiosDirAttrib['HeaderBase'] != None))):
    log.error ("ERROR: Don't support specify HeaderBase only, Base need be declared")
    return False

  if (BiosDirAttrib['Size'] != None):
    if ((StrToNum(BiosDirAttrib['Size']) % 0x1000) != 0):
      log.error ("ERROR: Dir Size must be 0x1000 aligned")
      return False
    if (StrToNum(BiosDirAttrib['Size']) > 0x400000):
      log.error ("ERROR: Exceed Max Dir Size 0x400000")
      return False

  if (BiosDirAttrib['SpiBlockSize'] != None):
    if ((StrToNum(BiosDirAttrib['SpiBlockSize']) % 0x1000) != 0):
      log.error ("ERROR: SpiBlockSize must be 0x1000 aligned")
      return False
    if (StrToNum(BiosDirAttrib['SpiBlockSize']) > 0x10000):
      log.error ("ERROR: Exceed Max SpiBlockSize 0x10000")
      return False
  return True

def ValidateComboDirAttrib (ComboDirAttrib):
  if (((ComboDirAttrib['Base'] == None) or  (ComboDirAttrib['LookUpMode'] == None))):
    log.error ("ERROR: Base and LookUpMode need be explicit defined in COMBO_DIR/BIOS_COMBO_DIR tag")
    return False
  if (StrToNum(ComboDirAttrib['LookUpMode']) > 1):
    log.error ("Invalid value for LookUpMode, should be 0 or 1")


def ValidatePspEntry (PspEntry):
  #Below items will be checked
  #entry: should be one of three: IMAGE_ENTRY;VALUE_ENTRY;POINT_ENTRY
  if (not (IsImageEntry (PspEntry) or IsValueEntry(PspEntry) or IsPointEntry (PspEntry))):
    return False
  #Type should be explicit defined
  if (PspEntry['Type'] == None):
    log.error ("ERROR: Type haven't been explicit defined")
    return False
  #"File" need be defined for Image entry:
  if (IsImageEntry (PspEntry)):
    if (PspEntry['File'] == None):
      log.error ("ERROR: File haven't been explicit defined for IMAGE_ENTRY")
      return False
  #"Value" need be defined for Value entry
  if (IsValueEntry (PspEntry)):
    if (PspEntry['Value'] == None):
      log.error ("ERROR: Value haven't been explicit defined for VALUE_ENTRY")
      return False
  #"Address" "Size" need be defined for Point entry
  if (IsPointEntry(PspEntry)):
    if ((PspEntry['Address'] == None) or (PspEntry['Size'] == None)):
      log.error ("ERROR: Address or Size haven't been explicit defined for VALUE_ENTRY")
      return False

  if (PspEntry['RomId'] != None):
    if (StrToNum(PspEntry['RomId']) > 4):
      log.error ("ERROR: Exceed RomId valid range [0-3]\n")
      return False
  return True

def ValidateBiosEntry (BiosEntry):
  #Below items will be checked
  #entry: should be one of three: IMAGE_ENTRY;VALUE_ENTRY;POINT_ENTRY
  if (not (IsImageEntry (BiosEntry) or IsValueEntry(BiosEntry) or IsPointEntry (BiosEntry))):
    return False
  #Type should be explicit defined
  if (BiosEntry['Type'] == None):
    log.error ("ERROR: Type haven't been explicit defined")
    return False
  #"File" need be defined for Image entry:
  if (IsImageEntry (BiosEntry)):
    if (BiosEntry['File'] == None):
      return False
  #"Value" need be defined for Value entry
  if (IsValueEntry (BiosEntry)):
    if (BiosEntry['Value'] == None):
      return False
  #"Address" "Size" need be defined for Point entry
  if (IsPointEntry(BiosEntry)):
    if ((BiosEntry['Address'] == None) or (BiosEntry['Size'] == None)):
      return False
  if (BiosEntry['Type'] > 0xFF):
      log.error ("ERROR: Type exceed limit 0xFF")
      return False

  if (BiosEntry['RegionType'] > 0xFF):
      log.error ("ERROR: RegionType exceed limit 0xFF")
      return False

  if (BiosEntry['ResetImage'] > 0x1):
      log.error ("ERROR: BiosResetImage exceed limit 0x1")
      return False

  if (BiosEntry['Copy'] > 0x1):
      log.error ("ERROR: Copy exceed limit 0x1")
      return False

  if (BiosEntry['ReadOnly'] > 0x1):
      log.error ("ERROR: ReadOnly exceed limit 0x1")
      return False

  if (BiosEntry['Compressed'] > 0x1):
      log.error ("ERROR: Compressed exceed limit 0x1")
      return False

  if (BiosEntry['Instance'] > 0xF):
      log.error ("ERROR: Instance exceed limit 0xF")
      return False

  if (BiosEntry['SubProgram'] > 0x7):
      log.error ("ERROR: SubProgram exceed limit 0x7")
      return False

  if (BiosEntry['RomId'] > 4):
    log.error ("ERROR: Exceed RomId valid range [0-3]\n")
    return False
  return True

def ValidateComboEntry (ComboEntry):
  #Below items will be checked
  #Below attribute should be explicit defined
  if (ComboEntry['IdSelect'] == None):
    log.error ("ERROR: IdSelect haven't been explicit defined")
    return False
  if (ComboEntry['Id'] == None):
    log.error ("ERROR: Id haven't been explicit defined")
    return False
  if (ComboEntry['Address'] == None):
    log.error ("ERROR: Address haven't been explicit defined")
    return False
  if (ComboEntry['IdSelect'] >0x1):
    log.error ("ERROR: Invalid value for IdSelect, should be either 0 or 1")
    return False
  return True

def ValidateCmnEntry (CmnEntry):
  #Below items will be checked
  #entry: should be IMAGE_ENTRY
  if (not (IsImageEntry (CmnEntry))):
    return False
  #"File" need be defined for Image entry:
  if (IsImageEntry (CmnEntry)):
    if (CmnEntry['File'] == None):
      log.error ("ERROR: File haven't been explicit defined for IMAGE_ENTRY")
      return False
    if ((CmnEntry['Address'] == None) or (CmnEntry['Size'] == None)):
      log.error ("ERROR: Address or Size haven't been explicit defined for IMAGE_ENTRY")
      return False
  if (CmnEntry['RomId'] != None):
    if (StrToNum(CmnEntry['RomId']) > 4):
      log.error ("ERROR: Exceed RomId valid range [0-3]\n")
      return False
  return True

def BuildPspEntries (PspDir, level):
  PspEntries = []
  first = 1
  found = 0
  error = 0
  for entry in PspDir:
    log.debug ("Psp Entry %s->%s",entry.tag, entry.attrib)
    PspEntry = {}
    # Initial default value
    PspEntry['Type'] = None
    PspEntry['File'] = None
    PspEntry['Address'] = None
    PspEntry['Size'] = None
    PspEntry['Entry'] = None
    PspEntry['Value'] = None
    PspEntry['Recovery'] = 0
    PspEntry['RomId'] = 0
    PspEntry['Instance'] = 0
    PspEntry['SubProgram'] = 0
    PspEntry['AbsoluteAddr'] = 0
    PspEntry['AddressMode'] = 0xFF
    #Update the attribute get from XML
    PspEntry.update (entry.attrib)
    PspEntry['Entry'] = entry.tag
    #Transfer all numeric field
    PspEntry['Type'] = StrToNum(PspEntry['Type'] )
    PspEntry['Address'] = StrToNum(PspEntry['Address'])
    PspEntry['Size'] = StrToNum(PspEntry['Size'])
    PspEntry['Value'] = StrToNum(PspEntry['Value'])
    PspEntry['Recovery'] = StrToNum(PspEntry['Recovery'])
    PspEntry['RomId'] = StrToNum (PspEntry['RomId'])
    PspEntry['Instance'] = StrToNum (PspEntry['Instance'])
    PspEntry['SubProgram'] = StrToNum (PspEntry['SubProgram'])
    PspEntry['AbsoluteAddr'] = StrToNum (PspEntry['AbsoluteAddr'])
    PspEntry['AddressMode'] = StrToNum (PspEntry['AddressMode'])

    #Compatibility For old programs without SubProgram
    if (PspEntry['SubProgram'] == 0):
      PspEntry['SubProgram'] = PspEntry['Type'] >> 8
    PspEntry['Type'] = PspEntry['Type'] & 0xFF

    log.debug ("PspEntry %s", PspEntry)
    if (ValidatePspEntry (PspEntry) == False):
      log.error ("ERROR: Unrecognized Attribute/Tag found in %s->%s", entry.tag, entry.attrib)
      FatalErrorExit ()
    if(PspEntry['Type'] == 0):
      found = 1
    if(level == 1 and first == 1):
      first = 0
      if(PspEntry['Type'] != 0):
        error = 1
    PspEntries.append(PspEntry)
  if(error == 1 and found == 1):
    #Report error if there is entry 0x0 in PSP L1 directory and it is not the first entry
    log.error ("ERROR: Entry 0x0 is not the first entry in PSP L1 directory")
    FatalErrorExit ()
  return PspEntries

def BuildBiosEntries (BiosDir):
  BiosEntries = []
  for entry in BiosDir:
    BiosEntry = {}
    log.debug ("%s->%s",entry.tag, entry.attrib)
    # Initial default value
    BiosEntry['Type'] = None
    BiosEntry['File'] = None
    BiosEntry['Address'] = None
    BiosEntry['Size'] = None
    BiosEntry['Entry'] = None
    BiosEntry['Destination'] = '0xFFFFFFFFFFFFFFFF'
    BiosEntry['Instance'] = 0
    BiosEntry['SubProgram'] = 0
    BiosEntry['RomId'] = 0
    BiosEntry['AbsoluteAddr'] = 0
    BiosEntry['AddressMode'] = 0xFF
    #Set all attributes default to 0
    BiosEntry['RegionType'] = 0
    BiosEntry['ResetImage'] = 0
    BiosEntry['Copy'] = 0
    BiosEntry['ReadOnly'] = 0
    BiosEntry['Compressed'] = 0
    BiosEntry['Recovery'] = 0
    #Update the attribute get from XML
    BiosEntry.update (entry.attrib)
    BiosEntry['Entry'] = entry.tag
    #Transfer all numeric field
    BiosEntry['Type'] = StrToNum(BiosEntry['Type'] )
    BiosEntry['Address'] = StrToNum(BiosEntry['Address'])
    BiosEntry['Size'] = StrToNum(BiosEntry['Size'])
    BiosEntry['Destination'] = StrToNum(BiosEntry['Destination'])
    BiosEntry['Instance'] = StrToNum(BiosEntry['Instance'])
    BiosEntry['Recovery'] = StrToNum(BiosEntry['Recovery'])
    BiosEntry['SubProgram'] = StrToNum (BiosEntry['SubProgram'])
    BiosEntry['RomId'] = StrToNum (BiosEntry['RomId'])
    BiosEntry['AbsoluteAddr'] = StrToNum (BiosEntry['AbsoluteAddr'])
    BiosEntry['AddressMode'] = StrToNum (BiosEntry['AddressMode'])

    for typeAttrib in entry:
      log.debug ("%s->%s",typeAttrib.tag, typeAttrib.attrib)
      #Update the attribute get from XML
      BiosEntry.update (typeAttrib.attrib)
      #Transfer all numeric field
      BiosEntry['RegionType'] = StrToNum(BiosEntry['RegionType'] )
      BiosEntry['ResetImage'] = StrToNum(BiosEntry['ResetImage'])
      BiosEntry['Copy'] = StrToNum(BiosEntry['Copy'])
      BiosEntry['ReadOnly'] = StrToNum(BiosEntry['ReadOnly'])
      BiosEntry['Compressed'] = StrToNum(BiosEntry['Compressed'])
    log.debug ("BiosEntry %s", BiosEntry)
    if (ValidateBiosEntry (BiosEntry) == False):
      log.error ("ERROR: Unrecognized Attribute/Tag found in %s->%s", entry.tag, entry.attrib)
      FatalErrorExit ()
    BiosEntries.append(BiosEntry)
  return BiosEntries

def BuildComboEntries (ComboDir):
  ComboEntries = []
  for entry in ComboDir:
    log.debug ("Combo Entry %s->%s",entry.tag, entry.attrib)
    ComboEntry = {}
    # Initial default value
    ComboEntry['IdSelect'] = None
    ComboEntry['Id'] = None
    ComboEntry['Address'] = None
    #Update the attribute get from XML
    ComboEntry.update (entry.attrib)
    #Transfer all numeric field
    ComboEntry['IdSelect'] = StrToNum(ComboEntry['IdSelect'] )
    ComboEntry['Id'] = StrToNum(ComboEntry['Id'])
    ComboEntry['Address'] = StrToNum(ComboEntry['Address'])
    log.debug ("ComboEntry %s", ComboEntry)
    if (ValidateComboEntry (ComboEntry) == False):
      log.error ("ERROR: Unrecognized Attribute/Tag found in %s->%s", entry.tag, entry.attrib)
      FatalErrorExit ()
    ComboEntries.append(ComboEntry)
  return ComboEntries

def BuildCmnEntries (CmnDir):
  CmnEntries = []
  for entry in CmnDir:
    log.debug ("Cmn Entry %s->%s",entry.tag, entry.attrib)
    CmnEntry = {}
    # Initial default value
    CmnEntry['File'] = None
    CmnEntry['Address'] = None
    CmnEntry['Size'] = None
    CmnEntry['RomId'] = 0
    #Update the attribute get from XML
    CmnEntry.update (entry.attrib)
    CmnEntry['Entry'] = entry.tag
    #Transfer all numeric field
    CmnEntry['Address'] = StrToNum(CmnEntry['Address'])
    CmnEntry['Size'] = StrToNum(CmnEntry['Size'])
    log.debug ("CmnEntry %s", CmnEntry)
    if (ValidateCmnEntry (CmnEntry) == False):
      log.error ("ERROR: Unrecognized Attribute/Tag found in %s->%s", entry.tag, entry.attrib)
      FatalErrorExit ()
    CmnEntries.append(CmnEntry)
  return CmnEntries

def ParseCfgFile (CfgFile):
  global AddressMode
  global Writable
  global DirHeaderSize
  global gXorBit24
  # To support multiple PSP Dir
  PspDicts = []
  BiosDicts = []
  IshDicts = []

  ComboDict = {}
  BiosComboDict = {}
  CmnDict = {}
  try:
    tree = ET.parse (CfgFile)
  except:
    log.error ("ERROR: Parse the %s fail, please check file's format", CfgFile)
    FatalErrorExit ()
  root = tree.getroot ()
  # Get Address mode, if have been defined, or else use the default value
  if 'AddressMode' in root.attrib:
    log.debug ("AddressMode explicit defined as %s", root.attrib['AddressMode'])

    AddressMode = StrToNum (root.attrib['AddressMode'])
    # Validate AddressMode Value
    if (not ((AddressMode == 0) or (AddressMode == 1) or (AddressMode == 2) or (AddressMode == 3))):
      log.error ("ERROR: Invalid AddressMode:\n  0: SPI(Default, if no explicit defined)\n  1: eMMC/UFS\n  2: relative to PSP/BIOS directory header\n  3: relative to current slot")
      FatalErrorExit ()
  # Get Writable Bit Enabled. If enabled, writable bit is set for writable entries
  if 'Writable' in root.attrib:
    log.debug ("Writable Bit Enabled defined as %s", root.attrib['Writable'])

    Writable = StrToNum (root.attrib['Writable'])
    # Validate AddressMode Value
    if (not ((Writable == 0) or (Writable == 1))):
      log.error ("ERROR: Invalid Writable: 0 Disabled(Default, if no explicit defined) 1: Enabled")
      FatalErrorExit ()
  if 'XorBit24' in root.attrib:
    log.debug ("XorBit24 explicit defined as %s", root.attrib['XorBit24'])
    gXorBit24 = StrToNum (root.attrib['XorBit24'])

  for Dir in root:
    log.debug(Dir.tag)
    #PSP DIR
    if (Dir.tag == 'PSP_DIR'):
      PspDict = {}
      #Init default value
      PspDict['Base'] = None
      PspDict['HeaderBase'] = None
      PspDict['Size'] = None
      PspDict['SpiBlockSize'] = None
      #Default Level 1
      PspDict['Level'] = 1
      #Set AddressMode to global Address Mode
      PspDict['AddressMode'] = AddressMode
      #Set Copy Attribute to 0
      PspDict['Copy'] = 0
      #Update the attribute get from XML
      PspDict.update (Dir.attrib)
      #Checking Attribute Keyword (Case sensitive)
      if (ValidatePspDirAttrib (PspDict) == False):
        log.error ("ERROR: Unrecognized Attribute found in %s", Dir.attrib)
        FatalErrorExit ()
      # log.debug ("PspDict: %s",PspDict)
      #Transfer all numeric field
      PspDict['Base'] = StrToNum (PspDict['Base'])
      PspDict['ImageBase'] = StrToNum (PspDict['Base'])
      PspDict['HeaderBase'] = StrToNum (PspDict['HeaderBase'])
      PspDict['Size'] = StrToNum (PspDict['Size'])
      PspDict['SpiBlockSize'] = StrToNum (PspDict['SpiBlockSize'])
      PspDict['Level'] = StrToNum (PspDict['Level'])
      PspDict['AddressMode'] = StrToNum (PspDict['AddressMode'])
      if(PspDict['SpiBlockSize'] == 0):
        PspDict['SpiBlockSize'] = 0x10000;#0 means SpiBlockSize = 64K
      #Build PSP Entries
      PspDict['Entries'] = BuildPspEntries (Dir, PspDict['Level'])
      log.debug ("PspDict: %s", PspDict)
      #Check Copy attribute
      if(StrToNum (PspDict['Copy']) == 1):
        log.debug ("Copy Attribute found. Copying from previous Level 2 Directory")
        if(PspDict['Level'] == 2):
          #Get the first level 2 PSP directory
          foungFlag = 0
          for d in PspDicts:
            if(d['Level'] == 2):
              log.debug ("Level 2 Directory Found. Copying...")
              foungFlag = 1
              base = PspDict['Base']
              PspDict = copy.deepcopy(d)
              PspDict['Base'] = base
              break
          if(foungFlag == 0):
            log.error ("Failed to find previous Level 2 PSP Directory!")
            FatalErrorExit ()
        else:
          log.error ("Only level 2 supports copy attribute!")
          FatalErrorExit ()
      PspDicts.append (PspDict)
    elif (Dir.tag == 'BIOS_DIR'):
      #BIOS DIR
      BiosDict = {}
      #Init default value
      BiosDict['Base'] = None
      BiosDict['ImageBase'] = None
      BiosDict['HeaderBase'] = None
      BiosDict['Size'] = None
      BiosDict['SpiBlockSize'] = None
      #Default Level 1
      BiosDict['Level'] = 1
      #Set AddressMode to global Address Mode
      BiosDict['AddressMode'] = AddressMode
      #Set Copy Attribute to 0
      BiosDict['Copy'] = 0
      #Update the attribute get from XML
      BiosDict.update (Dir.attrib)
      #Checking Attribute Keyword (Case sensitive)
      if (ValidateBiosDirAttrib (BiosDict) == False):
        log.error ("ERROR: Unrecognized Attribute found in %s", Dir.attrib)
      #Build PSP Entries
      BiosDict['Entries'] = BuildBiosEntries (Dir)
      #Transfer all numeric field
      BiosDict['Base'] = StrToNum (BiosDict['Base'])
      BiosDict['ImageBase'] = StrToNum (BiosDict['ImageBase'])
      BiosDict['HeaderBase'] = StrToNum (BiosDict['HeaderBase'])
      BiosDict['Size'] = StrToNum (BiosDict['Size'])
      BiosDict['SpiBlockSize'] = StrToNum (BiosDict['SpiBlockSize'])
      BiosDict['Level'] = StrToNum (BiosDict['Level'])
      BiosDict['AddressMode'] = StrToNum (BiosDict['AddressMode'])
      if(BiosDict['SpiBlockSize'] == 0):
        BiosDict['SpiBlockSize'] = 0x10000;#0 means SpiBlockSize = 64K
      log.debug ("BiosDict: %s", BiosDict)
      #Check Copy attribute
      if(StrToNum (BiosDict['Copy']) == 1):
        log.debug ("Copy Attribute found. Copying from previous Level 2 Directory")
        if(BiosDict['Level'] == 2):
          #Get the first level 2 PSP directory
          foungFlag = 0
          for d in BiosDicts:
            if(d['Level'] == 2):
              log.debug ("Level 2 Directory Found. Copying...")
              foungFlag = 1
              base = BiosDict['Base']
              BiosDict = copy.deepcopy(d)
              BiosDict['Base'] = base
              break
          if(foungFlag == 0):
            log.error ("Failed to find previous Level 2 BIOS Directory!")
            FatalErrorExit ()
        else:
          log.error ("Only level 2 supports copy attribute!")
          FatalErrorExit ()
      BiosDicts.append (BiosDict)
    elif (Dir.tag == 'COMBO_DIR'):
      #PSP COMBO DIR
      #Init default value
      ComboDict['Base'] = None
      ComboDict['LookUpMode'] = None
      #Set AddressMode to global Address Mode
      ComboDict['AddressMode'] = AddressMode
      #Update the attribute get from XML
      ComboDict.update (Dir.attrib)
      #Checking Attribute Keyword (Case sensitive)
      #use BIOS Dir rule to validate
      if (ValidateComboDirAttrib (ComboDict) == False):
        log.error ("ERROR: Unrecognized Attribute found in %s", Dir.attrib)
      #Build Combo Entries
      ComboDict['Entries'] = BuildComboEntries (Dir)
      #Transfer all numeric field
      ComboDict['Base'] = StrToNum (ComboDict['Base'])
      ComboDict['LookUpMode'] = StrToNum (ComboDict['LookUpMode'])
      ComboDict['AddressMode'] = StrToNum (ComboDict['AddressMode'])
      log.debug ("ComboDict: %s", ComboDict)
    elif (Dir.tag == 'BIOS_COMBO_DIR'):
      #BIOS COMBO DIR
      #Init default value
      BiosComboDict['Base'] = None
      BiosComboDict['LookUpMode'] = None
      #Set AddressMode to global Address Mode
      BiosComboDict['AddressMode'] = AddressMode
      #Update the attribute get from XML
      BiosComboDict.update (Dir.attrib)
      #Checking Attribute Keyword (Case sensitive)
      #use BIOS Dir rule to validate
      if (ValidateComboDirAttrib (BiosComboDict) == False):
        log.error ("ERROR: Unrecognized Attribute found in %s", Dir.attrib)
      #Build Combo Entries
      BiosComboDict['Entries'] = BuildComboEntries (Dir)
      #Transfer all numeric field
      BiosComboDict['Base'] = StrToNum (BiosComboDict['Base'])
      BiosComboDict['LookUpMode'] = StrToNum (BiosComboDict['LookUpMode'])
      BiosComboDict['AddressMode'] = StrToNum (BiosComboDict['AddressMode'])
      log.debug ("BiosComboDict: %s", BiosComboDict)
    elif (Dir.tag == 'CMN_DIR'):
      #Common DIR
      #Set AddressMode to global Address Mode
      CmnDict['AddressMode'] = AddressMode
      #Update the attribute get from XML
      CmnDict.update (Dir.attrib)
      #Build Combo Entries
      CmnDict['Entries'] = BuildCmnEntries (Dir)
      #Transfer all numeric field
      CmnDict['AddressMode'] = StrToNum (CmnDict['AddressMode'])
      log.debug ("CmnDict: %s", CmnDict)
    elif (Dir.tag == 'ISH_HEADER'):
      #ISH Header
      IshHeader = {}
      IshHeader ['Reserved_1'] = None
      IshHeader.update (Dir.attrib)
      IshHeader ['Base'] = StrToNum (IshHeader['Base'])
      IshHeader ['BootPriority'] = StrToNum (IshHeader['BootPriority'])
      IshHeader ['UpdateRetries'] = StrToNum (IshHeader['UpdateRetries'])
      IshHeader ['GlitchRetries'] = StrToNum (IshHeader['GlitchRetries'])
      IshHeader ['GlitchRetries'] = (IshHeader ['GlitchRetries'] & 0xFF) | 0xFFFFFF00
      IshHeader ['Location'] = StrToNum (IshHeader['Location'])
      IshHeader ['PspId'] = StrToNum (IshHeader['PspId'])
      IshHeader ['SlotMaxSize'] = StrToNum (IshHeader['SlotMaxSize'])
      if(IshHeader ['Reserved_1'] != None):
        IshHeader ['Reserved_1'] = StrToNum (IshHeader['Reserved_1'])
      else:
        IshHeader ['Reserved_1'] = 0xFFFFFFFF
      IshDicts.append(IshHeader)
    else:
      log.error ("ERROR: Unrecognized Tag (%s) found in %s", Dir.tag, CfgFile)
      FatalErrorExit ()
  #Check DIR region overlap
  #Make a generic DIR array with base size information
  DirList = []
  if (len(BiosDicts) != 0):
    for BiosDict in BiosDicts:
      if ((BiosDict['Base'] != None) and (BiosDict['Size'] != None)):
        DirElmt = {}
        DirElmt ['Base'] = BiosDict ['Base']
        DirElmt ['Size'] = BiosDict ['Size']
        DirList.append (DirElmt)

  if (len(BiosDicts) != 0):
    for BiosDict in BiosDicts:
      if (BiosDict['HeaderBase'] != None):
        DirElmt = {}
        DirElmt ['Base'] = BiosDict ['HeaderBase']
        DirElmt ['Size'] = DirHeaderSize
        DirList.append (DirElmt)

  if (len(PspDicts) != 0):
    for PspDict in PspDicts:
      if ((PspDict['Base'] != None) and (PspDict['Size'] != None)):
        DirElmt = {}
        DirElmt ['Base'] = PspDict ['Base']
        DirElmt ['Size'] = PspDict ['Size']
        DirList.append (DirElmt)

  if (len(PspDicts) != 0):
    for PspDict in PspDicts:
      if (PspDict['HeaderBase'] != None):
        DirElmt = {}
        DirElmt ['Base'] = PspDict ['HeaderBase']
        DirElmt ['Size'] = DirHeaderSize
        DirList.append (DirElmt)

  if (len(IshDicts) != 0):
    for IshDict in IshDicts:
      if (IshDict['Base'] != None):
        DirElmt = {}
        DirElmt ['Base'] = IshDict ['Base']
        DirElmt ['Size'] = 0x20
        DirList.append (DirElmt)

  for i in range (len (DirList)):
    for j in range (len (DirList)):
      #exclude Current index
      if (i != j):
        CurDirStart = DirList[i]['Base']
        CurDirEnd = DirList[i]['Base']+DirList[i]['Size'] - 1;
        OtherDirStart = DirList[j]['Base']
        OtherDirEnd = DirList[j]['Base']+DirList[j]['Size'] - 1;
        if (not((CurDirEnd < OtherDirStart) or (OtherDirEnd < CurDirStart))):
          log.error ("\nERROR: DIR [%x~%x] and DIR [%x~%x] region overlapped, please check PSP_DIR & BIOS_DIR \"Base\" & \"Size\" definition\n", CurDirStart, CurDirEnd, OtherDirStart, OtherDirEnd)
          FatalErrorExit ()

  return (PspDicts, BiosDicts, IshDicts, ComboDict, BiosComboDict, CmnDict)

def OptPspEntryOrder (PspDict):
#Do not change entry order to avoid unexpected error
  return PspDict

def CheckPspEntryTypeConflict (PspDict):
  global ProgramStr
  SortedList = sorted(PspDict['Entries'], key=SortPspEntryFunc)
  TypeCode = -1
  for Entry in SortedList:
    newTypeCode = Entry["Type"] + (Entry["RomId"] << 8) + (Entry["Instance"] << 16) + (Entry["SubProgram"] << 24)
    if(newTypeCode <= TypeCode):
      log.error("Duplicated PSP Entry Type found! Type: %x, RomId: %x, Instance: %x, SubProgram: %x", Entry["Type"], Entry["RomId"], Entry["Instance"], Entry["SubProgram"])
      if((Entry["Type"] != PSP_ENTRY_TYPE_LEVEL2A_DIR) and (Entry["Type"] != PSP_ENTRY_TYPE_LEVEL2B_DIR)):
        #VN may have multiple L2A directories
        FatalErrorExit ()
    TypeCode = newTypeCode
  return

def SortPspEntryFunc(Entry):
  TypeCode = Entry["Type"] + (Entry["RomId"] << 8) + (Entry["Instance"] << 16) + (Entry["SubProgram"] << 24)
  return TypeCode

def CheckBiosEntryTypeConflict (BiosDict):
  SortedList = sorted(BiosDict['Entries'], key=SortBiosEntryFunc)
  TypeCode = -1
  for Entry in SortedList:
    newTypeCode = Entry["Type"] + (Entry["Instance"] << 8) + (Entry["SubProgram"] << 12) + (Entry["RomId"] << 16)
    if(newTypeCode <= TypeCode):
      log.error("Duplicated BIOS Entry Type found! Type: %x, Instance: %x, SubProgram: %x, RomId: %x", Entry["Type"], Entry["Instance"], Entry["SubProgram"], Entry["RomId"])
      FatalErrorExit ()
    TypeCode = newTypeCode
  return

def SortBiosEntryFunc(Entry):
  TypeCode = Entry["Type"] + (Entry["Instance"] << 8) + (Entry["SubProgram"] << 12) + (Entry["RomId"] << 16)
  return TypeCode

def CalcEntryOffset (BiosSize, Directory, Name):
  global DirHeaderSize
  global gAlignment
  isPspEntry = 1
  if(Name == "BIOS_DIR"):
    isPspEntry = 0
  Alignment = 0
  # Calculate the Entry offset for dynamic format configure file
  # Which Base and Size have been explicit define in the DIR Node(PSP_DIR or BIOS_DIR)
  if ((Directory['Base'] != None) and (Directory['Size'] != None)):
    #PSP Entry Start Address
    if (Directory['HeaderBase'] != None):
      BaseOffset = AlignAddress (Directory['Base'], gAlignment)
      EntryOffset = BaseOffset
    else:
      BaseOffset = AlignAddress (Directory['Base'] + DirHeaderSize, gAlignment)
      EntryOffset = BaseOffset
    #Validate all entries:
    for Entry in Directory['Entries']:
      #Image Entry should not have Address attribute
      if (IsImageEntry(Entry)):
        log.debug (Entry)
        if (Entry['Address'] != None ):
          log.error ("ERROR: Entry should not have Address attribute as the 'Base' & 'Size' have been defined in parameter node")
          FatalErrorExit ()
        EntrySize = 0
        if (CheckFileExistance (Entry['File']) == False):#Add Entry['Size'] check for PSB signing
          if (Entry['Size'] == None):
            log.error ("ERROR: Can't open \"%s\" for entry type 0x%X", Entry['File'], Entry['Type'])
            FatalErrorExit ()
        else:
          EntrySize = os.path.getsize(Entry['File'])
        #If Size has been specified it will override the actual file size
        if (Entry['Size'] == None):
          Entry['Size'] = EntrySize
        elif (EntrySize > Entry['Size']):
          log.error ("ERROR: File %s size(0x%X) exceeds its size limit(0x%X)", Entry['File'], EntrySize, Entry['Size'])
          FatalErrorExit ()
        #Worker functions>>
        #Check if this entry is modifiable & SpiBlockSize is defined, using SpiBlockSize as Alignment, else using default Alignment
        if ((IsEntryModifiable (Entry['Type'], isPspEntry)) and (Directory['SpiBlockSize'] != None)):
          Alignment = Directory['SpiBlockSize']
        else:
          Alignment = gAlignment
        #log.debug("Before Align %x", EntryOffset)
        EntryOffset = AlignAddress (EntryOffset, Alignment)
        #Record to structure
        Entry['Address'] = EntryOffset
        #Update to Next Entry offset
        EntryOffset += Entry['Size']
        #log.debug("Type 0x%x Base %x Size %x", Entry['Type'], Entry['Address'], Entry['Size'])
    # Result
    UsedSize = EntryOffset - BaseOffset
    if(UsedSize > Directory['Size']):
      log.error ("[Error] Used size 0x%x Bytes exceed the limit of Dir Size 0x%x Bytes at %s Level 0x%x\n", UsedSize, Directory['Size'], Name, Directory['Level'])
      FatalErrorExit()
    percent = 100.0
    if(Directory['Size'] != 0):
      percent = float (UsedSize)/float (Directory['Size']) *100
    log.info("%s FV 0x%x bytes used, 0x%x bytes free [%.2f%% full]", "PSP" if isPspEntry else "BIOS" ,UsedSize, (Directory['Size'] -  UsedSize), percent )
  return Directory

def DumpPspDict (BiosSize, PspDict):
  for Entry in PspDict['Entries']:
    if (IsImageEntry(Entry) or IsPointEntry(Entry)):
      log.debug ("%s 0x%02X 0x%08x 0x%08x 0x%08x(R) (%s)", Entry['Entry'], Entry['Type'], Entry['Size'], Entry['Address'], Entry['Address'], getEntryName(Entry['Type'], 1))
    elif (IsValueEntry(Entry)):
      log.debug ("%s 0x%02X 0x%08x (%s)", Entry['Entry'], Entry['Type'], Entry['Value'], getEntryName(Entry['Type'], 1))

def DumpBiosDict (BiosSize, BiosDict):
  for Entry in BiosDict['Entries']:
    if (IsImageEntry(Entry) or IsPointEntry(Entry)):
      log.debug ("%s Type[%02X] RegionType[%02X] Reset[%01x] Copy[%01x] RO[%01x] ZIP[%01x] 0x%08x 0x%08x 0x%08x(R) 0x%x (%s)",Entry['Entry'], Entry['Type'],Entry['RegionType'],Entry['ResetImage'],Entry['Copy'],Entry['ReadOnly'], Entry['Compressed'], Entry['Size'], Entry['Address'],  Entry['Address'], Entry['Destination'], getEntryName(Entry['Type'], 0))
    elif (IsValueEntry(Entry)):
      log.debug ("%s 0x%02X 0x%08x (%s)", Entry['Entry'], Entry['Type'], Entry['Value'], getEntryName(Entry['Type'], 0))

def BuildPspDir (BiosSize, PspDict):
  global Writable
  AddressMode = PspDict['AddressMode'];
  PspEntryFmt = 'I I Q'
  PspEntry = b''
  PspEntrySize = struct.calcsize(PspEntryFmt)
  # Process each PSP entry
  for Entry in PspDict['Entries']:
    # typedef struct {
    #   UINT32 Type:8 ;          ///< Type of PSP Directory entry
    #   UINT32 SubProgram:8;       ///< Specify the SubProgram
    #   UINT32 RomId:2;            ///< Specify the ROM ID
    #   UINT32 Writable:1;         ///< 1: Region writable, 0: Region read only
    #   UINT32 Instance:4;         ///< Specify the Instance
    #   UINT32 Reserved:9;        ///< Reserved
    # } PSP_DIRECTORY_ENTRY_TYPE;
    TypeAttrib = Entry['Type'] + (Entry["SubProgram"] << 8) + (Entry['RomId'] << 16) + (Entry['Instance'] << 19)
    if((Writable == 1) and (IsEntryModifiable(Entry['Type'], 1))):
      TypeAttrib |= (1 << 18)
    EntryAddressMode = AddressMode
    if(Entry['AddressMode'] != 0xFF):
      EntryAddressMode = Entry['AddressMode']
    if(AddressMode == 2 and Entry['AbsoluteAddr'] == 1 and IsPointEntry(Entry)):
      EntryAddressMode = 0
    log.debug("Type: %d, Mode: %d", Entry['Type'], Entry['AddressMode'])

    if (IsImageEntry(Entry) or IsPointEntry(Entry)):
      if (EntryAddressMode == 0) :
        EntryData = (TypeAttrib, Entry['Size'], getMmioAddress (BiosSize, Entry['Address']))
      elif (EntryAddressMode == 1):
        EntryData = (TypeAttrib, Entry['Size'], Entry['Address'])
        if ((AddressMode == 2) or (AddressMode == 3)):
          EntryData = (TypeAttrib, Entry['Size'], Entry['Address'] + (0x40000000 << 32))
      elif (EntryAddressMode == 2):
        if(IsPointEntry(Entry)):
          EntryData = (TypeAttrib, Entry['Size'], Entry['Address'] + (0x80000000 << 32))
        else:
          EntryData = (TypeAttrib, Entry['Size'], Entry['Address'] - PspDict['Base'] + (0x80000000 << 32))
      elif (EntryAddressMode == 3):
        if(IsPointEntry(Entry)):
          EntryData = (TypeAttrib, Entry['Size'], Entry['Address'] + (0xC0000000 << 32))
        else:
          EntryData = (TypeAttrib, Entry['Size'], Entry['Address'] - PspDict['ImageBase'] + (0xC0000000 << 32))
    elif (IsValueEntry(Entry)):
      # Size field always be 0xFFFFFFFF for Value
      EntryData = (Entry['Type'], 0xFFFFFFFF, Entry['Value'])
    s = struct.Struct(PspEntryFmt)
    #Pack Type
    PspEntry += s.pack(*EntryData)

  s = struct.Struct('I I')
  if (PspDict['Base'] != None and PspDict['Size'] != None):
    # Put PspDict['Size'] to the reserved field for Binary level replacement usage
    # [0:9] Directory Size in 4K bytes, Max:1K * 4K = 4M
    # [10:13] SpiBlockSize, Max: 16 * 4K = 64K
    # [14:28] Base, [26:12] of Base address, Max support to 64M address
    # [29:30] Address Mode
    if(PspDict['Size'] >= 0x400000):
      log.error ("PSP directory size exceeds max limit 4MB(exclusive).")
      FatalErrorExit()
    Rsvd =  ((PspDict['Size'] >> 12) << 0)
    if (PspDict['SpiBlockSize'] != None):
      if(PspDict['SpiBlockSize'] != 0x10000):#Write 0 if SpiBlockSize = 64K
        Rsvd +=  ((PspDict['SpiBlockSize'] >> 12) << 10)
    if (PspDict['HeaderBase'] != None):
      Rsvd += ((PspDict['Base'] >> 12) << 14)
    if (AddressMode != 0):
      Rsvd += AddressMode << 29
  else:
    Rsvd =  0
  values = (len (PspEntry)//PspEntrySize , Rsvd)
  CrcList = s.pack(*values) + PspEntry
  crc = Fletcher32Crc (CrcList, len (CrcList)//2)
  if (PspDict["Level"] == 1):
    Signature = b"$PSP"
  elif (PspDict["Level"] == 2):
    Signature = b"$PL2"
  else:
    log.error ("Unsupported Level detected.")
    FatalErrorExit ()

  values = (Signature, crc, len (PspEntry)//PspEntrySize , Rsvd)
  s = struct.Struct('4s I I I')
  PspHeader = s.pack(*values)
  PspDir = PspHeader+PspEntry
  return PspDir

def BuildIshHeader (IshDict):
  IshHeaderNoCrcFmt = 'I I I I I I I'
  IshHeaderFmt = 'I I I I I I I I'
  IshHeader = b''
  IshHeaderSize = struct.calcsize(IshHeaderFmt)
  #Generate crc
  crc = 0
  IshData = (IshDict['BootPriority'], IshDict['UpdateRetries'], IshDict['GlitchRetries'], IshDict['Location'], IshDict['PspId'], IshDict['SlotMaxSize'], IshDict['Reserved_1'])
  s = struct.Struct(IshHeaderNoCrcFmt)
  #Pack Type
  IshHeader = s.pack(*IshData)
  CrcList = s.pack(*IshData)
  crc = Fletcher32Crc (CrcList, 14)
  log.error(crc)

  IshData = (crc, IshDict['BootPriority'], IshDict['UpdateRetries'], IshDict['GlitchRetries'], IshDict['Location'], IshDict['PspId'], IshDict['SlotMaxSize'], IshDict['Reserved_1'])
  s = struct.Struct(IshHeaderFmt)
  IshHeader = s.pack(*IshData)
  return IshHeader

def BuildBiosDir (BiosSize, BiosDict):
  AddressMode = BiosDict['AddressMode'];
  BiosEntry=b""
  BiosEntryFmt = 'I I Q Q'
  BiosEntrySize = struct.calcsize(BiosEntryFmt)
  MmioAddress = 0
  # Process each PSP entry
  for Entry in BiosDict['Entries']:
    if (IsImageEntry(Entry) or IsPointEntry(Entry)):
      # typedef struct {
      #   UINT32 Type : 8 ;          ///< Type of BIOS entry
      #   UINT32 RegionType : 8;     ///< 0 Normal memory, 1 TA1 memory, 2 TA2 memor
      #   UINT32 BiosResetImage: 1;  ///< Set for SEC or EL3 fw, which will be authenticate by PSP FW known as HVB
      #   UINT32 Copy: 1;            ///< Copy: 1- copy BIOS image image from source to destination 0- Set region attribute based on <ReadOnly, Source, size> attributes
      #   UINT32 ReadOnly : 1;       ///< 1: Set region to read-only (applicable for ARM- TA1/TA2) 0: Set region to read/write
      #   UINT32 Compressed : 1;     ///< 1: Compresed
      #   UINT32 Instance : 4;       ///< Specify the Instance of an entry
      #   UINT32 SubProgram : 3;       ///< Specify the SubProgram
      #   UINT32 RomId:2;           ///< Specify the RomId
      #   UINT32 Writable:1;         ///< 1: Region writable, 0: Region read only
      #   UINT32 Reserved : 2;      ///< Reserve for future use
      # } TYPE_ATTRIB;
      TypeAttrib = (Entry['Type'] + (Entry['RegionType'] << 8) + (Entry['ResetImage'] << 16)\
                   + (Entry['Copy'] << 17) + (Entry['ReadOnly'] <<18) + (Entry['Compressed'] <<19) + (Entry['Instance'] <<20) + (Entry['SubProgram'] << 24) + (Entry['RomId'] << 27))
      if((Writable == 1) and (IsEntryModifiable(Entry['Type'], 0))):
        TypeAttrib |= 1 << 29
      EntryAddressMode = AddressMode
      if(Entry['AddressMode'] != 0xFF):
        EntryAddressMode = Entry['AddressMode']
      if(AddressMode == 2 and Entry['AbsoluteAddr'] == 1 and IsPointEntry(Entry)):
        EntryAddressMode = 0

      if (EntryAddressMode == 0) :
        # Always Set Source to 0 for APOB entry
        if (Entry['Type'] == 0x61):
          MmioAddress = 0
        else:
          MmioAddress = getMmioAddress (BiosSize, Entry['Address'])
        EntryData = (TypeAttrib, Entry['Size'], MmioAddress, Entry['Destination'])
      elif (EntryAddressMode == 1):
        EntryData = (TypeAttrib, Entry['Size'], Entry['Address'], Entry['Destination'])
        if ((AddressMode == 2) or (AddressMode == 3)):
          EntryData = (TypeAttrib, Entry['Size'], Entry['Address'] + (0x40000000 << 32), Entry['Destination'])
      elif (EntryAddressMode == 2):
        if(IsPointEntry(Entry)):
          if(Entry['Type'] == 0x61):
            EntryData = (TypeAttrib, Entry['Size'], 0, Entry['Destination'])
          else:
            EntryData = (TypeAttrib, Entry['Size'], Entry['Address'] + (0x80000000 << 32), Entry['Destination'])
        else:
          EntryData = (TypeAttrib, Entry['Size'], Entry['Address'] - BiosDict['Base'] + (0x80000000 << 32), Entry['Destination'])
      elif (EntryAddressMode == 3):
        if(IsPointEntry(Entry)):
          if(Entry['Type'] == 0x61):
            EntryData = (TypeAttrib, Entry['Size'], 0, Entry['Destination'])
          else:
            EntryData = (TypeAttrib, Entry['Size'], Entry['Address'] + (0xC0000000 << 32), Entry['Destination'])
        else:
          if(BiosDict['ImageBase'] == None):
            log.error("Please provide ImageBase attribute if you are using AddressMode 3 in a BIOS directory")
            FatalErrorExit()
          else:
            EntryData = (TypeAttrib, Entry['Size'], Entry['Address'] - BiosDict['ImageBase'] + (0xC0000000 << 32), Entry['Destination'])
    elif (IsValueEntry(Entry)):
      # Size field always be 0xFFFFFFFF for Value
      EntryData = (Entry['Type'], 0xFFFFFFFF, Entry['Value'], Entry['Destination'])
    s = struct.Struct(BiosEntryFmt)
    #Pack Type
    BiosEntry += s.pack(*EntryData)

  s = struct.Struct('I I')
  if (BiosDict['Base'] != None and BiosDict['Size'] != None):
    # Put BiosDict['Size'] to the reserved field for Binary level replacement usage
    # [0:9] Directory Size in 4K bytes, Max:1K * 4K = 4M
    # [10:13] SpiBlockSize, Max: 16 * 4K = 64K
    # [14:28] Base, [26:12] of Base address, Max support to 64M address
    # [29:30] Address Mode
    if(BiosDict['Size'] >= 0x400000):
      log.error ("BIOS directory size exceeds max limit 4MB(exclusive).")
      FatalErrorExit()
    Rsvd =  ((BiosDict['Size'] >> 12) << 0)
    if (BiosDict['SpiBlockSize'] != None):
      if(BiosDict['SpiBlockSize'] != 0x10000):#Write 0 if SpiBlockSize = 64K
        Rsvd +=  ((BiosDict['SpiBlockSize'] >> 12) << 10)
    if (BiosDict['HeaderBase'] != None):
      Rsvd += ((BiosDict['Base'] >> 12) << 14)
    if (AddressMode != 0):
      Rsvd += AddressMode << 29
  else:
    Rsvd =  0
  values = (len (BiosEntry)//BiosEntrySize , Rsvd)
  CrcList = s.pack(*values) + BiosEntry
  crc = Fletcher32Crc (CrcList, len (CrcList)//2)
  if (BiosDict["Level"] == 1):
    Signature = b"$BHD"
  elif (BiosDict["Level"] == 2):
    Signature = b"$BL2"
  else:
    log.error ("Unsupport Level detected.")
    FatalErrorExit ()
  values = (Signature, crc, len (BiosEntry)//BiosEntrySize , Rsvd)
  s = struct.Struct('4s I I I')
  BiosHeader = s.pack(*values)
  BiosDir = BiosHeader+BiosEntry
  return BiosDir

def BuildComboDir (BiosSize, ComboDict, isBiosDict):
  AddressMode = ComboDict['AddressMode'];
  ComboEntry=b""
  ComboEntryFmt = 'I I Q'
  ComboEntrySize = struct.calcsize(ComboEntryFmt)
  # Process each Combo entry
  for Entry in ComboDict['Entries']:
    if (AddressMode == 0) :
      EntryData = (Entry['IdSelect'], Entry['Id'], getMmioAddress (BiosSize, Entry['Address']))
    elif (AddressMode == 1):
      EntryData = (Entry['IdSelect'], Entry['Id'], Entry['Address'])
    elif (AddressMode == 2):
      log.error ("AddressMode 2 is not supported in combo dir!")
      FatalErrorExit ()
    s = struct.Struct(ComboEntryFmt)
    #Pack Type
    ComboEntry += s.pack(*EntryData)

  s = struct.Struct('I I 4I')
  LookUpMode =  ComboDict['LookUpMode']
  values = (len (ComboEntry)//ComboEntrySize , LookUpMode, 0, 0, 0, 0)
  CrcList = s.pack(*values) + ComboEntry
  crc = Fletcher32Crc (CrcList, len (CrcList)//2)
  if(isBiosDict == 1):
    values = (b"2BHD", crc, len (ComboEntry)//ComboEntrySize , LookUpMode, 0, 0, 0, 0)
  else:
    values = (b"2PSP", crc, len (ComboEntry)//ComboEntrySize , LookUpMode, 0, 0, 0, 0)
  s = struct.Struct('4s I I I 4I')
  ComboHeader = s.pack(*values)
  ComboDir = ComboHeader+ComboEntry
  return ComboDir

def GetBiosTypeAttribs (TypeAttrib):
  Type = (TypeAttrib & 0xFF) >> 0
  RegionType = (TypeAttrib & 0xFF00) >> 8
  Reset = (TypeAttrib & 0x10000) >> 16
  Copy = (TypeAttrib & 0x20000) >> 17
  ReadOnly = (TypeAttrib & 0x40000) >> 18
  Compressed = (TypeAttrib & 0x80000) >> 19
  Instance = (TypeAttrib & 0xF00000) >> 20
  SubProgram = (TypeAttrib & 0x7000000) >> 24
  RomId = (TypeAttrib & 0x18000000) >> 27
  EntryWritable = (TypeAttrib & 0x20000000) >> 29
  return (Type, RegionType, Reset, Copy, ReadOnly, Compressed, Instance, SubProgram, RomId, EntryWritable)

def ParsePspDirBin (FileHandle, BinarySize, PspDirOffSetInBin, Level, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos, LevelType="", LevelIndex=0):
  global IsABRecovery
  global ProgramStr
  global Writable
  global ProgramTable
  BiosDirOffset = 0
  BiosDirRawSize = 0
  #Verify PSP Directory blob
  #read Psp Header
  PspDirectoryInfo = {}
  FileHandle.seek (AddrGlobalXorBit24 (PspDirOffSetInBin))
  PspDirTblRaw = FileHandle.read (16)
  (Signature, Checksum, NumEntries, Rsvd) = struct.unpack('4I',PspDirTblRaw)
  # [0:9] Directory Size in 4K bytes, Max:1K * 4K = 4M
  # [10:13] SpiBlockSize, Max: 16 * 4K = 64K
  # [14:28] Base, [26:12] of Base address, Max support to 64M address
  # [29:30] AddressMode, Valid: 0, 1, 2
  PspDirSize = ((Rsvd & 0x3FF) >> 0) << 12
  SpiBlockSize = ((Rsvd & 0x3C00) >> 10) << 12
  BaseOffset = ((Rsvd & 0x1FFFC000) >> 14) << 12
  PspDirAddressMode = (Rsvd & 0x60000000) >> 29
  if(SpiBlockSize == 0):
    SpiBlockSize = 0x10000;
  PspDirectoryInfo ['DirSize'] = PspDirSize
  PspDirectoryInfo ['DirOffset'] = PspDirOffSetInBin
  PspDirectoryInfo ['SpiBlockSize'] = SpiBlockSize
  PspDirectoryInfo ['BaseOffset'] = BaseOffset
  PspDirectoryInfo ['Level'] = Level
  PspDirectoryInfo ['AddressMode'] = PspDirAddressMode
  PspDirectoryInfo ['LevelType'] = LevelType
  PspDirectoryInfo ['ImageBase'] = 0

  log.debug ("Psp Directory Header Offset 0x%x Base offset 0x%x FV Size 0x%X SpiBlockSize 0x%X" ,PspDirOffSetInBin, BaseOffset, PspDirSize, SpiBlockSize)

  #Check Signature
  if ((Signature != PSP_DIR_SIGNATURE) and (Signature != PSP_LEVEL2_DIR_SIGNATURE)):
    #Check ISH structure
    if (ProgramTable[ProgramStr]["ISH_STRUCTURE"] == 1):
      #VN structure
      FileHandle.seek (AddrGlobalXorBit24 (PspDirOffSetInBin))
      PspDirTblRaw = FileHandle.read (16)
      (Signature, Checksum, NumEntries, MROffset) = struct.unpack('4I',PspDirTblRaw)
      log.debug ("VN ISH structure: Psp Directory Header Offset 0x%x" ,MROffset)
      ParsePspDirBin (FileHandle, BinarySize, MROffset, 2, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos, LevelType, LevelIndex)
      #Hard code here to skip second PSP directory
      return
    elif (ProgramTable[ProgramStr]["ISH_STRUCTURE"] == 2):
      #RMB structure
      FileHandle.seek (AddrGlobalXorBit24 (PspDirOffSetInBin))
      PspDirTblRaw = FileHandle.read (32)
      (Checksum, BootPriority, UpdateRetries, GlitchRetries, Location, PspId, SlotMaxSize, Reserved_1) = struct.unpack('8I',PspDirTblRaw)
      #ISH Header
      IshHeader = {}
      IshHeader ['Base'] = PspDirOffSetInBin
      IshHeader ['Checksum'] = Checksum
      IshHeader ['BootPriority'] = BootPriority
      IshHeader ['UpdateRetries'] = UpdateRetries
      IshHeader ['GlitchRetries'] = GlitchRetries & 0xFF
      IshHeader ['Location'] = Location
      IshHeader ['PspId'] = PspId
      IshHeader ['SlotMaxSize'] = SlotMaxSize
      IshHeader ['Reserved_1'] = Reserved_1

      #Check PSP Id
      if ((ProgramTable[ProgramStr]["PSPID"] & ProgramTable[ProgramStr]["AndMask"]) != (PspId & ProgramTable[ProgramStr]["AndMask"])):
        return;

      IshHeaderInfos.append(IshHeader)
      log.debug ("RMB ISH structure: Psp Directory Header Offset 0x%x" ,Location)
      ParsePspDirBin (FileHandle, BinarySize, Location, 2, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos, LevelType, LevelIndex)
      return
    else:
      log.error ("Psp Directory Table Signature Verify Fail")
      FatalErrorExit ()
  #To be Done Check CRC Checksum

  PspDirectoryInfos.append (PspDirectoryInfo)
  #Render & build structure
  #Read the whole Dir Table
  EntryInfoArray = []
  PspLevel2Addr = 0
  PspLevel2A_Addr_Array = []
  PspLevel2B_Addr_Array = []
  BiosLevel2AB_Addr = 0
  for i in range (NumEntries):
    EntryRaw = FileHandle.read (16)
    (TypeAttrib, Size, Location) = struct.unpack ('IIQ', EntryRaw)
    Type = (TypeAttrib & 0xFF) >> 0
    Type16 = TypeAttrib & 0xFFFF
    SubProgram = (TypeAttrib & 0xFF00) >> 8
    RomId = (TypeAttrib & 0x30000) >> 16
    EntryWritable = (TypeAttrib & 0x40000) >> 18
    Instance = (TypeAttrib & 0x780000) >> 19
    if(EntryWritable):
      Writable = 1

    AbsoluteAddr = 0
    EntryAddressMode = 0
    if (Size != 0xFFFFFFFF):
      # Type16 specify the 1st 16 bits of Type filed, while Type only specify the lowest 8bits of Type field
      log.debug ("Type16[0x%X] Type[0x%02X] SubProgram[0x%02X] RomId[0x%01X] Instance[0x%01X] Size[0x%08X] Location[0x%08X ~ 0x%08X] (%s)",  Type16, Type, SubProgram, RomId, Instance, Size, Location, Location + Size -1, getEntryName(Type, 1))
      if(PspDirAddressMode == 0):
        #For earlier BIOS, AddressMode is not recorded in directory header. Need further analysis
        if (Location & 0xF0000000):
          #AddressMode = 0
          PspDirEntryAddress = getOffsetInBin(BinarySize, Location & 0xFFFFFFFF)
        else:
          if(((ProgramStr == "ZP") or (ProgramStr == "RV")) and (i == NumEntries - 1)):
            #Hardcode for Harlan
            PspDirEntryAddress = Location & 0xFFFFFFFF
          else:
            #AddressMode = 1
            PspDirAddressMode = 1
            PspDirectoryInfo ['AddressMode'] = PspDirAddressMode
            PspDirEntryAddress = Location & 0xFFFFFFFF
      elif(PspDirAddressMode == 1):
        PspDirEntryAddress = Location & 0xFFFFFFFF
      elif(PspDirAddressMode == 2):
        PspDirectoryInfo ['ImageBase'] = PspDirOffSetInBin
        EntryAddressBit = (Location & (0xC0000000 << 32)) >> 62
        #Check if bit62/bit63 is set
        if (EntryAddressBit == 3):
          #AddressMode = 3
          EntryAddressMode = 3
          #For A/B recovery, AddressMode 3 need to Plus ImageBase, currently ImageBase is equal to PspDirEntryAddress
          PspDirEntryAddress = PspDirOffSetInBin + (Location & 0xFFFFFFFF)
        elif (EntryAddressBit == 2):
          #AddressMode = 2
          EntryAddressMode = 2
          PspDirEntryAddress = PspDirOffSetInBin + (Location & 0xFFFFFFFF)
        elif(EntryAddressBit == 1):
          #AddressMode = 1
          EntryAddressMode = 1
          #Some old BIOS may use this bit to represent AddressMode 3
          PspDirEntryAddress = Location & 0xFFFFFFFF
        else:
          #AddressMode = 0
          PspDirEntryAddress = getOffsetInBin(BinarySize, Location)
          AbsoluteAddr = 1
      else:
        #Error
        log.error ("Unsupported AddressMode!")
        FatalErrorExit ()

      EntryInfo = {'Type16':Type16,'Type':Type, 'SubProgram':SubProgram, 'RomId':RomId, 'Instance':Instance, 'Size':Size,'RTOffset':Location, 'Description':getEntryName(Type & 0xFF, 1), 'Address':PspDirEntryAddress, 'XorAddr':AddrGlobalXorBit24 (PspDirEntryAddress), 'AbsoluteAddr': AbsoluteAddr, 'AddressMode': EntryAddressMode}
      #Update the Description filed with subprogram information, if upper bits of Type filed is non-zero
      if (EntryInfo['Type16'] & 0xFF00):
        if (EntryInfo['Type16'] & 0xFF00 == 0x100):
          EntryInfo['Description'] = EntryInfo['Description'] + " (RV2,PiR,CF,VM,RMB B0)"
        elif (EntryInfo['Type16'] & 0xFF00 == 0x200):
          EntryInfo['Description'] = EntryInfo['Description'] + " (PCO)"
      # Check if it the entry point 2nd level DIR. Assuming no duplicated entry type would be found
      if (EntryInfo['Type'] == PSP_ENTRY_TYPE_LEVEL2_DIR):
        log.debug ("2nd PSP DIR found\n")
        PspLevel2Addr = EntryInfo["Address"]
      if (EntryInfo['Type'] == PSP_ENTRY_TYPE_LEVEL2A_DIR):
        log.debug ("L2A PSP DIR found\n")
        IsABRecovery+=1
        PspLevel2A_Addr_Array.append(EntryInfo["Address"])
      if (EntryInfo['Type'] == PSP_ENTRY_TYPE_LEVEL2B_DIR):
        log.debug ("L2B PSP DIR found\n")
        IsABRecovery+=1
        PspLevel2B_Addr_Array.append(EntryInfo["Address"])
      if (EntryInfo['Type'] == BIOS_ENTRY_TYPE_LEVEL2AB_DIR):
        log.debug ("L2A/B BIOS DIR found\n")
        BiosLevel2AB_Addr = EntryInfo["Address"]
      EntryInfo['Entry'] = getEntryType(PspDirectoryInfo, EntryInfo, 1)
      # Found a unknown type, set it to IMAGE_ENTRY to allow replace the unknown type
      if (EntryInfo['Entry'] == "UNKNOWN_ENTRY"):
        EntryInfo['Entry'] = "IMAGE_ENTRY"
      #Only given File attribute for IMAGE_ENTRY
      if (GetOutEntryFileName (EntryInfo, "PspDir", Level, 1, LevelType, LevelIndex) != "Unsupported"):
        EntryInfo ['File'] = GetOutEntryFileName (EntryInfo, "PspDir", Level, 1, LevelType, LevelIndex)
        log.debug(LevelIndex)
    else:
      log.debug ("Type [0x%02X] VALUE [0x%08X] (%s)",  Type, Location, getEntryName(Type, 1))
      #Ignore Size in Value entry
      EntryInfo = {'Type':Type,'Entry':'VALUE_ENTRY','Value':Location, 'Description':getEntryName(Type, 1)}
    EntryInfoArray.append (EntryInfo)
  #Parse entry point level 2 directory
  if (PspLevel2Addr > 0):
    ParsePspDirBin (FileHandle, BinarySize, PspLevel2Addr, 2, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos)
  L2AIndex = 0
  for PspLevel2A_Addr in PspLevel2A_Addr_Array:
    ParsePspDirBin (FileHandle, BinarySize, PspLevel2A_Addr, 2, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos, "A", L2AIndex)
    L2AIndex = L2AIndex + 1
    #Only parse the first L2A Dir for now
    # break
  L2BIndex = 0
  for PspLevel2B_Addr in PspLevel2B_Addr_Array:
    SameABAddr = 0
    for PspLevel2A_Addr in PspLevel2A_Addr_Array:
      if (PspLevel2B_Addr == PspLevel2A_Addr):
        SameABAddr = 1
        print ("Found L2B has same address with L2A. Skip L2B...")
        break
    if (SameABAddr == 0):
      ParsePspDirBin (FileHandle, BinarySize, PspLevel2B_Addr, 2, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos, "B", L2BIndex)
      L2BIndex = L2BIndex + 1
  if (BiosLevel2AB_Addr > 0):
    ParseBiosDirBin (FileHandle, BinarySize, BiosLevel2AB_Addr, 2, BiosDirectoryInfos, LevelType, PspDirOffSetInBin, LevelIndex)
  PspDirectoryInfo['Entries'] = EntryInfoArray


def ParseBiosDirBin (FileHandle, BinarySize, BiosDirOffset, Level, BiosDirectoryInfos, LevelType="", ImageBase = 0, LevelIndex=0):
  '''
      Build BIOS Dir structure
  '''
  global ProgramTable
  global ProgramStr
  global Writable
  BiosFwId = 0
  BiosDirectoryInfo = {}
  BiosDirectoryInfos.append (BiosDirectoryInfo)
  FileHandle.seek (AddrGlobalXorBit24 (BiosDirOffset))
  BiosDirTblRaw = FileHandle.read (16)
  (Signature, Checksum, NumEntries, Rsvd) = struct.unpack('4I',BiosDirTblRaw)

  #First check if this is a combo header
  if(Signature == BIOS_COMBO_SIGNATURE):
    log.debug ("BIOS Combo Directory Found")
    EntryFound = 0
    #loop the PSP Combo Directory to found entry for specific program
    #Ignore 16 bytes reserved
    FileHandle.read (16)
    for i in range (NumEntries):
      EntryRaw = FileHandle.read (16)
      (IdSelect, Id, BiosDirOffset) = struct.unpack ('IIQ', EntryRaw)
      #convert to raw binary offset
      PspDirAddressMode = 1
      if (BiosDirOffset & 0xF0000000):
        # Check if Pysical MMIO address or binary offset
        BiosDirOffset = getOffsetInBin (BinarySize, BiosDirOffset)
        PspDirAddressMode = 0
      else:
        BiosDirOffset = BiosDirOffset

      #Check if match the input program
      if ((ProgramTable[ProgramStr]["PSPID"] & ProgramTable[ProgramStr]["AndMask"]) == (Id & ProgramTable[ProgramStr]["AndMask"])):
        EntryFound = 1
        FileHandle.seek (AddrGlobalXorBit24 (BiosDirOffset))
        BiosDirTblRaw = FileHandle.read (16)
        (Signature, Checksum, NumEntries, Rsvd) = struct.unpack('4I',BiosDirTblRaw)
        break
    if(EntryFound == 0):
      #Error
      log.error ("No matching program found!")
      FatalErrorExit ()

  # [0:9] Directory Size in 4K bytes, Max:1K * 4K = 4M
  # [10:13] SpiBlockSize, Max: 16 * 4K = 64K
  # [14:28] Base, [26:12] of Base address, Max support to 64M address
  # [29:30] AddressMode, Valid: 0, 1, 2
  BiosDirSize = ((Rsvd & 0x3FF) >> 0) << 12
  SpiBlockSize = ((Rsvd & 0x3C00) >> 10) << 12
  BaseOffset = ((Rsvd & 0x1FFFC000) >> 14) << 12
  BiosDirAddressMode = (Rsvd & 0x60000000) >> 29
  if(SpiBlockSize == 0):
    SpiBlockSize = 0x10000;
  BiosDirectoryInfo ['DirOffset'] = BiosDirOffset
  BiosDirectoryInfo ['DirSize'] = BiosDirSize
  BiosDirectoryInfo ['SpiBlockSize'] = SpiBlockSize
  BiosDirectoryInfo ['BaseOffset'] = BaseOffset
  BiosDirectoryInfo ['Level'] = Level
  BiosDirectoryInfo ['AddressMode'] = BiosDirAddressMode
  BiosDirectoryInfo ['LevelType'] = LevelType
  BiosDirectoryInfo ['ImageBase'] = 0
  log.debug ("BIOS Directory Header Offset 0x%x Base offset 0x%x Size 0x%X SpiBlockSize 0x%X" ,BiosDirOffset, BaseOffset, BiosDirSize, SpiBlockSize)
  #Check Signature
  if ((Signature != BIOS_DIR_SIGNATURE) and (Signature != BIOS_LEVEL2_DIR_SIGNATURE)):
    log.error ("BIOS Directory Table Signature Verify Fail")
    FatalErrorExit ()

  EntryInfoArray = []
  BiosLevel2Addr = 0
  FileNameDict = {}
  FileName = ''
  for i in range (NumEntries):
    EntryRaw = FileHandle.read (24)
    (TypeAttrib, Size, Location, Destination) = struct.unpack ('IIQQ', EntryRaw)
    AbsoluteAddr = 0
    EntryAddressMode = 0xFF
    if (Size != 0xFFFFFFFF):
      (Type, RegionType, ResetImage, Copy, ReadOnly, Compressed, Instance, SubProgram, RomId, EntryWritable)= GetBiosTypeAttribs (TypeAttrib)
      log.debug ("Type[%02X] RomId[%02X] SubProgram[%02x] Instance[%02X] RegionType[%02X] Reset[%01x] Copy[%01x] RO[%01x] ZIP[%01x]\n>>Size [0x%08X] Source [0x%08X ~ 0x%08X] Dest[0x%08x] (%s)",\
      Type, RomId, SubProgram, Instance, RegionType, ResetImage, Copy, ReadOnly, Compressed,\
      Size, Location, Location + Size -1, Destination, getEntryName(Type, 0))
      if(EntryWritable):
        Writable = 1

      if(BiosDirAddressMode == 0):
        #For earlier BIOS, AddressMode is not recorded in directory header. Need further analysis
        if (Location & 0xF0000000):
          #AddressMode = 0
          BiosDirEntryAddress = getOffsetInBin(BinarySize, Location & 0xFFFFFFFF)
          log.debug(BiosDirEntryAddress)
        else:
          if (Type != 0x61):
            #AddressMode = 1
            BiosDirAddressMode = 1
            BiosDirectoryInfo ['AddressMode'] = BiosDirAddressMode
            BiosDirEntryAddress = Location & 0xFFFFFFFF
      elif(BiosDirAddressMode == 1):
        BiosDirEntryAddress = Location & 0xFFFFFFFF
      elif((BiosDirAddressMode == 2) or (BiosDirAddressMode == 3)):
        BiosDirectoryInfo ['ImageBase'] = ImageBase
        EntryAddressBit = (Location & (0xC0000000 << 32)) >> 62
        #Check if bit62/bit63 is set
        if(EntryAddressBit == 3):
          #AddressMode = 3
          EntryAddressMode = 3
          #For A/B recovery, AddressMode 3 need to Plus ImageBase
          BiosDirEntryAddress = (Location & 0xFFFFFFFF) + ImageBase
        elif (EntryAddressBit == 2):
          #AddressMode = 2
          EntryAddressMode = 2
          BiosDirEntryAddress = BiosDirOffset + (Location & 0xFFFFFFFF)
        elif(EntryAddressBit == 1):
          #AddressMode = 1
          EntryAddressMode = 1
          BiosDirEntryAddress = Location & 0xFFFFFFFF
        else:
          #AddressMode = 0
          BiosDirEntryAddress = getOffsetInBin(BinarySize, Location)
          AbsoluteAddr = 1
      else:
        #Error
        log.error ("Unsupported AddressMode!")
        FatalErrorExit ()

      EntryInfo = {'Type':Type, 'RomId':RomId, 'SubProgram':SubProgram, 'Instance':Instance, 'RegionType':RegionType, 'ResetImage':ResetImage, 'Copy': Copy, 'ReadOnly': ReadOnly, 'Compressed':Compressed, \
      'Size':Size,'RTOffset':Location, 'Description':getEntryName(Type, 0), \
      'Address':BiosDirEntryAddress, 'Destination':Destination, 'XorAddr': AddrGlobalXorBit24 (BiosDirEntryAddress), 'AbsoluteAddr': AbsoluteAddr, 'AddressMode': EntryAddressMode}
      EntryInfo['Entry'] = getEntryType(BiosDirectoryInfo, EntryInfo, 0);
      # Check if it the entry point 2nd level DIR
      if (EntryInfo['Type'] == BIOS_ENTRY_TYPE_LEVEL2_DIR):
        log.debug ("2nd BIOS DIR found\n")
        #It is the entry point level 2 directory
        BiosLevel2Addr = EntryInfo["Address"]
      # Found a unknown type, set it to IMAGE_ENTRY to allow replace the unknown type
      if (EntryInfo['Entry'] == "UNKNOWN_ENTRY"):
        EntryInfo['Entry'] = "IMAGE_ENTRY"
      #Only given File attribute for IMAGE_ENTRY
      if (GetOutEntryFileBaseName (EntryInfo, "BiosDir", Level, 0, LevelType, LevelIndex) != "Unsupported"):
        FileName = GetOutEntryFileBaseName (EntryInfo, "BiosDir", Level, 0, LevelType, LevelIndex)
        #Check if duplicated file name
        if (FileName in FileNameDict):
          FileIndex = FileNameDict[FileName]
          FileNameDict[FileName] += 1
          FileName += ("_" + "%d"%FileIndex)
        else:
          FileNameDict[FileName] = 1
        #add file surfix
        EntryInfo ['File'] = FileName + ".bin"

      #Always return physical address for APOB, and should be ZERO
      if (EntryInfo['Type'] == 0x61):
        EntryInfo['Address'] = getOffsetInBin(0, Location)
    else:
      log.debug ("Type [0x%02X] VALUE [0x%08X] (%s)",  Type, Location, getEntryName(Type, 0))
      #Ignore size,Destination in Value entry
      EntryInfo = {'Type':Type, 'Entry':'VALUE_ENTRY','Value':Location, 'Description':getEntryName(Type, 0)}

    EntryInfoArray.append (EntryInfo)
  #Parse entry point level 2 directory
  if (BiosLevel2Addr > 0):
    ParseBiosDirBin (FileHandle, BinarySize, BiosLevel2Addr, 2, BiosDirectoryInfos)
  BiosDirectoryInfo['Entries'] = EntryInfoArray

def GetPspDirectory (BinaryFile):
  global AddressMode
  global ProgramTable
  global ProgramStr
  global gXorBit24
  global IsABRecovery
  PspDirectoryInfos = []
  BiosDirectoryInfos = []
  IshHeaderInfos = []
  BinaryInfo = {}
  try:
    FileHandle = open (BinaryFile,'rb')
  except:
    log.error ("Error: Opening {}".format(BinaryFile))
    FatalErrorExit ()
  #read whole binary to buffer
  FileHandle.seek (0)
  BinarySize = os.path.getsize(BinaryFile)
  BinaryInfo ['Binary'] = FileHandle.read (BinarySize)
  BinaryInfo ['BinaryName'] = BinaryFile
  BinaryInfo ['BinarySize'] = BinarySize

  ######################################
  # 1st try the Traditional ROMSIG way
  ######################################
  # Check Signature
  # log.debug ("2 try the Traditional ROMSIG way")

  #
  gXorBit24 = 0
  for i in range(2):
    # Try upper 16M if can't find in low 16M
    if (i == 1):
      gXorBit24 = 1
    for RomSigOffset in RomSigAddrTable:
      RomSigOffset = AddrGlobalXorBit24 (RomSigOffset)
      if (RomSigOffset > BinarySize):
        continue
      log.debug ("Check ROMSIG @ 0x%X" ,RomSigOffset)
      FileHandle.seek (RomSigOffset)
      OemSigRaw = FileHandle.read (0x30)
      OemSigArray = struct.unpack ('12I',OemSigRaw)
      #Check OEM signature, ang get Psp Directory offset
      if (OemSigArray[ROMSIG_SIG_OFFSET] == 0x55aa55aa):
        log.debug ("Oem Sig Table Found")
        # if the EFS gen match the program
        if BinarySize > 0x1000000:
          if ((OemSigArray[SECOND_GEN_EFS_OFFSET] & 0x1) != ProgramTable[ProgramStr]["SECOND_GEN_EFS"] ):
            log.debug ("SECOND_GEN_EFS %x not match %x continue search", (OemSigArray[SECOND_GEN_EFS_OFFSET] & 0x1) , ProgramTable[ProgramStr]["SECOND_GEN_EFS"])
            continue
        PspDirOffset = OemSigArray[ROMSIG_PSPDIR_OFFSET]
        log.debug ("Psp Directory Offset 0x%X" ,PspDirOffset)
        #convert to raw binary offset
        PspDirAddressMode = 1
        if (PspDirOffset & 0xF0000000):
          # Check if Pysical MMIO address or binary offset
          PspDirOffset = getOffsetInBin (BinarySize, PspDirOffset)
          PspDirAddressMode = 0
        else:
          PspDirOffset = PspDirOffset

        log.debug ("Psp Directory Offset in binary 0x%X" ,PspDirOffset)
        FileHandle.seek (AddrGlobalXorBit24 (PspDirOffset))
        PspDirTblRaw = FileHandle.read (16)
        (Signature, Checksum, NumEntries, PspDirSize) = struct.unpack('4I',PspDirTblRaw)
        if (Signature == PSP_DIR_SIGNATURE):
          log.debug ("PSP Directory Found")
          #We found a Valid PSP Directory Header through ROM SIG
          ParsePspDirBin (FileHandle, BinarySize, PspDirOffset, 1, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos)
          BiosDirOffset = OemSigArray[ProgramTable[ProgramStr]["BIOSDIR_OFFSET"]]
          if ((IsABRecovery < 2) and (BiosDirOffset != 0)):
            log.debug ("BIOS Directory Offset found at ROMSIG[%x]0x%X" ,ProgramTable[ProgramStr]["BIOSDIR_OFFSET"], BiosDirOffset)
            BiosDirAddressMode = 1
            #convert to raw binary offset
            if (BiosDirOffset & 0xF0000000):
              # Check if Pysical MMIO address or binary offset
              BiosDirOffset = getOffsetInBin (BinarySize, BiosDirOffset)
              BiosDirAddressMode = 0
            else:
              BiosDirOffset =  BiosDirOffset

            ParseBiosDirBin (FileHandle, BinarySize, BiosDirOffset, 1, BiosDirectoryInfos)
            #######################################################
            # Verify to make sure all element use same addressmode
            #######################################################
            PreEntryAddressMode = 0xFFFF
            for PspDirectoryInfo in PspDirectoryInfos:
              PspDirEntryAddressMode = PspDirectoryInfo['AddressMode']
              if ((PreEntryAddressMode != 0xFFFF) and (PreEntryAddressMode != PspDirEntryAddressMode)):
                log.warn ("Warning: Address mode is not aligned between PSP Directories\n")
                # FatalErrorExit ()
              PreEntryAddressMode = PspDirectoryInfo['AddressMode']

            PreEntryAddressMode = 0xFFFF
            for BiosDirectoryInfo in BiosDirectoryInfos:
              BiosDirEntryAddressMode = BiosDirectoryInfo['AddressMode']
              if ((PreEntryAddressMode != 0xFFFF) and (PreEntryAddressMode != BiosDirEntryAddressMode)):
                log.warn ("Warning: Address mode is not aligned between Bios Directories\n")
                # FatalErrorExit ()
              PreEntryAddressMode = BiosDirectoryInfo['AddressMode']

            if ((PspDirAddressMode == BiosDirAddressMode) and (BiosDirAddressMode == PspDirEntryAddressMode) and (PspDirEntryAddressMode == BiosDirEntryAddressMode)):
              AddressMode = PspDirAddressMode
            else:
              log.warn ("Warning: Address mode is not aligned between EFS/ROMSIG & Bios Directories\n")
              # FatalErrorExit ()

          # Oem Sig Table contains two PSP L1 pointer starting from RMB
          # The two L1 directory has the same content. Do not show duplicate directories.
          # if (ProgramTable[ProgramStr]["ISH_STRUCTURE"] == 2):
            # PspDirOffset = OemSigArray[ROMSIG_PSPDIR_RECOVERY_OFFSET]
            # log.debug ("Psp Directory Offset 0x%X" ,PspDirOffset)
            # ParsePspDirBin (FileHandle, BinarySize, PspDirOffset, 1, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos)
          return (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos)
        elif (Signature == PSP_COMBO_SIGNATURE):
          log.debug ("PSP Combo Directory Found")
          #loop the PSP Combo Directory to found entry for specific program
          EntryFound = 0
          FileHandle.seek (AddrGlobalXorBit24 (PspDirOffset))
          PspDirTblRaw = FileHandle.read (16)
          (Signature, Checksum, NumEntries, LookUpMode) = struct.unpack('4I',PspDirTblRaw)
          #Ignore 16 bytes reserved
          FileHandle.read (16)
          for i in range (NumEntries):
            EntryRaw = FileHandle.read (16)
            (IdSelect, Id, PspDirOffset) = struct.unpack ('IIQ', EntryRaw)
            #convert to raw binary offset
            PspDirAddressMode = 1
            if (PspDirOffset & 0xF0000000):
              # Check if Pysical MMIO address or binary offset
              PspDirOffset = getOffsetInBin (BinarySize, PspDirOffset)
              PspDirAddressMode = 0
            else:
              PspDirOffset = PspDirOffset

            #Check if match the input program
            if ((ProgramTable[ProgramStr]["PSPID"] & ProgramTable[ProgramStr]["AndMask"]) == (Id & ProgramTable[ProgramStr]["AndMask"])):
              EntryFound = 1
              break

          if (EntryFound == 1):
            log.debug ("PSP Directory for %s Found @ %x", ProgramStr, PspDirOffset)
            ParsePspDirBin (FileHandle, BinarySize, PspDirOffset, 1, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos)
            BiosDirOffset = OemSigArray[ProgramTable[ProgramStr]["BIOSDIR_OFFSET"]]
            if ((IsABRecovery < 2) and (BiosDirOffset != 0)):
              log.debug ("BIOS Directory Offset found at ROMSIG[%x]0x%X" ,ProgramTable[ProgramStr]["BIOSDIR_OFFSET"], BiosDirOffset)
              BiosDirAddressMode = 1
              #convert to raw binary offset
              if (BiosDirOffset & 0xF0000000):
                # Check if Pysical MMIO address or binary offset
                BiosDirOffset = getOffsetInBin (BinarySize, BiosDirOffset)
                BiosDirAddressMode = 0
              else:
                BiosDirOffset = BiosDirOffset

              ParseBiosDirBin (FileHandle, BinarySize, BiosDirOffset, 1, BiosDirectoryInfos)

              #######################################################
              # Verify to make sure all element use same addressmode
              #######################################################
              PreEntryAddressMode = 0xFFFF
              for PspDirectoryInfo in PspDirectoryInfos:
                PspDirEntryAddressMode = PspDirectoryInfo['AddressMode']
                if ((PreEntryAddressMode != 0xFFFF) and (PreEntryAddressMode != PspDirEntryAddressMode)):
                  log.warn ("Warning: Address mode is not aligned between PSP Directories\n")
                  # FatalErrorExit ()
                PreEntryAddressMode = PspDirectoryInfo['AddressMode']

              PreEntryAddressMode = 0xFFFF
              for BiosDirectoryInfo in BiosDirectoryInfos:
                BiosDirEntryAddressMode = BiosDirectoryInfo['AddressMode']
                if ((PreEntryAddressMode != 0xFFFF) and (PreEntryAddressMode != BiosDirEntryAddressMode)):
                  log.warn ("Warning: Address mode is not aligned between Bios Directories\n")
                  # FatalErrorExit ()
                PreEntryAddressMode = BiosDirectoryInfo['AddressMode']

              if ((PspDirAddressMode == BiosDirAddressMode) and (BiosDirAddressMode == PspDirEntryAddressMode) and (PspDirEntryAddressMode == BiosDirEntryAddressMode)):
                AddressMode = PspDirAddressMode
              else:
                log.warn ("Warning: Address mode is not aligned between EFS_PSP_Dir %x,EFS_BIOS_Dir %x,PSP Directories %x, BIOS Directories %x\n", PspDirAddressMode, BiosDirAddressMode, PspDirEntryAddressMode, BiosDirEntryAddressMode)
                # FatalErrorExit ()
            return (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos)

  # # Check BIOS SPI Image layout Or PSP SPI image layout
  # ######################################
  # # 2nd check if  PSP Image  # $PSP
  # ######################################
  FileHandle.seek (0)
  PspDirTblRaw = FileHandle.read (16)
  (Signature, Checksum, NumEntries, PspDirSize) = struct.unpack('4I',PspDirTblRaw)
  log.debug ("2 check if PSP Image")
  if (Signature == PSP_DIR_SIGNATURE):
    # Parse PSP DIR Header
    # Binary Size set 0, implies it is eMMC image, no need to do the address translation
    log.debug ("PSP Image identified")
    AddressMode = 1
    gXorBit24 = 0
    ParsePspDirBin (FileHandle, 0, 0, 1, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos)
    return (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos)

def OutPutPspBinaries (bios, Entries, Level, LevelType):
  for Entry in Entries:
    outputFileName = GetOutEntryFileBaseName (Entry, "PspDir", Level, 1, LevelType)
    if (outputFileName != "Unsupported"):
      outputFileName = outputFileName + ".bin"
      if(IsImageEntry(Entry)):
        outputFileName = Entry['File']
      #slice the binary
      print ("Output %s of BIOS [%x:%x]" %(outputFileName, Entry['XorAddr'], Entry['XorAddr'] + Entry ['Size'] - 1))
      buffer = bios [Entry['XorAddr']: Entry['XorAddr'] + Entry ['Size']]
      WriteBinaryFile (outputFileName, buffer)

def OutPutBiosBinaries (bios, Entries, Level, LevelType):
  for Entry in Entries:
    outputFileName = GetOutEntryFileBaseName (Entry, "BiosDir", Level, 0, LevelType)
    if (outputFileName != "Unsupported"):
      outputFileName = outputFileName + ".bin"
      if(IsImageEntry(Entry)):
        outputFileName = Entry['File']
      #slice the binary
      print ("Output %s of BIOS [%x:%x]" %(outputFileName, Entry['XorAddr'], Entry['XorAddr'] + Entry ['Size'] - 1))
      if(IsResetImageEntry(Entry) and Entry['Compressed'] == 0x1):
        ZlibSizeOffset = 0x14
        ZlibSizeByte = 0x4
        ZlibHeaderSize = 0x100
        ActualBiosSize = struct.unpack ('I',bios [Entry['XorAddr'] + ZlibSizeOffset : Entry['XorAddr'] + ZlibSizeOffset + ZlibSizeByte])

        print (ActualBiosSize[0])
        buffer = bios [Entry['XorAddr']: Entry['XorAddr'] + ActualBiosSize[0] + ZlibHeaderSize]
      else:
        buffer = bios [Entry['XorAddr']: Entry['XorAddr'] + Entry ['Size']]
      WriteBinaryFile (outputFileName, buffer)

def DefaultFwInfo (bios, entry):
  defaultHeaderSize = 0x100
  verOffset = 0x60
  binaryInfo = {}
  binaryHeader = bios [entry['XorAddr']:entry['XorAddr']+defaultHeaderSize]
  pspVer = struct.unpack ('I',binaryHeader[verOffset:verOffset+4])
  verStr ="%X.%X.%X.%X" %(((pspVer[0] & 0xFF000000)>>24), ((pspVer[0] & 0xFF0000) >> 16),((pspVer[0] & 0xFF00) >> 8), ((pspVer[0] & 0xFF) >> 0))
  binaryInfo['Version'] = verStr
  return binaryInfo

def pspBootLoaderInfo (bios, entry):
  pspBLHeaderSize = 0x100
  verOffset = 0x60
  pspBLInfo = {}
  pspBLHeader = bios [entry['XorAddr']:entry['XorAddr']+pspBLHeaderSize]
  pspVer = struct.unpack ('I',pspBLHeader[verOffset:verOffset+4])
  verStr ="%X.%X.%X.%X" %(((pspVer[0] & 0xFF000000)>>24), ((pspVer[0] & 0xFF0000) >> 16),((pspVer[0] & 0xFF00) >> 8), ((pspVer[0] & 0xFF) >> 0))
  pspBLInfo['Version'] = verStr
  return pspBLInfo


def pspSecureOsInfo (bios, entry):
  pspOSHeaderSize = 0x100
  verOffset = 0x60
  pspOSInfo = {}
  pspOSHeader = bios [entry['XorAddr']:entry['XorAddr']+pspOSHeaderSize]
  pspVer = struct.unpack ('I',pspOSHeader[verOffset:verOffset+4])
  verStr ="%X.%X.%X.%X" %(((pspVer[0] & 0xFF000000)>>24), ((pspVer[0] & 0xFF0000) >> 16),((pspVer[0] & 0xFF00) >> 8), ((pspVer[0] & 0xFF) >> 0))
  pspOSInfo['Version'] = verStr
  return pspOSInfo

def KvmInfo (bios, entry):
  KvmHeaderSize = 0x100
  verOffset = 0x60
  KvmInfo = {}
  KvmHeader = bios [entry['XorAddr']:entry['XorAddr']+KvmHeaderSize]
  KvmVer = struct.unpack ('I',KvmHeader[verOffset:verOffset+4])
  verStr ="%d.%d.%d" %(((KvmVer[0] & 0xFF) >> 0), ((KvmVer[0] & 0xFF00) >> 8), ((KvmVer[0] & 0xFFFF0000) >> 16), )
  KvmInfo['Version'] = verStr
  return KvmInfo

def smuFwInfo (bios, entry):
  SmuHeaderSize = 0x100
  verOffset = 0x0
  SmuInfo = {}
  SmuHeader = bios [entry['XorAddr']:entry['XorAddr']+SmuHeaderSize]
  SmuBin_00 = struct.unpack ('I',SmuHeader[0x60:0x60+4])
  # Check if it is old SMU version format, ZP, PR
  if ((SmuBin_00[0x0] & 0x00FF0000) != 0):
    verOffset = 0x60

  SmuVer = struct.unpack ('I',SmuHeader[verOffset:verOffset+4])
  # Check the format x.x.x.x or X.X.X
  if (SmuVer[0] & 0xFF000000):
    # format x.x.x.x
    verStr ="%d.%d.%d.%d" %(((SmuVer[0] & 0xFF000000)>>24), ((SmuVer[0] & 0x00FF0000)>>16), ((SmuVer[0] & 0xFF00) >> 8),((SmuVer[0] & 0x00FF) >> 0))
  else:
    # format x.x.x
    verStr ="%d.%d.%d" %(((SmuVer[0] & 0xFFFF0000)>>16), ((SmuVer[0] & 0xFF00) >> 8),((SmuVer[0] & 0x00FF) >> 0))
  SmuInfo['Version'] = verStr
  return SmuInfo

def DxioFwInfo (bios, entry):
  DxioHeaderSize = 0x100
  verOffset = 0x60
  DxioInfo = {}
  DxioHeader = bios [entry['XorAddr']:entry['XorAddr']+DxioHeaderSize]
  DxioVer = struct.unpack ('I',DxioHeader[verOffset:verOffset+4])
  verStr ="%d.%d.%d" %(((DxioVer[0] & 0xFF00) >> 8), ((DxioVer[0] & 0xFF) >> 0), ((DxioVer[0] & 0xFFFF0000) >> 16))
  DxioInfo['Version'] = verStr
  return DxioInfo

def ucodeInfo (bios, entry):
  HeaderSize = 0x30
  ucodeInfo = {}
  ucodeHeader = bios [entry['XorAddr']:entry['XorAddr']+HeaderSize]
  DateCode = struct.unpack ('I',ucodeHeader[0:4])
  PatchId = struct.unpack ('I',ucodeHeader[4:8])
  ProcessorRevisionID = struct.unpack ('H',ucodeHeader[24:26])
  ucodeInfo['DateCode'] = "%x/%x/%x" %(((DateCode[0] & 0xFF000000) >> 24),((DateCode[0] & 0xFF0000) >> 16), ((DateCode[0] & 0xFFFF) >> 0))
  ucodeInfo['PatchId'] = "%x" %(PatchId[0])
  ucodeInfo['EquivalentProcessorRevisionID'] = "%x" %(ProcessorRevisionID[0])
  return ucodeInfo

def ablInfo (bios, entry):
  ablHeaderSize = 0x110
  verOffset = 0x60
  abl0OffsetOffset = 0x104
  ablInfo = {}
  ablHeader = bios [entry['XorAddr']:entry['XorAddr']+ablHeaderSize]
  ablVer = struct.unpack ('I',ablHeader[verOffset:verOffset+4])
  if (ablVer[0] > 0):
    #Old version
    verStr ="%08x" %(ablVer[0])
    ablInfo['Version'] = verStr
    return ablInfo
  else:
    #New version
    abl0Offset = struct.unpack ('I',ablHeader[abl0OffsetOffset:abl0OffsetOffset+4])
    abl0OffsetNum = abl0Offset[0]
    if(abl0OffsetNum >= entry['Size']):
      ablInfo['Version'] = "00000000"
      return ablInfo
    abl0Header = bios [entry['XorAddr']+abl0OffsetNum:entry['XorAddr']+abl0OffsetNum+ablHeaderSize]
    ablVer = struct.unpack ('I',abl0Header[verOffset:verOffset+4])
    verStr ="%08x" %(ablVer[0])
    ablInfo['Version'] = verStr
    return ablInfo

def version_data_from_header_buffer (buffer, version_format):
  if len (buffer) < version_format['header_size']:
    return 'Unknown'
  version_data = struct.unpack_from(version_format['unpack_format'],
                        buffer, version_format['version_offset'])
  return version_data

def version_string_from_header_buffer (buffer, version_format):
  version_data = version_data_from_header_buffer (buffer, version_format)
  if type (version_data) == str:
    return version_data
  version = version_format['print_format'].format(*version_data)
  return version

def header_from_fw_file (filename, version_format):
  with open(filename, 'rb') as f:
    header = f.read(version_format['header_size'])
  return header

def version_from_fw_file (filename, version_format):
  buffer = header_from_fw_file (filename, version_format)
  version = version_string_from_header_buffer (buffer, version_format)
  return version

def version_smu_from_dir_Entry (Entry, directory=None):
  # Check if new version type is valid
  # Make copy so it can be modified
  version_format = version_info_dir['smu'].copy()
  header = header_from_fw_file (Entry['File'], version_format)
  version_data = version_data_from_header_buffer(header, version_format)
  # If new type is not valid, select old format
  if version_data[2] == 0:
    version_format['version_offset'] = 0
 # Check for Version format
  version_data = version_data_from_header_buffer(header, version_format)
  if version_data[3] == 0:
    version_format['print_format'] = "{2:d}.{1:d}.{0:d}"
  version = version_from_fw_file (Entry['File'], version_format)
  return version

def version_abl_from_dir_Entry (Entry, directory=None):
  # Get Old version and return it if not 0
  version_format = version_info_dir['abl_base']
  version = version_from_fw_file (Entry['File'], version_format)
  if (version != '00000000'):
    return version
  # Get New version offset and adjust version_format in copy
  version_format = version_info_dir['abl_version_location'].copy ()
  abl_location_header = header_from_fw_file (Entry['File'], version_format)
  version_location = version_data_from_header_buffer (abl_location_header, version_format)
  version_location = version_location[0]
  version_format = version_info_dir['abl_base']
  version_format['header_size'] += version_location
  version_format['version_offset'] += version_location
  # Get New ABL version
  version = version_from_fw_file (Entry['File'], version_format)
  return version

def version_from_dir_Entry (Entry, directory=None):
  if directory not in ('psp', 'bios'):
    raise ValueError('directory must be one of "psp, bios"')
  key = (directory, Entry['Type'])
  if key in version_info_dir.keys():
    version_format = version_info_dir[key]
  else:
    version_format = version_info_dir['Default']
    #version = 'N/A'
    #return version
  while 'redirect' in version_format.keys ():
    version_format = version_info_dir[version_format['redirect']]
  if 'function' in version_format.keys ():
    version = eval(version_format['function']) (Entry, directory)
    return version
  if Entry['File'].endswith ('bin'):
    version = version_from_fw_file (Entry['File'], version_format)
  else:
    version = 'Unknown'
  return version

def OutPutPspVersionTxt (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos, xmlFileName):
  try:
    FileHandle = open (xmlFileName,'w')
  except:
    log.error("Error: Opening {}".format(xmlFileName))
    FatalErrorExit ()
  root = Element('Dirs')

  if (len (PspDirectoryInfos) != 0):
    for PspDirectoryInfo in PspDirectoryInfos:
      FileHandle.write("PSP Directory Level {0}:\n".format(PspDirectoryInfo ['Level']));
      FileHandle.write("RomId SubProgram Type  ShortName            Version\n");
      for entry in PspDirectoryInfo['Entries']:
        version = "null"
        shortName = ""
        if (entry['Type'] in PspDirectoryEntryName):
            shortName = PspDirectoryEntryName[entry['Type']][2]
        #Get Entry ShortName from PspDirectoryEntryName
        #                          Type:   DIR, Entry Type    ShortName    Full description
        #                            0x00: ['PSP' ,'IMAGE_ENTRY','AmdPubKey','AMD public Key', ],
        if (entry['Type'] == 1) or (entry['Type'] == 3):
          version = pspBootLoaderInfo(BinaryInfo['Binary'], entry)
          version = version['Version']
        elif (entry['Type'] == 2):
          version = pspSecureOsInfo(BinaryInfo['Binary'], entry)
          version = version['Version']
        elif (entry['Type'] == 8) or (entry['Type'] == 0x12) or (entry['Type'] == 0x2A):
          version = smuFwInfo(BinaryInfo['Binary'], entry)
          version = version['Version']
        elif ((entry['Type'] >= 0x30) and (entry['Type'] <= 0x37)):
          version = ablInfo(BinaryInfo['Binary'], entry)
          version = version['Version']
        elif (entry['Type'] == 0x29):
          version = KvmInfo(BinaryInfo['Binary'], entry)
          version = version['Version']
        elif (entry['Type'] == 0xb):
          version = "null"
        elif(IsImageEntry(entry) and entry['Size']>0x100): #Get default version
          version = DefaultFwInfo(BinaryInfo['Binary'], entry)
          version = version['Version']
        subProgram = 0;
        entryType16 = entry['Type']
        if 'Type16' in entry:
          entryType16 = entry['Type16']
        if (entryType16 > 0x100):
          subProgram = int(entryType16 / 0x100)
        RomId = 0
        if 'RomId' in entry:
          RomId = entry['RomId']
        FileHandle.write("0x{0:0>2X}  0x{1:0>2X}       0x{2:0>2X}  {3:<20s} {4}\n".format(RomId, subProgram, entry['Type'], shortName, version));
      FileHandle.write("\n\n");
  if (len (BiosDirectoryInfos) != 0):
    for BiosDirectoryInfo in BiosDirectoryInfos:
      FileHandle.write("BIOS Directory Level {0}:\n".format(BiosDirectoryInfo ['Level']));
      FileHandle.write("RomId SubProgram Instance Type  ShortName            Version\n");
      for entry in BiosDirectoryInfo['Entries']:
        version = "null"
        shortName = ""
        if (entry['Type'] in BiosDirectoryEntryName):
          shortName = BiosDirectoryEntryName[entry['Type']][2]
        if(IsImageEntry(entry) and entry['Size']>0x100): #Get default version
          version = DefaultFwInfo(BinaryInfo['Binary'], entry)
          version = version['Version']
        FileHandle.write("0x{0:0>2X}  0x{1:0>2X}       0x{2:0>2X}     0x{3:0>2X}  {4:<20s} {5}\n".format(entry['RomId'], entry['SubProgram'], entry['Instance'], entry['Type'], shortName, version));
      FileHandle.write("\n\n");
  # FileHandle.write (prettify(root))

def OutPutPspDirInfoXml (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos, xmlFileName):
  global AddressMode
  global Writable
  global gXorBit24
  try:
    FileHandle = open (xmlFileName,'w')
  except:
    log.error("Error: Opening {}".format(xmlFileName))
    FatalErrorExit ()
  RootDict = {}
  if (AddressMode):
    RootDict['AddressMode'] = AddressMode
  if (Writable):
    RootDict['Writable'] = Writable
  if (gXorBit24):
    RootDict['XorBit24'] = gXorBit24
  RootDict = StringlizeDict (RootDict)
  root = Element('Dirs', RootDict)

  if (len (PspDirectoryInfos) != 0):
    for PspDirectoryInfo in PspDirectoryInfos:
      DirDict = {}
      # DirDict ['Base'] = PspDirectoryInfo ['PspDirOffset']
      if (PspDirectoryInfo ['BaseOffset'] != 0):
        DirDict ['HeaderBase'] = PspDirectoryInfo ['DirOffset']
        DirDict ['Base'] = PspDirectoryInfo ['BaseOffset']
      else:
        DirDict ['Base'] = PspDirectoryInfo ['DirOffset']

      if (PspDirectoryInfo ['DirSize'] != 0):
        DirDict ['Size'] = PspDirectoryInfo ['DirSize']
      if (PspDirectoryInfo ['SpiBlockSize'] != 0):
        DirDict ['SpiBlockSize'] = PspDirectoryInfo ['SpiBlockSize']
      if (PspDirectoryInfo ['LevelType'] != ""):
        DirDict ['LevelType'] = PspDirectoryInfo ['LevelType']
      if (PspDirectoryInfo ['ImageBase'] != 0):
        DirDict ['ImageBase'] = PspDirectoryInfo ['ImageBase']
      DirDict ['Level'] = PspDirectoryInfo ['Level']
      DirDict ['AddressMode'] = PspDirectoryInfo ['AddressMode']
      DirBaseAddress = DirDict ['Base']
      DirDict = StringlizeDict (DirDict)
      PspDirElmt = SubElement(root, 'PspDirectory', DirDict)
      for entry in PspDirectoryInfo['Entries']:
        _entry = entry.copy()
        # Change _entry['Type'] value to ['Type16'], to void BVM change
        # if 'Type16' in _entry:
          # _entry['Type'] = entry['Type16']
        #convert all numberic type to string, for tostring function only accept string
        _entry = StringlizeDict (_entry)
        #Override the default if it has been defined
        EntryType = 'IMAGE_ENTRY'
        if (entry['Type'] in PspDirectoryEntryName):
            EntryType = entry['Entry']
        #Get Entry type from PspDirectoryEntryName
        #                          Type:   DIR, Entry Type    ShortName    Full description
        #                            0x00: ['PSP' ,'IMAGE_ENTRY','AmdPubKey','AMD public Key', ],
        entryElmt = SubElement (PspDirElmt, EntryType, _entry)
        if (entry['Type'] == 1) or (entry['Type'] == 3):
          SubElement (entryElmt, 'Detail', pspBootLoaderInfo(BinaryInfo['Binary'], entry))
        elif (entry['Type'] == 2):
          SubElement (entryElmt, 'Detail', pspSecureOsInfo(BinaryInfo['Binary'], entry))
        elif (entry['Type'] == 8) or (entry['Type'] == 0x12) or (entry['Type'] == 0x2A):
          SubElement (entryElmt, 'Detail', smuFwInfo(BinaryInfo['Binary'], entry))
        elif ((entry['Type'] >= 0x30) and (entry['Type'] <= 0x37)):
          SubElement (entryElmt, 'Detail', ablInfo(BinaryInfo['Binary'], entry))
        elif (entry['Type'] == 0x29):
          SubElement (entryElmt, 'Detail', KvmInfo(BinaryInfo['Binary'], entry))
        # elif (entry['Type'] == 0x42):
          # SubElement (entryElmt, 'Detail', DxioFwInfo(BinaryInfo['Binary'], entry))
        elif (entry['Type'] == 0xb):
          SubElement (entryElmt, 'Detail', {"Help":'''BIT0:PSP Secure Debug Control Flag (0-Disabled, 1-Enabled);\
           BIT4:Enable applying of Security Policy to unsecure ASIC;\
           BIT28:MP0 DPM Enable
           BIT29:Skip MP2 FW loading. 0: Load MP2 FW normally 1: Skip MP2 FW Loading'''})
        elif(IsImageEntry(entry) and entry['Size']>0x100): #Get default version
          SubElement (entryElmt, 'Detail', DefaultFwInfo(BinaryInfo['Binary'], entry))

  if (len (BiosDirectoryInfos) != 0):
    for BiosDirectoryInfo in BiosDirectoryInfos:
      DirDict = {}
      if (BiosDirectoryInfo ['BaseOffset'] != 0):
        DirDict ['HeaderBase'] = BiosDirectoryInfo ['DirOffset']
        DirDict ['Base'] = BiosDirectoryInfo ['BaseOffset']
      else:
        DirDict ['Base'] = BiosDirectoryInfo ['DirOffset']

      if (BiosDirectoryInfo ['DirSize'] != 0):
        DirDict ['Size'] = BiosDirectoryInfo ['DirSize']
      if (BiosDirectoryInfo ['SpiBlockSize'] != 0):
        DirDict ['SpiBlockSize'] = BiosDirectoryInfo ['SpiBlockSize']
      if (BiosDirectoryInfo ['LevelType'] != ""):
        DirDict ['LevelType'] = BiosDirectoryInfo ['LevelType']
      if (BiosDirectoryInfo ['ImageBase'] != 0):
        DirDict ['ImageBase'] = BiosDirectoryInfo ['ImageBase']
      DirDict ['Level'] = BiosDirectoryInfo ['Level']
      DirDict ['AddressMode'] = BiosDirectoryInfo ['AddressMode']
      DirBaseAddress = DirDict ['Base']
      DirDict = StringlizeDict (DirDict)
      BiosDirElmt = SubElement(root, 'BiosDirectory',DirDict)
      for entry in BiosDirectoryInfo['Entries']:
        _entry = entry.copy()
        #convert all numberic type to string, for tostring function only accept string
        _entry = StringlizeDict (_entry)
        #Override the default if it has been defined
        if (entry['Type'] in BiosDirectoryEntryName):
            EntryType = entry['Entry']
        entryElmt = SubElement (BiosDirElmt, EntryType, _entry)
        if (entry['Type'] == 0x66):
          SubElement (entryElmt, 'Detail', ucodeInfo(BinaryInfo['Binary'], entry))

  if (len (IshHeaderInfos) != 0):
    for IshHeaderInfo in IshHeaderInfos:
      IshDict = {}
      IshDict ['Base'] = IshHeaderInfo ['Base']
      # IshDict ['Checksum'] = IshHeaderInfo ['Checksum']
      IshDict ['BootPriority'] = IshHeaderInfo ['BootPriority']
      IshDict ['UpdateRetries'] = IshHeaderInfo ['UpdateRetries']
      IshDict ['GlitchRetries'] = IshHeaderInfo ['GlitchRetries']
      IshDict ['Location'] = IshHeaderInfo ['Location']
      IshDict ['PspId'] = IshHeaderInfo ['PspId']
      IshDict ['SlotMaxSize'] = IshHeaderInfo ['SlotMaxSize']
      IshDict ['Reserved_1'] = IshHeaderInfo ['Reserved_1']
      IshDict = StringlizeDict (IshDict)
      DirElmt = SubElement (root, 'ISH_HEADER', IshDict)

  FileHandle.write (prettify(root))

def OutPutBiosDirectoryXml (root, BiosDirectoryInfo):
  DirDict = {}
  if (BiosDirectoryInfo ['BaseOffset'] != 0):
    DirDict ['HeaderBase'] = BiosDirectoryInfo ['DirOffset']
    DirDict ['Base'] = BiosDirectoryInfo ['BaseOffset']
  else:
    DirDict ['Base'] = BiosDirectoryInfo ['DirOffset']

  # if (BiosDirectoryInfo ['DirSize'] != 0):
  DirDict ['Size'] = BiosDirectoryInfo ['DirSize']
  if (BiosDirectoryInfo ['SpiBlockSize'] != 0):
    DirDict ['SpiBlockSize'] = BiosDirectoryInfo ['SpiBlockSize']
  if (BiosDirectoryInfo ['LevelType'] != ""):
    DirDict ['LevelType'] = BiosDirectoryInfo ['LevelType']
  if (BiosDirectoryInfo ['ImageBase'] != 0):
    DirDict ['ImageBase'] = BiosDirectoryInfo ['ImageBase']
  DirDict ['Level'] = BiosDirectoryInfo ['Level']
  DirDict ['AddressMode'] = BiosDirectoryInfo ['AddressMode']
  DirBaseAddress = DirDict ['Base']
  DirDict = StringlizeDict (DirDict)
  DirElmt = SubElement (root, 'BIOS_DIR', DirDict)
  for entry in BiosDirectoryInfo['Entries']:
    EntryDict = {}
    if (IsValueEntry (entry)):
      EntryDict = dict ((k, entry[k]) for k in ['Type', 'Value'])
      EntryDict = StringlizeDict (EntryDict)
      SubElement (DirElmt, 'VALUE_ENTRY', EntryDict)
    elif (IsImageEntry(entry)):
      EntryDict['File'] = OutputPath + entry['File']
      EntryDict['Type'] = entry ['Type']
      #Prepare for the 'Size' & 'Address'
      #fill the Size, Address attributes when Dir Size is 0, which means
      #User disable the auto allocation function
      if (BiosDirectoryInfo ['DirSize'] == 0):
        EntryDict['Size'] = entry ['Size']
        EntryDict['Address'] = entry ['Address']
      #Prepare for the 'Destination'
      if (entry['Destination'] != 0xFFFFFFFFFFFFFFFF):
        EntryDict['Destination'] = entry ['Destination']
      #Fill the instance, if instance is not 0
      if (entry['Instance'] != 0):
        EntryDict['Instance'] = entry ['Instance']
      #Fill the SubProgram, if SubProgram is not 0
      if (entry['SubProgram'] != 0):
        EntryDict['SubProgram'] = entry ['SubProgram']
      #Fill the RomId, if RomId is not 0
      if (entry['RomId'] != 0):
        EntryDict['RomId'] = entry ['RomId']
      EntryDict = StringlizeDict (EntryDict)
      EntryElmt = SubElement (DirElmt, 'IMAGE_ENTRY', EntryDict)

      #Prepare for the TypeAttrib tag, if any below attributes in non-zero
      if (not ((entry['RegionType'] == 0) and\
          (entry['ResetImage'] == 0) and\
          (entry['Copy'] == 0) and\
          (entry['ReadOnly'] == 0) and\
          (entry['Compressed'] == 0))):
        AttribsDict = {}
        AttribsDict = dict ((k, entry[k]) for k in ['RegionType', 'ResetImage', 'Copy', 'ReadOnly', 'Compressed'])
        AttribsDict = StringlizeDict (AttribsDict)
        SubElement (EntryElmt, 'TypeAttrib', AttribsDict)

    elif (IsPointEntry (entry)):
      EntryDict['Type'] = entry ['Type']
      EntryDict['Size'] = entry ['Size']
      if(entry ['AbsoluteAddr']):
        EntryDict['AbsoluteAddr'] = entry ['AbsoluteAddr']
        EntryDict['Address'] = entry ['Address']
      else:
        if((BiosDirectoryInfo ['AddressMode'] == 2) or (BiosDirectoryInfo ['AddressMode'] == 3)):
          if(entry ['AddressMode'] == 3):
            EntryDict['AddressMode'] = entry ['AddressMode']
            EntryDict['Address'] = entry ['Address'] - BiosDirectoryInfo ['ImageBase']
          elif(entry ['AddressMode'] == 1 or entry ['AddressMode'] == 0):
            EntryDict['AddressMode'] = entry ['AddressMode']
            EntryDict['Address'] = entry ['Address']
          elif(entry ['AddressMode'] == 2):
            #Write relative address to directory file
            EntryDict['Address'] = entry ['Address'] - DirBaseAddress
        else:
          EntryDict['Address'] = entry ['Address']
      #Prepare for the 'Destination'
      if (entry['Destination'] != 0xFFFFFFFFFFFFFFFF):
        EntryDict['Destination'] = entry ['Destination']
      #Fill the instance, if instance is not 0
      if (entry['Instance'] != 0):
        EntryDict['Instance'] = entry ['Instance']
      #Fill the SubProgram, if SubProgram is not 0
      if (entry['SubProgram'] != 0):
        EntryDict['SubProgram'] = entry ['SubProgram']
      #Fill the RomId, if RomId is not 0
      if (entry['RomId'] != 0):
        EntryDict['RomId'] = entry ['RomId']

      EntryDict = StringlizeDict (EntryDict)
      EntryElmt = SubElement (DirElmt, 'POINT_ENTRY', EntryDict)

      #Prepare for the TypeAttrib tag, if any below attributes in non-zero
      if (not ((entry['RegionType'] == 0) and\
          (entry['ResetImage'] == 0) and\
          (entry['Copy'] == 0) and\
          (entry['ReadOnly'] == 0) and\
          (entry['Compressed'] == 0))):
        AttribsDict = {}
        AttribsDict = dict ((k, entry[k]) for k in ['RegionType', 'ResetImage', 'Copy', 'ReadOnly', 'Compressed'])
        AttribsDict = StringlizeDict (AttribsDict)
        SubElement (EntryElmt, 'TypeAttrib', AttribsDict)
    else:
      log.warn ("Unrecognized entry Type")
      # FatalErrorExit ()

def OutPutIshHeaderXml (root, IshHeaderInfo):
  IshDict = {}
  IshDict ['Base'] = IshHeaderInfo ['Base']
  # IshDict ['Checksum'] = IshHeaderInfo ['Checksum']
  IshDict ['BootPriority'] = IshHeaderInfo ['BootPriority']
  IshDict ['UpdateRetries'] = IshHeaderInfo ['UpdateRetries']
  IshDict ['GlitchRetries'] = IshHeaderInfo ['GlitchRetries']
  IshDict ['Location'] = IshHeaderInfo ['Location']
  IshDict ['PspId'] = IshHeaderInfo ['PspId']
  IshDict ['SlotMaxSize'] = IshHeaderInfo ['SlotMaxSize']
  IshDict ['Reserved_1'] = IshHeaderInfo ['Reserved_1']
  IshDict = StringlizeDict (IshDict)
  DirElmt = SubElement (root, 'ISH_HEADER', IshDict)

def OutPutPspDirectoryXml (PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos, cfgFileName):
  global AddressMode
  global Writable
  global gXorBit24
  try:
    FileHandle = open (cfgFileName,'w')
  except:
    log.error("Error: Opening {}".format(cfgFileName))
    sys.exit(2)
  RootDict = {}
  if (AddressMode):
    RootDict['AddressMode'] = AddressMode
  if (Writable):
    RootDict['Writable'] = Writable
  if (gXorBit24):
    RootDict['XorBit24'] = gXorBit24
  RootDict = StringlizeDict (RootDict)
  root = Element('DIRS', RootDict)
  if (len (PspDirectoryInfos) != 0):
    for PspDirectoryInfo in PspDirectoryInfos:
      DirDict = {}
      # DirDict ['Base'] = PspDirectoryInfo ['PspDirOffset']
      if (PspDirectoryInfo ['BaseOffset'] != 0):
        DirDict ['HeaderBase'] = PspDirectoryInfo ['DirOffset']
        DirDict ['Base'] = PspDirectoryInfo ['BaseOffset']
      else:
        DirDict ['Base'] = PspDirectoryInfo ['DirOffset']

      # if (PspDirectoryInfo ['DirSize'] != 0):
      DirDict ['Size'] = PspDirectoryInfo ['DirSize']
      if (PspDirectoryInfo ['SpiBlockSize'] != 0):
        DirDict ['SpiBlockSize'] = PspDirectoryInfo ['SpiBlockSize']
      if (PspDirectoryInfo ['LevelType'] != ""):
        DirDict ['LevelType'] = PspDirectoryInfo ['LevelType']
      if (PspDirectoryInfo ['ImageBase'] != 0):
        DirDict ['ImageBase'] = PspDirectoryInfo ['ImageBase']
      DirDict ['Level'] = PspDirectoryInfo ['Level']
      DirDict ['AddressMode'] = PspDirectoryInfo ['AddressMode']
      DirBaseAddress = DirDict ['Base']
      DirDict = StringlizeDict (DirDict)
      DirElmt = SubElement (root, 'PSP_DIR', DirDict)
      for entry in PspDirectoryInfo['Entries']:
        EntryDict = {}
        if (IsValueEntry (entry)):
          EntryDict = dict ((k, entry[k]) for k in ['Type', 'Value'])
          EntryDict = StringlizeDict (EntryDict)
          SubElement (DirElmt, 'VALUE_ENTRY', EntryDict)
        elif (IsImageEntry (entry)):
          # Dynamic Address
          # EntryInfo = {'Type':Type,'Size':Size,'RTOffset':Location, 'Description':getEntryName(Type, 1), 'Address':getOffsetInBin(biosSize, Location)}
          if (entry['RomId'] != 0):
            EntryDict['RomId'] = entry['RomId']
          if (entry['SubProgram'] != 0):
            EntryDict['SubProgram'] = entry['SubProgram']
          if (entry['Instance'] != 0):
            EntryDict['Instance'] = entry['Instance']
          EntryDict['File'] = OutputPath +  entry['File']
          if (PspDirectoryInfo ['DirSize'] != 0):
            EntryDict['Type'] = entry['Type']
          else:
            EntryDict['Type'] = entry ['Type']
            EntryDict['Size'] = entry ['Size']
            EntryDict['Address'] = entry ['Address']
          EntryDict = StringlizeDict (EntryDict)
          SubElement (DirElmt, 'IMAGE_ENTRY', EntryDict)
        elif (IsPointEntry (entry)):
          if (entry['RomId'] != 0):
            EntryDict['RomId'] = entry['RomId']
          if (entry['SubProgram'] != 0):
            EntryDict['SubProgram'] = entry['SubProgram']
          if (entry['Instance'] != 0):
            EntryDict['Instance'] = entry['Instance']
          EntryDict['Type'] = entry ['Type']
          EntryDict['Size'] = entry ['Size']
          if(entry ['AbsoluteAddr']):
            EntryDict['AbsoluteAddr'] = entry ['AbsoluteAddr']
            EntryDict['Address'] = entry ['Address']
          else:
            if((PspDirectoryInfo ['AddressMode'] == 2) or (PspDirectoryInfo ['AddressMode'] == 3)):
              if(entry ['AddressMode'] == 3):
                EntryDict['AddressMode'] = entry ['AddressMode']
                EntryDict['Address'] = entry ['Address'] - PspDirectoryInfo ['ImageBase']
              elif(entry ['AddressMode'] == 1 or entry ['AddressMode'] == 0):
                EntryDict['AddressMode'] = entry ['AddressMode']
                EntryDict['Address'] = entry ['Address']
              elif(entry ['AddressMode'] == 2):
                #Write relative address to directory file
                EntryDict['Address'] = entry ['Address'] - DirBaseAddress
            else:
              EntryDict['Address'] = entry ['Address']
          # EntryDict.update = {(k, entry[k]) for k in ['Type', 'Size', 'Address']}
          EntryDict = StringlizeDict (EntryDict)
          SubElement (DirElmt, 'POINT_ENTRY', EntryDict)
        else:
          log.warn ("Unrecognized entry Type")
          # FatalErrorExit ()

  if (len (BiosDirectoryInfos) != 0):
    for BiosDirectoryInfo in BiosDirectoryInfos:
      OutPutBiosDirectoryXml (root, BiosDirectoryInfo)

  if (len (IshHeaderInfos) != 0):
    for IshHeaderInfo in IshHeaderInfos:
      OutPutIshHeaderXml (root, IshHeaderInfo)

  FileHandle.write(prettify(root))

def main ():
  global OutputPath
  Args = ParseArg()
  log.debug ("Args: %s",Args)
  OutputPath = Args.outputpath+"/"
  print("\nBuildPspDirectory   Version "+ Version+"\n")
  #Always create output folder except dp command with no additional parameters
  if not (Args.subcommand == 'dp' and
          Args.binary != True and
          Args.xml != True and
          Args.directory != True):
    if not os.path.exists(OutputPath):
            os.makedirs(OutputPath)
  if (Args.subcommand == 'bd' or Args.subcommand == 'bb'):
    BiosSize=os.path.getsize(Args.InBiosImage)
    log.info ("Parse Configure File: %s\n", Args.CfgFile)
    (PspDicts, BiosDicts, IshDicts, ComboDict, BiosComboDict, CmnDict) = ParseCfgFile (Args.CfgFile)
    if (len(PspDicts) != 0):
      # Traverse PspDicts array
      for PspDict in PspDicts:
        log.debug ("Optimize PSP Entry Order\n")
        PspDict = OptPspEntryOrder (PspDict)

        #Check if there exists type conflict
        CheckPspEntryTypeConflict(PspDict)

        log.debug ("Calculate PSP Entry Address\n")
        PspDict = CalcEntryOffset(BiosSize, PspDict, "PSP_DIR")
        DumpPspDict (BiosSize, PspDict)
        log.debug ("Prepare for PspDirecotry Header Data")
        PspDict ["PspDirHeader"] = BuildPspDir (BiosSize, PspDict)

    if (len(BiosDicts) != 0):
      # Traverse PspDicts array
      for BiosDict in BiosDicts:
        log.debug ("Calculate BIOS Entry Address\n")
        BiosDict = CalcEntryOffset (BiosSize, BiosDict, "BIOS_DIR")
        DumpBiosDict (BiosSize, BiosDict)

        #Check if there exists type conflict
        CheckBiosEntryTypeConflict(BiosDict)

        log.debug ("Prepare for BiosDirecotry Header Data")
        BiosDict ["BiosDirHeader"] = BuildBiosDir (BiosSize, BiosDict)

    if (len(ComboDict) != 0):
      log.debug ("Prepare for ComboDict Header Data")
      ComboDirHeader = BuildComboDir (BiosSize, ComboDict, 0)

    if (len(BiosComboDict) != 0):
      log.debug ("Prepare for BiosComboDict Header Data")
      BiosComboDirHeader = BuildComboDir (BiosSize, BiosComboDict, 1)

    if (Args.subcommand == 'bd'):
      if (len(PspDicts) != 0):
        for PspDict in PspDicts:
          log.info ("Write the data to file %s%s", OutputPath, "PspDirHeaderL"+str (PspDict["Level"])+".bin")
          WriteBinaryFile ("PspDirHeaderL"+str (PspDict["Level"])+".bin", PspDict["PspDirHeader"])
      if (len(BiosDicts) != 0):
        for BiosDict in BiosDicts:
          #Sometimes L2A != L2B
          postfix = ""
          if(("LevelType" in BiosDict) and (str(BiosDict["LevelType"]) == "B")):
            postfix = "B"
          log.info ("Write the data to file %s%s", OutputPath, "BiosDirHeaderL"+ postfix +".bin")
          # Only write the Level 1 Header
          WriteBinaryFile ("BiosDirHeaderL"+ str (BiosDict["Level"])+ postfix +".bin", BiosDict["BiosDirHeader"])

    elif (Args.subcommand == 'bb'):
      log.info ("Patch BIOS Image")
      # Read the BIOS file
      BiosBinary = ReadBinaryFile (Args.InBiosImage)
      #First update the PSPDirectory in the binary image
      if (len(PspDicts) != 0):
        for PspDict in PspDicts:
          if (PspDict['Base'] == None):
            log.error ("Error: Base attribute haven't been defined in PSP_DIR tag")
            FatalErrorExit()
          if (PspDict['HeaderBase'] != None):
            BiosBinary = PatchBinary (BiosBinary, PspDict["PspDirHeader"], PspDict['HeaderBase'], "---PSPDirectory---")
          else:
            BiosBinary = PatchBinary (BiosBinary, PspDict["PspDirHeader"], PspDict['Base'], "---PSPDirectory---")
          # Process each PSP entry
          for Entry in PspDict['Entries']:
            if (IsImageEntry(Entry)):
              PspBinary = ReadBinaryFile (Entry['File'])
              if (PspBinary == None):
                log.error ("Error Reading %s\n", Entry['File'])
                FatalErrorExit()
              # And patch the output image
              display_string = "0x{:02X} {} Version: {}".format(
                                                  Entry['Type'],
                                                  Entry['File'],
                                                  version_from_dir_Entry (
                                                    Entry,
                                                    'psp'
                                                  )
                                                  )
              BiosBinary = PatchBinary (BiosBinary,
                                        PspBinary,
                                        Entry['Address'],
                                        display_string)
            elif (IsPointEntry(Entry)):
              print("POINT: %25s <0x%02X Addr=0x%X Size=0x%X>" %("", Entry['Type'], Entry['Address'], Entry['Size']))

      #Second update the BiosDirectory in the binary image
      if (len(BiosDicts) != 0):
        for BiosDict in BiosDicts:
          if (BiosDict['Base'] == None):
            log.error ("Error: Base attribute haven't been defined in BIOS_DIR tag")
            FatalErrorExit()
          if (BiosDict['HeaderBase'] != None):
            BiosBinary = PatchBinary (BiosBinary, BiosDict["BiosDirHeader"], BiosDict['HeaderBase'], "---BIOSDirectory---")
          else:
            BiosBinary = PatchBinary (BiosBinary, BiosDict["BiosDirHeader"], BiosDict['Base'], "---BIOSDirectory---")
          # Process each BIOS entry
          for Entry in BiosDict['Entries']:
            if (IsImageEntry(Entry)):
              PspBinary = ReadBinaryFile (Entry['File'])
              if (PspBinary == None):
                log.error ("Error Reading %s\n", Entry['File'])
                FatalErrorExit()
              # And patch the output image
              display_string = "0x{:02X} {} Version: {}".format(
                                                  Entry['Type'],
                                                  Entry['File'],
                                                  version_from_dir_Entry (
                                                    Entry,
                                                    'bios'
                                                  )
                                                  )
              BiosBinary = PatchBinary (BiosBinary,
                                        PspBinary,
                                        Entry['Address'],
                                        display_string)
            elif (IsPointEntry(Entry)):
              print("POINT: %25s <0x%02X Addr=0x%X Size=0x%X Dest=0x%X ResetImage=%X Copy=%X>"
                %("", Entry['Type'], Entry['Address'], Entry['Size'], Entry['Destination'],
                Entry['ResetImage'], Entry['Copy']))

      #update Combo Header in the binary image
      if (len(ComboDict) != 0):
        if (ComboDict['Base'] == None):
          log.error ("Error: Base attribute haven't been defined in ComboDict tag")
          FatalErrorExit()
        BiosBinary = PatchBinary (BiosBinary, ComboDirHeader, ComboDict['Base'], "---COMBODirecory---")

      #update BIOS Combo Header in the binary image
      if (len(BiosComboDict) != 0):
        if (BiosComboDict['Base'] == None):
          log.error ("Error: Base attribute haven't been defined in BiosComboDict tag")
          FatalErrorExit()
        BiosBinary = PatchBinary (BiosBinary, BiosComboDirHeader, BiosComboDict['Base'], "---COMBODirecory---")

      #update CmnDirectory in the binary image
      if (len(CmnDict) != 0):
        # Process each Cmn entry
        for Entry in CmnDict['Entries']:
          if (IsImageEntry(Entry)):
            PspBinary = ReadBinaryFile (Entry['File'])
            if (PspBinary == None):
              log.error ("Error Reading %s\n", Entry['File'])
              FatalErrorExit()
            #Check file size
            if(len (PspBinary) > Entry['Size']):
              log.error ("%s size is larger than defined entry size %x\n", Entry['File'], Entry['Size'])
              FatalErrorExit()
            # And patch the output image
            BiosBinary = PatchBinary (BiosBinary, PspBinary, Entry['Address'], Entry['File'])

      #update Ish Header in the binary image
      if (len(IshDicts) != 0):
        for IshDict in IshDicts:
          #Generate Ish Header Binary
          IshBinary = BuildIshHeader(IshDict)
          # And patch the output image
          BiosBinary = PatchBinary (BiosBinary, IshBinary, IshDict['Base'], "Ish Header")
      log.info ("Generate BIOS image [%s]" ,OutputPath + Args.OutBiosImage)
      WriteBinaryFile (Args.OutBiosImage, BiosBinary)
  elif (Args.subcommand == 'dp'):
    log.info ("Dump BIOS Psp DirectoryOut for %s" %(Args.program))
    (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos) = GetPspDirectory (Args.InBiosImage)
    if ((len (PspDirectoryInfos) == 0) and (len (BiosDirectoryInfos) == 0)):
      log.error("Error: Result of GetPspDirectory is invalid")
      FatalErrorExit ()
    if (Args.binary == True):
      print("Output  PSP binaries")
      if (len (PspDirectoryInfos) != 0):
        for PspDirectoryInfo in PspDirectoryInfos:
          OutPutPspBinaries (BinaryInfo['Binary'], PspDirectoryInfo['Entries'], PspDirectoryInfo['Level'], PspDirectoryInfo['LevelType'])

      print("Output  BIOS binaries")
      if (len (BiosDirectoryInfos) != 0):
        for BiosDirectoryInfo in BiosDirectoryInfos:
          OutPutBiosBinaries (BinaryInfo['Binary'], BiosDirectoryInfo['Entries'], BiosDirectoryInfo['Level'], BiosDirectoryInfo['LevelType'])
    if (Args.xml == True):
      print("Output  PspDirInfo.xml")
      OutPutPspDirInfoXml (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos, OutputPath + "PspDirInfo.xml")
    if (Args.versiontxt == True):
      print("Output  PspVersion.txt")
      OutPutPspVersionTxt (BinaryInfo, PspDirectoryInfos, BiosDirectoryInfos, OutputPath + "PspVersion.txt")
    if (Args.directory == True):
      print("Output  PspDirectory.xml")
      OutPutPspDirectoryXml (PspDirectoryInfos, BiosDirectoryInfos, IshHeaderInfos, OutputPath + "PspDirectory.xml")
if __name__ == "__main__":
  main()
