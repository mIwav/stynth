PROJECT_CEEDLING_ROOT = "vendor/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/ceedling/rakefile.rb"

task :default => %w[ test:all release ]

# Dummy task to ensure that the SERIAL_PORT environment variable is set.
# It can be set on the command line as follows:
#   $ rake SERIAL_PORT=[serial port name]
task :serial_port do
  unless ENV['SERIAL_PORT']
    raise "SERIAL_PORT is not defined in the environment!"
  end
end

desc "Convert the output binary to a hex file for programming to the STM"
task :convert => :release do
  bin_file = "build/release/#{RELEASE_BUILD_OUTPUT}"
  hex_file = "build/release/#{RELEASE_BUILD_OUTPUT}.hex"
  # cmd = "/cygdrive/c/STM32Toolchain/gcc-arm/bin/arm-none-eabi-objcopy.exe -O ihex #{bin_file} #{hex_file}"
  # cmd = "/usr/local/x-tools/arm-bare_newlib_cortex_m3_nommu-eabihf/bin/arm-bare_newlib_cortex_m3_nommu-eabihf-objcopy -O ihex #{bin_file} #{hex_file}"
  cmd = "/usr/local/x-tools/gcc-arm-none-eabi/bin/arm-none-eabi-objcopy -O ihex #{bin_file} #{hex_file}"
  puts cmd
  sh cmd
end

desc "Print size of exported built elf file"
task :size => :release do
  bin_file = "build/release/#{RELEASE_BUILD_OUTPUT}"
  # cmd = "/cygdrive/c/STM32Toolchain/gcc-arm/bin/arm-none-eabi-size.exe --format=berkeley #{bin_file}"
  # cmd = "/usr/local/x-tools/arm-bare_newlib_cortex_m3_nommu-eabihf/bin/arm-bare_newlib_cortex_m3_nommu-eabihf-size --format=berkeley #{bin_file}"
  cmd = "/usr/local/x-tools/gcc-arm-none-eabi/bin/arm-none-eabi-size --format=berkeley #{bin_file}"
  puts cmd
  sh cmd
end

task :release do
    Rake::Task[:convert].invoke
    Rake::Task[:size].invoke
end

# desc "Program the Arduino over the serial port."
# task :program => [:convert, :serial_port] do
#   hex_file = "build/release/#{RELEASE_BUILD_OUTPUT}.hex"
#   cmd = "avrdude -F -V -c arduino -p #{ENV['MCU']} -P #{ENV['SERIAL_PORT']} -b 115200 -U flash:w:#{hex_file}"
#   puts cmd
#   sh cmd
# end
