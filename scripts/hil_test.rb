load "scripts/probe.rb"
load "scripts/target.rb"

namespace :probe do
  namespace :"hw:test" do
    filenames = get_all_tests("test/Hardware/**/test_*.c")
    # Run tests with hardware-in-the-loop
    desc 'Run all hardware-in-the-loop tests'
    task :all => ['target:release', 'probe:flash'] + filenames
  end
end