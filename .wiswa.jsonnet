local utils = import 'utils.libjsonnet';

{
  uses_user_defaults: true,
  project_type: 'c',
  project_name: 'hello-world-efi',
  version: '0.0.2',
  description: 'Very simple demonstration of UEFI facilities, building with CMake, and testing with CMocka.',
  social+: {
    custom_badges: [
      '[![Tests](https://github.com/Tatsh/hello-world-efi/actions/workflows/tests.yml/badge.svg)](https://github.com/Tatsh/hello-world-efi/actions/workflows/tests.yml)',
      '[![Coverage Status](https://coveralls.io/repos/github/Tatsh/hello-world-efi/badge.svg?branch=master)](https://coveralls.io/github/Tatsh/hello-world-efi?branch=master)',
    ],
  },
  keywords: ['efi', 'hello-world', 'uefi'],
  want_main: false,
  want_codeql: false,
  want_tests: false,
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
