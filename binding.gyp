{
  "targets": [
   {
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
      "target_name": "addon",
      "sources": [ "src/index.cc", 'src/VssController.h', 'src/VssController.cc' ]
    }
  ]
}
