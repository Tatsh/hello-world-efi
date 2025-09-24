local utils = import 'utils.libjsonnet';

{
  project_type: 'c',
  project_name: 'hello-world-efi',
  version: '0.0.2',
  description: 'Very simple demonstration of UEFI facilities, building with CMake, and testing with CMocka.',
  keywords: ['efi', 'hello-world', 'uefi'],
  want_main: false,
  want_codeql: false,
  want_tests: false,
  copilot+: {
    intro: 'This is a demonstration of UEFI code, building with CMake, and testing with CMocka.',
  },
  package_json+: {
    scripts+: {
      'check-formatting': "cmake-format --check CMakeLists.txt && clang-format -n *.c && prettier -c . && markdownlint-cli2 '**/*.md' '#node_modules'",
      format: 'cmake-format -i CMakeLists.txt && clang-format -i *.c && yarn prettier -w .',
    },
  },
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
