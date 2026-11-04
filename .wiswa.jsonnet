local utils = import 'utils.libjsonnet';

{
  uses_user_defaults: true,
  project_type: 'c',
  project_name: 'hello-world-efi',
  version: '0.0.4',
  description: 'Very simple demonstration of UEFI facilities, building with CMake, and testing with CMocka.',
  custom_project_badges: [
    {
      anchor: '[![Tests](https://github.com/Tatsh/hello-world-efi/actions/workflows/tests.yml/badge.svg)]',
      href: 'https://github.com/Tatsh/hello-world-efi/actions/workflows/tests.yml',
    },
    {
      anchor: '[![Coverage Status](https://coveralls.io/repos/github/Tatsh/hello-world-efi/badge.svg?branch=master)]',
      href: 'https://coveralls.io/github/Tatsh/hello-world-efi?branch=master',
    },
  ],
  keywords: ['efi', 'hello-world', 'uefi'],
  want_main: false,
  want_codeql: true,
  github+: {
    codeql+: {
      languages: ['actions'],
      runs_on: 'ubuntu-latest',
      workflow_file: 'codeql-actions.yml',
    },
  },
  want_tests: false,
  want_winget: false,
  clang_format_args: '*.c',
  vscode+: {
    c_cpp+: {
      configurations: [
        {
          cStandard: 'gnu23',
          compilerArgs: ['-fno-stack-protector', '-fpic', '-fshort-wchar', '-mno-red-zone', '-Wall'],
          compilerPath: '/usr/bin/gcc',
          cppStandard: 'gnu++23',
          defines: ['EFI_FUNCTION_WRAPPER', 'GOP=1'],
          includePath: [
            '${default}',
            '${workspaceFolder}',
            '/usr/include/efi/protocol',
            '/usr/include/efi/x86_64',
          ],
          name: 'Linux',
        },
      ],
    },
    launch+: {
      configurations: [
        {
          cwd: '${workspaceFolder}/build',
          name: 'General tests',
          program: '${workspaceFolder}/build/general_tests',
          request: 'launch',
          type: 'cppdbg',
        },
      ],
    },
    settings+: {
      'C_Cpp.default.includePath': [
        '/usr/include/efi',
        '/usr/include/efi/protocol',
        '/usr/include/efi/x86_64',
      ],
      'cmake.configureArgs': ['-DWITH_GOP=ON', '-DWITH_TESTS=ON'],
    },
  },
}
+ {
  want_scan_build: true,
  github+: { workflows+: {
    scan_build+: {
      configure_command: 'cmake -S . -B build-scan -DCMAKE_BUILD_TYPE=Debug -DWITH_GOP=ON',
      documentation_steps: [
        {
          name: 'Build project homepage',
          uses: 'actions/jekyll-build-pages@44a6e6beabd48582f863aeeb6cb2151cc1716697',
          with: {
            destination: 'build/html',
          },
        },
      ],
      env: {},
      setup_steps: [
        {
          name: 'Install build dependencies',
          run: |||
            sudo apt-get update
            sudo apt-get install -y gnu-efi ninja-build
          |||,
        },
        {
          name: 'Install current CMake',
          run: 'pipx install cmake',
        },
      ],
    },
  } },
}
+ { static_analysis_paths: ['main.c', 'test.c'] }
