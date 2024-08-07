project('simple_tcp_rpc', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

# Add homebrew includes
if host_machine.system() == 'darwin'
  homebrew_prefix = run_command('brew', '--prefix', check: true).stdout().strip()
  include_dir = include_directories('.', homebrew_prefix / 'include')
else
  include_dir = include_directories('.', '/usr/include')
endif

executable('demo_client',
           'demo_client.cpp',
           include_directories: include_dir,
           )

executable('demo_server',
           'demo_server.cpp',
           include_directories: include_dir,
           )
