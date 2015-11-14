load "scripts/probe.rb"

namespace :probe do
  namespace :test do
    filenames = get_all_tests("test/Probe/**/test_*.c")
    # Test probe using mocks
    desc 'Run all probe tests'
    task :all => filenames
  end
end
