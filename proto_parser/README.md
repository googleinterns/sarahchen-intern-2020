# Proto++ Demo

## Preparation
    1.  Clone the repository 
        git clone https://github.com/googleinterns/sarahchen-intern-2020.git
    2.  Clone the "proto_parser" folder into your project

## Run the Demo
Follow the following steps to compile the parser for your project

    1.  Install bazel by following the steps of 
        https://docs.bazel.build/versions/3.2.0/install.html
        for your specific system eg. ubuntu
    2.  Since bazel does not allow changes in working directory (to maintain
        the invariant that the workspace is consistent) we need to specify where
        demo_main would print the changed files.
    3.  Type bazel run //demo:demo_main -- <Absolute path of output> 
        to run the demo.