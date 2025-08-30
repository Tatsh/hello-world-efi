local utils = import 'utils.libjsonnet';

(import 'defaults.libjsonnet') + {
  local top = self,
  // General settings
  project_type: 'c',

  // Shared
  github_username: 'Tatsh',
  security_policy_supported_versions: { '0.0.x': ':white_check_mark:' },
  authors: [
    {
      'family-names': 'Udvare',
      'given-names': 'Andrew',
      email: 'audvare@gmail.com',
      name: '%s %s' % [self['given-names'], self['family-names']],
    },
  ],
  project_name: 'hello-world-efi',
  version: '0.0.2',
  description: 'Very simple demonstration of UEFI facilities, building with CMake, and testing with CMocka.',
  keywords: ['efi', 'hello-world', 'uefi'],
  want_main: false,
  copilot: {
    intro: 'This is a demonstration of UEFI code, building with CMake, and testing with CMocka.',
  },
  social+: {
    mastodon+: { id: '109370961877277568' },
  },

  // GitHub
  github+: {
    funding+: {
      ko_fi: 'tatsh2',
      liberapay: 'tatsh2',
      patreon: 'tatsh2',
    },
  },
}
