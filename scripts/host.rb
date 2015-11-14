# Set the path to CException source code
CEXCEPTION_PATH = "vendor/ceedling/vendor/c_exception/lib"

# Load build script to help build C program
load "scripts/cbuild.rb"
  
# Main dependency list
main_dependency = {
  # depender                dependees
  'ErrorCode.o'       => ['ErrorCode.c', 'ErrorCode.o'],
  'FileToken.o'       => ['FileToken.c', 'FileToken.h', 'Token.h', 'StringObject.h'],
  'GetHeaders.o'      => ['GetHeaders.c', 'GetHeaders.h', 'Read_File.h', 'elf.h', 'ProgramElf.h'],
  'IdentifierToken.o' => ['IdentifierToken.c', 'IdentifierToken.h', 'Token.h', 'StringObject.h'],
  'Interface.o'       => ['Interface.c', 'Interface.h', 'Token.h', 'Tlv.h', 'ErrorCode.h'],
  'MemoryReadWrite.o' => ['MemoryReadWrite.c', 'MemoryReadWrite.h', 'Yield.h', 'ProgramLoader.h'],
  'NumberToken.o'     => ['NumberToken.c', 'NumberToken.h', 'Token.h', 'StringObject.h'],
  'OperatorToken.o'   => ['OperatorToken.c', 'OperatorToken.o', 'ErrorCode.h', 'StringObject.h'],
  'ProgramLoader.o'   => ['ProgramLoader.c', 'ProgramLoader.h', 'Tlv.h', 'GetHeaders.h', 'ErrorCode.h',
                          'Interface.h', 'SystemTime.h'],
  'Read_File.o'       => ['Read_File.c', 'Read_File.h', 'elf.h'],
  'Relocator.o'       => ['Relocator.c', 'Relocator.h', 'Read_File.h', 'elf.h', 'ProgramElf.h', 'GetHeaders.h'],
  'StringObject.o'    => ['StringObject.c', 'StringObject.h'],
  'SystemTime.o'      => ['SystemTime.c', 'SystemTime.h'],
  'Tlv.o'             => ['Tlv.c', 'Tlv.h', 'Uart.h', 'TlvEx.h', 'ErrorCode.h', 'SystemTime.h', 'Yield.h'],
  'Token.o'           => ['Token.c', 'Token.h', 'StringObject.h', 'NumberToken.h', 'OperatorToken.h',
                          'IdentifierToken.h', 'FileToken.h', 'ErrorCode.h'],
  'Uart.o'            => ['Uart.c', 'Uart.h', 'ErrorCode.h'],
  'Main.o'            => ['Main.c', 'ProgramLoader.h', 'Tlv.h', 'GetHeaders.h', 'ErrorCode.h',
                          'Interface.h', 'SystemTime.h'],
  'Main.exe'          => ['Main.o', 'CException.o', 'ErrorCode.o', 'ProgramLoader.o', 'MemoryReadWrite.o',
                          'SystemTime.o', 'Interface.o', 'Tlv.o', 'Uart.o', 'Token.o', 'Read_File.o',
                          'GetHeaders.o', 'Relocator.o', 'FileToken.o', 'IdentifierToken.o', 'NumberToken.o',
                          'StringObject.o'],
}

# Support library dependency list
exception_dependency = {'CException.o'  => ['CException.c', 'CException.h']}

# Support dependency list
header_dependency = {
  'FileToken.o'       => ['Token.h', 'StringObject.h'],
  'GetHeaders.o'      => ['Read_File.h', 'elf.h', 'ProgramElf.h'],
  'IdentifierToken.o' => ['Token.h', 'StringObject.h'],
  'Interface.o'       => ['Token.h', 'Tlv.h', 'ErrorCode.h'],
  'MemoryReadWrite.o' => ['Yield.h', 'ProgramLoader.h'],
  'OperatorToken.o'   => ['ErrorCode.h', 'StringObject.h'],
  'NumberToken.o'     => ['Token.h', 'StringObject.h'],
  'ProgramLoader.o'   => ['Tlv.h', 'GetHeaders.h', 'ErrorCode.h', 'Interface.h', 'SystemTime.h'],
  'Read_File.o'       => ['elf.h'],
  'Relocator.o'       => ['Read_File.h', 'elf.h', 'ProgramElf.h', 'GetHeaders.h'],
  'Token.o'           => ['StringObject.h', 'NumberToken.h', 'OperatorToken.h',
                          'IdentifierToken.h', 'FileToken.h', 'ErrorCode.h'],
  'Tlv.o'             => ['Uart.h', 'TlvEx.h', 'ErrorCode.h', 'SystemTime.h', 'Yield.h'],
  'Uart.o'            => ['ErrorCode.h'],
}

# Configuration parameters
config = {
  :verbose      => :no,
  :compiler     => 'gcc',
  :linker       => 'gcc',
  :include_path => [CEXCEPTION_PATH,
                    'src/app/Tlv',
                    'src/Host',
                    'src/Host/ElfReader',
                    'src/Host/StringObject',
                    'src/Host/PlatformSpecific'],
  :user_define  => ['HOST='],
#  :library_path => 'lib',
#  :library => ['libusb'],
#  :linker_script => 'MyLinkerScript.ld',
#  :compiler_options => ['-DOK'],                 # Other compiler options
#  :linker_options => ['-DOK'],                   # Other linker options
  :option_keys  => {:library => '-l',
                    :library_path => '-L',
                    :include_path => '-I',
                    :output_file => '-o',
                    :compile => '-c',
                    :linker_script => '-T',
                    :define => '-D'}
}

namespace :host do
  desc 'Build CLI for probe'
  task :release do
    puts "Creating CLI program for ARM probe..."
    dep_list = compile_list(exception_dependency, CEXCEPTION_PATH, 'build/release/host/c', '.', config)
    dep_list.merge!(compile_all(['src/app/Tlv',                 # directory of dependee
                                 'src/Host/ElfReader', 
                                 'src/Host/StringObject',
                                 'src/Host/PlatformSpecific',
                                 'src/Host'],
                                 'build/release/host/c',        # director of .o files
                                  config))                      # config object
    link_all(getDependers(dep_list), 'build/release/Main.exe', config)
    Rake::Task["build/release/Main.exe"].invoke
  end
end

namespace :host do
  namespace :test do
    filenames = get_all_tests("test/Host/**/test_*.c")
    desc 'Run all probe tests'
    task :all => filenames
  end
end
