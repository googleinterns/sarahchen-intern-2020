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
    2.  Check to make sure that all input files are present at "demo/proto_texts"
    3.  Since bazel does not allow changes in working directory (to maintain
        the invariant that the workspace is consistent) we need to specify where
        demo_main would print the changed files.
    4.  Type bazel run //demo:demo_main -- --output_dir="<Absolute path of output>" 
        to run the demo.