{
  'targets': [
    {
      'target_name': 'geoip',
      'sources': [
          'src/city.cc'
      ],
      'link_settings': {
      'libraries': [
          '-lGeoIP'
      ],
      },
      'include_dirs': [
          '<(node_root_dir)/src'
      ],
      'conditions': [
      ]
    },
  ]
}
