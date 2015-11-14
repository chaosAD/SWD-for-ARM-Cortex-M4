# Load build script to help build C program
load "scripts/cbuild.rb"

FLASHER = get_value_from_env("flasher", "ST-LINK_CLI") unless defined? FLASHER
ELF_TO_HEX = get_value_from_env("elf_to_hex", "arm-none-eabi-objcopy") unless defined? ELF_TO_HEX
C_EXCEPTION_PATH = "vendor/ceedling/vendor/c_exception/lib " unless defined? C_EXCEPTION_PATH
TARGET_OUTPUT_PATH = 'build/release/target/' unless defined? TARGET_OUTPUT_PATH

namespace :target do
  # Configuration parameters
  config = {
    :verbose      => :no,
    :compiler     => 'arm-none-eabi-gcc',
    :linker       => 'arm-none-eabi-gcc',
  # -IC:\Users\user26\CoIDE\workspace\RTOS
    :include_path => ['src/app/Stub', 'src/app/SystemConfig',
                      'FlashProgrammer/app/Drivers',
                      'FlashProgrammer/app/Legacy',
                      'FlashProgrammer/app'],
    :user_define  => ['STM32F429ZI', 'STM32F429xx'],
    :library_path => '.',
  #  :library => ['libusb'],
    :linker_script => 'FlashProgrammer/ToRam.ld',
    :compiler_options => ['-mcpu=cortex-m4 -mthumb -g2 -Wall -O0 -g'],          # Other compiler options
    :linker_options => ['-mcpu=cortex-m4 -mthumb -g2 -O0 -Wl,--gc-sections'],   # Other linker options
    :option_keys  => {:library => '-l',
                      :library_path => '-L',
                      :include_path => '-I',
                      :output_file => '-o',
                      :compile => '-c',
                      :linker_script => '-Wl,-T',
                      :define => '-D'}
  }
  ouput_elf = nil
  ouput_hex = nil
  task :prepare_release, [:coproj] do |t, args|
    filenames, coproj = get_all_source_files_in_coproj(args[:coproj], "./FlashProgrammer")
    puts "Building sources in #{coproj}..."
    file = File.basename(coproj, '.coproj')
    ouput_elf = File.join(TARGET_OUTPUT_PATH, file + '.elf')
    ouput_hex = File.join(TARGET_OUTPUT_PATH, file + '.hex')
    dep_list = createCompilationDependencyList(filenames, ['c', '.c++', '.s', 'cpp', 'asm'], '.', '.o')
    dep_list = compile_list(dep_list, '.', TARGET_OUTPUT_PATH, '.', config)
  #  p dep_list
    link_all(getDependers(dep_list), ouput_elf, config)

    file ouput_hex => ouput_elf do |n|
      if(program_available?(ELF_TO_HEX) == nil)
        puts "Error: Cannot find #{ELF_TO_HEX} program to turn ELF to HEX."
        exit
      end
      puts "converting #{n.prerequisites[0]} to hex..."
      sys_cli "#{ELF_TO_HEX} -O ihex #{n.prerequisites[0]} #{n.name}"
    end
  end

  desc 'Build probe hardware release code'
  task :release, [:coproj] => :prepare_release do |t, args|
    Rake::Task[ouput_elf].invoke(args)
  end

  desc "Just duplicating .gitignore"
  task :ignore do
    src = ".gitignore"
    target = ".gitignoreXXX"
    if !up_to_date?(target, src)
      p "duplicating .gitignore"
      sh "cp #{src} #{target}"
    else
      p "already have the latest copy..."
    end
  end
end