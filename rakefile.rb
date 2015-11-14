PROJECT_CEEDLING_ROOT = "vendor/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/ceedling.rb"

load "scripts/helper.rb"

# With this now we can choose which '.yml' configuration file to be loaded by
# issuing e.g.:  
#     rake project=./myproject.yml test:all
#
project_file = get_value_from_env('p', './project.yml')
raise ArgumentError, "Error: #{project_file} file does not exist."  \
                                          if !File.exist?(project_file)
Ceedling.load_project(config: project_file)
load_extra_scripts(project_file)

desc 'Run complete tests'
task :complete_tests do
  # Run all (host) CLI for probe tests?
  system "rake host:test:all"
  # Run all probe tests in host using mocks
  system "rake p=probe.yml probe:test:all"
  # Run all probe tests with hardware-in-the-loop
  system "rake p=probe_hil.yml probe:hw:test:all"
end

task :default => %w[ complete_tests host:release ]