## Server 
Follow the following steps to run the server 
    1.  install bazel following the steps of 
        https://docs.bazel.build/versions/3.2.0/install.html
    2.  Run "bazel build //server" from the hologram_dashboard folder
    3.  Run "bazel run //server" to start listening on port 8000

## Extension
Follow the following steps to upload the extension onto chrome
    1.  Navigate to chrome://extensions from a chrome browser
    2.  Enable developer's mode
    3.  Click "load unpacked" to load "hologram_dashboard/front-end/"